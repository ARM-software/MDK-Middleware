/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    netio.c
 * Purpose: Server for socket benchmark using BSD sockets
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "cmsis_os2.h"
#include "RTE_Components.h"
#include "rl_net.h"

// Size of the test buffer
#define BUF_SIZE        4096

// netio ports
#define NETIO_PORT      18767
#define NETIO_AUXPORT   18768

// netio commands
#define CMD_QUIT        0           // Quit the test
#define CMD_C2S         1           // Client to server test
#define CMD_S2C         2           // Server to client test
#define CMD_RES         3           // Send Results to client

// Thread exit macro
#define __THREAD_EXIT(err)          thread_exit (__LINE__, err)

// netio command structure
typedef struct {
  uint32_t cmd;
  uint32_t data;
} CONTROL;

// netio control structure
struct {
  int32_t      command;             // netio command received
  uint32_t     pkt_size;            // Packet size for the test
  osTimerId_t  timer;               // Timer id
  SOCKADDR_IN *client;              // Client address
  uint8_t     *buf;                 // Test buffer
  uint32_t     buf_size;            // Size of test buffer
  int32_t      udp_sock;            // Socket for UDP benchmarking
  uint32_t     pkt_count;           // Number of packets transmitted/received
  uint32_t     rx_bytes;            // Number of bytes received
  uint32_t     tx_bytes;            // Number of bytes transmitted
  bool         timeover;            // Test timeout flag (6 seconds elapsed)
  int32_t      err_line;            // Error line number
  int32_t      err_bsd;             // BSD error code
} io;

// Functions
static void cb_timer (void *arg);
static uint32_t rand32 (void);
static void init_buffer (uint8_t *buf, int32_t size);
static int32_t recv_data (int32_t sock, void *buf, int32_t size);
static int32_t send_data (int32_t sock, const void *buf, int32_t size);
static void thread_exit (int32_t line, int32_t bsd_error);
static void UDP_Receiver (void *argument);
static void UDP_Server (void *argument);
static void TCP_Server (void *argument);
static void app_main_thread (void *argument);

// Timer callback function
static void cb_timer (void *arg) {
  io.timeover = true;
}

// LCG pseudo random generator
static uint32_t rand32 (void) {
  static uint32_t rnd_state;

  rnd_state *= 1103515245;
  rnd_state += 12345;
  return (rnd_state);
}

// Fill buffer with random data
static void init_buffer (uint8_t *buf, int32_t size) {
  uint32_t i;
  uint8_t ch;

  buf[0] = 0;
  for (i = 1; i < size; i++) {
    do ch = (uint8_t)rand32 ();
    while (ch == 0);
    buf[i] = ch;
  }
}

// Receive data of requested size
static int32_t recv_data (int32_t sock, void *buf, int32_t size) {
  int32_t rc;

  rc = recv (sock, buf, size, 0);
  if (rc < 0)     return (rc);
  if (rc != size) return (BSD_ERROR);
  return (0);
}

// Send data of requested size
static int32_t send_data (int32_t sock, const void *buf, int32_t size) {
  int32_t rc;

  rc = send (sock, buf, size, 0);
  if (rc < 0)     return (rc);
  if (rc != size) return (BSD_ERROR);
  return (0);
}

// Exit server thread and record error code
static void thread_exit (int32_t line, int32_t bsd_error) {
  io.err_line = line;
  io.err_bsd  = bsd_error;
  printf ("Error: line %d, code %d\n", line, bsd_error);
  osThreadExit ();
}

// Wait function for the driver
static void wait (void) {
  static volatile uint32_t cnt;

  for (cnt = 0; cnt < 500; cnt++);
}

// UDP receiver thread
static void UDP_Receiver (void *argument) {
  int32_t rc;

  for (;;) {
    rc = recv (io.udp_sock, (char *)io.buf, io.pkt_size, 0);
    if (rc < 0) {
      break;
    }
    io.rx_bytes += rc;
    io.pkt_count++;
  }
  osThreadExit ();
}

// UDP server control thread
static void UDP_Server (void *argument) {
  SOCKADDR_IN sa;
  CONTROL ctl;
  int32_t sock,csock,rc;
  int32_t sa_len,nb;
  struct timeval tv;
  fd_set fds;

  // Server socket for UDP benchmark
  sock = socket (PF_INET, SOCK_STREAM, 0);
  if (sock < 0) __THREAD_EXIT (sock);

  sa.sin_family      = AF_INET;
  sa.sin_port        = htons (NETIO_AUXPORT);
  sa.sin_addr.s_addr = INADDR_ANY;
  rc = bind (sock, (SOCKADDR *)&sa, sizeof(sa));
  if (rc < 0) __THREAD_EXIT (rc);

  rc = listen (sock, 1);
  if (rc < 0) __THREAD_EXIT (rc);

  printf (" UDP server listening\n");

  for (;;) {
    FD_ZERO(&fds);
    FD_SET(sock, &fds);
    tv.tv_sec  = 120;
    tv.tv_usec = 0;

    // Wait for the client to connect
    rc = select (sock+1, &fds, 0, 0, &tv);
    if (rc < 0) __THREAD_EXIT (rc);

    if ((rc == 0) || (FD_ISSET(sock, &fds) == 0)) {
      continue;
    }

    sa_len = sizeof (*io.client);
    csock = accept (sock, (SOCKADDR *)io.client, &sa_len);
    if (csock < 0) {
      continue;
    }

    // Client is now connected
    printf ("\nUDP Client connected\n");

    for (;;) {
      rc = recv_data (csock, &ctl, sizeof (ctl));
      if (rc < 0) __THREAD_EXIT (rc);

      // Convert a command to host endian format
      io.command  = ntohl (ctl.cmd);

      if (io.command == CMD_C2S) {
        // Client to Server (starts the test)
        io.pkt_size  = ntohl (ctl.data);
        io.pkt_count = 0;
        io.rx_bytes  = 0;

        printf (" Address %s, packet size %d bytes\n",
                inet_ntoa (io.client->sin_addr), io.pkt_size);

        if (io.pkt_size > io.buf_size) __THREAD_EXIT (BSD_EMSGSIZE);

        // Receiver socket for UDP benchmark
        rc = socket (PF_INET, SOCK_DGRAM, 0);
        if (rc < 0) __THREAD_EXIT (rc);
        io.udp_sock = rc;

        sa.sin_family      = AF_INET;
        sa.sin_port        = htons (NETIO_AUXPORT);
        sa.sin_addr.s_addr = INADDR_ANY;
        rc = bind (io.udp_sock, (SOCKADDR *)&sa, sizeof (sa));
        if (rc < 0) __THREAD_EXIT (rc);

        osThreadNew(UDP_Receiver, NULL, NULL);

        printf (" Receiving ...\n");

        // Echo command back to the client
        rc = send_data (csock, (char *)&ctl, sizeof (ctl));
        if (rc < 0) __THREAD_EXIT (rc);
      }
      else if (io.command == CMD_RES) {
        // Print benchmark result

        printf (" Done %d packets\n", io.pkt_count);

        ctl.cmd  = htonl (CMD_RES);
        ctl.data = htonl (io.pkt_count);
        rc = send_data (csock, (char *)&ctl, sizeof (ctl));
        if (rc < 0) __THREAD_EXIT (rc);
      }
      else if (io.command == CMD_S2C) {
        // Server to Client
        io.pkt_size  = ntohl (ctl.data);
        io.tx_bytes  = 0;
        io.pkt_count = 0;
        if (io.pkt_size > io.buf_size) __THREAD_EXIT (BSD_EMSGSIZE);

        printf (" Sending ...\n");

        io.client->sin_port = htons (NETIO_AUXPORT);
        io.timeover  = false;
        init_buffer (io.buf, io.buf_size);
        osTimerStart (io.timer, 6000);

        io.buf[0] = 0;
        while (!io.timeover) {
          for (nb = 0; nb < io.pkt_size; ) {
            rc = sendto (io.udp_sock, (char *)&io.buf[nb], io.pkt_size - nb, 0,
                         (SOCKADDR *)io.client, sizeof (*io.client));
            if (rc < 0) {
              while (!io.timeover) osDelay (2);
              goto exit;
            }
            nb += rc;
            wait ();
          }
          io.tx_bytes += io.pkt_size;
          io.pkt_count++;
        }
exit:   closesocket (io.udp_sock);

        printf (" Done %d packets\n", io.pkt_count);

        io.udp_sock = -1;
        ctl.cmd  = htonl (CMD_RES);
        ctl.data = htonl (io.pkt_count);
        rc = send_data (csock, (char *)&ctl, sizeof (ctl));
        if (rc < 0) __THREAD_EXIT (rc);
      }
      else {
        // Quit
        break;
      }
    }
    closesocket (csock);
  }
  closesocket (sock);
}
 
// TCP control thread
static void TCP_Server (void *argument) {
  SOCKADDR_IN sa;
  CONTROL ctl;
  int32_t sock,csock,rc;
  int32_t sa_len,nb;
  struct timeval tv;
  fd_set fds;

  // Server socket for TCP benchmark
  sock = socket (PF_INET, SOCK_STREAM, 0);
  if (sock < 0) __THREAD_EXIT (sock);

  sa.sin_family      = AF_INET;
  sa.sin_port        = htons (NETIO_PORT);
  sa.sin_addr.s_addr = INADDR_ANY;
  rc = bind (sock, (SOCKADDR *)&sa, sizeof(sa));
  if (rc < 0) __THREAD_EXIT (rc);

  rc = listen (sock, 1);
  if (rc < 0) __THREAD_EXIT (rc);

  printf (" TCP Server listening\n");

  for (;;) {
    FD_ZERO(&fds);
    FD_SET(sock, &fds);
    tv.tv_sec  = 120;
    tv.tv_usec = 0;

    // Wait for the client to connect
    rc = select (sock+1, &fds, 0, 0, &tv);
    if (rc < 0) __THREAD_EXIT (rc);

    if ((rc == 0) || (FD_ISSET(sock, &fds) == 0)) {
      continue;
    }

    sa_len = sizeof (*io.client);
    csock = accept (sock, (SOCKADDR *)io.client, &sa_len);
    if (csock < 0) {
      continue;
    }

    // Client is now connected
    printf ("\nTCP Client connected\n");

    for (;;) {
      rc = recv_data (csock, &ctl, sizeof (ctl));
      if (rc < 0) __THREAD_EXIT (rc);

      // Convert a command to host endian format
      io.command = ntohl (ctl.cmd);

      if (io.command == CMD_C2S) {
        // Client to Server (starts the test)
        io.pkt_size = ntohl (ctl.data);
        io.pkt_count= 0;
        io.rx_bytes = 0;

        printf (" Address %s, packet size %d bytes\n",
                inet_ntoa (io.client->sin_addr), io.pkt_size);

        if (io.pkt_size > io.buf_size) __THREAD_EXIT (BSD_EMSGSIZE);

        printf (" Receiving ...\n");

        do {
          for (nb = 0; nb < io.pkt_size; ) {
            rc = recv (csock, (char *)&io.buf[nb], io.pkt_size - nb, 0);
            if (rc < 0) {
              break;
            }
            nb += rc;
          }
          io.rx_bytes += io.pkt_size;
          io.pkt_count++;
        } while (io.buf[0] == 0 && rc > 0);

        printf (" Done %d packets\n", io.pkt_count);
      }
      else if (io.command == CMD_S2C) {
        // Server to Client
        io.pkt_size = ntohl (ctl.data);
        io.pkt_count= 0;
        io.tx_bytes = 0;
        if (io.pkt_size > io.buf_size) __THREAD_EXIT (BSD_EMSGSIZE);

        printf (" Sending ...\n");

        init_buffer (io.buf, io.buf_size);
        io.timeover  = false;
        osTimerStart (io.timer, 6000);

        io.buf[0] = 0;
        while (!io.timeover) {
          for (nb = 0; nb < io.pkt_size; ) {
            rc = send (csock, (char *)&io.buf[nb], io.pkt_size - nb, 0);
            if (rc < 0) {
              break;
            }
            nb += rc;
          }
          io.tx_bytes += io.pkt_size;
          io.pkt_count++;
        }
        io.buf[0] = 1;
        rc = send_data (csock, io.buf, io.pkt_size);
        if (rc < 0) __THREAD_EXIT (rc);

        printf (" Done %d packets\n", io.pkt_count);
      }
      else {
        // Quit
        break;
      }
    }
    closesocket (csock);
  }
  closesocket (sock);
}

// Application main thread
static void app_main_thread (void *argument) {
  static uint8_t io_buf[BUF_SIZE];
  static SOCKADDR_IN sa_client;

  printf ("NETIO Benchmark test\n");

  netInitialize ();

  io.buf      = &io_buf[0];
  io.buf_size = sizeof (io_buf);
  io.client   = &sa_client;
  io.timer    = osTimerNew (cb_timer, osTimerOnce, NULL, NULL);
  io.udp_sock = -1;

  osDelay (500);
  osThreadNew (TCP_Server, NULL, NULL);
  osThreadNew (UDP_Server, NULL, NULL);

  osThreadExit ();
}

int app_main (void) {
  osKernelInitialize ();
  osThreadNew (app_main_thread, NULL, NULL);
  osKernelStart ();
  return (0);
}

# Netio Performance Test

This MDK-Middleware test project implements **netio server**, a simple TCP/IP
network benchmark. The example uses BSD sockets for the implementation and runs both
TCP and UDP protocol servers.

Use the **netio client** running on a PC to measure the transmit and receive rates.
The client is available for download at
[github repository](https://github.com/kai-uwe-rommel/netio/tree/master).

### TCP test

```shell
netio -t -b1k my_host  
```

### UDP test

```shell
netio -u -b1k my_host  
```

### Test results

- using ARM compiler 6

  | Packet size |   TCP-tx   |  TCP-rx   |  UDP-tx    |  UDP-rx   |
  | ----------- | ---------- | --------- | ---------- | --------- |
  |  256 bytes  |  9.8 MB/s  |  4.7 MB/s | 3.0 MB/s   |  4.4 MB/s |
  | 1024 bytes  |  9.6 MB/s  |  3.9 MB/s |  11.2 MB/s | 10.7 MB/s |
  | 1440 bytes  | 10.1 MB/s  | 10.9 MB/s |  11.4 MB/s |  9.8 MB/s |
  | 2000 bytes  |  8.3 MB/s  |  8.5 MB/s |  10.4 MB/s | 10.4 MB/s |
  | 4000 bytes  |  8.0 MB/s  | 10.1 MB/s |    ---     |   ---     |

- using GCC compiler

  | Packet size |   TCP-tx   |  TCP-rx   |  UDP-tx    |  UDP-rx   |
  | ----------- | ---------- | --------- | ---------- | --------- |
  |  256 bytes  |  3.0 MB/s  | 1.9 MB/s  |  0.3 MB/s  | 2.4 MB/s  |
  | 1024 bytes  |  3.2 MB/s  | 4.1 MB/s  |  1.8 MB/s  | 4.5 MB/s  |
  | 1440 bytes  |  3.2 MB/s  | 4.5 MB/s  |  2.8 MB/s  | 5.0 MB/s  |
  | 2000 bytes  |  3.2 MB/s  | 4.0 MB/s  |  0.08 MB/s | 3.1 MB/s  |
  | 4000 bytes  |  3.1 MB/s  | 4.5 MB/s  |    ---     |   ---     |

> *Note:*
>
> - The netio client floods the server with data. The netio server is usually not able
>   to process all data in real time, so it is normal for some packets to be dropped,
>   especially during the UDP receive test.
> - The standard clib function **memcpy** used in the GCC variant is not optimized
>   and uses byte copies. Therefore, the application spends most of the CPU time in this
>   function during the test. This is the main reason why the results are not as good
>   when compiled with the GCC compiler.

/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    SNMP_Agent_MIB.c
 * Purpose: SNMP Agent Management Information Base Module
 * Rev.:    V7.0.0
 *----------------------------------------------------------------------------*/

#include <stdio.h>

#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "cmsis_vio.h"                  // ::CMSIS Driver:VIO

#include "rl_net.h"                     // Keil::Network&MDK:CORE
 
// System up time
extern uint32_t netSNMP_sysUpTime;

// MIB read only integer constants
static const uint8_t sysServices = 79;

// Local variables
static uint8_t LedOut;
static uint8_t KeyIn;
static char lcd_text[2][20+1] = { "line 1", "line 2" };

// MIB event callback functions
static void write_leds (int32_t mode) {
  if (mode == NET_SNMP_MIB_WRITE) {
    vioSetSignal(0xFFU, LedOut);
  }
}

static void read_key (int32_t mode) {
  if (mode == NET_SNMP_MIB_READ) {
    KeyIn = (uint8_t)vioGetSignal(0xFFU);
  }
}

static void upd_display (int32_t mode) {
  if (mode == NET_SNMP_MIB_WRITE) {
    printf("LCD1: %s\n", lcd_text[0]);
    printf("LCD2: %s\n", lcd_text[1]);
  }
}

// MIB data table
static const NET_SNMP_MIB_INFO mib_table[] = {
// ---------- System MIB ----------- 
  // SysDescr Entry
  { NET_SNMP_MIB_OCTET_STR | NET_SNMP_MIB_ATR_RO,
    8, {NET_SNMP_MIB_OID0(1,3), 6, 1, 2, 1, 1, 1, 0},
    NET_SNMP_MIB_STR("Embedded System SNMP V1.0"),
    NULL },
  // SysObjectID Entry
  { NET_SNMP_MIB_OBJECT_ID | NET_SNMP_MIB_ATR_RO,
    8, {NET_SNMP_MIB_OID0(1,3), 6, 1, 2, 1, 1, 2, 0},
    NET_SNMP_MIB_STR("\x2b\x06\x01\x02\x01\x01\x02\x00"),
    NULL },
  // SysUpTime Entry
  { NET_SNMP_MIB_TIME_TICKS | NET_SNMP_MIB_ATR_RO,
    8, {NET_SNMP_MIB_OID0(1,3), 6, 1, 2, 1, 1, 3, 0},
    4, &netSNMP_sysUpTime,
    NULL },
  // SysContact Entry
  { NET_SNMP_MIB_OCTET_STR | NET_SNMP_MIB_ATR_RO,
    8, {NET_SNMP_MIB_OID0(1,3), 6, 1, 2, 1, 1, 4, 0},
    NET_SNMP_MIB_STR("test@keil.com"),
    NULL },
  // SysName Entry
  { NET_SNMP_MIB_OCTET_STR | NET_SNMP_MIB_ATR_RO,
    8, {NET_SNMP_MIB_OID0(1,3), 6, 1, 2, 1, 1, 5, 0},
    NET_SNMP_MIB_STR("Evaluation board"),
    NULL },
  // SysLocation Entry
  { NET_SNMP_MIB_OCTET_STR | NET_SNMP_MIB_ATR_RO,
    8, {NET_SNMP_MIB_OID0(1,3), 6, 1, 2, 1, 1, 6, 0},
    NET_SNMP_MIB_STR("Local"),
    NULL },
  // SysServices Entry
  { NET_SNMP_MIB_INTEGER | NET_SNMP_MIB_ATR_RO,
    8, {NET_SNMP_MIB_OID0(1,3), 6, 1, 2, 1, 1, 7, 0},
    NET_SNMP_MIB_INT(sysServices),
    NULL },
// ---------- Experimental MIB -----------
  // AddLedOut Entry
  { NET_SNMP_MIB_INTEGER,
    6, {NET_SNMP_MIB_OID0(1,3), 6, 1, 3, 1, 0},
    NET_SNMP_MIB_INT(LedOut),
    write_leds },
  // KeyIn Entry
  { NET_SNMP_MIB_INTEGER | NET_SNMP_MIB_ATR_RO,
    6, {NET_SNMP_MIB_OID0(1,3), 6, 1, 3, 2, 0},
    NET_SNMP_MIB_INT(KeyIn),
    read_key },
  // LCD line1 Entry
  { NET_SNMP_MIB_OCTET_STR,
    6, {NET_SNMP_MIB_OID0(1,3), 6, 1, 3, 3, 0},
    NET_SNMP_MIB_STR(lcd_text[0]),
    upd_display },
  // LCD line2 Entry
  { NET_SNMP_MIB_OCTET_STR,
    6, {NET_SNMP_MIB_OID0(1,3), 6, 1, 3, 4, 0},
    NET_SNMP_MIB_STR(lcd_text[1]),
    upd_display },
};

// Register MIB table in SNMP agent.
void register_mib_table (void) {
  netStatus res;
 
  res = netSNMP_SetMIB_Table(mib_table, sizeof(mib_table));
  switch (res) {
    case netOK:
      // Success, SNMP agent is active
      break;

    case netInvalidParameter:
      // Error, invalid MIB table or size
      break;

    default:
      break;
  }
}

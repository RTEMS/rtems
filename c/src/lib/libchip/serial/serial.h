/*
 *  This file contains the TTY driver table definition
 *
 *  This driver uses the termios pseudo driver.
 *
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *
 * THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 * AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 * You are hereby granted permission to use, copy, modify, and distribute
 * this file, provided that this notice, plus the above copyright notice
 * and disclaimer, appears in all copies. Radstone Technology will provide
 * no support for this code.
 *
 *  $Id$
 */

#ifndef __LIBCHIP_SERIAL_h
#define __LIBCHIP_SERIAL_h

#include <ringbuf.h>

typedef struct _console_fns {
  boolean (*deviceProbe)(int minor);
  int (*deviceFirstOpen)(int major, int minor, void *arg);
  int (*deviceLastClose)(int major, int minor, void *arg);
  int (*deviceRead)(int minor);
  int (*deviceWrite)(int minor, const char *buf, int len);
  void  (*deviceInitialize)(int minor);
  void  (*deviceWritePolled)(int minor, char cChar);
  int deviceOutputUsesInterrupts;
} console_fns;

typedef struct _console_flow {
  int (*deviceStopRemoteTx)(int minor);
  int (*deviceStartRemoteTx)(int minor);
} console_flow;

typedef struct _console_tbl {
  char    *sDeviceName;
  console_fns *pDeviceFns;
  boolean   (*deviceProbe)(int minor);
  console_flow  *pDeviceFlow;
  unsigned32  ulMargin;
  unsigned32  ulHysteresis;
  void    *pDeviceParams;
  unsigned32  ulCtrlPort1;
  unsigned32  ulCtrlPort2;
  unsigned32  ulDataPort;
        unsigned8       (*getRegister)(unsigned32 port, unsigned8 register);
        void            (*setRegister)(
                            unsigned32 port, unsigned8 reg, unsigned8 value);
        unsigned8       (*getData)(unsigned32 port);
        void            (*setData)(unsigned32 port, unsigned8 value);
        unsigned32      ulClock;
  unsigned int  ulIntVector;
} console_tbl;

typedef struct _console_data {
  void  *termios_data;
  volatile boolean  bActive;
  volatile Ring_buffer_t  TxBuffer;
  /*
   * This field may be used for any purpose required by the driver
   */
  void  *pDeviceContext;
} console_data;

extern console_tbl  Console_Port_Tbl[];
extern console_data Console_Port_Data[];
extern unsigned long  Console_Port_Count;

#endif
/* end of include file */

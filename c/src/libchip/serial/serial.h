/*
 *  This file contains the TTY driver table definition
 *
 *  This driver uses the termios pseudo driver.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __LIBCHIP_SERIAL_h
#define __LIBCHIP_SERIAL_h

#include <termios.h>

/*
 *  Types for get and set register routines
 */

typedef unsigned8 (*getRegister_f)(unsigned32 port, unsigned8 register);
typedef void      (*setRegister_f)(
                            unsigned32 port, unsigned8 reg, unsigned8 value);
typedef unsigned8 (*getData_f)(unsigned32 port);
typedef void      (*setData_f)(unsigned32 port, unsigned8 value);

typedef struct _console_fns {
  boolean (*deviceProbe)(int minor);
  int     (*deviceFirstOpen)(int major, int minor, void *arg);
  int     (*deviceLastClose)(int major, int minor, void *arg);
  int     (*deviceRead)(int minor);
  int     (*deviceWrite)(int minor, const char *buf, int len);
  void    (*deviceInitialize)(int minor);
  void    (*deviceWritePolled)(int minor, char cChar);
  int     (*deviceSetAttributes)(int minor, const struct termios *t);
  int       deviceOutputUsesInterrupts;
} console_fns;

typedef struct _console_flow {
  int (*deviceStopRemoteTx)(int minor);
  int (*deviceStartRemoteTx)(int minor);
} console_flow;

typedef enum {
  SERIAL_MC68681,              /* Motorola MC68681 or Exar 88681 */
  SERIAL_NS16550,              /* National Semiconductor NS16550 */
  SERIAL_Z85C30,               /* Zilog Z85C30 */
  SERIAL_CUSTOM                /* BSP specific driver */
} console_devs;

/*
 * Each field is interpreted thus:
 * 
 * sDeviceName  This is the name of the device. 
 *
 * deviceType   This indicates the chip type.  It is especially important when
 *              multiple devices share the same interrupt vector and must be
 *              distinguished.
 *
 * pDeviceFns   This is a pointer to the set of driver routines to use.
 *
 * pDeviceFlow  This is a pointer to the set of flow control routines to
 *              use. Serial device drivers will typically supply RTSCTS
 *              and DTRCTS handshake routines for DCE to DCE communication,
 *              however for DCE to DTE communication, no such routines
 *              should be necessary as RTS will be driven automatically
 *              when the transmitter is active.
 *
 * ulMargin     The high water mark in the input buffer is set to the buffer
 *              size less ulMargin. Once this level is reached, the driver's
 *              flow control routine used to stop the remote transmitter will
 *              be called. This figure should be greater than or equal to
 *              the number of stages of FIFO between the transmitter and
 *              receiver. 
 *
 *              NOTE: At the current time, this parameter is hard coded
 *                    in termios and this number is ignored.
 *
 * ulHysteresis After the high water mark specified by ulMargin has been
 *              reached, the driver's routine to re-start the remote
 *              transmitter will be called once the level in the input
 *              buffer has fallen by ulHysteresis bytes.
 *
 *              NOTE: At the current time, this parameter is hard coded
 *                    in termios and this number is ignored.
 *
 * pDeviceParams This contains either device specific data or a pointer to a
 *              device specific structure containing additional information
 *              not provided in this table. 
 *
 * ulCtrlPort1  This is the primary control port number for the device. This
 *              may be used to specify different instances of the same device
 *              type.
 *
 * ulCtrlPort2  This is the secondary control port number, of use when a given
 *              device has more than one available channel.
 *
 * ulDataPort   This is the port number for the data port of the device
 *
 * getRegister  This is the routine used to read register values.
 *
 * setRegister  This is the routine used to write register values.
 *
 * getData      This is the routine used to read the data register (RX).
 *
 * setData      This is the routine used to write the data register (TX).
 *
 * ulClock      This is the baud rate clock speed.
 *
 * ulIntVector  This encodes the interrupt vector of the device.
 */

typedef struct _console_tbl {
  char          *sDeviceName;
  console_devs   deviceType;
  console_fns   *pDeviceFns;
  boolean      (*deviceProbe)(int minor);
  console_flow  *pDeviceFlow;
  unsigned32     ulMargin;
  unsigned32     ulHysteresis;
  void          *pDeviceParams;
  unsigned32     ulCtrlPort1;
  unsigned32     ulCtrlPort2;
  unsigned32     ulDataPort;
  getRegister_f  getRegister;
  setRegister_f  setRegister;
  getData_f      getData;
  setData_f      setData;
  unsigned32     ulClock;
  unsigned int   ulIntVector;
} console_tbl;

typedef struct _console_data {
  void                   *termios_data;
  volatile boolean        bActive;
  /*
   * This field may be used for any purpose required by the driver
   */
  void                   *pDeviceContext;
} console_data;

extern console_tbl  Console_Port_Tbl[];
extern console_data Console_Port_Data[];
extern unsigned long  Console_Port_Count;

#endif
/* end of include file */

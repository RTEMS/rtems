/*  consolex.h
 *
 *  This file describes the Extended Console Device Driver
 *  This driver provides support for the standard C Library.
 *
 *  This file was created originally by 
 *  On-Line Applications Research Corporation (OAR)
 *  and modified by:
 *
 *  Katsutoshi Shibuya - BU-Denken Co.,Ltd. - Sapporo, JAPAN
 *
 *  featuring support of:
 *
 *     - Multi-SCC chip handling
 *     - Non-blocking I/O (O_NDELAY flag in libc)
 *     - Raw mode device (no CR/LF detection)
 *     - RTS/CTS flow control
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

#ifndef _CONSOLEX_DRIVER_h
#define _CONSOLEX_DRIVER_h

#include <rtems.h>
#include <termios.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CONSOLEX_DRIVER_TABLE_ENTRY \
  { consolex_initialize, consolex_open, consolex_close, \
    consolex_read, consolex_write, consolex_control }

rtems_device_driver consolex_initialize(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver consolex_open(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver consolex_close(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver consolex_read(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver consolex_write(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

rtems_device_driver consolex_control(
  rtems_device_major_number,
  rtems_device_minor_number,
  void *
);

/* Low level IO functions */
void	SCCInitialize();
rtems_boolean	SCCGetOne(int port, char *ch);
char		SCCGetOneBlocked(int port);
rtems_boolean	SCCSendOne(int port, char ch);
void		SCCSendOneBlocked(int port, char ch);
unsigned32	SCCSetAttributes(int port, struct termios *t);
unsigned32	SCCGetAttributes(int port, struct termios *t);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

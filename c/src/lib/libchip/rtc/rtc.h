/*
 *  This file contains the Real-Time Clock definitions.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *
 *  $Id$
 */

#ifndef __LIBCHIP_RTC_h
#define __LIBCHIP_RTC_h

/*
 *  Types for get and set register routines
 */

typedef unsigned32 (*getRegister_f)(unsigned32 port, unsigned8 register);
typedef void       (*setRegister_f)(
                            unsigned32 port, unsigned8 reg, unsigned32 value);

typedef struct _rtc_fns {
  boolean (*deviceProbe)(int minor);
  void    (*deviceInitialize)(int minor);
  int     (*deviceGetTime)(int minor, rtems_time_of_day *time);
  int     (*deviceSetTime)(int minor, rtems_time_of_day *time);
} rtc_fns;

typedef enum {
  RTC_M48T08,                  /* SGS-Thomsom M48T08 or M48T18 */
  RTC_ICM_7170,                /* Harris ICM-7170 */
  RTC_CUSTOM                   /* BSP specific driver */
} rtc_devs;

/*
 * Each field is interpreted thus:
 * 
 * sDeviceName  This is the name of the device. 
 *
 * deviceType   This indicates the chip type.
 *
 * pDeviceFns   This is a pointer to the set of driver routines to use.
 *
 * pDeviceParams This contains either device specific data or a pointer to a
 *              device specific information table.
 *
 * ulCtrlPort1  This is the primary control port number for the device.
 *
 * ulCtrlPort2  This is the secondary control port number.
 *
 * ulDataPort   This is the port number for the data port of the device
 *
 * getRegister  This is the routine used to read register values.
 *
 * setRegister  This is the routine used to write register values.
 */

typedef struct _rtc_tbl {
  char          *sDeviceName;
  rtc_devs       deviceType;
  rtc_fns       *pDeviceFns;
  boolean      (*deviceProbe)(int minor);
  void          *pDeviceParams;
  unsigned32     ulCtrlPort1;
  unsigned32     ulCtrlPort2;
  unsigned32     ulDataPort;
  getRegister_f  getRegister;
  setRegister_f  setRegister;
  unsigned int   ulIntVector;
} rtc_tbl;

extern rtc_tbl        RTC_Port_Tbl[];
extern unsigned long  RTC_Port_Count;


boolean rtc_probe( int minor );

#endif
/* end of include file */

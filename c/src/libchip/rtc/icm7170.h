/*
 *  This file contains the definitions for the following real-time clocks:
 *
 *    + Harris Semiconduction ICM7170
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

#ifndef __LIBCHIP_ICM7170_h
#define __LIBCHIP_ICM7170_h

/*
 *  Register indices
 */

#define ICM7170_CONTROL             0x11


#define ICM7170_COUNTER_HUNDREDTHS  0x00
#define ICM7170_HOUR                0x01
#define ICM7170_MINUTE              0x02
#define ICM7170_SECOND              0x03
#define ICM7170_MONTH               0x04
#define ICM7170_DATE                0x05
#define ICM7170_YEAR                0x06
#define ICM7170_DAY_OF_WEEK         0x07

/*
 *  Configuration information in the parameters field
 */

#define ICM7170_AT_32_KHZ  0x00
#define ICM7170_AT_1_MHZ   0x01
#define ICM7170_AT_2_MHZ   0x02
#define ICM7170_AT_4_MHZ   0x03

/*
 *  Driver function table
 */

extern rtc_fns icm7170_fns;

/*
 * Default register access routines
 */

unsigned32 icm7170_get_register(    /* registers are at 1 byte boundaries */
  unsigned32  ulCtrlPort,           /*   and accessed as bytes            */
  unsigned8   ucRegNum
);

void  icm7170_set_register(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum,
  unsigned32  ucData
);

unsigned32 icm7170_get_register_2(  /* registers are at 2 byte boundaries */
  unsigned32  ulCtrlPort,           /*   and accessed as bytes            */
  unsigned8   ucRegNum
);

void  icm7170_set_register_2(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum,
  unsigned32  ucData
);

unsigned32 icm7170_get_register_4(  /* registers are at 4 byte boundaries */
  unsigned32  ulCtrlPort,           /*   and accessed as bytes            */
  unsigned8   ucRegNum
);

void  icm7170_set_register_4(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum,
  unsigned32  ucData
);

unsigned32 icm7170_get_register_8(  /* registers are at 8 byte boundaries */
  unsigned32  ulCtrlPort,           /*   and accessed as bytes            */
  unsigned8   ucRegNum
);

void  icm7170_set_register_8(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum,
  unsigned32  ucData
);

#endif
/* end of include file */

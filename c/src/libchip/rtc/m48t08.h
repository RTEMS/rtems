/*
 *  This file contains the definitions for the following real-time clocks:
 *
 *    + Mostek M48T08
 *    + Mostek M48T18 
 *    + Dallas Semiconductor DS1643
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

#ifndef __LIBCHIP_M48T08_h
#define __LIBCHIP_M48T08_h

/*
 *  Register indices
 */

#define M48T08_CONTROL     0
#define M48T08_SECOND      1
#define M48T08_MINUTE      2
#define M48T08_HOUR        3
#define M48T08_DAY_OF_WEEK 4
#define M48T08_DATE        5
#define M48T08_MONTH       6
#define M48T08_YEAR        7
 
/*
 *  Driver function table
 */

extern rtc_fns m48t08_fns;

/*
 * Default register access routines
 */

unsigned32 m48t08_get_register(     /* registers are at 1 byte boundaries */
  unsigned32  ulCtrlPort,           /*   and accessed as bytes            */
  unsigned8   ucRegNum
);

void  m48t08_set_register(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum,
  unsigned32  ucData
);

unsigned32 m48t08_get_register_2(   /* registers are at 2 byte boundaries */
  unsigned32  ulCtrlPort,           /*   and accessed as bytes            */
  unsigned8   ucRegNum
);

void  m48t08_set_register_2(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum,
  unsigned32  ucData
);

unsigned32 m48t08_get_register_4(   /* registers are at 4 byte boundaries */
  unsigned32  ulCtrlPort,           /*   and accessed as bytes            */
  unsigned8   ucRegNum
);

void  m48t08_set_register_4(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum,
  unsigned32  ucData
);

unsigned32 m48t08_get_register_8(   /* registers are at 8 byte boundaries */
  unsigned32  ulCtrlPort,           /*   and accessed as bytes            */
  unsigned8   ucRegNum
);

void  m48t08_set_register_8(
  unsigned32  ulCtrlPort,
  unsigned8   ucRegNum,
  unsigned32  ucData
);

#endif
/* end of include file */

/*
 *  This file contains the definitions for the following real-time clocks:
 *
 *    + Mostek M48T08
 *    + Mostek M48T18 
 *    + Dallas Semiconductor DS1643
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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

#endif
/* end of include file */

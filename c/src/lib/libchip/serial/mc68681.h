/*
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

#ifndef _MC68681_H_
#define _MC68681_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  These are just used in the interface between this driver and
 *  the read/write register routines when accessing the first
 *  control port.
 */

#define MC68681_STATUS          1
#define MC68681_RX_BUFFER       2

#define MC68681_MODE            0
#define MC68681_CLOCK_SELECT    1
#define MC68681_COMMAND         2
#define MC68681_TX_BUFFER       3

/*
 *  Data Port bit map configuration
 */

#define MC68681_DATA_BAUD_RATE_SET_1      0
#define MC68681_DATA_BAUD_RATE_SET_2      1

/*
 * Driver function table
 */

extern console_fns mc68681_fns;
extern console_fns mc68681_fns_polled;

#ifdef __cplusplus
}
#endif

#endif /* _MC68681_H_ */

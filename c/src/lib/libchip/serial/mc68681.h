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
 * Driver function table
 */
extern console_fns mc68681_fns;
extern console_fns mc68681_fns_polled;

/*
 * Flow control function tables
 */
extern console_flow mc68681_flow_RTSCTS;
extern console_flow mc68681_flow_DTRCTS;

#ifdef __cplusplus
}
#endif

#endif /* _MC68681_H_ */

/*  stackchk.h
 *
 *  This include file contains information necessary to utilize
 *  and install the stack checker mechanism.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __STACK_CHECK_h
#define __STACK_CHECK_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Stack_check_Initialize
 */

void Stack_check_Initialize( void );

/*
 *  Stack_check_Dump_usage
 */

void Stack_check_Dump_usage( void );

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

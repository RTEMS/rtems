/*  cpuuse.h
 *
 *  This include file contains information necessary to utilize
 *  and install the cpu usage reporting mechanism.
 *
 *  COPYRIGHT (c) 1989-1998. 1996.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __CPU_USE_h
#define __CPU_USE_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  CPU_usage_Dump
 */

void CPU_usage_Dump( void );

/*
 *  CPU_usage_Reset
 */

void CPU_usage_Reset( void );

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

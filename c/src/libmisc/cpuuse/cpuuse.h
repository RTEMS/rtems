/*  cpuuse.h
 *
 *  This include file contains information necessary to utilize
 *  and install the cpu usage reporting mechanism.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994, 1996.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  stackchk.h,v 1.3 1995/12/19 20:13:52 joel Exp
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

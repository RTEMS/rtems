/**
 *  @file  rtems/score/copyrt.h
 *
 *  This include file contains the copyright notice for RTEMS
 *  which is included in every binary copy of the executive.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_COPYRT_H
#define _RTEMS_SCORE_COPYRT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  This is the copyright string for RTEMS.
 */
#ifdef SCORE_INIT
const char _Copyright_Notice[] =
"COPYRIGHT (c) 1989-2008.\n\
On-Line Applications Research Corporation (OAR).\n";
#else
extern const char _Copyright_Notice[];
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

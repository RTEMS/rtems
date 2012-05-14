/**
 * @file rtems/score/h8300.h
 */

/*
 *  This file contains information pertaining to the Hitachi H8/300
 *  processor family.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_H8300_H
#define _RTEMS_SCORE_H8300_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the "h8300"
 *  family when executing in protected mode.  It does
 *  this by setting variables to indicate which implementation
 *  dependent features are present in a particular member
 *  of the family.
 */

#define CPU_NAME  "Hitachi H8300"
#define CPU_MODEL_NAME  "h8300"
#define H8300_HAS_FPU     0

#ifdef __cplusplus
}
#endif

#endif

/**
 * @file
 */

/*
 *  This file contains information pertaining to the Moxie processor.
 *
 *  COPYRIGHT (c) 2011, 2013
 *  Anthony Green
 *
 *  Based on code with the following copyright...
 *  COPYRIGHT (c) 1989-1999, 2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_MOXIE_H
#define _RTEMS_SCORE_MOXIE_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the "moxie"
 *  family when executing in protected mode.  It does
 *  this by setting variables to indicate which implementation
 *  dependent features are present in a particular member
 *  of the family.
 */

#define CPU_NAME  "Moxie"
#define CPU_MODEL_NAME  "MoxieLite"

#ifdef __cplusplus
}
#endif

#endif

/*  
 *  $Id$
 *
 *
 *  COPYRIGHT (c) 2000 Canon Research Centre France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  Copyright (c) 2002 Advent Networks, Inc.
 *       Jay Monkman <jmonkman@adventnetworks.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 */

#ifndef _INCLUDE_ARM_h
#define _INCLUDE_ARM_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the "arm"
 *  family when executing in protected mode.  It does
 *  this by setting variables to indicate which implementation
 *  dependent features are present in a particular member
 *  of the family.
 */
#if defined(__arm9__)
#  define CPU_MODEL_NAME  "arm9"
#  define ARM_HAS_FPU     0
#elif defined(__arm9tdmi__)
#  define CPU_MODEL_NAME  "arm9tdmi"
#  define ARM_HAS_FPU     0
#elif defined(__arm7__)
#  define CPU_MODEL_NAME  "arm7"
#  define ARM_HAS_FPU     0
#elif defined(__arm7tdmi__)
#  define CPU_MODEL_NAME  "arm7tdmi"
#  define ARM_HAS_FPU     0
#elif defined(__arm__)
#  define CPU_MODEL_NAME  "unknown ARM"
#  define ARM_HAS_FPU     0
#else
#  error "Unsupported CPU Model"
#endif

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "ARM"

#ifdef __cplusplus
}
#endif

#endif /* ! _INCLUDE_ARM_h */
/* end of include file */

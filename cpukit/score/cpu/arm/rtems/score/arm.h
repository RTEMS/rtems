/*  
 *  $Id$
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
#if defined(__ARM_ARCH_4__)
#  define CPU_MODEL_NAME  "ARMv4"
#  define ARM_HAS_CLZ     0

#elif defined(__ARM_ARCH_4T__)
#  define CPU_MODEL_NAME  "ARMv4T"
#  define ARM_HAS_CLZ     0

#elif defined(__ARM_ARCH_5__)
#  define CPU_MODEL_NAME  "ARMv5"
#  define ARM_HAS_CLZ     1

#elif defined(__ARM_ARCH_5T__)
#  define CPU_MODEL_NAME  "ARMv5T"
#  define ARM_HAS_CLZ     1

#elif defined(__ARM_ARCH_5E__)
#  define CPU_MODEL_NAME  "ARMv5E"
#  define ARM_HAS_CLZ     1

#elif defined(__ARM_ARCH_5TE__)
#  define CPU_MODEL_NAME  "ARMv5TE"
#  define ARM_HAS_CLZ     1

#else
#  error "Unsupported CPU Model"

#endif

/* All ARM CPUs are assumed to not have floating point units */
#if defined(__SOFTFP__)
#define ARM_HAS_FPU     0
#else
#define ARM_HAS_FPU	1
# error "FPU-support not yet implemented for the arm"
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

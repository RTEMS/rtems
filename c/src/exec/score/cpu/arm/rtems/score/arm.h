/*  no_cpu.h
 *
 *  This file is an example (i.e. "no CPU") of the file which is
 *  created for each CPU family port of RTEMS.
 *
 *
 *  COPYRIGHT (c) 2000 Canon Research Centre France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
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
 
#if defined(__arm__)
 
#define CPU_MODEL_NAME  "arm"
#define ARM_HAS_FPU     0
 
#else
 
#error "Unsupported CPU Model"
 
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

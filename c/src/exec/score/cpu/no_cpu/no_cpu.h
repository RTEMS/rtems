/*  no_cpu.h
 *
 *  This file is an example (i.e. "no CPU") of the file which is
 *  created for each CPU family port of RTEMS.
 *
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 *
 */

#ifndef _INCLUDE_NO_CPU_h
#define _INCLUDE_NO_CPU_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the "no cpu"
 *  family when executing in protected mode.  It does
 *  this by setting variables to indicate which implementation
 *  dependent features are present in a particular member
 *  of the family.
 */
 
#if defined(no_cpu)
 
#define CPU_MODEL_NAME  "no_cpu"
#define NOCPU_HAS_FPU     1
 
#else
 
#error "Unsupported CPU Model"
 
#endif

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "NO CPU"

#ifdef __cplusplus
}
#endif

#endif /* ! _INCLUDE_NO_CPU_h */
/* end of include file */

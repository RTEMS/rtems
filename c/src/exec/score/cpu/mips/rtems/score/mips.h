/*  mips64orion.h
 *
 *  Author:     Craig Lebakken <craigl@transition.com>
 *
 *  COPYRIGHT (c) 1996 by Transition Networks Inc.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of Transition Networks not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      Transition Networks makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/exec/score/cpu/no_cpu/no_cpu.h:
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */
/* @(#)mips64orion.h       08/29/96     1.3 */

#ifndef _INCLUDE_MIPS_h
#define _INCLUDE_MIPS_h

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
 
#if defined(__mips_soft_float)
#define MIPS_HAS_FPU 0
#else
#define MIPS_HAS_FPU 1
#endif 

#if (__mips == 1)
#define CPU_MODEL_NAME  "ISA Level 1 or 2"
#elif (__mips == 3)
#if defined(__mips64)
#define CPU_MODEL_NAME  "ISA Level 4"
#else
#define CPU_MODEL_NAME  "ISA Level 3"
#endif
#else
#error "Unknown MIPS ISA level"
#endif

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "MIPS"

#ifdef __cplusplus
}
#endif

#endif /* ! _INCLUDE_MIPS_h */
/* end of include file */

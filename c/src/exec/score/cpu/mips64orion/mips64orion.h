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
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */
/* @(#)mips64orion.h       08/29/96     1.3 */

#ifndef _INCLUDE_MIPS64ORION_h
#define _INCLUDE_MIPS64ORION_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  The following define the CPU Family and Model within the family
 *
 *  NOTE: The string "REPLACE_THIS_WITH_THE_CPU_MODEL" is replaced
 *        with the name of the appropriate macro for this target CPU.
 */
 
#ifdef mips64orion
#undef mips64orion
#endif
#define mips64orion

#ifdef REPLACE_THIS_WITH_THE_CPU_MODEL
#undef REPLACE_THIS_WITH_THE_CPU_MODEL
#endif
#define REPLACE_THIS_WITH_THE_CPU_MODEL
 
#ifdef REPLACE_THIS_WITH_THE_BSP
#undef REPLACE_THIS_WITH_THE_BSP
#endif
#define REPLACE_THIS_WITH_THE_BSP

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the "no cpu"
 *  family when executing in protected mode.  It does
 *  this by setting variables to indicate which implementation
 *  dependent features are present in a particular member
 *  of the family.
 */
 
#if defined(R4650)
 
#define CPU_MODEL_NAME  "R4650"
#define MIPS64ORION_HAS_FPU     1
 
#elif defined(R4600)
 
#define CPU_MODEL_NAME  "R4600"
#define MIPS64ORION_HAS_FPU     1

#else
 
#error "Unsupported CPU Model"
 
#endif

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "MIPS R46xxx"

#ifdef __cplusplus
}
#endif

#endif /* ! _INCLUDE_MIPS64ORION_h */
/* end of include file */

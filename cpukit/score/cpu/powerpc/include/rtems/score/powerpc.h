/**
 * @file
 * 
 * @brief IBM/Motorola PowerPC Definitions
 * 
 * This file contains definitions for the IBM/Motorola PowerPC
 * family members.
 */

/*
 *  Author:	Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  MPC860 support code was added by Jay Monkman <jmonkman@frasca.com>
 *  MPC8260 support added by Andy Dachs <a.dachs@sstl.co.uk>
 *  Surrey Satellite Technology Limited
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/exec/cpu/no_cpu/no_cpu.h:
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *
 * Note:
 *      This file is included by both C and assembler code ( -DASM )
 */


#ifndef _RTEMS_SCORE_POWERPC_H
#define _RTEMS_SCORE_POWERPC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/basedefs.h>

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "PowerPC"

/*
 *  This file contains the information required to build
 *  RTEMS for the PowerPC family.
 */

/* Generic ppc */

#ifdef _SOFT_FLOAT
#define CPU_MODEL_NAME "Generic (no FPU)"
#elif defined(__NO_FPRS__) || defined(__SPE__)
#define CPU_MODEL_NAME "Generic (E500/float-gprs/SPE)"
#else
#define CPU_MODEL_NAME "Generic (classic FPU)"
#endif

#ifdef __PPC_CPU_E6500__
#define PPC_DEFAULT_CACHE_LINE_POWER 6
#else
#define PPC_DEFAULT_CACHE_LINE_POWER 5
#endif

#define PPC_DEFAULT_CACHE_LINE_SIZE (1 << PPC_DEFAULT_CACHE_LINE_POWER)

#define PPC_STRUCTURE_ALIGNMENT PPC_DEFAULT_CACHE_LINE_SIZE

/*
 *  Application binary interfaces.
 *
 *  PPC_ABI MUST be defined as one of these.
 *  Only big endian is currently supported.
 */

/*
 *  SVR4 ABI
 */
#define PPC_ABI_SVR4		2
/*
 *  Embedded ABI
 */
#define PPC_ABI_EABI		3

/*
 *  Default to the EABI used by current GNU tools
 */

#ifndef PPC_ABI
#define PPC_ABI PPC_ABI_EABI
#endif

/*
 *  Use worst case stack alignment.  For the EABI an 8-byte alignment would be
 *  sufficient.
 */

#define PPC_STACK_ALIGN_POWER 4
#define PPC_STACK_ALIGNMENT (1 << PPC_STACK_ALIGN_POWER)

/*
 *  Assume PPC_HAS_FPU to be a synonym for _SOFT_FLOAT.
 */

#if defined(_SOFT_FLOAT) \
  || defined(__NO_FPRS__) /* e500 has unified integer/FP registers */ \
  || defined(__PPC_CPU_E6500__)
#define PPC_HAS_FPU 0
#else
#define PPC_HAS_FPU 1
#endif

#if defined(__PPC_CPU_E6500__) && defined(__ALTIVEC__)
#define PPC_MULTILIB_ALTIVEC
#endif

#if defined(__PPC_CPU_E6500__) && !defined(_SOFT_FLOAT)
#define PPC_MULTILIB_FPU
#endif

#ifdef PPC_MULTILIB_ALTIVEC
#define PPC_ALIGNMENT 16
#else
#define PPC_ALIGNMENT 8
#endif

#ifdef __powerpc64__
#define PPC_STACK_RED_ZONE_SIZE 512
#else
#define PPC_STACK_RED_ZONE_SIZE 0
#endif

/*
 *  Unless specified above, If the model has FP support, it is assumed to
 *  support doubles (8-byte floating point numbers).
 *
 *  If the model does NOT have FP support, then the model does
 *  NOT have double length FP registers.
 */

#if (PPC_HAS_FPU)
#define PPC_HAS_DOUBLE 1
#else
#define PPC_HAS_DOUBLE 0
#endif

/*
 *  Assemblers.
 *  PPC_ASM MUST be defined as one of these.
 *
 *  PPC_ASM_ELF:   ELF assembler. Currently used for all ABIs.
 *
 *  NOTE: Only PPC_ABI_ELF is currently fully supported.
 *
 *  Also NOTE: cpukit doesn't need this but asm.h which is defined
 *             in cpukit for consistency with other ports does.
 */

#define PPC_ASM_ELF   0

/*
 *  Default to the assembler format used by the current GNU tools.
 */
#define PPC_ASM PPC_ASM_ELF


#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_POWERPC_H */

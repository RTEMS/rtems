/*  ppc.h
 *
 *  This file contains definitions for the IBM/Motorola PowerPC
 *  family members.
 *
 *  Author:	Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
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
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *
 * Note:
 *      This file is included by both C and assembler code ( -DASM )
 *
 *  $Id$
 */

#ifndef _INCLUDE_PPC_h
#define _INCLUDE_PPC_h

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
 
#if defined(ppc403)
 
#define CPU_MODEL_NAME  "PowerPC 403"

#define PPC_ALIGNMENT		4
#define PPC_CACHE_ALIGNMENT	16
#define PPC_CACHE_ALIGN_POWER	4
#define PPC_INTERRUPT_MAX	16
#define PPC_HAS_FPU     	0
#define PPC_HAS_DOUBLE		0
#define PPC_HAS_RFCI		1
#define PPC_MSR_DISABLE_MASK	0x00029200
#define PPC_MSR_INITIAL		0x00000000
#define PPC_INIT_FPSCR		0x00000000
#define PPC_USE_MULTIPLE	1
#define PPC_I_CACHE		2048
#define PPC_D_CACHE		1024

#define PPC_MSR_0		0x00029200
#define PPC_MSR_1		0x00021200
#define PPC_MSR_2		0x00021000
#define PPC_MSR_3		0x00000000

#elif defined(ppc601)
 
#define CPU_MODEL_NAME  "PowerPC 601"

#define PPC_ALIGNMENT		8
#define PPC_CACHE_ALIGNMENT	32
#define PPC_CACHE_ALIGN_POWER	5
#define PPC_INTERRUPT_MAX	16
#define PPC_HAS_FPU     	1
#define PPC_HAS_DOUBLE		1
#define PPC_HAS_RFCI		0
#define PPC_MSR_DISABLE_MASK	0x00009900
#define PPC_MSR_INITIAL		0x00002000
#define PPC_INIT_FPSCR		0x000000f8
#define PPC_USE_MULTIPLE	1
#define PPC_I_CACHE		0
#define PPC_D_CACHE		32768

#define PPC_MSR_0		0x00009900
#define PPC_MSR_1		0x00001000
#define PPC_MSR_2		0x00001000
#define PPC_MSR_3		0x00000000
 
#elif defined(ppc602)
 
#define CPU_MODEL_NAME  "PowerPC 602"

#define PPC_ALIGNMENT		4
#define PPC_CACHE_ALIGNMENT	32
#define PPC_CACHE_ALIGN_POWER	5
#define PPC_INTERRUPT_MAX	16
#define PPC_HAS_FPU     	1
#define PPC_HAS_DOUBLE		0
#define PPC_HAS_RFCI		0
#define PPC_MSR_DISABLE_MASK	
#define PPC_MSR_INITIAL		
#define PPC_INIT_FPSCR		
#define PPC_USE_MULTIPLE	0
#define PPC_I_CACHE		4096
#define PPC_D_CACHE		4096

#elif defined(ppc603)
 
#define CPU_MODEL_NAME  "PowerPC 603"

#define PPC_ALIGNMENT		8
#define PPC_CACHE_ALIGNMENT	32
#define PPC_CACHE_ALIGN_POWER	5
#define PPC_INTERRUPT_MAX	16
#define PPC_HAS_FPU     	1
#define PPC_HAS_DOUBLE		1
#define PPC_HAS_RFCI		0
#define PPC_MSR_DISABLE_MASK	0x00009900
#define PPC_MSR_INITIAL		0x00002000
#define PPC_INIT_FPSCR		0x000000f8
#define PPC_USE_MULTIPLE	0
#define PPC_I_CACHE		8192
#define PPC_D_CACHE		8192

#define PPC_MSR_0		0x00009900
#define PPC_MSR_1		0x00001000
#define PPC_MSR_2		0x00001000
#define PPC_MSR_3		0x00000000

#elif defined(ppc603e)
 
#define CPU_MODEL_NAME  "PowerPC 603e"

#define PPC_ALIGNMENT		8
#define PPC_CACHE_ALIGNMENT	32
#define PPC_CACHE_ALIGN_POWER	5
#define PPC_INTERRUPT_MAX	16
#define PPC_HAS_FPU     	1
#define PPC_HAS_DOUBLE		1
#define PPC_HAS_RFCI		0
#define PPC_MSR_DISABLE_MASK	0x00009900
#define PPC_MSR_INITIAL		0x00002000
#define PPC_INIT_FPSCR		0x000000f8
#define PPC_USE_MULTIPLE	0
#define PPC_I_CACHE		16384
#define PPC_D_CACHE		16384

#define PPC_MSR_0		0x00009900
#define PPC_MSR_1		0x00001000
#define PPC_MSR_2		0x00001000
#define PPC_MSR_3		0x00000000

#elif defined(ppc604)
 
#define CPU_MODEL_NAME  "PowerPC 604"

#define PPC_ALIGNMENT		8
#define PPC_CACHE_ALIGNMENT	32
#define PPC_CACHE_ALIGN_POWER	5
#define PPC_INTERRUPT_MAX	16
#define PPC_HAS_FPU     	1
#define PPC_HAS_DOUBLE		1
#define PPC_HAS_RFCI		0
#define PPC_MSR_DISABLE_MASK	0x00009900
#define PPC_MSR_INITIAL		0x00002000
#define PPC_INIT_FPSCR		0x000000f8
#define PPC_USE_MULTIPLE	0
#define PPC_I_CACHE		16384
#define PPC_D_CACHE		16384
 
#define PPC_MSR_0		0x00009900
#define PPC_MSR_1		0x00001000
#define PPC_MSR_2		0x00001000
#define PPC_MSR_3		0x00000000

#else
 
#error "Unsupported CPU Model"
 
#endif

/*
 *  Application binary interfaces.
 *  PPC_ABI MUST be defined as one of these.
 *  Only PPC_ABI_POWEROPEN is currently fully supported.
 *  Only EABI will be supported in the end when
 *  the tools are there.
 *  Only big endian is currently supported.
 */
/*
 *  PowerOpen ABI.  This is Andy's hack of the
 *  PowerOpen ABI to ELF.  ELF rather than a
 *  XCOFF assembler is used.  This may work
 *  if PPC_ASM == PPC_ASM_XCOFF is defined.
 */
#define PPC_ABI_POWEROPEN	0
/*
 *  GCC 2.7.0 munched version of EABI, with
 *  PowerOpen calling convention and stack frames,
 *  but EABI style indirect function calls.
 */
#define PPC_ABI_GCC27		1
/*
 *  SVR4 ABI
 */
#define PPC_ABI_SVR4		2
/*
 *  Embedded ABI
 */
#define PPC_ABI_EABI		3

#if (PPC_ABI == PPC_ABI_POWEROPEN)
#define PPC_STACK_ALIGNMENT	8
#elif (PPC_ABI == PPC_ABI_GCC27)
#define PPC_STACK_ALIGNMENT	8
#elif (PPC_ABI == PPC_ABI_SVR4)
#define PPC_STACK_ALIGNMENT	16
#elif (PPC_ABI == PPC_ABI_EABI)
#define PPC_STACK_ALIGNMENT	8
#else
#error  "PPC_ABI is not properly defined"
#endif
#ifndef PPC_ABI
#error  "PPC_ABI is not properly defined"
#endif

/*
 *  Assemblers.
 *  PPC_ASM MUST be defined as one of these.
 *  Only PPC_ABI_ELF is currently fully supported.
 */
/*
 *  ELF assembler. Currently used for all ABIs.
 */
#define PPC_ASM_ELF		0
/*
 *  XCOFF assembler, may be needed for PowerOpen ABI.
 */
#define PPC_ASM_XCOFF		1

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "PowerPC"

/*
 *  Interrupt vectors.
 */
/* Machine check */
#define PPC_IRQ_MCHECK		0
/* Protection violation */
#define PPC_IRQ_PROTECT		1
/* External interrupt */
#define PPC_IRQ_EXTERNAL	2
/* Program exception */
#define PPC_IRQ_PROGRAM		3
/* System call */
#define PPC_IRQ_SCALL		4
/* Floating point unavailable */
#define PPC_IRQ_NOFP		5
/* Program interval timer */
#define PPC_IRQ_PIT		6
/* Fixed interval timer */
#define PPC_IRQ_FIT		7
/* Critical interrupt pin */
#define PPC_IRQ_CRIT		8
/* Watchdog timer */
#define PPC_IRQ_WATCHDOG	9
/* Debug exceptions */
#define PPC_IRQ_DEBUG		10

/*
 *  The following exceptions are not maskable, and are not
 *  necessarily predictable, so cannot be offered to RTEMS:
 *    Alignment exception - handled by the CPU module
 *    Data exceptions.
 *    Instruction exceptions.
 */

#ifdef __cplusplus
}
#endif

#endif /* ! _INCLUDE_PPC_h */
/* end of include file */

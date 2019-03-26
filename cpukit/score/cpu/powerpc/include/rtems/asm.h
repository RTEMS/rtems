/**
 * @file
 * 
 * @brief Address the Problems Caused by Incompatible Flavor of 
 * Assemblers and Toolsets
 *
 * This include file attempts to address the problems
 * caused by incompatible flavors of assemblers and
 * toolsets.  It primarily addresses variations in the
 * use of leading underscores on symbols and the requirement
 * that register names be preceded by a %.
 * 
 * NOTE: The spacing in the use of these macros
 *       is critical to them working as advertised.
 */

/*
 *  COPYRIGHT:
 *
 *  This file is based on similar code found in newlib available
 *  from ftp.cygnus.com.  The file which was used had no copyright
 *  notice.  This file is freely distributable as long as the source
 *  of the file is noted.  This file is:
 *
 *  COPYRIGHT (c) 1995.
 *  i-cubed ltd.
 *
 *  COPYRIGHT (c) 1994.
 *  On-Line Applications Research Corporation (OAR).
 */

#ifndef _RTEMS_ASM_H
#define _RTEMS_ASM_H

/*
 *  Indicate we are in an assembly file and get the basic CPU definitions.
 */

#ifndef ASM
#define ASM
#endif
#include <rtems/score/cpuopts.h>
#include <rtems/score/powerpc.h>

/**
 * @defgroup RTEMSScoreCPUPowerPCASM PowerPC Assembler Support
 *
 * @ingroup RTEMSScoreCPUPowerPC
 *
 * @brief PowerPC Assembler Support
 *
 * @{
 */

/*
 *  Recent versions of GNU cpp define variables which indicate the
 *  need for underscores and percents.  If not using GNU cpp or
 *  the version does not support this, then you will obviously
 *  have to define these as appropriate.
 */

#ifndef __USER_LABEL_PREFIX__
#define __USER_LABEL_PREFIX__
#endif

#ifndef __REGISTER_PREFIX__
#define __REGISTER_PREFIX__
#endif

#ifndef __FLOAT_REGISTER_PREFIX__
#define __FLOAT_REGISTER_PREFIX__  __REGISTER_PREFIX__
#endif

#ifndef __PROC_LABEL_PREFIX__
#define __PROC_LABEL_PREFIX__  __USER_LABEL_PREFIX__
#endif

#include <rtems/concat.h>

/* Use the right prefix for global labels.  */

#define SYM(x) CONCAT1 (__USER_LABEL_PREFIX__, x)

/* Use the right prefix for procedure labels.  */

#define PROC(x) CONCAT1 (__PROC_LABEL_PREFIX__, x)

/* Use the right prefix for registers.  */

#define REG(x) CONCAT1 (__REGISTER_PREFIX__, x)

/* Use the right prefix for floating point registers.  */

#define FREG(x) CONCAT1 (__FLOAT_REGISTER_PREFIX__, x)

/*
 *  define macros for all of the registers on this CPU
 *
 *  EXAMPLE:     #define d0 REG (d0)
 */
#define r0 REG(0)
#define r1 REG(1)
#define r2 REG(2)
#define r3 REG(3)
#define r4 REG(4)
#define r5 REG(5)
#define r6 REG(6)
#define r7 REG(7)
#define r8 REG(8)
#define r9 REG(9)
#define r10 REG(10)
#define r11 REG(11)
#define r12 REG(12)
#define r13 REG(13)
#define r14 REG(14)
#define r15 REG(15)
#define r16 REG(16)
#define r17 REG(17)
#define r18 REG(18)
#define r19 REG(19)
#define r20 REG(20)
#define r21 REG(21)
#define r22 REG(22)
#define r23 REG(23)
#define r24 REG(24)
#define r25 REG(25)
#define r26 REG(26)
#define r27 REG(27)
#define r28 REG(28)
#define r29 REG(29)
#define r30 REG(30)
#define r31 REG(31)
#define f0 FREG(0)
#define f1 FREG(1)
#define f2 FREG(2)
#define f3 FREG(3)
#define f4 FREG(4)
#define f5 FREG(5)
#define f6 FREG(6)
#define f7 FREG(7)
#define f8 FREG(8)
#define f9 FREG(9)
#define f10 FREG(10)
#define f11 FREG(11)
#define f12 FREG(12)
#define f13 FREG(13)
#define f14 FREG(14)
#define f15 FREG(15)
#define f16 FREG(16)
#define f17 FREG(17)
#define f18 FREG(18)
#define f19 FREG(19)
#define f20 FREG(20)
#define f21 FREG(21)
#define f22 FREG(22)
#define f23 FREG(23)
#define f24 FREG(24)
#define f25 FREG(25)
#define f26 FREG(26)
#define f27 FREG(27)
#define f28 FREG(28)
#define f29 FREG(29)
#define f30 FREG(30)
#define f31 FREG(31)
#define v0 0
#define v1 1
#define v2 2
#define v3 3
#define v4 4
#define v5 5
#define v6 6
#define v7 7
#define v8 8
#define v9 9
#define v10 10
#define v11 11
#define v12 12
#define v13 13
#define v14 14
#define v15 15
#define v16 16
#define v17 17
#define v18 18
#define v19 19
#define v20 20
#define v21 21
#define v22 22
#define v23 23
#define v24 24
#define v25 25
#define v26 26
#define v27 27
#define v28 28
#define v29 29
#define v30 30
#define v31 31

/*
 * Some special purpose registers (SPRs).
 */
#define srr0	0x01a
#define srr1	0x01b
#define srr2	0x3de /* IBM 400 series only */
#define srr3	0x3df /* IBM 400 series only */
#define csrr0	58 /* Book E */
#define csrr1	59 /* Book E */
#define mcsrr0	570 /* e500 */
#define mcsrr1	571 /* e500 */
#define dsrr0	574 /* e200 */
#define dsrr1	575 /* e200 */

#define sprg0	0x110
#define sprg1	0x111
#define sprg2	0x112
#define sprg3	0x113
#define sprg4	276
#define sprg5	277
#define sprg6	278
#define sprg7	279

#define usprg0	256

#define dar     0x013   /* Data Address Register */
#define dec     0x016   /* Decrementer Register */

#if defined(ppc403) || defined(ppc405)
/* the following SPR/DCR registers exist only in IBM 400 series */
#define dear	0x3d5
#define evpr    0x3d6   /* SPR: exception vector prefix register   */
#define iccr    0x3fb   /* SPR: instruction cache control reg.     */
#define dccr    0x3fa   /* SPR: data cache control reg.            */

#if defined (ppc403)
#define exisr   0x040   /* DCR: external interrupt status register */
#define exier   0x042   /* DCR: external interrupt enable register */
#endif /* ppc403 */
#if defined(ppc405)
#define exisr   0x0C0   /* DCR: external interrupt status register */
#define exier   0x0C2   /* DCR: external interrupt enable register */
#endif /* ppc405 */

#define br0     0x080   /* DCR: memory bank register 0             */
#define br1     0x081   /* DCR: memory bank register 1             */
#define br2     0x082   /* DCR: memory bank register 2             */
#define br3     0x083   /* DCR: memory bank register 3             */
#define br4     0x084   /* DCR: memory bank register 4             */
#define br5     0x085   /* DCR: memory bank register 5             */
#define br6     0x086   /* DCR: memory bank register 6             */
#define br7     0x087   /* DCR: memory bank register 7             */

/* end of IBM400 series register definitions */

#elif defined(mpc860) || defined(mpc821)
/* The following registers are for the MPC8x0 */
#define der     0x095   /* Debug Enable Register */
#define ictrl   0x09E   /* Instruction Support Control Register */
#define immr    0x27E   /* Internal Memory Map Register */
/* end of MPC8x0 registers */
#endif

/*
 *  Following must be tailor for a particular flavor of the C compiler.
 *  They may need to put underscores in front of the symbols.
 */

#define PUBLIC_VAR(sym) .globl SYM (sym)
#define EXTERN_VAR(sym) .extern SYM (sym)
#define PUBLIC_PROC(sym) .globl PROC (sym)
#define EXTERN_PROC(sym) .extern PROC (sym)

/* Other potentially assembler specific operations */
#if PPC_ASM == PPC_ASM_ELF
#define ALIGN(n,p)	.align	p
#define DESCRIPTOR(x)	\
	.section .descriptors,"aw";	\
	PUBLIC_VAR (x);			\
SYM (x):;				\
	.long	PROC (x);		\
	.long	s.got;			\
	.long	0

#define EXT_SYM_REF(x)	.long x
#define EXT_PROC_REF(x)	.long x

/*
 *  Define macros to handle section beginning and ends.
 */

#define BEGIN_CODE_DCL .text
#define END_CODE_DCL
#define BEGIN_DATA_DCL .data
#define END_DATA_DCL
#define BEGIN_CODE .text
#define END_CODE
#define BEGIN_DATA .data
#define END_DATA
#define BEGIN_BSS  .bss
#define END_BSS
#define END

#else
#error "PPC_ASM_TYPE is not properly defined"
#endif
#ifndef PPC_ASM
#error "PPC_ASM_TYPE is not properly defined"
#endif

#if defined(__powerpc64__)
#define PPC64_NOP_FOR_LINKER_TOC_POINTER_RESTORE nop
#else
#define PPC64_NOP_FOR_LINKER_TOC_POINTER_RESTORE
#endif

#endif

/** @} */

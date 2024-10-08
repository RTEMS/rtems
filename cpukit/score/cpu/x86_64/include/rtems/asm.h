/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Addresses Incompatible Flavors Problems
 *
 *  This include file attempts to address the problems
 *  caused by incompatible flavors of assemblers and
 *  toolsets.  It primarily addresses variations in the
 *  use of leading underscores on symbols and the requirement
 *  that register names be preceded by a %.
 *
 *  NOTE: The spacing in the use of these macros
 *        is critical to them working as advertised.
 */

/*
 *  COPYRIGHT:
 *
 *  This file is based on similar code found in newlib available
 *  from ftp.cygnus.com.  The file which was used had no copyright
 *  notice.  This file is freely distributable as long as the source
 *  of the file is noted.  This file is:
 *
 *  COPYRIGHT (c) 2018 Amaan Cheval <amaan.cheval@gmail.com>
 *
 *  COPYRIGHT (c) 1994-2006.
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
#include <rtems/score/x86_64.h>
#include <rtems/score/percpu.h>

/**
 * @defgroup RTEMSScoreCPUx86-64ASM x86-64 Assembler Support
 *
 * @ingroup RTEMSScoreCPUx86-64
 *
 * @brief x86-64 Assembler Support
 *
 * @{
 */

#ifndef __USER_LABEL_PREFIX__
/**
 *  Recent versions of GNU cpp define variables which indicate the
 *  need for underscores and percents.  If not using GNU cpp or
 *  the version does not support this, then you will obviously
 *  have to define these as appropriate.
 *
 *  This symbol is prefixed to all C program symbols.
 */
#define __USER_LABEL_PREFIX__ _
#endif

#undef __REGISTER_PREFIX__
#define __REGISTER_PREFIX__ %

#include <rtems/concat.h>

/** Use the right prefix for global labels.  */
#define SYM(x) CONCAT1 (__USER_LABEL_PREFIX__, x)

/** Use the right prefix for registers.  */
#define REG(x) CONCAT1 (__REGISTER_PREFIX__, x)

/*
 *  define macros for all of the registers on this CPU
 */
#define rax REG (rax)
#define rbx REG (rbx)
#define rcx REG (rcx)
#define rdx REG (rdx)
#define rdi REG (rdi)
#define rsi REG (rsi)
#define rbp REG (rbp)
#define rsp REG (rsp)
#define r8  REG (r8)
#define r9  REG (r9)
#define r10 REG (r10)
#define r11 REG (r11)
#define r12 REG (r12)
#define r13 REG (r13)
#define r14 REG (r14)
#define r15 REG (r15)

/*
 * Order of register usage for function arguments as per the calling convention
 */
#define REG_ARG0 rdi
#define REG_ARG1 rsi
#define REG_ARG2 rdx
#define REG_ARG3 rcx
#define REG_ARG4 r8
#define REG_ARG5 r9

// XXX: eax, ax, etc., segment registers

/*
 *  Define macros to handle section beginning and ends.
 */

/** This macro is used to denote the beginning of a code declaration. */
#define BEGIN_CODE_DCL .text
/** This macro is used to denote the end of a code declaration. */
#define END_CODE_DCL
/** This macro is used to denote the beginning of a data declaration section. */
#define BEGIN_DATA_DCL .data
/** This macro is used to denote the end of a data declaration section. */
#define END_DATA_DCL
/** This macro is used to denote the beginning of a code section. */
#define BEGIN_CODE .text
/** This macro is used to denote the end of a code section. */
#define END_CODE
/** This macro is used to denote the beginning of a data section. */
#define BEGIN_DATA
/** This macro is used to denote the end of a data section. */
#define END_DATA
/** This macro is used to denote the beginning of the
 *  unitialized data section.
 */
#define BEGIN_BSS
/** This macro is used to denote the end of the unitialized data section.  */
#define END_BSS
/** This macro is used to denote the end of the assembly file.  */
#define END

/**
 *  This macro is used to declare a public global symbol.
 *
 *  @note This must be tailored for a particular flavor of the C compiler.
 *  They may need to put underscores in front of the symbols.
 */
#define PUBLIC(sym) .globl SYM (sym)

/**
 *  This macro is used to prototype a public global symbol.
 *
 *  @note This must be tailored for a particular flavor of the C compiler.
 *  They may need to put underscores in front of the symbols.
 */
#define EXTERN(sym) .globl SYM (sym)

#ifdef RTEMS_SMP
/* REG32 must be the lower 32 bits of REG */
.macro GET_CPU_INDEX REG REG32
    .set LAPIC_ID,       0x20
    .set LAPIC_ID_SHIFT, 24
    movq amd64_lapic_base, \REG
    movl LAPIC_ID(\REG), \REG32
    shrq $LAPIC_ID_SHIFT, \REG                /* LAPIC_ID in REG */
    movzbq amd64_lapic_to_cpu_map(\REG), \REG /* CPU ID in REG */
.endm

/* REG32 must be the lower 32 bits of REG */
.macro GET_SELF_CPU_CONTROL REG REG32
    GET_CPU_INDEX \REG \REG32
    shlq $PER_CPU_CONTROL_SIZE_LOG2, \REG /* Calculate offset for CPU structure */
    leaq _Per_CPU_Information(\REG), \REG /* Address of info for current CPU in REG */
.endm
#else
.macro GET_CPU_INDEX REG REG32
    movq $0, \REG
.endm

.macro GET_SELF_CPU_CONTROL REG REG32
    leaq _Per_CPU_Information, \REG
.endm
#endif

/* Couldn't find a better way to do this under the GNU as macro limitations */
.macro GET_SELF_CPU_CONTROL_RAX
    GET_SELF_CPU_CONTROL rax,%eax
.endm
.macro GET_SELF_CPU_CONTROL_RBX
    GET_SELF_CPU_CONTROL rbx,%ebx
.endm
.macro GET_SELF_CPU_CONTROL_RCX
    GET_SELF_CPU_CONTROL rcx,%ecx
.endm
.macro GET_SELF_CPU_CONTROL_RDX
    GET_SELF_CPU_CONTROL rdx,%edx
.endm
.macro GET_SELF_CPU_CONTROL_RDI
    GET_SELF_CPU_CONTROL rdi,%edi
.endm
.macro GET_SELF_CPU_CONTROL_RSI
    GET_SELF_CPU_CONTROL rsi,%esi
.endm
.macro GET_SELF_CPU_CONTROL_R8
    GET_SELF_CPU_CONTROL r8,%r8d
.endm
.macro GET_SELF_CPU_CONTROL_R9
    GET_SELF_CPU_CONTROL r9,%r9d
.endm
.macro GET_SELF_CPU_CONTROL_R10
    GET_SELF_CPU_CONTROL r10,%r10d
.endm
.macro GET_SELF_CPU_CONTROL_R11
    GET_SELF_CPU_CONTROL r11,%r11d
.endm
.macro GET_SELF_CPU_CONTROL_R12
    GET_SELF_CPU_CONTROL r12,%r12d
.endm
.macro GET_SELF_CPU_CONTROL_R13
    GET_SELF_CPU_CONTROL r13,%r13d
.endm
.macro GET_SELF_CPU_CONTROL_R14
    GET_SELF_CPU_CONTROL r14,%r14d
.endm
.macro GET_SELF_CPU_CONTROL_R15
    GET_SELF_CPU_CONTROL r15,%r15d
.endm

#endif // _RTEMS_ASM_H

/** @} */

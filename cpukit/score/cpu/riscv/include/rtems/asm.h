/**
 * @file
 *
 *  This include file attempts to address the problems
 *  caused by incompatible flavors of assemblers and
 *  toolsets.  It primarily addresses variations in the
 *  use of leading underscores on symbols and the requirement
 *  that register names be preceded by a %.
 */

/*
 *  NOTE: The spacing in the use of these macros
 *        is critical to them working as advertised.
 *
 *  This file is based on similar code found in newlib available
 *  from ftp.cygnus.com.  The file which was used had no copyright
 *  notice.  This file is freely distributable as long as the source
 *  of the file is noted.  This file is:
 *
 * Copyright (c) 2015 University of York.
 * Hesham Almatary <hesham@alumni.york.ac.uk>
 *
 *
 * COPYRIGHT (c) 1994-1997.
 * On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __RISCV_ASM_H
#define __RISCV_ASM_H

/*
 *  Indicate we are in an assembly file and get the basic CPU definitions.
 */

#ifndef ASM
#define ASM
#endif
#include <rtems/score/cpuopts.h>
#include <rtems/score/riscv.h>

/**
 * @defgroup RTEMSScoreCPURISCVASM RISC-V Assembler Support
 *
 * @ingroup RTEMSScoreCPURISCV
 *
 * @brief RISC-V Assembler Support
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
#define __USER_LABEL_PREFIX__ _
#endif

#ifndef __REGISTER_PREFIX__
#define __REGISTER_PREFIX__
#endif

/* ANSI concatenation macros.  */

#define CONCAT1(a, b) CONCAT2(a, b)
#define CONCAT2(a, b) a ## b

/* Use the right prefix for global labels.  */

#define SYM(x) CONCAT1 (__USER_LABEL_PREFIX__, x)

/* Use the right prefix for registers.  */

#define REG(x) CONCAT1 (__REGISTER_PREFIX__, x)

/*
 *  define macros for all of the registers on this CPU
 *
 *  EXAMPLE:     #define d0 REG (d0)
 */

/*
 *  Define macros to handle section beginning and ends.
 */
#define BEGIN_CODE_DCL .text
#define END_CODE_DCL
#define BEGIN_DATA_DCL .data
#define END_DATA_DCL
#define BEGIN_CODE .text
#define END_CODE
#define BEGIN_DATA
#define END_DATA
#define BEGIN_BSS
#define END_BSS
#define END

/*
 *  Following must be tailor for a particular flavor of the C compiler.
 *  They may need to put underscores in front of the symbols.
 */

#define PUBLIC(sym)    .global SYM (sym)
#define EXTERN(sym)    .extern SYM (sym)
#define TYPE_FUNC(sym) .type SYM (sym), %function

#if __riscv_xlen == 32

#define LREG lw

#define SREG sw

#elif __riscv_xlen == 64

#define LREG ld

#define SREG sd

#endif /* __riscv_xlen */

#ifdef __riscv_cmodel_medany

#define LADDR lla

#else /* !__riscv_cmodel_medany */

#define LADDR la

#endif /* __riscv_cmodel_medany */

#if __riscv_flen == 32

#define FLREG flw

#define FSREG fsw

#define FMVYX fmv.s.x

#define FMVXY fmv.x.s

#elif __riscv_flen == 64

#define FLREG fld

#define FSREG fsd

#if __riscv_xlen == 32

#define FMVYX fmv.s.x

#define FMVXY fmv.x.s

#elif __riscv_xlen == 64

#define FMVYX fmv.d.x

#define FMVXY fmv.x.d

#endif /* __riscv_xlen */

#endif /* __riscv_flen */

.macro GET_SELF_CPU_CONTROL REG
#ifdef RTEMS_SMP
	csrr	\REG, mscratch
#else
	LADDR	\REG, _Per_CPU_Information
#endif
.endm

.macro CLEAR_RESERVATIONS REG
#ifdef __riscv_atomic
	/*
	 * Clear reservations, see also RISC-V User-Level ISA V2.3, comment in
	 * section 8.2 "Load-Reserved/Store-Conditional Instructions".
	 */
	sc.w	zero, zero, (\REG)
#endif
.endm

#endif

/** @} */

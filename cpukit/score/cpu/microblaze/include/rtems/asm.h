/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief MicroBlaze assembler support
 *
 * This include file attempts to address the problems
 * caused by incompatible flavors of assemblers and
 * toolsets.  It primarily addresses variations in the
 * use of leading underscores on symbols and the requirement
 * that register names be preceded by a %.
 */

/*
 * Copyright (c) 2015, Hesham Almatary
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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

#ifndef __REGISTER_PREFIX__
/**
 *  Recent versions of GNU cpp define variables which indicate the
 *  need for underscores and percents.  If not using GNU cpp or
 *  the version does not support this, then you will obviously
 *  have to define these as appropriate.
 *
 *  This symbol is prefixed to all register names.
 */
#define __REGISTER_PREFIX__
#endif

#include <rtems/concat.h>

/** Use the right prefix for global labels.  */
#define SYM(x) CONCAT1 (__USER_LABEL_PREFIX__, x)

/** Use the right prefix for registers.  */
#define REG(x) CONCAT1 (__REGISTER_PREFIX__, x)

/*
 *  define macros for all of the registers on this CPU
 *
 *  EXAMPLE:     #define d0 REG (d0)
 */

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

#endif

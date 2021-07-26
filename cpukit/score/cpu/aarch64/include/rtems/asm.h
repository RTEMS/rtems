/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief AArch64 Assembler Support API
 *
 * This include file attempts to address the problems
 * caused by incompatible flavors of assemblers and
 * toolsets.  It primarily addresses variations in the
 * use of leading underscores on symbols and the requirement
 * that register names be preceded by a %.
 *
 *
 * NOTE: The spacing in the use of these macros
 *       is critical to them working as advertised.
 */

/*
 * Copyright (C) 2020 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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
#include <rtems/score/percpu.h>

/**
 * @defgroup RTEMSScoreCPUAArch64ASM AArch64 Assembler Support
 *
 * @ingroup RTEMSScoreCPUAArch64
 *
 * @brief AArch64 Assembler Support
 */
/**@{**/

/*
 *  Following must be tailor for a particular flavor of the C compiler.
 *  They may need to put underscores in front of the symbols.
 */

#define FUNCTION_ENTRY(name) \
  .align 8; \
  .globl name; \
  .type name, %function; \
  name:

#define FUNCTION_END(name) \
  .size name, . - name

#define DEFINE_FUNCTION_AARCH64(name) \
  .align 8 ; .globl name ; name: ; .globl name ## _aarch64 ; name ## _aarch64:

.macro GET_SELF_CPU_CONTROL REG
#ifdef RTEMS_SMP
	/* Use Thread ID Register (TPIDR_EL1) */
	mrs	\REG, TPIDR_EL1
#else
	ldr	\REG, =_Per_CPU_Information
#endif
.endm

/** @} */

#endif /* _RTEMS_ASM_H */

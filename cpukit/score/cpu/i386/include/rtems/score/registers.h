/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * 
 * @brief Intel CPU Constants and Definitions
 * 
 * @addtogroup RTEMSScoreCPUi386
 * 
 * This file contains definition and constants related to Intel Cpu
 */

/*
 *  COPYRIGHT (c) 1998 valette@crf.canon.fr
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

#ifndef _RTEMS_SCORE_REGISTERS_H
#define _RTEMS_SCORE_REGISTERS_H

/*
 * definition related to EFLAGS
 */
#define EFLAGS_CARRY			0x1
#define EFLAGS_PARITY			0x4

#define EFLAGS_AUX_CARRY		0x10
#define EFLAGS_ZERO			0x40
#define EFLAGS_SIGN			0x80

#define EFLAGS_TRAP			0x100
#define EFLAGS_INTR_ENABLE		0x200
#define EFLAGS_DIRECTION		0x400
#define EFLAGS_OVERFLOW			0x800

#define	EFLAGS_IOPL_MASK		0x3000
#define EFLAGS_NESTED_TASK		0x8000

#define EFLAGS_RESUME			0x10000
#define EFLAGS_VIRTUAL_MODE		0x20000
#define EFLAGS_ALIGN_CHECK		0x40000
#define EFLAGS_VIRTUAL_INTR		0x80000

#define EFLAGS_VIRTUAL_INTR_PEND	0x100000
#define EFLAGS_ID			0x200000

/*
 * definitions related to CR0
 */
#define CR0_PROTECTION_ENABLE		0x1
#define CR0_MONITOR_COPROC		0x2
#define CR0_COPROC_SOFT_EMUL		0x4
#define CR0_FLOATING_INSTR_EXCEPTION	0x8

#define CR0_EXTENSION_TYPE		0x10
#define CR0_NUMERIC_ERROR		0x20

#define CR0_WRITE_PROTECT		0x10000
#define CR0_ALIGMENT_MASK		0x40000

#define CR0_NO_WRITE_THROUGH		0x20000000
#define CR0_PAGE_LEVEL_CACHE_DISABLE	0x40000000
#define CR0_PAGING			0x80000000

/*
 * definitions related to CR3
 */

#define CR3_PAGE_CACHE_DISABLE		0x10
#define CR3_PAGE_WRITE_THROUGH		0x8
#define CR3_PAGE_DIRECTORY_MASK		0xFFFFF000

#endif

/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief PowerPC Paravirtualization Definitions
 *
 * This include file contains definitions pertaining to paravirtualization
 * of the PowerPC port.
 */

/*
 *  COPYRIGHT (c) 2018.
 *  On-Line Applications Research Corporation (OAR).
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

#ifndef RTEMS_PARAVIRT
#error "This file should only be included with paravirtualization is enabled."
#endif

#ifndef _RTEMS_SCORE_PARAVIRT_H
#define _RTEMS_SCORE_PARAVIRT_H

/**
 * @defgroup RTEMSScoreCPUPowerPCParavirt PowerPC Paravirtualization Support
 *
 * @ingroup RTEMSScoreCPUPowerPC
 * 
 * @brief PowerPC Paravirtualization Support
 *
 * This handler encapulates the functionality (primarily conditional
 * feature defines) related to paravirtualization on the PowerPC.
 *
 * Paravirtualization on the PowerPC makes the following assumptions:
 *
 *   - RTEMS executes in user space
 *   - In user space there is no access to the MSR.
 *   - Interrupt enable/disable support using the MSR must be disabled
 *     and replaced with BSP provided methods which are adapted to the
 *     hosting environment.
 */

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif  /* !ASM */

/**
 * In a paravirtualized environment, RTEMS executes in user space
 * and cannot disable/enable external exceptions (e.g. interrupts).
 * The BSP which acts as an adapter to the hosting environment will
 * provide the interrupt enable/disable methods.
 */
#define PPC_DISABLE_INLINE_ISR_DISABLE_ENABLE

/**
 * In a paravirtualized environment, RTEMS executes in user space
 * and cannot access the MSR.
 *
 * Try to have as little impact as possible with this define.  Leave
 * the msr in the thread context because that would impact the definition
 * of offsets for assembly code.
 */
#define PPC_DISABLE_MSR_ACCESS

#endif

/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUAArch64Paravirt
 *
 * @brief This header file provides definitions pertaining to
 *   paravirtualization of the AArch64 port.
 */

/*
 *  COPYRIGHT (c) 2023-2024.
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
 * @defgroup RTEMSScoreCPUAArch64Paravirt AArch64 Paravirtualization Support
 *
 * @ingroup RTEMSScoreCPUAArch64
 *
 * @brief This handler encapsulates the functionality (primarily conditional
 *        feature defines) related to paravirtualization on the AArch64.
 *
 * Paravirtualization on the AArch64 makes the following assumptions:
 *
 *   - RTEMS executes in user space (EL0)
 *   - Interrupt enable/disable support using the MSR must be disabled
 *     and replaced with BSP provided methods which are adapted to the
 *     hosting environment.
 *
 * @{
 */

/**
 * In a paravirtualized environment, RTEMS executes in user space
 * and cannot disable/enable external exceptions (e.g. interrupts).
 * The BSP which acts as an adapter to the hosting environment will
 * provide the interrupt enable/disable methods.
 */
#define AARCH64_DISABLE_INLINE_ISR_DISABLE_ENABLE

/** @} */

#endif

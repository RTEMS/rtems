/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup lm32_clock
 * @brief LatticeMico32 Timer (Clock) definitions
 */

/*
 *  This file contains definitions for LatticeMico32 Timer (Clock)
 *
 *  COPYRIGHT (c) 1989-1999.
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
 *
 *  Jukka Pietarinen <jukka.pietarinen@mrf.fi>, 2008,
 *  Micro-Research Finland Oy
 */

/**
 * @defgroup lm32_clock LM32 Clock
 * @ingroup RTEMSBSPsLM32Shared
 * @brief LatticeMico32 Timer (Clock) definitions.
 * @{
 */

#ifndef _BSPCLOCK_H
#define _BSPCLOCK_H

/** @brief Status Register */

#define LM32_CLOCK_SR       (0x0000)
#define LM32_CLOCK_SR_TO    (0x0001)
#define LM32_CLOCK_SR_RUN   (0x0002)

/** @brief Control Register */

#define LM32_CLOCK_CR       (0x0004)
#define LM32_CLOCK_CR_ITO   (0x0001)
#define LM32_CLOCK_CR_CONT  (0x0002)
#define LM32_CLOCK_CR_START (0x0004)
#define LM32_CLOCK_CR_STOP  (0x0008)

/** @brief Period Register */

#define LM32_CLOCK_PERIOD   (0x0008)

/** @brief Snapshot Register */

#define LM32_CLOCK_SNAPSHOT (0x000C)

#endif /* _BSPCLOCK_H */

/** @} */

/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsShared
 *
 * @brief Header defining architecture-specific clock functions.
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

#include <rtems/score/basedefs.h>

/**
 * This function returns the current compare value for the ARM General Purpose
 * Timer.
 *
 * @return The current compare value.
 */
uint64_t arm_gt_clock_get_compare_value(void);

/**
 * This function sets the current compare value for the ARM General Purpose
 * Timer.
 *
 * @param[in] cval The value to set as the compare value
 */
void arm_gt_clock_set_compare_value(uint64_t cval);

/**
 * This function returns the count for the ARM General Purpose Timer.
 *
 * @return The current count.
 */
uint64_t arm_gt_clock_get_count(void);

/**
 * This function sets the control register for the ARM General Purpose Timer.
 *
 * @param[in] ctl The value to set to the control register
 */
void arm_gt_clock_set_control(uint32_t ctl);

/**
 * This function gets the frequency and IRQ number used by the ARM General
 * Purpose Timer.
 *
 * @param[out] frequency The frequency at which the timer will fire.
 * @param[out] irq The number of the IRQ on which the timer will fire.
 */
void arm_generic_timer_get_config(uint32_t *frequency, uint32_t *irq);

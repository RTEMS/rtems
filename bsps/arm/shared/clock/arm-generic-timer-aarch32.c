/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMShared
 *
 * @brief ARM-specific clock driver functions.
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

#include <libcpu/arm-cp15.h>
#include <dev/clock/arm-generic-timer.h>

uint64_t arm_gt_clock_get_compare_value(void)
{
#ifdef ARM_GENERIC_TIMER_USE_VIRTUAL
  return arm_cp15_get_counter_pl1_virtual_compare_value();
#else
  return arm_cp15_get_counter_pl1_physical_compare_value();
#endif
}

void arm_gt_clock_set_compare_value(uint64_t cval)
{
#ifdef ARM_GENERIC_TIMER_USE_VIRTUAL
  arm_cp15_set_counter_pl1_virtual_compare_value(cval);
#else
  arm_cp15_set_counter_pl1_physical_compare_value(cval);
#endif
}

uint64_t arm_gt_clock_get_count(void)
{
#ifdef ARM_GENERIC_TIMER_USE_VIRTUAL
  return arm_cp15_get_counter_virtual_count();
#else
  return arm_cp15_get_counter_physical_count();
#endif
}

void arm_gt_clock_set_control(uint32_t ctl)
{
#ifdef ARM_GENERIC_TIMER_USE_VIRTUAL
  arm_cp15_set_counter_pl1_virtual_timer_control(ctl);
#else
  arm_cp15_set_counter_pl1_physical_timer_control(ctl);
#endif
}

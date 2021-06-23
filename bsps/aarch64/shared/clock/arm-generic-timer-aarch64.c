/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64Shared
 *
 * @brief AArch64-specific ARM GPT system register accessors.
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

#include <dev/clock/arm-generic-timer.h>
#include <bsp/irq.h>

uint64_t arm_gt_clock_get_compare_value(void)
{
  uint64_t val;
  __asm__ volatile (
#ifdef AARCH64_GENERIC_TIMER_USE_VIRTUAL
    "mrs %[val], cntv_cval_el0"
#elif defined(AARCH64_GENERIC_TIMER_USE_PHYSICAL_SECURE)
    "mrs %[val], cntps_cval_el1"
#else
    "mrs %[val], cntp_cval_el0"
#endif
    : [val] "=&r" (val)
  );
  return val;
}

void arm_gt_clock_set_compare_value(uint64_t cval)
{
  __asm__ volatile (
#ifdef AARCH64_GENERIC_TIMER_USE_VIRTUAL
    "msr cntv_cval_el0, %[cval]"
#elif defined(AARCH64_GENERIC_TIMER_USE_PHYSICAL_SECURE)
    "msr cntps_cval_el1, %[cval]"
#else
    "msr cntp_cval_el0, %[cval]"
#endif
    :
    : [cval] "r" (cval)
  );
}

uint64_t arm_gt_clock_get_count(void)
{
  uint64_t val;
  __asm__ volatile (
#ifdef AARCH64_GENERIC_TIMER_USE_VIRTUAL
    "mrs %[val], cntvct_el0"
#else
    "mrs %[val], cntpct_el0"
#endif
    : [val] "=&r" (val)
  );
  return val;
}

void arm_gt_clock_set_control(uint32_t ctl)
{
  __asm__ volatile (
#ifdef AARCH64_GENERIC_TIMER_USE_VIRTUAL
    "msr cntv_ctl_el0, %[ctl]"
#elif defined(AARCH64_GENERIC_TIMER_USE_PHYSICAL_SECURE)
    "msr cntps_ctl_el1, %[ctl]"
#else
    "msr cntp_ctl_el0, %[ctl]"
#endif
    :
    : [ctl] "r" (ctl)
  );
}

void arm_generic_timer_get_config( uint32_t *frequency, uint32_t *irq )
{
  uint64_t val;
  __asm__ volatile (
    "mrs %[val], cntfrq_el0"
    : [val] "=&r" (val)
  );
  *frequency = val;

#ifdef ARM_GENERIC_TIMER_USE_VIRTUAL
  *irq = BSP_TIMER_VIRT_PPI;
#elif defined(AARCH64_GENERIC_TIMER_USE_PHYSICAL_SECURE)
  *irq = BSP_TIMER_PHYS_S_PPI;
#else
  *irq = BSP_TIMER_PHYS_NS_PPI;
#endif
}

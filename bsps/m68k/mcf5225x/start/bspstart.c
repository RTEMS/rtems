/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  This routine does the bulk of the system initialisation.
 */

/*
 *  Author:
 *    David Fiddes, D.J@fiddes.surfaid.org
 *    http://www.calm.hw.ac.uk/davidf/coldfire/
 *
 *  COPYRIGHT (c) 1989-1998.
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

#include <bsp.h>
#include <bsp/bootcard.h>

void __attribute__((weak)) bsp_start(void)
{
}

uint32_t  __attribute__((weak)) bsp_get_CPU_clock_speed(void)
{
  #define DEF_CLOCK_SPEED 8000000.0F //8.0 MHz
  #define MCF_MFD0_2_MASK 0x7000U
  #define MCF_RFD0_2_MASK 0x0700U
  #define MCF_MFD0_2_OFFSET 4U

  #define SPEED_BIAS  ((((MCF_CLOCK_SYNCR & MCF_MFD0_2_MASK) >> 11) + MCF_MFD0_2_OFFSET) / (float)(((MCF_CLOCK_SYNCR & MCF_RFD0_2_MASK)>>7) ? : 1.0F))

  return MCF_CLOCK_SYNCR & MCF_CLOCK_SYNCR_PLLEN ? SPEED_BIAS * DEF_CLOCK_SPEED : DEF_CLOCK_SPEED;
}

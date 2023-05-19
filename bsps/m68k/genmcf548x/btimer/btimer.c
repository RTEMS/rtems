/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * RTEMS generic mcf548x BSP
 *
 * The file contains the diagnostic timer code of generic MCF548x
 * BSP.
 *
 * Parts of the code has been derived from the "dBUG source code"
 * package Freescale is providing for M548X EVBs. The usage of
 * the modified or unmodified code and it's integration into the
 * generic mcf548x BSP has been done according to the Freescale
 * license terms.
 *
 * The Freescale license terms can be reviewed in the file
 *
 *    LICENSE.Freescale
 *
 * The generic mcf548x BSP has been developed on the basic
 * structures and modules of the av5282 BSP.
 */

/*
 * Copyright (c) 2007 embedded brains GmbH & Co. KG
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

/*
 *  Timer Init
 *
 *  Use the second slice timer (SLT1) as the diagnostic timer.
 */

#include <rtems.h>
#include <rtems/btimer.h>
#include <bsp.h>
#include <mcf548x/mcf548x.h>

bool benchmark_timer_find_average_overhead;
static uint32_t microsecond_timer_value = 0;

void benchmark_timer_initialize(void)
{
    MCF548X_SLT_SLTCNT1 = 0xFFFFFFFF;
    MCF548X_SLT_SCR1 |= (MCF548X_SLT_SCR_TEN | MCF548X_SLT_SCR_RUN);
}

/*
 * Return timer value in microsecond units
 * XLB clock 100 MHz / 1us is equal to 100 counts
 */
uint32_t
benchmark_timer_read(void)
{
    microsecond_timer_value = (0xFFFFFFFF - MCF548X_SLT_SCNT1)/100;

    return microsecond_timer_value;
}

void
benchmark_timer_disable_subtracting_average_overhead(bool find_flag)
{
    benchmark_timer_find_average_overhead = find_flag;
}

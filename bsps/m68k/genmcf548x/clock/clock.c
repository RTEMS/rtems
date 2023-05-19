/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * RTEMS generic mcf548x BSP
 *
 * The file contains the clock driver code of generic MCF548x BSP. |
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
 * Copyright (c) 2008 embedded brains GmbH & Co. KG
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
 * Use first slice timer (SLT0) as the system clock.
 *
 */

#include <rtems.h>
#include <bsp.h>
#include <bsp/irq-generic.h>
#include <mcf548x/mcf548x.h>

/*
 * Use SLT 0
 */
#define CLOCK_IRQ MCF548X_IRQ_SLT0

/*
 * Periodic interval timer interrupt handler
 */
#define Clock_driver_support_at_tick()             \
    do {                                           \
        MCF548X_SLT_SSR0 = MCF548X_SLT_SSR_ST;     \
    } while (0)                                    \

/*
 * Attach clock interrupt handler
 */
#define Clock_driver_support_install_isr( _new ) \
    set_vector(_new, CLOCK_IRQ + 64, 1)

/*
 * Set up the clock hardware
 *
 * We need to have 1 interrupt every 10,000 microseconds
 * XLB clock 100 MHz / MCF548X_SLT_SLTCNT0 = XLB clock/100
 */
#define Clock_driver_support_initialize_hardware()			\
  do {									\
    bsp_interrupt_vector_enable(CLOCK_IRQ);				\
    MCF548X_SLT_SLTCNT0 = get_CPU_clock_speed()				\
      / 1000								\
      * rtems_configuration_get_microseconds_per_tick()			\
      / 1000;								\
    MCF548X_SLT_SCR0 |= (MCF548X_SLT_SCR_TEN | MCF548X_SLT_SCR_RUN | MCF548X_SLT_SCR_IEN); \
  } while (0)

#define CLOCK_DRIVER_USE_DUMMY_TIMECOUNTER

#include "../../../shared/dev/clock/clockimpl.h"


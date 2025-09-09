/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup RTEMSBSPsM68kAV5282
 *
 * @brief Use the last periodic interval timer (PIT3) as the system clock.
 */

/*
 * Copyright (C) 2005 Eric Norum <eric@norum.ca>
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

#include <rtems.h>
#include <bsp.h>
#include <mcf5282/mcf5282.h>

/*
 * Use INTC0 base
 */
#define CLOCK_VECTOR (64+58)

/*
 * Periodic interval timer interrupt handler
 */
#define Clock_driver_support_at_tick(arg)          \
    do {                                           \
        MCF5282_PIT3_PCSR |= MCF5282_PIT_PCSR_PIF; \
    } while (0)                                    \

/*
 * Attach clock interrupt handler
 */
#define Clock_driver_support_install_isr( _new ) \
    rtems_interrupt_handler_install( \
        CLOCK_VECTOR, \
        "Install clock interrupt", \
        RTEMS_INTERRUPT_UNIQUE, \
        (void *)_new, \
        NULL \
    );

/*
 * Set up the clock hardware
 *
 * We need to have 1 interrupt every 10,000 microseconds
 * so we need to set prescaler to 64 and the PMR register to 0x23FE
 */
#define Clock_driver_support_initialize_hardware()                       \
    do {                                                                 \
        int level;                                                       \
        int preScaleCode = 5;                                             \
        MCF5282_INTC0_ICR58 = MCF5282_INTC_ICR_IL(PIT3_IRQ_LEVEL) |      \
                              MCF5282_INTC_ICR_IP(PIT3_IRQ_PRIORITY);    \
        rtems_interrupt_disable( level );                                \
        MCF5282_INTC0_IMRH &= ~MCF5282_INTC_IMRH_INT58;                  \
		MCF5282_PIT3_PCSR &= ~MCF5282_PIT_PCSR_EN;                       \
        rtems_interrupt_enable( level );                                 \
		MCF5282_PIT3_PMR = 0x23FE;  \
		MCF5282_PIT3_PCSR = MCF5282_PIT_PCSR_PRE(preScaleCode) |         \
                            MCF5282_PIT_PCSR_PIE |                       \
                            MCF5282_PIT_PCSR_RLD |                       \
                            MCF5282_PIT_PCSR_EN;                         \
    } while (0)

#define CLOCK_DRIVER_USE_DUMMY_TIMECOUNTER

#include "../../../shared/dev/clock/clockimpl.h"

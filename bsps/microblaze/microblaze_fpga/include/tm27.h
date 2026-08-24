/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsMicroblaze
 *
 * @brief BSP tm27 header
 */

/*
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
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

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
#endif

#ifndef __tm27_h
#define __tm27_h

#include <bsp.h>
#include <bsp/intc.h>
#include <bsp/irq.h>
#include <bsp/microblaze-fdt-support.h>
#include <bsp/microblaze-timer.h>

#include <dev/serial/uartlite_l.h>

#include <rtems/score/isr.h>

/*
 * A write to the interrupt status register of the AXI interrupt controller
 * raises an interrupt in software.  The controller accepts this write only
 * while the hardware interrupt enable of the master enable register is clear.
 * This enable is write once on some implementations of the controller.
 * Install_tm27_vector() probes the controller.  Where the probe fails,
 * Cause_tm27_intr() uses the second counter of the AXI timer.  That counter
 * shares the interrupt of the counter used by the clock, so each handler of
 * the vector checks its own interrupt bit.  The counter raises the interrupt
 * some ticks after the request.  A test which needs the interrupt in a narrow
 * window fails with this delay.
 */

#define MUST_WAIT_FOR_INTERRUPT 1

/* Number of counter ticks until the interrupt is raised */
#define MICROBLAZE_TM27_TICKS 10

static rtems_interrupt_entry microblaze_tm27_interrupt_entry;

static volatile Microblaze_Timer *microblaze_tm27_timer;

static volatile Microblaze_INTC *microblaze_tm27_intc;

static uint32_t microblaze_tm27_irq;

static bool microblaze_tm27_software_raise;

static uint32_t microblaze_tm27_uart;

static uint32_t microblaze_tm27_uart_irq;

/*
 * The test needs a second interrupt which it can raise and which no handler
 * occupies.  The AXI UART Lite is the only other device of this design.  The
 * console driver of this BSP polls, so nothing else uses the interrupt of the
 * device.
 */
#define TM27_INTERRUPT_VECTOR_ALTERNATIVE microblaze_tm27_uart_irq

static inline void Install_tm27_vector( rtems_interrupt_handler handler )
{
  uint32_t irq;
  uint32_t mask;

  microblaze_tm27_intc = (volatile Microblaze_INTC *)
    try_get_prop_from_device_tree(
      "xlnx,xps-intc-1.00.a",
      "reg",
      BSP_MICROBLAZE_FPGA_INTC_BASE
    );
  microblaze_tm27_timer = (volatile Microblaze_Timer *)
    try_get_prop_from_device_tree(
      "xlnx,xps-timer-1.00.a",
      "reg",
      BSP_MICROBLAZE_FPGA_TIMER_BASE
    );
  irq = try_get_prop_from_device_tree(
    "xlnx,xps-timer-1.00.a",
    "interrupts",
    0
  );
  microblaze_tm27_irq = irq;

  microblaze_tm27_uart = try_get_prop_from_device_tree(
    "xlnx,xps-uartlite-1.00.a",
    "reg",
    BSP_MICROBLAZE_FPGA_UART_BASE
  );
  microblaze_tm27_uart_irq = try_get_prop_from_device_tree(
    "xlnx,xps-uartlite-1.00.a",
    "interrupts",
    BSP_MICROBLAZE_FPGA_UART_IRQ
  );
  XUartLite_DisableIntr( microblaze_tm27_uart );

  /* Stop the counter and clear a pending interrupt */
  microblaze_tm27_timer->tcsr1 = MICROBLAZE_TIMER_TCSR0_T0INT;

  /*
   * The processor takes no interrupt during the system initialization, so the
   * probe raises and clears the interrupt without a handler call.
   */
  mask = UINT32_C( 1 ) << irq;
  microblaze_tm27_intc->mer = MICROBLAZE_INTC_MER_ME;
  microblaze_tm27_intc->isr = mask;
  microblaze_tm27_software_raise = ( microblaze_tm27_intc->isr & mask ) != 0;
  microblaze_tm27_intc->iar = mask;
  microblaze_tm27_intc->mer = MICROBLAZE_INTC_MER_ME |
    MICROBLAZE_INTC_MER_HIE;

  rtems_interrupt_entry_initialize(
    &microblaze_tm27_interrupt_entry,
    handler,
    NULL,
    "tm27"
  );
  (void) rtems_interrupt_entry_install(
    irq,
    RTEMS_INTERRUPT_SHARED,
    &microblaze_tm27_interrupt_entry
  );
}

static inline void Cause_tm27_intr( void )
{
  if ( microblaze_tm27_software_raise ) {
    microblaze_tm27_intc->mer = MICROBLAZE_INTC_MER_ME;
    microblaze_tm27_intc->isr = UINT32_C( 1 ) << microblaze_tm27_irq;
    microblaze_tm27_intc->mer = MICROBLAZE_INTC_MER_ME |
      MICROBLAZE_INTC_MER_HIE;
  } else {
    microblaze_tm27_timer->tlr1 = MICROBLAZE_TM27_TICKS;
    microblaze_tm27_timer->tcsr1 = MICROBLAZE_TIMER_TCSR0_LOAD0;
    microblaze_tm27_timer->tcsr1 = MICROBLAZE_TIMER_TCSR0_ENIT0 |
      MICROBLAZE_TIMER_TCSR0_ENT0 | MICROBLAZE_TIMER_TCSR0_UDT0;
  }
}

static inline void Clear_tm27_intr( void )
{
  microblaze_tm27_timer->tcsr1 = MICROBLAZE_TIMER_TCSR0_T0INT;
  microblaze_tm27_intc->iar = UINT32_C( 1 ) << microblaze_tm27_irq;
}

static inline void Lower_tm27_intr( void )
{
  /*
   * The dispatch acknowledges the interrupt controller only after the handler
   * returned.  Acknowledge here so that the interrupt raised next is delivered
   * as a nested interrupt.  The acknowledge of the dispatch is then a no-op.
   */
  microblaze_tm27_intc->iar = UINT32_C( 1 ) << microblaze_tm27_irq;
  _ISR_Set_level( 0 );
}

/*
 * The interrupt controller latches the request of the UART on the rising edge
 * of its input.  A pulse of the interrupt enable of the device gives exactly
 * one request.
 */
static inline rtems_status_code _TM27_Raise_alternative( void )
{
  XUartLite_EnableIntr( microblaze_tm27_uart );
  XUartLite_DisableIntr( microblaze_tm27_uart );
  return RTEMS_SUCCESSFUL;
}

static inline rtems_status_code _TM27_Clear_alternative( void )
{
  XUartLite_DisableIntr( microblaze_tm27_uart );
  microblaze_tm27_intc->iar = UINT32_C( 1 ) << microblaze_tm27_uart_irq;
  return RTEMS_SUCCESSFUL;
}

#endif /* __tm27_h */

/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsOR1K
 *
 * @brief This header file includes the generic tm27 support implementation.
 */

/*
 * Copyright (C) 2017 embedded brains GmbH & Co. KG
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
#include <bsp/generic_or1k.h>
#include <bsp/irq.h>

#include <rtems/score/cpu.h>
#include <rtems/score/isr.h>
#include <rtems/score/or1k-utility.h>

#define MUST_WAIT_FOR_INTERRUPT 1

/*
 * The interrupt of the UART is the one which bsp_interrupt_raise() raises.
 */
#define TM27_INTERRUPT_VECTOR_DEFAULT 2

/*
 * The test needs a vector which it can raise and which no handler occupies.
 * The alarm of the real time clock is the only interrupt left which software
 * can raise.
 */
#define TM27_INTERRUPT_VECTOR_ALTERNATIVE OR1K_BSP_RTC_IRQ

static rtems_interrupt_entry generic_or1k_tm27_interrupt_entry;

static inline void Clear_tm27_intr( void )
{
  (void) rtems_interrupt_clear( TM27_INTERRUPT_VECTOR_DEFAULT );
}

static inline void Install_tm27_vector( rtems_interrupt_handler handler )
{
  Clear_tm27_intr();
  rtems_interrupt_entry_initialize(
    &generic_or1k_tm27_interrupt_entry,
    handler,
    NULL,
    "tm27"
  );
  (void) rtems_interrupt_entry_install(
    TM27_INTERRUPT_VECTOR_DEFAULT,
    RTEMS_INTERRUPT_SHARED,
    &generic_or1k_tm27_interrupt_entry
  );
}

static inline void Cause_tm27_intr( void )
{
  (void) rtems_interrupt_raise( TM27_INTERRUPT_VECTOR_DEFAULT );
}

static inline void Lower_tm27_intr( void )
{
  _ISR_Set_level( 0 );
}

/*
 * An alarm time which already passed raises the interrupt of the real time
 * clock at once.
 */
static inline rtems_status_code _TM27_Raise_alternative( void )
{
  /*
   * The input of the interrupt controller is level triggered, so the interrupt
   * has to be taken back first to get a transition out of the device.
   */
  OR1K_BSP_RTC_WRITE( OR1K_BSP_RTC_REG_CLEAR_INTERRUPT, 1 );
  OR1K_BSP_RTC_WRITE( OR1K_BSP_RTC_REG_IRQ_ENABLED, 1 );
  OR1K_BSP_RTC_WRITE( OR1K_BSP_RTC_REG_ALARM_HIGH, 0 );
  OR1K_BSP_RTC_WRITE( OR1K_BSP_RTC_REG_ALARM_LOW, 0 );
  return RTEMS_SUCCESSFUL;
}

static inline rtems_status_code _TM27_Clear_alternative( void )
{
  OR1K_BSP_RTC_WRITE( OR1K_BSP_RTC_REG_CLEAR_ALARM, 1 );
  OR1K_BSP_RTC_WRITE( OR1K_BSP_RTC_REG_CLEAR_INTERRUPT, 1 );
  return RTEMS_SUCCESSFUL;
}

#endif /* __tm27_h */

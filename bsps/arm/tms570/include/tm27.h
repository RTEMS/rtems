/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This header file provides a TM27 support implementation.
 */

/*
 * Copyright (c) 2023 embedded brains GmbH & Co. KG
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

#include <rtems.h>

#include <bsp/irq.h>
#include <bsp/tms570.h>
#include <rtems/score/armv4.h>

#define MUST_WAIT_FOR_INTERRUPT 1

#define TM27_INTERRUPT_VECTOR_DEFAULT TMS570_IRQ_TIMER_3

#define TM27_INTERRUPT_VECTOR_ALTERNATIVE TMS570_IRQ_TIMER_1

static inline void Install_tm27_vector( rtems_interrupt_handler handler )
{
  static rtems_interrupt_entry entry_2;
  static rtems_interrupt_entry entry_3;

  TMS570_RTI.CNT[1].CPUCx = 1;
  TMS570_RTI.CNT[1].UCx = 0;
  TMS570_RTI.CNT[1].FRCx = 0;
  TMS570_RTI.CMP[1].COMPx = 1;
  TMS570_RTI.CMP[1].UDCPx = 1;
  TMS570_RTI.CMP[2].COMPx = 1;
  TMS570_RTI.CMP[2].UDCPx = 1;
  TMS570_RTI.CMP[3].COMPx = 1;
  TMS570_RTI.CMP[3].UDCPx = 1;
  TMS570_RTI.COMPCTRL |= TMS570_RTI_COMPCTRL_COMPSEL1 |
    TMS570_RTI_COMPCTRL_COMPSEL2 |
    TMS570_RTI_COMPCTRL_COMPSEL3;
  TMS570_RTI.GCTRL |= TMS570_RTI_GCTRL_CNT1EN;

  rtems_interrupt_entry_initialize(
    &entry_2,
    handler,
    NULL,
    "tm27"
  );
  (void) rtems_interrupt_entry_install(
    TMS570_IRQ_TIMER_2,
    RTEMS_INTERRUPT_SHARED,
    &entry_2
  );

  rtems_interrupt_entry_initialize(
    &entry_3,
    handler,
    NULL,
    "tm27"
  );
  (void) rtems_interrupt_entry_install(
    TMS570_IRQ_TIMER_3,
    RTEMS_INTERRUPT_SHARED,
    &entry_3
  );
}

static inline void Cause_tm27_intr(void)
{
  TMS570_RTI.SETINTENA = TMS570_RTI_SETINTENA_SETINT3;
}

static inline void Clear_tm27_intr(void)
{
  TMS570_RTI.CLEARINTENA = TMS570_RTI_CLEARINTENA_CLEARINT2 |
    TMS570_RTI_CLEARINTENA_CLEARINT3;
}

static inline void Lower_tm27_intr(void)
{
  TMS570_RTI.SETINTENA = TMS570_RTI_SETINTENA_SETINT2;
  (void) _ARMV4_Status_irq_enable();
}

static inline rtems_status_code _TM27_Raise_alternative(void)
{
  TMS570_RTI.SETINTENA = TMS570_RTI_SETINTENA_SETINT1;
  return RTEMS_SUCCESSFUL;
}

static inline rtems_status_code _TM27_Clear_alternative(void)
{
  TMS570_RTI.CLEARINTENA = TMS570_RTI_CLEARINTENA_CLEARINT1;
  return RTEMS_SUCCESSFUL;
}

#endif /* __tm27_h */

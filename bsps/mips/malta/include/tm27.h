/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsMIPSMalta
 *
 * @brief This header file provides the tm27 test support.
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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

#include <bsp/irq.h>

#include <rtems/mips/idtcpu.h>
#include <rtems/score/isr.h>
#include <rtems/score/mips.h>

#define MUST_WAIT_FOR_INTERRUPT 1

/*
 * The two software interrupts of the processor are the only sources this BSP
 * can raise.  The first carries the test interrupt and the second is the
 * alternative vector, so a test which needs two of them has one of each.
 */
#define TM27_INTERRUPT_VECTOR_DEFAULT     MALTA_CPU_INT_SW0
#define TM27_INTERRUPT_VECTOR_ALTERNATIVE MALTA_CPU_INT_SW1

static rtems_interrupt_entry malta_tm27_interrupt_entry;

static inline void Install_tm27_vector( rtems_interrupt_handler handler )
{
  rtems_interrupt_entry_initialize(
    &malta_tm27_interrupt_entry,
    handler,
    NULL,
    "tm27"
  );
  (void) rtems_interrupt_entry_install(
    TM27_INTERRUPT_VECTOR_DEFAULT,
    RTEMS_INTERRUPT_SHARED,
    &malta_tm27_interrupt_entry
  );
}

/*
 * A software interrupt of the processor is a bit of the cause register.  It
 * is pending from the write on.  It arrives as soon as the interrupts are
 * enabled.  It stays pending until it is cleared.
 */
static inline void Cause_tm27_intr( void )
{
  (void) rtems_interrupt_raise( TM27_INTERRUPT_VECTOR_DEFAULT );
}

static inline void Clear_tm27_intr( void )
{
  (void) rtems_interrupt_clear( TM27_INTERRUPT_VECTOR_DEFAULT );
}

static inline void Lower_tm27_intr( void )
{
  uint32_t sr;

  /*
   * The entry of an exception sets the exception level, which holds off every
   * interrupt of the processor while the handler runs.  The interrupt enable
   * alone does not undo that.  Clear the exception level, so that the
   * interrupt raised next arrives as a nested interrupt.  The frame of the
   * handler holds the exception program counter and the exit restores it.  A
   * nested exception which overwrites the register therefore does no harm.
   */
  mips_get_sr( sr );
  sr &= ~SR_EXL;
  mips_set_sr( sr );

  _ISR_Set_level( 0 );
}

static inline rtems_status_code _TM27_Raise_alternative( void )
{
  return rtems_interrupt_raise( TM27_INTERRUPT_VECTOR_ALTERNATIVE );
}

static inline rtems_status_code _TM27_Clear_alternative( void )
{
  return rtems_interrupt_clear( TM27_INTERRUPT_VECTOR_ALTERNATIVE );
}

#endif /* __tm27_h */

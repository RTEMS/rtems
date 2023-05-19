/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsRISCVGeneric
 *
 * @brief This header file contains the generic RISC-V tm27 support
 *   implementation.
 */

/*
 * Copyright (C) 2022 embedded brains GmbH & Co. KG
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

#include <bsp/irq-generic.h>
#include <rtems/score/assert.h>
#include <rtems/score/riscv-utility.h>
#include <rtems/score/percpu.h>

#define MUST_WAIT_FOR_INTERRUPT 1

static bool riscv_tm27_can_use_mtime;

static rtems_interrupt_entry riscv_tm27_interrupt_entry;

static inline void Install_tm27_vector( rtems_interrupt_handler handler )
{
  rtems_vector_number irq;
  bool                enabled;

  irq = RISCV_INTERRUPT_VECTOR_TIMER;
  enabled = false;
  rtems_interrupt_vector_is_enabled( irq, &enabled );

  if ( enabled ) {
    irq = RISCV_INTERRUPT_VECTOR_SOFTWARE;
  } else {
    riscv_tm27_can_use_mtime = true;
  }

  rtems_interrupt_entry_initialize(
    &riscv_tm27_interrupt_entry,
    handler,
    NULL,
    "tm27"
  );

  (void) rtems_interrupt_entry_install(
    irq,
    RTEMS_INTERRUPT_SHARED,
    &riscv_tm27_interrupt_entry
  );
}

static inline void Cause_tm27_intr( void )
{
  if ( riscv_tm27_can_use_mtime ) {
    rtems_interrupt_level level;
    Per_CPU_Control      *cpu_self;

    rtems_interrupt_local_disable( level );
    cpu_self = _Per_CPU_Get();
    cpu_self->cpu_per_cpu.clint_mtimecmp->val_64 = 0;
    rtems_interrupt_local_enable( level );
  } else {
    (void) rtems_interrupt_raise( RISCV_INTERRUPT_VECTOR_SOFTWARE );
  }
}

static inline void Clear_tm27_intr( void )
{
  if ( riscv_tm27_can_use_mtime ) {
    rtems_interrupt_level level;
    Per_CPU_Control      *cpu_self;

    rtems_interrupt_local_disable( level );
    cpu_self = _Per_CPU_Get();
    cpu_self->cpu_per_cpu.clint_mtimecmp->val_64 = UINT64_MAX;
    rtems_interrupt_local_enable( level );
  } else {
    (void) rtems_interrupt_clear( RISCV_INTERRUPT_VECTOR_SOFTWARE );
  }
}

static inline void Lower_tm27_intr( void )
{
  rtems_vector_number irq;

  /*
   * This is an ugly hack just to for tm27.  The support for nested interrupts
   * is currently quite bad on RISC-V.
   */
  irq = RISCV_INTERRUPT_VECTOR_SOFTWARE;

  if ( bsp_interrupt_dispatch_table[ irq ] == NULL ) {
    _Assert( riscv_tm27_can_use_mtime );
    bsp_interrupt_dispatch_table[ irq ] = &riscv_tm27_interrupt_entry;
    (void) rtems_interrupt_vector_enable( irq );
  }

  _ISR_Set_level( 0 );
  (void) rtems_interrupt_raise( irq );
}

#endif

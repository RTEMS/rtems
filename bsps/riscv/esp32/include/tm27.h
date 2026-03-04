/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsRISCVESP32
 *
 * @brief This header file contains the ESP32 tm27 support implementation.
 */

/*
 * Copyright (C) 2026 Kinsey Moore <wkmoore@gmail.com>
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

static rtems_interrupt_entry riscv_tm27_interrupt_entry;

static inline void Install_tm27_vector( rtems_interrupt_handler handler )
{
  rtems_interrupt_entry_initialize(
    &riscv_tm27_interrupt_entry,
    handler,
    NULL,
    "tm27"
  );

  (void) rtems_interrupt_entry_install(
    RISCV_INTERRUPT_VECTOR_SOFTWARE,
    RTEMS_INTERRUPT_SHARED,
    &riscv_tm27_interrupt_entry
  );
}

static inline void Cause_tm27_intr( void )
{
  (void) rtems_interrupt_raise( RISCV_INTERRUPT_VECTOR_SOFTWARE );
}

static inline void Clear_tm27_intr( void )
{
  (void) rtems_interrupt_clear( RISCV_INTERRUPT_VECTOR_SOFTWARE );
}

static inline void Lower_tm27_intr( void )
{
  _ISR_Set_level( 0 );
  (void) rtems_interrupt_raise( RISCV_INTERRUPT_VECTOR_SOFTWARE );
}

#endif

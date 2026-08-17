/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicIntr
 *
 * @brief Interrupt support.
 */

/*
 * Copyright (C) 2005 by Cogent Computer Systems
 * Written by Jay Monkman <jtm@lopingdog.com>
 *
 * Copyright (C) 1989-2012 On-Line Applications Research Corporation (OAR)
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
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <libcpu/isr_entries.h>
#include <rtems/fatal.h>
#include <rtems/score/interr.h>

void bsp_interrupt_facility_initialize(void)
{
  uint32_t sr;

  mips_install_isr_entries();

  /*
   * The interrupt mask of the status register is global state of the
   * interrupt controller and no longer part of the context of a thread, so it
   * is established once here.  Every source of the processor starts enabled,
   * including the two software interrupts, which mips_interrupt_mask() leaves
   * out.  Which of them reaches the processor is up to the controller.
   */
  mips_get_sr( sr );
  sr |= mips_interrupt_mask() | SR_IBIT1 | SR_IBIT2;
  mips_set_sr( sr );
}

void bsp_interrupt_handler_default(rtems_vector_number vector)
{
  /*
   * An exception which no handler consumed is reported with the source and
   * the argument every other port uses, so that a fatal error extension can
   * recognise it and, where the exception is expected, resume with a
   * longjmp().  Do not report it here: the extension prints the frame if the
   * system really terminates.
   */
  if ( mips_exception_frame != NULL ) {
    rtems_fatal(
      RTEMS_FATAL_SOURCE_EXCEPTION,
      (rtems_fatal_code) mips_exception_frame
    );
  }

  _Terminate( RTEMS_FATAL_SOURCE_SPURIOUS_INTERRUPT, vector );
}

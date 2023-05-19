/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2013, 2021 embedded brains GmbH & Co. KG
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

#include <bsp/irq-generic.h>

#include <mcf548x/mcf548x.h>

void asm_default_interrupt(void);

typedef void (*void_func)(void);

static uint32_t vector_to_reg(rtems_vector_number vector)
{
  return ((vector + 32U) >> 5) & 0x1;
}

static uint32_t vector_to_bit(rtems_vector_number vector)
{
  return 1U << (vector & 0x1fU);
}

static volatile uint32_t *vector_to_imr(rtems_vector_number vector)
{
  volatile uint32_t *imr = &MCF548X_INTC_IMRH;

  return &imr[vector_to_reg(vector)];
}

static rtems_vector_number exception_vector_to_vector(
  rtems_vector_number exception_vector
)
{
  return exception_vector - 64U;
}

static rtems_vector_number vector_to_exception_vector(
  rtems_vector_number vector
)
{
  return vector + 64U;
}

rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(pending != NULL);
  *pending = false;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_raise(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_clear(rtems_vector_number vector)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_is_enabled(
  rtems_vector_number vector,
  bool               *enabled
)
{
  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));
  bsp_interrupt_assert(enabled != NULL);
  *enabled = false;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  volatile uint32_t *imr = vector_to_imr(vector);
  uint32_t bit = vector_to_bit(vector);
  rtems_interrupt_level level;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  rtems_interrupt_disable(level);
  *imr &= ~bit;
  rtems_interrupt_enable(level);

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  volatile uint32_t *imr = vector_to_imr(vector);
  uint32_t bit = vector_to_bit(vector);
  rtems_interrupt_level level;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  rtems_interrupt_disable(level);
  *imr |= bit;
  rtems_interrupt_enable(level);

  return RTEMS_SUCCESSFUL;
}

static void set_exception_handler(rtems_vector_number vector, void_func handler)
{
  void **vbr;
  void_func *exception_table;

  m68k_get_vbr(vbr);

  exception_table = (void_func *)vbr;

  exception_table[vector_to_exception_vector(vector)] = handler;
}

static void dispatch_handler(rtems_vector_number exception_vector)
{
  bsp_interrupt_handler_dispatch_unchecked(
    exception_vector_to_vector(exception_vector)
  );
}

void mcf548x_interrupt_vector_install(rtems_vector_number vector)
{
  _ISR_Vector_table[vector_to_exception_vector(vector)]
    = dispatch_handler;
  set_exception_handler(vector, _ISR_Handler);
}

void mcf548x_interrupt_vector_remove(rtems_vector_number vector)
{
  set_exception_handler(vector, asm_default_interrupt);
}

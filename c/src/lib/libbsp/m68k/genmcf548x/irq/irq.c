/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp/irq-generic.h>

#include <mcf548x/mcf548x.h>

void asm_default_interrupt(void);

typedef void (*void_func)(void);

typedef struct {
  rtems_interrupt_handler handler;
  void *arg;
  const char *info;
} interrupt_control;

static interrupt_control interrupt_controls[BSP_INTERRUPT_VECTOR_MAX + 1];

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

void bsp_interrupt_vector_enable(rtems_vector_number vector)
{
  volatile uint32_t *imr = vector_to_imr(vector);
  uint32_t bit = vector_to_bit(vector);
  rtems_interrupt_level level;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  rtems_interrupt_disable(level);
  *imr &= ~bit;
  rtems_interrupt_enable(level);
}

void bsp_interrupt_vector_disable(rtems_vector_number vector)
{
  volatile uint32_t *imr = vector_to_imr(vector);
  uint32_t bit = vector_to_bit(vector);
  rtems_interrupt_level level;

  bsp_interrupt_assert(bsp_interrupt_is_valid_vector(vector));

  rtems_interrupt_disable(level);
  *imr |= bit;
  rtems_interrupt_enable(level);
}

static void_func get_exception_handler(rtems_vector_number vector)
{
  void **vbr;
  void_func *exception_table;

  m68k_get_vbr(vbr);

  exception_table = (void_func *)vbr;

  return exception_table[vector_to_exception_vector(vector)];
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
  const interrupt_control *ic =
    &interrupt_controls[exception_vector_to_vector(exception_vector)];

  (*ic->handler)(ic->arg);
}

static uint8_t get_intc_icr(rtems_vector_number vector)
{
  volatile uint8_t *icr = &MCF548X_INTC_ICR0;

  return icr[vector];
}

rtems_status_code rtems_interrupt_handler_install(
  rtems_vector_number vector,
  const char *info,
  rtems_option options,
  rtems_interrupt_handler handler,
  void *arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (bsp_interrupt_is_valid_vector(vector)) {
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);

    if (
      get_exception_handler(vector) == asm_default_interrupt
        && get_intc_icr(vector) != 0
    ) {
      interrupt_control *ic = &interrupt_controls[vector];

      ic->handler = handler;
      ic->arg = arg;
      ic->info = info;

      _ISR_Vector_table[vector_to_exception_vector(vector)]
        = dispatch_handler;
      set_exception_handler(vector, _ISR_Handler);
      bsp_interrupt_vector_enable(vector);
    } else {
      sc = RTEMS_RESOURCE_IN_USE;
    }

    rtems_interrupt_enable(level);
  } else {
    sc = RTEMS_INVALID_ID;
  }

  return sc;
}

static bool is_occupied_by_us(rtems_vector_number vector)
{
  return get_exception_handler(vector) == _ISR_Handler
    && _ISR_Vector_table[vector_to_exception_vector(vector)]
      == dispatch_handler;
}

rtems_status_code rtems_interrupt_handler_remove(
  rtems_vector_number vector,
  rtems_interrupt_handler handler,
  void *arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (bsp_interrupt_is_valid_vector(vector)) {
    rtems_interrupt_level level;
    interrupt_control *ic = &interrupt_controls[vector];

    rtems_interrupt_disable(level);

    if (
      is_occupied_by_us(vector)
        && ic->handler == handler
        && ic->arg == arg
    ) {
      bsp_interrupt_vector_disable(vector);
      set_exception_handler(vector, asm_default_interrupt);

      memset(ic, 0, sizeof(*ic));
    } else {
      sc = RTEMS_UNSATISFIED;
    }

    rtems_interrupt_enable(level);
  } else {
    sc = RTEMS_INVALID_ID;
  }

  return sc;
}

rtems_status_code rtems_interrupt_handler_iterate(
  rtems_vector_number vector,
  rtems_interrupt_per_handler_routine routine,
  void *arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (bsp_interrupt_is_valid_vector(vector)) {
    if (is_occupied_by_us(vector)) {
      const interrupt_control *ic = &interrupt_controls[vector];

      (*routine)(arg, ic->info, RTEMS_INTERRUPT_UNIQUE, ic->handler, ic->arg);
    }
  } else {
    sc = RTEMS_INVALID_ID;
  }

  return sc;
}

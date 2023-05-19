/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicIntr
 *
 * @brief This source file contains the implementation of
 *   bsp_interrupt_report() and bsp_interrupt_report_with_plugin().
 */

/*
 * Copyright (C) 2008, 2010 embedded brains GmbH & Co. KG
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

#include <inttypes.h>

#include <rtems/printer.h>

#include <bsp/irq-generic.h>
#include <bsp/irq-info.h>

typedef struct {
  const rtems_printer *printer;
  rtems_vector_number vector;
} bsp_interrupt_report_entry;

static void bsp_interrupt_report_per_handler_routine(
  void *arg,
  const char *info,
  rtems_option options,
  rtems_interrupt_handler handler,
  void *handler_arg
)
{
  bsp_interrupt_report_entry *e = (bsp_interrupt_report_entry *) arg;
  const char *opt = options == RTEMS_INTERRUPT_UNIQUE ? "UNIQUE" : "SHARED";

  rtems_printf(
    e->printer,
    "%7" PRIu32 " | %-32s | %7s | %p | %p\n",
    e->vector,
    info,
    opt,
    handler,
    handler_arg
  );
}

void bsp_interrupt_report_with_plugin(
  const rtems_printer *printer
)
{
  rtems_vector_number v = 0;
  bsp_interrupt_report_entry e = {
    .printer = printer,
    .vector = 0
  };

  rtems_printf(
    printer,
    "-------------------------------------------------------------------------------\n"
    "                             INTERRUPT INFORMATION\n"
    "--------+----------------------------------+---------+------------+------------\n"
    " VECTOR | INFO                             | OPTIONS | HANDLER    | ARGUMENT   \n"
    "--------+----------------------------------+---------+------------+------------\n"
  );

  for (v = 0; v < BSP_INTERRUPT_VECTOR_COUNT; ++v) {
    e.vector = v;
    rtems_interrupt_handler_iterate(
      v,
      bsp_interrupt_report_per_handler_routine,
      &e
    );
  }

  rtems_printf(
    printer,
    "--------+----------------------------------+---------+------------+------------\n"
  );
}

void bsp_interrupt_report(void)
{
  rtems_printer printer;
  rtems_print_printer_printk(&printer);
  bsp_interrupt_report_with_plugin(&printer);
}

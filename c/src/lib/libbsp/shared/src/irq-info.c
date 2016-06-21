/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief Generic BSP interrupt information implementation.
 */

/*
 * Copyright (c) 2008, 2009, 2010
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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

  for (v = BSP_INTERRUPT_VECTOR_MIN; v <= BSP_INTERRUPT_VECTOR_MAX; ++v) {
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

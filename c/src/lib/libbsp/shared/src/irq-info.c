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
 * http://www.rtems.com/license/LICENSE.
 */

#include <inttypes.h>

#include <bsp/irq-generic.h>
#include <bsp/irq-info.h>

typedef struct {
  void *context;
  rtems_printk_plugin_t print;
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

  e->print(
    e->context,
    "%7" PRIu32 " | %-32s | %7s | %010p | %010p\n",
    e->vector,
    info,
    opt,
    handler,
    handler_arg
  );
}

void bsp_interrupt_report_with_plugin(
  void *context,
  rtems_printk_plugin_t print
)
{
  rtems_vector_number v = 0;
  bsp_interrupt_report_entry e = {
    .context = context,
    .print = print,
    .vector = 0
  };

  print(
    context,
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

  print(
    context,
    "--------+----------------------------------+---------+------------+------------\n"
  );
}

void bsp_interrupt_report(void)
{
  bsp_interrupt_report_with_plugin(NULL, printk_plugin);
}

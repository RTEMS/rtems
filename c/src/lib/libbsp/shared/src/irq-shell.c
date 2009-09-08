/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief Generic BSP interrupt shell implementation.
 */

/*
 * Copyright (c) 2009
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

#include <stdio.h>

#include <rtems/shell.h>

#include <bsp/irq-info.h>

static int bsp_interrupt_shell_main(int argc, char **argv)
{
  bsp_interrupt_report_with_plugin(stdout, (rtems_printk_plugin_t) fprintf);

  return 0;
}

struct rtems_shell_cmd_tt bsp_interrupt_shell_command = {
  .name     = "irq",
  .usage   = "Prints interrupt information",
  .topic   = "rtems",
  .command = bsp_interrupt_shell_main,
  .alias   = NULL,
  .next    = NULL
};

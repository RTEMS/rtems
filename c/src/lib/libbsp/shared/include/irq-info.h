/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief Generic BSP interrupt information API.
 */

/*
 * Copyright (c) 2008, 2009
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

#ifndef LIBBSP_SHARED_IRQ_INFO_H
#define LIBBSP_SHARED_IRQ_INFO_H

#include <rtems/shell.h>
#include <rtems/bspIo.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup bsp_interrupt BSP Interrupt Information
 *
 * @ingroup rtems_interrupt_extension
 *
 * @{
 */

/**
 * @brief Prints interrupt information via the printk plugin @a print with the
 * context @a context.
 */
void bsp_interrupt_report_with_plugin(
  void *context,
  rtems_printk_plugin_t print
);

/**
 * @brief Prints interrupt information via the default printk plugin.
 */
void bsp_interrupt_report(void);

/**
 * @brief Shell command entry for interrupt information.
 */
extern struct rtems_shell_cmd_tt bsp_interrupt_shell_command;

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_SHARED_IRQ_INFO_H */

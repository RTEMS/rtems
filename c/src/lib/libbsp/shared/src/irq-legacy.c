/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief Generic BSP interrupt support legacy implementation.
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

#include <string.h>

#define BSP_SHARED_HANDLER_SUPPORT

#include <rtems.h>
#include <rtems/irq.h>

#include <bsp/irq-generic.h>

/**
 * @deprecated Obsolete.
 */
int BSP_get_current_rtems_irq_handler(rtems_irq_connect_data *cd)
{
  memset(cd, 0, sizeof(*cd));

  return 1;
}

/**
 * @deprecated Use rtems_interrupt_handler_install() instead.
 */
int BSP_install_rtems_irq_handler(const rtems_irq_connect_data *cd)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_interrupt_handler_install(
    cd->name,
    "LEGACY INSTALLED",
    RTEMS_INTERRUPT_UNIQUE,
    cd->hdl,
    cd->handle
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return 0;
  }

  if (cd->on != NULL) {
    cd->on(cd);
  }

  return 1;
}

/**
 * @deprecated Use rtems_interrupt_handler_install() instead.
 */
int BSP_install_rtems_shared_irq_handler(const rtems_irq_connect_data *cd)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_interrupt_handler_install(
    cd->name,
    "LEGACY INSTALLED",
    RTEMS_INTERRUPT_SHARED,
    cd->hdl,
    cd->handle
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return 0;
  }

  if (cd->on != NULL) {
    (*cd->on)(cd);
  }

  return 1;
}

/**
 * @deprecated Use rtems_interrupt_handler_remove() instead.
 */
int BSP_remove_rtems_irq_handler(const rtems_irq_connect_data *cd)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (cd->off != NULL) {
    (*cd->off)(cd);
  }

  sc = rtems_interrupt_handler_remove(cd->name, cd->hdl, cd->handle);
  if (sc != RTEMS_SUCCESSFUL) {
    return 0;
  }

  return 1;
}

/**
 * @deprecated Use bsp_interrupt_initialize() instead.
 */
int BSP_rtems_irq_mngt_set(rtems_irq_global_settings *config)
{
  return 0;
}

/**
 * @deprecated Obsolete.
 */
int BSP_rtems_irq_mngt_get(rtems_irq_global_settings **config)
{
  *config = NULL;
  return 0;
}

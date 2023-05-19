/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicIntr
 *
 * @brief This source file contains the legacy interrupt controller support
 *   implementation.
 */

/*
 * Copyright (C) 2008, 2009 embedded brains GmbH & Co. KG
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

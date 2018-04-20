/*
 * Copyright (c) 2013, 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/zynq-uart.h>

void bsp_reset(void)
{
  volatile uint32_t *slcr_unlock = (volatile uint32_t *) 0xf8000008;
  volatile uint32_t *pss_rst_ctrl = (volatile uint32_t *) 0xf8000200;

  zynq_uart_reset_tx_flush(&zynq_uart_instances[BSP_CONSOLE_MINOR]);

  while (true) {
    *slcr_unlock = 0xdf0d;
    *pss_rst_ctrl = 0x1;
  }
}

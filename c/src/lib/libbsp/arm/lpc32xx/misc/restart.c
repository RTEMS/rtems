/**
 * @file
 *
 * @ingroup lpc32xx
 *
 * @brief Restart implementation.
 */

/*
 * Copyright (c) 2010, 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>

#include <libcpu/arm-cp15.h>

#include <bsp.h>

void bsp_restart(void *addr)
{
  LPC32XX_DO_RESTART(addr);
}

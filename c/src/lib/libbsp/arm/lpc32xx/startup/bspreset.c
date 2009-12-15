/**
 * @file
 *
 * @ingroup lpc32xx
 *
 * @brief Reset code.
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

#include <rtems.h>

#include <bsp/bootcard.h>

void bsp_reset( void)
{
  while (true) {
    /* Do nothing */
  }
}

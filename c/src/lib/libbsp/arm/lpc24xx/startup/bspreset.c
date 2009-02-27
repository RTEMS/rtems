/**
 * @file
 *
 * @ingroup lpc24xx
 *
 * @brief Reset code.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#include <bsp/bootcard.h>
#include <bsp/start.h>

void bsp_reset( void)
{
  start();
}

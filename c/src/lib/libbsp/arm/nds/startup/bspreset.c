/*
 * RTEMS for Nintendo DS platform initialization.
 * Copyright (c) 2008 by Matthieu Bucchianeri <mbucchia@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <nds.h>

void bsp_reset(void)
{
  swiSoftReset();
}

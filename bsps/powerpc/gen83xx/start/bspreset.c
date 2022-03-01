/*
 * Copyright (c) 2008 embedded brains GmbH.  All rights reserved.
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>

#include <mpc83xx/mpc83xx.h>

void bsp_reset(void)
{
  mpc83xx_reset();
}

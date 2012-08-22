/**
 * @file
 *
 * @ingroup beagle
 *
 * @brief Restart implementation.
 */

/*
 * Copyright (c) 2012 Claas Ziemke. All rights reserved.
 *
 *  Claas Ziemke
 *  Kernerstrasse 11
 *  70182 Stuttgart
 *  Germany
 *  <claas.ziemke@gmx.net>
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
  BEAGLE_DO_RESTART(addr);
}

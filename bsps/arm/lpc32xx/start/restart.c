/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC32XX
 *
 * @brief Restart implementation.
 */

/*
 * Copyright (c) 2010, 2011 embedded brains GmbH.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>

#include <libcpu/arm-cp15.h>

#include <bsp.h>

void bsp_restart(void *addr)
{
  LPC32XX_DO_RESTART(addr);
}

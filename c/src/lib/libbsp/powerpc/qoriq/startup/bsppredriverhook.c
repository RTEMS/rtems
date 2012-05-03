/**
 * @file
 *
 * @ingroup QorIQ
 *
 * @brief bsp_predriver_hook() implementation.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
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

#include <bsp/bootcard.h>
#include <bsp/intercom.h>

void bsp_predriver_hook(void)
{
  #ifdef RTEMS_MULTIPROCESSING
    qoriq_intercom_init();
  #endif
}

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

#include <bsp/console-generic.h>
#include <bsp/console-esci.h>
#include <bsp/console-linflex.h>

CONSOLE_GENERIC_INFO_TABLE = {
  #ifdef MPC55XX_HAS_ESCI
    CONSOLE_GENERIC_INFO(mpc55xx_esci_devices + 0, &mpc55xx_esci_callbacks, "/dev/ttyS0"),
    CONSOLE_GENERIC_INFO(mpc55xx_esci_devices + 1, &mpc55xx_esci_callbacks, "/dev/ttyS1")
  #endif
  #ifdef MPC55XX_HAS_LINFLEX
    CONSOLE_GENERIC_INFO(mpc55xx_linflex_devices + 0, &mpc55xx_linflex_callbacks, "/dev/ttyS0"),
    CONSOLE_GENERIC_INFO(mpc55xx_linflex_devices + 1, &mpc55xx_linflex_callbacks, "/dev/ttyS1")
  #endif
};

CONSOLE_GENERIC_INFO_COUNT;

CONSOLE_GENERIC_MINOR(0);

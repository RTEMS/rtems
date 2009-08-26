/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief BSP Idle Thread Code
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

#include <bsp.h>

/**
 * @brief Idle thread body.
 */
void *bsp_idle_thread( uintptr_t ignored )
{

  while (1) {
    asm volatile(
      "mfmsr 3;"
      "oris 3,3,4;"
      "sync;"
      "mtmsr 3;"
      "isync;"
      "ori 3,3,0;"
      "ori 3,3,0"
    );
  }
  return 0;
}

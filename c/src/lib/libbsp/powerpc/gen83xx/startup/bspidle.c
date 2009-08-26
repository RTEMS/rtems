/**
 * @file
 *
 * @ingroup mpc83xx
 *
 * @brief Source for BSP Idle Thread
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
 *
 * $Id$
 */

#include <bsp.h>

/**
 * @brief Idle thread body.
 *
 * Replaces the one in c/src/exec/score/src/threadidlebody.c
 * The MSR[POW] bit is set to put the CPU into the low power mode
 * defined in HID0.  HID0 is set during starup in start.S.
 */
void *bsp_idle_thread( uintptr_t ignored )
{

  while (1) {
    asm volatile (
      "mfmsr 3;"
      "oris 3, 3, 4;"
      "sync;"
      "mtmsr 3;"
      "isync;"
      "ori 3, 3, 0;"
      "ori 3, 3, 0"
    );
  }

  return NULL;
}

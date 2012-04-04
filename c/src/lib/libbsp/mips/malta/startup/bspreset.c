/**
 *  @file
 *
 *  This file contains the code necessary to reset the Malta board.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>

void bsp_reset(void);

void bsp_reset(void)
{
  uint32_t *reset;

  reset= (uint32_t *)0x9F000500;
  /*
   * Qemu understands 0x42 to reset simulated machine.
   * We added code to recognize 0xFF to exit simulator.
   *
   * TBD: Qemu PC simulation has option to exit on reset.
   *      find processing of that command line option and
   *      use it to change behaviour of 0x42.
   */
  // *reset = 0x42;
  *reset = 0xFF;
}

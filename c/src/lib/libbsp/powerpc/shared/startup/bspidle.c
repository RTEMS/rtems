/*
 *  Moved to libbsp/powerpc/shared by Joel Sherrill (9 Sept 09).
 *
 *  The MPC860 specific stuff was written by Jay Monkman (jmonkman@frasca.com)
 *
 *  Modified for the MPC8260ADS board by Andy Dachs <a.dachs@sstl.co.uk>
 *  Surrey Satellite Technology Limited, 2001
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>

/**
 * @brief BSP Idle thread body.
 *
 *  The MSR[POW] bit is set to put the CPU into the low power mode
 *  defined in HID0.  HID0 is set during starup in start.S.
 */
void *bsp_idle_thread( uintptr_t ignored )
{
  for( ; ; ) {
    __asm__ volatile(
      "mfmsr 3; oris 3,3,4; sync; mtmsr 3; isync; ori 3,3,0; ori 3,3,0"
    );
  }

  return 0; /* to remove warning */
}

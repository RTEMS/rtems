/*
 *  BSP specific Idle thread
 */

/*
 *  Copyright (c) 2000 Canon Research Centre France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>

void *bsp_idle_thread(uintptr_t ignored)
{
  while(1) {
    __asm__ volatile ("MCR p15,0,r0,c7,c0,4     \n");
  }
  return NULL;
}


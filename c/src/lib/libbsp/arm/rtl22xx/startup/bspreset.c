/*
 *  Copyright (c) 2007 by Ray Xu <rayx.cn@gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <lpc22xx.h>

void bsp_reset(void)
{
#if ON_SKYEYE == 1
  #define SKYEYE_MAGIC_ADDRESS (*(volatile unsigned int *)(0xb0000000))

  SKYEYE_MAGIC_ADDRESS = 0xff;
#else
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);

  #ifdef __thumb__
    int tmp;
    __asm__ volatile (" .code 16            \n" \
                  "ldr %[tmp], =_start  \n" \
                  "bx  %[tmp]           \n" \
                  "nop                  \n" \
                  : [tmp]"=&r" (tmp) );
  #else
    __asm__ volatile ("b _start");
  #endif
  while(1);
#endif
}

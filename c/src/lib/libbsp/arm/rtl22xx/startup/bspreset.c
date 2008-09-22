/*
 *  Copyright (c) 2007 by Ray Xu <rayx.cn@gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>
#include <lpc22xx.h>

void bsp_reset(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);

#ifdef __thumb__
  int tmp;
  asm volatile (" .code 16            \n" \
                "ldr %[tmp], =_start  \n" \
                "bx  %[tmp]           \n" \
                "nop                  \n" \
                : [tmp]"=&r" (tmp) );
#else                   
  asm volatile ("b _start");
#endif   

  while(1);
}

/**
 *  COPYRIGHT (c) 2016.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>

void _CPU_Fatal_halt(uint32_t source, uint32_t error)
{
  __asm__ volatile (
         "cli\n\t"
	 "movl %0,%%eax\n\t"
	 "hlt\n\t"
         : "=r" ((error))
         : "0" ((error))
  );
  #ifdef __GNUC__
    __builtin_unreachable();
  #endif
}

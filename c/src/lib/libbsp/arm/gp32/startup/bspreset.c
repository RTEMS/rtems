/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>

void bsp_reset(void)
{
  rtems_interrupt_level level;
  rtems_interrupt_disable(level);
  /* disable mmu, invalide i-cache and call swi #4 */
  __asm__ volatile(""
    "mrc    p15,0,r0,c1,c0,0  \n"
    "bic    r0,r0,#1          \n"
    "mcr    p15,0,r0,c1,c0,0  \n"
    "nop                      \n"
    "nop                      \n"
    "nop                      \n"
    "nop                      \n"
    "nop                      \n"
    "mov    r0,#0             \n"
    "MCR    p15,0,r0,c7,c5,0  \n"
    "nop                      \n"
    "nop                      \n"
    "nop                      \n"
    "nop                      \n"
    "nop                      \n"
    "swi    #4                "
    :
    :
    : "r0"
  );
  /* we should be back in bios now */
}

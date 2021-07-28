/**
 * @file
 * @ingroup RTEMSBSPsSPARCShared
 * @brief ERC32/LEON2 BSP Fatal_halt handler.
 *
 *  COPYRIGHT (c) 2014.
 *  Aeroflex Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <rtems/score/cpuimpl.h>

void _CPU_Fatal_halt( uint32_t source, CPU_Uint32ptr error )
{
#ifdef BSP_POWER_DOWN_AT_FATAL_HALT
  /* Spin CPU on fatal error exit */
  uint32_t level = sparc_disable_interrupts();

  __asm__ volatile ( "mov  %0, %%g1 " : "=r" (level) : "0" (level) );

  while (1) ; /* loop forever */
#else
  /*
   * Return to debugger, simulator, hypervisor or similar by exiting
   * with an error code. g1=1, g2=FATAL_SOURCE, G3=error-code.
   */
  sparc_syscall_exit(source, error);
#endif
}

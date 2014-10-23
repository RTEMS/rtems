/**
 * @file
 * @ingroup sparc_leon3
 * @brief LEON3 BSP Fatal_halt handler.
 *
 *  COPYRIGHT (c) 2014.
 *  Aeroflex Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <leon.h>

#ifdef BSP_POWER_DOWN_AT_FATAL_HALT

/* Power down LEON CPU on fatal error exit */
void _CPU_Fatal_halt(uint32_t source, uint32_t error)
{
  sparc_disable_interrupts();
  leon3_power_down_loop();
}

#else

/* return to debugger, simulator, hypervisor or similar by exiting
 * with an error code. g1=1, g2=FATAL_SOURCE, G3=error-code.
 */
void _CPU_Fatal_halt(uint32_t source, uint32_t error)
{
  sparc_syscall_exit(source, error);
}

#endif

/*
 *  This set of routines starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before any of these are invoked.
 *
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON3 BSP.
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <leon.h>
#include <bsp/bootcard.h>
#include <rtems/sysinit.h>

#if defined(RTEMS_SMP) || defined(RTEMS_MULTIPROCESSING)
/* Irq used by shared memory driver and for inter-processor interrupts.
 * Can be overridden by being defined in the application.
 */
const unsigned char LEON3_mp_irq __attribute__((weak)) = 14;
#endif

/*
 * Tells us if data cache snooping is available
 */
int CPU_SPARC_HAS_SNOOPING;

/* Index of CPU, in an AMP system CPU-index may be non-zero */
uint32_t LEON3_Cpu_Index = 0;

/*
 * set_snooping
 *
 * Read the cache control register to determine if
 * bus snooping is available and enabled. This is needed for some
 * drivers so that they can select the most efficient copy routines.
 *
 */

static inline int set_snooping(void)
{
  return (leon3_get_cache_control_register() >> 23) & 1;
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */
void bsp_start( void )
{
  CPU_SPARC_HAS_SNOOPING = set_snooping();
}

static void leon3_cpu_index_init(void)
{
  /* Get the LEON3 CPU index, normally 0, but for MP systems we do
   * _not_ assume that this is CPU0. One may run another OS on CPU0
   * and RTEMS on this CPU, and AMP system with mixed operating
   * systems
   */
  LEON3_Cpu_Index = _LEON3_Get_current_processor();
}

RTEMS_SYSINIT_ITEM(
  leon3_cpu_index_init,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_FIRST
);

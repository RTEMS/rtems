/*
 *  By Yang Xi <hiyangxi@gmail.com>.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq-generic.h>
#include <rtems/libcsupport.h>
#include <rtems/libio.h>
#include <pxa255.h>

/* Function prototypes */
void rtems_exception_init_mngt(void);

/*
 *
 * NAME: bsp_start_default - BSP initialization function
 *
 * DESCRIPTION:
 *   This function is called before RTEMS is initialized and used
 *   adjust the kernel's configuration.
 *
 *   This function also configures the CPU's memory protection unit.
 *
 * RESTRICTIONS/LIMITATIONS:
 *   Since RTEMS is not configured, no RTEMS functions can be called.
 *
 */
void bsp_start_default( void )
{
  /* disable interrupts */
  XSCALE_INT_ICMR = 0x0;
  rtems_exception_init_mngt();
  if (bsp_interrupt_initialize() != RTEMS_SUCCESSFUL) {
    _CPU_Fatal_halt(0xe);
  }
} /* bsp_start */

/*
 *  By making this a weak alias for bsp_start_default, a brave soul
 *  can override the actual bsp_start routine used.
 */
void bsp_start (void) __attribute__ ((weak, alias("bsp_start_default")));


void bsp_reset( void )
{
#if ON_SKYEYE == 1
  SKYEYE_MAGIC_ADDRESS = 0xff;
#endif
}

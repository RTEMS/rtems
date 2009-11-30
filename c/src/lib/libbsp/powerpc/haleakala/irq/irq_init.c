/*===============================================================*\
| Project: RTEMS Haleakala BSP                                    |
|  *  by Michael Hamel ADInstruments Ltd 2008                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the irq controller handler                   |
\*===============================================================*/
#include <libcpu/spr.h>
#include <bsp/irq.h>
#include <bsp.h>
#include <bsp/vectors.h>
#include <rtems/bspIo.h>
#include <rtems/powerpc/powerpc.h>


/*
 * default on/off function
 */
static void nop_func(void)
{
}

/*
 * default isOn function
 */
static int not_connected(void)
{
	return 0;
}

static rtems_irq_connect_data      rtemsIrq[BSP_IRQ_NUMBER];
static rtems_irq_global_settings   initial_config;
static rtems_irq_connect_data      defaultIrq = {
  /* name,     hdl   , handle  , on       , off      , isOn */
      0,    nop_func , NULL    , nop_func , nop_func , not_connected
};

static rtems_irq_prio irqPrioTable[BSP_IRQ_NUMBER]={
  /*
   * Processor exceptions handled as interrupts
   */
  0
};

  /*
   * This code assumes the exceptions management setup has already
   * been done. We just need to replace the exceptions that will
   * be handled like interrupt. On mcp750/mpc750 and many PPC processors
   * this means the decrementer exception and the external exception.
   */

void BSP_rtems_irq_mng_init(unsigned cpuId)
{
  int i;

  /*
   * re-init the rtemsIrq table
   */
  for (i = 0; i < BSP_IRQ_NUMBER; i++) {
    rtemsIrq[i]      = defaultIrq;
    rtemsIrq[i].name = i;
  }
  /*
   * Init initial Interrupt management config
   */
  initial_config.irqNb        = BSP_IRQ_NUMBER;
  initial_config.defaultEntry = defaultIrq;
  initial_config.irqHdlTbl    = rtemsIrq;
  initial_config.irqBase      = BSP_LOWEST_OFFSET;
  initial_config.irqPrioTbl   = irqPrioTable;

  if (!BSP_rtems_irq_mngt_set(&initial_config)) {
    /*
     * put something here that will show the failure...
     */
    BSP_panic(
      "Unable to initialize RTEMS interrupt management!!! System locked\n"
    );
  }

  #ifdef TRACE_IRQ_INIT
    printk("RTEMS IRQ management is now operational\n");
  #endif
}

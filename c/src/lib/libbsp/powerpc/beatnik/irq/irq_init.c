/* irq_init.c
 *
 *  This file contains the implementation of rtems initialization
 *  related to interrupt handling.
 *
 *  CopyRight (C) 1999 valette@crf.canon.fr
 *
 * Enhanced by Jay Kulpinski <jskulpin@eng01.gdds.com>
 * to make it valid for MVME2300 Motorola boards.
 *
 * Modified by T. Straumann for the 'beatnik' BSP.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/vectors.h>
#include <rtems/bspIo.h>

#if 0
#include <libcpu/io.h>
#include <libcpu/spr.h>
#include <bsp/pci.h>
#include <bsp/residual.h>
#include <bsp/openpic.h>
#include <bsp/irq.h>
#include <bsp.h>
#include <bsp/motorola.h>
#endif

/*
#define SHOW_ISA_PCI_BRIDGE_SETTINGS
*/

extern unsigned int external_exception_vector_prolog_code_size[];
extern void external_exception_vector_prolog_code(void);
extern unsigned int decrementer_exception_vector_prolog_code_size[];
extern void decrementer_exception_vector_prolog_code(void);

/*
 * default handler
 */
static void nop_func(void *arg){}

static rtems_irq_connect_data     	rtemsIrq[BSP_IRQ_NUMBER];
static rtems_irq_global_settings    initial_config;
static rtems_irq_prio				rtemsPrioTbl[BSP_IRQ_NUMBER];
static rtems_irq_connect_data     	defaultIrq = {
  name:   0,
  hdl:    nop_func,
  handle: 0,
  on:     0,
  off:    0,
  isOn:   0
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
	 * First initialize the Interrupt management hardware
	 */

	/*
	 * Initialize Rtems management interrupt table
	 */

	/*
	 * re-init the rtemsIrq table
	 */
	for (i = BSP_LOWEST_OFFSET; i <= BSP_MAX_OFFSET; i++) {
		rtemsIrq[i]      = defaultIrq;
		rtemsIrq[i].name = i;
		rtemsPrioTbl[i]  = BSP_IRQ_DEFAULT_PRIORITY;
	}

	/*
	 * Init initial Interrupt management config
	 */
	initial_config.irqNb 	= BSP_IRQ_NUMBER;
	initial_config.defaultEntry = defaultIrq;
	initial_config.irqHdlTbl	= rtemsIrq;
	initial_config.irqBase	= BSP_LOWEST_OFFSET;
	initial_config.irqPrioTbl	= rtemsPrioTbl;

	if (!BSP_rtems_irq_mngt_set(&initial_config)) {
		/*
		 * put something here that will show the failure...
		 */
		BSP_panic("Unable to initialize RTEMS interrupt Management!!! System locked\n");
	}

#ifdef TRACE_IRQ_INIT  
	printk("RTEMS IRQ management is now operationnal\n");
#endif
}


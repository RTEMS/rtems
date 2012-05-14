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
 * Till Straumann <strauman@slac.stanford.edu>, 12/20/2001:
 * Use the new interface to openpic_init
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <libcpu/io.h>
#include <libcpu/spr.h>
#include <bsp/pci.h>
#include <bsp/residual.h>
#include <bsp/irq.h>
#include <bsp.h>
#include <bsp/vectors.h>
#include <rtems/bspIo.h>

#define SHOW_ISA_PCI_BRIDGE_SETTINGS 1
#define SCAN_PCI_PRINT               1
#define TRACE_IRQ_INIT               0

typedef struct {
  unsigned char bus;	/* few chance the PCI/ISA bridge is not on first bus but ... */
  unsigned char device;
  unsigned char function;
} pci_isa_bridge_device;

pci_isa_bridge_device* via_82c586 = 0;

extern unsigned int external_exception_vector_prolog_code_size[];
extern void external_exception_vector_prolog_code(void);
extern unsigned int decrementer_exception_vector_prolog_code_size[];
extern void decrementer_exception_vector_prolog_code(void);

static void IRQ_Default_rtems_irq_hdl( rtems_irq_hdl_param ptr ) {}
static void IRQ_Default_rtems_irq_enable (const struct __rtems_irq_connect_data__ *ptr){}
static void IRQ_Default_rtems_irq_disable(const struct __rtems_irq_connect_data__ *ptr){}
static int  IRQ_Default_rtems_irq_is_enabled(const struct __rtems_irq_connect_data__ *ptr){ return 1; }

static rtems_irq_connect_data     	rtemsIrq[BSP_IRQ_NUMBER];
static rtems_irq_global_settings     	initial_config;

static rtems_irq_connect_data     	defaultIrq = {
/*name,	 hdl				handle	on				off				isOn */
  0,	 IRQ_Default_rtems_irq_hdl,	NULL,	IRQ_Default_rtems_irq_enable,	IRQ_Default_rtems_irq_disable,	IRQ_Default_rtems_irq_is_enabled
};

static rtems_irq_prio irqPrioTable[BSP_IRQ_NUMBER];

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
   * Initialize RTEMS management interrupt table
   */
    /*
     * re-init the rtemsIrq table
     */
    for (i = 0; i < BSP_IRQ_NUMBER; i++) {
      irqPrioTable[i]  = 8;
      rtemsIrq[i]      = defaultIrq;
      rtemsIrq[i].name = i;
#ifdef BSP_SHARED_HANDLER_SUPPORT
      rtemsIrq[i].next_handler = NULL;
#endif
    }

    /*
     * Init initial Interrupt management config
     */
    initial_config.irqNb 	= BSP_IRQ_NUMBER;
    initial_config.defaultEntry = defaultIrq;
    initial_config.irqHdlTbl	= rtemsIrq;
    initial_config.irqBase	= BSP_LOWEST_OFFSET;
    initial_config.irqPrioTbl	= irqPrioTable;

    if (!BSP_rtems_irq_mngt_set(&initial_config)) {
      /*
       * put something here that will show the failure...
       */
      BSP_panic("Unable to initialize RTEMS interrupt Management!!! System locked\n");
    }

#ifdef TRACE_IRQ_INIT
    printk("RTEMS IRQ management is now operational\n");
#endif
}

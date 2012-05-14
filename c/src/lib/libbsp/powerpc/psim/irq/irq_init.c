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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <libcpu/io.h>
#include <libcpu/spr.h>
#include <bsp/irq.h>
#include <bsp.h>
#include <psim.h>
#include <bsp/vectors.h>
#include <rtems/bspIo.h>
#include <bsp/openpic.h>

static rtems_irq_connect_data      rtemsIrq[BSP_IRQ_NUMBER];
static rtems_irq_global_settings   initial_config;
static rtems_irq_connect_data      defaultIrq = {
  /* vectorIdex, hdl  , handle  , on  , off , isOn */
      0,          NULL, NULL    , NULL, NULL, NULL
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
   * First initialize the Interrupt management hardware
   */
  OpenPIC = (void*)PSIM.OpenPIC;
  openpic_init(1,0,0,16,0,0);

  /*
   * Initialize Rtems management interrupt table
   */
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

  for (i = BSP_PCI_IRQ_LOWEST_OFFSET; i< BSP_PCI_IRQ_NUMBER; i++ ) {
  	irqPrioTable[i] = 8;
  }

  if (!BSP_rtems_irq_mngt_set(&initial_config)) {
    /*
     * put something here that will show the failure...
     */
    BSP_panic(
      "Unable to initialize RTEMS interrupt Management!!! System locked\n"
    );
  }

  #ifdef TRACE_IRQ_INIT
    printk("RTEMS IRQ management is now operationnal\n");
  #endif
}

int psim_exception_handler( BSP_Exception_frame *frame, unsigned exception_number)
{
  BSP_panic("Unexpected interrupt occured");
  return 0;
}

/*
 * functions to enable/disable a source at the ipic
 */
rtems_status_code bsp_interrupt_vector_enable( rtems_vector_number irqnum)
{
  /* FIXME: do something */
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable( rtems_vector_number irqnum)
{
  /* FIXME: do something */
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
  /* Install exception handler */
  if (ppc_exc_set_handler( ASM_EXT_VECTOR, psim_exception_handler)) {
    return RTEMS_IO_ERROR;
  }
  
  return RTEMS_SUCCESSFUL;
}

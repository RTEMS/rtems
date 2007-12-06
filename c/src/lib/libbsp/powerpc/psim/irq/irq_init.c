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
 *
 *  irq_init.c,v 1.6.2.5 2003/09/04 18:45:20 joel Exp
 */

#include <libcpu/io.h>
#include <libcpu/spr.h>
#include <bsp/irq.h>
#include <bsp.h>
#include <libcpu/raw_exception.h>
#include <rtems/bspIo.h>

/*
 * default on/off function
 */
static void nop_func(){}
/*
 * default isOn function
 */
static int not_connected() {return 0;}
/*
 * default possible isOn function
 */
static int connected() {return 1;}

static rtems_irq_connect_data      rtemsIrq[BSP_IRQ_NUMBER];
static rtems_irq_global_settings   initial_config;
static rtems_irq_connect_data      defaultIrq = {
  /* vectorIdex,     hdl   , handle  , on       , off      , isOn */
      0,          nop_func , NULL    , nop_func , nop_func , not_connected
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
  rtems_raw_except_connect_data vectorDesc;
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
      "Unable to initialize RTEMS interrupt Management!!! System locked\n"
    );
  }
  
  /*
   * We must connect the raw irq handler for the two
   * expected interrupt sources : decrementer and external interrupts.
   */
  vectorDesc.exceptIndex      = ASM_DEC_VECTOR;
  vectorDesc.hdl.vector       = ASM_DEC_VECTOR;
  vectorDesc.hdl.raw_hdl      = decrementer_exception_vector_prolog_code;
  vectorDesc.hdl.raw_hdl_size = 
     (unsigned) decrementer_exception_vector_prolog_code_size;
  vectorDesc.on               =   nop_func;
  vectorDesc.off              =   nop_func;
  vectorDesc.isOn             =   connected;
  if (!ppc_set_exception (&vectorDesc))
  {
    BSP_panic("Unable to initialize RTEMS decrementer raw exception\n");
  }

  vectorDesc.exceptIndex      = ASM_EXT_VECTOR;
  vectorDesc.hdl.vector       = ASM_EXT_VECTOR;
  vectorDesc.hdl.raw_hdl      = external_exception_vector_prolog_code;
  vectorDesc.hdl.raw_hdl_size = 
    (unsigned) external_exception_vector_prolog_code_size;
  if (!ppc_set_exception (&vectorDesc)) {
    BSP_panic("Unable to initialize RTEMS external raw exception\n");
  }
  #ifdef TRACE_IRQ_INIT  
    printk("RTEMS IRQ management is now operationnal\n");
  #endif
}

/* irq_init.c
 *
 *  This file contains the implementation of rtems initialization
 *  related to interrupt handling.
 *
 *  CopyRight (C) 2001 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */
#include <bsp/irq.h>
#include <bsp.h>
#include <rtems/bspIo.h>
#include <libcpu/raw_exception.h>
/*
#include <bsp/8xx_immap.h>
#include <bsp/mbx.h>
#include <bsp/commproc.h>
*/

extern unsigned int external_exception_vector_prolog_code_size;
extern void external_exception_vector_prolog_code();
extern unsigned int decrementer_exception_vector_prolog_code_size;
extern void decrementer_exception_vector_prolog_code();

extern void BSP_panic(char *s);
extern void _BSP_Fatal_error(unsigned int v);
/*
volatile unsigned int ppc_cached_irq_mask;
*/

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

static rtems_irq_connect_data     	rtemsIrq[BSP_IRQ_NUMBER];
static rtems_irq_global_settings     	initial_config;
static rtems_irq_connect_data     	defaultIrq = {
  /* vectorIdex,	 hdl		, on		, off		, isOn */
  0, 			 nop_func	, nop_func	, nop_func	, not_connected
};
static rtems_irq_prio irqPrioTable[BSP_IRQ_NUMBER]={
  /*
   * actual priorities for interrupt :
   */
  /*
   * CPM Interrupts
   */
  0,  45, 63, 44, 66, 68, 35, 39, 50, 62, 34,  0,  30, 40, 52, 58,
  2,  3,  0,  5,  15, 16, 17, 18, 49, 51,  0,  0,  0,  0,  0,  0,
  6,  7,  8,  0,  11, 12, 0,  0,  20, 21, 22, 23,  0,  0,  0,  0,
  29, 31, 33, 37, 38, 41, 47, 48, 55, 56, 57, 60, 64, 65, 69, 70,
  /*
   * Processor exceptions handled as interrupts
   */
  0
};


/* 
 * Initialize CPM interrupt management
 */
void
BSP_CPM_irq_init(void)
{
   m8260.simr_l = 0;
   m8260.simr_h = 0;
   m8260.sipnr_l = 0xffffffff;
   m8260.sipnr_h = 0xffffffff;
   m8260.sicr = 0;

  /*
   * Initialize the interrupt priorities.
   */
   m8260.siprr   = 0x05309770;	/* reset value */
   m8260.scprr_h = 0x05309770;	/* reset value */
   m8260.scprr_l = 0x05309770;	/* reset value */

}

void BSP_rtems_irq_mng_init(unsigned cpuId)
{
  rtems_raw_except_connect_data vectorDesc;
  int i;
  
  BSP_CPM_irq_init();
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
    initial_config.irqNb 	= BSP_IRQ_NUMBER;
    initial_config.defaultEntry = defaultIrq;
    initial_config.irqHdlTbl	= rtemsIrq;
    initial_config.irqBase	= BSP_ASM_IRQ_VECTOR_BASE;
    initial_config.irqPrioTbl	= irqPrioTable;

    if (!BSP_rtems_irq_mngt_set(&initial_config)) {
      /*
       * put something here that will show the failure...
       */
      BSP_panic("Unable to initialize RTEMS interrupt Management!!! System locked\n");
    }
  
  /*
   * We must connect the raw irq handler for the two
   * expected interrupt sources : decrementer and external interrupts.
   */
    vectorDesc.exceptIndex 	=	ASM_DEC_VECTOR;
    vectorDesc.hdl.vector	=	ASM_DEC_VECTOR;
    vectorDesc.hdl.raw_hdl	=	decrementer_exception_vector_prolog_code;
    vectorDesc.hdl.raw_hdl_size	=	(unsigned) &decrementer_exception_vector_prolog_code_size;
    vectorDesc.on		=	nop_func;
    vectorDesc.off		=	nop_func;
    vectorDesc.isOn		=	connected;
    if (!mpc8xx_set_exception (&vectorDesc)) {
      BSP_panic("Unable to initialize RTEMS decrementer raw exception\n");
    }
    vectorDesc.exceptIndex	=	ASM_EXT_VECTOR;
    vectorDesc.hdl.vector	=	ASM_EXT_VECTOR;
    vectorDesc.hdl.raw_hdl	=	external_exception_vector_prolog_code;
    vectorDesc.hdl.raw_hdl_size	=	(unsigned) &external_exception_vector_prolog_code_size;
    if (!mpc8xx_set_exception (&vectorDesc)) {
      BSP_panic("Unable to initialize RTEMS external raw exception\n");
    }
#ifdef TRACE_IRQ_INIT  
    printk("RTEMS IRQ management is now operationnal\n");
#endif
}


/* irq_init.c
 *
 *  This file contains the implementation of rtems initialization
 *  related to interrupt handling.
 *
 *  CopyRight (C) 2001 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
#include <bsp/irq.h>
#include <bsp.h>
#include <libcpu/raw_exception.h>
#include <bsp/8xx_immap.h>
#include <bsp/mbx.h>
#include <bsp/commproc.h>

extern unsigned int external_exception_vector_prolog_code_size;
extern void external_exception_vector_prolog_code();
extern unsigned int decrementer_exception_vector_prolog_code_size;
extern void decrementer_exception_vector_prolog_code();

volatile unsigned int ppc_cached_irq_mask;

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
   * actual rpiorities for interrupt :
   *	0   means that only current interrupt is masked
   *	255 means all other interrupts are masked
   */
  /*
   * SIU interrupts.
   */
  7,7, 6,6, 5,5, 4,4, 3,3, 2,2, 1,1, 0,0,
  /*
   * CPM Interrupts
   */
  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
  16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
  /*
   * Processor exceptions handled as interrupts
   */
  0
};

void BSP_SIU_irq_init()
{
  /*
   * In theory we should initialize two registers at least :
   * SIMASK, SIEL. SIMASK is reset at 0 value meaning no interrupt. But
   * we should take care that a monitor may have restoreed to another value.
   * If someone find a reasonnable value for SIEL, AND THE NEED TO CHANGE IT
   * please feel free to add it here.
   */
  ((volatile immap_t *)IMAP_ADDR)->im_siu_conf.sc_simask = 0;
  ((volatile immap_t *)IMAP_ADDR)->im_siu_conf.sc_sipend = 0xffff0000;
  ppc_cached_irq_mask = 0;
  ((volatile immap_t *)IMAP_ADDR)->im_siu_conf.sc_siel = ((volatile immap_t *)IMAP_ADDR)->im_siu_conf.sc_siel;
}

/* 
 * Initialize CPM interrupt management
 */
void
BSP_CPM_irq_init(void)
{
  /*
   * Initialize the CPM interrupt controller.
   */
  ((volatile immap_t *)IMAP_ADDR)->im_cpic.cpic_cicr =
#ifdef mpc860
    (CICR_SCD_SCC4 | CICR_SCC_SCC3 | CICR_SCB_SCC2 | CICR_SCA_SCC1) |
#else
    (CICR_SCB_SCC2 | CICR_SCA_SCC1) |
#endif    
    ((BSP_CPM_INTERRUPT/2) << 13) | CICR_HP_MASK;
  ((volatile immap_t *)IMAP_ADDR)->im_cpic.cpic_cimr = 0;

  ((volatile immap_t *)IMAP_ADDR)->im_cpic.cpic_cicr |= CICR_IEN;
}

void BSP_rtems_irq_mng_init(unsigned cpuId)
{
  rtems_raw_except_connect_data vectorDesc;
  int i;
  
  BSP_SIU_irq_init();
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


/* irq_init.c
 *
 * This file contains the implementation of rtems initialization
 * related to interrupt handling.
 *
 * CopyRight (C) 1999 valette@crf.canon.fr
 *
 * Special acknowledgement to Till Straumann <strauman@slac.stanford.edu>
 * for providing inputs to the IRQ optimization.
 * 
 * Modified and added support for the MVME5500.
 * Copyright 2003, 2004, Brookhaven National Laboratory and
 *                 Shuchen Kate Feng <feng1@bnl.gov>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 * 
 */
#include <libcpu/io.h>
#include <libcpu/spr.h>
#include <bsp/irq.h>
#include <bsp.h>
#include <libcpu/raw_exception.h>  /* ASM_EXT_VECTOR, ASM_DEC_VECTOR ... */

extern unsigned int external_exception_vector_prolog_code_size[];
extern void external_exception_vector_prolog_code();
extern unsigned int decrementer_exception_vector_prolog_code_size[];
extern void decrementer_exception_vector_prolog_code();
extern void GT_GPP_IntHandler0(), GT_GPP_IntHandler1();
extern void GT_GPP_IntHandler2(), GT_GPP_IntHandler3();
extern void BSP_GT64260INT_init();

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
  /* vectorIdex,	 hdl		, handle	, on		, off		, isOn */
  0, 			 nop_func	, NULL		, nop_func	, nop_func	, not_connected
};

rtems_irq_prio BSPirqPrioTable[BSP_MAIN_IRQ_NUMBER]={
  /*
   * This table is where the developers can change the levels of priority
   * based on the need of their applications.
   *
   * actual priorities for CPU MAIN interrupts 0-63:
   *	0   means that only current interrupt is masked (lowest priority)
   *	255 means all other interrupts are masked
   */
  /* CPU Main cause low interrupt */
  /* 0-15 */
  0, 0, 0, 0, 0, 0, 0, 0, 4/*Timer*/, 0, 0, 0, 0, 0, 0, 0,
   /* 16-31 */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  /* CPU Main cause high interrupt */
  /* 32-47 */
  1/*10/100MHZ*/, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  /* 48-63 */
  0, 0, 0, 0, 0, 0, 0, 0, 0/*serial*/, 3/*VME*/, 2/*1GHZ*/, 5/*WD*/, 0, 0, 0, 0
};

/* The mainIrqTbl[64] lists the enabled CPU main interrupt
 * numbers [0-63] starting from the highest priority one
 * to the lowest priority one.
 *
 * The highest priority interrupt is located at mainIrqTbl[0], and 
 * the lowest priority interrupt is located at 
 * mainIrqTbl[MainIrqTblPtr-1].
 */

#if DynamicIrqTbl
/* The mainIrqTbl[64] is updated dynamically based on the priority
 * levels set at BSPirqPrioTable[64], as the BSP_enable_main_irq() and
 * BSP_disable_main_irq() commands are invoked.
 *
 * Caveat: The eight GPP IRQs for each BSP_MAIN_GPPx_y_IRQ group are set
 * at the same main priority in the BSPirqPrioTable, while the 
 * sub-priority levels for the eight GPP in each group  are sorted 
 * statically by developers in the GPPx_yIrqTbl[8] from the highest
 * priority to the lowest one.
 */
int MainIrqTblPtr=0;
unsigned long long MainIrqInTbl=0;
unsigned char GPPinMainIrqTbl[4]={0,0,0,0};
/* BitNums for Main Interrupt Lo/High Cause, -1 means invalid bit */ 
unsigned int mainIrqTbl[BSP_MAIN_IRQ_NUMBER]={  
                               -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			       -1, -1, -1, -1};
#else
/* Pre-sorted for IRQ optimization, and prioritization 
 * The interrupts sorted are :

 1. Watchdog timer      (GPP #25)
 2. Timers 0-1          (Main interrupt low cause, bit 8)
 3. VME interrupt       (GPP #12)
 4. 1 GHZ ethernet      (GPP #20)
 5. 10/100 MHZ ethernet (Main interrupt high cause, bit 0)
 6. COM1/COM2           (GPP #0)

*/ 
/* BitNums for Main Interrupt Lo/High Cause, -1 means invalid bit */ 
unsigned int mainIrqTbl[64]={ BSP_MAIN_GPP31_24_IRQ, /* 59:watchdog timer */
			       BSP_MAIN_TIMER0_1_IRQ, /* 8:Timers 0-1 */
			       BSP_MAIN_GPP15_8_IRQ,  /* 57:VME interrupt */
			       BSP_MAIN_GPP23_16_IRQ, /* 58: 1 GHZ ethernet */
			       BSP_MAIN_ETH0_IRQ,  /* 32:10/100 MHZ ethernet */
			       BSP_MAIN_GPP7_0_IRQ, /* 56:COM1/COM2 */
			       -1, -1, -1, -1,
			       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			       -1, -1, -1, -1};
#endif

unsigned int GPP7_0IrqTbl[8]={0, /* COM1/COM2 */
                               -1, -1, -1, -1, -1, -1, -1};
unsigned int GPP15_8IrqTbl[8]={ 4, 5, 6, 7,  /* VME interrupt 0-3 */
				0, 1, 2, 3   /* PMC1 INT A, B, C, D */};
unsigned int GPP23_16IrqTbl[8]={4, /* 82544 1GHZ ethernet (20-16=4)*/
				0, 1, 2, 3, /* PMC2 INT A, B, C, D */
			     	-1, -1, -1};
unsigned int GPP31_24IrqTbl[8]={1, /* watchdog timer (25-24=1) */
				-1, -1, -1, -1, -1, -1, -1};

static int
doit(unsigned intNum, rtems_irq_hdl handler, int (*p)(const rtems_irq_connect_data*))
{
	rtems_irq_connect_data d={0};
	d.name = intNum;
	d.isOn = connected;
	d.hdl  = handler;
	return p(&d);
}

int BSP_GT64260_install_isr(unsigned intNum,rtems_irq_hdl handler)
{
  return doit(intNum, handler, BSP_install_rtems_irq_handler);
}

/*
 * This code assumes the exceptions management setup has already
 * been done. We just need to replace the exceptions that will
 * be handled like interrupt. On MPC7455 and many PPC processors
 * this means the decrementer exception and the external exception.
 */
void BSP_rtems_irq_mng_init(unsigned cpuId)
{
  rtems_raw_except_connect_data vectorDesc;
  int i;

  /*
   * First initialize the Interrupt management hardware
   */
#ifdef TRACE_IRQ_INIT  
  printk("Initializing the interrupt controller of the GT64260\n");
#endif       
  BSP_GT64260INT_init();

#ifdef TRACE_IRQ_INIT  
  printk("Going to re-initialize the rtemsIrq table %d\n",BSP_IRQ_NUMBER);
#endif       
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
  initial_config.defaultEntry   = defaultIrq;
  initial_config.irqHdlTbl	= rtemsIrq;
  initial_config.irqBase	= BSP_ASM_IRQ_VECTOR_BASE;
  initial_config.irqPrioTbl	= BSPirqPrioTable;

#ifdef TRACE_IRQ_INIT  
  printk("Going to setup irq mngt configuration\n");
#endif       

  if (!BSP_rtems_irq_mngt_set(&initial_config)) {
      /*
       * put something here that will show the failure...
       */
      BSP_panic("Unable to initialize RTEMS interrupt Management!!! System locked\n");
  }

  /* Connect the GPP int handler to each of the associated main cause bits */
  BSP_GT64260_install_isr(BSP_MAIN_GPP7_0_IRQ, GT_GPP_IntHandler0); /* COM1 & COM2, .... */
  BSP_GT64260_install_isr(BSP_MAIN_GPP15_8_IRQ, GT_GPP_IntHandler1);
  BSP_GT64260_install_isr(BSP_MAIN_GPP23_16_IRQ, GT_GPP_IntHandler2);
  BSP_GT64260_install_isr(BSP_MAIN_GPP31_24_IRQ, GT_GPP_IntHandler3);
 
  /*
   * We must connect the raw irq handler for the two
   * expected interrupt sources : decrementer and external interrupts.
   */
  vectorDesc.exceptIndex 	= ASM_DEC_VECTOR;
  vectorDesc.hdl.vector	= ASM_DEC_VECTOR;
  vectorDesc.hdl.raw_hdl	= decrementer_exception_vector_prolog_code;
  vectorDesc.hdl.raw_hdl_size	= (unsigned) decrementer_exception_vector_prolog_code_size;
  vectorDesc.on		= nop_func;
  vectorDesc.off		= nop_func;
  vectorDesc.isOn		= connected;
  if (!ppc_set_exception (&vectorDesc)) {
      BSP_panic("Unable to initialize RTEMS decrementer raw exception\n");
  }
  vectorDesc.exceptIndex	= ASM_EXT_VECTOR;
  vectorDesc.hdl.vector	= ASM_EXT_VECTOR;
  vectorDesc.hdl.raw_hdl	= external_exception_vector_prolog_code;
  vectorDesc.hdl.raw_hdl_size	= (unsigned) external_exception_vector_prolog_code_size;
  if (!ppc_set_exception (&vectorDesc)) {
      BSP_panic("Unable to initialize RTEMS external raw exception\n");
    }
#ifdef TRACE_IRQ_INIT  
  printk("RTEMS IRQ management is now operationnal\n");
#endif
}

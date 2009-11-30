/* irq_init.c
 *
 * This file contains the implementation of rtems initialization
 * related to interrupt handling.
 *
 * CopyRight (C) 1999 valette@crf.canon.fr
 *
 * Modified and added support for the MVME5500.
 * Copyright 2003, 2004, 2005, Brookhaven National Laboratory and
 *                 Shuchen Kate Feng <feng1@bnl.gov>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE
 *
 */
#include <libcpu/io.h>
#include <libcpu/spr.h>
#include <bsp/irq.h>
#include <bsp.h>
#include <bsp/vectors.h>  /* ASM_EXT_VECTOR, ASM_DEC_VECTOR ... */
/*#define  TRACE_IRQ_INIT*/

/*
 * default on/off function
 */
static void nop_func(void){}
/*
 * default isOn function
 */
static int not_connected(void) {return 0;}
/*
 * default possible isOn function
 */
static int connected(void) {return 1;}

static rtems_irq_connect_data     	rtemsIrq[BSP_IRQ_NUMBER];
static rtems_irq_global_settings     	initial_config;

#ifdef BSP_SHARED_HANDLER_SUPPORT
static rtems_irq_connect_data     	defaultIrq = {
  /* vectorIdex,  hdl	    ,handle  , on	, off	   , isOn      ,next_handler, */
  0, 		  nop_func  , NULL   , nop_func	, nop_func , not_connected, 0
};
#else
static rtems_irq_connect_data     	defaultIrq = {
  /* vectorIdex,	 hdl	  , handle	, on		, off		, isOn */
  0, 			 nop_func  , NULL	, nop_func	, nop_func	, not_connected
};
#endif

rtems_irq_prio BSPirqPrioTable[BSP_PIC_IRQ_NUMBER]={
  /*
   * This table is where the developers can change the levels of priority
   * based on the need of their applications.
   *
   * actual priorities for CPU MAIN and GPP interrupts (0-95)
   *
   *	0   means that only current interrupt is masked (lowest priority)
   *	255 is only used by bits 24, 25, 26 and 27 of the CPU high
   *        interrupt Mask: (e.g. GPP7_0, GPP15_8, GPP23_16, GPP31_24).
   *        The IRQs of those four bits are always enabled. When it's used,
   *        the IRQ number is never listed in the dynamic picIsrTable[96].
   *
   *        The priorities of GPP interrupts were decided by their own
   *        value set at  BSPirqPrioTable.
   *
   */
  /* CPU Main cause low interrupt */
  /* 0-15 */
  0, 0, 0, 0, 0, 0, 0, 0, 64/*Timer*/, 0, 0, 0, 0, 0, 0, 0,
   /* 16-31 */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  /* CPU Main cause high interrupt */
  /* 32-47 */
  2/*10/100MHZ*/, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  /* 48-63 */
  0, 0, 0, 0, 0, 0, 0, 0,
  255 /*GPP0-7*/, 255/*GPP8-15*/, 255/*GPP16-23*/, 255/*GPP24-31*/, 0, 0, 0, 0,
  /* GPP interrupts */
  /* GPP0-7 */
  1/*serial*/,0, 0, 0, 0, 0, 0, 0,
  /* GPP8-15 */
  47/*PMC1A*/,46/*PMC1B*/,45/*PMC1C*/,44/*PMC1D*/,30/*VME0*/, 29/*VME1*/,3,1,
  /* GPP16-23 */
  37/*PMC2A*/,36/*PMC2B*/,35/*PMC2C*/,34/*PMC2D*/,23/*1GHZ*/, 0,0,0,
  /* GPP24-31 */
  7/*watchdog*/, 0,0,0,0,0,0,0
};

/*
 * This code assumes the exceptions management setup has already
 * been done. We just need to replace the exceptions that will
 * be handled like interrupt. On MPC7455 and many PPC processors
 * this means the decrementer exception and the external exception.
 */
void BSP_rtems_irq_mng_init(unsigned cpuId)
{
  int                   i;
  rtems_interrupt_level l;

  /*
   * First initialize the Interrupt management hardware
   */
#ifdef TRACE_IRQ_INIT
  printk("Initializing the interrupt controller of the GT64260\n");
#endif

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
  initial_config.irqBase	= BSP_LOWEST_OFFSET;
  initial_config.irqPrioTbl	= BSPirqPrioTable;

#ifdef TRACE_IRQ_INIT
  printk("Going to setup irq mngt configuration\n");
#endif

  rtems_interrupt_disable(l);
  if (!BSP_rtems_irq_mngt_set(&initial_config)) {
      /*
       * put something here that will show the failure...
       */
      BSP_panic("Unable to initialize RTEMS interrupt Management!!! System locked\n");
  }
#ifdef TRACE_IRQ_INIT
  printk("Done setup irq mngt configuration\n");
#endif

#ifdef TRACE_IRQ_INIT
  printk("RTEMS IRQ management is now operationnal\n");
#endif
}

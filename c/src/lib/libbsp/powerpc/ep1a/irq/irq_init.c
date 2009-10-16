/* irq_init.c
 *
 *  This file contains the implementation of rtems initialization
 *  related to interrupt handling.
 *
 *  CopyRight (C) 1999 valette@crf.canon.fr
 *
 *  Enhanced by Jay Kulpinski <jskulpin@eng01.gdds.com>
 *  to make it valid for MVME2300 Motorola boards.
 *
 *  Till Straumann <strauman@slac.stanford.edu>, 12/20/2001:
 *  Use the new interface to openpic_init
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <libcpu/io.h>
#include <libcpu/spr.h>
#include <bsp/pci.h>
#include <bsp/residual.h>
#include <bsp/openpic.h>
#include <bsp/irq.h>
#include <bsp.h>
#include <libcpu/raw_exception.h>
#include <bsp/motorola.h>
#include <rtems/bspIo.h>

#if 0 
#define TRACE_IRQ_INIT 1   /* XXX */
#endif

typedef struct {
  unsigned char bus;	/* few chance the PCI/ISA bridge is not on first bus but ... */
  unsigned char device;
  unsigned char function;
} pci_isa_bridge_device;

pci_isa_bridge_device* via_82c586 = 0;
#if 0
static pci_isa_bridge_device bridge;
#endif



extern unsigned int external_exception_vector_prolog_code_size[];
extern void external_exception_vector_prolog_code();
extern unsigned int decrementer_exception_vector_prolog_code_size[];
extern void decrementer_exception_vector_prolog_code();


static void IRQ_Default_rtems_irq_hdl( rtems_irq_hdl_param ptr ) { printk("IRQ_Default_rtems_irq_hdl\n"); }
static void IRQ_Default_rtems_irq_enable (const struct __rtems_irq_connect_data__ *ptr)   {}
static void IRQ_Default_rtems_irq_disable(const struct __rtems_irq_connect_data__ *ptr)   {}
static int  IRQ_Default_rtems_irq_is_enabled(const struct __rtems_irq_connect_data__ *ptr){ return 1; }
static rtems_irq_connect_data     	rtemsIrq[BSP_IRQ_NUMBER];
static rtems_irq_global_settings     	initial_config;
static rtems_irq_connect_data     	defaultIrq = {
/*name,	 hdl				handle	on				off				isOn */
  0,	 IRQ_Default_rtems_irq_hdl,	NULL,	IRQ_Default_rtems_irq_enable,	IRQ_Default_rtems_irq_disable,	IRQ_Default_rtems_irq_is_enabled
};


/*
 *  If the BSP_IRQ_NUMBER changes the following if will force the tables to be corrected.
 */
#if ( (BSP_ISA_IRQ_NUMBER == 16)      && \
      (BSP_PCI_IRQ_NUMBER == 26)      && \
      (BSP_PROCESSOR_IRQ_NUMBER == 1) && \
      (BSP_MISC_IRQ_NUMBER == 8) )


static rtems_irq_prio irqPrioTable[BSP_IRQ_NUMBER]={
  /*
   * ISA interrupts.
   */
  0, 0,
  (OPENPIC_NUM_PRI-1),
  0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,

  /*
   * PCI Interrupts
   */
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 

  /*
   * Processor exceptions handled as interrupts
   */
  8,
 
  8, 8, 8, 8, 8, 8, 8, 8
};

static unsigned char mpc8245_openpic_initpolarities[] = {
    1,  /*  0 8259 cascade */
    0,	/*  1 */
    0,  /*  2 */
    0,  /*  3 */
    0,  /*  4 */
    0,  /*  5 */
    0,  /*  6 */
    0,  /*  7 */
    0,  /*  8 */
    0,  /*  9 */
    0,  /* 10 */
    0,  /* 11 */
    0,  /* 12 */
    0,  /* 13 */
    0,  /* 14 */
    0,  /* 15 */
    0,  /* 16 */
    0,  /* 17 */
    1,  /* 18 all the rest of them */
    1,  /* 19 all the rest of them */
    1,  /* 20 all the rest of them */
    1,  /* 21 all the rest of them */
    1,  /* 22 all the rest of them */
    1,  /* 23 all the rest of them */
    1,  /* 24 all the rest of them */
    1,  /* 25 all the rest of them */
    1,  /* 26 all the rest of them */
    1,	/* 27 all the rest of them */
    1,  /* 28 all the rest of them */
    1,  /* 29 all the rest of them */
    1,  /* 30 all the rest of them */
    1,  /* 31 all the rest of them */
    1,  /* 32 all the rest of them */
    1,  /* 33 all the rest of them */
    1,  /* 34 all the rest of them */
    1,  /* 35 all the rest of them */
    1,  /* 36 all the rest of them */
    1,  /* 37 all the rest of them */
    1,  /* 38 all the rest of them */
    1,  /* 39 all the rest of them */
    1,  /* 40 all the rest of them */
    1,  /* 41 all the rest of them */
    1,  /* 42 all the rest of them */
    1,  /* 43 all the rest of them */
    1,  /* 44 all the rest of them */
    1,  /* 45 all the rest of them */
    1,  /* 46 all the rest of them */
    1,  /* 47 all the rest of them */
    1,  /* 48 all the rest of them */
    1,  /* 49 all the rest of them */
    1,  /* 50 all the rest of them */
    1,  /* 51 all the rest of them */

};

static unsigned char mpc8245_openpic_initsenses[] = {
    1,	/* 0  */
    0,	/* 1  */
    1,	/* 2  */
    1,	/* 3  */
    1,	/* 4  */
    1,	/* 5  */
    1,	/* 6  */
    1,	/* 7  */
    1,	/* 8  */
    1,	/* 9  */
    1,	/*10  */
    1,	/*11  */
    1,	/*12  */
    1,	/*13  */
    1,	/*14  */
    1,	/*15  */
    1,
    1, 
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1, 
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1
};
#endif

  /*
   * This code assumes the exceptions management setup has already
   * been done. We just need to replace the exceptions that will
   * be handled like interrupt. On mcp750/mpc750 and many PPC processors
   * this means the decrementer exception and the external exception.
   */
void BSP_rtems_irq_mng_init(unsigned cpuId)
{
  int                           i;

  /*
   * First initialize the Interrupt management hardware
   */
#ifdef TRACE_IRQ_INIT  
  uint32_t                      msr;

  _CPU_MSR_GET( msr );
  printk("BSP_rtems_irq_mng_init: Initialize openpic compliant device with MSR %x \n", msr);
  printk(" BSP_ISA_IRQ_NUMBER %d\n",BSP_ISA_IRQ_NUMBER );
  printk(" BSP_ISA_IRQ_LOWEST_OFFSET %d\n",BSP_ISA_IRQ_LOWEST_OFFSET );
  printk(" BSP_ISA_IRQ_MAX_OFFSET %d\n", BSP_ISA_IRQ_MAX_OFFSET);
  printk(" BSP_PCI_IRQ_NUMBER %d\n",BSP_PCI_IRQ_NUMBER );
  printk(" BSP_PCI_IRQ_LOWEST_OFFSET %d\n",BSP_PCI_IRQ_LOWEST_OFFSET );
  printk(" BSP_PCI_IRQ_MAX_OFFSET %d\n",BSP_PCI_IRQ_MAX_OFFSET );
  printk(" BSP_PROCESSOR_IRQ_NUMBER %d\n",BSP_PROCESSOR_IRQ_NUMBER );
  printk(" BSP_PROCESSOR_IRQ_LOWEST_OFFSET %d\n",BSP_PROCESSOR_IRQ_LOWEST_OFFSET );
  printk(" BSP_PROCESSOR_IRQ_MAX_OFFSET %d\n", BSP_PROCESSOR_IRQ_MAX_OFFSET);
  printk(" BSP_MISC_IRQ_NUMBER %d\n", BSP_MISC_IRQ_NUMBER);
  printk(" BSP_MISC_IRQ_LOWEST_OFFSET %d\n", BSP_MISC_IRQ_LOWEST_OFFSET);
  printk(" BSP_MISC_IRQ_MAX_OFFSET %d\n",BSP_MISC_IRQ_MAX_OFFSET );
  printk(" BSP_IRQ_NUMBER %d\n",BSP_IRQ_NUMBER );
  printk(" BSP_LOWEST_OFFSET %d\n",BSP_LOWEST_OFFSET );
  printk(" BSP_MAX_OFFSET %d\n",BSP_MAX_OFFSET );
#endif
       
  /* FIXME (t.s.): we should probably setup the EOI delay by
   * passing a non-zero 'epic_freq' argument (frequency of the
   * EPIC serial interface) but I don't know the value on this
   * board (8245 SDRAM freq, IIRC)...
   *
   * When tested this appears to work correctly.
   */
  openpic_init(1, mpc8245_openpic_initpolarities, mpc8245_openpic_initsenses, 0, 0, 0);

  /*
   * Initialize Rtems management interrupt table
   */
    /*
     * re-init the rtemsIrq table
     */
    for (i = 0; i < BSP_IRQ_NUMBER; i++) {
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


#ifdef TRACE_IRQ_INIT  
    _CPU_MSR_GET( msr );
    printk("BSP_rtems_irq_mng_init: Set initial configuration with MSR %x\n", msr);
#endif
    if (!BSP_rtems_irq_mngt_set(&initial_config)) {
      /*
       * put something here that will show the failure...
       */
      BSP_panic("Unable to initialize RTEMS interrupt Management!!! System locked\n");
    }  else {
      printk(" Initialized RTEMS Interrupt Manager\n");
    }

#ifdef TRACE_IRQ_INIT  
    printk("RTEMS IRQ management is now operationnal\n");
#endif
}


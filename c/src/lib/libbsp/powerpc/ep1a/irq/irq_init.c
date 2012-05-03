/*
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
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <libcpu/io.h>
#include <libcpu/spr.h>
#include <bsp/pci.h>
#include <bsp/residual.h>
#include <bsp/openpic.h>
#include <bsp/irq.h>
#include <bsp.h>
#include <bsp/vectors.h>
#include <bsp/motorola.h>
#include <rtems/bspIo.h>

/*
#define SHOW_ISA_PCI_BRIDGE_SETTINGS
*/
#define TRACE_IRQ_INIT

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
static rtems_irq_connect_data     	defaultIrq = {
  /* vectorIdex,	 hdl		, handle	, on		, off		, isOn */
  0, 			 nop_func	, NULL		, nop_func	, nop_func	, not_connected
};
static rtems_irq_prio irqPrioTable[BSP_IRQ_NUMBER]={
  /*
   * actual rpiorities for interrupt :
   *	0   means that only current interrupt is masked
   *	255 means all other interrupts are masked
   */
  /*
   * ISA interrupts.
   * The second entry has a priority of 255 because
   * it is the slave pic entry and is should always remain
   * unmasked.
   */
  0,0,
  255,
  0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*
   * PCI Interrupts
   */
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, /* for raven prio 0 means unactive... */
  /*
   * Processor exceptions handled as interrupts
   */
  0
};

static unsigned char mcp750_openpic_initpolarities[] = {
    1,  /*  0 8259 cascade */
    0,	/*  1 all the rest of them */
    0,  /*  2 all the rest of them */
    0,  /*  3 all the rest of them */
    0,  /*  4 all the rest of them */
    0,  /*  5 all the rest of them */
    0,  /*  6 all the rest of them */
    0,  /*  7 all the rest of them */
    0,  /*  8 all the rest of them */
    0,  /*  9 all the rest of them */
    0,  /* 10 all the rest of them */
    0,  /* 11 all the rest of them */
    0,  /* 12 all the rest of them */
    0,  /* 13 all the rest of them */
    0,  /* 14 all the rest of them */
    0,  /* 15 all the rest of them */
    0,  /* 16 all the rest of them */
    0,  /* 17 all the rest of them */
    1,  /* 18 all the rest of them */
    1,  /* 19 all the rest of them */
    1,  /* 20 all the rest of them */
    1,  /* 21 all the rest of them */
    1,  /* 22 all the rest of them */
    1,  /* 23 all the rest of them */
    1,  /* 24 all the rest of them */
    1,  /* 25 all the rest of them */
};

static unsigned char mcp750_openpic_initsenses[] = {
    1,	/* 0 MCP750_INT_PCB(8259) */
    0,	/* 1 MCP750_INT_FALCON_ECC_ERR */
    1,	/* 2 MCP750_INT_PCI_ETHERNET */
    1,	/* 3 MCP750_INT_PCI_PMC */
    1,	/* 4 MCP750_INT_PCI_WATCHDOG_TIMER1 */
    1,	/* 5 MCP750_INT_PCI_PRST_SIGNAL */
    1,	/* 6 MCP750_INT_PCI_FALL_SIGNAL */
    1,	/* 7 MCP750_INT_PCI_DEG_SIGNAL */
    1,	/* 8 MCP750_INT_PCI_BUS1_INTA */
    1,	/* 9 MCP750_INT_PCI_BUS1_INTB */
    1,	/*10 MCP750_INT_PCI_BUS1_INTC */
    1,	/*11 MCP750_INT_PCI_BUS1_INTD */
    1,	/*12 MCP750_INT_PCI_BUS2_INTA */
    1,	/*13 MCP750_INT_PCI_BUS2_INTB */
    1,	/*14 MCP750_INT_PCI_BUS2_INTC */
    1,	/*15 MCP750_INT_PCI_BUS2_INTD */
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
#ifdef TRACE_IRQ_INIT
  printk("Going to initialize openpic compliant device\n");
#endif
  /* FIXME (t.s.): we should probably setup the EOI delay by
   * passing a non-zero 'epic_freq' argument (frequency of the
   * EPIC serial interface) but I don't know the value on this
   * board (8245 SDRAM freq, IIRC)...
   */
  openpic_init(1, mcp750_openpic_initpolarities, mcp750_openpic_initsenses, 0, 16, 0 /* epic_freq */);

#ifdef TRACE_IRQ_INIT
  printk("Going to initialize the PCI/ISA bridge IRQ related setting (VIA 82C586)\n");
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
    initial_config.defaultEntry = defaultIrq;
    initial_config.irqHdlTbl	= rtemsIrq;
    initial_config.irqBase	= BSP_LOWEST_OFFSET;
    initial_config.irqPrioTbl	= irqPrioTable;

printk("Call BSP_rtems_irq_mngt_set\n");
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


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
 * Adapted for the mvme3100 BSP by T. Straumann, 2007.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <libcpu/io.h>
#include <bsp/pci.h>
#include <bsp/openpic.h>
#include <bsp/irq.h>
#include <bsp.h>
#include <bsp/vectors.h>
#include <rtems/bspIo.h>

static void nop_func(void *unused)
{
	printk("Unhandled IRQ\n");
}

static rtems_irq_connect_data     	rtemsIrq[BSP_IRQ_NUMBER];
static rtems_irq_global_settings    initial_config;
static rtems_irq_connect_data     	defaultIrq = {
  /* vectorIdex,	 hdl		, handle	, on		, off		, isOn */
  0, 			 nop_func	, NULL		, 0	, 0	, 0
};

static rtems_irq_prio irqPrioTable[BSP_IRQ_NUMBER]={
  /*
   * actual priorities for interrupt :
   *	0   means that only current interrupt is masked
   *	255 means all other interrupts are masked
   */
  /*
   * PCI Interrupts
   */
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,             /* for raven prio 0 means unactive... */
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, /* for raven prio 0 means unactive... */
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, /* for raven prio 0 means unactive... */
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
	/* We should really have a way to find the number of sources
	 * the driver will use so that the size of the polarity-array
	 * matches the driver's idea of it.
	 */
	unsigned char pol[56];
	int i;

	/* Note: The openpic driver initializes only as many
	 *       'pic-external' interrupt sources as reported
	 *       by the feature register.
	 *       The 8540's openpic supports 12 core-external
	 *       and 23 core-internal (both of these groups
	 *       are external to the PIC, i.e., 'pic-external')
	 *       interrupts but between the corresponding
	 *       banks of vector/priority registers there is
	 *       a gap leaving space for 4 (unsupported) irqs.
	 *       The driver, not knowing of this gap, would
	 *       initialize the 12 core-external sources
	 *       followed by 4 unsupported sources and 19
	 *       core-internal sources thus leaving the last
	 *       four core-internal sources uninitialized.
	 *       Luckily, the feature register reports
	 *       too many sources:
	 *          - the 4 IPI plus 4 timer plus 4 messaging
	 *            sources are included with the count
	 *          - there are unused core-internal sources 24..32
	 *            which are also supported by the pic
	 *       bringing the reported number of sources to
	 *       a count of 56 (12+32+4+4+4) which is enough
	 *       so that all pic-external sources are covered
	 *       and initialized.
	 *
	 *       NOTE: All core-internal sources are active-high.
	 *             The manual says that setting the polarity
	 *             to 'low/0' will disable the interrupt but
	 *             I found this not to be true: on the device
	 *             I tested the interrupt was asserted hard.
	 */

	/* core-external sources on the mvme3100 are active-low,
	 * core-internal sources are active high.
	 */
	for (i=0; i<BSP_EXT_IRQ_NUMBER; i++)
		pol[i]=0;
	for (i=BSP_EXT_IRQ_NUMBER; i< BSP_EXT_IRQ_NUMBER + BSP_CORE_IRQ_NUMBER; i++)
		pol[i]=1;

	openpic_init(1, pol, 0, 0, 0, 0);

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

	if (!BSP_rtems_irq_mngt_set(&initial_config)) {
		/*
		 * put something here that will show the failure...
		 */
		BSP_panic("Unable to initialize RTEMS interrupt Management!!! System locked\n");
	}
}

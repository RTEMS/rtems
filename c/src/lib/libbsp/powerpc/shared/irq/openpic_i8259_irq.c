/*
 *
 *  This file contains the i8259/openpic-specific implementation of
 *  the function described in irq.h
 *
 *  Copyright (C) 1998, 1999 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <stdlib.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq_supp.h>
#ifndef BSP_HAS_NO_VME
#include <bsp/VMEConfig.h>
#endif
#if BSP_PCI_IRQ_NUMBER > 0
#include <bsp/openpic.h>
#endif
#include <libcpu/io.h>
#include <bsp/vectors.h>
#include <stdlib.h>

#include <rtems/bspIo.h> /* for printk */

#ifndef qemu
#define RAVEN_INTR_ACK_REG 0xfeff0030
#else
#define RAVEN_INTR_ACK_REG 0xbffffff0
#endif

#if BSP_ISA_IRQ_NUMBER > 0
/*
 * pointer to the mask representing the additionnal irq vectors
 * that must be disabled when a particular entry is activated.
 * They will be dynamically computed from the priority table given
 * in BSP_rtems_irq_mngt_set();
 * CAUTION : this table is accessed directly by interrupt routine
 * 	     prologue.
 */
rtems_i8259_masks 	irq_mask_or_tbl[BSP_IRQ_NUMBER];
#endif

/*
 * default handler connected on each irq after bsp initialization
 */
static rtems_irq_connect_data	default_rtems_entry;

static rtems_irq_connect_data*		rtems_hdl_tbl;

#if BSP_ISA_IRQ_NUMBER > 0
/*
 * Check if IRQ is an ISA IRQ
 */
static inline int is_isa_irq(const rtems_irq_number irqLine)
{
  return (((int) irqLine <= BSP_ISA_IRQ_MAX_OFFSET) &&
	  ((int) irqLine >= BSP_ISA_IRQ_LOWEST_OFFSET)
	 );
}
#endif

#if BSP_PCI_IRQ_NUMBER > 0
/*
 * Check if IRQ is an OPENPIC IRQ
 */
static inline int is_pci_irq(const rtems_irq_number irqLine)
{
  return OpenPIC && (((int) irqLine <= BSP_PCI_IRQ_MAX_OFFSET) &&
	  ((int) irqLine >= BSP_PCI_IRQ_LOWEST_OFFSET)
	 );
}
#endif

/*
 * ------------------------ RTEMS Irq helper functions ----------------
 */

#if BSP_ISA_IRQ_NUMBER > 0
/*
 * Caution : this function assumes the variable "*config"
 * is already set and that the tables it contains are still valid
 * and accessible.
 */
static void compute_i8259_masks_from_prio (rtems_irq_global_settings* config)
{
  int i;
  int j;
  /*
   * Always mask at least current interrupt to prevent re-entrance
   */
  for (i=BSP_ISA_IRQ_LOWEST_OFFSET; i < BSP_ISA_IRQ_LOWEST_OFFSET + BSP_ISA_IRQ_NUMBER; i++) {
    * ((unsigned short*) &irq_mask_or_tbl[i]) = (1 << i);
    for (j = BSP_ISA_IRQ_LOWEST_OFFSET; j < BSP_ISA_IRQ_LOWEST_OFFSET + BSP_ISA_IRQ_NUMBER; j++) {
      /*
       * Mask interrupts at i8259 level that have a lower priority
       */
      if (config->irqPrioTbl [i] > config->irqPrioTbl [j]) {
	* ((unsigned short*) &irq_mask_or_tbl[i]) |= (1 << j);
      }
    }
  }
}
#endif

void
BSP_enable_irq_at_pic(const rtems_irq_number name)
{
#if BSP_ISA_IRQ_NUMBER > 0
    if (is_isa_irq(name)) {
      /*
       * Enable interrupt at PIC level
       */
      BSP_irq_enable_at_i8259s ((int) name - BSP_ISA_IRQ_LOWEST_OFFSET);
    }
#endif

#if BSP_PCI_IRQ_NUMBER > 0
    if (is_pci_irq(name)) {
      /*
       * Enable interrupt at OPENPIC level
       */
      openpic_enable_irq ((int) name - BSP_PCI_IRQ_LOWEST_OFFSET);
    }
#endif
}

int
BSP_disable_irq_at_pic(const rtems_irq_number name)
{
#if BSP_ISA_IRQ_NUMBER > 0
    if (is_isa_irq(name)) {
      /*
       * disable interrupt at PIC level
       */
      return BSP_irq_disable_at_i8259s ((int) name - BSP_ISA_IRQ_LOWEST_OFFSET);
    }
#endif
#if BSP_PCI_IRQ_NUMBER > 0
    if (is_pci_irq(name)) {
      /*
       * disable interrupt at OPENPIC level
       */
      return openpic_disable_irq ((int) name - BSP_PCI_IRQ_LOWEST_OFFSET);
    }
#endif
	return -1;
}

/*
 * RTEMS Global Interrupt Handler Management Routines
 */
int BSP_setup_the_pic(rtems_irq_global_settings* config)
{
    int i;
   /*
    * Store various code accelerators
    */
    default_rtems_entry	= config->defaultEntry;
    rtems_hdl_tbl 		= config->irqHdlTbl;

    /*
     * set up internal tables used by rtems interrupt prologue
     */

#if BSP_ISA_IRQ_NUMBER > 0
    /*
     * start with ISA IRQ
     */
    compute_i8259_masks_from_prio (config);

    for (i=BSP_ISA_IRQ_LOWEST_OFFSET; i < BSP_ISA_IRQ_LOWEST_OFFSET + BSP_ISA_IRQ_NUMBER; i++) {
      if (rtems_hdl_tbl[i].hdl != default_rtems_entry.hdl) {
         BSP_irq_enable_at_i8259s (i);
      }
      else {
         BSP_irq_disable_at_i8259s (i);
      }
    }

	if ( BSP_ISA_IRQ_NUMBER > 0 ) {
    	/*
		 * must enable slave pic anyway
		 */
		BSP_irq_enable_at_i8259s (2);
	}
#endif

#if BSP_PCI_IRQ_NUMBER > 0
	if ( ! OpenPIC )
		return 1;
    /*
     * continue with PCI IRQ
     */
    for (i=BSP_PCI_IRQ_LOWEST_OFFSET; i < BSP_PCI_IRQ_LOWEST_OFFSET + BSP_PCI_IRQ_NUMBER ; i++) {
      /*
       * Note that openpic_set_priority() sets the TASK priority of the PIC
       */
      openpic_set_source_priority(i - BSP_PCI_IRQ_LOWEST_OFFSET,
				  config->irqPrioTbl[i]);
      if (rtems_hdl_tbl[i].hdl != default_rtems_entry.hdl) {
         openpic_enable_irq ((int) i - BSP_PCI_IRQ_LOWEST_OFFSET);
      }
      else {
         openpic_disable_irq ((int) i - BSP_PCI_IRQ_LOWEST_OFFSET);
      }
    }

#ifdef BSP_PCI_ISA_BRIDGE_IRQ
   	/*
     * Must enable PCI/ISA bridge IRQ
     */
   	openpic_enable_irq (BSP_PCI_ISA_BRIDGE_IRQ);
#endif
#endif

    return 1;
}

int _BSP_vme_bridge_irq = -1;

unsigned BSP_spuriousIntr = 0;

/*
 * High level IRQ handler called from shared_raw_irq_code_entry
 */
int C_dispatch_irq_handler (BSP_Exception_frame *frame, unsigned int excNum)
{
  register unsigned int irq;
#if BSP_ISA_IRQ_NUMBER > 0
  register unsigned isaIntr;                  /* boolean */
  register unsigned oldMask = 0;	      /* old isa pic masks */
  register unsigned newMask;                  /* new isa pic masks */
#endif

  if (excNum == ASM_DEC_VECTOR) {

  	bsp_irq_dispatch_list(rtems_hdl_tbl, BSP_DECREMENTER, default_rtems_entry.hdl);

    return 0;

  }

#if BSP_PCI_IRQ_NUMBER > 0
  if ( OpenPIC ) {
    irq = openpic_irq(0);
    if (irq == OPENPIC_VEC_SPURIOUS) {
      ++BSP_spuriousIntr;
      return 0;
    }

    /* some BSPs might want to use a different numbering... */
    irq = irq - OPENPIC_VEC_SOURCE + BSP_PCI_IRQ_LOWEST_OFFSET;
  } else {
#if BSP_ISA_IRQ_NUMBER > 0
#ifdef BSP_PCI_ISA_BRIDGE_IRQ
	irq = BSP_PCI_ISA_BRIDGE_IRQ;
#else
#error "Configuration Error -- BSP with ISA + PCI IRQs MUST define BSP_PCI_ISA_BRIDGE_IRQ"
#endif
#else
	BSP_panic("MUST have an OpenPIC if BSP has PCI IRQs but no ISA IRQs");
	/* BSP_panic() never returns but the 'return' statement silences
	 * a compiler warning about 'irq' possibly being used w/o initialization.
	 */
	return -1;
#endif
  }
#endif

#if BSP_ISA_IRQ_NUMBER > 0
#ifdef BSP_PCI_ISA_BRIDGE_IRQ
#if 0 == BSP_PCI_IRQ_NUMBER 
#error "Configuration Error -- BSP w/o PCI IRQs MUST NOT define BSP_PCI_ISA_BRIDGE_IRQ"
#endif
  isaIntr = (irq == BSP_PCI_ISA_BRIDGE_IRQ);
#else
  isaIntr = 1;
#endif
  if (isaIntr)  {
    /*
     * Acknowledge and read 8259 vector
     */
    irq = (unsigned int) (*(unsigned char *) RAVEN_INTR_ACK_REG);
    /*
     * store current PIC mask
     */
    oldMask = i8259s_cache;
    newMask = oldMask | irq_mask_or_tbl [irq];
    i8259s_cache = newMask;
    outport_byte(PIC_MASTER_IMR_IO_PORT, i8259s_cache & 0xff);
    outport_byte(PIC_SLAVE_IMR_IO_PORT, ((i8259s_cache & 0xff00) >> 8));
    BSP_irq_ack_at_i8259s (irq);
#if BSP_PCI_IRQ_NUMBER > 0
	if ( OpenPIC )
      openpic_eoi(0);
#endif
  }
#endif

  /* dispatch handlers */
  bsp_irq_dispatch_list(rtems_hdl_tbl, irq, default_rtems_entry.hdl);

#if BSP_ISA_IRQ_NUMBER > 0
  if (isaIntr)  {
    i8259s_cache = oldMask;
    outport_byte(PIC_MASTER_IMR_IO_PORT, i8259s_cache & 0xff);
    outport_byte(PIC_SLAVE_IMR_IO_PORT, ((i8259s_cache & 0xff00) >> 8));
  }
  else
#endif
  {
#if BSP_PCI_IRQ_NUMBER > 0
#ifdef BSP_PCI_VME_DRIVER_DOES_EOI
	/* leave it to the VME bridge driver to do EOI, so
     * it can re-enable the openpic while handling
     * VME interrupts (-> VME priorities in software)
	 */
	if (_BSP_vme_bridge_irq != irq && OpenPIC)
#endif
    		openpic_eoi(0);
#else
	do {} while (0);
#endif
  }
  return 0;
}

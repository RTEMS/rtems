/*
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
#include <bsp/VMEConfig.h>
#include <bsp/openpic.h>
#include <libcpu/io.h>
#include <bsp/vectors.h>
#include <stdlib.h>

#include <rtems/bspIo.h> /* for printk */
#define RAVEN_INTR_ACK_REG 0xfeff0030

#ifdef BSP_PCI_ISA_BRIDGE_IRQ
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

#ifdef BSP_PCI_ISA_BRIDGE_IRQ
/*
 * Check if IRQ is an ISA IRQ
 */
static inline int is_isa_irq(const rtems_irq_number irqLine)
{
  return (((int) irqLine <= BSP_ISA_IRQ_MAX_OFFSET) &
	  ((int) irqLine >= BSP_ISA_IRQ_LOWEST_OFFSET)
	 );
}
#endif

/*
 * Check if IRQ is an OPENPIC IRQ
 */
static inline int is_pci_irq(const rtems_irq_number irqLine)
{
  return (((int) irqLine <= BSP_PCI_IRQ_MAX_OFFSET) &
	  ((int) irqLine >= BSP_PCI_IRQ_LOWEST_OFFSET)
	 );
}

/*
 * ------------------------ RTEMS Irq helper functions ----------------
 */

#ifdef BSP_PCI_ISA_BRIDGE_IRQ
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
#ifdef BSP_PCI_ISA_BRIDGE_IRQ
    if (is_isa_irq(name)) {
      /*
       * Enable interrupt at PIC level
       */
printk("ERROR BSP_irq_enable_at_i8259s Being Called for %d\n", (int)name);
      BSP_irq_enable_at_i8259s ((int) name);
    }
#endif

    if (is_pci_irq(name)) {
      /*
       * Enable interrupt at OPENPIC level
       */
printk(" openpic_enable_irq %d\n", (int)name );
       openpic_enable_irq ((int) name);
    }
}

int
BSP_disable_irq_at_pic(const rtems_irq_number name)
{
#ifdef BSP_PCI_ISA_BRIDGE_IRQ
    if (is_isa_irq(name)) {
      /*
       * disable interrupt at PIC level
       */
      return BSP_irq_disable_at_i8259s ((int) name);
    }
#endif
    if (is_pci_irq(name)) {
      /*
       * disable interrupt at OPENPIC level
       */
      return openpic_disable_irq ((int) name );
    }
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
#if 0
#ifdef BSP_PCI_ISA_BRIDGE_IRQ
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
#endif

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
         openpic_enable_irq ((int) i );
      }
      else {
         openpic_disable_irq ((int) i );
      }
    }

#ifdef BSP_PCI_ISA_BRIDGE_IRQ
	if ( BSP_ISA_IRQ_NUMBER > 0 ) {
    	/*
	     * Must enable PCI/ISA bridge IRQ
	     */
    	openpic_enable_irq (0);
	}
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
#ifdef BSP_PCI_ISA_BRIDGE_IRQ
  register unsigned isaIntr;                  /* boolean */
  register unsigned oldMask = 0;	      /* old isa pic masks */
  register unsigned newMask;                  /* new isa pic masks */
#endif

  if (excNum == ASM_DEC_VECTOR) {
/* printk("ASM_DEC_VECTOR\n"); */
  	bsp_irq_dispatch_list(rtems_hdl_tbl, BSP_DECREMENTER, default_rtems_entry.hdl);

    return 0;

  }
  irq = openpic_irq(0);
  if (irq == OPENPIC_VEC_SPURIOUS) {
/* printk("OPENPIC_VEC_SPURIOUS interrupt %d\n", OPENPIC_VEC_SPURIOUS ); */
     ++BSP_spuriousIntr;
    return 0;
  }

  /* some BSPs might want to use a different numbering... */
  irq = irq - OPENPIC_VEC_SOURCE + BSP_PCI_IRQ_LOWEST_OFFSET;
/* printk("OpenPic Irq: %d\n", irq); */

#ifdef BSP_PCI_ISA_BRIDGE_IRQ
  isaIntr = (irq == BSP_PCI_ISA_BRIDGE_IRQ);
  if (isaIntr)  {
/* printk("BSP_PCI_ISA_BRIDGE_IRQ\n"); */
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
    openpic_eoi(0);
  }
#endif


  /* dispatch handlers */
/* printk("dispatch\n"); */
irq -=16;
  bsp_irq_dispatch_list(rtems_hdl_tbl, irq, default_rtems_entry.hdl);
/* printk("Back from dispatch\n"); */

#ifdef BSP_PCI_ISA_BRIDGE_IRQ
  if (isaIntr)  {\
    i8259s_cache = oldMask;
    outport_byte(PIC_MASTER_IMR_IO_PORT, i8259s_cache & 0xff);
    outport_byte(PIC_SLAVE_IMR_IO_PORT, ((i8259s_cache & 0xff00) >> 8));
  }
  else
#endif
  {
#ifdef BSP_PCI_VME_DRIVER_DOES_EOI
	/* leave it to the VME bridge driver to do EOI, so
     * it can re-enable the openpic while handling
     * VME interrupts (-> VME priorities in software)
	 */
	if (_BSP_vme_bridge_irq != irq)
#endif
    		openpic_eoi(0);
  }
  return 0;
}

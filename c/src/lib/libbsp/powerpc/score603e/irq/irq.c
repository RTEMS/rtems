/*
 *  This file contains the implementation of the function described in irq.h
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
#include <bsp/VME.h>
#include <rtems/score/apiext.h>  /* for post ISR signal processing */
#include <libcpu/io.h>
#include <bsp/vectors.h>
#include <stdlib.h>
#include <rtems/bspIo.h> /* for printk */

/*
 * default handler connected on each irq after bsp initialization
 */
static rtems_irq_connect_data	default_rtems_entry;

/*
 * location used to store initial tables used for interrupt
 * management.
 */
static rtems_irq_global_settings* 	internal_config;
static rtems_irq_connect_data*		rtems_hdl_tbl;

/*
 * Check if IRQ is an ISA IRQ
 */
static inline int is_isa_irq(const rtems_irq_number irqLine)
{
  return (((int) irqLine <= BSP_ISA_IRQ_MAX_OFFSET) &
	  ((int) irqLine >= BSP_ISA_IRQ_LOWEST_OFFSET)
	 );
}

/*
 * Check if IRQ is an pci IRQ
 */
static inline int is_pci_irq(const rtems_irq_number irqLine)
{
  return (((int) irqLine <= BSP_PCI_IRQ_MAX_OFFSET) &
	  ((int) irqLine >= BSP_PCI_IRQ_LOWEST_OFFSET)
	 );
}

/*
 * Check if IRQ is a Processor IRQ
 */
static inline int is_processor_irq(const rtems_irq_number irqLine)
{
  return (((int) irqLine <= BSP_PROCESSOR_IRQ_MAX_OFFSET) &
	  ((int) irqLine >= BSP_PROCESSOR_IRQ_LOWEST_OFFSET)
	 );
}

/*
 * ------------------------ RTEMS Irq helper functions ----------------
 */

/*
 * This function check that the value given for the irq line
 * is valid.
 */

static int isValidInterrupt(int irq)
{
  if ( (irq < BSP_LOWEST_OFFSET) || (irq > BSP_MAX_OFFSET))
    return 0;
  return 1;
}

/*
 * ------------------------ RTEMS Shared Irq Handler Mngt Routines ----------------
 */
int BSP_install_rtems_shared_irq_handler  (const rtems_irq_connect_data* irq)
{
    rtems_interrupt_level   level;
    rtems_irq_connect_data* vchain;

printk(" BSP_install_rtems_shared_irq_handler %d\n", irq->name );

    if (!isValidInterrupt(irq->name)) {
      printk("Invalid interrupt vector %d\n",irq->name);
      return 0;
    }

    rtems_interrupt_disable(level);

    if ( (int)rtems_hdl_tbl[irq->name].next_handler  == -1 ) {
      rtems_interrupt_enable(level);
      printk("IRQ vector %d already connected to an unshared handler\n",irq->name);
      return 0;
    }

     vchain = (rtems_irq_connect_data*)malloc(sizeof(rtems_irq_connect_data));

    /* save off topmost handler */
    vchain[0]= rtems_hdl_tbl[irq->name];

    /*
     * store the data provided by user
     */
    rtems_hdl_tbl[irq->name] = *irq;

    /* link chain to new topmost handler */
    rtems_hdl_tbl[irq->name].next_handler = (void *)vchain;

    /*
     * XXX FIX ME
     */
    if (is_pci_irq(irq->name)) {
    }

    if (is_processor_irq(irq->name)) {
      /*
       * Enable exception at processor level
       */
    }
    /*
     * Enable interrupt on device
     */
	if (irq->on)
    	irq->on(irq);

    rtems_interrupt_enable(level);

    return 1;
}

/*
 * This function disables a given XXX interrupt
 */
rtems_status_code bsp_interrupt_vector_disable( rtems_vector_number irqLine)
{
  /* XXX FIX ME!!!! */

  printk("bsp_interrupt_vector_disable: 0x%x\n", irqLine );
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_enable( rtems_vector_number irqLine)
{
  /* XXX FIX ME!!!! */
  printk("bsp_interrupt_vector_enable: 0x%x\n", irqLine );

  return RTEMS_SUCCESSFUL;
}



/*
 * ------------------------ RTEMS Single Irq Handler Mngt Routines ----------------
 */

int BSP_install_rtems_irq_handler  (const rtems_irq_connect_data* irq)
{
    rtems_interrupt_level       level;

printk(" BSP_install_rtems_irq_handler %d\n", irq->name );

    if (!isValidInterrupt(irq->name)) {
      printk("Invalid interrupt vector %d\n",irq->name);
      return 0;
    }
    /*
     * Check if default handler is actually connected. If not issue an error.
     * You must first get the current handler via i386_get_current_idt_entry
     * and then disconnect it using i386_delete_idt_entry.
     * RATIONALE : to always have the same transition by forcing the user
     * to get the previous handler before accepting to disconnect.
     */
    rtems_interrupt_disable(level);
    if (rtems_hdl_tbl[irq->name].hdl != default_rtems_entry.hdl) {
      rtems_interrupt_enable(level);
      printk("IRQ vector %d already connected\n",irq->name);
      return 0;
    }

    /*
     * store the data provided by user
     */
    rtems_hdl_tbl[irq->name] = *irq;
    rtems_hdl_tbl[irq->name].next_handler = (void *)-1;

    /* XXX -FIX ME !! */
    if (is_pci_irq(irq->name)) {
      /*
       * Enable interrupt
       */
      printk("is_pci_irq = TRUE - FIX THIS!\n");
    }

    if (is_processor_irq(irq->name)) {
      /*
       * Enable exception at processor level
       */
      printk("is_processor_irq = TRUE : Fix This\n");
    }

    /*
     * Enable interrupt on device
     */
    if (irq->on) {
        printk("Call 0x%x\n", irq->on );
    	irq->on(irq);
    }

    rtems_interrupt_enable(level);

    return 1;
}

int BSP_get_current_rtems_irq_handler	(rtems_irq_connect_data* irq)
{
    rtems_interrupt_level       level;

printk(" BSP_get_current_rtems_irq_handler %d\n", irq->name );
    if (!isValidInterrupt(irq->name)) {
      return 0;
    }
    rtems_interrupt_disable(level);
      *irq = rtems_hdl_tbl[irq->name];
    rtems_interrupt_enable(level);
    return 1;
}

int BSP_remove_rtems_irq_handler  (const rtems_irq_connect_data* irq)
{
    rtems_irq_connect_data *pchain= NULL, *vchain = NULL;
    rtems_interrupt_level       level;

printk(" BSP_remove_rtems_irq_handler %d\n", irq->name );
    if (!isValidInterrupt(irq->name)) {
      return 0;
    }
    /*
     * Check if default handler is actually connected. If not issue an error.
     * You must first get the current handler via i386_get_current_idt_entry
     * and then disconnect it using i386_delete_idt_entry.
     * RATIONALE : to always have the same transition by forcing the user
     * to get the previous handler before accepting to disconnect.
     */
    rtems_interrupt_disable(level);
    if (rtems_hdl_tbl[irq->name].hdl != irq->hdl) {
      rtems_interrupt_enable(level);
      return 0;
    }

    if( (int)rtems_hdl_tbl[irq->name].next_handler != -1 )
    {
       int found = 0;

       for( (pchain= NULL, vchain = &rtems_hdl_tbl[irq->name]);
            (vchain->hdl != default_rtems_entry.hdl);
            (pchain= vchain, vchain = (rtems_irq_connect_data*)vchain->next_handler) )
       {
          if( vchain->hdl == irq->hdl )
          {
             found= -1; break;
          }
       }

       if( !found )
       {
          rtems_interrupt_enable(level);
          return 0;
       }
    }
    else
    {
       if (rtems_hdl_tbl[irq->name].hdl != irq->hdl)
       {
          rtems_interrupt_enable(level);
         return 0;
       }
    }

    /* XXX - FIX ME !! */
    if (is_pci_irq(irq->name)) {
      /*
       * disable interrupt
       */
    }
    if (is_processor_irq(irq->name)) {
      /*
       * disable exception at processor level
       */
    }

    /*
     * Disable interrupt on device
     */
	if (irq->off)
    	irq->off(irq);

    /*
     * restore the default irq value
     */
    if( !vchain )
    {
       /* single handler vector... */
       rtems_hdl_tbl[irq->name] = default_rtems_entry;
    }
    else
    {
       if( pchain )
       {
          /* non-first handler being removed */
          pchain->next_handler = vchain->next_handler;
       }
       else
       {
          /* first handler isn't malloc'ed, so just overwrite it.  Since
          the contents of vchain are being struct copied, vchain itself
          goes away */
          rtems_hdl_tbl[irq->name]= *vchain;
       }
       free(vchain);
    }

    rtems_interrupt_enable(level);

    return 1;
}

/*
 * RTEMS Global Interrupt Handler Management Routines
 */

int BSP_rtems_irq_mngt_set(rtems_irq_global_settings* config)
{
    int                    i;
    rtems_interrupt_level  level;

    /*
     * Store various code accelerators
     */
    internal_config 		= config;
    default_rtems_entry 	= config->defaultEntry;
    rtems_hdl_tbl 		= config->irqHdlTbl;

printk(" BSP_rtems_irq_mngt_set\n");

    rtems_interrupt_disable(level);
    /*
     * set up internal tables used by rtems interrupt prologue
     */

    /*
     *  XXX - FIX ME !!!
     */
    for (i=BSP_PCI_IRQ_LOWEST_OFFSET; i < BSP_PCI_IRQ_LOWEST_OFFSET + BSP_PCI_IRQ_NUMBER ; i++) {
      if (rtems_hdl_tbl[i].hdl != default_rtems_entry.hdl) {
         {
            rtems_irq_connect_data* vchain;
            for( vchain = &rtems_hdl_tbl[i];
                 ((int)vchain != -1 && vchain->hdl != default_rtems_entry.hdl);
                 vchain = (rtems_irq_connect_data*)vchain->next_handler )
            {
			  if (vchain->on)
               	vchain->on(vchain);
            }
         }

      }
      else {
         /* if (rtems_hdl_tbl[i].off) rtems_hdl_tbl[i].off(&rtems_hdl_tbl[i]); */
         {
            rtems_irq_connect_data* vchain;
            for( vchain = &rtems_hdl_tbl[i];
                 ((int)vchain != -1 && vchain->hdl != default_rtems_entry.hdl);
                 vchain = (rtems_irq_connect_data*)vchain->next_handler )
            {
			  if (vchain->off)
               vchain->off(vchain);
            }
         }
      }
    }
    /*
     * finish with Processor exceptions handled like IRQ
     */
    for (i=BSP_PROCESSOR_IRQ_LOWEST_OFFSET; i < BSP_PROCESSOR_IRQ_LOWEST_OFFSET+BSP_PROCESSOR_IRQ_NUMBER; i++){
      if (rtems_hdl_tbl[i].hdl != default_rtems_entry.hdl) {
         {
            rtems_irq_connect_data* vchain;
            for( vchain = &rtems_hdl_tbl[i];
                 ((int)vchain != -1 && vchain->hdl != default_rtems_entry.hdl);
                 vchain = (rtems_irq_connect_data*)vchain->next_handler )
            {
			  if (vchain->on)
               vchain->on(vchain);
            }
         }

      }
      else {
         {
            rtems_irq_connect_data* vchain;
            for( vchain = &rtems_hdl_tbl[i];
                 ((int)vchain != -1 && vchain->hdl != default_rtems_entry.hdl);
                 vchain = (rtems_irq_connect_data*)vchain->next_handler )
            {
			  if (vchain->off)
               vchain->off(vchain);
            }
         }

      }
    }
    rtems_interrupt_enable(level);
    return 1;
}

int BSP_rtems_irq_mngt_get(rtems_irq_global_settings** config)
{
    *config = internal_config;
    return 0;
}

unsigned BSP_spuriousIntr = 0;

/*
 * High level IRQ handler called from shared_raw_irq_code_entry
 */
int C_dispatch_irq_handler (CPU_Interrupt_frame *frame, unsigned int excNum)
{
  register unsigned int irq;
  register unsigned msr;
  register unsigned new_msr;

  if (excNum == ASM_DEC_VECTOR) {
    _CPU_MSR_GET(msr);
    new_msr = msr | MSR_EE;
    _CPU_MSR_SET(new_msr);

    rtems_hdl_tbl[BSP_DECREMENTER].hdl(rtems_hdl_tbl[BSP_DECREMENTER].handle);

    _CPU_MSR_SET(msr);
    return 0;

  }

  irq = read_and_clear_irq();
  _CPU_MSR_GET(msr);
  new_msr = msr | MSR_EE;
  _CPU_MSR_SET(new_msr);

  /* rtems_hdl_tbl[irq].hdl(rtems_hdl_tbl[irq].handle); */
  {
     rtems_irq_connect_data* vchain;
     for( vchain = &rtems_hdl_tbl[irq];
          ((int)vchain != -1 && vchain->hdl != default_rtems_entry.hdl);
          vchain = (rtems_irq_connect_data*)vchain->next_handler )
     {
        vchain->hdl(vchain->handle);
     }
  }

  _CPU_MSR_SET(msr);

  return 0;
}

rtems_status_code bsp_interrupt_facility_initialize(void)
{
  /* Install exception handler */
  if (ppc_exc_set_handler( ASM_EXT_VECTOR, C_dispatch_irq_handler)) {
    return RTEMS_IO_ERROR;
  }
  if (ppc_exc_set_handler( ASM_DEC_VECTOR, C_dispatch_irq_handler)) {
    return RTEMS_IO_ERROR;
  }
  if (ppc_exc_set_handler( ASM_E300_SYSMGMT_VECTOR, C_dispatch_irq_handler)) {
    return RTEMS_IO_ERROR;
  }

  return RTEMS_SUCCESSFUL;
}

void bsp_interrupt_handler_default( rtems_vector_number vector )
{
  if (vector != BSP_DECREMENTER) {
    printk( "Spurious interrupt: 0x%08x\n", vector);
  }
}


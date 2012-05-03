/*
 *
 *  This file contains the PIC-independent implementation of the functions described in irq.h
 *
 *  Copyright (C) 1998, 1999 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <stdlib.h>

#include <rtems.h>
#include "irq_supp.h"
#include <rtems/score/apiext.h>  /* for post ISR signal processing */
#include <bsp/vectors.h>
#include <stdlib.h>
#include <rtems/bspIo.h> /* for printk */
#include <libcpu/spr.h>

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


SPR_RW(BOOKE_TSR)
SPR_RW(PPC405_TSR)

/* legacy mode for bookE DEC exception;
 * to avoid the double layer of function calls
 * (dec_handler_bookE -> C_dispatch_irq_handler -> user handler)
 * it is preferrable for the user to hook the DEC
 * exception directly.
 * However, the legacy mode works with less modifications
 * of user code.
 */
int C_dispatch_dec_handler_bookE (BSP_Exception_frame *frame, unsigned int excNum)
{
	/* clear interrupt; we must do this
	 * before C_dispatch_irq_handler()
	 * re-enables MSR_EE.
	 * Note that PPC405 uses a different SPR# for TSR
	 */
	if ( ppc_cpu_is_bookE()==PPC_BOOKE_405)
		_write_PPC405_TSR( BOOKE_TSR_DIS );
	else
		_write_BOOKE_TSR( BOOKE_TSR_DIS );
	return C_dispatch_irq_handler(frame, ASM_DEC_VECTOR);
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
  if ( (irq < internal_config->irqBase) || (irq >= internal_config->irqBase + internal_config->irqNb))
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

    if (!isValidInterrupt(irq->name)) {
      printk("Invalid interrupt vector %d\n",irq->name);
      return 0;
    }

	/* pre-allocate memory outside of critical section */
    vchain = (rtems_irq_connect_data*)malloc(sizeof(rtems_irq_connect_data));

    rtems_interrupt_disable(level);

    if ( (int)rtems_hdl_tbl[irq->name].next_handler  == -1 ) {
      rtems_interrupt_enable(level);
      printk("IRQ vector %d already connected to an unshared handler\n",irq->name);
	  free(vchain);
      return 0;
    }

    /* save off topmost handler */
    vchain[0]= rtems_hdl_tbl[irq->name];

    /*
     * store the data provided by user
     */
    rtems_hdl_tbl[irq->name] = *irq;

    /* link chain to new topmost handler */
    rtems_hdl_tbl[irq->name].next_handler = (void *)vchain;

	/*
	 * enable_irq_at_pic is supposed to ignore
	 * requests to disable interrupts outside
	 * of the range handled by the PIC
	 */
	BSP_enable_irq_at_pic(irq->name);

    /*
     * Enable interrupt on device
     */
	if (irq->on)
    	irq->on(irq);

    rtems_interrupt_enable(level);

    return 1;
}

/*
 * ------------------------ RTEMS Single Irq Handler Mngt Routines ----------------
 */

int BSP_install_rtems_irq_handler  (const rtems_irq_connect_data* irq)
{
    rtems_interrupt_level  level;

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

	/*
	 * enable_irq_at_pic is supposed to ignore
	 * requests to disable interrupts outside
	 * of the range handled by the PIC
	 */
	BSP_enable_irq_at_pic(irq->name);

    /*
     * Enable interrupt on device
     */
	if (irq->on)
    	irq->on(irq);

    rtems_interrupt_enable(level);

    return 1;
}

int BSP_get_current_rtems_irq_handler	(rtems_irq_connect_data* irq)
{
    rtems_interrupt_level       level;

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
    rtems_interrupt_level   level;

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
          vchain = vchain->next_handler;
          rtems_hdl_tbl[irq->name]= *vchain;
       }
    }

	/* Only disable at PIC if we removed the last handler */
	if ( rtems_hdl_tbl[irq->name].hdl == default_rtems_entry.hdl ) {
		/*
		 * disable_irq_at_pic is supposed to ignore
		 * requests to disable interrupts outside
		 * of the range handled by the PIC;
		 */
		BSP_disable_irq_at_pic(irq->name);
	}

    rtems_interrupt_enable(level);

    free(vchain);

    return 1;
}

/*
 * Less cumbersome, alternate entry points;
 * RETURNS: more traditional, 0 on success, nonzero on error
 */

static int doit(
	int (*p)(const rtems_irq_connect_data*),
	rtems_irq_number n,
	rtems_irq_hdl hdl,
	rtems_irq_hdl_param prm)
{
rtems_irq_connect_data 	xx;
	xx.name   = n;
	xx.hdl    = hdl;
	xx.handle = prm;
	xx.on     = 0;
	xx.off    = 0;
	xx.isOn   = 0;
	return ! p(&xx);
}

int BSP_rtems_int_connect(rtems_irq_number n, rtems_irq_hdl hdl, rtems_irq_hdl_param p)
{
	return doit(BSP_install_rtems_shared_irq_handler, n, hdl, p);
}

int BSP_rtems_int_disconnect(rtems_irq_number n, rtems_irq_hdl hdl, rtems_irq_hdl_param p)
{
	return doit(BSP_remove_rtems_irq_handler, n, hdl, p);
}


/*
 * RTEMS Global Interrupt Handler Management Routines
 */

int BSP_rtems_irq_mngt_set(rtems_irq_global_settings* config)
{
    int                           i;
    rtems_interrupt_level         level;
    rtems_irq_connect_data*       vchain;

    /*
     * Store various code accelerators
     */
    internal_config 		= config;
    default_rtems_entry 	= config->defaultEntry;
    rtems_hdl_tbl 		= config->irqHdlTbl;

    rtems_interrupt_disable(level);

	if ( !BSP_setup_the_pic(config) ) {
		printk("PIC setup failed; leaving IRQs OFF\n");
		return 0;
	}

	for ( i = config->irqBase; i < config->irqBase + config->irqNb; i++ ) {
		for( vchain = &rtems_hdl_tbl[i];
		     ((int)vchain != -1 && vchain->hdl != default_rtems_entry.hdl);
		     vchain = (rtems_irq_connect_data*)vchain->next_handler )
		{
			if (vchain->on)
              vchain->on(vchain);
		}
		if ( vchain != &rtems_hdl_tbl[i] ) {
			/* at least one handler registered */
			BSP_enable_irq_at_pic(i);
		} else {
/* Do NOT disable; there might be boards with cascaded
 * interrupt controllers where the BSP (incorrectly) does
 * not ignore the cascaded interrupts in BSP_disable_irq_at_pic()!
 * Instead, we rely on BSP_setup_the_pic() for a good
 * initial configuration.
 *
			BSP_disable_irq_at_pic(i);
 */
		}
	}

    rtems_interrupt_enable(level);

	{
			ppc_exc_set_handler(ASM_EXT_VECTOR, C_dispatch_irq_handler);

			if ( ppc_cpu_is_bookE() ) {
				/* bookE decrementer interrupt needs to be cleared BEFORE
				 * dispatching the user ISR (because the user ISR is called
				 * with EE enabled)
				 * We do this so that existing DEC handlers can be used
				 * with minor modifications.
				 */
				ppc_exc_set_handler(ASM_BOOKE_DEC_VECTOR, C_dispatch_dec_handler_bookE);
			} else {
				ppc_exc_set_handler(ASM_DEC_VECTOR, C_dispatch_irq_handler);
			}
	}
    return 1;
}

int BSP_rtems_irq_mngt_get(rtems_irq_global_settings** config)
{
    *config = internal_config;
    return 0;
}

/*
 *
 *  This file contains the implementation of the function described in irq.h
 *
 *  Copyright (C) 1998, 1999 valette@crf.canon.fr
 *
 *  Modified for mpc8260 Andy Dachs <a.dachs@sstl.co.uk>
 *  Surrey Satellite Technology Limited, 2000
+  *  	21/4/2002 Added support for nested interrupts and improved
+  *  	masking operations.  Now we compute priority mask based
+  * 		on table in irq_init.c
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
  
#include <bsp.h>
#include <bsp/irq.h>
#include <rtems.h>
#include <rtems/score/apiext.h>
#include <rtems/bspIo.h>
#include <libcpu/raw_exception.h>
#include <bsp/vectors.h>
#include <mpc8260.h>

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
 * Check if symbolic IRQ name is an CPM IRQ
 */
static inline int is_cpm_irq(const rtems_irq_symbolic_name irqLine)
{
	return (((int) irqLine <= BSP_CPM_IRQ_MAX_OFFSET) &
			((int) irqLine >= BSP_CPM_IRQ_LOWEST_OFFSET)
	);
}

/*
 * Check if symbolic IRQ name is a Processor IRQ
 */
static inline int is_processor_irq(const rtems_irq_symbolic_name irqLine)
{
	return (((int) irqLine <= BSP_PROCESSOR_IRQ_MAX_OFFSET) &
			((int) irqLine >= BSP_PROCESSOR_IRQ_LOWEST_OFFSET)
	);
}

typedef struct {
	rtems_unsigned32 mask_h;	/* mask for sipnr_h and simr_h */
	rtems_unsigned32 mask_l;	/* mask for sipnr_l and simr_l */
	rtems_unsigned32 priority_h;  /* mask this and lower priority ints */
	rtems_unsigned32 priority_l;
} m82xxIrqMasks_t;

/*
 *  Mask fields should have a '1' in the bit position for that
 *  interrupt.
 *	Priority masks calculated later based on priority table
 */

static m82xxIrqMasks_t SIU_MaskBit[BSP_CPM_IRQ_NUMBER] =
{
	{ 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* err */
	{ 0x00000000, 0x00008000, 0x00000000, 0x00000000 }, /* i2c */
	{ 0x00000000, 0x00004000, 0x00000000, 0x00000000 }, /* spi */
	{ 0x00000000, 0x00002000, 0x00000000, 0x00000000 }, /* rtt */
	{ 0x00000000, 0x00001000, 0x00000000, 0x00000000 }, /* smc1 */
	{ 0x00000000, 0x00000800, 0x00000000, 0x00000000 }, /* smc2 */
	{ 0x00000000, 0x00000400, 0x00000000, 0x00000000 }, /* idma1 */
	{ 0x00000000, 0x00000200, 0x00000000, 0x00000000 }, /* idma2 */
	{ 0x00000000, 0x00000100, 0x00000000, 0x00000000 }, /* idma3 */
	{ 0x00000000, 0x00000080, 0x00000000, 0x00000000 }, /* idma4 */
	{ 0x00000000, 0x00000040, 0x00000000, 0x00000000 }, /* sdma */
	{ 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
	{ 0x00000000, 0x00000010, 0x00000000, 0x00000000 }, /* tmr1 */
	{ 0x00000000, 0x00000008, 0x00000000, 0x00000000 }, /* tmr2 */
	{ 0x00000000, 0x00000004, 0x00000000, 0x00000000 }, /* tmr3 */
	{ 0x00000000, 0x00000002, 0x00000000, 0x00000000 }, /* tmr4 */
	{ 0x00000004, 0x00000000, 0x00000000, 0x00000000 }, /* tmcnt */
	{ 0x00000002, 0x00000000, 0x00000000, 0x00000000 }, /* pit */
	{ 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
	{ 0x00004000, 0x00000000, 0x00000000, 0x00000000 }, /* irq1 */
	{ 0x00002000, 0x00000000, 0x00000000, 0x00000000 }, /* irq2 */
	{ 0x00001000, 0x00000000, 0x00000000, 0x00000000 }, /* irq3 */
	{ 0x00000800, 0x00000000, 0x00000000, 0x00000000 }, /* irq4 */
	{ 0x00000400, 0x00000000, 0x00000000, 0x00000000 }, /* irq5 */
	{ 0x00000200, 0x00000000, 0x00000000, 0x00000000 }, /* irq6 */
	{ 0x00000100, 0x00000000, 0x00000000, 0x00000000 }, /* irq7 */
	{ 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
	{ 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
	{ 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
	{ 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
	{ 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
	{ 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
	{ 0x00000000, 0x80000000, 0x00000000, 0x00000000 }, /* fcc1 */
	{ 0x00000000, 0x40000000, 0x00000000, 0x00000000 }, /* fcc2 */
	{ 0x00000000, 0x20000000, 0x00000000, 0x00000000 }, /* fcc3 */
	{ 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
	{ 0x00000000, 0x08000000, 0x00000000, 0x00000000 }, /* mcc1 */
	{ 0x00000000, 0x04000000, 0x00000000, 0x00000000 }, /* mcc2 */
	{ 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
	{ 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
	{ 0x00000000, 0x00800000, 0x00000000, 0x00000000 }, /* scc1 */
	{ 0x00000000, 0x00400000, 0x00000000, 0x00000000 }, /* scc2 */
	{ 0x00000000, 0x00200000, 0x00000000, 0x00000000 }, /* scc3 */
	{ 0x00000000, 0x00100000, 0x00000000, 0x00000000 }, /* scc4 */
	{ 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
	{ 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
	{ 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
	{ 0x00000000, 0x00000000, 0x00000000, 0x00000000 }, /* reserved */
	{ 0x00010000, 0x00000000, 0x00000000, 0x00000000 }, /* pc15 */
	{ 0x00020000, 0x00000000, 0x00000000, 0x00000000 }, /* pc14 */
	{ 0x00040000, 0x00000000, 0x00000000, 0x00000000 }, /* pc13 */
	{ 0x00080000, 0x00000000, 0x00000000, 0x00000000 }, /* pc12 */
	{ 0x00100000, 0x00000000, 0x00000000, 0x00000000 }, /* pc11 */
	{ 0x00200000, 0x00000000, 0x00000000, 0x00000000 }, /* pc10 */
	{ 0x00400000, 0x00000000, 0x00000000, 0x00000000 }, /* pc9 */
	{ 0x00800000, 0x00000000, 0x00000000, 0x00000000 }, /* pc8 */
	{ 0x01000000, 0x00000000, 0x00000000, 0x00000000 }, /* pc7 */
	{ 0x02000000, 0x00000000, 0x00000000, 0x00000000 }, /* pc6 */
	{ 0x04000000, 0x00000000, 0x00000000, 0x00000000 }, /* pc5 */
	{ 0x08000000, 0x00000000, 0x00000000, 0x00000000 }, /* pc4 */
	{ 0x10000000, 0x00000000, 0x00000000, 0x00000000 }, /* pc3 */
	{ 0x20000000, 0x00000000, 0x00000000, 0x00000000 }, /* pc2 */
	{ 0x40000000, 0x00000000, 0x00000000, 0x00000000 }, /* pc1 */
	{ 0x80000000, 0x00000000, 0x00000000, 0x00000000 }, /* pc0 */

};



void dump_irq_masks(void )
{
	int i;
	for( i=0; i<BSP_CPM_IRQ_NUMBER;i++ )	
	{
		printk( "%04d: %08X %08X\n",
	 		i,
	 		SIU_MaskBit[i].priority_h,
		 	SIU_MaskBit[i].priority_l
		);
	}
}

/*
 * ------------------------ RTEMS Irq helper functions ----------------
 */

/*
 * Caution : this function assumes the variable "internal_config"
 * is already set and that the tables it contains are still valid
 * and accessible.
 */
static void compute_SIU_IvectMask_from_prio ()
{
	/*
	 * The actual masks defined
	 * correspond to the priorities defined
	 * for the SIU in irq_init.c.
	 */
	
	 int i,j;
	
	 for( i=0; i<BSP_CPM_IRQ_NUMBER; i++ )
	 {
	 	for( j=0;j<BSP_CPM_IRQ_NUMBER; j++ )
		 	if( internal_config->irqPrioTbl[j] < internal_config->irqPrioTbl[i] )
		 	{
				SIU_MaskBit[i].priority_h |= SIU_MaskBit[j].mask_h;
				SIU_MaskBit[i].priority_l |= SIU_MaskBit[j].mask_l;
			}		 		
	 }
	
}

/*
 * This function check that the value given for the irq line
 * is valid.
 */

static int isValidInterrupt(int irq)
{
	if ( (irq < BSP_LOWEST_OFFSET) || (irq > BSP_MAX_OFFSET) )
		return 0;
	return 1;
}

int BSP_irq_enable_at_cpm(const rtems_irq_symbolic_name irqLine)
{
	int cpm_irq_index;

	if (!is_cpm_irq(irqLine))
		return 1;

	cpm_irq_index = ((int) (irqLine) - BSP_CPM_IRQ_LOWEST_OFFSET);

	m8260.simr_h |= SIU_MaskBit[cpm_irq_index].mask_h;
	m8260.simr_l |= SIU_MaskBit[cpm_irq_index].mask_l;

	return 0;
}

int BSP_irq_disable_at_cpm(const rtems_irq_symbolic_name irqLine)
{
	int cpm_irq_index;
  
	if (!is_cpm_irq(irqLine))
		return 1;
  
	cpm_irq_index = ((int) (irqLine) - BSP_CPM_IRQ_LOWEST_OFFSET);

	m8260.simr_h &= ~(SIU_MaskBit[cpm_irq_index].mask_h);
	m8260.simr_l &= ~(SIU_MaskBit[cpm_irq_index].mask_l);


	return 0;
}

int BSP_irq_enabled_at_cpm(const rtems_irq_symbolic_name irqLine)
{
	int cpm_irq_index;
  
	if (!is_cpm_irq(irqLine))
		return 0;
  
	cpm_irq_index = ((int) (irqLine) - BSP_CPM_IRQ_LOWEST_OFFSET);

	return ((m8260.simr_h & SIU_MaskBit[cpm_irq_index].mask_h) ||
		    (m8260.simr_l & SIU_MaskBit[cpm_irq_index].mask_l));
}


/*
 * ------------------------ RTEMS Single Irq Handler Mngt Routines ----------------
 */

int BSP_install_rtems_irq_handler  (const rtems_irq_connect_data* irq)
{
	unsigned int level;

	if (!isValidInterrupt(irq->name)) {
		printk( "not a valid intr\n" ) ;
		return 0;
	}
	/*
	 * Check if default handler is actually connected. If not issue an error.
	 * You must first get the current handler via i386_get_current_idt_entry
	 * and then disconnect it using i386_delete_idt_entry.
	 * RATIONALE : to always have the same transition by forcing the user
	 * to get the previous handler before accepting to disconnect.
	 */
	if (rtems_hdl_tbl[irq->name].hdl != default_rtems_entry.hdl) {
		printk( "Default handler not there\n" );
		return 0;
	}

	_CPU_ISR_Disable(level);

	/*
	 * store the data provided by user
	 */
	rtems_hdl_tbl[irq->name] = *irq;
	
	if (is_cpm_irq(irq->name)) {
	    /*
	     * Enable interrupt at PIC level
	     */
	    BSP_irq_enable_at_cpm (irq->name);
	}

#if 0
	if (is_processor_irq(irq->name)) {
		/*
		 * Should Enable exception at processor level but not needed.  Will restore
		 * EE flags at the end of the routine anyway.
		 */
	}
#endif

	/*
	 * Enable interrupt on device
	 */
	irq->on(irq);

	_CPU_ISR_Enable(level);

	/*
	    printk( "Enabled\n" );
	*/
	return 1;
}


int BSP_get_current_rtems_irq_handler	(rtems_irq_connect_data* irq)
{
	if (!isValidInterrupt(irq->name)) {
		return 0;
	}
	*irq = rtems_hdl_tbl[irq->name];
	return 1;
}

int BSP_remove_rtems_irq_handler  (const rtems_irq_connect_data* irq)
{
	unsigned int level;
  
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
	if (rtems_hdl_tbl[irq->name].hdl != irq->hdl) {
	  return 0;
	}
	_CPU_ISR_Disable(level);
	
	if (is_cpm_irq(irq->name)) {
	  /*
	   * disable interrupt at PIC level
	   */
	  BSP_irq_disable_at_cpm (irq->name);
	}
	
	if (is_processor_irq(irq->name)) {
	  /*
	   * disable exception at processor level
	   */
	}    
	
	/*
	 * Disable interrupt on device
	 */
	irq->off(irq);
	
	/*
	 * restore the default irq value
	 */
	rtems_hdl_tbl[irq->name] = default_rtems_entry;
	
	_CPU_ISR_Enable(level);
	
	return 1;
}

/*
 * ------------------------ RTEMS Global Irq Handler Mngt Routines ----------------
 */

int BSP_rtems_irq_mngt_set(rtems_irq_global_settings* config)
{
	int i;
	unsigned int level;
	/*
	 * Store various code accelerators
	 */
	internal_config			= config;
	default_rtems_entry		= config->defaultEntry;
	rtems_hdl_tbl       	= config->irqHdlTbl;

	/* Fill in priority masks */
	compute_SIU_IvectMask_from_prio();
	
	_CPU_ISR_Disable(level);
	/*
	 * start with CPM IRQ
	 */
	for (i=BSP_CPM_IRQ_LOWEST_OFFSET; i < BSP_CPM_IRQ_LOWEST_OFFSET + BSP_CPM_IRQ_NUMBER ; i++) {
		if (rtems_hdl_tbl[i].hdl != default_rtems_entry.hdl) {
			BSP_irq_enable_at_cpm (i);
			rtems_hdl_tbl[i].on(&rtems_hdl_tbl[i]);
		} else {
			rtems_hdl_tbl[i].off(&rtems_hdl_tbl[i]);
			BSP_irq_disable_at_cpm (i);
		}
	}

	/*
	 * finish with Processor exceptions handled like IRQ
	 */
	for (i=BSP_PROCESSOR_IRQ_LOWEST_OFFSET; i < BSP_PROCESSOR_IRQ_LOWEST_OFFSET + BSP_PROCESSOR_IRQ_NUMBER; i++) {
		if (rtems_hdl_tbl[i].hdl != default_rtems_entry.hdl) {
			rtems_hdl_tbl[i].on(&rtems_hdl_tbl[i]);
		} else {
			rtems_hdl_tbl[i].off(&rtems_hdl_tbl[i]);
		}
	}

	_CPU_ISR_Enable(level);
	return 1;
}

int BSP_rtems_irq_mngt_get(rtems_irq_global_settings** config)
{
	*config = internal_config;
	return 0;
}

#ifdef DISPATCH_HANDLER_STAT
volatile unsigned int maxLoop = 0;
#endif


/*
 * High level IRQ handler called from shared_raw_irq_code_entry
 */
void C_dispatch_irq_handler (CPU_Interrupt_frame *frame, unsigned int excNum)
{
	register unsigned int irq;
#if 0
	register unsigned oldMask;		      /* old siu pic masks */
#endif
	register unsigned msr;
	register unsigned new_msr;
	register unsigned old_simr_h;
	register unsigned old_simr_l;
#ifdef DISPATCH_HANDLER_STAT
	unsigned loopCounter;
#endif



	/*
	 * Handle decrementer interrupt
	 */
	if (excNum == ASM_DEC_VECTOR) {
		_CPU_MSR_GET(msr);
		new_msr = msr | MSR_EE;
		_CPU_MSR_SET(new_msr);

		rtems_hdl_tbl[BSP_DECREMENTER].hdl();

		_CPU_MSR_SET(msr);

		return;
	}

	/*
	 * Handle external interrupt generated by SIU on PPC core
	 */
#ifdef DISPATCH_HANDLER_STAT
	loopCounter = 0;
#endif  

	while (1) {

		if( ((m8260.sipnr_h & m8260.simr_h) | (m8260.sipnr_l & m8260.simr_l)) == 0 ) {
#ifdef DISPATCH_HANDLER_STAT
			if (loopCounter >  maxLoop) maxLoop = loopCounter;
#endif      
			break;
		}

		irq = (m8260.sivec >> 26) + BSP_CPM_IRQ_LOWEST_OFFSET;

		/* Clear mask and pending register */
		if( irq <= BSP_CPM_IRQ_MAX_OFFSET ) {
			/* save interrupt masks */
			old_simr_h = m8260.simr_h;
			old_simr_l = m8260.simr_l;

			/* mask off current interrupt and lower priority ones */
			m8260.simr_h &= SIU_MaskBit[irq].priority_h;
			m8260.simr_l &= SIU_MaskBit[irq].priority_l;

			/* clear pending bit */
			m8260.sipnr_h |= SIU_MaskBit[irq].mask_h;
			m8260.sipnr_l |= SIU_MaskBit[irq].mask_l;

			/* re-enable external exceptions */
			_CPU_MSR_GET(msr);
			new_msr = msr | MSR_EE;
			_CPU_MSR_SET(new_msr);

			/* call handler */
			rtems_hdl_tbl[irq].hdl();

			/* disable exceptions again */
			_CPU_MSR_SET(msr);
			
			/* restore interrupt masks */
			m8260.simr_h = old_simr_h;
			m8260.simr_l = old_simr_l;
			
		}
#ifdef DISPATCH_HANDLER_STAT
		++ loopCounter;
#endif
	}
}

    
  
void _ThreadProcessSignalsFromIrq (BSP_Exception_frame* ctx)
{
	/*
	 * Process pending signals that have not already been
	 * processed by _Thread_Displatch. This happens quite
	 * unfrequently : the ISR must have posted an action
	 * to the current running thread.
	 */
	if ( _Thread_Do_post_task_switch_extension ||
		_Thread_Executing->do_post_task_switch_extension ) {
		_Thread_Executing->do_post_task_switch_extension = FALSE;
		_API_extensions_Run_postswitch();
	}

	/*
	 * I plan to process other thread related events here.
	 * This will include DEBUG session requested from keyboard...
	 */
}

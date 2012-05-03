/*
 * irq.c
 *
 *  This file contains the implementation of the function described in irq.h
 *
 *  MPC5xx port sponsored by Defence Research and Development Canada - Suffield
 *  Copyright (C) 2004, Real-Time Systems Inc. (querbach@realtime.bc.ca)
 *
 *  Derived from libbsp/powerpc/mbx8xx/irq/irq.c:
 *
 *  Copyright (C) 1998, 1999 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/score/apiext.h>
#include <mpc5xx.h>
#include <libcpu/vectors.h>
#include <libcpu/raw_exception.h>
#include <libcpu/irq.h>

/*
 * Convert an rtems_irq_number constant to an interrupt level
 * suitable for programming into an I/O device's interrupt level field.
 */

int CPU_irq_level_from_symbolic_name(const rtems_irq_number name)
{
  if (CPU_USIU_EXT_IRQ_0 <= name && name <= CPU_USIU_INT_IRQ_7)
    return (name - CPU_USIU_EXT_IRQ_0) / 2;

  if (CPU_UIMB_IRQ_8 <= name && name <= CPU_UIMB_IRQ_31)
    return 8 + (name - CPU_UIMB_IRQ_8);

  return 31;			/* reasonable default */
}

/*
 * default handler connected on each irq after bsp initialization
 */
static rtems_irq_connect_data		default_rtems_entry;

/*
 * location used to store initial tables used for interrupt
 * management.
 */
static rtems_irq_global_settings* 	internal_config;
static rtems_irq_connect_data*		rtems_hdl_tbl;

/*
 * Check if symbolic IRQ name is an USIU IRQ
 */
static inline int is_usiu_irq(const rtems_irq_number irqLine)
{
  return (((int) irqLine <= CPU_USIU_IRQ_MAX_OFFSET) &&
	  ((int) irqLine >= CPU_USIU_IRQ_MIN_OFFSET)
	 );
}

/*
 * Check if symbolic IRQ name is an UIMB IRQ
 */
static inline int is_uimb_irq(const rtems_irq_number irqLine)
{
  return (((int) irqLine <= CPU_UIMB_IRQ_MAX_OFFSET) &&
	  ((int) irqLine >= CPU_UIMB_IRQ_MIN_OFFSET)
	 );
}

/*
 * Check if symbolic IRQ name is a Processor IRQ
 */
static inline int is_proc_irq(const rtems_irq_number irqLine)
{
  return (((int) irqLine <= CPU_PROC_IRQ_MAX_OFFSET) &&
	  ((int) irqLine >= CPU_PROC_IRQ_MIN_OFFSET)
	 );
}


/*
 * Masks used to mask off the interrupts. For exmaple, for ILVL2, the
 * mask is used to mask off interrupts ILVL2, IRQ3, ILVL3, ... IRQ7
 * and ILVL7.
 *
 */
const static unsigned int USIU_IvectMask[CPU_USIU_IRQ_COUNT] =
{
  0,		 		/* external IRQ 0 */
  0xFFFFFFFF << 31, 		/* internal level 0 */
  0xFFFFFFFF << 30, 		/* external IRQ 1 */
  0xFFFFFFFF << 29, 		/* internal level 1 */
  0xFFFFFFFF << 28, 		/* external IRQ 2 */
  0xFFFFFFFF << 27, 		/* internal level 2 */
  0xFFFFFFFF << 26, 		/* external IRQ 3 */
  0xFFFFFFFF << 25, 		/* internal level 3 */
  0xFFFFFFFF << 24, 		/* external IRQ 4 */
  0xFFFFFFFF << 23, 		/* internal level 4 */
  0xFFFFFFFF << 22, 		/* external IRQ 5 */
  0xFFFFFFFF << 21, 		/* internal level 5 */
  0xFFFFFFFF << 20, 		/* external IRQ 6 */
  0xFFFFFFFF << 19, 		/* internal level 6 */
  0xFFFFFFFF << 18, 		/* external IRQ 7 */
  0xFFFFFFFF << 17 		/* internal level 7 */
};


/*
 * ------------------------ RTEMS Irq helper functions ----------------
 */

/*
 * Caution : this function assumes the variable "internal_config"
 * is already set and that the tables it contains are still valid
 * and accessible.
 */
static void compute_USIU_IvectMask_from_prio (void)
{
  /*
   * In theory this is feasible. No time to code it yet. See i386/shared/irq.c
   * for an example based on 8259 controller mask. The actual masks defined
   * correspond to the priorities defined for the USIU in irq_init.c.
   */
}

/*
 * This function check that the value given for the irq line
 * is valid.
 */
static int isValidInterrupt(int irq)
{
  if ( (irq < CPU_MIN_OFFSET) || (irq > CPU_MAX_OFFSET)
        || (irq == CPU_UIMB_INTERRUPT) )
    return 0;
  return 1;
}

int CPU_irq_enable_at_uimb(const rtems_irq_number irqLine)
{
  if (!is_uimb_irq(irqLine))
    return 1;
  return 0;
}

int CPU_irq_disable_at_uimb(const rtems_irq_number irqLine)
{
  if (!is_uimb_irq(irqLine))
    return 1;
  return 0;
}

int CPU_irq_enabled_at_uimb(const rtems_irq_number irqLine)
{
  if (!is_uimb_irq(irqLine))
    return 0;
  return 1;
}

int CPU_irq_enable_at_usiu(const rtems_irq_number irqLine)
{
  int usiu_irq_index;

  if (!is_usiu_irq(irqLine))
    return 1;

  usiu_irq_index = ((int) (irqLine) - CPU_USIU_IRQ_MIN_OFFSET);
  ppc_cached_irq_mask |= (1 << (31-usiu_irq_index));
  usiu.simask = ppc_cached_irq_mask;

  return 0;
}

int CPU_irq_disable_at_usiu(const rtems_irq_number irqLine)
{
  int usiu_irq_index;

  if (!is_usiu_irq(irqLine))
    return 1;

  usiu_irq_index = ((int) (irqLine) - CPU_USIU_IRQ_MIN_OFFSET);
  ppc_cached_irq_mask &= ~(1 << (31-usiu_irq_index));
  usiu.simask = ppc_cached_irq_mask;

  return 0;
}

int CPU_irq_enabled_at_usiu(const rtems_irq_number irqLine)
{
  int usiu_irq_index;

  if (!is_usiu_irq(irqLine))
    return 0;

  usiu_irq_index = ((int) (irqLine) - CPU_USIU_IRQ_MIN_OFFSET);
  return ppc_cached_irq_mask & (1 << (31-usiu_irq_index));
}

/*
 * --------------- RTEMS Single Irq Handler Mngt Routines ----------------
 */

int CPU_install_rtems_irq_handler	(const rtems_irq_connect_data* irq)
{
    rtems_interrupt_level       level;

    if (!isValidInterrupt(irq->name)) {
      return 0;
    }
    /*
     * Check if default handler is actually connected. If not issue an error.
     * You must first get the current handler via CPU_get_current_idt_entry
     * and then disconnect it using CPU_delete_idt_entry.
     * RATIONALE : to always have the same transition by forcing the user
     * to get the previous handler before accepting to disconnect.
     */
    if (rtems_hdl_tbl[irq->name].hdl != default_rtems_entry.hdl) {
      return 0;
    }

    rtems_interrupt_disable(level);

    /*
     * store the data provided by user
     */
    rtems_hdl_tbl[irq->name] = *irq;

    if (is_uimb_irq(irq->name)) {
      /*
       * Enable interrupt at UIMB level
       */
      CPU_irq_enable_at_uimb (irq->name);
    }

    if (is_usiu_irq(irq->name)) {
      /*
       * Enable interrupt at USIU level
       */
      CPU_irq_enable_at_usiu (irq->name);
    }

    if (is_proc_irq(irq->name)) {
      /*
       * Should Enable exception at processor level but not needed.  Will restore
       * EE flags at the end of the routine anyway.
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


int CPU_get_current_rtems_irq_handler	(rtems_irq_connect_data* irq)
{
     if (!isValidInterrupt(irq->name)) {
       return 0;
     }
     *irq = rtems_hdl_tbl[irq->name];
     return 1;
}

int CPU_remove_rtems_irq_handler  (const rtems_irq_connect_data* irq)
{
    rtems_interrupt_level       level;

    if (!isValidInterrupt(irq->name)) {
      return 0;
    }
    /*
     * Check if default handler is actually connected. If not issue an error.
     * You must first get the current handler via CPU_get_current_idt_entry
     * and then disconnect it using CPU_delete_idt_entry.
     * RATIONALE : to always have the same transition by forcing the user
     * to get the previous handler before accepting to disconnect.
     */
    if (rtems_hdl_tbl[irq->name].hdl != irq->hdl) {
      return 0;
    }
    rtems_interrupt_disable(level);

    /*
     * Disable interrupt on device
     */
	if (irq->off)
    	irq->off(irq);

    if (is_uimb_irq(irq->name)) {
      /*
       * disable interrupt at UIMB level
       */
      CPU_irq_disable_at_uimb (irq->name);
    }
    if (is_usiu_irq(irq->name)) {
      /*
       * disable interrupt at USIU level
       */
      CPU_irq_disable_at_usiu (irq->name);
    }
    if (is_proc_irq(irq->name)) {
      /*
       * disable exception at processor level
       */
    }

    /*
     * restore the default irq value
     */
    rtems_hdl_tbl[irq->name] = default_rtems_entry;

    rtems_interrupt_enable(level);

    return 1;
}

/*
 * ---------------- RTEMS Global Irq Handler Mngt Routines ----------------
 */

int CPU_rtems_irq_mngt_set	(rtems_irq_global_settings* config)
{
    int                    i;
    rtems_interrupt_level  level;

   /*
    * Store various code accelerators
    */
    internal_config 		= config;
    default_rtems_entry 	= config->defaultEntry;
    rtems_hdl_tbl 		= config->irqHdlTbl;

    rtems_interrupt_disable(level);

    /*
     * Start with UIMB IRQ
     */
    for (i = CPU_UIMB_IRQ_MIN_OFFSET; i <= CPU_UIMB_IRQ_MAX_OFFSET ; i++) {
      if (rtems_hdl_tbl[i].hdl != default_rtems_entry.hdl) {
	CPU_irq_enable_at_uimb (i);
	if (rtems_hdl_tbl[i].on)
		rtems_hdl_tbl[i].on(&rtems_hdl_tbl[i]);
      }
      else {
	if (rtems_hdl_tbl[i].off)
		rtems_hdl_tbl[i].off(&rtems_hdl_tbl[i]);
	CPU_irq_disable_at_uimb (i);
      }
    }

    /*
     * Continue with USIU IRQ
     * Set up internal tables used by rtems interrupt prologue
     */
    compute_USIU_IvectMask_from_prio ();

    for (i = CPU_USIU_IRQ_MIN_OFFSET; i <= CPU_USIU_IRQ_MAX_OFFSET ; i++) {
      if (rtems_hdl_tbl[i].hdl != default_rtems_entry.hdl) {
	CPU_irq_enable_at_usiu (i);
	if (rtems_hdl_tbl[i].on)
		rtems_hdl_tbl[i].on(&rtems_hdl_tbl[i]);
      }
      else {
	if (rtems_hdl_tbl[i].off)
		rtems_hdl_tbl[i].off(&rtems_hdl_tbl[i]);
	CPU_irq_disable_at_usiu (i);
       }
    }

    /*
     * Enable all UIMB interrupt lines, then enable at USIU.
     */
    imb.uimb.umcr |= UIMB_UMCR_IRQMUX(3);
    CPU_irq_enable_at_usiu (CPU_UIMB_INTERRUPT);

    /*
     * finish with Processor exceptions handled like IRQ
     */
    for (i = CPU_PROC_IRQ_MIN_OFFSET; i <= CPU_PROC_IRQ_MAX_OFFSET; i++) {
      if (rtems_hdl_tbl[i].hdl != default_rtems_entry.hdl) {
	if (rtems_hdl_tbl[i].on)
		rtems_hdl_tbl[i].on(&rtems_hdl_tbl[i]);
      }
      else {
	if (rtems_hdl_tbl[i].off)
		rtems_hdl_tbl[i].off(&rtems_hdl_tbl[i]);
      }
    }
    rtems_interrupt_enable(level);
    return 1;
}

int CPU_rtems_irq_mngt_get(rtems_irq_global_settings** config)
{
    *config = internal_config;
    return 0;
}


/*
 * High level IRQ handler called from shared_raw_irq_code_entry
 */
void C_dispatch_irq_handler (CPU_Interrupt_frame *frame, unsigned int excNum)
{
  register unsigned int irq;
  register unsigned uimbIntr;                 /* boolean */
  register unsigned oldMask;		      /* old siu pic masks */
  register unsigned msr;
  register unsigned new_msr;

  /*
   * Handle decrementer interrupt
   */
  if (excNum == ASM_DEC_VECTOR) {
    _CPU_MSR_GET(msr);
    new_msr = msr | MSR_EE;
    _CPU_MSR_SET(new_msr);

    rtems_hdl_tbl[CPU_DECREMENTER].hdl(rtems_hdl_tbl[CPU_DECREMENTER].handle);

    _CPU_MSR_SET(msr);
    return;
  }

  /*
   * Handle external interrupt generated by USIU on PPC core
   */
  while ((ppc_cached_irq_mask & usiu.sipend) != 0) {
    irq = (usiu.sivec >> 26);
    uimbIntr = (irq == CPU_UIMB_INTERRUPT);
    /*
     * Disable the interrupt of the same and lower priority.
     */
    oldMask = ppc_cached_irq_mask;
    ppc_cached_irq_mask = oldMask & USIU_IvectMask[irq];
    usiu.simask = ppc_cached_irq_mask;
    /*
     * Acknowledge current interrupt. This has no effect on internal level
     * interrupts.
     */
    usiu.sipend = (1 << (31 - irq));

    if (uimbIntr)  {
      /*
       * Look at the bits set in the UIMB interrupt-pending register.  The
       * highest-order set bit indicates the handler we will run.
       *
       * Unfortunately, we can't easily mask individual UIMB interrupts
       * unless they use USIU levels 0 to 6, so we must mask all low-level
       * (level > 7) UIMB interrupts while we service any interrupt.
       */
      int uipend = imb.uimb.uipend << 8;

      if (uipend == 0) {	/* spurious interrupt?  use last vector */
        irq = CPU_UIMB_IRQ_MAX_OFFSET;
      }
      else {
        irq = CPU_UIMB_IRQ_MIN_OFFSET;
        for ( ; (uipend & 0x8000000) == 0; uipend <<= 1) {
          irq++;
        }
      }
    }
    _CPU_MSR_GET(msr);
    new_msr = msr | MSR_EE;
    _CPU_MSR_SET(new_msr);

    rtems_hdl_tbl[irq].hdl(rtems_hdl_tbl[irq].handle);

    _CPU_MSR_SET(msr);

    ppc_cached_irq_mask = oldMask;
    usiu.simask = ppc_cached_irq_mask;
  }
}

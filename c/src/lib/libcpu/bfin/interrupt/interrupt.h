/*
 *  RTEMS support for Blackfin interrupt controller
 *
 *  COPYRIGHT (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *            written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _interrupt_h_
#define _interrupt_h_

/* Some rules for using this module:

   SIC_IARx registers must not be changed after calling
   bfin_interrupt_init().

   The bfin_isr structures must stick around for as long as the isr is
   registered.

   For any interrupt source (SIC bit) that could be shared, it is only
   safe to disable an ISR through this module if the ultimate source is
   also disabled (the ultimate source must be disabled prior to disabling
   it through this module, and must remain disabled until after enabling
   it through this module).

   For any source that is shared with modules that cannot be disabled,
   give careful thought to the control of those interrupts.
   bfin_interrupt_enable_all() or bfin_interrupt_enable_global() can
   be used to help solve the problems caused by that.


   Note that this module does not provide prioritization.  It is assumed
   that the priorities afforded by the CEC are sufficient.  If finer
   grained priority control is required then this wlll need to be
   redesigned.
*/


#ifdef __cplusplus
extern "C" {
#endif

/* source is the source to the SIC (the bit number in SIC_ISR).  isr is
   the function that will be called when the interrupt is active. */
typedef struct bfin_isr_s {
  int source;
  void (*isr)(int source);
  /* the following are for internal use only */
  uint32_t mask;
  int vector;
  struct bfin_isr_s *next;
} bfin_isr_t;

/* If non-default mapping is desired, the BSP should set the SIC_IARx
   registers prior to calling this. */
void bfin_interrupt_init(void);

/* ISR starts out disabled */
void bfin_interrupt_register(bfin_isr_t *isr);
void bfin_interrupt_unregister(bfin_isr_t *isr);

/* enable/disable specific ISR */
void bfin_interrupt_enable(bfin_isr_t *isr, bool enable);

/* atomically enable/disable all ISRs attached to specified source */
void bfin_interrupt_enable_all(int source, bool enable);

/* disable a source independently of the individual ISR enables (starts
   out all enabled) */
void bfin_interrupt_enable_global(int source, bool enable);

#ifdef __cplusplus
}
#endif

#endif /* _interrupt_h_ */


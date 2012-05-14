/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef IRQ_SHARED_IRQ_C_GLUE_H
#define IRQ_SHARED_IRQ_C_GLUE_H
/*
 *  This header describes the routines that are needed by the shared
 *  version of 'irq.c' (implementing the RTEMS irq API). They
 *  must be provided by the BSP.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#ifndef BSP_SHARED_HANDLER_SUPPORT
#define BSP_SHARED_HANDLER_SUPPORT      1
#endif

#include <rtems.h>
#include <rtems/irq.h>

#include <bsp/vectors.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * PIC-independent functions to enable/disable interrupt lines at
 * the pic.
 *
 * NOTE: the routines must ignore requests for enabling/disabling
 *       interrupts that are outside of the range handled by the
 *       PIC(s).
 */
extern void BSP_enable_irq_at_pic(const rtems_irq_number irqLine);
/*
 * RETURNS: nonzero (> 0 ) if irq was enabled originally, zero if irq
 *          was off and negative value if there was an error.
 */
extern int  BSP_disable_irq_at_pic(const rtems_irq_number irqLine);

/*
 * Initialize the PIC.
 */
extern int  BSP_setup_the_pic(rtems_irq_global_settings* config);

/* IRQ dispatcher to be defined by the PIC driver; note that it MUST
 * implement shared interrupts.
 * Note also that the exception frame passed to this handler is not very
 * meaningful. Only the volatile registers and vector info are stored.
 *
 *******************************************************************
 * The routine must return zero if the interrupt was handled. If a
 * nonzero value is returned the dispatcher may panic and flag an
 * uncaught exception.
 *******************************************************************
 */
int C_dispatch_irq_handler (BSP_Exception_frame *frame, unsigned int excNum);

/*
 * Snippet to be used by PIC drivers and by bsp_irq_dispatch_list
 * traverses list of shared handlers for a given interrupt
 *
 */

static inline void
bsp_irq_dispatch_list_base(
  rtems_irq_connect_data *tbl,
  unsigned irq,
  rtems_irq_hdl sentinel
)
{
	rtems_irq_connect_data* vchain;
	for( vchain = &tbl[irq];
			((int)vchain != -1 && vchain->hdl != sentinel);
			vchain = (rtems_irq_connect_data*)vchain->next_handler )
	{
          vchain->hdl(vchain->handle);
	}
}


/*
 * Snippet to be used by PIC drivers;
 * enables interrupts, traverses list of
 * shared handlers for a given interrupt
 * and restores original irq level
 *
 * Note that _ISR_Get_level() & friends are preferable to
 * manipulating MSR directly.
 */

static inline void
bsp_irq_dispatch_list(
  rtems_irq_connect_data *tbl,
  unsigned irq,
  rtems_irq_hdl sentinel
)
{
	register uint32_t l_orig;

	l_orig = _ISR_Get_level();

	/* Enable all interrupts */
	_ISR_Set_level(0);


        bsp_irq_dispatch_list_base( tbl, irq, sentinel );

	/* Restore original level */
	_ISR_Set_level(l_orig);
}

#ifdef __cplusplus
}
#endif

#endif

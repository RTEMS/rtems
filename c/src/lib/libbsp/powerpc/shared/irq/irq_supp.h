#ifndef IRQ_SHARED_IRQ_C_GLUE_H
#define IRQ_SHARED_IRQ_C_GLUE_H
/* 
 *  This header describes the routines that are needed by the shared
 *  version of 'irq.c' (implementing the RTEMS irq API). They
 *  must be provided by the BSP.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#ifndef BSP_SHARED_HANDLER_SUPPORT
#define BSP_SHARED_HANDLER_SUPPORT      1
#endif

#include <rtems/irq.h>

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
extern void BSP_enable_irq_at_pic		(const rtems_irq_number irqLine);
extern void BSP_disable_irq_at_pic		(const rtems_irq_number irqLine);

/*
 * Initialize the PIC.
 * Return nonzero on success, zero on failure (which will be treated
 * as fatal by the manager).
 */
extern int  BSP_setup_the_pic			(rtems_irq_global_settings* config);

struct _BSP_Exception_frame;

/* IRQ dispatcher to be defined by the PIC driver; note that it MUST
 * implement shared interrupts.
 * Note that the exception frame passed to this handler is not very
 * meaningful. Only the volatile registers and info are set up.
 */
void C_dispatch_irq_handler (struct _BSP_Exception_frame *frame, unsigned int excNum);

#ifdef __cplusplus
}
#endif

#endif

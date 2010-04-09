/*
 * Interrupt handler Header file for PXA By Yang Xi <hiyangxi@gmail.com>
 * Copyright (c) 2004 by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __IRQ_H__
#define __IRQ_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __asm__

/*
 * Include some preprocessor value also used by assember code
 */

#include <rtems/irq.h>
#include <rtems.h>
#include <pxa255.h>

extern void default_int_handler(rtems_irq_hdl_param unused);
extern void (*IRQ_table[PRIMARY_IRQS])(rtems_irq_hdl_param param);
extern void dummy_handler(rtems_irq_hdl_param unused);

extern void BSP_rtems_irq_mngt_init(void);

#endif /* __asm__ */

#ifdef __cplusplus
}
#endif

#endif /* __IRQ_H__ */

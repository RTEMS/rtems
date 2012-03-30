/*===============================================================*\
| Project: RTEMS virtex BSP                                       |
+-----------------------------------------------------------------+
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file declares constants of the interrupt controller        |
\*===============================================================*/
#ifndef VIRTEX5_IRQ_IRQ_H
#define VIRTEX5_IRQ_IRQ_H

#include <rtems/irq.h>

/*
 * the following definitions specify the indices used
 * to interface the interrupt handler API
 */

/*
 * Peripheral IRQ handlers related definitions
 */
  /* Not supported at this level */

/*
 * Processor IRQ handlers related definitions
 */
#define BSP_PROCESSOR_IRQ_NUMBER        3
#define BSP_PROCESSOR_IRQ_LOWEST_OFFSET 0
#define BSP_PROCESSOR_IRQ_MAX_OFFSET    (BSP_PROCESSOR_IRQ_LOWEST_OFFSET\
                                         +BSP_PROCESSOR_IRQ_NUMBER-1)

#define BSP_IS_PROCESSOR_IRQ(irqnum)                 \
  (((irqnum) >= BSP_PROCESSOR_IRQ_LOWEST_OFFSET) &&  \
   ((irqnum) <= BSP_PROCESSOR_IRQ_MAX_OFFSET))

/*
 * Summary
 */
#define BSP_IRQ_NUMBER                  (BSP_PROCESSOR_IRQ_MAX_OFFSET+1)
#define BSP_LOWEST_OFFSET                BSP_PROCESSOR_IRQ_LOWEST_OFFSET
#define BSP_MAX_OFFSET                   BSP_PROCESSOR_IRQ_MAX_OFFSET

#define BSP_IS_VALID_IRQ(irqnum)        (BSP_IS_PROCESSOR_IRQ(irqnum))

#ifndef ASM
#ifdef __cplusplus
extern "C" {
#endif

/*
 * index table for the module specific handlers, a few entries are only placeholders
 */
  typedef enum {
    BSP_EXT               = BSP_PROCESSOR_IRQ_LOWEST_OFFSET + 0,
    BSP_PIT               = BSP_PROCESSOR_IRQ_LOWEST_OFFSET + 1,
    BSP_CRIT              = BSP_PROCESSOR_IRQ_LOWEST_OFFSET + 2
  } rtems_irq_symbolic_name;

  extern rtems_irq_connect_data *BSP_rtems_irq_tbl;
  void BSP_rtems_irq_mngt_init(unsigned cpuId);

#ifdef __cplusplus
}
#endif
#endif /* ASM */

#endif /* VIRTEX5_IRQ_IRQ_H */

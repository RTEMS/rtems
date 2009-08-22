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
  
#include <rtems.h>
#include <pxa255.h>

extern void default_int_handler();
extern void (*IRQ_table[PRIMARY_IRQS])(uint32_t vector);
extern void dummy_handler(uint32_t vector);



/* vector table used by shared/irq_init.c */

typedef unsigned char  rtems_irq_level;
typedef unsigned char  rtems_irq_trigger;
struct  __rtems_irq_connect_data__;     /* forward declaratiuon */
typedef unsigned int rtems_irq_number;
typedef void (*rtems_irq_hdl)       (uint32_t vector);
typedef void (*rtems_irq_enable)    (const struct __rtems_irq_connect_data__*);
typedef void (*rtems_irq_disable)   (const struct __rtems_irq_connect_data__*);
typedef int  (*rtems_irq_is_enabled)(const struct __rtems_irq_connect_data__*);

typedef struct __rtems_irq_connect_data__ {
    /* IRQ line */
    rtems_irq_number		 name;

    /* Handler */
    rtems_irq_hdl                 hdl;

    /* function for enabling interrupts at device level. */
    rtems_irq_enable              on;

    /* function for disabling interrupts at device level. */
    rtems_irq_disable             off;

    /* Function to test if interrupt is enabled */
    rtems_irq_is_enabled        isOn;

    /* priority level of interrupt */
    rtems_irq_level               irqLevel;

    /* Trigger method (rising/falling edge or high/low level) */
    rtems_irq_trigger             irqTrigger;
} rtems_irq_connect_data;

/*
 * function to initialize the interrupt for a specific BSP
 */
void BSP_rtems_irq_mngt_init();


/*
 * function to connect a particular irq handler.
 */
int BSP_install_rtems_irq_handler       (const rtems_irq_connect_data*);

/*
 * function to get the current RTEMS irq handler for ptr->name. 
 */
int BSP_get_current_rtems_irq_handler   (rtems_irq_connect_data* ptr);

/*
 * function to disconnect the RTEMS irq handler for ptr->name.
 */
int BSP_remove_rtems_irq_handler        (const rtems_irq_connect_data*);

#endif /* __asm__ */

#ifdef __cplusplus
}
#endif

#endif /* __IRQ_H__ */

/*
 * Interrupt handler Header file
 *
 * Copyright (c) 2004 by Jay Monkman <jtm@lopingdog.com>
 *      
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.OARcorp.com/rtems/license.html.
 *
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
#include <at91rm9200.h>

extern void default_int_handler();
/***********************************************************************
 * Constants
 **********************************************************************/
/* enum of the possible interrupt sources on the AT91RM9200 */
typedef enum {
    AT91RM9200_INT_FIQ  = 0,
    AT91RM9200_INT_SYSIRQ,
    AT91RM9200_INT_PIOA,
    AT91RM9200_INT_PIOB,
    AT91RM9200_INT_PIOC,
    AT91RM9200_INT_PIOD,
    AT91RM9200_INT_US0,
    AT91RM9200_INT_US1,
    AT91RM9200_INT_US2,
    AT91RM9200_INT_US3,
    AT91RM9200_INT_MCI,
    AT91RM9200_INT_UDP,
    AT91RM9200_INT_TWI,
    AT91RM9200_INT_SPI,
    AT91RM9200_INT_SSC0,
    AT91RM9200_INT_SSC1,
    AT91RM9200_INT_SSC2,
    AT91RM9200_INT_TC0,
    AT91RM9200_INT_TC1,
    AT91RM9200_INT_TC2,
    AT91RM9200_INT_TC3,
    AT91RM9200_INT_TC4,
    AT91RM9200_INT_TC5,
    AT91RM9200_INT_UHP,
    AT91RM9200_INT_EMAC,
    AT91RM9200_INT_IRQ0,
    AT91RM9200_INT_IRQ1,
    AT91RM9200_INT_IRQ2,
    AT91RM9200_INT_IRQ3,
    AT91RM9200_INT_IRQ4,
    AT91RM9200_INT_IRQ5,
    AT91RM9200_INT_IRQ6,
    AT91RM9200_MAX_INT
} rtems_irq_symbolic_name;

/* vector table used by shared/irq_init.c */
/* we can treat the AT91RM9200 AIC_SVR_BASE as */
/* a vector table */
#define VECTOR_TABLE AIC_SVR_BASE
                                                                                           
typedef unsigned char  rtems_irq_level;
typedef unsigned char  rtems_irq_trigger;

struct  __rtems_irq_connect_data__;     /* forward declaratiuon */

typedef void (*rtems_irq_hdl)       (void);
typedef void (*rtems_irq_enable)    (const struct __rtems_irq_connect_data__*);
typedef void (*rtems_irq_disable)   (const struct __rtems_irq_connect_data__*);
typedef int  (*rtems_irq_is_enabled)(const struct __rtems_irq_connect_data__*);

typedef struct __rtems_irq_connect_data__ {
    /* IRQ line */
    rtems_irq_symbolic_name       name;

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

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

  /* define that can be useful (the values are just examples) */
#ifndef __asm__

/*
 * Include some preprocessor value also used by assember code
 */
  
#include <rtems.h>
#include <mc9328mxl.h>

extern void default_int_handler();
/***********************************************************************
 * Constants
 **********************************************************************/

/* enum of the possible interrupt sources on the AT91RM9200 */
typedef enum {
    BSP_INT_UART3_PFERR = 0,
    BSP_INT_UART3_RTS,
    BSP_INT_UART3_DTR,
    BSP_INT_UART3_UARTC,
    BSP_INT_UART3_TX,
    BSP_INT_PEN_UP,
    BSP_INT_CSI,
    BSP_INT_MMA_MAC,
    BSP_INT_MMA,
    BSP_INT_COMP,
    BSP_INT_MSIRQ,
    BSP_INT_GPIO_PORTA,
    BSP_INT_GPIO_PORTB,
    BSP_INT_GPIO_PORTC,
    BSP_INT_LCDC,
    BSP_INT_SIM_IRQ,
    BSP_INT_SIM_DATA,
    BSP_INT_RTC,
    BSP_INT_RTC_SAM,
    BSP_INT_UART2_PFERR,
    BSP_INT_UART2_RTS,
    BSP_INT_UART2_DTR,
    BSP_INT_UART2_UARTC,
    BSP_INT_UART2_TX,
    BSP_INT_UART2_RX,
    BSP_INT_UART1_PFERR,
    BSP_INT_UART1_RTS,
    BSP_INT_UART1_DTR,
    BSP_INT_UART1_UARTC,
    BSP_INT_UART1_TX,
    BSP_INT_UART1_RX,
    BSP_INT_RES31,
    BSP_INT_RES32,
    BSP_INT_PEN_DATA,
    BSP_INT_PWM,
    BSP_INT_MMC_IRQ,
    BSP_INT_SSI2_TX,
    BSP_INT_SSI2_RX,
    BSP_INT_SSI2_ERR,
    BSP_INT_I2C,
    BSP_INT_SPI2,
    BSP_INT_SPI1,
    BSP_INT_SSI_TX,
    BSP_INT_SSI_TX_ERR,
    BSP_INT_SSI_RX,
    BSP_INT_SSI_RX_ERR,
    BSP_INT_TOUCH,
    BSP_INT_USBD0,
    BSP_INT_USBD1,
    BSP_INT_USBD2,
    BSP_INT_USBD3,
    BSP_INT_USBD4,
    BSP_INT_USBD5,
    BSP_INT_USBD6,
    BSP_INT_UART3_RX,
    BSP_INT_BTSYS,
    BSP_INT_BTTIM,
    BSP_INT_BTWUI,
    BSP_INT_TIMER2,
    BSP_INT_TIMER1,
    BSP_INT_DMA_ERR,
    BSP_INT_DMA,
    BSP_INT_GPIO_PORTD,
    BSP_INT_WDT,

    BSP_MAX_INT
} rtems_irq_symbolic_name;

extern void *bsp_vector_table;
#define VECTOR_TABLE &bsp_vector_table;
  											   
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

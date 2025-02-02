/*
 * Copyright (C) 2010 embedded brains GmbH & Co. KG
 * Copyright (C) 2004 by Jay Monkman <jtm@lopingdog.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Interrupt handler Header file
 */

#ifndef __IRQ_H__
#define __IRQ_H__

#ifndef __asm__

#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>

#endif /* __asm__ */

/* possible interrupt sources on the MC9328MXL */
#define BSP_INT_UART3_PFERR       0
#define BSP_INT_UART3_RTS         1
#define BSP_INT_UART3_DTR         2
#define BSP_INT_UART3_UARTC       3
#define BSP_INT_UART3_TX          4
#define BSP_INT_PEN_UP            5
#define BSP_INT_CSI               6
#define BSP_INT_MMA_MAC           7
#define BSP_INT_MMA               8
#define BSP_INT_COMP              9
#define BSP_INT_MSIRQ            10
#define BSP_INT_GPIO_PORTA       11
#define BSP_INT_GPIO_PORTB       12
#define BSP_INT_GPIO_PORTC       13
#define BSP_INT_LCDC             14
#define BSP_INT_SIM_IRQ          15
#define BSP_INT_SIM_DATA         16
#define BSP_INT_RTC              17
#define BSP_INT_RTC_SAM          18
#define BSP_INT_UART2_PFERR      19
#define BSP_INT_UART2_RTS        20
#define BSP_INT_UART2_DTR        21
#define BSP_INT_UART2_UARTC      22
#define BSP_INT_UART2_TX         23
#define BSP_INT_UART2_RX         24
#define BSP_INT_UART1_PFERR      25
#define BSP_INT_UART1_RTS        26
#define BSP_INT_UART1_DTR        27
#define BSP_INT_UART1_UARTC      28
#define BSP_INT_UART1_TX         29
#define BSP_INT_UART1_RX         30
#define BSP_INT_RES31            31
#define BSP_INT_RES32            32
#define BSP_INT_PEN_DATA         33
#define BSP_INT_PWM              34
#define BSP_INT_MMC_IRQ          35
#define BSP_INT_SSI2_TX          36
#define BSP_INT_SSI2_RX          37
#define BSP_INT_SSI2_ERR         38
#define BSP_INT_I2C              39
#define BSP_INT_SPI2             40
#define BSP_INT_SPI1             41
#define BSP_INT_SSI_TX           42
#define BSP_INT_SSI_TX_ERR       43
#define BSP_INT_SSI_RX           44
#define BSP_INT_SSI_RX_ERR       45
#define BSP_INT_TOUCH            46
#define BSP_INT_USBD0            47
#define BSP_INT_USBD1            48
#define BSP_INT_USBD2            49
#define BSP_INT_USBD3            50
#define BSP_INT_USBD4            51
#define BSP_INT_USBD5            52
#define BSP_INT_USBD6            53
#define BSP_INT_UART3_RX         54
#define BSP_INT_BTSYS            55
#define BSP_INT_BTTIM            56
#define BSP_INT_BTWUI            57
#define BSP_INT_TIMER2           58
#define BSP_INT_TIMER1           59
#define BSP_INT_DMA_ERR          60
#define BSP_INT_DMA              61
#define BSP_INT_GPIO_PORTD       62
#define BSP_INT_WDT              63
#define BSP_MAX_INT              64

#define BSP_INTERRUPT_VECTOR_COUNT BSP_MAX_INT

#endif /* __IRQ_H__ */

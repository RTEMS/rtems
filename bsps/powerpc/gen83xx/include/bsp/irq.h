/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * RTEMS generic MPC83xx BSP
 *
 * This file declares constants of the interrupt controller.
 */

/*
 * Copyright (C) 2008, 2010 embedded brains GmbH & Co. KG
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

#ifndef GEN83xx_IRQ_IRQ_H
#define GEN83xx_IRQ_IRQ_H

#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>

#include <bspopts.h>

/*
 * the following definitions specify the indices used
 * to interface the interrupt handler API
 */

/*
 * Peripheral IRQ handlers related definitions
 */
#define BSP_IPIC_PER_IRQ_NUMBER	        128
#define BSP_IPIC_IRQ_LOWEST_OFFSET        0
#define BSP_IPIC_IRQ_MAX_OFFSET      (BSP_IPIC_IRQ_LOWEST_OFFSET\
                                         +BSP_IPIC_PER_IRQ_NUMBER-1)

#define BSP_IS_IPIC_IRQ(irqnum)				\
          (((irqnum) >= BSP_IPIC_IRQ_LOWEST_OFFSET) &&	\
	   ((irqnum) <= BSP_IPIC_IRQ_MAX_OFFSET))
/*
 * Processor IRQ handlers related definitions
 */
#define BSP_PROCESSOR_IRQ_NUMBER        1
#define BSP_PROCESSOR_IRQ_LOWEST_OFFSET	(BSP_IPIC_IRQ_MAX_OFFSET+1)
#define BSP_PROCESSOR_IRQ_MAX_OFFSET	(BSP_PROCESSOR_IRQ_LOWEST_OFFSET\
                                         +BSP_PROCESSOR_IRQ_NUMBER-1)

#define BSP_IS_PROCESSOR_IRQ(irqnum)				\
          (((irqnum) >= BSP_PROCESSOR_IRQ_LOWEST_OFFSET) &&	\
	   ((irqnum) <= BSP_PROCESSOR_IRQ_MAX_OFFSET))
/*
 * Summary
 */
#define BSP_IRQ_NUMBER                  (BSP_PROCESSOR_IRQ_MAX_OFFSET+1)
#define BSP_LOWEST_OFFSET		BSP_IPIC_IRQ_LOWEST_OFFSET
#define BSP_MAX_OFFSET                  BSP_PROCESSOR_IRQ_MAX_OFFSET

#define BSP_IS_VALID_IRQ(irqnum) 	\
          (BSP_IS_PROCESSOR_IRQ(irqnum)	\
           || BSP_IS_IPIC_IRQ(irqnum))

#ifndef ASM
#ifdef __cplusplus
extern "C" {
#endif

/*
 * index table for the module specific handlers, a few entries are only placeholders
 */
  typedef enum {
    BSP_IPIC_IRQ_FIRST     = BSP_IPIC_IRQ_LOWEST_OFFSET,
    BSP_IPIC_IRQ_ERROR     = BSP_IPIC_IRQ_LOWEST_OFFSET +  0,
#if MPC83XX_CHIP_TYPE / 10 == 830
    BSP_IPIC_IRQ_DMA1      = BSP_IPIC_IRQ_LOWEST_OFFSET +  3,
    BSP_IPIC_IRQ_UART      = BSP_IPIC_IRQ_LOWEST_OFFSET +  9,
    BSP_IPIC_IRQ_FLEXCAN   = BSP_IPIC_IRQ_LOWEST_OFFSET + 10,
#else
    BSP_IPIC_IRQ_UART1     = BSP_IPIC_IRQ_LOWEST_OFFSET +  9,
    BSP_IPIC_IRQ_UART2     = BSP_IPIC_IRQ_LOWEST_OFFSET + 10,
    BSP_IPIC_IRQ_SEC       = BSP_IPIC_IRQ_LOWEST_OFFSET + 11,
#endif
    BSP_IPIC_IRQ_I2C1      = BSP_IPIC_IRQ_LOWEST_OFFSET + 14,
    BSP_IPIC_IRQ_I2C2      = BSP_IPIC_IRQ_LOWEST_OFFSET + 15,
    BSP_IPIC_IRQ_SPI       = BSP_IPIC_IRQ_LOWEST_OFFSET + 16,
    BSP_IPIC_IRQ_IRQ1      = BSP_IPIC_IRQ_LOWEST_OFFSET + 17,
    BSP_IPIC_IRQ_IRQ2      = BSP_IPIC_IRQ_LOWEST_OFFSET + 18,
    BSP_IPIC_IRQ_IRQ3      = BSP_IPIC_IRQ_LOWEST_OFFSET + 19,
#if MPC83XX_CHIP_TYPE / 10 == 830
    BSP_IPIC_IRQ_QUICC_HI  = BSP_IPIC_IRQ_LOWEST_OFFSET + 32,
    BSP_IPIC_IRQ_QUICC_LO  = BSP_IPIC_IRQ_LOWEST_OFFSET + 33,
#else
    BSP_IPIC_IRQ_IRQ4      = BSP_IPIC_IRQ_LOWEST_OFFSET + 20,
    BSP_IPIC_IRQ_IRQ5      = BSP_IPIC_IRQ_LOWEST_OFFSET + 21,
    BSP_IPIC_IRQ_IRQ6      = BSP_IPIC_IRQ_LOWEST_OFFSET + 22,
    BSP_IPIC_IRQ_IRQ7      = BSP_IPIC_IRQ_LOWEST_OFFSET + 23,
    BSP_IPIC_IRQ_TSEC1_TX  = BSP_IPIC_IRQ_LOWEST_OFFSET + 32,
    BSP_IPIC_IRQ_TSEC1_RX  = BSP_IPIC_IRQ_LOWEST_OFFSET + 33,
    BSP_IPIC_IRQ_TSEC1_ERR = BSP_IPIC_IRQ_LOWEST_OFFSET + 34,
    BSP_IPIC_IRQ_TSEC2_TX  = BSP_IPIC_IRQ_LOWEST_OFFSET + 35,
    BSP_IPIC_IRQ_TSEC2_RX  = BSP_IPIC_IRQ_LOWEST_OFFSET + 36,
    BSP_IPIC_IRQ_TSEC2_ERR = BSP_IPIC_IRQ_LOWEST_OFFSET + 37,
#endif
    BSP_IPIC_IRQ_USB_DR    = BSP_IPIC_IRQ_LOWEST_OFFSET + 38,
#if MPC83XX_CHIP_TYPE / 10 == 830
    BSP_IPIC_IRQ_ESDHC     = BSP_IPIC_IRQ_LOWEST_OFFSET + 42,
#else
    BSP_IPIC_IRQ_USB_MPH   = BSP_IPIC_IRQ_LOWEST_OFFSET + 39,
#endif
    BSP_IPIC_IRQ_IRQ0      = BSP_IPIC_IRQ_LOWEST_OFFSET + 48,
    BSP_IPIC_IRQ_RTC_SEC   = BSP_IPIC_IRQ_LOWEST_OFFSET + 64,
    BSP_IPIC_IRQ_PIT       = BSP_IPIC_IRQ_LOWEST_OFFSET + 65,
    BSP_IPIC_IRQ_PCI1      = BSP_IPIC_IRQ_LOWEST_OFFSET + 66,
#if MPC83XX_CHIP_TYPE / 10 == 830
    BSP_IPIC_IRQ_MSIR1     = BSP_IPIC_IRQ_LOWEST_OFFSET + 67,
#else
    BSP_IPIC_IRQ_PCI2      = BSP_IPIC_IRQ_LOWEST_OFFSET + 67,
#endif
    BSP_IPIC_IRQ_RTC_ALR   = BSP_IPIC_IRQ_LOWEST_OFFSET + 68,
    BSP_IPIC_IRQ_MU        = BSP_IPIC_IRQ_LOWEST_OFFSET + 69,
    BSP_IPIC_IRQ_SBA       = BSP_IPIC_IRQ_LOWEST_OFFSET + 70,
    BSP_IPIC_IRQ_DMA       = BSP_IPIC_IRQ_LOWEST_OFFSET + 71,
    BSP_IPIC_IRQ_GTM4      = BSP_IPIC_IRQ_LOWEST_OFFSET + 72,
    BSP_IPIC_IRQ_GTM8      = BSP_IPIC_IRQ_LOWEST_OFFSET + 73,
#if MPC83XX_CHIP_TYPE / 10 == 830
    BSP_IPIC_IRQ_QUICC_PORTS = BSP_IPIC_IRQ_LOWEST_OFFSET + 74,
    BSP_IPIC_IRQ_GPIO      = BSP_IPIC_IRQ_LOWEST_OFFSET + 75,
#else
    BSP_IPIC_IRQ_GPIO1     = BSP_IPIC_IRQ_LOWEST_OFFSET + 74,
    BSP_IPIC_IRQ_GPIO2     = BSP_IPIC_IRQ_LOWEST_OFFSET + 75,
#endif
    BSP_IPIC_IRQ_DDR       = BSP_IPIC_IRQ_LOWEST_OFFSET + 76,
    BSP_IPIC_IRQ_LBC       = BSP_IPIC_IRQ_LOWEST_OFFSET + 77,
    BSP_IPIC_IRQ_GTM2      = BSP_IPIC_IRQ_LOWEST_OFFSET + 78,
    BSP_IPIC_IRQ_GTM6      = BSP_IPIC_IRQ_LOWEST_OFFSET + 79,
    BSP_IPIC_IRQ_PMC       = BSP_IPIC_IRQ_LOWEST_OFFSET + 80,
#if MPC83XX_CHIP_TYPE / 10 == 830
    BSP_IPIC_IRQ_MSIR2     = BSP_IPIC_IRQ_LOWEST_OFFSET + 81,
    BSP_IPIC_IRQ_MSIR3     = BSP_IPIC_IRQ_LOWEST_OFFSET + 82,
#else
    BSP_IPIC_IRQ_GTM3      = BSP_IPIC_IRQ_LOWEST_OFFSET + 84,
    BSP_IPIC_IRQ_GTM7      = BSP_IPIC_IRQ_LOWEST_OFFSET + 85,
#endif
#if MPC83XX_CHIP_TYPE / 10 == 830
    BSP_IPIC_IRQ_MSIR4     = BSP_IPIC_IRQ_LOWEST_OFFSET + 86,
    BSP_IPIC_IRQ_MSIR5     = BSP_IPIC_IRQ_LOWEST_OFFSET + 87,
    BSP_IPIC_IRQ_MSIR6     = BSP_IPIC_IRQ_LOWEST_OFFSET + 88,
    BSP_IPIC_IRQ_MSIR7     = BSP_IPIC_IRQ_LOWEST_OFFSET + 89,
#endif
    BSP_IPIC_IRQ_GTM1      = BSP_IPIC_IRQ_LOWEST_OFFSET + 90,
    BSP_IPIC_IRQ_GTM5      = BSP_IPIC_IRQ_LOWEST_OFFSET + 91,
#if MPC83XX_CHIP_TYPE / 10 == 830
    BSP_IPIC_IRQ_DMA1_ERR  = BSP_IPIC_IRQ_LOWEST_OFFSET + 94,
    BSP_IPIC_IRQ_DPTC      = BSP_IPIC_IRQ_LOWEST_OFFSET + 95,
#endif

    BSP_IPIC_IRQ_LAST     = BSP_IPIC_IRQ_MAX_OFFSET,
  } rtems_irq_symbolic_name;

#define BSP_INTERRUPT_VECTOR_COUNT (BSP_MAX_OFFSET + 1)

rtems_status_code mpc83xx_ipic_set_mask( rtems_vector_number vector, rtems_vector_number mask_vector, bool mask);

#define MPC83XX_IPIC_INTERRUPT_NORMAL 0

#define MPC83XX_IPIC_INTERRUPT_SYSTEM 1

#define MPC83XX_IPIC_INTERRUPT_CRITICAL 2

rtems_status_code mpc83xx_ipic_set_highest_priority_interrupt( rtems_vector_number vector, int type);

#ifdef __cplusplus
}
#endif
#endif /* ASM */

#endif /* GEN83XX_IRQ_IRQ_H */

/**
 * @file
 *
 * @ingroup genmcf548x_interrupt
 *
 * @brief Interrupt definitions.
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_M68K_MCF548X_IRQ_H
#define LIBBSP_M68K_MCF548X_IRQ_H

#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>

/**
 * @defgroup genmcf548x_interrupt Interrupt Support
 *
 * @ingroup RTEMSBSPsM68kGenMCF548X
 *
 * @brief Interrupt support.
 */


#define MCF548X_IRQ_EPORT_EPF1 1
#define MCF548X_IRQ_EPORT_EPF2 2
#define MCF548X_IRQ_EPORT_EPF3 3
#define MCF548X_IRQ_EPORT_EPF4 4
#define MCF548X_IRQ_EPORT_EPF5 5
#define MCF548X_IRQ_EPORT_EPF6 6
#define MCF548X_IRQ_EPORT_EPF7 7
#define MCF548X_IRQ_USB_EP0ISR 15
#define MCF548X_IRQ_USB_EP1ISR 16
#define MCF548X_IRQ_USB_EP2ISR 17
#define MCF548X_IRQ_USB_EP3ISR 18
#define MCF548X_IRQ_USB_EP4ISR 19
#define MCF548X_IRQ_USB_EP5ISR 20
#define MCF548X_IRQ_USB_EP6ISR 21
#define MCF548X_IRQ_USB_ISR 22
#define MCF548X_IRQ_USB_AISR 23
#define MCF548X_IRQ_DSPI_RFOF_TFUF 25
#define MCF548X_IRQ_DSPI_RFOF 26
#define MCF548X_IRQ_DSPI_RFDF 27
#define MCF548X_IRQ_DSPI_TFUF 28
#define MCF548X_IRQ_DSPI_TCF 29
#define MCF548X_IRQ_DSPI_TFFF 30
#define MCF548X_IRQ_DSPI_EOQF 31
#define MCF548X_IRQ_DSPI 25
#define MCF548X_IRQ_PSC3 32
#define MCF548X_IRQ_PSC2 33
#define MCF548X_IRQ_PSC1 34
#define MCF548X_IRQ_PSC0 35
#define MCF548X_IRQ_PSC(i) (35 - (i))
#define MCF548X_IRQ_COMMTIM 36
#define MCF548X_IRQ_SEC 37
#define MCF548X_IRQ_FEC1 38
#define MCF548X_IRQ_FEC0 39
#define MCF548X_IRQ_FEC(i) (39 - (i))
#define MCF548X_IRQ_I2C 40
#define MCF548X_IRQ_PCIARB 41
#define MCF548X_IRQ_CBPCI 42
#define MCF548X_IRQ_XLBPCI 43
#define MCF548X_IRQ_XLBARB 47
#define MCF548X_IRQ_DMA 48
#define MCF548X_IRQ_CAN0_ERROR 49
#define MCF548X_IRQ_CAN0_BUSOFF 50
#define MCF548X_IRQ_CAN0_MBOR 51
#define MCF548X_IRQ_SLT1 53
#define MCF548X_IRQ_SLT0 54
#define MCF548X_IRQ_CAN1_ERROR 55
#define MCF548X_IRQ_CAN1_BUSOFF 56
#define MCF548X_IRQ_CAN1_MBOR 57
#define MCF548X_IRQ_GPT3 59
#define MCF548X_IRQ_GPT2 60
#define MCF548X_IRQ_GPT1 61
#define MCF548X_IRQ_GPT0 62

#define BSP_INTERRUPT_VECTOR_MIN 1

#define BSP_INTERRUPT_VECTOR_MAX 63

#endif /* LIBBSP_M68K_MCF548X_IRQ_H */

/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup genmcf548x_interrupt
 *
 * @brief Interrupt definitions.
 */

/*
 * Copyright (c) 2013 embedded brains GmbH & Co. KG
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

#define BSP_INTERRUPT_VECTOR_COUNT 64

#define BSP_INTERRUPT_CUSTOM_VALID_VECTOR

static inline bool bsp_interrupt_is_valid_vector(rtems_vector_number vector)
{
  return 1 <= vector
    && vector < (rtems_vector_number) BSP_INTERRUPT_VECTOR_COUNT;
}

void mcf548x_interrupt_vector_install(rtems_vector_number vector);

void mcf548x_interrupt_vector_remove(rtems_vector_number vector);

#define bsp_interrupt_vector_install(v) mcf548x_interrupt_vector_install(v)

#define bsp_interrupt_vector_remove(v) mcf548x_interrupt_vector_remove(v)

#endif /* LIBBSP_M68K_MCF548X_IRQ_H */

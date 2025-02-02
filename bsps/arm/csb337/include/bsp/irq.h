/**
 * @file
 *
 * @ingroup csb337_interrupt
 *
 * @brief Interrupt Support.
 */

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

#ifndef __IRQ_H__
#define __IRQ_H__

#ifndef __asm__

#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>

#endif /* __asm__ */

/**
 * @defgroup csb337_interrupt Interrrupt Support
 *
 * @ingroup RTEMSBSPsARMCSB337
 *
 * @brief Interrupt Support.
 */

/* possible interrupt sources on the AT91RM9200 */
#define AT91RM9200_INT_FIQ        0
#define AT91RM9200_INT_SYSIRQ     1
#define AT91RM9200_INT_PIOA       2
#define AT91RM9200_INT_PIOB       3
#define AT91RM9200_INT_PIOC       4
#define AT91RM9200_INT_PIOD       5
#define AT91RM9200_INT_US0        6
#define AT91RM9200_INT_US1        7
#define AT91RM9200_INT_US2        8
#define AT91RM9200_INT_US3        9
#define AT91RM9200_INT_MCI       10
#define AT91RM9200_INT_UDP       11
#define AT91RM9200_INT_TWI       12
#define AT91RM9200_INT_SPI       13
#define AT91RM9200_INT_SSC0      14
#define AT91RM9200_INT_SSC1      15
#define AT91RM9200_INT_SSC2      16
#define AT91RM9200_INT_TC0       17
#define AT91RM9200_INT_TC1       18
#define AT91RM9200_INT_TC2       19
#define AT91RM9200_INT_TC3       20
#define AT91RM9200_INT_TC4       21
#define AT91RM9200_INT_TC5       22
#define AT91RM9200_INT_UHP       23
#define AT91RM9200_INT_EMAC      24
#define AT91RM9200_INT_IRQ0      25
#define AT91RM9200_INT_IRQ1      26
#define AT91RM9200_INT_IRQ2      27
#define AT91RM9200_INT_IRQ3      28
#define AT91RM9200_INT_IRQ4      28
#define AT91RM9200_INT_IRQ5      30
#define AT91RM9200_INT_IRQ6      31
#define AT91RM9200_MAX_INT       32

#define BSP_INTERRUPT_VECTOR_COUNT AT91RM9200_MAX_INT

#endif /* __IRQ_H__ */

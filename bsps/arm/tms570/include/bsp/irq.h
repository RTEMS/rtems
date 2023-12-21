/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This header file provides TMS570 interrupt definitions.
 */

/*
 * Copyright (C) 2014 Premysl Houdek <kom541000@gmail.com>
 *
 * Google Summer of Code 2014 at
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
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

#ifndef LIBBSP_ARM_TMS570_IRQ_H
#define LIBBSP_ARM_TMS570_IRQ_H

#ifndef ASM
#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>
#endif

#define TMS570_IRQ_ESM_HIGH 0
#define TMS570_IRQ_RESERVED_0 1
#define TMS570_IRQ_TIMER_0 2
#define TMS570_IRQ_TIMER_1 3
#define TMS570_IRQ_TIMER_2 4
#define TMS570_IRQ_TIMER_3 5
#define TMS570_IRQ_RTI_OVERFLOW_0 6
#define TMS570_IRQ_RTI_OVERFLOW_1 7
#define TMS570_IRQ_RTI_TIMEBASE 8
#define TMS570_IRQ_GIO_HIGH 9
#define TMS570_IRQ_HET_HIGH 10
#define TMS570_IRQ_HET_TU_HIGH 11
#define TMS570_IRQ_MIBSPI1_HIGH 12
#define TMS570_IRQ_SCI_LEVEL_0 13
#define TMS570_IRQ_ADC1_EVENT 14
#define TMS570_IRQ_ADC1_GROUP_1 15
#define TMS570_IRQ_CAN1_HIGH 16
#define TMS570_IRQ_RESERVED_1 17
#define TMS570_IRQ_FLEXRAY_HIGH 18
#define TMS570_IRQ_CRC_1 19
#define TMS570_IRQ_ESM_LOW 20
#define TMS570_IRQ_SSI 21
#define TMS570_IRQ_PMU 22
#define TMS570_IRQ_GIO_LOW 23
#define TMS570_IRQ_HET_LOW 24
#define TMS570_IRQ_HET_TU_LOW 25
#define TMS570_IRQ_MIBSPI1_LOW 26
#define TMS570_IRQ_SCI_LEVEL_1 27
#define TMS570_IRQ_ADC1_GROUP_2 28
#define TMS570_IRQ_CAN1_LOW 29
#define TMS570_IRQ_RESERVED_2 30
#define TMS570_IRQ_ADC1_MAG 31
#define TMS570_IRQ_FLEXRAY_LOW 32
#define TMS570_IRQ_DMA_FTCA 33
#define TMS570_IRQ_DMA_LFSA 34
#define TMS570_IRQ_CAN2_HIGH 35
#define TMS570_IRQ_DMM_HIGH 36
#define TMS570_IRQ_MIBSPI3_HIGH 37
#define TMS570_IRQ_MIBSPI3_LOW 38
#define TMS570_IRQ_DMA_HBCA 39
#define TMS570_IRQ_DMA_BTCA 40
#define TMS570_IRQ_DMA_BERA 41
#define TMS570_IRQ_CAN2_LOW 42
#define TMS570_IRQ_DMM_LOW 43
#define TMS570_IRQ_CAN1_IF3 44
#define TMS570_IRQ_CAN3_HIGH 45
#define TMS570_IRQ_CAN2_IF3 46
#define TMS570_IRQ_FPU 47
#define TMS570_IRQ_FLEXRAY_TU 48
#define TMS570_IRQ_SPI4_HIGH 49
#define TMS570_IRQ_ADC2_EVENT 50
#define TMS570_IRQ_ADC2_GROUP_1 51
#define TMS570_IRQ_FLEXRAY_T0C 52
#define TMS570_IRQ_MIBSPIP5_HIGH 53
#define TMS570_IRQ_SPI4_LOW 54
#define TMS570_IRQ_CAN3_LOW 55
#define TMS570_IRQ_MIBSPIP5_LOW 56
#define TMS570_IRQ_ADC2_GROUP_2 57
#define TMS570_IRQ_FLEXRAY_TU_ERROR 58
#define TMS570_IRQ_ADC2_MAG 59
#define TMS570_IRQ_CAN3_IF3 60
#define TMS570_IRQ_FSM_DONE 61
#define TMS570_IRQ_FLEXRAY_T1C 62
#define TMS570_IRQ_HET2_LEVEL_0 63
#define TMS570_IRQ_SCI2_LEVEL_0 64
#define TMS570_IRQ_HET_TU2_LEVEL_0 65
#define TMS570_IRQ_IC2_INTERRUPT 66
#define TMS570_IRQ_HET2_LEVEL_1 73
#define TMS570_IRQ_SCI2_LEVEL_1 74
#define TMS570_IRQ_HET_TU2_LEVEL_1 75
#define TMS570_IRQ_EMAC_MISC 76
#define TMS570_IRQ_EMAC_TX   77
#define TMS570_IRQ_EMAC_THRESH 78
#define TMS570_IRQ_EMAC_RX   79
#define TMS570_IRQ_HWAG1_INT_REQ_H 80
#define TMS570_IRQ_HWAG2_INT_REQ_H 81
#define TMS570_IRQ_DCC_DONE_INTERRUPT 82
#define TMS570_IRQ_DCC2_DONE_INTERRUPT 83
#define TMS570_IRQ_HWAG1_INT_REQ_L 88
#define TMS570_IRQ_HWAG2_INT_REQ_L 89
#define BSP_INTERRUPT_VECTOR_COUNT 95

#define TMS570_IRQ_PRIORITY_VALUE_MIN 0U
#define TMS570_IRQ_PRIORITY_VALUE_MAX 0U

#define TMS570_IRQ_PRIORITY_COUNT ( TMS570_IRQ_PRIORITY_VALUE_MAX + 1U )
#define TMS570_IRQ_PRIORITY_HIGHEST TMS570_IRQ_PRIORITY_VALUE_MIN
#define TMS570_IRQ_PRIORITY_LOWEST TMS570_IRQ_PRIORITY_VALUE_MAX

#ifndef ASM

/**
 * @brief Sets the priority of the interrupt vector.
 *
 * The priority is defined by the VIM interrupt channel.  Firstly, the VIM
 * Interrupt Control (CHANCTRL) registers are searched to get the current
 * channel associated with the interrupt vector.  The interrupt vector of the
 * channel associated with the priority is assigned to this channel.  The
 * specified interrupt vector is assigned to the channel associated with the
 * priority.  So, this function swaps the channels of two interrupt vectors.
 *
 * @param vector is the number of the interrupt vector to set the priority.
 *
 * @param priority is the priority to set.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no interrupt vector associated with the
 *   number specified by ``vector``.
 *
 * @retval ::RTEMS_INVALID_PRIORITY The interrupt priority specified in
 *   ``priority`` was invalid.
 */
rtems_status_code tms570_irq_set_priority(
  rtems_vector_number vector,
  uint32_t            priority
);

/**
 * @brief Gets the priority of the interrupt vector.
 *
 * The priority is defined by the VIM interrupt channel.  The VIM Interrupt
 * Control (CHANCTRL) registers are searched to get the channel associated with
 * the interrupt vector.
 *
 * @param vector is the number of the interrupt vector to set the priority.
 *
 * @param priority is the priority to set.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``priority`` parameter was NULL.
 *
 * @retval ::RTEMS_INVALID_ID There was no interrupt vector associated with the
 *   number specified by ``vector``.
 *
 * @retval ::RTEMS_NOT_DEFINED The interrupt has no associated priority.
 */
rtems_status_code tms570_irq_get_priority(
  rtems_vector_number  vector,
  uint32_t            *priority
);

#endif /* ASM */

/** @} */

#endif /* LIBBSP_ARM_TMS570_IRQ_H */

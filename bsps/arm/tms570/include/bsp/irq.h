/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief TMS570 interrupt definitions.
 */

/*
 * Copyright (c) 2014 Premysl Houdek <kom541000@gmail.com>
 *
 * Google Summer of Code 2014 at
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * Based on LPC24xx and LPC1768 BSP
 * by embedded brains GmbH and others
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_TMS570_IRQ_H
#define LIBBSP_ARM_TMS570_IRQ_H

#ifndef ASM
#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>
#endif

#define BSP_INTERRUPT_VECTOR_MIN 0U
#define TMS570_IRQ_ESM_HIGH 0
#define TMS570_IRQ_RESERVED 1
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
#define TMS570_IRQ_RESERVED 17
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
#define TMS570_IRQ_RESERVED
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
#define TMS570_IRQ_HWA_INT_REQ_H 80
#define TMS570_IRQ_HWA_INT_REQ_H 81
#define TMS570_IRQ_DCC_DONE_INTERRUPT 82
#define TMS570_IRQ_DCC2_DONE_INTERRUPT 83
#define TMS570_IRQ_HWAG1_INT_REQ_L 88
#define TMS570_IRQ_HWAG2_INT_REQ_L 89
#define BSP_INTERRUPT_VECTOR_MAX 94

#define TMS570_IRQ_PRIORITY_VALUE_MIN 0U
#define TMS570_IRQ_PRIORITY_VALUE_MAX 0U

#define TMS570_IRQ_PRIORITY_COUNT ( TMS570_IRQ_PRIORITY_VALUE_MAX + 1U )
#define TMS570_IRQ_PRIORITY_HIGHEST TMS570_IRQ_PRIORITY_VALUE_MIN
#define TMS570_IRQ_PRIORITY_LOWEST TMS570_IRQ_PRIORITY_VALUE_MAX

#ifndef ASM

/**
 * @brief Set priority of the interrupt vector.
 *
 * This function is here because of compability. It should set
 * priority of the interrupt vector.
 * @warning It does not set any priority at HW layer. It is nearly imposible to
 * @warning set priority of the interrupt on TMS570 in a nice way.
 * @param[in] vector vector of isr
 * @param[in] priority new priority assigned to the vector
 * @return Void
 */
void tms570_irq_set_priority(
  rtems_vector_number vector,
  unsigned            priority
);

/**
 * @brief Gets priority of the interrupt vector.
 *
 * This function is here because of compability. It returns priority
 * of the isr vector last set by tms570_irq_set_priority function.
 *
 * @warning It does not return any real priority of the HW layer.
 * @param[in] vector vector of isr
 * @retval 0 vector is invalid.
 * @retval priority priority of the interrupt
 */
unsigned tms570_irq_get_priority( rtems_vector_number vector );

#endif /* ASM */

/** @} */

#endif /* LIBBSP_ARM_TMS570_IRQ_H */

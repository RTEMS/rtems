/**
 * @file
 *
 * @ingroup QorIQInterrupt
 *
 * @brief Interrupt API.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_POWERPC_QORIQ_IRQ_H
#define LIBBSP_POWERPC_QORIQ_IRQ_H

#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup QoriqInterruptP1020 QorIQ - P1020 Internal Interrupt Sources
 *
 * @ingroup QorIQInterrupt
 *
 * @brief P1020 internal interrupt sources.
 *
 * @{
 */

#define QORIQ_IRQ_ERROR 0
#define QORIQ_IRQ_ETSEC_TX_1_GROUP_1 1
#define QORIQ_IRQ_ETSEC_RX_1_GROUP_1 2
#define QORIQ_IRQ_ETSEC_ER_1_GROUP_1 8
#define QORIQ_IRQ_ETSEC_TX_3_GROUP_1 9
#define QORIQ_IRQ_ETSEC_RX_3_GROUP_1 10
#define QORIQ_IRQ_ETSEC_ER_3_GROUP_1 11
#define QORIQ_IRQ_ETSEC_TX_2_GROUP_1 35
#define QORIQ_IRQ_ETSEC_RX_2_GROUP_1 36
#define QORIQ_IRQ_TDM 46
#define QORIQ_IRQ_TDM_ERROR 47
#define QORIQ_IRQ_ETSEC_ER_2_GROUP_1 51

/** @} */

/**
 * @defgroup QoriqInterruptP2020 QorIQ - P2020 Internal Interrupt Sources
 *
 * @ingroup QorIQInterrupt
 *
 * @brief P2020 internal interrupt sources.
 *
 * @{
 */

#define QORIQ_IRQ_L2_CACHE 0
#define QORIQ_IRQ_ECM 1
#define QORIQ_IRQ_DDR_CONTROLLER 2
#define QORIQ_IRQ_PCI_EXPRESS_3 8
#define QORIQ_IRQ_PCI_EXPRESS_2 9
#define QORIQ_IRQ_PCI_EXPRESS_1 10
#define QORIQ_IRQ_SRIO_ERR_WRT_1_2 32
#define QORIQ_IRQ_SRIO_OUT_DOORBELL_1 33
#define QORIQ_IRQ_SRIO_IN_DOORBELL_1 34
#define QORIQ_IRQ_SRIO_OUT_MSG_1 37
#define QORIQ_IRQ_SRIO_IN_MSG_1 38
#define QORIQ_IRQ_SRIO_OUT_MSG_2 39
#define QORIQ_IRQ_SRIO_IN_MSG_2 40

/** @} */

/**
 * @defgroup QoriqInterruptAll QorIQ - Internal Interrupt Sources
 *
 * @ingroup QorIQInterrupt
 *
 * @brief Internal interrupt sources.
 *
 * @{
 */

#define QORIQ_IRQ_ELBC 3
#define QORIQ_IRQ_DMA_CHANNEL_1_1 4
#define QORIQ_IRQ_DMA_CHANNEL_2_1 5
#define QORIQ_IRQ_DMA_CHANNEL_3_1 6
#define QORIQ_IRQ_DMA_CHANNEL_4_1 7
#define QORIQ_IRQ_USB_1 12
#define QORIQ_IRQ_ETSEC_TX_1 13
#define QORIQ_IRQ_ETSEC_RX_1 14
#define QORIQ_IRQ_ETSEC_TX_3 15
#define QORIQ_IRQ_ETSEC_RX_3 16
#define QORIQ_IRQ_ETSEC_ER_3 17
#define QORIQ_IRQ_ETSEC_ER_1 18
#define QORIQ_IRQ_ETSEC_TX_2 19
#define QORIQ_IRQ_ETSEC_RX_2 20
#define QORIQ_IRQ_ETSEC_ER_2 24
#define QORIQ_IRQ_DUART 26
#define QORIQ_IRQ_I2C 27
#define QORIQ_IRQ_PERFORMANCE_MONITOR 28
#define QORIQ_IRQ_SECURITY_1 29
#define QORIQ_IRQ_USB_2 30
#define QORIQ_IRQ_GPIO 31
#define QORIQ_IRQ_SECURITY_2 42
#define QORIQ_IRQ_ESPI 43
#define QORIQ_IRQ_ETSEC_IEEE_1588_1 52
#define QORIQ_IRQ_ETSEC_IEEE_1588_2 53
#define QORIQ_IRQ_ETSEC_IEEE_1588_3 54
#define QORIQ_IRQ_ESDHC 56
#define QORIQ_IRQ_DMA_CHANNEL_1_2 60
#define QORIQ_IRQ_DMA_CHANNEL_2_2 61
#define QORIQ_IRQ_DMA_CHANNEL_3_2 62
#define QORIQ_IRQ_DMA_CHANNEL_4_2 63

/** @} */

/**
 * @defgroup QoriqInterruptExternal QorIQ - External Interrupt Sources
 *
 * @ingroup QorIQInterrupt
 *
 * @brief External interrupt sources.
 *
 * @{
 */

#define QORIQ_IRQ_EXT_BASE 64
#define QORIQ_IRQ_EXT_0 (QORIQ_IRQ_EXT_BASE + 0)
#define QORIQ_IRQ_EXT_1 (QORIQ_IRQ_EXT_BASE + 1)
#define QORIQ_IRQ_EXT_2 (QORIQ_IRQ_EXT_BASE + 2)
#define QORIQ_IRQ_EXT_3 (QORIQ_IRQ_EXT_BASE + 3)
#define QORIQ_IRQ_EXT_4 (QORIQ_IRQ_EXT_BASE + 4)
#define QORIQ_IRQ_EXT_5 (QORIQ_IRQ_EXT_BASE + 5)
#define QORIQ_IRQ_EXT_6 (QORIQ_IRQ_EXT_BASE + 6)
#define QORIQ_IRQ_EXT_7 (QORIQ_IRQ_EXT_BASE + 7)
#define QORIQ_IRQ_EXT_8 (QORIQ_IRQ_EXT_BASE + 8)
#define QORIQ_IRQ_EXT_9 (QORIQ_IRQ_EXT_BASE + 9)
#define QORIQ_IRQ_EXT_10 (QORIQ_IRQ_EXT_BASE + 10)
#define QORIQ_IRQ_EXT_11 (QORIQ_IRQ_EXT_BASE + 11)

/** @} */

/**
 * @defgroup QoriqInterruptIPI QorIQ - Interprocessor Interrupts
 *
 * @ingroup QorIQInterrupt
 *
 * @brief Interprocessor interrupts.
 *
 * @{
 */

#define QORIQ_IRQ_IPI_BASE (QORIQ_IRQ_EXT_11 + 1)
#define QORIQ_IRQ_IPI_0 (QORIQ_IRQ_IPI_BASE + 0)
#define QORIQ_IRQ_IPI_1 (QORIQ_IRQ_IPI_BASE + 1)
#define QORIQ_IRQ_IPI_2 (QORIQ_IRQ_IPI_BASE + 2)
#define QORIQ_IRQ_IPI_3 (QORIQ_IRQ_IPI_BASE + 3)

/** @} */

/**
 * @defgroup QoriqInterruptIPI QorIQ - Message Interrupts
 *
 * @ingroup QorIQInterrupt
 *
 * @brief Message interrupts.
 *
 * @{
 */

#define QORIQ_IRQ_MI_BASE (QORIQ_IRQ_IPI_3 + 1)
#define QORIQ_IRQ_MI_0 (QORIQ_IRQ_MI_BASE + 0)
#define QORIQ_IRQ_MI_1 (QORIQ_IRQ_MI_BASE + 1)
#define QORIQ_IRQ_MI_2 (QORIQ_IRQ_MI_BASE + 2)
#define QORIQ_IRQ_MI_3 (QORIQ_IRQ_MI_BASE + 3)
#define QORIQ_IRQ_MI_4 (QORIQ_IRQ_MI_BASE + 4)
#define QORIQ_IRQ_MI_5 (QORIQ_IRQ_MI_BASE + 5)
#define QORIQ_IRQ_MI_6 (QORIQ_IRQ_MI_BASE + 6)
#define QORIQ_IRQ_MI_7 (QORIQ_IRQ_MI_BASE + 7)

/** @} */

/**
 * @defgroup QoriqInterruptIPI QorIQ - Shared Message Signaled Interrupts
 *
 * @ingroup QorIQInterrupt
 *
 * @brief Shared message signaled interrupts.
 *
 * @{
 */

#define QORIQ_IRQ_MSI_BASE (QORIQ_IRQ_MI_7 + 1)
#define QORIQ_IRQ_MSI_0 (QORIQ_IRQ_MSI_BASE + 0)
#define QORIQ_IRQ_MSI_1 (QORIQ_IRQ_MSI_BASE + 1)
#define QORIQ_IRQ_MSI_2 (QORIQ_IRQ_MSI_BASE + 2)
#define QORIQ_IRQ_MSI_3 (QORIQ_IRQ_MSI_BASE + 3)
#define QORIQ_IRQ_MSI_4 (QORIQ_IRQ_MSI_BASE + 4)
#define QORIQ_IRQ_MSI_5 (QORIQ_IRQ_MSI_BASE + 5)
#define QORIQ_IRQ_MSI_6 (QORIQ_IRQ_MSI_BASE + 6)
#define QORIQ_IRQ_MSI_7 (QORIQ_IRQ_MSI_BASE + 7)

/** @} */

/**
 * @defgroup QoriqInterruptIPI QorIQ - Global Timer Interrupts
 *
 * @ingroup QorIQInterrupt
 *
 * @brief Global Timer interrupts.
 *
 * @{
 */

#define QORIQ_IRQ_GT_BASE (QORIQ_IRQ_MSI_7 + 1)
#define QORIQ_IRQ_GT_A_0 (QORIQ_IRQ_GT_BASE + 0)
#define QORIQ_IRQ_GT_A_1 (QORIQ_IRQ_GT_BASE + 1)
#define QORIQ_IRQ_GT_A_2 (QORIQ_IRQ_GT_BASE + 2)
#define QORIQ_IRQ_GT_A_3 (QORIQ_IRQ_GT_BASE + 3)
#define QORIQ_IRQ_GT_B_0 (QORIQ_IRQ_GT_BASE + 4)
#define QORIQ_IRQ_GT_B_1 (QORIQ_IRQ_GT_BASE + 5)
#define QORIQ_IRQ_GT_B_2 (QORIQ_IRQ_GT_BASE + 6)
#define QORIQ_IRQ_GT_B_3 (QORIQ_IRQ_GT_BASE + 7)

/** @} */

/**
 * @defgroup QorIQInterrupt QorIQ - Interrupt Support
 *
 * @ingroup QorIQ
 *
 * @brief Interrupt support.
 *
 * @{
 */

#define BSP_INTERRUPT_VECTOR_MIN 0
#define BSP_INTERRUPT_VECTOR_MAX QORIQ_IRQ_GT_B_3

#define QORIQ_PIC_PRIORITY_LOWEST 1
#define QORIQ_PIC_PRIORITY_HIGHEST 15
#define QORIQ_PIC_PRIORITY_DISABLED 0
#define QORIQ_PIC_PRIORITY_INVALID (QORIQ_PIC_PRIORITY_HIGHEST + 1)
#define QORIQ_PIC_PRIORITY_DEFAULT (QORIQ_PIC_PRIORITY_LOWEST + 1)
#define QORIQ_PIC_PRIORITY_IS_VALID(p) \
  ((p) >= QORIQ_PIC_PRIORITY_DISABLED && (p) <= QORIQ_PIC_PRIORITY_HIGHEST)

rtems_status_code qoriq_pic_set_priority(
  rtems_vector_number vector,
  int new_priority,
  int *old_priority
);

rtems_status_code qoriq_pic_set_affinity(
  rtems_vector_number vector,
  uint32_t processor_index
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_POWERPC_QORIQ_IRQ_H */

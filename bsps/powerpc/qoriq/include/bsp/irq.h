/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCQorIQInterrupt
 *
 * @brief Interrupt API.
 */

/*
 * Copyright (c) 2010-2015 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_POWERPC_QORIQ_IRQ_H
#define LIBBSP_POWERPC_QORIQ_IRQ_H

#include <bsp.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>
#include <rtems/score/processormask.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef QORIQ_IS_HYPERVISOR_GUEST

#define BSP_INTERRUPT_VECTOR_MAX 1023

#else /* !QORIQ_IS_HYPERVISOR_GUEST */

#define QORIQ_IRQ_ERROR 0

#if QORIQ_CHIP_IS_T_VARIANT(QORIQ_CHIP_VARIANT)

#define QORIQ_IRQ_PCI_EXPRESS_1 4
#define QORIQ_IRQ_PCI_EXPRESS_2 5
#define QORIQ_IRQ_PCI_EXPRESS_3 6
#define QORIQ_IRQ_PCI_EXPRESS_4 7
#define QORIQ_IRQ_PAMU 8
#define QORIQ_IRQ_IFC 9
#define QORIQ_IRQ_DMA_CHANNEL_1_1 12
#define QORIQ_IRQ_DMA_CHANNEL_1_2 13
#define QORIQ_IRQ_DMA_CHANNEL_1_3 14
#define QORIQ_IRQ_DMA_CHANNEL_1_4 15
#define QORIQ_IRQ_DMA_CHANNEL_2_1 16
#define QORIQ_IRQ_DMA_CHANNEL_2_2 17
#define QORIQ_IRQ_DMA_CHANNEL_2_3 18
#define QORIQ_IRQ_DMA_CHANNEL_2_4 19
#define QORIQ_IRQ_DUART_1 20
#define QORIQ_IRQ_DUART_2 21
#define QORIQ_IRQ_DUARL_I2C_1 22
#define QORIQ_IRQ_DUARL_I2C_2 23
#define QORIQ_IRQ_PCI_EXPRESS_1_INTA 24
#define QORIQ_IRQ_PCI_EXPRESS_2_INTA 25
#define QORIQ_IRQ_PCI_EXPRESS_3_INTA 26
#define QORIQ_IRQ_PCI_EXPRESS_4_INTA 27
#define QORIQ_IRQ_USB_1 28
#define QORIQ_IRQ_USB_2 29
#define QORIQ_IRQ_ESDHC 32
#define QORIQ_IRQ_PERF_MON 36
#define QORIQ_IRQ_ESPI 37
#define QORIQ_IRQ_GPIO_2 38
#define QORIQ_IRQ_GPIO_1 39
#define QORIQ_IRQ_SATA_1 52
#define QORIQ_IRQ_SATA_2 53
#define QORIQ_IRQ_DMA_CHANNEL_1_5 60
#define QORIQ_IRQ_DMA_CHANNEL_1_6 61
#define QORIQ_IRQ_DMA_CHANNEL_1_7 62
#define QORIQ_IRQ_DMA_CHANNEL_1_8 63
#define QORIQ_IRQ_DMA_CHANNEL_2_5 64
#define QORIQ_IRQ_DMA_CHANNEL_2_6 65
#define QORIQ_IRQ_DMA_CHANNEL_2_7 66
#define QORIQ_IRQ_DMA_CHANNEL_2_8 67
#define QORIQ_IRQ_EVENT_PROC_UNIT_1 68
#define QORIQ_IRQ_EVENT_PROC_UNIT_2 69
#define QORIQ_IRQ_GPIO_3 70
#define QORIQ_IRQ_GPIO_4 71
#define QORIQ_IRQ_SEC_5_2_JOB_QUEUE_1 72
#define QORIQ_IRQ_SEC_5_2_JOB_QUEUE_2 73
#define QORIQ_IRQ_SEC_5_2_JOB_QUEUE_3 74
#define QORIQ_IRQ_SEC_5_2_JOB_QUEUE_4 75
#define QORIQ_IRQ_SEC_5_2_GLOBAL_ERROR 76
#define QORIQ_IRQ_SEC_MON 77
#define QORIQ_IRQ_EVENT_PROC_UNIT_3 78
#define QORIQ_IRQ_EVENT_PROC_UNIT_4 79
#define QORIQ_IRQ_FRAME_MGR 80
#define QORIQ_IRQ_MDIO_1 84
#define QORIQ_IRQ_MDIO_2 85
#define QORIQ_IRQ_QUEUE_MGR_PORTAL_0 88
#define QORIQ_IRQ_BUFFER_MGR_PORTAL_0 89
#define QORIQ_IRQ_QUEUE_MGR_PORTAL_1 90
#define QORIQ_IRQ_BUFFER_MGR_PORTAL_1 91
#define QORIQ_IRQ_QUEUE_MGR_PORTAL_2 92
#define QORIQ_IRQ_BUFFER_MGR_PORTAL_2 93
#define QORIQ_IRQ_QUEUE_MGR_PORTAL_3 94
#define QORIQ_IRQ_BUFFER_MGR_PORTAL_3 95
#define QORIQ_IRQ_QUEUE_MGR_PORTAL_4 96
#define QORIQ_IRQ_BUFFER_MGR_PORTAL_4 97
#define QORIQ_IRQ_QUEUE_MGR_PORTAL_5 98
#define QORIQ_IRQ_BUFFER_MGR_PORTAL_5 99
#define QORIQ_IRQ_QUEUE_MGR_PORTAL_6 100
#define QORIQ_IRQ_BUFFER_MGR_PORTAL_6 101
#define QORIQ_IRQ_QUEUE_MGR_PORTAL_7 102
#define QORIQ_IRQ_BUFFER_MGR_PORTAL_7 103
#define QORIQ_IRQ_QUEUE_MGR_PORTAL_8 104
#define QORIQ_IRQ_BUFFER_MGR_PORTAL_8 105
#define QORIQ_IRQ_QUEUE_MGR_PORTAL_9 106
#define QORIQ_IRQ_BUFFER_MGR_PORTAL_9 107
#define QORIQ_IRQ_QUEUE_MGR_PORTAL_10 109
#define QORIQ_IRQ_BUFFER_MGR_PORTAL_10 109
#define QORIQ_IRQ_QUEUE_MGR_PORTAL_11 110
#define QORIQ_IRQ_BUFFER_MGR_PORTAL_11 111
#define QORIQ_IRQ_QUEUE_MGR_PORTAL_12 112
#define QORIQ_IRQ_BUFFER_MGR_PORTAL_12 113
#define QORIQ_IRQ_QUEUE_MGR_PORTAL_13 114
#define QORIQ_IRQ_BUFFER_MGR_PORTAL_13 115
#define QORIQ_IRQ_QUEUE_MGR_PORTAL_14 116
#define QORIQ_IRQ_BUFFER_MGR_PORTAL_14 117
#define QORIQ_IRQ_QUEUE_MGR_PORTAL_15 118
#define QORIQ_IRQ_BUFFER_MGR_PORTAL_15 119
#define QORIQ_IRQ_QUEUE_MGR_PORTAL_16 120
#define QORIQ_IRQ_BUFFER_MGR_PORTAL_16 121
#define QORIQ_IRQ_QUEUE_MGR_PORTAL_17 122
#define QORIQ_IRQ_BUFFER_MGR_PORTAL_17 123
#define QORIQ_IRQ_DMA_CHANNEL_3_1 240
#define QORIQ_IRQ_DMA_CHANNEL_3_2 241
#define QORIQ_IRQ_DMA_CHANNEL_3_3 242
#define QORIQ_IRQ_DMA_CHANNEL_3_4 243
#define QORIQ_IRQ_DMA_CHANNEL_3_5 244
#define QORIQ_IRQ_DMA_CHANNEL_3_6 245
#define QORIQ_IRQ_DMA_CHANNEL_3_7 246
#define QORIQ_IRQ_DMA_CHANNEL_3_8 247

#define QORIQ_IRQ_EXT_BASE 256

#else /* QORIQ_CHIP_VARIANT */

/**
 * @defgroup QoriqInterruptP1020 QorIQ - P1020 Internal Interrupt Sources
 *
 * @ingroup RTEMSBSPsPowerPCQorIQInterrupt
 *
 * @brief P1020 internal interrupt sources.
 *
 * @{
 */

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
 * @ingroup RTEMSBSPsPowerPCQorIQInterrupt
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
 * @ingroup RTEMSBSPsPowerPCQorIQInterrupt
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
#define QORIQ_IRQ_DUART_1 26
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

#define QORIQ_IRQ_EXT_BASE 64

#endif /* QORIQ_CHIP_VARIANT */

/**
 * @defgroup QoriqInterruptExternal QorIQ - External Interrupt Sources
 *
 * @ingroup RTEMSBSPsPowerPCQorIQInterrupt
 *
 * @brief External interrupt sources.
 *
 * @{
 */

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
 * @ingroup RTEMSBSPsPowerPCQorIQInterrupt
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
 * @ingroup RTEMSBSPsPowerPCQorIQInterrupt
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
 * @ingroup RTEMSBSPsPowerPCQorIQInterrupt
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
 * @ingroup RTEMSBSPsPowerPCQorIQInterrupt
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

#define BSP_INTERRUPT_VECTOR_MAX QORIQ_IRQ_GT_B_3

/** @} */

#endif /* QORIQ_IS_HYPERVISOR_GUEST */

/**
 * @defgroup RTEMSBSPsPowerPCQorIQInterrupt QorIQ - Interrupt Support
 *
 * @ingroup RTEMSBSPsPowerPCQorIQ
 *
 * @brief Interrupt support.
 *
 * @{
 */

#define BSP_INTERRUPT_VECTOR_MIN 0

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

void bsp_interrupt_set_affinity(
  rtems_vector_number vector,
  const Processor_mask *affinity
);

void bsp_interrupt_get_affinity(
  rtems_vector_number vector,
  Processor_mask *affinity
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_POWERPC_QORIQ_IRQ_H */

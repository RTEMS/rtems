/*
 * Copyright 2017-2020 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_FLEXRAM_H_
#define _FSL_FLEXRAM_H_

#include "fsl_common.h"
#include "fsl_flexram_allocate.h"

/*!
 * @addtogroup flexram
 * @{
 */

/******************************************************************************
 * Definitions.
 *****************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief Driver version 2.0.7. */
#define FSL_FLEXRAM_DRIVER_VERSION (MAKE_VERSION(2U, 0U, 7U))
/*@}*/

/*! @brief Flexram write/read selection. */
enum
{
    kFLEXRAM_Read  = 0U, /*!< read */
    kFLEXRAM_Write = 1U, /*!< write */
};

/*! @brief Interrupt status flag mask */
enum
{
    kFLEXRAM_OCRAMAccessError = FLEXRAM_INT_STATUS_OCRAM_ERR_STATUS_MASK, /*!< OCRAM accesses unallocated address */
    kFLEXRAM_DTCMAccessError  = FLEXRAM_INT_STATUS_DTCM_ERR_STATUS_MASK,  /*!< DTCM accesses unallocated address */
    kFLEXRAM_ITCMAccessError  = FLEXRAM_INT_STATUS_ITCM_ERR_STATUS_MASK,  /*!< ITCM accesses unallocated address */

#if defined(FSL_FEATURE_FLEXRAM_HAS_MAGIC_ADDR) && FSL_FEATURE_FLEXRAM_HAS_MAGIC_ADDR
    kFLEXRAM_OCRAMMagicAddrMatch = FLEXRAM_INT_STATUS_OCRAM_MAM_STATUS_MASK, /*!< OCRAM magic address match */
    kFLEXRAM_DTCMMagicAddrMatch  = FLEXRAM_INT_STATUS_DTCM_MAM_STATUS_MASK,  /*!< DTCM magic address match */
    kFLEXRAM_ITCMMagicAddrMatch  = FLEXRAM_INT_STATUS_ITCM_MAM_STATUS_MASK,  /*!< ITCM magic address match */

    kFLEXRAM_InterruptStatusAll = FLEXRAM_INT_STATUS_OCRAM_ERR_STATUS_MASK | FLEXRAM_INT_STATUS_DTCM_ERR_STATUS_MASK |
                                  FLEXRAM_INT_STATUS_ITCM_ERR_STATUS_MASK | FLEXRAM_INT_STATUS_OCRAM_MAM_STATUS_MASK |
                                  FLEXRAM_INT_STATUS_DTCM_MAM_STATUS_MASK | FLEXRAM_INT_STATUS_ITCM_MAM_STATUS_MASK,
/*!< all the interrupt status mask */
#else
    kFLEXRAM_InterruptStatusAll = FLEXRAM_INT_STATUS_OCRAM_ERR_STATUS_MASK | FLEXRAM_INT_STATUS_DTCM_ERR_STATUS_MASK |
                                  FLEXRAM_INT_STATUS_ITCM_ERR_STATUS_MASK, /*!< all the interrupt status mask */
#endif /* FSL_FEATURE_FLEXRAM_HAS_MAGIC_ADDR */
};

/*! @brief FLEXRAM TCM access mode.
 * Fast access mode expected to be finished in 1-cycle;
 * Wait access mode expected to be finished in 2-cycle.
 * Wait access mode is a feature of the flexram and it should be used when
 * the CPU clock is too fast to finish TCM access in 1-cycle.
 * Normally, fast mode is the default mode, the efficiency of the TCM access will better.
 */
typedef enum _flexram_tcm_access_mode
{
    kFLEXRAM_TCMAccessFastMode = 0U, /*!< fast access mode */
    kFLEXRAM_TCMAccessWaitMode = 1U, /*!< wait access mode */
} flexram_tcm_access_mode_t;

/*! @brief FLEXRAM TCM support size */
enum
{
    kFLEXRAM_TCMSize32KB  = 32 * 1024U,  /*!< TCM total size be 32KB */
    kFLEXRAM_TCMSize64KB  = 64 * 1024U,  /*!< TCM total size be 64KB */
    kFLEXRAM_TCMSize128KB = 128 * 1024U, /*!< TCM total size be 128KB */
    kFLEXRAM_TCMSize256KB = 256 * 1024U, /*!< TCM total size be 256KB */
    kFLEXRAM_TCMSize512KB = 512 * 1024U, /*!< TCM total size be 512KB */
};

/*******************************************************************************
 * APIs
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Initialization and de-initialization
 * @{
 */

/*!
 * @brief FLEXRAM module initialization function.
 *
 * @param base FLEXRAM base address.
 */
void FLEXRAM_Init(FLEXRAM_Type *base);

/*!
 * @brief De-initializes the FLEXRAM.
 *
 */
void FLEXRAN_Deinit(FLEXRAM_Type *base);

/* @} */

/*!
 * @name Status
 * @{
 */
/*!
 * @brief FLEXRAM module gets interrupt status.
 *
 * @param base FLEXRAM base address.
 */
static inline uint32_t FLEXRAM_GetInterruptStatus(FLEXRAM_Type *base)
{
    return base->INT_STATUS & (uint32_t)kFLEXRAM_InterruptStatusAll;
}

/*!
 * @brief FLEXRAM module clears interrupt status.
 *
 * @param base FLEXRAM base address.
 * @param status Status to be cleared.
 */
static inline void FLEXRAM_ClearInterruptStatus(FLEXRAM_Type *base, uint32_t status)
{
    base->INT_STATUS |= status;
}

/*!
 * @brief FLEXRAM module enables interrupt status.
 *
 * @param base FLEXRAM base address.
 * @param status Status to be enabled.
 */
static inline void FLEXRAM_EnableInterruptStatus(FLEXRAM_Type *base, uint32_t status)
{
    base->INT_STAT_EN |= status;
}

/*!
 * @brief FLEXRAM module disable interrupt status.
 *
 * @param base FLEXRAM base address.
 * @param status Status to be disabled.
 */
static inline void FLEXRAM_DisableInterruptStatus(FLEXRAM_Type *base, uint32_t status)
{
    base->INT_STAT_EN &= ~status;
}

/* @} */

/*!
 * @name Interrupts
 * @{
 */

/*!
 * @brief FLEXRAM module enables interrupt.
 *
 * @param base FLEXRAM base address.
 * @param status Status interrupt to be enabled.
 */
static inline void FLEXRAM_EnableInterruptSignal(FLEXRAM_Type *base, uint32_t status)
{
    base->INT_SIG_EN |= status;
}

/*!
 * @brief FLEXRAM module disables interrupt.
 *
 * @param base FLEXRAM base address.
 * @param status Status interrupt to be disabled.
 */
static inline void FLEXRAM_DisableInterruptSignal(FLEXRAM_Type *base, uint32_t status)
{
    base->INT_SIG_EN &= ~status;
}
/* @} */

/*!
 * @brief FLEXRAM module sets TCM read access mode
 *
 * @param base  FLEXRAM base address.
 * @param mode  Access mode.
 */
static inline void FLEXRAM_SetTCMReadAccessMode(FLEXRAM_Type *base, flexram_tcm_access_mode_t mode)
{
    base->TCM_CTRL &= ~FLEXRAM_TCM_CTRL_TCM_RWAIT_EN_MASK;
    base->TCM_CTRL |= (uint32_t)mode;
}

/*!
 * @brief FLEXRAM module set TCM write access mode
 *
 * @param base  FLEXRAM base address.
 * @param mode  Access mode.
 */
static inline void FLEXRAM_SetTCMWriteAccessMode(FLEXRAM_Type *base, flexram_tcm_access_mode_t mode)
{
    base->TCM_CTRL &= ~FLEXRAM_TCM_CTRL_TCM_WWAIT_EN_MASK;
    base->TCM_CTRL |= (uint32_t)mode;
}

/*!
 * @brief FLEXRAM module force ram clock on
 *
 * @param base  FLEXRAM base address.
 * @param enable Enable or disable clock force on.
 */
static inline void FLEXRAM_EnableForceRamClockOn(FLEXRAM_Type *base, bool enable)
{
    if (enable)
    {
        base->TCM_CTRL |= FLEXRAM_TCM_CTRL_FORCE_CLK_ON_MASK;
    }
    else
    {
        base->TCM_CTRL &= ~FLEXRAM_TCM_CTRL_FORCE_CLK_ON_MASK;
    }
}

#if defined(FSL_FEATURE_FLEXRAM_HAS_MAGIC_ADDR) && FSL_FEATURE_FLEXRAM_HAS_MAGIC_ADDR
/*!
 * @brief FLEXRAM OCRAM magic addr configuration.
 * When read/write access hit magic address, it will generate interrupt.
 * @param base  FLEXRAM base address.
 * @param magicAddr Magic address, the actual address bits [18:3] is corresponding to the register field [16:1].
 * @param rwSel Read/write selection. 0 for read access while 1 for write access.
 */
static inline void FLEXRAM_SetOCRAMMagicAddr(FLEXRAM_Type *base, uint16_t magicAddr, uint32_t rwSel)
{
    base->OCRAM_MAGIC_ADDR = FLEXRAM_OCRAM_MAGIC_ADDR_OCRAM_WR_RD_SEL(rwSel) |
                             FLEXRAM_OCRAM_MAGIC_ADDR_OCRAM_MAGIC_ADDR((uint32_t)magicAddr >> 3);
}

/*!
 * @brief FLEXRAM DTCM magic addr configuration.
 * When read/write access hits magic address, it will generate interrupt.
 * @param base  FLEXRAM base address.
 * @param magicAddr Magic address, the actual address bits [18:3] is corresponding to the register field [16:1].
 * @param rwSel Read/write selection. 0 for read access while 1 write access.
 */
static inline void FLEXRAM_SetDTCMMagicAddr(FLEXRAM_Type *base, uint16_t magicAddr, uint32_t rwSel)
{
    base->DTCM_MAGIC_ADDR = FLEXRAM_DTCM_MAGIC_ADDR_DTCM_WR_RD_SEL(rwSel) |
                            FLEXRAM_DTCM_MAGIC_ADDR_DTCM_MAGIC_ADDR((uint32_t)magicAddr >> 3);
}

/*!
 * @brief FLEXRAM ITCM magic addr configuration.
 * When read/write access hits magic address, it will generate interrupt.
 * @param base  FLEXRAM base address.
 * @param magicAddr Magic address, the actual address bits [18:3] is corresponding to the register field [16:1].
 * @param rwSel Read/write selection. 0 for read access while 1 for write access.
 */
static inline void FLEXRAM_SetITCMMagicAddr(FLEXRAM_Type *base, uint16_t magicAddr, uint32_t rwSel)
{
    base->ITCM_MAGIC_ADDR = FLEXRAM_ITCM_MAGIC_ADDR_ITCM_WR_RD_SEL(rwSel) |
                            FLEXRAM_ITCM_MAGIC_ADDR_ITCM_MAGIC_ADDR((uint32_t)magicAddr >> 3);
}
#endif /* FSL_FEATURE_FLEXRAM_HAS_MAGIC_ADDR */

#if defined(__cplusplus)
}
#endif

/*! @}*/

#endif

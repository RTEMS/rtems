/*
 * Copyright (c) 2018, Freescale Semiconductor, Inc.
 * Copyright 2019 - 2020, NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_PDM_SDMA_H_
#define _FSL_PDM_SDMA_H_

#include "fsl_pdm.h"
#include "fsl_sdma.h"

/*!
 * @addtogroup pdm_sdma PDM SDMA Driver
 * @ingroup pdm
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
#define FSL_PDM_SDMA_DRIVER_VERSION (MAKE_VERSION(2, 7, 0)) /*!< Version 2.7.0 */
/*@}*/

typedef struct _pdm_sdma_handle pdm_sdma_handle_t;

/*! @brief PDM eDMA transfer callback function for finish and error */
typedef void (*pdm_sdma_callback_t)(PDM_Type *base, pdm_sdma_handle_t *handle, status_t status, void *userData);

/*! @brief PDM DMA transfer handle, users should not touch the content of the handle.*/
struct _pdm_sdma_handle
{
    sdma_handle_t *dmaHandle;     /*!< DMA handler for PDM send */
    uint8_t nbytes;               /*!< eDMA minor byte transfer count initially configured. */
    uint8_t fifoWidth;            /*!< fifo width */
    uint8_t endChannel;           /*!< The last enabled channel */
    uint8_t channelNums;          /*!< total channel numbers */
    uint32_t count;               /*!< The transfer data count in a DMA request */
    uint32_t state;               /*!< Internal state for PDM eDMA transfer */
    uint32_t eventSource;         /*!< PDM event source number */
    pdm_sdma_callback_t callback; /*!< Callback for users while transfer finish or error occurs */
    void *userData;               /*!< User callback parameter */
    sdma_buffer_descriptor_t bdPool[PDM_XFER_QUEUE_SIZE]; /*!< BD pool for SDMA transfer. */
    pdm_transfer_t pdmQueue[PDM_XFER_QUEUE_SIZE];         /*!< Transfer queue storing queued transfer. */
    size_t transferSize[PDM_XFER_QUEUE_SIZE];             /*!< Data bytes need to transfer */
    volatile uint8_t queueUser;                           /*!< Index for user to queue transfer. */
    volatile uint8_t queueDriver;                         /*!< Index for driver to get the transfer data and size */
};

/*******************************************************************************
 * APIs
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name eDMA Transactional
 * @{
 */

/*!
 * @brief Initializes the PDM eDMA handle.
 *
 * This function initializes the PDM DMA handle, which can be used for other PDM master transactional APIs.
 * Usually, for a specified PDM instance, call this API once to get the initialized handle.
 *
 * @param base PDM base pointer.
 * @param handle PDM eDMA handle pointer.
 * @param callback Pointer to user callback function.
 * @param userData User parameter passed to the callback function.
 * @param dmaHandle eDMA handle pointer, this handle shall be static allocated by users.
 * @param eventSource PDM event source number.
 */
void PDM_TransferCreateHandleSDMA(PDM_Type *base,
                                  pdm_sdma_handle_t *handle,
                                  pdm_sdma_callback_t callback,
                                  void *userData,
                                  sdma_handle_t *dmaHandle,
                                  uint32_t eventSource);

/*!
 * @brief Performs a non-blocking PDM receive using eDMA.
 *
 * @note This interface returns immediately after the transfer initiates. Call
 * the PDM_GetReceiveRemainingBytes to poll the transfer status and check whether the PDM transfer is finished.
 *
 * @param base PDM base pointer
 * @param handle PDM eDMA handle pointer.
 * @param xfer Pointer to DMA transfer structure.
 * @retval kStatus_Success Start a PDM eDMA receive successfully.
 * @retval kStatus_InvalidArgument The input argument is invalid.
 * @retval kStatus_RxBusy PDM is busy receiving data.
 */
status_t PDM_TransferReceiveSDMA(PDM_Type *base, pdm_sdma_handle_t *handle, pdm_transfer_t *xfer);

/*!
 * @brief Aborts a PDM receive using eDMA.
 *
 * @param base PDM base pointer
 * @param handle PDM eDMA handle pointer.
 */
void PDM_TransferAbortReceiveSDMA(PDM_Type *base, pdm_sdma_handle_t *handle);

/*!
 * @brief PDM channel configurations.
 *
 * @param base PDM base pointer.
 * @param handle PDM eDMA handle pointer.
 * @param channel channel number.
 * @param config channel configurations.
 */
void PDM_SetChannelConfigSDMA(PDM_Type *base,
                              pdm_sdma_handle_t *handle,
                              uint32_t channel,
                              const pdm_channel_config_t *config);

/*!
 * @brief Terminate all the PDM sdma receive transfer.
 *
 * @param base PDM base pointer.
 * @param handle PDM SDMA handle pointer.
 */
void PDM_TransferTerminateReceiveSDMA(PDM_Type *base, pdm_sdma_handle_t *handle);

/*! @} */

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif

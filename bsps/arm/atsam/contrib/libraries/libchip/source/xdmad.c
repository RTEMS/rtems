/* ---------------------------------------------------------------------------- */
/*                  Atmel Microcontroller Software Support                      */
/*                       SAM Software Package License                           */
/* ---------------------------------------------------------------------------- */
/* Copyright (c) 2015, Atmel Corporation                                        */
/*                                                                              */
/* All rights reserved.                                                         */
/*                                                                              */
/* Redistribution and use in source and binary forms, with or without           */
/* modification, are permitted provided that the following condition is met:    */
/*                                                                              */
/* - Redistributions of source code must retain the above copyright notice,     */
/* this list of conditions and the disclaimer below.                            */
/*                                                                              */
/* Atmel's name may not be used to endorse or promote products derived from     */
/* this software without specific prior written permission.                     */
/*                                                                              */
/* DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR   */
/* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE   */
/* DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,      */
/* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT */
/* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,  */
/* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    */
/* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING         */
/* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, */
/* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                           */
/* ---------------------------------------------------------------------------- */

/** \addtogroup xdmad_module
 *
 * \section Xdma xDma Configuration Usage
 *
 * To configure a XDMA channel, the user has to follow these few steps :
 * <ul>
 * <li> Initialize a XDMA driver instance by XDMAD_Initialize().</li>
 * <li> choose an available (disabled) channel using XDMAD_AllocateChannel().</li>
 * <li> After the XDMAC selected channel has been programmed,
 * XDMAD_PrepareChannel() is to enable clock and dma peripheral of the DMA, and
 * set Configuration register to set up the transfer type (memory or non-memory
 * peripheral for source and destination) and flow control device.</li>
 * <li> Invoke XDMAD_StartTransfer() to start DMA transfer  or
 * XDMAD_StopTransfer() to force stop DMA transfer.</li>
 * <li> Once the buffer of data is transferred, XDMAD_IsTransferDone()
 * checks if DMA transfer is finished.</li>
 * <li> XDMAD_Handler() handles XDMA interrupt, and invoking XDMAD_SetCallback()
 * if provided.</li>
 * </ul>
 *
 * Related files:\n
 * \ref xdmad.h\n
 * \ref xdmad.c.\n
 */

/** \file */

/** \addtogroup dmad_functions
  @{*/

/*----------------------------------------------------------------------------
 *        Includes
 *----------------------------------------------------------------------------*/

#include "chip.h"
#ifdef __rtems__
#include "../../../utils/utility.h"
#include <rtems/irq-extension.h>
#include <rtems/sysinit.h>
#include <bsp/fatal.h>
#endif /* __rtems__ */
#include <assert.h>

/*----------------------------------------------------------------------------
 *        Local functions
 *----------------------------------------------------------------------------*/
static void XDMAD_Handler(void *arg);
/**
 * \brief Try to allocate a DMA channel for on given controller.
 * \param pDmad  Pointer to DMA driver instance.
 * \param bSrcID Source peripheral ID, 0xFF for memory.
 * \param bDstID Destination peripheral ID, 0xFF for memory.
 * \return Channel number if allocation successful, return
 * DMAD_ALLOC_FAILED if allocation failed.
 */
static uint32_t XDMAD_AllocateXdmacChannel(sXdmad *pXdmad,
		uint8_t bSrcID,
		uint8_t bDstID)
{
	uint32_t i;

	/* Can't support peripheral to peripheral */
	if (((bSrcID != XDMAD_TRANSFER_MEMORY)
		 && (bDstID != XDMAD_TRANSFER_MEMORY)))
		return XDMAD_ALLOC_FAILED;

	/* dma transfer from peripheral to memory */
	if (bDstID == XDMAD_TRANSFER_MEMORY) {
		if ((!XDMAIF_IsValidatedPeripherOnDma(bSrcID))) {
			TRACE_ERROR("%s:: Allocation failed", __FUNCTION__);
			return XDMAD_ALLOC_FAILED;
		}
	}

	/* dma transfer from memory to peripheral */
	if (bSrcID == XDMAD_TRANSFER_MEMORY) {
		if ((!XDMAIF_IsValidatedPeripherOnDma(bDstID))) {
			TRACE_ERROR("%s:: Allocation failed", __FUNCTION__);
			return XDMAD_ALLOC_FAILED;
		}
	}

	for (i = 0; i < pXdmad->numChannels; i ++) {
		if (pXdmad->XdmaChannels[i].state == XDMAD_STATE_FREE) {
			/* Allocate the channel */
			pXdmad->XdmaChannels[i].state = XDMAD_STATE_ALLOCATED;
			/* Get general informations */
			pXdmad->XdmaChannels[i].bSrcPeriphID = bSrcID;
			pXdmad->XdmaChannels[i].bDstPeriphID = bDstID;
			pXdmad->XdmaChannels[i].bSrcTxIfID =
				XDMAIF_Get_ChannelNumber(bSrcID, 0);
			pXdmad->XdmaChannels[i].bSrcRxIfID =
				XDMAIF_Get_ChannelNumber(bSrcID, 1);
			pXdmad->XdmaChannels[i].bDstTxIfID =
				XDMAIF_Get_ChannelNumber(bDstID, 0);
			pXdmad->XdmaChannels[i].bDstRxIfID =
				XDMAIF_Get_ChannelNumber(bDstID, 1);
			return  ((i) & 0xFF);
		}
	}

	TRACE_ERROR("%s:: Allocation failed, all channels are occupied", __FUNCTION__);
	return XDMAD_ALLOC_FAILED;
}

void XDMAD_DoNothingCallback(uint32_t Channel, void *pArg, uint32_t status)
{
	/* Do nothing */
}

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

sXdmad XDMAD_Instance;

static void XDMAD_SysInitialize(void)
{
	sXdmad *pXdmad;
	uint32_t j;
	rtems_status_code sc;

	pXdmad = &XDMAD_Instance;

	pXdmad->pXdmacs = XDMAC;
	pXdmad->numControllers = XDMAC_CONTROLLER_NUM;
	pXdmad->numChannels    = (XDMAC_GTYPE_NB_CH(XDMAC_GetType(XDMAC)) + 1);

	for (j = 0; j < pXdmad->numChannels; j ++) {
		pXdmad->XdmaChannels[j].fCallback = XDMAD_DoNothingCallback;
	}

	sc = rtems_interrupt_handler_install(
		ID_XDMAC,
		"XDMA",
		RTEMS_INTERRUPT_UNIQUE,
		XDMAD_Handler,
		pXdmad
	);
	if (sc != RTEMS_SUCCESSFUL) {
		bsp_fatal(ATSAM_FATAL_XDMA_IRQ_INSTALL);
	}
}

RTEMS_SYSINIT_ITEM(XDMAD_SysInitialize, RTEMS_SYSINIT_BSP_START,
    RTEMS_SYSINIT_ORDER_LAST);


/**
 * \brief Allocate a XDMA channel for upper layer.
 * \param pXdmad  Pointer to xDMA driver instance.
 * \param bSrcID Source peripheral ID, 0xFF for memory.
 * \param bDstID Destination peripheral ID, 0xFF for memory.
 * \return Channel number if allocation successful, return
 * XDMAD_ALLOC_FAILED if allocation failed.
 */
uint32_t XDMAD_AllocateChannel(sXdmad *pXdmad,
								uint8_t bSrcID,
								uint8_t bDstID)
{
	uint32_t dwChannel = XDMAD_ALLOC_FAILED;
	uint32_t volatile timer = 0x7FF;

	LockMutex(pXdmad->xdmaMutex, timer);
	dwChannel = XDMAD_AllocateXdmacChannel(pXdmad,  bSrcID, bDstID);
	ReleaseMutex(pXdmad->xdmaMutex);

	return dwChannel;
}

/**
 * \brief Free the specified xDMA channel.
 * \param pXdmad     Pointer to xDMA driver instance.
 * \param dwChannel ControllerNumber << 8 | ChannelNumber.
 */
eXdmadRC XDMAD_FreeChannel(sXdmad *pXdmad,
							uint32_t dwChannel)
{
	uint8_t iChannel    = (dwChannel) & 0xFF;
	assert(pXdmad != NULL);

	if (iChannel >= pXdmad->numChannels) return XDMAD_ERROR;

	switch (pXdmad->XdmaChannels[iChannel].state) {
	case XDMAD_STATE_ALLOCATED:
	case XDMAD_STATE_START:
	case XDMAD_STATE_IN_XFR:
		return XDMAD_BUSY;

	case XDMAD_STATE_DONE:
	case XDMAD_STATE_HALTED:
		pXdmad->XdmaChannels[iChannel].state = XDMAD_STATE_FREE;
		break;
	}

	return XDMAD_OK;
}

/**
 * \brief Set the callback function for xDMA channel transfer.
 * \param pXdmad     Pointer to xDMA driver instance.
 * \param dwChannel ControllerNumber << 8 | ChannelNumber.
 * \param fCallback Pointer to callback function.
 * \param pArg Pointer to optional argument for callback.
 */
eXdmadRC XDMAD_SetCallback(sXdmad *pXdmad,
							uint32_t dwChannel,
							XdmadTransferCallback fCallback,
							void *pArg)
{

	uint8_t iChannel    = (dwChannel) & 0xFF;
	assert(pXdmad != NULL);

	if (iChannel >= pXdmad->numChannels) return XDMAD_ERROR;

	if (pXdmad->XdmaChannels[iChannel].state == XDMAD_STATE_FREE)
		return XDMAD_ERROR;
	else if (pXdmad->XdmaChannels[iChannel].state == XDMAD_STATE_START)
		return XDMAD_BUSY;

	pXdmad->XdmaChannels[iChannel].fCallback = fCallback;
	pXdmad->XdmaChannels[iChannel].pArg = pArg;

	return XDMAD_OK;
}


/**
 * \brief Enable clock of the xDMA peripheral, Enable the dma peripheral,
 * configure configuration register for xDMA transfer.
 * \param pXdmad     Pointer to xDMA driver instance.
 * \param dwChannel ControllerNumber << 8 | ChannelNumber.
 * \param dwCfg     Configuration value.
 */
eXdmadRC XDMAD_PrepareChannel(sXdmad *pXdmad, uint32_t dwChannel)
{

	uint8_t iChannel    = (dwChannel) & 0xFF;
	Xdmac *pXdmac = pXdmad->pXdmacs;

	assert(pXdmad != NULL);

	if (iChannel >= pXdmad->numChannels) return XDMAD_ERROR;

	if (pXdmad->XdmaChannels[iChannel].state == XDMAD_STATE_FREE)
		return XDMAD_ERROR;
	else if ((pXdmad->XdmaChannels[iChannel].state == XDMAD_STATE_START)
			  || (pXdmad->XdmaChannels[iChannel].state == XDMAD_STATE_IN_XFR))
		return XDMAD_BUSY;


	/* Enable clock of the DMA peripheral */
	if (!PMC_IsPeriphEnabled(ID_XDMAC))
		PMC_EnablePeripheral(ID_XDMAC);

	/* Clear dummy status */
	XDMAC_GetChannelIsr(pXdmac, iChannel);
	/* Disables XDMAC interrupt for the given channel. */
	XDMAC_DisableGIt (pXdmac, iChannel);
	XDMAC_DisableChannelIt (pXdmac, iChannel, 0xFF);
	/* Disable the given dma channel. */
	XDMAC_DisableChannel(pXdmac, iChannel);
	XDMAC_SetSourceAddr(pXdmac, iChannel, 0);
	XDMAC_SetDestinationAddr(pXdmac, iChannel, 0);
	XDMAC_SetBlockControl(pXdmac, iChannel, 0);
	XDMAC_SetChannelConfig(pXdmac, iChannel, 0x20);
	XDMAC_SetDescriptorAddr(pXdmac, iChannel, 0, 0);
	XDMAC_SetDescriptorControl(pXdmac, iChannel, 0);
	return XDMAD_OK;
}

/**
 * \brief xDMA interrupt handler
 * \param arg Pointer to DMA driver instance.
 */
static void XDMAD_Handler(void *arg)
{
	sXdmad *pDmad;
	Xdmac *pXdmac;
	uint32_t xdmaGlobaIntStatus;

	pDmad = arg;
	pXdmac = pDmad->pXdmacs;
	xdmaGlobaIntStatus = XDMAC_GetGIsr(pXdmac) & 0xFFFFFF;

	while (xdmaGlobaIntStatus != 0) {
		uint8_t _iChannel;
		uint32_t xdmaChannelIntStatus;
		sXdmadChannel *pCh;

		_iChannel = 31 - __builtin_clz(xdmaGlobaIntStatus);
		xdmaChannelIntStatus = XDMAC_GetChannelIsr(pXdmac, _iChannel);
		pCh = &pDmad->XdmaChannels[_iChannel];

		xdmaGlobaIntStatus &= ~(UINT32_C(1) << _iChannel);

		/* Execute callback */
		pCh->fCallback(_iChannel, pCh->pArg, xdmaChannelIntStatus);
	}
}


/**
 * \brief Configure DMA for a single transfer.
 * \param pXdmad     Pointer to xDMA driver instance.
 * \param dwChannel ControllerNumber << 8 | ChannelNumber.
 */
eXdmadRC XDMAD_ConfigureTransfer(sXdmad *pXdmad,
								  uint32_t dwChannel,
								  sXdmadCfg *pXdmaParam,
								  uint32_t dwXdmaDescCfg,
								  uint32_t dwXdmaDescAddr,
								  uint32_t dwXdmaIntEn)
{
	uint8_t iChannel    = (dwChannel) & 0xFF;

	assert(pXdmad != NULL);

	if (iChannel >= pXdmad->numChannels)
		return XDMAD_ERROR;

	Xdmac *pXdmac = pXdmad->pXdmacs;
	XDMAC_GetChannelIsr(pXdmac, iChannel);

	if (pXdmad->XdmaChannels[iChannel].state == XDMAD_STATE_FREE)
		return XDMAD_ERROR;

	if (pXdmad->XdmaChannels[iChannel].state == XDMAD_STATE_START)
		return XDMAD_BUSY;

	/* Linked List is enabled */
	if ((dwXdmaDescCfg & XDMAC_CNDC_NDE) == XDMAC_CNDC_NDE_DSCR_FETCH_EN) {
		if ((dwXdmaDescCfg & XDMAC_CNDC_NDVIEW_Msk) == XDMAC_CNDC_NDVIEW_NDV0) {
			XDMAC_SetChannelConfig(pXdmac, iChannel, pXdmaParam->mbr_cfg);
			XDMAC_SetSourceAddr(pXdmac, iChannel, pXdmaParam->mbr_sa);
			XDMAC_SetDestinationAddr(pXdmac, iChannel, pXdmaParam->mbr_da);
		}

		if ((dwXdmaDescCfg & XDMAC_CNDC_NDVIEW_Msk) == XDMAC_CNDC_NDVIEW_NDV1)
			XDMAC_SetChannelConfig(pXdmac, iChannel, pXdmaParam->mbr_cfg);

		XDMAC_SetDescriptorAddr(pXdmac, iChannel, dwXdmaDescAddr, 0);
		XDMAC_SetDescriptorControl(pXdmac, iChannel, dwXdmaDescCfg);
		XDMAC_DisableChannelIt (pXdmac, iChannel, 0xFF);
		XDMAC_EnableChannelIt (pXdmac, iChannel, dwXdmaIntEn);
	} else {
		/* LLI is disabled. */
		XDMAC_SetSourceAddr(pXdmac, iChannel, pXdmaParam->mbr_sa);
		XDMAC_SetDestinationAddr(pXdmac, iChannel, pXdmaParam->mbr_da);
		XDMAC_SetMicroblockControl(pXdmac, iChannel, pXdmaParam->mbr_ubc);
		XDMAC_SetBlockControl(pXdmac, iChannel, pXdmaParam->mbr_bc);
		XDMAC_SetDataStride_MemPattern(pXdmac, iChannel, pXdmaParam->mbr_ds);
		XDMAC_SetSourceMicroBlockStride(pXdmac, iChannel, pXdmaParam->mbr_sus);
		XDMAC_SetDestinationMicroBlockStride(pXdmac, iChannel, pXdmaParam->mbr_dus);
		XDMAC_SetChannelConfig(pXdmac, iChannel, pXdmaParam->mbr_cfg);
		XDMAC_SetDescriptorAddr(pXdmac, iChannel, 0, 0);
		XDMAC_SetDescriptorControl(pXdmac, iChannel, 0);
		XDMAC_EnableChannelIt (pXdmac, iChannel, dwXdmaIntEn);
	}

	return XDMAD_OK;
}

/**
 * \brief Start xDMA transfer.
 * \param pXdmad     Pointer to XDMA driver instance.
 * \param dwChannel ControllerNumber << 8 | ChannelNumber.
 */
eXdmadRC XDMAD_StartTransfer(sXdmad *pXdmad, uint32_t dwChannel)
{
	uint8_t iChannel    = (dwChannel) & 0xFF;

	assert(pXdmad != NULL);

	if (iChannel >= pXdmad->numChannels) return XDMAD_ERROR;

	Xdmac *pXdmac = pXdmad->pXdmacs;

	if (pXdmad->XdmaChannels[iChannel].state == XDMAD_STATE_FREE) {
		TRACE_ERROR("%s:: XDMAD_STATE_FREE \n\r", __FUNCTION__);
		return XDMAD_ERROR;
	} else if (pXdmad->XdmaChannels[iChannel].state == XDMAD_STATE_START) {
		TRACE_ERROR("%s:: XDMAD_STATE_START \n\r", __FUNCTION__)
		return XDMAD_BUSY;
	}

	/* Change state to transferring */
	pXdmad->XdmaChannels[iChannel].state = XDMAD_STATE_START;
	XDMAC_EnableChannel(pXdmac, iChannel);
	XDMAC_EnableGIt(pXdmac, iChannel);

	return XDMAD_OK;
}


/**
 * \brief Stop DMA transfer.
 * \param pDmad     Pointer to DMA driver instance.
 * \param dwChannel ControllerNumber << 8 | ChannelNumber.
 */
eXdmadRC XDMAD_StopTransfer(sXdmad *pXdmad, uint32_t dwChannel)
{
	uint8_t _iChannel    = (dwChannel) & 0xFF;
	assert(pXdmad != NULL);

	if (_iChannel >= pXdmad->numChannels) return XDMAD_ERROR;

	Xdmac *pXdmac = pXdmad->pXdmacs;

	pXdmad->XdmaChannels[_iChannel].state = XDMAD_STATE_HALTED;
	/* Disable channel */
	XDMAC_DisableChannel(pXdmac, _iChannel);
	/* Disable interrupts */
	XDMAC_DisableChannelIt(pXdmac, _iChannel, 0xFF);
	/* Clear pending status */
	XDMAC_GetChannelIsr(pXdmac, _iChannel);
	XDMAC_GetGlobalChStatus(pXdmac);
	return XDMAD_OK;
}

/**@}*/


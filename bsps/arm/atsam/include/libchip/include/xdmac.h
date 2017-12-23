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

/** \file */

/** \addtogroup dmac_module Working with DMAC
 *  \ingroup peripherals_module
 *
 * \section Usage
 * <ul>
 * <li> Enable or disable the a DMAC controller with DMAC_Enable() and or
 * DMAC_Disable().</li>
 * <li> Enable or disable %Dma interrupt using DMAC_EnableIt()or
 * DMAC_DisableIt().</li>
 * <li> Get %Dma interrupt status by DMAC_GetStatus() and
 * DMAC_GetInterruptMask().</li>
 * <li> Enable or disable specified %Dma channel with DMAC_EnableChannel() or
 * DMAC_DisableChannel().</li>
 * <li> Get %Dma channel status by DMAC_GetChannelStatus().</li>
 * <li> ControlA and ControlB register is set by DMAC_SetControlA() and
 * DMAC_SetControlB().</li>
 * <li> Configure source and/or destination start address with
 * DMAC_SetSourceAddr() and/or DMAC_SetDestinationAddr().</li>
 * <li> Set %Dma descriptor address using DMAC_SetDescriptorAddr().</li>
 * <li> Set source transfer buffer size with DMAC_SetBufferSize().</li>
 * <li> Configure source and/or destination Picture-In-Picuture mode with
 * DMAC_SetSourcePip() and/or DMAC_SetDestPip().</li>
 * </ul>
 *
 * For more accurate information, please look at the DMAC section of the
 * Datasheet.
 *
 * \sa \ref dmad_module
 *
 * Related files :\n
 * \ref dmac.c\n
 * \ref dmac.h.\n
 *
 */

#ifndef DMAC_H
#define DMAC_H
/**@{*/

/*------------------------------------------------------------------------------
 *         Headers
 *----------------------------------------------------------------------------*/

#include "chip.h"

#ifndef __rtems__
#include <../../../../utils/utility.h>
#endif /* __rtems__ */
#include <stdint.h>
#include <rtems/score/assert.h>

/*------------------------------------------------------------------------------
 *         Definitions
 *----------------------------------------------------------------------------*/

/** \addtogroup dmac_defines DMAC Definitions
 *      @{
 */
/** Number of DMA channels */
#define XDMAC_CONTROLLER_NUM            1
/** Number of DMA channels */
#define XDMAC_CHANNEL_NUM               24
/** Max DMA single transfer size */
#define XDMAC_MAX_BT_SIZE               0xFFFF
/**     @}*/

/*----------------------------------------------------------------------------
 *         Macro
 *----------------------------------------------------------------------------*/
#define XDMA_GET_DATASIZE(size) ((size==0)? XDMAC_CC_DWIDTH_BYTE : \
								 ((size==1)? XDMAC_CC_DWIDTH_HALFWORD : \
								  (XDMAC_CC_DWIDTH_WORD)))
#define XDMA_GET_CC_SAM(s)      ((s==0)? XDMAC_CC_SAM_FIXED_AM : \
								 ((s==1)? XDMAC_CC_SAM_INCREMENTED_AM : \
								  ((s==2)? XDMAC_CC_SAM_UBS_AM : \
								   XDMAC_CC_SAM_UBS_DS_AM)))
#define XDMA_GET_CC_DAM(d)      ((d==0)? XDMAC_CC_DAM_FIXED_AM : \
								 ((d==1)? XDMAC_CC_DAM_INCREMENTED_AM : \
								  ((d==2)? XDMAC_CC_DAM_UBS_AM : \
								   XDMAC_CC_DAM_UBS_DS_AM)))
#define XDMA_GET_CC_MEMSET(m)   ((m==0)? XDMAC_CC_MEMSET_NORMAL_MODE : \
								 XDMAC_CC_MEMSET_HW_MODE)

/*------------------------------------------------------------------------------
 *         Global functions
 *----------------------------------------------------------------------------*/
/** \addtogroup dmac_functions
 *      @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Get XDMAC global type.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 */
static inline uint32_t XDMAC_GetType(Xdmac *pXdmac)
{
	_Assert(pXdmac);
	return pXdmac->XDMAC_GTYPE;
}

/**
 * \brief Get XDMAC global configuration.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 */
static inline uint32_t XDMAC_GetConfig(Xdmac *pXdmac)
{
	_Assert(pXdmac);
	return pXdmac->XDMAC_GCFG;
}

/**
 * \brief Get XDMAC global weighted arbiter configuration.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 */
static inline uint32_t XDMAC_GetArbiter(Xdmac *pXdmac)
{
	_Assert(pXdmac);
	return pXdmac->XDMAC_GWAC;
}

/**
 * \brief Enables XDMAC global interrupt.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param dwInteruptMask IT to be enabled.
 */
static inline void XDMAC_EnableGIt (Xdmac *pXdmac, uint8_t dwInteruptMask)
{
	_Assert(pXdmac);
	pXdmac->XDMAC_GIE = (XDMAC_GIE_IE0 << dwInteruptMask);
}

/**
 * \brief Disables XDMAC global interrupt
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param dwInteruptMask IT to be enabled
 */
static inline void XDMAC_DisableGIt (Xdmac *pXdmac, uint8_t dwInteruptMask)
{
	_Assert(pXdmac);
	pXdmac->XDMAC_GID = (XDMAC_GID_ID0 << dwInteruptMask);
}

/**
 * \brief Get XDMAC global interrupt mask.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 */
static inline uint32_t XDMAC_GetGItMask(Xdmac *pXdmac)
{
	_Assert(pXdmac);
	return (pXdmac->XDMAC_GIM);
}

/**
 * \brief Get XDMAC global interrupt status.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 */
static inline uint32_t XDMAC_GetGIsr(Xdmac *pXdmac)
{
	_Assert(pXdmac);
	return (pXdmac->XDMAC_GIS);
}

/**
 * \brief Get XDMAC masked global interrupt.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 */
static inline uint32_t XDMAC_GetMaskedGIsr(Xdmac *pXdmac)
{
	uint32_t _dwStatus;
	_Assert(pXdmac);
	_dwStatus = pXdmac->XDMAC_GIS;
	_dwStatus &= pXdmac->XDMAC_GIM;
	return _dwStatus;
}

/**
 * \brief enables the relevant channel of given XDMAC.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 */
static inline void XDMAC_EnableChannel(Xdmac *pXdmac, uint8_t channel)
{
	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	pXdmac->XDMAC_GE = (XDMAC_GE_EN0 << channel);
}

/**
 * \brief enables the relevant channels of given XDMAC.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param bmChannels Channels bitmap.
 */
static inline void XDMAC_EnableChannels(Xdmac *pXdmac, uint32_t bmChannels)
{
	_Assert(pXdmac);
	pXdmac->XDMAC_GE = bmChannels;
}

/**
 * \brief Disables the relevant channel of given XDMAC.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 */
static inline void XDMAC_DisableChannel(Xdmac *pXdmac, uint8_t channel)
{
	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	pXdmac->XDMAC_GD = (XDMAC_GD_DI0 << channel);
}

/**
 * \brief Disables the relevant channels of given XDMAC.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param bmChannels Channels bitmap.
 */
static inline void XDMAC_DisableChannels(Xdmac *pXdmac, uint32_t bmChannels)
{
	_Assert(pXdmac);
	pXdmac->XDMAC_GD = bmChannels;
}


/**
 * \brief Get Global channel status of given XDMAC.
 * \note: When set to 1, this bit indicates that the channel x is enabled.
   If a channel disable request is issued, this bit remains asserted
   until pending transaction is completed.
 * \param pXdmac Pointer to the XDMAC peripheral.
 */
static inline uint32_t XDMAC_GetGlobalChStatus(Xdmac *pXdmac)
{
	_Assert(pXdmac);
	return pXdmac->XDMAC_GS;
}

/**
 * \brief Suspend the relevant channel's read.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 */
static inline void XDMAC_SuspendReadChannel(Xdmac *pXdmac, uint8_t channel)
{
	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	pXdmac->XDMAC_GRS |= XDMAC_GRS_RS0 << channel;
}

/**
 * \brief Suspend the relevant channel's write.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 */
static inline void XDMAC_SuspendWriteChannel(Xdmac *pXdmac, uint8_t channel)
{
	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	pXdmac->XDMAC_GWS |= XDMAC_GWS_WS0 << channel;
}

/**
 * \brief Suspend the relevant channel's read & write.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 */
static inline void XDMAC_SuspendReadWriteChannel(Xdmac *pXdmac, uint8_t channel)
{
	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	pXdmac->XDMAC_GRWS = (XDMAC_GRWS_RWS0 << channel);
}

/**
 * \brief Resume the relevant channel's read & write.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 */
static inline void XDMAC_ResumeReadWriteChannel(Xdmac *pXdmac, uint8_t channel)
{
	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	pXdmac->XDMAC_GRWR = (XDMAC_GRWR_RWR0 << channel);
}

/**
 * \brief Set software transfer request on the relevant channel.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 */
static inline void XDMAC_SoftwareTransferReq(Xdmac *pXdmac, uint8_t channel)
{

	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	pXdmac->XDMAC_GSWR = (XDMAC_GSWR_SWREQ0 << channel);
}

/**
 * \brief Get software transfer status of the relevant channel.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 */
static inline uint32_t XDMAC_GetSoftwareTransferStatus(Xdmac *pXdmac)
{

	_Assert(pXdmac);
	return pXdmac->XDMAC_GSWS;
}

/**
 * \brief Get interrupt status for the relevant channel of given XDMA.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 */
static inline uint32_t XDMAC_GetChannelIsr (Xdmac *pXdmac, uint8_t channel)
{
	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	return pXdmac->XDMAC_CHID[channel].XDMAC_CIS;
}

/**
 * \brief Set software flush request on the relevant channel.
 * \note: This API is used as polling without enabling FIE interrupt.
 * The user can use it in interrupt mode after deleting while sentense.
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 */
static inline void XDMAC_SoftwareFlushReq(Xdmac *pXdmac, uint8_t channel)
{
	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	pXdmac->XDMAC_GSWF = (XDMAC_GSWF_SWF0 << channel);

	while (!(XDMAC_GetChannelIsr(pXdmac, channel) & XDMAC_CIS_FIS));
}

/**
 * \brief Disable interrupt with mask on the relevant channel of given XDMA.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 * \param dwInteruptMask Interrupt mask.
 */
static inline void XDMAC_EnableChannelIt (Xdmac *pXdmac, uint8_t channel,
							uint8_t dwInteruptMask)
{
	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	pXdmac->XDMAC_CHID[channel].XDMAC_CIE = dwInteruptMask;
}

/**
 * \brief Enable interrupt with mask on the relevant channel of given XDMA.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 * \param dwInteruptMask Interrupt mask.
 */
static inline void XDMAC_DisableChannelIt (Xdmac *pXdmac, uint8_t channel,
							 uint8_t dwInteruptMask)
{
	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	pXdmac->XDMAC_CHID[channel].XDMAC_CID = dwInteruptMask;
}

/**
 * \brief Get interrupt mask for the relevant channel of given XDMA.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 */
static inline uint32_t XDMAC_GetChannelItMask (Xdmac *pXdmac, uint8_t channel)
{
	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	return pXdmac->XDMAC_CHID[channel].XDMAC_CIM;
}

/**
 * \brief Get masked interrupt status for the relevant channel of given XDMA.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 */
static inline uint32_t XDMAC_GetMaskChannelIsr (Xdmac *pXdmac, uint8_t channel)
{
	uint32_t status;
	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	status = pXdmac->XDMAC_CHID[channel].XDMAC_CIS;
	status &= pXdmac->XDMAC_CHID[channel].XDMAC_CIM;

	return status;
}

/**
 * \brief Set source address for the relevant channel of given XDMA.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 * \param addr Source address.
 */
static inline void XDMAC_SetSourceAddr(Xdmac *pXdmac, uint8_t channel, uint32_t addr)
{
	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	pXdmac->XDMAC_CHID[channel].XDMAC_CSA = addr;
}

/**
 * \brief Set destination address for the relevant channel of given XDMA.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 * \param addr Destination address.
 */
static inline void XDMAC_SetDestinationAddr(Xdmac *pXdmac, uint8_t channel, uint32_t addr)
{
	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	pXdmac->XDMAC_CHID[channel].XDMAC_CDA = addr;
}

/**
 * \brief Set next descriptor's address & interface for the relevant channel of
 *  given XDMA.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 * \param addr Address of next descriptor.
 * \param ndaif Interface of next descriptor.
 */
static inline void XDMAC_SetDescriptorAddr(Xdmac *pXdmac, uint8_t channel,
							 uint32_t addr, uint8_t ndaif)
{
	_Assert(pXdmac);
	_Assert(ndaif < 2);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	pXdmac->XDMAC_CHID[channel].XDMAC_CNDA =  (addr & 0xFFFFFFFC) | ndaif;
}

/**
 * \brief Set next descriptor's configuration for the relevant channel of
 *  given XDMA.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 * \param config Configuration of next descriptor.
 */
static inline void XDMAC_SetDescriptorControl(Xdmac *pXdmac, uint8_t channel, uint8_t config)
{
	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	pXdmac->XDMAC_CHID[channel].XDMAC_CNDC = config;
}

/**
 * \brief Set microblock length for the relevant channel of given XDMA.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 * \param ublen Microblock length.
 */
static inline void XDMAC_SetMicroblockControl(Xdmac *pXdmac, uint8_t channel, uint32_t ublen)
{
	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	pXdmac->XDMAC_CHID[channel].XDMAC_CUBC = XDMAC_CUBC_UBLEN(ublen);
}

/**
 * \brief Set block length for the relevant channel of given XDMA.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 * \param blen Block length.
 */
static inline void XDMAC_SetBlockControl(Xdmac *pXdmac, uint8_t channel, uint16_t blen)
{
	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	pXdmac->XDMAC_CHID[channel].XDMAC_CBC = XDMAC_CBC_BLEN(blen);
}

/**
 * \brief Set configuration for the relevant channel of given XDMA.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 * \param config Channel configuration.
 */
static inline void XDMAC_SetChannelConfig(Xdmac *pXdmac, uint8_t channel, uint32_t config)
{
	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	pXdmac->XDMAC_CHID[channel].XDMAC_CC = config;
}

/**
 * \brief Get the relevant channel's configuration of given XDMA.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 */
static inline uint32_t XDMAC_GetChannelConfig(Xdmac *pXdmac, uint8_t channel)
{
	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	return pXdmac->XDMAC_CHID[channel].XDMAC_CC;
}

/**
 * \brief Set the relevant channel's data stride memory pattern of given XDMA.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 * \param dds_msp Data stride memory pattern.
 */
static inline void XDMAC_SetDataStride_MemPattern(Xdmac *pXdmac, uint8_t channel,
									uint32_t dds_msp)
{

	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	pXdmac->XDMAC_CHID[channel].XDMAC_CDS_MSP = dds_msp;
}

/**
 * \brief Set the relevant channel's source microblock stride of given XDMA.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 * \param subs Source microblock stride.
 */
static inline void XDMAC_SetSourceMicroBlockStride(Xdmac *pXdmac, uint8_t channel,
									 uint32_t subs)
{
	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	pXdmac->XDMAC_CHID[channel].XDMAC_CSUS = XDMAC_CSUS_SUBS(subs);
}

/**
 * \brief Set the relevant channel's destination microblock stride of given XDMA.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 * \param dubs Destination microblock stride.
 */
static inline void XDMAC_SetDestinationMicroBlockStride(Xdmac *pXdmac, uint8_t channel,
		uint32_t dubs)
{
	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	pXdmac->XDMAC_CHID[channel].XDMAC_CDUS = XDMAC_CDUS_DUBS(dubs);
}

/**
 * \brief Get the relevant channel's destination address of given XDMA.
 *
 * \param pXdmac Pointer to the XDMAC peripheral.
 * \param channel Particular channel number.
 */
static inline uint32_t XDMAC_GetChDestinationAddr(Xdmac *pXdmac, uint8_t channel)
{
	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	return pXdmac->XDMAC_CHID[channel].XDMAC_CDA;
}

static inline void XDMAC_StartTransfer(Xdmac *pXdmac, uint8_t channel)
{
	_Assert(pXdmac);
	_Assert(channel < XDMAC_CHANNEL_NUM);
	pXdmac->XDMAC_GE = (XDMAC_GE_EN0 << channel);
	pXdmac->XDMAC_GIE = (XDMAC_GIE_IE0 << channel);
}

#ifdef __cplusplus
}
#endif

/**     @}*/
/**@}*/
#endif //#ifndef DMAC_H


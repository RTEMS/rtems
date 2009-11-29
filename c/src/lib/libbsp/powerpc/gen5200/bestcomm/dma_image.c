/******************************************************************************
*
* Copyright (c) 2004 Freescale Semiconductor, Inc.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
* OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
******************************************************************************/

#include "dma_image.h"

TASK_PCI_TX_api_t  TASK_PCI_TX_storage;
TASK_PCI_TX_api_t *TASK_PCI_TX_api=&TASK_PCI_TX_storage;

TASK_PCI_RX_api_t  TASK_PCI_RX_storage;
TASK_PCI_RX_api_t *TASK_PCI_RX_api=&TASK_PCI_RX_storage;

TASK_FEC_TX_api_t  TASK_FEC_TX_storage;
TASK_FEC_TX_api_t *TASK_FEC_TX_api=&TASK_FEC_TX_storage;

TASK_FEC_RX_api_t  TASK_FEC_RX_storage;
TASK_FEC_RX_api_t *TASK_FEC_RX_api=&TASK_FEC_RX_storage;

TASK_LPC_api_t  TASK_LPC_storage;
TASK_LPC_api_t *TASK_LPC_api=&TASK_LPC_storage;

TASK_ATA_api_t  TASK_ATA_storage;
TASK_ATA_api_t *TASK_ATA_api=&TASK_ATA_storage;

TASK_CRC16_DP_0_api_t  TASK_CRC16_DP_0_storage;
TASK_CRC16_DP_0_api_t *TASK_CRC16_DP_0_api=&TASK_CRC16_DP_0_storage;

TASK_CRC16_DP_1_api_t  TASK_CRC16_DP_1_storage;
TASK_CRC16_DP_1_api_t *TASK_CRC16_DP_1_api=&TASK_CRC16_DP_1_storage;

TASK_GEN_DP_0_api_t  TASK_GEN_DP_0_storage;
TASK_GEN_DP_0_api_t *TASK_GEN_DP_0_api=&TASK_GEN_DP_0_storage;

TASK_GEN_DP_1_api_t  TASK_GEN_DP_1_storage;
TASK_GEN_DP_1_api_t *TASK_GEN_DP_1_api=&TASK_GEN_DP_1_storage;

TASK_GEN_DP_2_api_t  TASK_GEN_DP_2_storage;
TASK_GEN_DP_2_api_t *TASK_GEN_DP_2_api=&TASK_GEN_DP_2_storage;

TASK_GEN_DP_3_api_t  TASK_GEN_DP_3_storage;
TASK_GEN_DP_3_api_t *TASK_GEN_DP_3_api=&TASK_GEN_DP_3_storage;

TASK_GEN_TX_BD_api_t  TASK_GEN_TX_BD_storage;
TASK_GEN_TX_BD_api_t *TASK_GEN_TX_BD_api=&TASK_GEN_TX_BD_storage;

TASK_GEN_RX_BD_api_t  TASK_GEN_RX_BD_storage;
TASK_GEN_RX_BD_api_t *TASK_GEN_RX_BD_api=&TASK_GEN_RX_BD_storage;

TASK_GEN_DP_BD_0_api_t  TASK_GEN_DP_BD_0_storage;
TASK_GEN_DP_BD_0_api_t *TASK_GEN_DP_BD_0_api=&TASK_GEN_DP_BD_0_storage;

TASK_GEN_DP_BD_1_api_t  TASK_GEN_DP_BD_1_storage;
TASK_GEN_DP_BD_1_api_t *TASK_GEN_DP_BD_1_api=&TASK_GEN_DP_BD_1_storage;




void init_dma_image_TASK_PCI_TX(uint8 *taskBar, sint64 vMemOffset)
{
	uint8 *vMem_taskBar = (taskBar - vMemOffset);
	TASK_PCI_TX_api->TaskNum            = 0;
	TASK_PCI_TX_api->PtrStartTDT        = (volatile uint32 *)(vMem_taskBar + 0x0000UL);
	TASK_PCI_TX_api->PtrEndTDT          = (volatile uint32 *)(vMem_taskBar + 0x0004UL);
	TASK_PCI_TX_api->PtrVarTab          = (volatile uint32 *)(vMem_taskBar + 0x0008UL);
	TASK_PCI_TX_api->PtrFDT             = (volatile uint32 *)(vMem_taskBar + 0x000cUL);
	TASK_PCI_TX_api->PtrCSave           = (volatile uint32 *)(vMem_taskBar + 0x0018UL);
	TASK_PCI_TX_api->TaskPragma         = (volatile uint8  *)(TASK_PCI_TX_api->PtrFDT)+3;
	TASK_PCI_TX_api->NumDRD             = 7;
	TASK_PCI_TX_api->DRD[0]             = (volatile uint32 *)(volatile uint32)(*(TASK_PCI_TX_api->PtrStartTDT) + 0x0008UL - vMemOffset);
	TASK_PCI_TX_api->DRD[1]             = (volatile uint32 *)(volatile uint32)(*(TASK_PCI_TX_api->PtrStartTDT) + 0x0010UL - vMemOffset);
	TASK_PCI_TX_api->DRD[2]             = (volatile uint32 *)(volatile uint32)(*(TASK_PCI_TX_api->PtrStartTDT) + 0x0018UL - vMemOffset);
	TASK_PCI_TX_api->DRD[3]             = (volatile uint32 *)(volatile uint32)(*(TASK_PCI_TX_api->PtrStartTDT) + 0x0020UL - vMemOffset);
	TASK_PCI_TX_api->DRD[4]             = (volatile uint32 *)(volatile uint32)(*(TASK_PCI_TX_api->PtrStartTDT) + 0x0024UL - vMemOffset);
	TASK_PCI_TX_api->DRD[5]             = (volatile uint32 *)(volatile uint32)(*(TASK_PCI_TX_api->PtrStartTDT) + 0x002cUL - vMemOffset);
	TASK_PCI_TX_api->DRD[6]             = (volatile uint32 *)(volatile uint32)(*(TASK_PCI_TX_api->PtrStartTDT) + 0x0038UL - vMemOffset);
	TASK_PCI_TX_api->NumVar             = 12;
	TASK_PCI_TX_api->var                = (volatile uint32 *)(volatile uint32)(*(TASK_PCI_TX_api->PtrVarTab) - vMemOffset);
	TASK_PCI_TX_api->NumInc             = 5;
	TASK_PCI_TX_api->inc                = (volatile uint32 *)(volatile uint32)(*(TASK_PCI_TX_api->PtrVarTab) + (4*24) - vMemOffset);
	TASK_PCI_TX_api->AddrDstFIFO        = &(TASK_PCI_TX_api->var[0]);
	TASK_PCI_TX_api->IncrBytes          = (volatile sint16 *)&(TASK_PCI_TX_api->inc[0])+1;
	TASK_PCI_TX_api->AddrPktSizeReg     = &(TASK_PCI_TX_api->var[1]);
	TASK_PCI_TX_api->IncrSrc            = (volatile sint16 *)&(TASK_PCI_TX_api->inc[1])+1;
	TASK_PCI_TX_api->AddrSCStatusReg    = &(TASK_PCI_TX_api->var[2]);
	TASK_PCI_TX_api->Bytes              = &(TASK_PCI_TX_api->var[3]);
	TASK_PCI_TX_api->IterExtra          = &(TASK_PCI_TX_api->var[4]);
	TASK_PCI_TX_api->StartAddrSrc       = &(TASK_PCI_TX_api->var[7]);
}


void init_dma_image_TASK_PCI_RX(uint8 *taskBar, sint64 vMemOffset)
{
	uint8 *vMem_taskBar = (taskBar - vMemOffset);
	TASK_PCI_RX_api->TaskNum            = 1;
	TASK_PCI_RX_api->PtrStartTDT        = (volatile uint32 *)(vMem_taskBar + 0x0020UL);
	TASK_PCI_RX_api->PtrEndTDT          = (volatile uint32 *)(vMem_taskBar + 0x0024UL);
	TASK_PCI_RX_api->PtrVarTab          = (volatile uint32 *)(vMem_taskBar + 0x0028UL);
	TASK_PCI_RX_api->PtrFDT             = (volatile uint32 *)(vMem_taskBar + 0x002cUL);
	TASK_PCI_RX_api->PtrCSave           = (volatile uint32 *)(vMem_taskBar + 0x0038UL);
	TASK_PCI_RX_api->TaskPragma         = (volatile uint8  *)(TASK_PCI_RX_api->PtrFDT)+3;
	TASK_PCI_RX_api->NumDRD             = 5;
	TASK_PCI_RX_api->DRD[0]             = (volatile uint32 *)(volatile uint32)(*(TASK_PCI_RX_api->PtrStartTDT) + 0x0008UL - vMemOffset);
	TASK_PCI_RX_api->DRD[1]             = (volatile uint32 *)(volatile uint32)(*(TASK_PCI_RX_api->PtrStartTDT) + 0x0010UL - vMemOffset);
	TASK_PCI_RX_api->DRD[2]             = (volatile uint32 *)(volatile uint32)(*(TASK_PCI_RX_api->PtrStartTDT) + 0x0018UL - vMemOffset);
	TASK_PCI_RX_api->DRD[3]             = (volatile uint32 *)(volatile uint32)(*(TASK_PCI_RX_api->PtrStartTDT) + 0x0020UL - vMemOffset);
	TASK_PCI_RX_api->DRD[4]             = (volatile uint32 *)(volatile uint32)(*(TASK_PCI_RX_api->PtrStartTDT) + 0x002cUL - vMemOffset);
	TASK_PCI_RX_api->NumVar             = 9;
	TASK_PCI_RX_api->var                = (volatile uint32 *)(volatile uint32)(*(TASK_PCI_RX_api->PtrVarTab) - vMemOffset);
	TASK_PCI_RX_api->NumInc             = 4;
	TASK_PCI_RX_api->inc                = (volatile uint32 *)(volatile uint32)(*(TASK_PCI_RX_api->PtrVarTab) + (4*24) - vMemOffset);
	TASK_PCI_RX_api->AddrPktSizeReg     = &(TASK_PCI_RX_api->var[0]);
	TASK_PCI_RX_api->IncrBytes          = (volatile sint16 *)&(TASK_PCI_RX_api->inc[0])+1;
	TASK_PCI_RX_api->AddrSrcFIFO        = &(TASK_PCI_RX_api->var[1]);
	TASK_PCI_RX_api->IncrDst            = (volatile sint16 *)&(TASK_PCI_RX_api->inc[1])+1;
	TASK_PCI_RX_api->Bytes              = &(TASK_PCI_RX_api->var[2]);
	TASK_PCI_RX_api->IterExtra          = &(TASK_PCI_RX_api->var[3]);
	TASK_PCI_RX_api->StartAddrDst       = &(TASK_PCI_RX_api->var[6]);
}


void init_dma_image_TASK_FEC_TX(uint8 *taskBar, sint64 vMemOffset)
{
	uint8 *vMem_taskBar = (taskBar - vMemOffset);
	TASK_FEC_TX_api->TaskNum            = 2;
	TASK_FEC_TX_api->PtrStartTDT        = (volatile uint32 *)(vMem_taskBar + 0x0040UL);
	TASK_FEC_TX_api->PtrEndTDT          = (volatile uint32 *)(vMem_taskBar + 0x0044UL);
	TASK_FEC_TX_api->PtrVarTab          = (volatile uint32 *)(vMem_taskBar + 0x0048UL);
	TASK_FEC_TX_api->PtrFDT             = (volatile uint32 *)(vMem_taskBar + 0x004cUL);
	TASK_FEC_TX_api->PtrCSave           = (volatile uint32 *)(vMem_taskBar + 0x0058UL);
	TASK_FEC_TX_api->TaskPragma         = (volatile uint8  *)(TASK_FEC_TX_api->PtrFDT)+3;
	TASK_FEC_TX_api->NumDRD             = 22;
	TASK_FEC_TX_api->DRD[0]             = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrStartTDT) + 0x0004UL - vMemOffset);
	TASK_FEC_TX_api->DRD[1]             = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrStartTDT) + 0x0008UL - vMemOffset);
	TASK_FEC_TX_api->DRD[2]             = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrStartTDT) + 0x0010UL - vMemOffset);
	TASK_FEC_TX_api->DRD[3]             = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrStartTDT) + 0x001cUL - vMemOffset);
	TASK_FEC_TX_api->DRD[4]             = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrStartTDT) + 0x0020UL - vMemOffset);
	TASK_FEC_TX_api->DRD[5]             = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrStartTDT) + 0x0024UL - vMemOffset);
	TASK_FEC_TX_api->DRD[6]             = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrStartTDT) + 0x002cUL - vMemOffset);
	TASK_FEC_TX_api->DRD[7]             = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrStartTDT) + 0x0030UL - vMemOffset);
	TASK_FEC_TX_api->DRD[8]             = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrStartTDT) + 0x0034UL - vMemOffset);
	TASK_FEC_TX_api->DRD[9]             = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrStartTDT) + 0x0038UL - vMemOffset);
	TASK_FEC_TX_api->DRD[10]            = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrStartTDT) + 0x0044UL - vMemOffset);
	TASK_FEC_TX_api->DRD[11]            = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrStartTDT) + 0x0048UL - vMemOffset);
	TASK_FEC_TX_api->DRD[12]            = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrStartTDT) + 0x004cUL - vMemOffset);
	TASK_FEC_TX_api->DRD[13]            = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrStartTDT) + 0x0054UL - vMemOffset);
	TASK_FEC_TX_api->DRD[14]            = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrStartTDT) + 0x0060UL - vMemOffset);
	TASK_FEC_TX_api->DRD[15]            = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrStartTDT) + 0x0064UL - vMemOffset);
	TASK_FEC_TX_api->DRD[16]            = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrStartTDT) + 0x0068UL - vMemOffset);
	TASK_FEC_TX_api->DRD[17]            = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrStartTDT) + 0x006cUL - vMemOffset);
	TASK_FEC_TX_api->DRD[18]            = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrStartTDT) + 0x0074UL - vMemOffset);
	TASK_FEC_TX_api->DRD[19]            = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrStartTDT) + 0x007cUL - vMemOffset);
	TASK_FEC_TX_api->DRD[20]            = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrStartTDT) + 0x0084UL - vMemOffset);
	TASK_FEC_TX_api->DRD[21]            = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrStartTDT) + 0x0088UL - vMemOffset);
	TASK_FEC_TX_api->NumVar             = 20;
	TASK_FEC_TX_api->var                = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrVarTab) - vMemOffset);
	TASK_FEC_TX_api->NumInc             = 7;
	TASK_FEC_TX_api->inc                = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_TX_api->PtrVarTab) + (4*24) - vMemOffset);
	TASK_FEC_TX_api->AddrDRD            = &(TASK_FEC_TX_api->var[0]);
	TASK_FEC_TX_api->AddrDRDIdx         = 19;
	*TASK_FEC_TX_api->AddrDRD           = (volatile uint32)(TASK_FEC_TX_api->DRD[19]);
	TASK_FEC_TX_api->IncrBytes          = (volatile sint16 *)&(TASK_FEC_TX_api->inc[0])+1;
	TASK_FEC_TX_api->AddrDstFIFO        = &(TASK_FEC_TX_api->var[1]);
	TASK_FEC_TX_api->IncrSrc            = (volatile sint16 *)&(TASK_FEC_TX_api->inc[1])+1;
	TASK_FEC_TX_api->AddrEnable         = &(TASK_FEC_TX_api->var[2]);
	TASK_FEC_TX_api->IncrSrcMA          = (volatile sint16 *)&(TASK_FEC_TX_api->inc[2])+1;
	TASK_FEC_TX_api->BDTableBase        = &(TASK_FEC_TX_api->var[3]);
	TASK_FEC_TX_api->BDTableLast        = &(TASK_FEC_TX_api->var[4]);
	TASK_FEC_TX_api->BDTableStart       = &(TASK_FEC_TX_api->var[5]);
	TASK_FEC_TX_api->Bytes              = &(TASK_FEC_TX_api->var[6]);
}


void init_dma_image_TASK_FEC_RX(uint8 *taskBar, sint64 vMemOffset)
{
	uint8 *vMem_taskBar = (taskBar - vMemOffset);
	TASK_FEC_RX_api->TaskNum            = 3;
	TASK_FEC_RX_api->PtrStartTDT        = (volatile uint32 *)(vMem_taskBar + 0x0060UL);
	TASK_FEC_RX_api->PtrEndTDT          = (volatile uint32 *)(vMem_taskBar + 0x0064UL);
	TASK_FEC_RX_api->PtrVarTab          = (volatile uint32 *)(vMem_taskBar + 0x0068UL);
	TASK_FEC_RX_api->PtrFDT             = (volatile uint32 *)(vMem_taskBar + 0x006cUL);
	TASK_FEC_RX_api->PtrCSave           = (volatile uint32 *)(vMem_taskBar + 0x0078UL);
	TASK_FEC_RX_api->TaskPragma         = (volatile uint8  *)(TASK_FEC_RX_api->PtrFDT)+3;
	TASK_FEC_RX_api->NumDRD             = 13;
	TASK_FEC_RX_api->DRD[0]             = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_RX_api->PtrStartTDT) + 0x0004UL - vMemOffset);
	TASK_FEC_RX_api->DRD[1]             = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_RX_api->PtrStartTDT) + 0x000cUL - vMemOffset);
	TASK_FEC_RX_api->DRD[2]             = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_RX_api->PtrStartTDT) + 0x0010UL - vMemOffset);
	TASK_FEC_RX_api->DRD[3]             = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_RX_api->PtrStartTDT) + 0x0014UL - vMemOffset);
	TASK_FEC_RX_api->DRD[4]             = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_RX_api->PtrStartTDT) + 0x0020UL - vMemOffset);
	TASK_FEC_RX_api->DRD[5]             = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_RX_api->PtrStartTDT) + 0x0024UL - vMemOffset);
	TASK_FEC_RX_api->DRD[6]             = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_RX_api->PtrStartTDT) + 0x002cUL - vMemOffset);
	TASK_FEC_RX_api->DRD[7]             = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_RX_api->PtrStartTDT) + 0x0038UL - vMemOffset);
	TASK_FEC_RX_api->DRD[8]             = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_RX_api->PtrStartTDT) + 0x003cUL - vMemOffset);
	TASK_FEC_RX_api->DRD[9]             = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_RX_api->PtrStartTDT) + 0x0040UL - vMemOffset);
	TASK_FEC_RX_api->DRD[10]            = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_RX_api->PtrStartTDT) + 0x0048UL - vMemOffset);
	TASK_FEC_RX_api->DRD[11]            = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_RX_api->PtrStartTDT) + 0x0050UL - vMemOffset);
	TASK_FEC_RX_api->DRD[12]            = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_RX_api->PtrStartTDT) + 0x0058UL - vMemOffset);
	TASK_FEC_RX_api->NumVar             = 15;
	TASK_FEC_RX_api->var                = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_RX_api->PtrVarTab) - vMemOffset);
	TASK_FEC_RX_api->NumInc             = 7;
	TASK_FEC_RX_api->inc                = (volatile uint32 *)(volatile uint32)(*(TASK_FEC_RX_api->PtrVarTab) + (4*24) - vMemOffset);
	TASK_FEC_RX_api->AddrEnable         = &(TASK_FEC_RX_api->var[0]);
	TASK_FEC_RX_api->IncrBytes          = (volatile sint16 *)&(TASK_FEC_RX_api->inc[0])+1;
	TASK_FEC_RX_api->AddrSrcFIFO        = &(TASK_FEC_RX_api->var[1]);
	TASK_FEC_RX_api->IncrDst            = (volatile sint16 *)&(TASK_FEC_RX_api->inc[1])+1;
	TASK_FEC_RX_api->BDTableBase        = &(TASK_FEC_RX_api->var[2]);
	TASK_FEC_RX_api->IncrDstMA          = (volatile sint16 *)&(TASK_FEC_RX_api->inc[2])+1;
	TASK_FEC_RX_api->BDTableLast        = &(TASK_FEC_RX_api->var[3]);
	TASK_FEC_RX_api->BDTableStart       = &(TASK_FEC_RX_api->var[4]);
	TASK_FEC_RX_api->Bytes              = &(TASK_FEC_RX_api->var[5]);
}


void init_dma_image_TASK_LPC(uint8 *taskBar, sint64 vMemOffset)
{
	uint8 *vMem_taskBar = (taskBar - vMemOffset);
	TASK_LPC_api->TaskNum               = 4;
	TASK_LPC_api->PtrStartTDT           = (volatile uint32 *)(vMem_taskBar + 0x0080UL);
	TASK_LPC_api->PtrEndTDT             = (volatile uint32 *)(vMem_taskBar + 0x0084UL);
	TASK_LPC_api->PtrVarTab             = (volatile uint32 *)(vMem_taskBar + 0x0088UL);
	TASK_LPC_api->PtrFDT                = (volatile uint32 *)(vMem_taskBar + 0x008cUL);
	TASK_LPC_api->PtrCSave              = (volatile uint32 *)(vMem_taskBar + 0x0098UL);
	TASK_LPC_api->TaskPragma            = (volatile uint8  *)(TASK_LPC_api->PtrFDT)+3;
	TASK_LPC_api->NumDRD                = 4;
	TASK_LPC_api->DRD[0]                = (volatile uint32 *)(volatile uint32)(*(TASK_LPC_api->PtrStartTDT) + 0x000cUL - vMemOffset);
	TASK_LPC_api->DRD[1]                = (volatile uint32 *)(volatile uint32)(*(TASK_LPC_api->PtrStartTDT) + 0x0018UL - vMemOffset);
	TASK_LPC_api->DRD[2]                = (volatile uint32 *)(volatile uint32)(*(TASK_LPC_api->PtrStartTDT) + 0x0024UL - vMemOffset);
	TASK_LPC_api->DRD[3]                = (volatile uint32 *)(volatile uint32)(*(TASK_LPC_api->PtrStartTDT) + 0x002cUL - vMemOffset);
	TASK_LPC_api->NumVar                = 9;
	TASK_LPC_api->var                   = (volatile uint32 *)(volatile uint32)(*(TASK_LPC_api->PtrVarTab) - vMemOffset);
	TASK_LPC_api->NumInc                = 8;
	TASK_LPC_api->inc                   = (volatile uint32 *)(volatile uint32)(*(TASK_LPC_api->PtrVarTab) + (4*24) - vMemOffset);
	TASK_LPC_api->Bytes                 = &(TASK_LPC_api->var[0]);
	TASK_LPC_api->IncrBytes             = (volatile sint16 *)&(TASK_LPC_api->inc[0])+1;
	TASK_LPC_api->IterExtra             = &(TASK_LPC_api->var[1]);
	TASK_LPC_api->IncrDst               = (volatile sint16 *)&(TASK_LPC_api->inc[1])+1;
	TASK_LPC_api->IncrDstMA             = (volatile sint16 *)&(TASK_LPC_api->inc[2])+1;
	TASK_LPC_api->IncrSrc               = (volatile sint16 *)&(TASK_LPC_api->inc[3])+1;
	TASK_LPC_api->StartAddrDst          = &(TASK_LPC_api->var[4]);
	TASK_LPC_api->IncrSrcMA             = (volatile sint16 *)&(TASK_LPC_api->inc[4])+1;
	TASK_LPC_api->StartAddrSrc          = &(TASK_LPC_api->var[5]);
}


void init_dma_image_TASK_ATA(uint8 *taskBar, sint64 vMemOffset)
{
	uint8 *vMem_taskBar = (taskBar - vMemOffset);
	TASK_ATA_api->TaskNum               = 5;
	TASK_ATA_api->PtrStartTDT           = (volatile uint32 *)(vMem_taskBar + 0x00a0UL);
	TASK_ATA_api->PtrEndTDT             = (volatile uint32 *)(vMem_taskBar + 0x00a4UL);
	TASK_ATA_api->PtrVarTab             = (volatile uint32 *)(vMem_taskBar + 0x00a8UL);
	TASK_ATA_api->PtrFDT                = (volatile uint32 *)(vMem_taskBar + 0x00acUL);
	TASK_ATA_api->PtrCSave              = (volatile uint32 *)(vMem_taskBar + 0x00b8UL);
	TASK_ATA_api->TaskPragma            = (volatile uint8  *)(TASK_ATA_api->PtrFDT)+3;
	TASK_ATA_api->NumDRD                = 7;
	TASK_ATA_api->DRD[0]                = (volatile uint32 *)(volatile uint32)(*(TASK_ATA_api->PtrStartTDT) + 0x0004UL - vMemOffset);
	TASK_ATA_api->DRD[1]                = (volatile uint32 *)(volatile uint32)(*(TASK_ATA_api->PtrStartTDT) + 0x000cUL - vMemOffset);
	TASK_ATA_api->DRD[2]                = (volatile uint32 *)(volatile uint32)(*(TASK_ATA_api->PtrStartTDT) + 0x0010UL - vMemOffset);
	TASK_ATA_api->DRD[3]                = (volatile uint32 *)(volatile uint32)(*(TASK_ATA_api->PtrStartTDT) + 0x0014UL - vMemOffset);
	TASK_ATA_api->DRD[4]                = (volatile uint32 *)(volatile uint32)(*(TASK_ATA_api->PtrStartTDT) + 0x0024UL - vMemOffset);
	TASK_ATA_api->DRD[5]                = (volatile uint32 *)(volatile uint32)(*(TASK_ATA_api->PtrStartTDT) + 0x002cUL - vMemOffset);
	TASK_ATA_api->DRD[6]                = (volatile uint32 *)(volatile uint32)(*(TASK_ATA_api->PtrStartTDT) + 0x0030UL - vMemOffset);
	TASK_ATA_api->NumVar                = 12;
	TASK_ATA_api->var                   = (volatile uint32 *)(volatile uint32)(*(TASK_ATA_api->PtrVarTab) - vMemOffset);
	TASK_ATA_api->NumInc                = 6;
	TASK_ATA_api->inc                   = (volatile uint32 *)(volatile uint32)(*(TASK_ATA_api->PtrVarTab) + (4*24) - vMemOffset);
	TASK_ATA_api->AddrEnable            = &(TASK_ATA_api->var[0]);
	TASK_ATA_api->IncrBytes             = (volatile sint16 *)&(TASK_ATA_api->inc[0])+1;
	TASK_ATA_api->BDTableBase           = &(TASK_ATA_api->var[1]);
	TASK_ATA_api->IncrDst               = (volatile sint16 *)&(TASK_ATA_api->inc[1])+1;
	TASK_ATA_api->BDTableLast           = &(TASK_ATA_api->var[2]);
	TASK_ATA_api->IncrSrc               = (volatile sint16 *)&(TASK_ATA_api->inc[2])+1;
	TASK_ATA_api->BDTableStart          = &(TASK_ATA_api->var[3]);
	TASK_ATA_api->Bytes                 = &(TASK_ATA_api->var[4]);
}


void init_dma_image_TASK_CRC16_DP_0(uint8 *taskBar, sint64 vMemOffset)
{
	uint8 *vMem_taskBar = (taskBar - vMemOffset);
	TASK_CRC16_DP_0_api->TaskNum        = 6;
	TASK_CRC16_DP_0_api->PtrStartTDT    = (volatile uint32 *)(vMem_taskBar + 0x00c0UL);
	TASK_CRC16_DP_0_api->PtrEndTDT      = (volatile uint32 *)(vMem_taskBar + 0x00c4UL);
	TASK_CRC16_DP_0_api->PtrVarTab      = (volatile uint32 *)(vMem_taskBar + 0x00c8UL);
	TASK_CRC16_DP_0_api->PtrFDT         = (volatile uint32 *)(vMem_taskBar + 0x00ccUL);
	TASK_CRC16_DP_0_api->PtrCSave       = (volatile uint32 *)(vMem_taskBar + 0x00d8UL);
	TASK_CRC16_DP_0_api->TaskPragma     = (volatile uint8  *)(TASK_CRC16_DP_0_api->PtrFDT)+3;
	TASK_CRC16_DP_0_api->NumDRD         = 9;
	TASK_CRC16_DP_0_api->DRD[0]         = (volatile uint32 *)(volatile uint32)(*(TASK_CRC16_DP_0_api->PtrStartTDT) + 0x0004UL - vMemOffset);
	TASK_CRC16_DP_0_api->DRD[1]         = (volatile uint32 *)(volatile uint32)(*(TASK_CRC16_DP_0_api->PtrStartTDT) + 0x0008UL - vMemOffset);
	TASK_CRC16_DP_0_api->DRD[2]         = (volatile uint32 *)(volatile uint32)(*(TASK_CRC16_DP_0_api->PtrStartTDT) + 0x0014UL - vMemOffset);
	TASK_CRC16_DP_0_api->DRD[3]         = (volatile uint32 *)(volatile uint32)(*(TASK_CRC16_DP_0_api->PtrStartTDT) + 0x0018UL - vMemOffset);
	TASK_CRC16_DP_0_api->DRD[4]         = (volatile uint32 *)(volatile uint32)(*(TASK_CRC16_DP_0_api->PtrStartTDT) + 0x0024UL - vMemOffset);
	TASK_CRC16_DP_0_api->DRD[5]         = (volatile uint32 *)(volatile uint32)(*(TASK_CRC16_DP_0_api->PtrStartTDT) + 0x0028UL - vMemOffset);
	TASK_CRC16_DP_0_api->DRD[6]         = (volatile uint32 *)(volatile uint32)(*(TASK_CRC16_DP_0_api->PtrStartTDT) + 0x0034UL - vMemOffset);
	TASK_CRC16_DP_0_api->DRD[7]         = (volatile uint32 *)(volatile uint32)(*(TASK_CRC16_DP_0_api->PtrStartTDT) + 0x0038UL - vMemOffset);
	TASK_CRC16_DP_0_api->DRD[8]         = (volatile uint32 *)(volatile uint32)(*(TASK_CRC16_DP_0_api->PtrStartTDT) + 0x0040UL - vMemOffset);
	TASK_CRC16_DP_0_api->NumVar         = 10;
	TASK_CRC16_DP_0_api->var            = (volatile uint32 *)(volatile uint32)(*(TASK_CRC16_DP_0_api->PtrVarTab) - vMemOffset);
	TASK_CRC16_DP_0_api->NumInc         = 8;
	TASK_CRC16_DP_0_api->inc            = (volatile uint32 *)(volatile uint32)(*(TASK_CRC16_DP_0_api->PtrVarTab) + (4*24) - vMemOffset);
	TASK_CRC16_DP_0_api->Bytes          = &(TASK_CRC16_DP_0_api->var[0]);
	TASK_CRC16_DP_0_api->IncrBytes      = (volatile sint16 *)&(TASK_CRC16_DP_0_api->inc[0])+1;
	TASK_CRC16_DP_0_api->IterExtra      = &(TASK_CRC16_DP_0_api->var[1]);
	TASK_CRC16_DP_0_api->IncrDst        = (volatile sint16 *)&(TASK_CRC16_DP_0_api->inc[1])+1;
	TASK_CRC16_DP_0_api->IncrDstMA      = (volatile sint16 *)&(TASK_CRC16_DP_0_api->inc[2])+1;
	TASK_CRC16_DP_0_api->IncrSrc        = (volatile sint16 *)&(TASK_CRC16_DP_0_api->inc[3])+1;
	TASK_CRC16_DP_0_api->StartAddrDst   = &(TASK_CRC16_DP_0_api->var[4]);
	TASK_CRC16_DP_0_api->IncrSrcMA      = (volatile sint16 *)&(TASK_CRC16_DP_0_api->inc[4])+1;
	TASK_CRC16_DP_0_api->StartAddrSrc   = &(TASK_CRC16_DP_0_api->var[5]);
}


void init_dma_image_TASK_CRC16_DP_1(uint8 *taskBar, sint64 vMemOffset)
{
	uint8 *vMem_taskBar = (taskBar - vMemOffset);
	TASK_CRC16_DP_1_api->TaskNum        = 7;
	TASK_CRC16_DP_1_api->PtrStartTDT    = (volatile uint32 *)(vMem_taskBar + 0x00e0UL);
	TASK_CRC16_DP_1_api->PtrEndTDT      = (volatile uint32 *)(vMem_taskBar + 0x00e4UL);
	TASK_CRC16_DP_1_api->PtrVarTab      = (volatile uint32 *)(vMem_taskBar + 0x00e8UL);
	TASK_CRC16_DP_1_api->PtrFDT         = (volatile uint32 *)(vMem_taskBar + 0x00ecUL);
	TASK_CRC16_DP_1_api->PtrCSave       = (volatile uint32 *)(vMem_taskBar + 0x00f8UL);
	TASK_CRC16_DP_1_api->TaskPragma     = (volatile uint8  *)(TASK_CRC16_DP_1_api->PtrFDT)+3;
	TASK_CRC16_DP_1_api->NumDRD         = 9;
	TASK_CRC16_DP_1_api->DRD[0]         = (volatile uint32 *)(volatile uint32)(*(TASK_CRC16_DP_1_api->PtrStartTDT) + 0x0004UL - vMemOffset);
	TASK_CRC16_DP_1_api->DRD[1]         = (volatile uint32 *)(volatile uint32)(*(TASK_CRC16_DP_1_api->PtrStartTDT) + 0x0008UL - vMemOffset);
	TASK_CRC16_DP_1_api->DRD[2]         = (volatile uint32 *)(volatile uint32)(*(TASK_CRC16_DP_1_api->PtrStartTDT) + 0x0014UL - vMemOffset);
	TASK_CRC16_DP_1_api->DRD[3]         = (volatile uint32 *)(volatile uint32)(*(TASK_CRC16_DP_1_api->PtrStartTDT) + 0x0018UL - vMemOffset);
	TASK_CRC16_DP_1_api->DRD[4]         = (volatile uint32 *)(volatile uint32)(*(TASK_CRC16_DP_1_api->PtrStartTDT) + 0x0024UL - vMemOffset);
	TASK_CRC16_DP_1_api->DRD[5]         = (volatile uint32 *)(volatile uint32)(*(TASK_CRC16_DP_1_api->PtrStartTDT) + 0x0028UL - vMemOffset);
	TASK_CRC16_DP_1_api->DRD[6]         = (volatile uint32 *)(volatile uint32)(*(TASK_CRC16_DP_1_api->PtrStartTDT) + 0x0034UL - vMemOffset);
	TASK_CRC16_DP_1_api->DRD[7]         = (volatile uint32 *)(volatile uint32)(*(TASK_CRC16_DP_1_api->PtrStartTDT) + 0x0038UL - vMemOffset);
	TASK_CRC16_DP_1_api->DRD[8]         = (volatile uint32 *)(volatile uint32)(*(TASK_CRC16_DP_1_api->PtrStartTDT) + 0x0040UL - vMemOffset);
	TASK_CRC16_DP_1_api->NumVar         = 10;
	TASK_CRC16_DP_1_api->var            = (volatile uint32 *)(volatile uint32)(*(TASK_CRC16_DP_1_api->PtrVarTab) - vMemOffset);
	TASK_CRC16_DP_1_api->NumInc         = 8;
	TASK_CRC16_DP_1_api->inc            = (volatile uint32 *)(volatile uint32)(*(TASK_CRC16_DP_1_api->PtrVarTab) + (4*24) - vMemOffset);
	TASK_CRC16_DP_1_api->Bytes          = &(TASK_CRC16_DP_1_api->var[0]);
	TASK_CRC16_DP_1_api->IncrBytes      = (volatile sint16 *)&(TASK_CRC16_DP_1_api->inc[0])+1;
	TASK_CRC16_DP_1_api->IterExtra      = &(TASK_CRC16_DP_1_api->var[1]);
	TASK_CRC16_DP_1_api->IncrDst        = (volatile sint16 *)&(TASK_CRC16_DP_1_api->inc[1])+1;
	TASK_CRC16_DP_1_api->IncrDstMA      = (volatile sint16 *)&(TASK_CRC16_DP_1_api->inc[2])+1;
	TASK_CRC16_DP_1_api->IncrSrc        = (volatile sint16 *)&(TASK_CRC16_DP_1_api->inc[3])+1;
	TASK_CRC16_DP_1_api->StartAddrDst   = &(TASK_CRC16_DP_1_api->var[4]);
	TASK_CRC16_DP_1_api->IncrSrcMA      = (volatile sint16 *)&(TASK_CRC16_DP_1_api->inc[4])+1;
	TASK_CRC16_DP_1_api->StartAddrSrc   = &(TASK_CRC16_DP_1_api->var[5]);
}


void init_dma_image_TASK_GEN_DP_0(uint8 *taskBar, sint64 vMemOffset)
{
	uint8 *vMem_taskBar = (taskBar - vMemOffset);
	TASK_GEN_DP_0_api->TaskNum          = 8;
	TASK_GEN_DP_0_api->PtrStartTDT      = (volatile uint32 *)(vMem_taskBar + 0x0100UL);
	TASK_GEN_DP_0_api->PtrEndTDT        = (volatile uint32 *)(vMem_taskBar + 0x0104UL);
	TASK_GEN_DP_0_api->PtrVarTab        = (volatile uint32 *)(vMem_taskBar + 0x0108UL);
	TASK_GEN_DP_0_api->PtrFDT           = (volatile uint32 *)(vMem_taskBar + 0x010cUL);
	TASK_GEN_DP_0_api->PtrCSave         = (volatile uint32 *)(vMem_taskBar + 0x0118UL);
	TASK_GEN_DP_0_api->TaskPragma       = (volatile uint8  *)(TASK_GEN_DP_0_api->PtrFDT)+3;
	TASK_GEN_DP_0_api->NumDRD           = 4;
	TASK_GEN_DP_0_api->DRD[0]           = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_0_api->PtrStartTDT) + 0x000cUL - vMemOffset);
	TASK_GEN_DP_0_api->DRD[1]           = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_0_api->PtrStartTDT) + 0x0018UL - vMemOffset);
	TASK_GEN_DP_0_api->DRD[2]           = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_0_api->PtrStartTDT) + 0x0024UL - vMemOffset);
	TASK_GEN_DP_0_api->DRD[3]           = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_0_api->PtrStartTDT) + 0x002cUL - vMemOffset);
	TASK_GEN_DP_0_api->NumVar           = 9;
	TASK_GEN_DP_0_api->var              = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_0_api->PtrVarTab) - vMemOffset);
	TASK_GEN_DP_0_api->NumInc           = 8;
	TASK_GEN_DP_0_api->inc              = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_0_api->PtrVarTab) + (4*24) - vMemOffset);
	TASK_GEN_DP_0_api->Bytes            = &(TASK_GEN_DP_0_api->var[0]);
	TASK_GEN_DP_0_api->IncrBytes        = (volatile sint16 *)&(TASK_GEN_DP_0_api->inc[0])+1;
	TASK_GEN_DP_0_api->IterExtra        = &(TASK_GEN_DP_0_api->var[1]);
	TASK_GEN_DP_0_api->IncrDst          = (volatile sint16 *)&(TASK_GEN_DP_0_api->inc[1])+1;
	TASK_GEN_DP_0_api->IncrDstMA        = (volatile sint16 *)&(TASK_GEN_DP_0_api->inc[2])+1;
	TASK_GEN_DP_0_api->IncrSrc          = (volatile sint16 *)&(TASK_GEN_DP_0_api->inc[3])+1;
	TASK_GEN_DP_0_api->StartAddrDst     = &(TASK_GEN_DP_0_api->var[4]);
	TASK_GEN_DP_0_api->IncrSrcMA        = (volatile sint16 *)&(TASK_GEN_DP_0_api->inc[4])+1;
	TASK_GEN_DP_0_api->StartAddrSrc     = &(TASK_GEN_DP_0_api->var[5]);
}


void init_dma_image_TASK_GEN_DP_1(uint8 *taskBar, sint64 vMemOffset)
{
	uint8 *vMem_taskBar = (taskBar - vMemOffset);
	TASK_GEN_DP_1_api->TaskNum          = 9;
	TASK_GEN_DP_1_api->PtrStartTDT      = (volatile uint32 *)(vMem_taskBar + 0x0120UL);
	TASK_GEN_DP_1_api->PtrEndTDT        = (volatile uint32 *)(vMem_taskBar + 0x0124UL);
	TASK_GEN_DP_1_api->PtrVarTab        = (volatile uint32 *)(vMem_taskBar + 0x0128UL);
	TASK_GEN_DP_1_api->PtrFDT           = (volatile uint32 *)(vMem_taskBar + 0x012cUL);
	TASK_GEN_DP_1_api->PtrCSave         = (volatile uint32 *)(vMem_taskBar + 0x0138UL);
	TASK_GEN_DP_1_api->TaskPragma       = (volatile uint8  *)(TASK_GEN_DP_1_api->PtrFDT)+3;
	TASK_GEN_DP_1_api->NumDRD           = 4;
	TASK_GEN_DP_1_api->DRD[0]           = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_1_api->PtrStartTDT) + 0x000cUL - vMemOffset);
	TASK_GEN_DP_1_api->DRD[1]           = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_1_api->PtrStartTDT) + 0x0018UL - vMemOffset);
	TASK_GEN_DP_1_api->DRD[2]           = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_1_api->PtrStartTDT) + 0x0024UL - vMemOffset);
	TASK_GEN_DP_1_api->DRD[3]           = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_1_api->PtrStartTDT) + 0x002cUL - vMemOffset);
	TASK_GEN_DP_1_api->NumVar           = 9;
	TASK_GEN_DP_1_api->var              = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_1_api->PtrVarTab) - vMemOffset);
	TASK_GEN_DP_1_api->NumInc           = 8;
	TASK_GEN_DP_1_api->inc              = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_1_api->PtrVarTab) + (4*24) - vMemOffset);
	TASK_GEN_DP_1_api->Bytes            = &(TASK_GEN_DP_1_api->var[0]);
	TASK_GEN_DP_1_api->IncrBytes        = (volatile sint16 *)&(TASK_GEN_DP_1_api->inc[0])+1;
	TASK_GEN_DP_1_api->IterExtra        = &(TASK_GEN_DP_1_api->var[1]);
	TASK_GEN_DP_1_api->IncrDst          = (volatile sint16 *)&(TASK_GEN_DP_1_api->inc[1])+1;
	TASK_GEN_DP_1_api->IncrDstMA        = (volatile sint16 *)&(TASK_GEN_DP_1_api->inc[2])+1;
	TASK_GEN_DP_1_api->IncrSrc          = (volatile sint16 *)&(TASK_GEN_DP_1_api->inc[3])+1;
	TASK_GEN_DP_1_api->StartAddrDst     = &(TASK_GEN_DP_1_api->var[4]);
	TASK_GEN_DP_1_api->IncrSrcMA        = (volatile sint16 *)&(TASK_GEN_DP_1_api->inc[4])+1;
	TASK_GEN_DP_1_api->StartAddrSrc     = &(TASK_GEN_DP_1_api->var[5]);
}


void init_dma_image_TASK_GEN_DP_2(uint8 *taskBar, sint64 vMemOffset)
{
	uint8 *vMem_taskBar = (taskBar - vMemOffset);
	TASK_GEN_DP_2_api->TaskNum          = 10;
	TASK_GEN_DP_2_api->PtrStartTDT      = (volatile uint32 *)(vMem_taskBar + 0x0140UL);
	TASK_GEN_DP_2_api->PtrEndTDT        = (volatile uint32 *)(vMem_taskBar + 0x0144UL);
	TASK_GEN_DP_2_api->PtrVarTab        = (volatile uint32 *)(vMem_taskBar + 0x0148UL);
	TASK_GEN_DP_2_api->PtrFDT           = (volatile uint32 *)(vMem_taskBar + 0x014cUL);
	TASK_GEN_DP_2_api->PtrCSave         = (volatile uint32 *)(vMem_taskBar + 0x0158UL);
	TASK_GEN_DP_2_api->TaskPragma       = (volatile uint8  *)(TASK_GEN_DP_2_api->PtrFDT)+3;
	TASK_GEN_DP_2_api->NumDRD           = 4;
	TASK_GEN_DP_2_api->DRD[0]           = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_2_api->PtrStartTDT) + 0x000cUL - vMemOffset);
	TASK_GEN_DP_2_api->DRD[1]           = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_2_api->PtrStartTDT) + 0x0018UL - vMemOffset);
	TASK_GEN_DP_2_api->DRD[2]           = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_2_api->PtrStartTDT) + 0x0024UL - vMemOffset);
	TASK_GEN_DP_2_api->DRD[3]           = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_2_api->PtrStartTDT) + 0x002cUL - vMemOffset);
	TASK_GEN_DP_2_api->NumVar           = 9;
	TASK_GEN_DP_2_api->var              = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_2_api->PtrVarTab) - vMemOffset);
	TASK_GEN_DP_2_api->NumInc           = 8;
	TASK_GEN_DP_2_api->inc              = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_2_api->PtrVarTab) + (4*24) - vMemOffset);
	TASK_GEN_DP_2_api->Bytes            = &(TASK_GEN_DP_2_api->var[0]);
	TASK_GEN_DP_2_api->IncrBytes        = (volatile sint16 *)&(TASK_GEN_DP_2_api->inc[0])+1;
	TASK_GEN_DP_2_api->IterExtra        = &(TASK_GEN_DP_2_api->var[1]);
	TASK_GEN_DP_2_api->IncrDst          = (volatile sint16 *)&(TASK_GEN_DP_2_api->inc[1])+1;
	TASK_GEN_DP_2_api->IncrDstMA        = (volatile sint16 *)&(TASK_GEN_DP_2_api->inc[2])+1;
	TASK_GEN_DP_2_api->IncrSrc          = (volatile sint16 *)&(TASK_GEN_DP_2_api->inc[3])+1;
	TASK_GEN_DP_2_api->StartAddrDst     = &(TASK_GEN_DP_2_api->var[4]);
	TASK_GEN_DP_2_api->IncrSrcMA        = (volatile sint16 *)&(TASK_GEN_DP_2_api->inc[4])+1;
	TASK_GEN_DP_2_api->StartAddrSrc     = &(TASK_GEN_DP_2_api->var[5]);
}


void init_dma_image_TASK_GEN_DP_3(uint8 *taskBar, sint64 vMemOffset)
{
	uint8 *vMem_taskBar = (taskBar - vMemOffset);
	TASK_GEN_DP_3_api->TaskNum          = 11;
	TASK_GEN_DP_3_api->PtrStartTDT      = (volatile uint32 *)(vMem_taskBar + 0x0160UL);
	TASK_GEN_DP_3_api->PtrEndTDT        = (volatile uint32 *)(vMem_taskBar + 0x0164UL);
	TASK_GEN_DP_3_api->PtrVarTab        = (volatile uint32 *)(vMem_taskBar + 0x0168UL);
	TASK_GEN_DP_3_api->PtrFDT           = (volatile uint32 *)(vMem_taskBar + 0x016cUL);
	TASK_GEN_DP_3_api->PtrCSave         = (volatile uint32 *)(vMem_taskBar + 0x0178UL);
	TASK_GEN_DP_3_api->TaskPragma       = (volatile uint8  *)(TASK_GEN_DP_3_api->PtrFDT)+3;
	TASK_GEN_DP_3_api->NumDRD           = 4;
	TASK_GEN_DP_3_api->DRD[0]           = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_3_api->PtrStartTDT) + 0x000cUL - vMemOffset);
	TASK_GEN_DP_3_api->DRD[1]           = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_3_api->PtrStartTDT) + 0x0018UL - vMemOffset);
	TASK_GEN_DP_3_api->DRD[2]           = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_3_api->PtrStartTDT) + 0x0024UL - vMemOffset);
	TASK_GEN_DP_3_api->DRD[3]           = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_3_api->PtrStartTDT) + 0x002cUL - vMemOffset);
	TASK_GEN_DP_3_api->NumVar           = 9;
	TASK_GEN_DP_3_api->var              = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_3_api->PtrVarTab) - vMemOffset);
	TASK_GEN_DP_3_api->NumInc           = 8;
	TASK_GEN_DP_3_api->inc              = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_3_api->PtrVarTab) + (4*24) - vMemOffset);
	TASK_GEN_DP_3_api->Bytes            = &(TASK_GEN_DP_3_api->var[0]);
	TASK_GEN_DP_3_api->IncrBytes        = (volatile sint16 *)&(TASK_GEN_DP_3_api->inc[0])+1;
	TASK_GEN_DP_3_api->IterExtra        = &(TASK_GEN_DP_3_api->var[1]);
	TASK_GEN_DP_3_api->IncrDst          = (volatile sint16 *)&(TASK_GEN_DP_3_api->inc[1])+1;
	TASK_GEN_DP_3_api->IncrDstMA        = (volatile sint16 *)&(TASK_GEN_DP_3_api->inc[2])+1;
	TASK_GEN_DP_3_api->IncrSrc          = (volatile sint16 *)&(TASK_GEN_DP_3_api->inc[3])+1;
	TASK_GEN_DP_3_api->StartAddrDst     = &(TASK_GEN_DP_3_api->var[4]);
	TASK_GEN_DP_3_api->IncrSrcMA        = (volatile sint16 *)&(TASK_GEN_DP_3_api->inc[4])+1;
	TASK_GEN_DP_3_api->StartAddrSrc     = &(TASK_GEN_DP_3_api->var[5]);
}


void init_dma_image_TASK_GEN_TX_BD(uint8 *taskBar, sint64 vMemOffset)
{
	uint8 *vMem_taskBar = (taskBar - vMemOffset);
	TASK_GEN_TX_BD_api->TaskNum         = 12;
	TASK_GEN_TX_BD_api->PtrStartTDT     = (volatile uint32 *)(vMem_taskBar + 0x0180UL);
	TASK_GEN_TX_BD_api->PtrEndTDT       = (volatile uint32 *)(vMem_taskBar + 0x0184UL);
	TASK_GEN_TX_BD_api->PtrVarTab       = (volatile uint32 *)(vMem_taskBar + 0x0188UL);
	TASK_GEN_TX_BD_api->PtrFDT          = (volatile uint32 *)(vMem_taskBar + 0x018cUL);
	TASK_GEN_TX_BD_api->PtrCSave        = (volatile uint32 *)(vMem_taskBar + 0x0198UL);
	TASK_GEN_TX_BD_api->TaskPragma      = (volatile uint8  *)(TASK_GEN_TX_BD_api->PtrFDT)+3;
	TASK_GEN_TX_BD_api->NumDRD          = 8;
	TASK_GEN_TX_BD_api->DRD[0]          = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_TX_BD_api->PtrStartTDT) + 0x0004UL - vMemOffset);
	TASK_GEN_TX_BD_api->DRD[1]          = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_TX_BD_api->PtrStartTDT) + 0x000cUL - vMemOffset);
	TASK_GEN_TX_BD_api->DRD[2]          = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_TX_BD_api->PtrStartTDT) + 0x0010UL - vMemOffset);
	TASK_GEN_TX_BD_api->DRD[3]          = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_TX_BD_api->PtrStartTDT) + 0x0014UL - vMemOffset);
	TASK_GEN_TX_BD_api->DRD[4]          = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_TX_BD_api->PtrStartTDT) + 0x0020UL - vMemOffset);
	TASK_GEN_TX_BD_api->DRD[5]          = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_TX_BD_api->PtrStartTDT) + 0x0028UL - vMemOffset);
	TASK_GEN_TX_BD_api->DRD[6]          = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_TX_BD_api->PtrStartTDT) + 0x0030UL - vMemOffset);
	TASK_GEN_TX_BD_api->DRD[7]          = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_TX_BD_api->PtrStartTDT) + 0x0034UL - vMemOffset);
	TASK_GEN_TX_BD_api->NumVar          = 13;
	TASK_GEN_TX_BD_api->var             = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_TX_BD_api->PtrVarTab) - vMemOffset);
	TASK_GEN_TX_BD_api->NumInc          = 7;
	TASK_GEN_TX_BD_api->inc             = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_TX_BD_api->PtrVarTab) + (4*24) - vMemOffset);
	TASK_GEN_TX_BD_api->AddrDstFIFO     = &(TASK_GEN_TX_BD_api->var[0]);
	TASK_GEN_TX_BD_api->IncrBytes       = (volatile sint16 *)&(TASK_GEN_TX_BD_api->inc[0])+1;
	TASK_GEN_TX_BD_api->AddrEnable      = &(TASK_GEN_TX_BD_api->var[1]);
	TASK_GEN_TX_BD_api->IncrSrc         = (volatile sint16 *)&(TASK_GEN_TX_BD_api->inc[1])+1;
	TASK_GEN_TX_BD_api->BDTableBase     = &(TASK_GEN_TX_BD_api->var[2]);
	TASK_GEN_TX_BD_api->IncrSrcMA       = (volatile sint16 *)&(TASK_GEN_TX_BD_api->inc[2])+1;
	TASK_GEN_TX_BD_api->BDTableLast     = &(TASK_GEN_TX_BD_api->var[3]);
	TASK_GEN_TX_BD_api->BDTableStart    = &(TASK_GEN_TX_BD_api->var[4]);
	TASK_GEN_TX_BD_api->Bytes           = &(TASK_GEN_TX_BD_api->var[5]);
}


void init_dma_image_TASK_GEN_RX_BD(uint8 *taskBar, sint64 vMemOffset)
{
	uint8 *vMem_taskBar = (taskBar - vMemOffset);
	TASK_GEN_RX_BD_api->TaskNum         = 13;
	TASK_GEN_RX_BD_api->PtrStartTDT     = (volatile uint32 *)(vMem_taskBar + 0x01a0UL);
	TASK_GEN_RX_BD_api->PtrEndTDT       = (volatile uint32 *)(vMem_taskBar + 0x01a4UL);
	TASK_GEN_RX_BD_api->PtrVarTab       = (volatile uint32 *)(vMem_taskBar + 0x01a8UL);
	TASK_GEN_RX_BD_api->PtrFDT          = (volatile uint32 *)(vMem_taskBar + 0x01acUL);
	TASK_GEN_RX_BD_api->PtrCSave        = (volatile uint32 *)(vMem_taskBar + 0x01b8UL);
	TASK_GEN_RX_BD_api->TaskPragma      = (volatile uint8  *)(TASK_GEN_RX_BD_api->PtrFDT)+3;
	TASK_GEN_RX_BD_api->NumDRD          = 7;
	TASK_GEN_RX_BD_api->DRD[0]          = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_RX_BD_api->PtrStartTDT) + 0x0004UL - vMemOffset);
	TASK_GEN_RX_BD_api->DRD[1]          = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_RX_BD_api->PtrStartTDT) + 0x000cUL - vMemOffset);
	TASK_GEN_RX_BD_api->DRD[2]          = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_RX_BD_api->PtrStartTDT) + 0x0010UL - vMemOffset);
	TASK_GEN_RX_BD_api->DRD[3]          = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_RX_BD_api->PtrStartTDT) + 0x0014UL - vMemOffset);
	TASK_GEN_RX_BD_api->DRD[4]          = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_RX_BD_api->PtrStartTDT) + 0x0020UL - vMemOffset);
	TASK_GEN_RX_BD_api->DRD[5]          = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_RX_BD_api->PtrStartTDT) + 0x0028UL - vMemOffset);
	TASK_GEN_RX_BD_api->DRD[6]          = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_RX_BD_api->PtrStartTDT) + 0x002cUL - vMemOffset);
	TASK_GEN_RX_BD_api->NumVar          = 12;
	TASK_GEN_RX_BD_api->var             = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_RX_BD_api->PtrVarTab) - vMemOffset);
	TASK_GEN_RX_BD_api->NumInc          = 5;
	TASK_GEN_RX_BD_api->inc             = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_RX_BD_api->PtrVarTab) + (4*24) - vMemOffset);
	TASK_GEN_RX_BD_api->AddrEnable      = &(TASK_GEN_RX_BD_api->var[0]);
	TASK_GEN_RX_BD_api->IncrBytes       = (volatile sint16 *)&(TASK_GEN_RX_BD_api->inc[0])+1;
	TASK_GEN_RX_BD_api->AddrSrcFIFO     = &(TASK_GEN_RX_BD_api->var[1]);
	TASK_GEN_RX_BD_api->IncrDst         = (volatile sint16 *)&(TASK_GEN_RX_BD_api->inc[1])+1;
	TASK_GEN_RX_BD_api->BDTableBase     = &(TASK_GEN_RX_BD_api->var[2]);
	TASK_GEN_RX_BD_api->BDTableLast     = &(TASK_GEN_RX_BD_api->var[3]);
	TASK_GEN_RX_BD_api->BDTableStart    = &(TASK_GEN_RX_BD_api->var[4]);
	TASK_GEN_RX_BD_api->Bytes           = &(TASK_GEN_RX_BD_api->var[5]);
}


void init_dma_image_TASK_GEN_DP_BD_0(uint8 *taskBar, sint64 vMemOffset)
{
	uint8 *vMem_taskBar = (taskBar - vMemOffset);
	TASK_GEN_DP_BD_0_api->TaskNum       = 14;
	TASK_GEN_DP_BD_0_api->PtrStartTDT   = (volatile uint32 *)(vMem_taskBar + 0x01c0UL);
	TASK_GEN_DP_BD_0_api->PtrEndTDT     = (volatile uint32 *)(vMem_taskBar + 0x01c4UL);
	TASK_GEN_DP_BD_0_api->PtrVarTab     = (volatile uint32 *)(vMem_taskBar + 0x01c8UL);
	TASK_GEN_DP_BD_0_api->PtrFDT        = (volatile uint32 *)(vMem_taskBar + 0x01ccUL);
	TASK_GEN_DP_BD_0_api->PtrCSave      = (volatile uint32 *)(vMem_taskBar + 0x01d8UL);
	TASK_GEN_DP_BD_0_api->TaskPragma    = (volatile uint8  *)(TASK_GEN_DP_BD_0_api->PtrFDT)+3;
	TASK_GEN_DP_BD_0_api->NumDRD        = 7;
	TASK_GEN_DP_BD_0_api->DRD[0]        = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_BD_0_api->PtrStartTDT) + 0x0004UL - vMemOffset);
	TASK_GEN_DP_BD_0_api->DRD[1]        = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_BD_0_api->PtrStartTDT) + 0x000cUL - vMemOffset);
	TASK_GEN_DP_BD_0_api->DRD[2]        = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_BD_0_api->PtrStartTDT) + 0x0010UL - vMemOffset);
	TASK_GEN_DP_BD_0_api->DRD[3]        = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_BD_0_api->PtrStartTDT) + 0x0014UL - vMemOffset);
	TASK_GEN_DP_BD_0_api->DRD[4]        = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_BD_0_api->PtrStartTDT) + 0x0024UL - vMemOffset);
	TASK_GEN_DP_BD_0_api->DRD[5]        = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_BD_0_api->PtrStartTDT) + 0x002cUL - vMemOffset);
	TASK_GEN_DP_BD_0_api->DRD[6]        = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_BD_0_api->PtrStartTDT) + 0x0030UL - vMemOffset);
	TASK_GEN_DP_BD_0_api->NumVar        = 12;
	TASK_GEN_DP_BD_0_api->var           = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_BD_0_api->PtrVarTab) - vMemOffset);
	TASK_GEN_DP_BD_0_api->NumInc        = 6;
	TASK_GEN_DP_BD_0_api->inc           = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_BD_0_api->PtrVarTab) + (4*24) - vMemOffset);
	TASK_GEN_DP_BD_0_api->AddrEnable    = &(TASK_GEN_DP_BD_0_api->var[0]);
	TASK_GEN_DP_BD_0_api->IncrBytes     = (volatile sint16 *)&(TASK_GEN_DP_BD_0_api->inc[0])+1;
	TASK_GEN_DP_BD_0_api->BDTableBase   = &(TASK_GEN_DP_BD_0_api->var[1]);
	TASK_GEN_DP_BD_0_api->IncrDst       = (volatile sint16 *)&(TASK_GEN_DP_BD_0_api->inc[1])+1;
	TASK_GEN_DP_BD_0_api->BDTableLast   = &(TASK_GEN_DP_BD_0_api->var[2]);
	TASK_GEN_DP_BD_0_api->IncrSrc       = (volatile sint16 *)&(TASK_GEN_DP_BD_0_api->inc[2])+1;
	TASK_GEN_DP_BD_0_api->BDTableStart  = &(TASK_GEN_DP_BD_0_api->var[3]);
	TASK_GEN_DP_BD_0_api->Bytes         = &(TASK_GEN_DP_BD_0_api->var[4]);
}


void init_dma_image_TASK_GEN_DP_BD_1(uint8 *taskBar, sint64 vMemOffset)
{
	uint8 *vMem_taskBar = (taskBar - vMemOffset);
	TASK_GEN_DP_BD_1_api->TaskNum       = 15;
	TASK_GEN_DP_BD_1_api->PtrStartTDT   = (volatile uint32 *)(vMem_taskBar + 0x01e0UL);
	TASK_GEN_DP_BD_1_api->PtrEndTDT     = (volatile uint32 *)(vMem_taskBar + 0x01e4UL);
	TASK_GEN_DP_BD_1_api->PtrVarTab     = (volatile uint32 *)(vMem_taskBar + 0x01e8UL);
	TASK_GEN_DP_BD_1_api->PtrFDT        = (volatile uint32 *)(vMem_taskBar + 0x01ecUL);
	TASK_GEN_DP_BD_1_api->PtrCSave      = (volatile uint32 *)(vMem_taskBar + 0x01f8UL);
	TASK_GEN_DP_BD_1_api->TaskPragma    = (volatile uint8  *)(TASK_GEN_DP_BD_1_api->PtrFDT)+3;
	TASK_GEN_DP_BD_1_api->NumDRD        = 7;
	TASK_GEN_DP_BD_1_api->DRD[0]        = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_BD_1_api->PtrStartTDT) + 0x0004UL - vMemOffset);
	TASK_GEN_DP_BD_1_api->DRD[1]        = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_BD_1_api->PtrStartTDT) + 0x000cUL - vMemOffset);
	TASK_GEN_DP_BD_1_api->DRD[2]        = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_BD_1_api->PtrStartTDT) + 0x0010UL - vMemOffset);
	TASK_GEN_DP_BD_1_api->DRD[3]        = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_BD_1_api->PtrStartTDT) + 0x0014UL - vMemOffset);
	TASK_GEN_DP_BD_1_api->DRD[4]        = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_BD_1_api->PtrStartTDT) + 0x0024UL - vMemOffset);
	TASK_GEN_DP_BD_1_api->DRD[5]        = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_BD_1_api->PtrStartTDT) + 0x002cUL - vMemOffset);
	TASK_GEN_DP_BD_1_api->DRD[6]        = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_BD_1_api->PtrStartTDT) + 0x0030UL - vMemOffset);
	TASK_GEN_DP_BD_1_api->NumVar        = 12;
	TASK_GEN_DP_BD_1_api->var           = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_BD_1_api->PtrVarTab) - vMemOffset);
	TASK_GEN_DP_BD_1_api->NumInc        = 6;
	TASK_GEN_DP_BD_1_api->inc           = (volatile uint32 *)(volatile uint32)(*(TASK_GEN_DP_BD_1_api->PtrVarTab) + (4*24) - vMemOffset);
	TASK_GEN_DP_BD_1_api->AddrEnable    = &(TASK_GEN_DP_BD_1_api->var[0]);
	TASK_GEN_DP_BD_1_api->IncrBytes     = (volatile sint16 *)&(TASK_GEN_DP_BD_1_api->inc[0])+1;
	TASK_GEN_DP_BD_1_api->BDTableBase   = &(TASK_GEN_DP_BD_1_api->var[1]);
	TASK_GEN_DP_BD_1_api->IncrDst       = (volatile sint16 *)&(TASK_GEN_DP_BD_1_api->inc[1])+1;
	TASK_GEN_DP_BD_1_api->BDTableLast   = &(TASK_GEN_DP_BD_1_api->var[2]);
	TASK_GEN_DP_BD_1_api->IncrSrc       = (volatile sint16 *)&(TASK_GEN_DP_BD_1_api->inc[2])+1;
	TASK_GEN_DP_BD_1_api->BDTableStart  = &(TASK_GEN_DP_BD_1_api->var[3]);
	TASK_GEN_DP_BD_1_api->Bytes         = &(TASK_GEN_DP_BD_1_api->var[4]);
}

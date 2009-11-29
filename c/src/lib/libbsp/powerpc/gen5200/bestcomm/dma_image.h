#ifndef __DMA_IMAGE_H
#define __DMA_IMAGE_H 1

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


#include "include/ppctypes.h"

void init_dma_image_TASK_PCI_TX(uint8 *vMem_taskBar, sint64 vMemOffset);
void init_dma_image_TASK_PCI_RX(uint8 *vMem_taskBar, sint64 vMemOffset);
void init_dma_image_TASK_FEC_TX(uint8 *vMem_taskBar, sint64 vMemOffset);
void init_dma_image_TASK_FEC_RX(uint8 *vMem_taskBar, sint64 vMemOffset);
void init_dma_image_TASK_LPC(uint8 *vMem_taskBar, sint64 vMemOffset);
void init_dma_image_TASK_ATA(uint8 *vMem_taskBar, sint64 vMemOffset);
void init_dma_image_TASK_CRC16_DP_0(uint8 *vMem_taskBar, sint64 vMemOffset);
void init_dma_image_TASK_CRC16_DP_1(uint8 *vMem_taskBar, sint64 vMemOffset);
void init_dma_image_TASK_GEN_DP_0(uint8 *vMem_taskBar, sint64 vMemOffset);
void init_dma_image_TASK_GEN_DP_1(uint8 *vMem_taskBar, sint64 vMemOffset);
void init_dma_image_TASK_GEN_DP_2(uint8 *vMem_taskBar, sint64 vMemOffset);
void init_dma_image_TASK_GEN_DP_3(uint8 *vMem_taskBar, sint64 vMemOffset);
void init_dma_image_TASK_GEN_TX_BD(uint8 *vMem_taskBar, sint64 vMemOffset);
void init_dma_image_TASK_GEN_RX_BD(uint8 *vMem_taskBar, sint64 vMemOffset);
void init_dma_image_TASK_GEN_DP_BD_0(uint8 *vMem_taskBar, sint64 vMemOffset);
void init_dma_image_TASK_GEN_DP_BD_1(uint8 *vMem_taskBar, sint64 vMemOffset);

/* MBAR_TASK_TABLE is the first address of task table */
#ifndef MBAR_TASK_TABLE
#define MBAR_TASK_TABLE                     0xf0008000UL
#endif

/* MBAR_DMA_FREE is the first free address after task table */
#define MBAR_DMA_FREE                       MBAR_TASK_TABLE + 0x00001500UL

/* TASK_BAR is the first address of the Entry table */
#define TASK_BAR                            MBAR_TASK_TABLE + 0x00000000UL
#define TASK_BAR_OFFSET                     0x00000000UL

typedef struct task_info0 {
	volatile uint32  TaskNum;
	volatile uint32 *PtrStartTDT;
	volatile uint32 *PtrEndTDT;
	volatile uint32 *PtrVarTab;
	volatile uint32 *PtrFDT;
	volatile uint32 *PtrCSave;
	volatile uint32  NumDRD;
	volatile uint32 *DRD[7];
	volatile uint32  NumVar;
	volatile uint32 *var;
	volatile uint32  NumInc;
	volatile uint32 *inc;
	volatile uint8  *TaskPragma;
	volatile uint32 *AddrDstFIFO;
	volatile sint16 *IncrBytes;
	volatile uint32 *AddrPktSizeReg;
	volatile sint16 *IncrSrc;
	volatile uint32 *AddrSCStatusReg;
	volatile uint32 *Bytes;
	volatile uint32 *IterExtra;
	volatile uint32 *StartAddrSrc;
} TASK_PCI_TX_api_t;
extern TASK_PCI_TX_api_t *TASK_PCI_TX_api;

typedef struct task_info1 {
	volatile uint32  TaskNum;
	volatile uint32 *PtrStartTDT;
	volatile uint32 *PtrEndTDT;
	volatile uint32 *PtrVarTab;
	volatile uint32 *PtrFDT;
	volatile uint32 *PtrCSave;
	volatile uint32  NumDRD;
	volatile uint32 *DRD[5];
	volatile uint32  NumVar;
	volatile uint32 *var;
	volatile uint32  NumInc;
	volatile uint32 *inc;
	volatile uint8  *TaskPragma;
	volatile uint32 *AddrPktSizeReg;
	volatile sint16 *IncrBytes;
	volatile uint32 *AddrSrcFIFO;
	volatile sint16 *IncrDst;
	volatile uint32 *Bytes;
	volatile uint32 *IterExtra;
	volatile uint32 *StartAddrDst;
} TASK_PCI_RX_api_t;
extern TASK_PCI_RX_api_t *TASK_PCI_RX_api;

typedef struct task_info2 {
	volatile uint32  TaskNum;
	volatile uint32 *PtrStartTDT;
	volatile uint32 *PtrEndTDT;
	volatile uint32 *PtrVarTab;
	volatile uint32 *PtrFDT;
	volatile uint32 *PtrCSave;
	volatile uint32  NumDRD;
	volatile uint32 *DRD[22];
	volatile uint32  NumVar;
	volatile uint32 *var;
	volatile uint32  NumInc;
	volatile uint32 *inc;
	volatile uint8  *TaskPragma;
	volatile uint32 *AddrDRD;
	volatile uint32  AddrDRDIdx;
	volatile sint16 *IncrBytes;
	volatile uint32 *AddrDstFIFO;
	volatile sint16 *IncrSrc;
	volatile uint32 *AddrEnable;
	volatile sint16 *IncrSrcMA;
	volatile uint32 *BDTableBase;
	volatile uint32 *BDTableLast;
	volatile uint32 *BDTableStart;
	volatile uint32 *Bytes;
} TASK_FEC_TX_api_t;
extern TASK_FEC_TX_api_t *TASK_FEC_TX_api;

typedef struct task_info3 {
	volatile uint32  TaskNum;
	volatile uint32 *PtrStartTDT;
	volatile uint32 *PtrEndTDT;
	volatile uint32 *PtrVarTab;
	volatile uint32 *PtrFDT;
	volatile uint32 *PtrCSave;
	volatile uint32  NumDRD;
	volatile uint32 *DRD[13];
	volatile uint32  NumVar;
	volatile uint32 *var;
	volatile uint32  NumInc;
	volatile uint32 *inc;
	volatile uint8  *TaskPragma;
	volatile uint32 *AddrEnable;
	volatile sint16 *IncrBytes;
	volatile uint32 *AddrSrcFIFO;
	volatile sint16 *IncrDst;
	volatile uint32 *BDTableBase;
	volatile sint16 *IncrDstMA;
	volatile uint32 *BDTableLast;
	volatile uint32 *BDTableStart;
	volatile uint32 *Bytes;
} TASK_FEC_RX_api_t;
extern TASK_FEC_RX_api_t *TASK_FEC_RX_api;

typedef struct task_info4 {
	volatile uint32  TaskNum;
	volatile uint32 *PtrStartTDT;
	volatile uint32 *PtrEndTDT;
	volatile uint32 *PtrVarTab;
	volatile uint32 *PtrFDT;
	volatile uint32 *PtrCSave;
	volatile uint32  NumDRD;
	volatile uint32 *DRD[4];
	volatile uint32  NumVar;
	volatile uint32 *var;
	volatile uint32  NumInc;
	volatile uint32 *inc;
	volatile uint8  *TaskPragma;
	volatile uint32 *Bytes;
	volatile sint16 *IncrBytes;
	volatile uint32 *IterExtra;
	volatile sint16 *IncrDst;
	volatile sint16 *IncrDstMA;
	volatile sint16 *IncrSrc;
	volatile uint32 *StartAddrDst;
	volatile sint16 *IncrSrcMA;
	volatile uint32 *StartAddrSrc;
} TASK_LPC_api_t;
extern TASK_LPC_api_t *TASK_LPC_api;

typedef struct task_info5 {
	volatile uint32  TaskNum;
	volatile uint32 *PtrStartTDT;
	volatile uint32 *PtrEndTDT;
	volatile uint32 *PtrVarTab;
	volatile uint32 *PtrFDT;
	volatile uint32 *PtrCSave;
	volatile uint32  NumDRD;
	volatile uint32 *DRD[7];
	volatile uint32  NumVar;
	volatile uint32 *var;
	volatile uint32  NumInc;
	volatile uint32 *inc;
	volatile uint8  *TaskPragma;
	volatile uint32 *AddrEnable;
	volatile sint16 *IncrBytes;
	volatile uint32 *BDTableBase;
	volatile sint16 *IncrDst;
	volatile uint32 *BDTableLast;
	volatile sint16 *IncrSrc;
	volatile uint32 *BDTableStart;
	volatile uint32 *Bytes;
} TASK_ATA_api_t;
extern TASK_ATA_api_t *TASK_ATA_api;

typedef struct task_info6 {
	volatile uint32  TaskNum;
	volatile uint32 *PtrStartTDT;
	volatile uint32 *PtrEndTDT;
	volatile uint32 *PtrVarTab;
	volatile uint32 *PtrFDT;
	volatile uint32 *PtrCSave;
	volatile uint32  NumDRD;
	volatile uint32 *DRD[9];
	volatile uint32  NumVar;
	volatile uint32 *var;
	volatile uint32  NumInc;
	volatile uint32 *inc;
	volatile uint8  *TaskPragma;
	volatile uint32 *Bytes;
	volatile sint16 *IncrBytes;
	volatile uint32 *IterExtra;
	volatile sint16 *IncrDst;
	volatile sint16 *IncrDstMA;
	volatile sint16 *IncrSrc;
	volatile uint32 *StartAddrDst;
	volatile sint16 *IncrSrcMA;
	volatile uint32 *StartAddrSrc;
} TASK_CRC16_DP_0_api_t;
extern TASK_CRC16_DP_0_api_t *TASK_CRC16_DP_0_api;

typedef struct task_info7 {
	volatile uint32  TaskNum;
	volatile uint32 *PtrStartTDT;
	volatile uint32 *PtrEndTDT;
	volatile uint32 *PtrVarTab;
	volatile uint32 *PtrFDT;
	volatile uint32 *PtrCSave;
	volatile uint32  NumDRD;
	volatile uint32 *DRD[9];
	volatile uint32  NumVar;
	volatile uint32 *var;
	volatile uint32  NumInc;
	volatile uint32 *inc;
	volatile uint8  *TaskPragma;
	volatile uint32 *Bytes;
	volatile sint16 *IncrBytes;
	volatile uint32 *IterExtra;
	volatile sint16 *IncrDst;
	volatile sint16 *IncrDstMA;
	volatile sint16 *IncrSrc;
	volatile uint32 *StartAddrDst;
	volatile sint16 *IncrSrcMA;
	volatile uint32 *StartAddrSrc;
} TASK_CRC16_DP_1_api_t;
extern TASK_CRC16_DP_1_api_t *TASK_CRC16_DP_1_api;

typedef struct task_info8 {
	volatile uint32  TaskNum;
	volatile uint32 *PtrStartTDT;
	volatile uint32 *PtrEndTDT;
	volatile uint32 *PtrVarTab;
	volatile uint32 *PtrFDT;
	volatile uint32 *PtrCSave;
	volatile uint32  NumDRD;
	volatile uint32 *DRD[4];
	volatile uint32  NumVar;
	volatile uint32 *var;
	volatile uint32  NumInc;
	volatile uint32 *inc;
	volatile uint8  *TaskPragma;
	volatile uint32 *Bytes;
	volatile sint16 *IncrBytes;
	volatile uint32 *IterExtra;
	volatile sint16 *IncrDst;
	volatile sint16 *IncrDstMA;
	volatile sint16 *IncrSrc;
	volatile uint32 *StartAddrDst;
	volatile sint16 *IncrSrcMA;
	volatile uint32 *StartAddrSrc;
} TASK_GEN_DP_0_api_t;
extern TASK_GEN_DP_0_api_t *TASK_GEN_DP_0_api;

typedef struct task_info9 {
	volatile uint32  TaskNum;
	volatile uint32 *PtrStartTDT;
	volatile uint32 *PtrEndTDT;
	volatile uint32 *PtrVarTab;
	volatile uint32 *PtrFDT;
	volatile uint32 *PtrCSave;
	volatile uint32  NumDRD;
	volatile uint32 *DRD[4];
	volatile uint32  NumVar;
	volatile uint32 *var;
	volatile uint32  NumInc;
	volatile uint32 *inc;
	volatile uint8  *TaskPragma;
	volatile uint32 *Bytes;
	volatile sint16 *IncrBytes;
	volatile uint32 *IterExtra;
	volatile sint16 *IncrDst;
	volatile sint16 *IncrDstMA;
	volatile sint16 *IncrSrc;
	volatile uint32 *StartAddrDst;
	volatile sint16 *IncrSrcMA;
	volatile uint32 *StartAddrSrc;
} TASK_GEN_DP_1_api_t;
extern TASK_GEN_DP_1_api_t *TASK_GEN_DP_1_api;

typedef struct task_info10 {
	volatile uint32  TaskNum;
	volatile uint32 *PtrStartTDT;
	volatile uint32 *PtrEndTDT;
	volatile uint32 *PtrVarTab;
	volatile uint32 *PtrFDT;
	volatile uint32 *PtrCSave;
	volatile uint32  NumDRD;
	volatile uint32 *DRD[4];
	volatile uint32  NumVar;
	volatile uint32 *var;
	volatile uint32  NumInc;
	volatile uint32 *inc;
	volatile uint8  *TaskPragma;
	volatile uint32 *Bytes;
	volatile sint16 *IncrBytes;
	volatile uint32 *IterExtra;
	volatile sint16 *IncrDst;
	volatile sint16 *IncrDstMA;
	volatile sint16 *IncrSrc;
	volatile uint32 *StartAddrDst;
	volatile sint16 *IncrSrcMA;
	volatile uint32 *StartAddrSrc;
} TASK_GEN_DP_2_api_t;
extern TASK_GEN_DP_2_api_t *TASK_GEN_DP_2_api;

typedef struct task_info11 {
	volatile uint32  TaskNum;
	volatile uint32 *PtrStartTDT;
	volatile uint32 *PtrEndTDT;
	volatile uint32 *PtrVarTab;
	volatile uint32 *PtrFDT;
	volatile uint32 *PtrCSave;
	volatile uint32  NumDRD;
	volatile uint32 *DRD[4];
	volatile uint32  NumVar;
	volatile uint32 *var;
	volatile uint32  NumInc;
	volatile uint32 *inc;
	volatile uint8  *TaskPragma;
	volatile uint32 *Bytes;
	volatile sint16 *IncrBytes;
	volatile uint32 *IterExtra;
	volatile sint16 *IncrDst;
	volatile sint16 *IncrDstMA;
	volatile sint16 *IncrSrc;
	volatile uint32 *StartAddrDst;
	volatile sint16 *IncrSrcMA;
	volatile uint32 *StartAddrSrc;
} TASK_GEN_DP_3_api_t;
extern TASK_GEN_DP_3_api_t *TASK_GEN_DP_3_api;

typedef struct task_info12 {
	volatile uint32  TaskNum;
	volatile uint32 *PtrStartTDT;
	volatile uint32 *PtrEndTDT;
	volatile uint32 *PtrVarTab;
	volatile uint32 *PtrFDT;
	volatile uint32 *PtrCSave;
	volatile uint32  NumDRD;
	volatile uint32 *DRD[8];
	volatile uint32  NumVar;
	volatile uint32 *var;
	volatile uint32  NumInc;
	volatile uint32 *inc;
	volatile uint8  *TaskPragma;
	volatile uint32 *AddrDstFIFO;
	volatile sint16 *IncrBytes;
	volatile uint32 *AddrEnable;
	volatile sint16 *IncrSrc;
	volatile uint32 *BDTableBase;
	volatile sint16 *IncrSrcMA;
	volatile uint32 *BDTableLast;
	volatile uint32 *BDTableStart;
	volatile uint32 *Bytes;
} TASK_GEN_TX_BD_api_t;
extern TASK_GEN_TX_BD_api_t *TASK_GEN_TX_BD_api;

typedef struct task_info13 {
	volatile uint32  TaskNum;
	volatile uint32 *PtrStartTDT;
	volatile uint32 *PtrEndTDT;
	volatile uint32 *PtrVarTab;
	volatile uint32 *PtrFDT;
	volatile uint32 *PtrCSave;
	volatile uint32  NumDRD;
	volatile uint32 *DRD[7];
	volatile uint32  NumVar;
	volatile uint32 *var;
	volatile uint32  NumInc;
	volatile uint32 *inc;
	volatile uint8  *TaskPragma;
	volatile uint32 *AddrEnable;
	volatile sint16 *IncrBytes;
	volatile uint32 *AddrSrcFIFO;
	volatile sint16 *IncrDst;
	volatile uint32 *BDTableBase;
	volatile uint32 *BDTableLast;
	volatile uint32 *BDTableStart;
	volatile uint32 *Bytes;
} TASK_GEN_RX_BD_api_t;
extern TASK_GEN_RX_BD_api_t *TASK_GEN_RX_BD_api;

typedef struct task_info14 {
	volatile uint32  TaskNum;
	volatile uint32 *PtrStartTDT;
	volatile uint32 *PtrEndTDT;
	volatile uint32 *PtrVarTab;
	volatile uint32 *PtrFDT;
	volatile uint32 *PtrCSave;
	volatile uint32  NumDRD;
	volatile uint32 *DRD[7];
	volatile uint32  NumVar;
	volatile uint32 *var;
	volatile uint32  NumInc;
	volatile uint32 *inc;
	volatile uint8  *TaskPragma;
	volatile uint32 *AddrEnable;
	volatile sint16 *IncrBytes;
	volatile uint32 *BDTableBase;
	volatile sint16 *IncrDst;
	volatile uint32 *BDTableLast;
	volatile sint16 *IncrSrc;
	volatile uint32 *BDTableStart;
	volatile uint32 *Bytes;
} TASK_GEN_DP_BD_0_api_t;
extern TASK_GEN_DP_BD_0_api_t *TASK_GEN_DP_BD_0_api;

typedef struct task_info15 {
	volatile uint32  TaskNum;
	volatile uint32 *PtrStartTDT;
	volatile uint32 *PtrEndTDT;
	volatile uint32 *PtrVarTab;
	volatile uint32 *PtrFDT;
	volatile uint32 *PtrCSave;
	volatile uint32  NumDRD;
	volatile uint32 *DRD[7];
	volatile uint32  NumVar;
	volatile uint32 *var;
	volatile uint32  NumInc;
	volatile uint32 *inc;
	volatile uint8  *TaskPragma;
	volatile uint32 *AddrEnable;
	volatile sint16 *IncrBytes;
	volatile uint32 *BDTableBase;
	volatile sint16 *IncrDst;
	volatile uint32 *BDTableLast;
	volatile sint16 *IncrSrc;
	volatile uint32 *BDTableStart;
	volatile uint32 *Bytes;
} TASK_GEN_DP_BD_1_api_t;
extern TASK_GEN_DP_BD_1_api_t *TASK_GEN_DP_BD_1_api;


#endif	/* __DMA_IMAGE_H */

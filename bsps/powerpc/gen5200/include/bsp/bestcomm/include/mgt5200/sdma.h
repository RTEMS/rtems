#ifndef __MGT5200_SDMA_H
#define __MGT5200_SDMA_H

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

typedef struct sdma_register_set {
    volatile uint32 taskBar;         /* MBAR_SDMA + 0x00 sdTpb */
    volatile uint32 currentPointer;  /* MBAR_SDMA + 0x04 sdMdeComplex */
    volatile uint32 endPointer;      /* MBAR_SDMA + 0x08 sdMdeComplex */
    volatile uint32 variablePointer; /* MBAR_SDMA + 0x0c sdMdeComplex */

    volatile uint8  IntVect1;   /* MBAR_SDMA + 0x10 sdPtd */
    volatile uint8  IntVect2;   /* MBAR_SDMA + 0x11 sdPtd */
    volatile uint16 PtdCntrl;   /* MBAR_SDMA + 0x12 sdPtd */

    volatile uint32 IntPend;    /* MBAR_SDMA + 0x14 sdPtd */
    volatile uint32 IntMask;    /* MBAR_SDMA + 0x18 sdPtd */

    volatile uint32 TCR01;      /* MBAR_SDMA + 0x1c sdPtd */
    volatile uint32 TCR23;      /* MBAR_SDMA + 0x20 sdPtd */
    volatile uint32 TCR45;      /* MBAR_SDMA + 0x24 sdPtd */
    volatile uint32 TCR67;      /* MBAR_SDMA + 0x28 sdPtd */
    volatile uint32 TCR89;      /* MBAR_SDMA + 0x2c sdPtd */
    volatile uint32 TCRAB;      /* MBAR_SDMA + 0x30 sdPtd */
    volatile uint32 TCRCD;      /* MBAR_SDMA + 0x34 sdPtd */
    volatile uint32 TCREF;      /* MBAR_SDMA + 0x38 sdPtd */

    volatile uint8  IPR0;       /* MBAR_SDMA + 0x3c sdPtd */
    volatile uint8  IPR1;       /* MBAR_SDMA + 0x3d sdPtd */
    volatile uint8  IPR2;       /* MBAR_SDMA + 0x3e sdPtd */
    volatile uint8  IPR3;       /* MBAR_SDMA + 0x3f sdPtd */
    volatile uint8  IPR4;       /* MBAR_SDMA + 0x40 sdPtd */
    volatile uint8  IPR5;       /* MBAR_SDMA + 0x41 sdPtd */
    volatile uint8  IPR6;       /* MBAR_SDMA + 0x42 sdPtd */
    volatile uint8  IPR7;       /* MBAR_SDMA + 0x43 sdPtd */
    volatile uint8  IPR8;       /* MBAR_SDMA + 0x44 sdPtd */
    volatile uint8  IPR9;       /* MBAR_SDMA + 0x45 sdPtd */
    volatile uint8  IPR10;      /* MBAR_SDMA + 0x46 sdPtd */
    volatile uint8  IPR11;      /* MBAR_SDMA + 0x47 sdPtd */
    volatile uint8  IPR12;      /* MBAR_SDMA + 0x48 sdPtd */
    volatile uint8  IPR13;      /* MBAR_SDMA + 0x49 sdPtd */
    volatile uint8  IPR14;      /* MBAR_SDMA + 0x4a sdPtd */
    volatile uint8  IPR15;      /* MBAR_SDMA + 0x4b sdPtd */
    volatile uint8  IPR16;      /* MBAR_SDMA + 0x4c sdPtd */
    volatile uint8  IPR17;      /* MBAR_SDMA + 0x4d sdPtd */
    volatile uint8  IPR18;      /* MBAR_SDMA + 0x4e sdPtd */
    volatile uint8  IPR19;      /* MBAR_SDMA + 0x4f sdPtd */
    volatile uint8  IPR20;      /* MBAR_SDMA + 0x50 sdPtd */
    volatile uint8  IPR21;      /* MBAR_SDMA + 0x51 sdPtd */
    volatile uint8  IPR22;      /* MBAR_SDMA + 0x52 sdPtd */
    volatile uint8  IPR23;      /* MBAR_SDMA + 0x53 sdPtd */
    volatile uint8  IPR24;      /* MBAR_SDMA + 0x54 sdPtd */
    volatile uint8  IPR25;      /* MBAR_SDMA + 0x55 sdPtd */
    volatile uint8  IPR26;      /* MBAR_SDMA + 0x56 sdPtd */
    volatile uint8  IPR27;      /* MBAR_SDMA + 0x57 sdPtd */
    volatile uint8  IPR28;      /* MBAR_SDMA + 0x58 sdPtd */
    volatile uint8  IPR29;      /* MBAR_SDMA + 0x59 sdPtd */
    volatile uint8  IPR30;      /* MBAR_SDMA + 0x5a sdPtd */
    volatile uint8  IPR31;      /* MBAR_SDMA + 0x5b sdPtd */

    volatile uint32 cReqSelect; /* MBAR_SDMA + 0x5c sdPtd */
    volatile uint32 taskSize0;  /* MBAR_SDMA + 0x60 sdPtd */
    volatile uint32 taskSize1;  /* MBAR_SDMA + 0x64 sdPtd */
    volatile uint32 MDEDebug;   /* MBAR_SDMA + 0x68 sdMdeComplex */
    volatile uint32 ADSDebug;   /* MBAR_SDMA + 0x6c sdAdsTop */
    volatile uint32 Value1;     /* MBAR_SDMA + 0x70 sdDbg */
    volatile uint32 Value2;     /* MBAR_SDMA + 0x74 sdDbg */
    volatile uint32 Control;    /* MBAR_SDMA + 0x78 sdDbg */
    volatile uint32 Status;     /* MBAR_SDMA + 0x7c sdDbg */
    volatile uint32 PTDDebug;   /* MBAR_SDMA + 0x80 sdPtd */
} sdma_regs;

#define SDMA_PTDCNTRL_TI	0x8000
#define SDMA_PTDCNTRL_TEA	0x4000
#define SDMA_PTDCNTRL_HE	0x2000
#define SDMA_PTDCNTRL_PE	0x0001

#define SDMA_CREQSELECT_REQ31_MASK		(~0xC0000000UL)
#define SDMA_CREQSELECT_REQ30_MASK		(~0x30000000UL)
#define SDMA_CREQSELECT_REQ29_MASK		(~0x0C000000UL)
#define SDMA_CREQSELECT_REQ28_MASK		(~0x03000000UL)
#define SDMA_CREQSELECT_REQ27_MASK		(~0x00C00000UL)
#define SDMA_CREQSELECT_REQ26_MASK		(~0x00300000UL)
#define SDMA_CREQSELECT_REQ25_MASK		(~0x000C0000UL)
#define SDMA_CREQSELECT_REQ24_MASK		(~0x00030000UL)
#define SDMA_CREQSELECT_REQ23_MASK		(~0x0000C000UL)
#define SDMA_CREQSELECT_REQ22_MASK		(~0x00003000UL)
#define SDMA_CREQSELECT_REQ21_MASK		(~0x00000C00UL)
#define SDMA_CREQSELECT_REQ20_MASK		(~0x00000300UL)
#define SDMA_CREQSELECT_REQ19_MASK		(~0x000000C0UL)
#define SDMA_CREQSELECT_REQ18_MASK		(~0x00000030UL)
#define SDMA_CREQSELECT_REQ17_MASK		(~0x0000000CUL)
#define SDMA_CREQSELECT_REQ16_MASK		(~0x00000003UL)

#define SDMA_CREQSELECT_REQ31_ALWAYS31	0xC0000000UL
#define SDMA_CREQSELECT_REQ30_ALWAYS30	0x30000000UL
#define SDMA_CREQSELECT_REQ29_ALWAYS29	0x0C000000UL
#define SDMA_CREQSELECT_REQ28_ALWAYS28	0x03000000UL
#define SDMA_CREQSELECT_REQ27_ALWAYS27	0x00C00000UL
#define SDMA_CREQSELECT_REQ26_ALWAYS26	0x00300000UL
#define SDMA_CREQSELECT_REQ25_ALWAYS25	0x000C0000UL
#define SDMA_CREQSELECT_REQ24_ALWAYS24	0x00030000UL
#define SDMA_CREQSELECT_REQ23_ALWAYS23	0x0000C000UL
#define SDMA_CREQSELECT_REQ22_ALWAYS22	0x00003000UL
#define SDMA_CREQSELECT_REQ21_ALWAYS21	0x00000C00UL
#define SDMA_CREQSELECT_REQ20_ALWAYS20	0x00000300UL
#define SDMA_CREQSELECT_REQ19_ALWAYS19	0x000000C0UL
#define SDMA_CREQSELECT_REQ18_ALWAYS18	0x00000030UL
#define SDMA_CREQSELECT_REQ17_ALWAYS17	0x0000000CUL
#define SDMA_CREQSELECT_REQ16_ALWAYS16	0x00000003UL

#define SDMA_CREQSELECT_REQ31_SCTIMER7	0x00000000UL
#define SDMA_CREQSELECT_REQ30_SCTIMER6	0x00000000UL
#define SDMA_CREQSELECT_REQ29_SCTIMER5	0x00000000UL
#define SDMA_CREQSELECT_REQ28_SCTIMER4	0x00000000UL
#define SDMA_CREQSELECT_REQ27_SCTIMER3	0x00000000UL
#define SDMA_CREQSELECT_REQ26_PSC6_TX	0x00000000UL
#define SDMA_CREQSELECT_REQ25_PSC6_RX	0x00000000UL
#define SDMA_CREQSELECT_REQ24_I2C1_TX	0x00000000UL
#define SDMA_CREQSELECT_REQ23_I2C1_RX	0x00000000UL
#define SDMA_CREQSELECT_REQ22_I2C2_TX	0x00000000UL
#define SDMA_CREQSELECT_REQ21_I2C2_RX	0x00000000UL
#define SDMA_CREQSELECT_REQ20_PSC4_TX	0x00000000UL
#define SDMA_CREQSELECT_REQ19_PSC4_RX	0x00000000UL
#define SDMA_CREQSELECT_REQ18_PSC5_TX	0x00000000UL
#define SDMA_CREQSELECT_REQ17_PSC5_RX	0x00000000UL
#define SDMA_CREQSELECT_REQ16_LP		0x00000000UL

#define SDMA_CREQSELECT_ALWAYS30	0xC0000000UL

#endif /* __MGT5200_SDMA_H */

/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This header file provides VIM interfaces.
 */

/* The header file is generated by make_header.py from VIM.json */
/* Current script's version can be found at: */
/* https://github.com/AoLaD/rtems-tms570-utils/tree/headers/headers/python */

/*
 * Copyright (c) 2014-2015, Premysl Houdek <kom541000@gmail.com>
 *
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
*/
#ifndef LIBBSP_ARM_TMS570_VIM
#define LIBBSP_ARM_TMS570_VIM

#include <bsp/utility.h>

typedef struct{
  uint32_t PARFLG;            /*Interrupt Vector Table Parity Flag Register*/
  uint32_t PARCTL;            /*Interrupt Vector Table Parity Control Register*/
  uint32_t ADDERR;            /*Address Parity Error Register*/
  uint32_t FBPARERR;          /*Fall-Back Address Parity Error Register*/
  uint8_t reserved1 [4];
  uint32_t IRQINDEX;          /*IRQ Index Offset Vector Register*/
  uint32_t FIQINDEX;          /*FIQ Index Offset Vector Register*/
  uint8_t reserved2 [8];
  uint32_t FIRQPR[3];         /*FIQ/IRQ Program Control Register*/
  uint8_t reserved3 [4];
  uint32_t INTREQ[3];         /*Pending Interrupt Read Location Register*/
  uint8_t reserved4 [4];
  uint32_t REQENASET[3];      /*Interrupt Enable Set Register */
  uint8_t reserved5 [4];
  uint32_t REQENACLR[3];      /*Interrupt Enable Clear Register */
  uint8_t reserved6 [4];
  uint32_t WAKEENASET[3];     /*Wake-Up Enable Set Register*/
  uint8_t reserved7 [4];
  uint32_t WAKEENACLR[3];     /*Wake-Up Enable Clear Registers*/
  uint8_t reserved8 [4];
  uint32_t IRQVECREG;         /*IRQ Interrupt Vector Register*/
  uint32_t FIQVECREG;         /*FIQ Interrupt Vector Register*/
  uint32_t CAPEVT;            /*Capture Event Register*/
  uint8_t reserved9 [4];
  uint32_t CHANCTRL[24];      /*VIM Interrupt Control Register*/
} tms570_vim_t;


/*---------------------TMS570_VIM_PARFLG---------------------*/
/* field: PARFLG - The PARFLG indicates that a parity error has been found and that theInterrupt Vector Table is */
#define TMS570_VIM_PARFLG_PARFLG BSP_BIT32(0)


/*---------------------TMS570_VIM_PARCTL---------------------*/
/* field: TEST - This bit maps the parity bits into the Interrupt Vector Table frame to make them accessible by the */
#define TMS570_VIM_PARCTL_TEST BSP_BIT32(8)

/* field: PARENA - VIM parity enable. */
#define TMS570_VIM_PARCTL_PARENA(val) BSP_FLD32(val,0, 3)
#define TMS570_VIM_PARCTL_PARENA_GET(reg) BSP_FLD32GET(reg,0, 3)
#define TMS570_VIM_PARCTL_PARENA_SET(reg,val) BSP_FLD32SET(reg, val,0, 3)


/*---------------------TMS570_VIM_ADDERR---------------------*/
/* field: Interrupt_Vector_Table - Interrupt Vector Table offset. */
#define TMS570_VIM_ADDERR_Interrupt_Vector_Table(val) BSP_FLD32(val,9, 31)
#define TMS570_VIM_ADDERR_Interrupt_Vector_Table_GET(reg) BSP_FLD32GET(reg,9, 31)
#define TMS570_VIM_ADDERR_Interrupt_Vector_Table_SET(reg,val) BSP_FLD32SET(reg, val,9, 31)

/* field: ADDERR - Address parity error register. */
#define TMS570_VIM_ADDERR_ADDERR(val) BSP_FLD32(val,2, 8)
#define TMS570_VIM_ADDERR_ADDERR_GET(reg) BSP_FLD32GET(reg,2, 8)
#define TMS570_VIM_ADDERR_ADDERR_SET(reg,val) BSP_FLD32SET(reg, val,2, 8)

/* field: Word_offset - Word offset. Reads are always 0; writes have no effect. */
#define TMS570_VIM_ADDERR_Word_offset(val) BSP_FLD32(val,0, 1)
#define TMS570_VIM_ADDERR_Word_offset_GET(reg) BSP_FLD32GET(reg,0, 1)
#define TMS570_VIM_ADDERR_Word_offset_SET(reg,val) BSP_FLD32SET(reg, val,0, 1)


/*--------------------TMS570_VIM_FBPARERR--------------------*/
/* field: FBPARERR - Fall back address parity error. */
/* Whole 32 bits */

/*--------------------TMS570_VIM_IRQINDEX--------------------*/
/* field: IRQINDEX - IRQ index vector. */
#define TMS570_VIM_IRQINDEX_IRQINDEX(val) BSP_FLD32(val,0, 7)
#define TMS570_VIM_IRQINDEX_IRQINDEX_GET(reg) BSP_FLD32GET(reg,0, 7)
#define TMS570_VIM_IRQINDEX_IRQINDEX_SET(reg,val) BSP_FLD32SET(reg, val,0, 7)


/*--------------------TMS570_VIM_FIQINDEX--------------------*/
/* field: FIQINDEX - FIQ index offset vector. */
#define TMS570_VIM_FIQINDEX_FIQINDEX(val) BSP_FLD32(val,0, 7)
#define TMS570_VIM_FIQINDEX_FIQINDEX_GET(reg) BSP_FLD32GET(reg,0, 7)
#define TMS570_VIM_FIQINDEX_FIQINDEX_SET(reg,val) BSP_FLD32SET(reg, val,0, 7)


/*---------------------TMS570_VIM_FIRQPR---------------------*/
/* field: FIRQPRx - FIQ/IRQ program control bits. 96 bit register. 0-1 bits reserved. */
/* Whole 32 bits */

/*---------------------TMS570_VIM_INTREQ---------------------*/
/* field: INTREQx - Pending interrupt bits. 96 bit register. */
/* Whole 32 bits */

/*--------------------TMS570_VIM_REQENASET--------------------*/
/* field: REQENASETx - Request enable set bits. 96 bit register. 0-1 bits reserved. */
/* Whole 32 bits */

/*--------------------TMS570_VIM_REQENACLR--------------------*/
/* field: REQENACLRx - Request enable clear bits. 96 bit register. 0-1 bits reserved. */
/* Whole 32 bits */

/*-------------------TMS570_VIM_WAKEENASET-------------------*/
/* field: WAKEENASETx - Wake-up enable set bits. This vector determines whether the wake-up interrupt line is enabled. */
/* Whole 32 bits */

/*-------------------TMS570_VIM_WAKEENACLR-------------------*/
/* field: WAKEENACLRx - Wake-up enable clear bits. This vector determines whether the wake-up interrupt line is enabled. */
/* Whole 32 bits */

/*--------------------TMS570_VIM_IRQVECREG--------------------*/
/* field: IRQVECREG - IRQ interrupt vector register. */
/* Whole 32 bits */

/*--------------------TMS570_VIM_FIQVECREG--------------------*/
/* field: FIQVECREG - FIQ interrupt vector register. */
/* Whole 32 bits */

/*---------------------TMS570_VIM_CAPEVT---------------------*/
/* field: CAPEVTSRC1 - Capture event source 1 mapping control. */
#define TMS570_VIM_CAPEVT_CAPEVTSRC1(val) BSP_FLD32(val,16, 22)
#define TMS570_VIM_CAPEVT_CAPEVTSRC1_GET(reg) BSP_FLD32GET(reg,16, 22)
#define TMS570_VIM_CAPEVT_CAPEVTSRC1_SET(reg,val) BSP_FLD32SET(reg, val,16, 22)

/* field: CAPEVTSRC0 - the capture event source 0 of the RTI: */
#define TMS570_VIM_CAPEVT_CAPEVTSRC0(val) BSP_FLD32(val,0, 6)
#define TMS570_VIM_CAPEVT_CAPEVTSRC0_GET(reg) BSP_FLD32GET(reg,0, 6)
#define TMS570_VIM_CAPEVT_CAPEVTSRC0_SET(reg,val) BSP_FLD32SET(reg, val,0, 6)


/*--------------------TMS570_VIM_CHANCTRL--------------------*/
/* field: CHANMAPx0 - CHANMAPx 0(6-0). Interrupt CHANx 0 mapping control. */
#define TMS570_VIM_CHANCTRL_CHANMAPx0(val) BSP_FLD32(val,24, 30)
#define TMS570_VIM_CHANCTRL_CHANMAPx0_GET(reg) BSP_FLD32GET(reg,24, 30)
#define TMS570_VIM_CHANCTRL_CHANMAPx0_SET(reg,val) BSP_FLD32SET(reg, val,24, 30)

/* field: CHANMAPx1 - CHANMAPx 1(6-0). Interrupt CHANx 1 mapping control. */
#define TMS570_VIM_CHANCTRL_CHANMAPx1(val) BSP_FLD32(val,16, 22)
#define TMS570_VIM_CHANCTRL_CHANMAPx1_GET(reg) BSP_FLD32GET(reg,16, 22)
#define TMS570_VIM_CHANCTRL_CHANMAPx1_SET(reg,val) BSP_FLD32SET(reg, val,16, 22)

/* field: CHANMAPx2 - CHANMAPx 2(6-0). Interrupt CHANx 2 mapping control. */
#define TMS570_VIM_CHANCTRL_CHANMAPx2(val) BSP_FLD32(val,8, 14)
#define TMS570_VIM_CHANCTRL_CHANMAPx2_GET(reg) BSP_FLD32GET(reg,8, 14)
#define TMS570_VIM_CHANCTRL_CHANMAPx2_SET(reg,val) BSP_FLD32SET(reg, val,8, 14)

/* field: CHANMAPx3 - CHANMAPx 3(6-0). Interrupt CHANx 3 mapping control. */
#define TMS570_VIM_CHANCTRL_CHANMAPx3(val) BSP_FLD32(val,0, 6)
#define TMS570_VIM_CHANCTRL_CHANMAPx3_GET(reg) BSP_FLD32GET(reg,0, 6)
#define TMS570_VIM_CHANCTRL_CHANMAPx3_SET(reg,val) BSP_FLD32SET(reg, val,0, 6)



#endif /* LIBBSP_ARM_TMS570_VIM */

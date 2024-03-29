/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This header file provides PMM interfaces.
 */

/* The header file is generated by make_header.py from PMM.json */
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
#ifndef LIBBSP_ARM_TMS570_PMM
#define LIBBSP_ARM_TMS570_PMM

#include <bsp/utility.h>

typedef struct{
  uint32_t LOGICPDPWRCTRL0;   /*Logic Power Domain Control Register 0*/
  uint8_t reserved1 [12];
  uint32_t MEMPDPWRCTRL0;     /*Memory Power Domain Control Register 0*/
  uint8_t reserved2 [12];
  uint32_t PDCLKDISREG;       /*Power Domain Clock Disable Register*/
  uint32_t PDCLKDISSETREG;    /*Power Domain Clock Disable Set Register*/
  uint32_t PDCLKDISCLRREG;    /*Power Domain Clock Disable Clear Register*/
  uint8_t reserved3 [20];
  uint32_t LOGICPDPWRSTAT[4]; /*Logic Power Domain PD2 Power Status Registers*/
  uint8_t reserved4 [48];
  uint32_t MEMPDPWRSTAT[3];   /*Memory Power Domain RAM_PD1 Power Status Registers*/
  uint8_t reserved5 [20];
  uint32_t GLOBALCTRL1;       /*Global Control Register 1*/
  uint8_t reserved6 [4];
  uint32_t GLOBALSTAT;        /*Global Status Register*/
  uint32_t PRCKEYREG;         /*PSCON Diagnostic Compare Key Register*/
  uint32_t LPDDCSTAT1;        /*LogicPD PSCON Diagnostic Compare Status Register 1*/
  uint32_t LPDDCSTAT2;        /*LogicPD PSCON Diagnostic Compare Status Register 2*/
  uint32_t MPDDCSTAT1;        /*Memory PD PSCON Diagnostic Compare Status Register 1*/
  uint32_t MPDDCSTAT2;        /*Memory PD PSCON Diagnostic Compare Status Register 2*/
  uint32_t ISODIAGSTAT;       /*Isolation Diagnostic Status Register*/
} tms570_pmm_t;


/*-----------------TMS570_PMM_LOGICPDPWRCTRL0-----------------*/
/* field: LOGICPDON0 - Read in User and Privileged Mode. Write in Privileged Mode only. */
#define TMS570_PMM_LOGICPDPWRCTRL0_LOGICPDON0(val) BSP_FLD32(val,24, 27)
#define TMS570_PMM_LOGICPDPWRCTRL0_LOGICPDON0_GET(reg) BSP_FLD32GET(reg,24, 27)
#define TMS570_PMM_LOGICPDPWRCTRL0_LOGICPDON0_SET(reg,val) BSP_FLD32SET(reg, val,24, 27)

/* field: LOGICPDON1 - Read in User and Privileged Mode. Write in Privileged Mode only. */
#define TMS570_PMM_LOGICPDPWRCTRL0_LOGICPDON1(val) BSP_FLD32(val,16, 19)
#define TMS570_PMM_LOGICPDPWRCTRL0_LOGICPDON1_GET(reg) BSP_FLD32GET(reg,16, 19)
#define TMS570_PMM_LOGICPDPWRCTRL0_LOGICPDON1_SET(reg,val) BSP_FLD32SET(reg, val,16, 19)

/* field: LOGICPDON2 - Read in User and Privileged Mode. Write in Privileged Mode only. */
#define TMS570_PMM_LOGICPDPWRCTRL0_LOGICPDON2(val) BSP_FLD32(val,8, 11)
#define TMS570_PMM_LOGICPDPWRCTRL0_LOGICPDON2_GET(reg) BSP_FLD32GET(reg,8, 11)
#define TMS570_PMM_LOGICPDPWRCTRL0_LOGICPDON2_SET(reg,val) BSP_FLD32SET(reg, val,8, 11)

/* field: LOGICPDON3 - Read in User and Privileged Mode. Write in Privileged Mode only. */
#define TMS570_PMM_LOGICPDPWRCTRL0_LOGICPDON3(val) BSP_FLD32(val,0, 3)
#define TMS570_PMM_LOGICPDPWRCTRL0_LOGICPDON3_GET(reg) BSP_FLD32GET(reg,0, 3)
#define TMS570_PMM_LOGICPDPWRCTRL0_LOGICPDON3_SET(reg,val) BSP_FLD32SET(reg, val,0, 3)


/*------------------TMS570_PMM_MEMPDPWRCTRL0------------------*/
/* field: MEMPDON0 - Read in User and Privileged Mode. Write in Privileged Mode only. */
#define TMS570_PMM_MEMPDPWRCTRL0_MEMPDON0(val) BSP_FLD32(val,24, 27)
#define TMS570_PMM_MEMPDPWRCTRL0_MEMPDON0_GET(reg) BSP_FLD32GET(reg,24, 27)
#define TMS570_PMM_MEMPDPWRCTRL0_MEMPDON0_SET(reg,val) BSP_FLD32SET(reg, val,24, 27)

/* field: MEMPDON1 - Read in User and Privileged Mode. Write in Privileged Mode only. */
#define TMS570_PMM_MEMPDPWRCTRL0_MEMPDON1(val) BSP_FLD32(val,16, 19)
#define TMS570_PMM_MEMPDPWRCTRL0_MEMPDON1_GET(reg) BSP_FLD32GET(reg,16, 19)
#define TMS570_PMM_MEMPDPWRCTRL0_MEMPDON1_SET(reg,val) BSP_FLD32SET(reg, val,16, 19)

/* field: MEMPDON2 - Read in User and Privileged Mode. Write in Privileged Mode only. */
#define TMS570_PMM_MEMPDPWRCTRL0_MEMPDON2(val) BSP_FLD32(val,8, 11)
#define TMS570_PMM_MEMPDPWRCTRL0_MEMPDON2_GET(reg) BSP_FLD32GET(reg,8, 11)
#define TMS570_PMM_MEMPDPWRCTRL0_MEMPDON2_SET(reg,val) BSP_FLD32SET(reg, val,8, 11)


/*-------------------TMS570_PMM_PDCLKDISREG-------------------*/
/* field: PDCLK_DIS_3 - Read in User and Privileged Mode returns the current value of PDCLK_DIS[3]. */
#define TMS570_PMM_PDCLKDISREG_PDCLK_DIS_3 BSP_BIT32(3)

/* field: PDCLK_DIS_2 - Read in User and Privileged Mode returns the current value of PDCLK_DIS[2]. */
#define TMS570_PMM_PDCLKDISREG_PDCLK_DIS_2 BSP_BIT32(2)

/* field: PDCLK_DIS_1 - ead in User and Privileged Mode returns the current value of PDCLK_DIS[1]. */
#define TMS570_PMM_PDCLKDISREG_PDCLK_DIS_1 BSP_BIT32(1)

/* field: PDCLK_DIS_0 - Read in User and Privileged Mode returns the current value of PDCLK_DIS[0]. */
#define TMS570_PMM_PDCLKDISREG_PDCLK_DIS_0 BSP_BIT32(0)


/*-----------------TMS570_PMM_PDCLKDISSETREG-----------------*/
/* field: PDCLK_DISSET_3 - Read in User and Privileged Mode returns the current value of PDCLK_DISSET[3]. */
#define TMS570_PMM_PDCLKDISSETREG_PDCLK_DISSET_3 BSP_BIT32(3)

/* field: PDCLK_DISSET_2 - Privileged Mode only. */
#define TMS570_PMM_PDCLKDISSETREG_PDCLK_DISSET_2 BSP_BIT32(2)

/* field: PDCLK_DISSET_1 - Read in User and Privileged Mode returns the current value of PDCLK_DISSET[1]. */
#define TMS570_PMM_PDCLKDISSETREG_PDCLK_DISSET_1 BSP_BIT32(1)

/* field: PDCLK_DISSET_0 - Read in User and Privileged Mode returns the current value of PDCLK_DISSET[0]. */
#define TMS570_PMM_PDCLKDISSETREG_PDCLK_DISSET_0 BSP_BIT32(0)


/*-----------------TMS570_PMM_PDCLKDISCLRREG-----------------*/
/* field: PDCLK_DISCLR_3 - PDCLK_DISCLR[3] */
#define TMS570_PMM_PDCLKDISCLRREG_PDCLK_DISCLR_3 BSP_BIT32(3)

/* field: PDCLK_DISCLR_2 - Read in User and Privileged Mode returns the current value of PDCLK_DIS[2]. */
#define TMS570_PMM_PDCLKDISCLRREG_PDCLK_DISCLR_2 BSP_BIT32(2)

/* field: PDCLK_DISCLR_1 - Read in User and Privileged Mode returns the current value of PDCLK_DIS[1]. */
#define TMS570_PMM_PDCLKDISCLRREG_PDCLK_DISCLR_1 BSP_BIT32(1)

/* field: PDCLK_DISCLR_0 - Read in User and Privileged Mode returns the current value of PDCLK_DIS[0]. */
#define TMS570_PMM_PDCLKDISCLRREG_PDCLK_DISCLR_0 BSP_BIT32(0)


/*-----------------TMS570_PMM_LOGICPDPWRSTAT-----------------*/
/* field: LOGIC_IN_TRANS0 - Logic in transition status for power domain PD2. */
#define TMS570_PMM_LOGICPDPWRSTAT_LOGIC_IN_TRANS0 BSP_BIT32(24)

/* field: MEM_IN_TRANS0 - Memory in transition status for power domain PD2. */
#define TMS570_PMM_LOGICPDPWRSTAT_MEM_IN_TRANS0 BSP_BIT32(16)

/* field: DOMAIN_ON0 - Current state of power domain PD2. */
#define TMS570_PMM_LOGICPDPWRSTAT_DOMAIN_ON0 BSP_BIT32(8)

/* field: LOGICPDPWR_STAT0 - Logic power domain PD2 power state. */
#define TMS570_PMM_LOGICPDPWRSTAT_LOGICPDPWR_STAT0(val) BSP_FLD32(val,0, 1)
#define TMS570_PMM_LOGICPDPWRSTAT_LOGICPDPWR_STAT0_GET(reg) BSP_FLD32GET(reg,0, 1)
#define TMS570_PMM_LOGICPDPWRSTAT_LOGICPDPWR_STAT0_SET(reg,val) BSP_FLD32SET(reg, val,0, 1)


/*------------------TMS570_PMM_MEMPDPWRSTAT------------------*/
/* field: LOGIC_IN_TRANS0 - Logic in transition status for power domain RAM_PD1. */
#define TMS570_PMM_MEMPDPWRSTAT_LOGIC_IN_TRANS0 BSP_BIT32(24)

/* field: MEM_IN_TRANS0 - Memory in transition status for power domain RAM_PD1. */
#define TMS570_PMM_MEMPDPWRSTAT_MEM_IN_TRANS0 BSP_BIT32(16)

/* field: DOMAIN_ON0 - Current state of power domain RAM_PD1. */
#define TMS570_PMM_MEMPDPWRSTAT_DOMAIN_ON0 BSP_BIT32(8)

/* field: MEMPDPWR_STAT0 - Memory power domain RAM_PD1 power state. */
#define TMS570_PMM_MEMPDPWRSTAT_MEMPDPWR_STAT0(val) BSP_FLD32(val,0, 1)
#define TMS570_PMM_MEMPDPWRSTAT_MEMPDPWR_STAT0_GET(reg) BSP_FLD32GET(reg,0, 1)
#define TMS570_PMM_MEMPDPWRSTAT_MEMPDPWR_STAT0_SET(reg,val) BSP_FLD32SET(reg, val,0, 1)


/*-------------------TMS570_PMM_GLOBALCTRL1-------------------*/
/* field: PMCTRL_PWRDN - PMC/PSCON Power Down */
#define TMS570_PMM_GLOBALCTRL1_PMCTRL_PWRDN BSP_BIT32(8)

/* field: AUTO_CLK_WAKE_ENA - Automatic Clock Enable on Wake Up */
#define TMS570_PMM_GLOBALCTRL1_AUTO_CLK_WAKE_ENA BSP_BIT32(0)


/*-------------------TMS570_PMM_GLOBALSTAT-------------------*/
/* field: PMCTRL_IDLE - State of PMC and all PSCONs. */
#define TMS570_PMM_GLOBALSTAT_PMCTRL_IDLE BSP_BIT32(0)


/*--------------------TMS570_PMM_PRCKEYREG--------------------*/
/* field: MKEY - Diagnostic PSCON Mode Key. The mode key is applied to all individual PSCON compare units. */
#define TMS570_PMM_PRCKEYREG_MKEY(val) BSP_FLD32(val,0, 3)
#define TMS570_PMM_PRCKEYREG_MKEY_GET(reg) BSP_FLD32GET(reg,0, 3)
#define TMS570_PMM_PRCKEYREG_MKEY_SET(reg,val) BSP_FLD32SET(reg, val,0, 3)


/*-------------------TMS570_PMM_LPDDCSTAT1-------------------*/
/* field: LCMPE - Logic Power Domain Compare Error */
#define TMS570_PMM_LPDDCSTAT1_LCMPE(val) BSP_FLD32(val,16, 19)
#define TMS570_PMM_LPDDCSTAT1_LCMPE_GET(reg) BSP_FLD32GET(reg,16, 19)
#define TMS570_PMM_LPDDCSTAT1_LCMPE_SET(reg,val) BSP_FLD32SET(reg, val,16, 19)

/* field: LSTC - Logic Power Domain Self-test Complete */
#define TMS570_PMM_LPDDCSTAT1_LSTC(val) BSP_FLD32(val,0, 3)
#define TMS570_PMM_LPDDCSTAT1_LSTC_GET(reg) BSP_FLD32GET(reg,0, 3)
#define TMS570_PMM_LPDDCSTAT1_LSTC_SET(reg,val) BSP_FLD32SET(reg, val,0, 3)


/*-------------------TMS570_PMM_LPDDCSTAT2-------------------*/
/* field: LSTET - Logic Power Domain Self-test Error Type */
#define TMS570_PMM_LPDDCSTAT2_LSTET(val) BSP_FLD32(val,16, 19)
#define TMS570_PMM_LPDDCSTAT2_LSTET_GET(reg) BSP_FLD32GET(reg,16, 19)
#define TMS570_PMM_LPDDCSTAT2_LSTET_SET(reg,val) BSP_FLD32SET(reg, val,16, 19)

/* field: LSTE - Logic Power Domain Self-test Error */
#define TMS570_PMM_LPDDCSTAT2_LSTE(val) BSP_FLD32(val,0, 3)
#define TMS570_PMM_LPDDCSTAT2_LSTE_GET(reg) BSP_FLD32GET(reg,0, 3)
#define TMS570_PMM_LPDDCSTAT2_LSTE_SET(reg,val) BSP_FLD32SET(reg, val,0, 3)


/*-------------------TMS570_PMM_MPDDCSTAT1-------------------*/
/* field: MCMPE - Memory Power Domain Compare Error */
#define TMS570_PMM_MPDDCSTAT1_MCMPE(val) BSP_FLD32(val,16, 18)
#define TMS570_PMM_MPDDCSTAT1_MCMPE_GET(reg) BSP_FLD32GET(reg,16, 18)
#define TMS570_PMM_MPDDCSTAT1_MCMPE_SET(reg,val) BSP_FLD32SET(reg, val,16, 18)

/* field: MSTC - Memory Power Domain Self-test Complete */
#define TMS570_PMM_MPDDCSTAT1_MSTC(val) BSP_FLD32(val,0, 2)
#define TMS570_PMM_MPDDCSTAT1_MSTC_GET(reg) BSP_FLD32GET(reg,0, 2)
#define TMS570_PMM_MPDDCSTAT1_MSTC_SET(reg,val) BSP_FLD32SET(reg, val,0, 2)


/*-------------------TMS570_PMM_MPDDCSTAT2-------------------*/
/* field: MSTET - Memory Power Domain Self-test Error Type */
#define TMS570_PMM_MPDDCSTAT2_MSTET(val) BSP_FLD32(val,16, 18)
#define TMS570_PMM_MPDDCSTAT2_MSTET_GET(reg) BSP_FLD32GET(reg,16, 18)
#define TMS570_PMM_MPDDCSTAT2_MSTET_SET(reg,val) BSP_FLD32SET(reg, val,16, 18)

/* field: MSTE - Memory Power Domain Self-test Error */
#define TMS570_PMM_MPDDCSTAT2_MSTE(val) BSP_FLD32(val,0, 2)
#define TMS570_PMM_MPDDCSTAT2_MSTE_GET(reg) BSP_FLD32GET(reg,0, 2)
#define TMS570_PMM_MPDDCSTAT2_MSTE_SET(reg,val) BSP_FLD32SET(reg, val,0, 2)


/*-------------------TMS570_PMM_ISODIAGSTAT-------------------*/
/* field: ISO_DIAG - Isolation Diagnostic */
#define TMS570_PMM_ISODIAGSTAT_ISO_DIAG(val) BSP_FLD32(val,0, 3)
#define TMS570_PMM_ISODIAGSTAT_ISO_DIAG_GET(reg) BSP_FLD32GET(reg,0, 3)
#define TMS570_PMM_ISODIAGSTAT_ISO_DIAG_SET(reg,val) BSP_FLD32SET(reg, val,0, 3)



#endif /* LIBBSP_ARM_TMS570_PMM */

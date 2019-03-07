/*
 * Modifications of the original file provided by Freescale are:
 *
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************/
/* FILE NAME: mpc5554.h                      COPYRIGHT (c) Freescale 2007 */
/* VERSION:  1.7                                  All Rights Reserved     */
/*                                                                        */
/* DESCRIPTION:                                                           */
/* This file contain all of the register and bit field definitions for    */
/* MPC5554.                                                               */
/*========================================================================*/
/* UPDATE HISTORY                                                         */
/* REV      AUTHOR      DATE       DESCRIPTION OF CHANGE                  */
/* ---   -----------  ---------    ---------------------                  */
/* 0.01  J. Loeliger  03/Mar/03    Initial version of file for MPC5554.   */
/*                                  Based on SoC version 0.7.             */
/* 0.02  J. Loeliger  05/Mar/03    All registers and bit fields now       */
/*                                  defined.                              */
/* 0.03  J. Loeliger  05/May/03    Updated to current spec., fixed several*/
/*                                   bugs and naming/formating issues.    */
/* 0.04  J. Loeliger  16/May/03    More fixes and naming/formating issues.*/
/* 0.05  J. Loeliger  19/Aug/03    Updated for latest documentation.      */
/* 0.06  J. Loeliger  03/Sep/03    Changed to include motint.h            */
/*                                  Updated many register names.          */
/* 0.07  J. Loeliger  04/Nov/03    Changed to include typedefs.h and more */
/*                                  register name updates.                */
/* 0.08  J. Loeliger  25/Feb/04    Added MetroWerks #pragmas.             */
/*                                  Updated for user manual 1.0           */
/* 0.09  J. Loeliger  27/Feb/04    Updated eDMA tcd section and some more */
/*                                  bit field names to match user's man.  */
/* 0.10  J. Loeliger  01/Apr/04    Fixed register spacing in ADC and eTPU */
/* 0.11  J. Loeliger  16/Jun/04    Many fixes and updated to user's       */
/*                                  manual, also some testing done.       */
/* 0.12  J. Loeliger  25/Jun/04    Fixed problems in edma and eTPU.       */
/* 0.13  J. Loeliger  16/Jul/04    Fixed mistake in FlexCAN TIMER size and*/
/*                                  changed eTPU memory defs to start with*/
/*                                  ETPU_                                 */
/* 0.14  J. Loeliger  17/Nov/04    Added ETPU_CODE_RAM definition.        */
/*                                  All code moved to CVS repository.     */
/*                                  Updated copyright to Freescale.       */
/*                                  Added new SCMOFFDATAR register to eTPU*/
/*                                  Fixed REDCR_A&B bit fields in eTPU.   */
/*                                  Added new DBR bit in CTAR for DSPI.   */
/* 0.15  J. Loeliger  29/Nov/04    Added support for new eTPU util funcs. */
/*                                  Added bit fields for FlexCAN buffer ID*/
/* 0.16  J. Loeliger  01/Dec/04    Corrected comments in  release 0.16.   */
/* 0.17  J. Loeliger  02/Dec/04    Moved eTPU variable definitions to a   */
/*                                   seperate new file.                   */
/*                                   Removed SIU variable the GPIO        */
/*                                   routines do not need it.             */
/* 1.0  G.Emerson     22/Feb/05    No real changes to this file.          */
/*                                 Joint generation with mpc5553.h        */
/* 1.1   G. Emerson   6/Jun/05     Changes to SIU to allow for upward     */
/*                                 expansion of PCR/GPDI/GPDO             */
/*                                 Added #defines for memory sizes etc    */
/* 1.2   G. Emerson   21/Sep/05    PBRIDGES fixes                         */
/* 1.3   G. Emerson   03/Jan/06    Pbridge MPCR/PACR/OPACR now generic    */
/*                                 XBAR MPR now generic                   */
/*                                 ECSM has FSBMCR on all integrations    */
/* 1.4   G. Emerson   24/Jan/06    Make Pbridges, XBAR, Flash BIU         */
/*                                 integration specific                   */
/* 1.5   S. Mathieson 28/Jul/06    Split out unused bit to support build  */
/*                                 process. No real change.               */
/* 1.6   S. Mathieson 30/Aug/06    SPR: L1SCR0, updated bit name from DPP */
/*                                 to DPB to align with documentation.    */
/* 1.7	 S. Mathieson 26/Feb/07    eDMA TCD format updated to include     */
/*                                 alternate configuration. INTC,         */
/*                                 correction to the number of PSR        */
/*                                 registers.                             */
/**************************************************************************/
/*>>>>NOTE! this file is auto-generated please do not edit it!<<<<*/

#ifndef _MPC5554_H_
#define _MPC5554_H_

#ifndef ASM

#include <stdint.h>

#include <mpc55xx/regs-edma.h>

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef __MWERKS__
#pragma push
#pragma ANSI_strict off
#endif

/****************************************************************************/
/*                          MODULE : PBRIDGE_A Peripheral Bridge            */
/****************************************************************************/
    struct PBRIDGE_A_tag {
        union {
            uint32_t R;
            struct {
                uint32_t MBW0:1;
                uint32_t MTR0:1;
                uint32_t MTW0:1;
                uint32_t MPL0:1;
                uint32_t MBW1:1;
                uint32_t MTR1:1;
                uint32_t MTW1:1;
                uint32_t MPL1:1;
                uint32_t MBW2:1;
                uint32_t MTR2:1;
                uint32_t MTW2:1;
                uint32_t MPL2:1;
                uint32_t MBW3:1;
                uint32_t MTR3:1;
                uint32_t MTW3:1;
                uint32_t MPL3:1;

                  uint32_t:4;

                  uint32_t:4;

                  uint32_t:4;

                  uint32_t:4;
            } B;
        } MPCR;                 /* Master Privilege Control Register */

        uint32_t pbridge_a_reserved2[7];

        union {
            uint32_t R;
            struct {
                uint32_t BW0:1;
                uint32_t SP0:1;
                uint32_t WP0:1;
                uint32_t TP0:1;
                  uint32_t:28;
            } B;
        } PACR0;

        uint32_t pbridge_a_reserved3[7];

        union {
            uint32_t R;
            struct {
                uint32_t BW0:1;
                uint32_t SP0:1;
                uint32_t WP0:1;
                uint32_t TP0:1;
                uint32_t BW1:1;
                uint32_t SP1:1;
                uint32_t WP1:1;
                uint32_t TP1:1;
                uint32_t BW2:1;
                uint32_t SP2:1;
                uint32_t WP2:1;
                uint32_t TP2:1;
                  uint32_t:4;
                uint32_t BW4:1;
                uint32_t SP4:1;
                uint32_t WP4:1;
                uint32_t TP4:1;
                  uint32_t:12;
            } B;
        } OPACR0;

        union {
            uint32_t R;
            struct {

                uint32_t BW0:1;        /* EMIOS */
                uint32_t SP0:1;
                uint32_t WP0:1;
                uint32_t TP0:1;

                  uint32_t:28;
            } B;
        } OPACR1;

        union {
            uint32_t R;
            struct {
                uint32_t BW0:1;
                uint32_t SP0:1;
                uint32_t WP0:1;
                uint32_t TP0:1;
                  uint32_t:4;
                uint32_t BW2:1;
                uint32_t SP2:1;
                uint32_t WP2:1;
                uint32_t TP2:1;
                uint32_t BW3:1;
                uint32_t SP3:1;
                uint32_t WP3:1;
                uint32_t TP3:1;
                uint32_t BW4:1;
                uint32_t SP4:1;
                uint32_t WP4:1;
                uint32_t TP4:1;
                  uint32_t:12;
            } B;
        } OPACR2;

    };

/****************************************************************************/
/*                          MODULE : PBRIDGE_B Peripheral Bridge            */
/****************************************************************************/
    struct PBRIDGE_B_tag {
        union {
            uint32_t R;
            struct {
                uint32_t MBW0:1;
                uint32_t MTR0:1;
                uint32_t MTW0:1;
                uint32_t MPL0:1;
                uint32_t MBW1:1;
                uint32_t MTR1:1;
                uint32_t MTW1:1;
                uint32_t MPL1:1;
                uint32_t MBW2:1;
                uint32_t MTR2:1;
                uint32_t MTW2:1;
                uint32_t MPL2:1;
                uint32_t MBW3:1;
                uint32_t MTR3:1;
                uint32_t MTW3:1;
                uint32_t MPL3:1;

                  uint32_t:4;

                  uint32_t:4;

                  uint32_t:4;

                  uint32_t:4;
            } B;
        } MPCR;                 /* Master Privilege Control Register */

        uint32_t pbridge_b_reserved2[7];

        union {
            uint32_t R;
            struct {
                uint32_t BW0:1;
                uint32_t SP0:1;
                uint32_t WP0:1;
                uint32_t TP0:1;
                uint32_t BW1:1;
                uint32_t SP1:1;
                uint32_t WP1:1;
                uint32_t TP1:1;
                  uint32_t:24;
            } B;
        } PACR0;

        uint32_t pbridge_b_reserved3;

        union {
            uint32_t R;
            struct {
                uint32_t BW0:1;
                uint32_t SP0:1;
                uint32_t WP0:1;
                uint32_t TP0:1;
                uint32_t BW1:1;
                uint32_t SP1:1;
                uint32_t WP1:1;
                uint32_t TP1:1;
                uint32_t BW2:1;
                uint32_t SP2:1;
                uint32_t WP2:1;
                uint32_t TP2:1;

                  uint32_t:4;

                  uint32_t:16;

            } B;
        } PACR2;

        uint32_t pbridge_b_reserved4[5];

        union {
            uint32_t R;
            struct {
                uint32_t BW0:1;
                uint32_t SP0:1;
                uint32_t WP0:1;
                uint32_t TP0:1;
                  uint32_t:12;

                uint32_t BW4:1;        /* DSPI_A */
                uint32_t SP4:1;
                uint32_t WP4:1;
                uint32_t TP4:1;

                uint32_t BW5:1;        /* DSPI_B */
                uint32_t SP5:1;
                uint32_t WP5:1;
                uint32_t TP5:1;

                uint32_t BW6:1;
                uint32_t SP6:1;
                uint32_t WP6:1;
                uint32_t TP6:1;
                uint32_t BW7:1;
                uint32_t SP7:1;
                uint32_t WP7:1;
                uint32_t TP7:1;
            } B;
        } OPACR0;

        union {
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t BW4:1;
                uint32_t SP4:1;
                uint32_t WP4:1;
                uint32_t TP4:1;

                uint32_t BW5:1;        /* ESCI_B */
                uint32_t SP5:1;
                uint32_t WP5:1;
                uint32_t TP5:1;

                  uint32_t:8;
            } B;
        } OPACR1;

        union {
            uint32_t R;
            struct {
                uint32_t BW0:1;
                uint32_t SP0:1;
                uint32_t WP0:1;
                uint32_t TP0:1;

                uint32_t BW1:1;        /* CAN_B */
                uint32_t SP1:1;
                uint32_t WP1:1;
                uint32_t TP1:1;

                uint32_t BW2:1;
                uint32_t SP2:1;
                uint32_t WP2:1;
                uint32_t TP2:1;

                  uint32_t:4;

                  uint32_t:4;

                  uint32_t:12;
            } B;
        } OPACR2;

        union {
            uint32_t R;
            struct {

                uint32_t:4;

                uint32_t:24;
                uint32_t BW7:1;
                uint32_t SP7:1;
                uint32_t WP7:1;
                uint32_t TP7:1;
            } B;
        } OPACR3;

    };
/****************************************************************************/
/*                     MODULE : FMPLL                                       */
/****************************************************************************/
    struct FMPLL_tag {
        union FMPLL_SYNCR_tag {
            uint32_t R;
            struct {
                uint32_t:1;
                uint32_t PREDIV:3;
                uint32_t MFD:5;
                  uint32_t:1;
                uint32_t RFD:3;
                uint32_t LOCEN:1;
                uint32_t LOLRE:1;
                uint32_t LOCRE:1;
                uint32_t DISCLK:1;
                uint32_t LOLIRQ:1;
                uint32_t LOCIRQ:1;
                uint32_t RATE:1;
                uint32_t DEPTH:2;
                uint32_t EXP:10;
            } B;
        } SYNCR;

        union FMPLL_SYNSR_tag {
            uint32_t R;
            struct {
                uint32_t:22;
                uint32_t LOLF:1;
                uint32_t LOC:1;
                uint32_t MODE:1;
                uint32_t PLLSEL:1;
                uint32_t PLLREF:1;
                uint32_t LOCKS:1;
                uint32_t LOCK:1;
                uint32_t LOCF:1;
                uint32_t CALDONE:1;
                uint32_t CALPASS:1;
            } B;
        } SYNSR;

    };
/****************************************************************************/
/*                     MODULE : External Bus Interface (EBI)                */
/****************************************************************************/
    struct EBI_CS_tag {
        union {                 /* Base Register Bank */
            uint32_t R;
            struct {
                uint32_t BA:17;
                  uint32_t:3;
                uint32_t PS:1;
                  uint32_t:4;
                uint32_t BL:1;
                uint32_t WEBS:1;
                uint32_t TBDIP:1;
                  uint32_t:2;
                uint32_t BI:1;
                uint32_t V:1;
            } B;
        } BR;

        union {                 /* Option Register Bank */
            uint32_t R;
            struct {
                uint32_t AM:17;
                  uint32_t:7;
                uint32_t SCY:4;
                  uint32_t:1;
                uint32_t BSCY:2;
                  uint32_t:1;
            } B;
        } OR;
    };

    struct EBI_CAL_CS_tag {
	    uint32_t ebi_cal_cs_reserved [2];
    };

    struct EBI_tag {
        union EBI_MCR_tag {     /* Module Configuration Register */
            uint32_t R;
            struct {
                uint32_t:5;
                uint32_t SIZEEN:1;
                uint32_t SIZE:2;
                  uint32_t:8;
                uint32_t ACGE:1;
                uint32_t EXTM:1;
                uint32_t EARB:1;
                uint32_t EARP:2;
                  uint32_t:4;
                uint32_t MDIS:1;
                  uint32_t:5;
                uint32_t DBM:1;
            } B;
        } MCR;

        uint32_t EBI_reserved1;

        union {                 /* Transfer Error Status Register */
            uint32_t R;
            struct {
                uint32_t:30;
                uint32_t TEAF:1;
                uint32_t BMTF:1;
            } B;
        } TESR;

        union {                 /* Bus Monitor Control Register */
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t BMT:8;
                uint32_t BME:1;
                  uint32_t:7;
            } B;
        } BMCR;

        struct EBI_CS_tag CS[4];

        uint32_t EBI_reserved2[4];

        struct EBI_CAL_CS_tag CAL_CS[4];
    };
/****************************************************************************/
/*                     MODULE : FLASH                                       */
/****************************************************************************/
    struct FLASH_tag {
        union {                 /* Module Configuration Register */
            uint32_t R;
            struct {
                uint32_t:4;
                uint32_t SIZE:4;
                  uint32_t:1;
                uint32_t LAS:3;
                  uint32_t:3;
                uint32_t MAS:1;
                uint32_t EER:1;
                uint32_t RWE:1;
                uint32_t BBEPE:1;
                uint32_t EPE:1;
                uint32_t PEAS:1;
                uint32_t DONE:1;
                uint32_t PEG:1;

                  uint32_t:2;

                uint32_t STOP:1;
                  uint32_t:1;
                uint32_t PGM:1;
                uint32_t PSUS:1;
                uint32_t ERS:1;
                uint32_t ESUS:1;
                uint32_t EHV:1;
            } B;
        } MCR;

        union LMLR_tag {        /* LML Register */
            uint32_t R;
            struct {
                uint32_t LME:1;
                  uint32_t:10;
                uint32_t SLOCK:1;
                uint32_t MLOCK:4;
                uint32_t LLOCK:16;
            } B;
        } LMLR;

        union HLR_tag {         /* HL Register */
            uint32_t R;
            struct {
                uint32_t HBE:1;
                  uint32_t:3;
                uint32_t HBLOCK:28;
            } B;
        } HLR;

        union SLMLR_tag {       /* SLML Register */
            uint32_t R;
            struct {
                uint32_t SLE:1;
                  uint32_t:10;
                uint32_t SSLOCK:1;
                uint32_t SMLOCK:4;
                uint32_t SLLOCK:16;
            } B;
        } SLMLR;

        union {                 /* LMS Register */
            uint32_t R;
            struct {
                uint32_t:12;
                uint32_t MSEL:4;
                uint32_t LSEL:16;
            } B;
        } LMSR;

        union {
            uint32_t R;
            struct {
                uint32_t:4;
                uint32_t HBSEL:28;
            } B;
        } HSR;

        union {
            uint32_t R;
            struct {
                uint32_t:10;
                uint32_t ADDR:19;
                  uint32_t:3;
            } B;
        } AR;

        union {
            uint32_t R;
            struct {

                uint32_t:11;

                uint32_t:1;

                uint32_t M3PFE:1;
                uint32_t M2PFE:1;
                uint32_t M1PFE:1;
                uint32_t M0PFE:1;
                uint32_t APC:3;
                uint32_t WWSC:2;
                uint32_t RWSC:3;

                uint32_t DPFEN:2;
                uint32_t IPFEN:2;

                uint32_t PFLIM:3;
                uint32_t BFEN:1;
            } B;
        } BIUCR;

        union {
            uint32_t R;
            struct {

                uint32_t:22;

                uint32_t:2;

                uint32_t M3AP:2;
                uint32_t M2AP:2;
                uint32_t M1AP:2;
                uint32_t M0AP:2;
            } B;
        } BIUAPR;
    };
/****************************************************************************/
/*                     MODULE : SIU                                         */
/****************************************************************************/
    struct SIU_tag {
        int32_t SIU_reserved0;

        union {                 /* MCU ID Register */
            uint32_t R;
            struct {
                uint32_t PARTNUM:16;
                uint32_t MASKNUM:16;
            } B;
        } MIDR;
        int32_t SIU_reserved00;

        union {                 /* Reset Status Register */
            uint32_t R;
            struct {
                uint32_t PORS:1;
                uint32_t ERS:1;
                uint32_t LLRS:1;
                uint32_t LCRS:1;
                uint32_t WDRS:1;
                uint32_t CRS:1;
                  uint32_t:8;
                uint32_t SSRS:1;
                uint32_t SERF:1;
                uint32_t WKPCFG:1;
                  uint32_t:12;
                uint32_t BOOTCFG:2;
                uint32_t RGF:1;
            } B;
        } RSR;

        union {                 /* System Reset Control Register */
            uint32_t R;
            struct {
                uint32_t SSR:1;
                uint32_t SER:1;
                  uint32_t:14;
                uint32_t CRE:1;
                  uint32_t:15;
            } B;
        } SRCR;

        union SIU_EISR_tag {    /* External Interrupt Status Register */
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t EIF15:1;
                uint32_t EIF14:1;
                uint32_t EIF13:1;
                uint32_t EIF12:1;
                uint32_t EIF11:1;
                uint32_t EIF10:1;
                uint32_t EIF9:1;
                uint32_t EIF8:1;
                uint32_t EIF7:1;
                uint32_t EIF6:1;
                uint32_t EIF5:1;
                uint32_t EIF4:1;
                uint32_t EIF3:1;
                uint32_t EIF2:1;
                uint32_t EIF1:1;
                uint32_t EIF0:1;
            } B;
        } EISR;

        union SIU_DIRER_tag {   /* DMA/Interrupt Request Enable Register */
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t EIRE15:1;
                uint32_t EIRE14:1;
                uint32_t EIRE13:1;
                uint32_t EIRE12:1;
                uint32_t EIRE11:1;
                uint32_t EIRE10:1;
                uint32_t EIRE9:1;
                uint32_t EIRE8:1;
                uint32_t EIRE7:1;
                uint32_t EIRE6:1;
                uint32_t EIRE5:1;
                uint32_t EIRE4:1;
                uint32_t EIRE3:1;
                uint32_t EIRE2:1;
                uint32_t EIRE1:1;
                uint32_t EIRE0:1;
            } B;
        } DIRER;

        union SIU_DIRSR_tag {   /* DMA/Interrupt Select Register */
            uint32_t R;
            struct {
                uint32_t:28;
                uint32_t DIRS3:1;
                uint32_t DIRS2:1;
                uint32_t DIRS1:1;
                uint32_t DIRS0:1;
            } B;
        } DIRSR;

        union {                 /* Overrun Status Register */
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t OVF15:1;
                uint32_t OVF14:1;
                uint32_t OVF13:1;
                uint32_t OVF12:1;
                uint32_t OVF11:1;
                uint32_t OVF10:1;
                uint32_t OVF9:1;
                uint32_t OVF8:1;
                uint32_t OVF7:1;
                uint32_t OVF6:1;
                uint32_t OVF5:1;
                uint32_t OVF4:1;
                uint32_t OVF3:1;
                uint32_t OVF2:1;
                uint32_t OVF1:1;
                uint32_t OVF0:1;
            } B;
        } OSR;

        union SIU_ORER_tag {    /* Overrun Request Enable Register */
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t ORE15:1;
                uint32_t ORE14:1;
                uint32_t ORE13:1;
                uint32_t ORE12:1;
                uint32_t ORE11:1;
                uint32_t ORE10:1;
                uint32_t ORE9:1;
                uint32_t ORE8:1;
                uint32_t ORE7:1;
                uint32_t ORE6:1;
                uint32_t ORE5:1;
                uint32_t ORE4:1;
                uint32_t ORE3:1;
                uint32_t ORE2:1;
                uint32_t ORE1:1;
                uint32_t ORE0:1;
            } B;
        } ORER;

        union SIU_IREER_tag {   /* External IRQ Rising-Edge Event Enable Register */
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t IREE15:1;
                uint32_t IREE14:1;
                uint32_t IREE13:1;
                uint32_t IREE12:1;
                uint32_t IREE11:1;
                uint32_t IREE10:1;
                uint32_t IREE9:1;
                uint32_t IREE8:1;
                uint32_t IREE7:1;
                uint32_t IREE6:1;
                uint32_t IREE5:1;
                uint32_t IREE4:1;
                uint32_t IREE3:1;
                uint32_t IREE2:1;
                uint32_t IREE1:1;
                uint32_t IREE0:1;
            } B;
        } IREER;

        union SIU_IFEER_tag {   /* External IRQ Falling-Edge Event Enable Register */
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t IFEE15:1;
                uint32_t IFEE14:1;
                uint32_t IFEE13:1;
                uint32_t IFEE12:1;
                uint32_t IFEE11:1;
                uint32_t IFEE10:1;
                uint32_t IFEE9:1;
                uint32_t IFEE8:1;
                uint32_t IFEE7:1;
                uint32_t IFEE6:1;
                uint32_t IFEE5:1;
                uint32_t IFEE4:1;
                uint32_t IFEE3:1;
                uint32_t IFEE2:1;
                uint32_t IFEE1:1;
                uint32_t IFEE0:1;
            } B;
        } IFEER;

        union SIU_IDFR_tag {    /* External IRQ Digital Filter Register */
            uint32_t R;
            struct {
                uint32_t:28;
                uint32_t DFL:4;
            } B;
        } IDFR;

        int32_t SIU_reserved1[3];

        union SIU_PCR_tag {     /* Pad Configuration Registers */
            uint16_t R;
            struct {
                uint16_t:3;
                uint16_t PA:3;
                uint16_t OBE:1;
                uint16_t IBE:1;
                uint16_t DSC:2;
                uint16_t ODE:1;
                uint16_t HYS:1;
                uint16_t SRC:2;
                uint16_t WPE:1;
                uint16_t WPS:1;
            } B;
        } PCR[512];

        int16_t SIU_reserved_0[224];

        union {                 /* GPIO Pin Data Output Registers */
            uint8_t R;
            struct {
                uint8_t:7;
                uint8_t PDO:1;
            } B;
        } GPDO[256];

        int32_t SIU_reserved_3[64];

        union {                 /* GPIO Pin Data Input Registers */
            uint8_t R;
            struct {
                uint8_t:7;
                uint8_t PDI:1;
            } B;
        } GPDI[256];

        union {                 /* IMUX Register */
            uint32_t R;
            struct {
                uint32_t TSEL5:2;
                uint32_t TSEL4:2;
                uint32_t TSEL3:2;
                uint32_t TSEL2:2;
                uint32_t TSEL1:2;
                uint32_t TSEL0:2;
                  uint32_t:20;
            } B;
        } ETISR;

        union {                 /* IMUX Register */
            uint32_t R;
            struct {
                uint32_t ESEL15:2;
                uint32_t ESEL14:2;
                uint32_t ESEL13:2;
                uint32_t ESEL12:2;
                uint32_t ESEL11:2;
                uint32_t ESEL10:2;
                uint32_t ESEL9:2;
                uint32_t ESEL8:2;
                uint32_t ESEL7:2;
                uint32_t ESEL6:2;
                uint32_t ESEL5:2;
                uint32_t ESEL4:2;
                uint32_t ESEL3:2;
                uint32_t ESEL2:2;
                uint32_t ESEL1:2;
                uint32_t ESEL0:2;
            } B;
        } EIISR;

        union {                 /* IMUX Register */
            uint32_t R;
            struct {
                uint32_t SINSELA:2;
                uint32_t SSSELA:2;
                uint32_t SCKSELA:2;
                uint32_t TRIGSELA:2;
                uint32_t SINSELB:2;
                uint32_t SSSELB:2;
                uint32_t SCKSELB:2;
                uint32_t TRIGSELB:2;
                uint32_t SINSELC:2;
                uint32_t SSSELC:2;
                uint32_t SCKSELC:2;
                uint32_t TRIGSELC:2;
                uint32_t SINSELD:2;
                uint32_t SSSELD:2;
                uint32_t SCKSELD:2;
                uint32_t TRIGSELD:2;
            } B;
        } DISR;

        int32_t SIU_reserved2[29];

        union {                 /* Chip Configuration Register Register */
            uint32_t R;
            struct {
                uint32_t:14;
                uint32_t MATCH:1;
                uint32_t DISNEX:1;
                  uint32_t:16;
            } B;
        } CCR;

        union {                 /* External Clock Configuration Register Register */
            uint32_t R;
            struct {
                uint32_t:18;
                uint32_t ENGDIV:6;
                  uint32_t:4;
                uint32_t EBTS:1;
                  uint32_t:1;
                uint32_t EBDF:2;
            } B;
        } ECCR;

        union {
            uint32_t R;
        } CARH;

        union {
            uint32_t R;
        } CARL;

        union {
            uint32_t R;
        } CBRH;

        union {
            uint32_t R;
        } CBRL;

    };
/****************************************************************************/
/*                          MODULE : EMIOS                                  */
/****************************************************************************/
    struct EMIOS_tag {
        union EMIOS_MCR_tag {
            uint32_t R;
            struct {
                uint32_t:1;
                uint32_t MDIS:1;
                uint32_t FRZ:1;
                uint32_t GTBE:1;
                uint32_t ETB:1;
                uint32_t GPREN:1;
                  uint32_t:6;
                uint32_t SRV:4;
                uint32_t GPRE:8;
                  uint32_t:8;
            } B;
        } MCR;                  /* Module Configuration Register */

        union {
            uint32_t R;
            struct {
                uint32_t:8;
                uint32_t F23:1;
                uint32_t F22:1;
                uint32_t F21:1;
                uint32_t F20:1;
                uint32_t F19:1;
                uint32_t F18:1;
                uint32_t F17:1;
                uint32_t F16:1;
                uint32_t F15:1;
                uint32_t F14:1;
                uint32_t F13:1;
                uint32_t F12:1;
                uint32_t F11:1;
                uint32_t F10:1;
                uint32_t F9:1;
                uint32_t F8:1;
                uint32_t F7:1;
                uint32_t F6:1;
                uint32_t F5:1;
                uint32_t F4:1;
                uint32_t F3:1;
                uint32_t F2:1;
                uint32_t F1:1;
                uint32_t F0:1;
            } B;
        } GFR;                  /* Global FLAG Register */

        union {
            uint32_t R;
            struct {
                uint32_t:8;
                uint32_t OU23:1;
                uint32_t OU22:1;
                uint32_t OU21:1;
                uint32_t OU20:1;
                uint32_t OU19:1;
                uint32_t OU18:1;
                uint32_t OU17:1;
                uint32_t OU16:1;
                uint32_t OU15:1;
                uint32_t OU14:1;
                uint32_t OU13:1;
                uint32_t OU12:1;
                uint32_t OU11:1;
                uint32_t OU10:1;
                uint32_t OU9:1;
                uint32_t OU8:1;
                uint32_t OU7:1;
                uint32_t OU6:1;
                uint32_t OU5:1;
                uint32_t OU4:1;
                uint32_t OU3:1;
                uint32_t OU2:1;
                uint32_t OU1:1;
                uint32_t OU0:1;
            } B;
        } OUDR;                 /* Output Update Disable Register */

        uint32_t emios_reserved[5];

        struct EMIOS_CH_tag {
            union {
                uint32_t R;    /* Channel A Data Register */
            } CADR;

            union {
                uint32_t R;    /* Channel B Data Register */
            } CBDR;

            union {
                uint32_t R;    /* Channel Counter Register */
            } CCNTR;

            union EMIOS_CCR_tag {
                uint32_t R;
                struct {
                    uint32_t FREN:1;
                    uint32_t ODIS:1;
                    uint32_t ODISSL:2;
                    uint32_t UCPRE:2;
                    uint32_t UCPREN:1;
                    uint32_t DMA:1;
                      uint32_t:1;
                    uint32_t IF:4;
                    uint32_t FCK:1;
                    uint32_t FEN:1;
                      uint32_t:3;
                    uint32_t FORCMA:1;
                    uint32_t FORCMB:1;
                      uint32_t:1;
                    uint32_t BSL:2;
                    uint32_t EDSEL:1;
                    uint32_t EDPOL:1;
                    uint32_t MODE:7;
                } B;
            } CCR;              /* Channel Control Register */

            union EMIOS_CSR_tag {
                uint32_t R;
                struct {
                    uint32_t OVR:1;
                      uint32_t:15;
                    uint32_t OVFL:1;
                      uint32_t:12;
                    uint32_t UCIN:1;
                    uint32_t UCOUT:1;
                    uint32_t FLAG:1;
                } B;
            } CSR;              /* Channel Status Register */
            uint32_t emios_channel_reserved[3];

        } CH[24];

    };
/****************************************************************************/
/*                              MODULE :ETPU                                */
/****************************************************************************/

/***************************Configuration Registers**************************/

    struct ETPU_tag {
        union {                 /* MODULE CONFIGURATION REGISTER */
            uint32_t R;
            struct {
                uint32_t GEC:1;        /* Global Exception Clear */
                  uint32_t:3;
                uint32_t MGE1:1;       /* Microcode Global Exception-ETPU_A */

                uint32_t MGE2:1;       /* Microcode Global Exception-ETPU_B */

                uint32_t ILF1:1;       /* Illegal Instruction Flag-ETPU_A */

                uint32_t ILF2:1;       /* Illegal Instruction Flag-ETPU_B */

                  uint32_t:3;
                uint32_t SCMSIZE:5;    /* Shared Code Memory size */
                  uint32_t:5;
                uint32_t SCMMISF:1;    /* SCM MISC Flag */
                uint32_t SCMMISEN:1;   /* SCM MISC Enable */
                  uint32_t:2;
                uint32_t VIS:1;        /* SCM Visability */
                  uint32_t:5;
                uint32_t GTBE:1;       /* Global Time Base Enable */
            } B;
        } MCR;

        union {                 /* COHERENT DUAL-PARAMETER CONTROL */
            uint32_t R;
            struct {
                uint32_t STS:1;        /* Start Status bit */
                uint32_t CTBASE:5;     /* Channel Transfer Base */
                uint32_t PBASE:10;     /* Parameter Buffer Base Address */
                uint32_t PWIDTH:1;     /* Parameter Width */
                uint32_t PARAM0:7;     /* Channel Parameter 0 */
                uint32_t WR:1;
                uint32_t PARAM1:7;     /* Channel Parameter 1 */
            } B;
        } CDCR;

        uint32_t etpu_reserved1;

        union {                 /* MISC Compare Register */
            uint32_t R;
        } MISCCMPR;

        union {                 /* SCM off-range Date Register */
            uint32_t R;
        } SCMOFFDATAR;

        union {                 /* ETPU_A Configuration Register */
            uint32_t R;
            struct {
                uint32_t FEND:1;       /* Force END */
                uint32_t MDIS:1;       /* Low power Stop */
                  uint32_t:1;
                uint32_t STF:1;        /* Stop Flag */
                  uint32_t:4;
                uint32_t HLTF:1;       /* Halt Mode Flag */
                  uint32_t:4;
                uint32_t FPSCK:3;      /* Filter Prescaler Clock Control */
                uint32_t CDFC:2;
                  uint32_t:9;
                uint32_t ETB:5;        /* Entry Table Base */
            } B;
        } ECR_A;

        union {                 /* ETPU_B Configuration Register */
            uint32_t R;
            struct {
                uint32_t FEND:1;       /* Force END */
                uint32_t MDIS:1;       /* Low power Stop */
                  uint32_t:1;
                uint32_t STF:1;        /* Stop Flag */
                  uint32_t:4;
                uint32_t HLTF:1;       /* Halt Mode Flag */
                  uint32_t:4;
                uint32_t FPSCK:3;      /* Filter Prescaler Clock Control */
                uint32_t CDFC:2;
                  uint32_t:9;
                uint32_t ETB:5;        /* Entry Table Base */
            } B;
        } ECR_B;

        uint32_t etpu_reserved4;

        union {                 /* ETPU_A Timebase Configuration Register */
            uint32_t R;
            struct {
                uint32_t TCR2CTL:3;     /* TCR2 Clock/Gate Control */
                uint32_t TCRCF:2;       /* TCRCLK Signal Filter Control */
                  uint32_t:1;
                uint32_t AM:1;  /* Angle Mode */
                  uint32_t:3;
                uint32_t TCR2P:6;       /* TCR2 Prescaler Control */
                uint32_t TCR1CTL:2;     /* TCR1 Clock/Gate Control */
                  uint32_t:6;
                uint32_t TCR1P:8;       /* TCR1 Prescaler Control */
            } B;
        } TBCR_A;

        union {                 /* ETPU_A TCR1 Visibility Register */
            uint32_t R;
        } TB1R_A;

        union {                 /* ETPU_A TCR2 Visibility Register */
            uint32_t R;
        } TB2R_A;

        union {                 /* ETPU_A STAC Configuration Register */
            uint32_t R;
            struct {
                uint32_t REN1:1;       /* Resource Enable TCR1 */
                uint32_t RSC1:1;       /* Resource Control TCR1 */
                  uint32_t:2;
                uint32_t SERVER_ID1:4;
                  uint32_t:4;
                uint32_t SRV1:4;       /* Resource Server Slot */
                uint32_t REN2:1;       /* Resource Enable TCR2 */
                uint32_t RSC2:1;       /* Resource Control TCR2 */
                  uint32_t:2;
                uint32_t SERVER_ID2:4;
                  uint32_t:4;
                uint32_t SRV2:4;       /* Resource Server Slot */
            } B;
        } REDCR_A;

        uint32_t etpu_reserved5[4];

        union {                 /* ETPU_B Timebase Configuration Register */
            uint32_t R;
            struct {
                uint32_t TCR2CTL:3;     /* TCR2 Clock/Gate Control */
                uint32_t TCRCF:2;       /* TCRCLK Signal Filter Control */
                  uint32_t:1;
                uint32_t AM:1;  /* Angle Mode */
                  uint32_t:3;
                uint32_t TCR2P:6;       /* TCR2 Prescaler Control */
                uint32_t TCR1CTL:2;     /* TCR1 Clock/Gate Control */
                  uint32_t:6;
                uint32_t TCR1P:8;       /* TCR1 Prescaler Control */
            } B;
        } TBCR_B;

        union {                 /* ETPU_B TCR1 Visibility Register */
            uint32_t R;
        } TB1R_B;

        union {                 /* ETPU_B TCR2 Visibility Register */
            uint32_t R;
        } TB2R_B;

        union {                 /* ETPU_B STAC Configuration Register */
            uint32_t R;
            struct {
                uint32_t REN1:1;       /* Resource Enable TCR1 */
                uint32_t RSC1:1;       /* Resource Control TCR1 */
                  uint32_t:2;
                uint32_t SERVER_ID1:4;
                  uint32_t:4;
                uint32_t SRV1:4;       /* Resource Server Slot */
                uint32_t REN2:1;       /* Resource Enable TCR2 */
                uint32_t RSC2:1;       /* Resource Control TCR2 */
                  uint32_t:2;
                uint32_t SERVER_ID2:4;
                  uint32_t:4;
                uint32_t SRV2:4;       /* Resource Server Slot */
            } B;
        } REDCR_B;

        uint32_t etpu_reserved7[108];

/*****************************Status and Control Registers**************************/

        union {                 /* ETPU_A Channel Interrut Status */
            uint32_t R;
            struct {
                uint32_t CIS31:1;      /* Channel 31 Interrut Status */
                uint32_t CIS30:1;      /* Channel 30 Interrut Status */
                uint32_t CIS29:1;      /* Channel 29 Interrut Status */
                uint32_t CIS28:1;      /* Channel 28 Interrut Status */
                uint32_t CIS27:1;      /* Channel 27 Interrut Status */
                uint32_t CIS26:1;      /* Channel 26 Interrut Status */
                uint32_t CIS25:1;      /* Channel 25 Interrut Status */
                uint32_t CIS24:1;      /* Channel 24 Interrut Status */
                uint32_t CIS23:1;      /* Channel 23 Interrut Status */
                uint32_t CIS22:1;      /* Channel 22 Interrut Status */
                uint32_t CIS21:1;      /* Channel 21 Interrut Status */
                uint32_t CIS20:1;      /* Channel 20 Interrut Status */
                uint32_t CIS19:1;      /* Channel 19 Interrut Status */
                uint32_t CIS18:1;      /* Channel 18 Interrut Status */
                uint32_t CIS17:1;      /* Channel 17 Interrut Status */
                uint32_t CIS16:1;      /* Channel 16 Interrut Status */
                uint32_t CIS15:1;      /* Channel 15 Interrut Status */
                uint32_t CIS14:1;      /* Channel 14 Interrut Status */
                uint32_t CIS13:1;      /* Channel 13 Interrut Status */
                uint32_t CIS12:1;      /* Channel 12 Interrut Status */
                uint32_t CIS11:1;      /* Channel 11 Interrut Status */
                uint32_t CIS10:1;      /* Channel 10 Interrut Status */
                uint32_t CIS9:1;       /* Channel 9 Interrut Status */
                uint32_t CIS8:1;       /* Channel 8 Interrut Status */
                uint32_t CIS7:1;       /* Channel 7 Interrut Status */
                uint32_t CIS6:1;       /* Channel 6 Interrut Status */
                uint32_t CIS5:1;       /* Channel 5 Interrut Status */
                uint32_t CIS4:1;       /* Channel 4 Interrut Status */
                uint32_t CIS3:1;       /* Channel 3 Interrut Status */
                uint32_t CIS2:1;       /* Channel 2 Interrut Status */
                uint32_t CIS1:1;       /* Channel 1 Interrut Status */
                uint32_t CIS0:1;       /* Channel 0 Interrut Status */
            } B;
        } CISR_A;

        union {                 /* ETPU_B Channel Interruput Status */
            uint32_t R;
            struct {
                uint32_t CIS31:1;      /* Channel 31 Interrut Status */
                uint32_t CIS30:1;      /* Channel 30 Interrut Status */
                uint32_t CIS29:1;      /* Channel 29 Interrut Status */
                uint32_t CIS28:1;      /* Channel 28 Interrut Status */
                uint32_t CIS27:1;      /* Channel 27 Interrut Status */
                uint32_t CIS26:1;      /* Channel 26 Interrut Status */
                uint32_t CIS25:1;      /* Channel 25 Interrut Status */
                uint32_t CIS24:1;      /* Channel 24 Interrut Status */
                uint32_t CIS23:1;      /* Channel 23 Interrut Status */
                uint32_t CIS22:1;      /* Channel 22 Interrut Status */
                uint32_t CIS21:1;      /* Channel 21 Interrut Status */
                uint32_t CIS20:1;      /* Channel 20 Interrut Status */
                uint32_t CIS19:1;      /* Channel 19 Interrut Status */
                uint32_t CIS18:1;      /* Channel 18 Interrut Status */
                uint32_t CIS17:1;      /* Channel 17 Interrut Status */
                uint32_t CIS16:1;      /* Channel 16 Interrut Status */
                uint32_t CIS15:1;      /* Channel 15 Interrut Status */
                uint32_t CIS14:1;      /* Channel 14 Interrut Status */
                uint32_t CIS13:1;      /* Channel 13 Interrut Status */
                uint32_t CIS12:1;      /* Channel 12 Interrut Status */
                uint32_t CIS11:1;      /* Channel 11 Interrut Status */
                uint32_t CIS10:1;      /* Channel 10 Interrut Status */
                uint32_t CIS9:1;       /* Channel 9 Interrut Status */
                uint32_t CIS8:1;       /* Channel 8 Interrut Status */
                uint32_t CIS7:1;       /* Channel 7 Interrut Status */
                uint32_t CIS6:1;       /* Channel 6 Interrut Status */
                uint32_t CIS5:1;       /* Channel 5 Interrut Status */
                uint32_t CIS4:1;       /* Channel 4 Interrut Status */
                uint32_t CIS3:1;       /* Channel 3 Interrut Status */
                uint32_t CIS2:1;       /* Channel 2 Interrut Status */
                uint32_t CIS1:1;       /* Channel 1 Interrupt Status */
                uint32_t CIS0:1;       /* Channel 0 Interrupt Status */
            } B;
        } CISR_B;

        uint32_t etpu_reserved9[2];

        union {                 /* ETPU_A Data Transfer Request Status */
            uint32_t R;
            struct {
                uint32_t DTRS31:1;     /* Channel 31 Data Transfer Request Status */
                uint32_t DTRS30:1;     /* Channel 30 Data Transfer Request Status */
                uint32_t DTRS29:1;     /* Channel 29 Data Transfer Request Status */
                uint32_t DTRS28:1;     /* Channel 28 Data Transfer Request Status */
                uint32_t DTRS27:1;     /* Channel 27 Data Transfer Request Status */
                uint32_t DTRS26:1;     /* Channel 26 Data Transfer Request Status */
                uint32_t DTRS25:1;     /* Channel 25 Data Transfer Request Status */
                uint32_t DTRS24:1;     /* Channel 24 Data Transfer Request Status */
                uint32_t DTRS23:1;     /* Channel 23 Data Transfer Request Status */
                uint32_t DTRS22:1;     /* Channel 22 Data Transfer Request Status */
                uint32_t DTRS21:1;     /* Channel 21 Data Transfer Request Status */
                uint32_t DTRS20:1;     /* Channel 20 Data Transfer Request Status */
                uint32_t DTRS19:1;     /* Channel 19 Data Transfer Request Status */
                uint32_t DTRS18:1;     /* Channel 18 Data Transfer Request Status */
                uint32_t DTRS17:1;     /* Channel 17 Data Transfer Request Status */
                uint32_t DTRS16:1;     /* Channel 16 Data Transfer Request Status */
                uint32_t DTRS15:1;     /* Channel 15 Data Transfer Request Status */
                uint32_t DTRS14:1;     /* Channel 14 Data Transfer Request Status */
                uint32_t DTRS13:1;     /* Channel 13 Data Transfer Request Status */
                uint32_t DTRS12:1;     /* Channel 12 Data Transfer Request Status */
                uint32_t DTRS11:1;     /* Channel 11 Data Transfer Request Status */
                uint32_t DTRS10:1;     /* Channel 10 Data Transfer Request Status */
                uint32_t DTRS9:1;      /* Channel 9 Data Transfer Request Status */
                uint32_t DTRS8:1;      /* Channel 8 Data Transfer Request Status */
                uint32_t DTRS7:1;      /* Channel 7 Data Transfer Request Status */
                uint32_t DTRS6:1;      /* Channel 6 Data Transfer Request Status */
                uint32_t DTRS5:1;      /* Channel 5 Data Transfer Request Status */
                uint32_t DTRS4:1;      /* Channel 4 Data Transfer Request Status */
                uint32_t DTRS3:1;      /* Channel 3 Data Transfer Request Status */
                uint32_t DTRS2:1;      /* Channel 2 Data Transfer Request Status */
                uint32_t DTRS1:1;      /* Channel 1 Data Transfer Request Status */
                uint32_t DTRS0:1;      /* Channel 0 Data Transfer Request Status */
            } B;
        } CDTRSR_A;

        union {                 /* ETPU_B Data Transfer Request Status */
            uint32_t R;
            struct {
                uint32_t DTRS31:1;     /* Channel 31 Data Transfer Request Status */
                uint32_t DTRS30:1;     /* Channel 30 Data Transfer Request Status */
                uint32_t DTRS29:1;     /* Channel 29 Data Transfer Request Status */
                uint32_t DTRS28:1;     /* Channel 28 Data Transfer Request Status */
                uint32_t DTRS27:1;     /* Channel 27 Data Transfer Request Status */
                uint32_t DTRS26:1;     /* Channel 26 Data Transfer Request Status */
                uint32_t DTRS25:1;     /* Channel 25 Data Transfer Request Status */
                uint32_t DTRS24:1;     /* Channel 24 Data Transfer Request Status */
                uint32_t DTRS23:1;     /* Channel 23 Data Transfer Request Status */
                uint32_t DTRS22:1;     /* Channel 22 Data Transfer Request Status */
                uint32_t DTRS21:1;     /* Channel 21 Data Transfer Request Status */
                uint32_t DTRS20:1;     /* Channel 20 Data Transfer Request Status */
                uint32_t DTRS19:1;     /* Channel 19 Data Transfer Request Status */
                uint32_t DTRS18:1;     /* Channel 18 Data Transfer Request Status */
                uint32_t DTRS17:1;     /* Channel 17 Data Transfer Request Status */
                uint32_t DTRS16:1;     /* Channel 16 Data Transfer Request Status */
                uint32_t DTRS15:1;     /* Channel 15 Data Transfer Request Status */
                uint32_t DTRS14:1;     /* Channel 14 Data Transfer Request Status */
                uint32_t DTRS13:1;     /* Channel 13 Data Transfer Request Status */
                uint32_t DTRS12:1;     /* Channel 12 Data Transfer Request Status */
                uint32_t DTRS11:1;     /* Channel 11 Data Transfer Request Status */
                uint32_t DTRS10:1;     /* Channel 10 Data Transfer Request Status */
                uint32_t DTRS9:1;      /* Channel 9 Data Transfer Request Status */
                uint32_t DTRS8:1;      /* Channel 8 Data Transfer Request Status */
                uint32_t DTRS7:1;      /* Channel 7 Data Transfer Request Status */
                uint32_t DTRS6:1;      /* Channel 6 Data Transfer Request Status */
                uint32_t DTRS5:1;      /* Channel 5 Data Transfer Request Status */
                uint32_t DTRS4:1;      /* Channel 4 Data Transfer Request Status */
                uint32_t DTRS3:1;      /* Channel 3 Data Transfer Request Status */
                uint32_t DTRS2:1;      /* Channel 2 Data Transfer Request Status */
                uint32_t DTRS1:1;      /* Channel 1 Data Transfer Request Status */
                uint32_t DTRS0:1;      /* Channel 0 Data Transfer Request Status */
            } B;
        } CDTRSR_B;

        uint32_t etpu_reserved11[2];

        union {                 /* ETPU_A Interruput Overflow Status */
            uint32_t R;
            struct {
                uint32_t CIOS31:1;     /* Channel 31 Interruput Overflow Status */
                uint32_t CIOS30:1;     /* Channel 30 Interruput Overflow Status */
                uint32_t CIOS29:1;     /* Channel 29 Interruput Overflow Status */
                uint32_t CIOS28:1;     /* Channel 28 Interruput Overflow Status */
                uint32_t CIOS27:1;     /* Channel 27 Interruput Overflow Status */
                uint32_t CIOS26:1;     /* Channel 26 Interruput Overflow Status */
                uint32_t CIOS25:1;     /* Channel 25 Interruput Overflow Status */
                uint32_t CIOS24:1;     /* Channel 24 Interruput Overflow Status */
                uint32_t CIOS23:1;     /* Channel 23 Interruput Overflow Status */
                uint32_t CIOS22:1;     /* Channel 22 Interruput Overflow Status */
                uint32_t CIOS21:1;     /* Channel 21 Interruput Overflow Status */
                uint32_t CIOS20:1;     /* Channel 20 Interruput Overflow Status */
                uint32_t CIOS19:1;     /* Channel 19 Interruput Overflow Status */
                uint32_t CIOS18:1;     /* Channel 18 Interruput Overflow Status */
                uint32_t CIOS17:1;     /* Channel 17 Interruput Overflow Status */
                uint32_t CIOS16:1;     /* Channel 16 Interruput Overflow Status */
                uint32_t CIOS15:1;     /* Channel 15 Interruput Overflow Status */
                uint32_t CIOS14:1;     /* Channel 14 Interruput Overflow Status */
                uint32_t CIOS13:1;     /* Channel 13 Interruput Overflow Status */
                uint32_t CIOS12:1;     /* Channel 12 Interruput Overflow Status */
                uint32_t CIOS11:1;     /* Channel 11 Interruput Overflow Status */
                uint32_t CIOS10:1;     /* Channel 10 Interruput Overflow Status */
                uint32_t CIOS9:1;      /* Channel 9 Interruput Overflow Status */
                uint32_t CIOS8:1;      /* Channel 8 Interruput Overflow Status */
                uint32_t CIOS7:1;      /* Channel 7 Interruput Overflow Status */
                uint32_t CIOS6:1;      /* Channel 6 Interruput Overflow Status */
                uint32_t CIOS5:1;      /* Channel 5 Interruput Overflow Status */
                uint32_t CIOS4:1;      /* Channel 4 Interruput Overflow Status */
                uint32_t CIOS3:1;      /* Channel 3 Interruput Overflow Status */
                uint32_t CIOS2:1;      /* Channel 2 Interruput Overflow Status */
                uint32_t CIOS1:1;      /* Channel 1 Interruput Overflow Status */
                uint32_t CIOS0:1;      /* Channel 0 Interruput Overflow Status */
            } B;
        } CIOSR_A;

        union {                 /* ETPU_B Interruput Overflow Status */
            uint32_t R;
            struct {
                uint32_t CIOS31:1;     /* Channel 31 Interruput Overflow Status */
                uint32_t CIOS30:1;     /* Channel 30 Interruput Overflow Status */
                uint32_t CIOS29:1;     /* Channel 29 Interruput Overflow Status */
                uint32_t CIOS28:1;     /* Channel 28 Interruput Overflow Status */
                uint32_t CIOS27:1;     /* Channel 27 Interruput Overflow Status */
                uint32_t CIOS26:1;     /* Channel 26 Interruput Overflow Status */
                uint32_t CIOS25:1;     /* Channel 25 Interruput Overflow Status */
                uint32_t CIOS24:1;     /* Channel 24 Interruput Overflow Status */
                uint32_t CIOS23:1;     /* Channel 23 Interruput Overflow Status */
                uint32_t CIOS22:1;     /* Channel 22 Interruput Overflow Status */
                uint32_t CIOS21:1;     /* Channel 21 Interruput Overflow Status */
                uint32_t CIOS20:1;     /* Channel 20 Interruput Overflow Status */
                uint32_t CIOS19:1;     /* Channel 19 Interruput Overflow Status */
                uint32_t CIOS18:1;     /* Channel 18 Interruput Overflow Status */
                uint32_t CIOS17:1;     /* Channel 17 Interruput Overflow Status */
                uint32_t CIOS16:1;     /* Channel 16 Interruput Overflow Status */
                uint32_t CIOS15:1;     /* Channel 15 Interruput Overflow Status */
                uint32_t CIOS14:1;     /* Channel 14 Interruput Overflow Status */
                uint32_t CIOS13:1;     /* Channel 13 Interruput Overflow Status */
                uint32_t CIOS12:1;     /* Channel 12 Interruput Overflow Status */
                uint32_t CIOS11:1;     /* Channel 11 Interruput Overflow Status */
                uint32_t CIOS10:1;     /* Channel 10 Interruput Overflow Status */
                uint32_t CIOS9:1;      /* Channel 9 Interruput Overflow Status */
                uint32_t CIOS8:1;      /* Channel 8 Interruput Overflow Status */
                uint32_t CIOS7:1;      /* Channel 7 Interruput Overflow Status */
                uint32_t CIOS6:1;      /* Channel 6 Interruput Overflow Status */
                uint32_t CIOS5:1;      /* Channel 5 Interruput Overflow Status */
                uint32_t CIOS4:1;      /* Channel 4 Interruput Overflow Status */
                uint32_t CIOS3:1;      /* Channel 3 Interruput Overflow Status */
                uint32_t CIOS2:1;      /* Channel 2 Interruput Overflow Status */
                uint32_t CIOS1:1;      /* Channel 1 Interruput Overflow Status */
                uint32_t CIOS0:1;      /* Channel 0 Interruput Overflow Status */
            } B;
        } CIOSR_B;

        uint32_t etpu_reserved13[2];

        union {                 /* ETPU_A Data Transfer Overflow Status */
            uint32_t R;
            struct {
                uint32_t DTROS31:1;    /* Channel 31 Data Transfer Overflow Status */
                uint32_t DTROS30:1;    /* Channel 30 Data Transfer Overflow Status */
                uint32_t DTROS29:1;    /* Channel 29 Data Transfer Overflow Status */
                uint32_t DTROS28:1;    /* Channel 28 Data Transfer Overflow Status */
                uint32_t DTROS27:1;    /* Channel 27 Data Transfer Overflow Status */
                uint32_t DTROS26:1;    /* Channel 26 Data Transfer Overflow Status */
                uint32_t DTROS25:1;    /* Channel 25 Data Transfer Overflow Status */
                uint32_t DTROS24:1;    /* Channel 24 Data Transfer Overflow Status */
                uint32_t DTROS23:1;    /* Channel 23 Data Transfer Overflow Status */
                uint32_t DTROS22:1;    /* Channel 22 Data Transfer Overflow Status */
                uint32_t DTROS21:1;    /* Channel 21 Data Transfer Overflow Status */
                uint32_t DTROS20:1;    /* Channel 20 Data Transfer Overflow Status */
                uint32_t DTROS19:1;    /* Channel 19 Data Transfer Overflow Status */
                uint32_t DTROS18:1;    /* Channel 18 Data Transfer Overflow Status */
                uint32_t DTROS17:1;    /* Channel 17 Data Transfer Overflow Status */
                uint32_t DTROS16:1;    /* Channel 16 Data Transfer Overflow Status */
                uint32_t DTROS15:1;    /* Channel 15 Data Transfer Overflow Status */
                uint32_t DTROS14:1;    /* Channel 14 Data Transfer Overflow Status */
                uint32_t DTROS13:1;    /* Channel 13 Data Transfer Overflow Status */
                uint32_t DTROS12:1;    /* Channel 12 Data Transfer Overflow Status */
                uint32_t DTROS11:1;    /* Channel 11 Data Transfer Overflow Status */
                uint32_t DTROS10:1;    /* Channel 10 Data Transfer Overflow Status */
                uint32_t DTROS9:1;     /* Channel 9 Data Transfer Overflow Status */
                uint32_t DTROS8:1;     /* Channel 8 Data Transfer Overflow Status */
                uint32_t DTROS7:1;     /* Channel 7 Data Transfer Overflow Status */
                uint32_t DTROS6:1;     /* Channel 6 Data Transfer Overflow Status */
                uint32_t DTROS5:1;     /* Channel 5 Data Transfer Overflow Status */
                uint32_t DTROS4:1;     /* Channel 4 Data Transfer Overflow Status */
                uint32_t DTROS3:1;     /* Channel 3 Data Transfer Overflow Status */
                uint32_t DTROS2:1;     /* Channel 2 Data Transfer Overflow Status */
                uint32_t DTROS1:1;     /* Channel 1 Data Transfer Overflow Status */
                uint32_t DTROS0:1;     /* Channel 0 Data Transfer Overflow Status */
            } B;
        } CDTROSR_A;

        union {                 /* ETPU_B Data Transfer Overflow Status */
            uint32_t R;
            struct {
                uint32_t DTROS31:1;    /* Channel 31 Data Transfer Overflow Status */
                uint32_t DTROS30:1;    /* Channel 30 Data Transfer Overflow Status */
                uint32_t DTROS29:1;    /* Channel 29 Data Transfer Overflow Status */
                uint32_t DTROS28:1;    /* Channel 28 Data Transfer Overflow Status */
                uint32_t DTROS27:1;    /* Channel 27 Data Transfer Overflow Status */
                uint32_t DTROS26:1;    /* Channel 26 Data Transfer Overflow Status */
                uint32_t DTROS25:1;    /* Channel 25 Data Transfer Overflow Status */
                uint32_t DTROS24:1;    /* Channel 24 Data Transfer Overflow Status */
                uint32_t DTROS23:1;    /* Channel 23 Data Transfer Overflow Status */
                uint32_t DTROS22:1;    /* Channel 22 Data Transfer Overflow Status */
                uint32_t DTROS21:1;    /* Channel 21 Data Transfer Overflow Status */
                uint32_t DTROS20:1;    /* Channel 20 Data Transfer Overflow Status */
                uint32_t DTROS19:1;    /* Channel 19 Data Transfer Overflow Status */
                uint32_t DTROS18:1;    /* Channel 18 Data Transfer Overflow Status */
                uint32_t DTROS17:1;    /* Channel 17 Data Transfer Overflow Status */
                uint32_t DTROS16:1;    /* Channel 16 Data Transfer Overflow Status */
                uint32_t DTROS15:1;    /* Channel 15 Data Transfer Overflow Status */
                uint32_t DTROS14:1;    /* Channel 14 Data Transfer Overflow Status */
                uint32_t DTROS13:1;    /* Channel 13 Data Transfer Overflow Status */
                uint32_t DTROS12:1;    /* Channel 12 Data Transfer Overflow Status */
                uint32_t DTROS11:1;    /* Channel 11 Data Transfer Overflow Status */
                uint32_t DTROS10:1;    /* Channel 10 Data Transfer Overflow Status */
                uint32_t DTROS9:1;     /* Channel 9 Data Transfer Overflow Status */
                uint32_t DTROS8:1;     /* Channel 8 Data Transfer Overflow Status */
                uint32_t DTROS7:1;     /* Channel 7 Data Transfer Overflow Status */
                uint32_t DTROS6:1;     /* Channel 6 Data Transfer Overflow Status */
                uint32_t DTROS5:1;     /* Channel 5 Data Transfer Overflow Status */
                uint32_t DTROS4:1;     /* Channel 4 Data Transfer Overflow Status */
                uint32_t DTROS3:1;     /* Channel 3 Data Transfer Overflow Status */
                uint32_t DTROS2:1;     /* Channel 2 Data Transfer Overflow Status */
                uint32_t DTROS1:1;     /* Channel 1 Data Transfer Overflow Status */
                uint32_t DTROS0:1;     /* Channel 0 Data Transfer Overflow Status */
            } B;
        } CDTROSR_B;

        uint32_t etpu_reserved15[2];

        union {                 /* ETPU_A Channel Interruput Enable */
            uint32_t R;
            struct {
                uint32_t CIE31:1;      /* Channel 31 Interruput Enable */
                uint32_t CIE30:1;      /* Channel 30 Interruput Enable */
                uint32_t CIE29:1;      /* Channel 29 Interruput Enable */
                uint32_t CIE28:1;      /* Channel 28 Interruput Enable */
                uint32_t CIE27:1;      /* Channel 27 Interruput Enable */
                uint32_t CIE26:1;      /* Channel 26 Interruput Enable */
                uint32_t CIE25:1;      /* Channel 25 Interruput Enable */
                uint32_t CIE24:1;      /* Channel 24 Interruput Enable */
                uint32_t CIE23:1;      /* Channel 23 Interruput Enable */
                uint32_t CIE22:1;      /* Channel 22 Interruput Enable */
                uint32_t CIE21:1;      /* Channel 21 Interruput Enable */
                uint32_t CIE20:1;      /* Channel 20 Interruput Enable */
                uint32_t CIE19:1;      /* Channel 19 Interruput Enable */
                uint32_t CIE18:1;      /* Channel 18 Interruput Enable */
                uint32_t CIE17:1;      /* Channel 17 Interruput Enable */
                uint32_t CIE16:1;      /* Channel 16 Interruput Enable */
                uint32_t CIE15:1;      /* Channel 15 Interruput Enable */
                uint32_t CIE14:1;      /* Channel 14 Interruput Enable */
                uint32_t CIE13:1;      /* Channel 13 Interruput Enable */
                uint32_t CIE12:1;      /* Channel 12 Interruput Enable */
                uint32_t CIE11:1;      /* Channel 11 Interruput Enable */
                uint32_t CIE10:1;      /* Channel 10 Interruput Enable */
                uint32_t CIE9:1;       /* Channel 9 Interruput Enable */
                uint32_t CIE8:1;       /* Channel 8 Interruput Enable */
                uint32_t CIE7:1;       /* Channel 7 Interruput Enable */
                uint32_t CIE6:1;       /* Channel 6 Interruput Enable */
                uint32_t CIE5:1;       /* Channel 5 Interruput Enable */
                uint32_t CIE4:1;       /* Channel 4 Interruput Enable */
                uint32_t CIE3:1;       /* Channel 3 Interruput Enable */
                uint32_t CIE2:1;       /* Channel 2 Interruput Enable */
                uint32_t CIE1:1;       /* Channel 1 Interruput Enable */
                uint32_t CIE0:1;       /* Channel 0 Interruput Enable */
            } B;
        } CIER_A;

        union {                 /* ETPU_B Channel Interruput Enable */
            uint32_t R;
            struct {
                uint32_t CIE31:1;      /* Channel 31 Interruput Enable */
                uint32_t CIE30:1;      /* Channel 30 Interruput Enable */
                uint32_t CIE29:1;      /* Channel 29 Interruput Enable */
                uint32_t CIE28:1;      /* Channel 28 Interruput Enable */
                uint32_t CIE27:1;      /* Channel 27 Interruput Enable */
                uint32_t CIE26:1;      /* Channel 26 Interruput Enable */
                uint32_t CIE25:1;      /* Channel 25 Interruput Enable */
                uint32_t CIE24:1;      /* Channel 24 Interruput Enable */
                uint32_t CIE23:1;      /* Channel 23 Interruput Enable */
                uint32_t CIE22:1;      /* Channel 22 Interruput Enable */
                uint32_t CIE21:1;      /* Channel 21 Interruput Enable */
                uint32_t CIE20:1;      /* Channel 20 Interruput Enable */
                uint32_t CIE19:1;      /* Channel 19 Interruput Enable */
                uint32_t CIE18:1;      /* Channel 18 Interruput Enable */
                uint32_t CIE17:1;      /* Channel 17 Interruput Enable */
                uint32_t CIE16:1;      /* Channel 16 Interruput Enable */
                uint32_t CIE15:1;      /* Channel 15 Interruput Enable */
                uint32_t CIE14:1;      /* Channel 14 Interruput Enable */
                uint32_t CIE13:1;      /* Channel 13 Interruput Enable */
                uint32_t CIE12:1;      /* Channel 12 Interruput Enable */
                uint32_t CIE11:1;      /* Channel 11 Interruput Enable */
                uint32_t CIE10:1;      /* Channel 10 Interruput Enable */
                uint32_t CIE9:1;       /* Channel 9 Interruput Enable */
                uint32_t CIE8:1;       /* Channel 8 Interruput Enable */
                uint32_t CIE7:1;       /* Channel 7 Interruput Enable */
                uint32_t CIE6:1;       /* Channel 6 Interruput Enable */
                uint32_t CIE5:1;       /* Channel 5 Interruput Enable */
                uint32_t CIE4:1;       /* Channel 4 Interruput Enable */
                uint32_t CIE3:1;       /* Channel 3 Interruput Enable */
                uint32_t CIE2:1;       /* Channel 2 Interruput Enable */
                uint32_t CIE1:1;       /* Channel 1 Interruput Enable */
                uint32_t CIE0:1;       /* Channel 0 Interruput Enable */
            } B;
        } CIER_B;

        uint32_t etpu_reserved17[2];

        union {                 /* ETPU_A Channel Data Transfer Request Enable */
            uint32_t R;
            struct {
                uint32_t DTRE31:1;     /* Channel 31 Data Transfer Request Enable */
                uint32_t DTRE30:1;     /* Channel 30 Data Transfer Request Enable */
                uint32_t DTRE29:1;     /* Channel 29 Data Transfer Request Enable */
                uint32_t DTRE28:1;     /* Channel 28 Data Transfer Request Enable */
                uint32_t DTRE27:1;     /* Channel 27 Data Transfer Request Enable */
                uint32_t DTRE26:1;     /* Channel 26 Data Transfer Request Enable */
                uint32_t DTRE25:1;     /* Channel 25 Data Transfer Request Enable */
                uint32_t DTRE24:1;     /* Channel 24 Data Transfer Request Enable */
                uint32_t DTRE23:1;     /* Channel 23 Data Transfer Request Enable */
                uint32_t DTRE22:1;     /* Channel 22 Data Transfer Request Enable */
                uint32_t DTRE21:1;     /* Channel 21 Data Transfer Request Enable */
                uint32_t DTRE20:1;     /* Channel 20 Data Transfer Request Enable */
                uint32_t DTRE19:1;     /* Channel 19 Data Transfer Request Enable */
                uint32_t DTRE18:1;     /* Channel 18 Data Transfer Request Enable */
                uint32_t DTRE17:1;     /* Channel 17 Data Transfer Request Enable */
                uint32_t DTRE16:1;     /* Channel 16 Data Transfer Request Enable */
                uint32_t DTRE15:1;     /* Channel 15 Data Transfer Request Enable */
                uint32_t DTRE14:1;     /* Channel 14 Data Transfer Request Enable */
                uint32_t DTRE13:1;     /* Channel 13 Data Transfer Request Enable */
                uint32_t DTRE12:1;     /* Channel 12 Data Transfer Request Enable */
                uint32_t DTRE11:1;     /* Channel 11 Data Transfer Request Enable */
                uint32_t DTRE10:1;     /* Channel 10 Data Transfer Request Enable */
                uint32_t DTRE9:1;      /* Channel 9 Data Transfer Request Enable */
                uint32_t DTRE8:1;      /* Channel 8 Data Transfer Request Enable */
                uint32_t DTRE7:1;      /* Channel 7 Data Transfer Request Enable */
                uint32_t DTRE6:1;      /* Channel 6 Data Transfer Request Enable */
                uint32_t DTRE5:1;      /* Channel 5 Data Transfer Request Enable */
                uint32_t DTRE4:1;      /* Channel 4 Data Transfer Request Enable */
                uint32_t DTRE3:1;      /* Channel 3 Data Transfer Request Enable */
                uint32_t DTRE2:1;      /* Channel 2 Data Transfer Request Enable */
                uint32_t DTRE1:1;      /* Channel 1 Data Transfer Request Enable */
                uint32_t DTRE0:1;      /* Channel 0 Data Transfer Request Enable */
            } B;
        } CDTRER_A;

        union {                 /* ETPU_B Channel Data Transfer Request Enable */
            uint32_t R;
            struct {
                uint32_t DTRE31:1;     /* Channel 31 Data Transfer Request Enable */
                uint32_t DTRE30:1;     /* Channel 30 Data Transfer Request Enable */
                uint32_t DTRE29:1;     /* Channel 29 Data Transfer Request Enable */
                uint32_t DTRE28:1;     /* Channel 28 Data Transfer Request Enable */
                uint32_t DTRE27:1;     /* Channel 27 Data Transfer Request Enable */
                uint32_t DTRE26:1;     /* Channel 26 Data Transfer Request Enable */
                uint32_t DTRE25:1;     /* Channel 25 Data Transfer Request Enable */
                uint32_t DTRE24:1;     /* Channel 24 Data Transfer Request Enable */
                uint32_t DTRE23:1;     /* Channel 23 Data Transfer Request Enable */
                uint32_t DTRE22:1;     /* Channel 22 Data Transfer Request Enable */
                uint32_t DTRE21:1;     /* Channel 21 Data Transfer Request Enable */
                uint32_t DTRE20:1;     /* Channel 20 Data Transfer Request Enable */
                uint32_t DTRE19:1;     /* Channel 19 Data Transfer Request Enable */
                uint32_t DTRE18:1;     /* Channel 18 Data Transfer Request Enable */
                uint32_t DTRE17:1;     /* Channel 17 Data Transfer Request Enable */
                uint32_t DTRE16:1;     /* Channel 16 Data Transfer Request Enable */
                uint32_t DTRE15:1;     /* Channel 15 Data Transfer Request Enable */
                uint32_t DTRE14:1;     /* Channel 14 Data Transfer Request Enable */
                uint32_t DTRE13:1;     /* Channel 13 Data Transfer Request Enable */
                uint32_t DTRE12:1;     /* Channel 12 Data Transfer Request Enable */
                uint32_t DTRE11:1;     /* Channel 11 Data Transfer Request Enable */
                uint32_t DTRE10:1;     /* Channel 10 Data Transfer Request Enable */
                uint32_t DTRE9:1;      /* Channel 9 Data Transfer Request Enable */
                uint32_t DTRE8:1;      /* Channel 8 Data Transfer Request Enable */
                uint32_t DTRE7:1;      /* Channel 7 Data Transfer Request Enable */
                uint32_t DTRE6:1;      /* Channel 6 Data Transfer Request Enable */
                uint32_t DTRE5:1;      /* Channel 5 Data Transfer Request Enable */
                uint32_t DTRE4:1;      /* Channel 4 Data Transfer Request Enable */
                uint32_t DTRE3:1;      /* Channel 3 Data Transfer Request Enable */
                uint32_t DTRE2:1;      /* Channel 2 Data Transfer Request Enable */
                uint32_t DTRE1:1;      /* Channel 1 Data Transfer Request Enable */
                uint32_t DTRE0:1;      /* Channel 0 Data Transfer Request Enable */
            } B;
        } CDTRER_B;

        uint32_t etpu_reserved20[10];
        union {                 /* ETPU_A Channel Pending Service Status */
            uint32_t R;
            struct {
                uint32_t SR31:1;       /* Channel 31 Pending Service Status */
                uint32_t SR30:1;       /* Channel 30 Pending Service Status */
                uint32_t SR29:1;       /* Channel 29 Pending Service Status */
                uint32_t SR28:1;       /* Channel 28 Pending Service Status */
                uint32_t SR27:1;       /* Channel 27 Pending Service Status */
                uint32_t SR26:1;       /* Channel 26 Pending Service Status */
                uint32_t SR25:1;       /* Channel 25 Pending Service Status */
                uint32_t SR24:1;       /* Channel 24 Pending Service Status */
                uint32_t SR23:1;       /* Channel 23 Pending Service Status */
                uint32_t SR22:1;       /* Channel 22 Pending Service Status */
                uint32_t SR21:1;       /* Channel 21 Pending Service Status */
                uint32_t SR20:1;       /* Channel 20 Pending Service Status */
                uint32_t SR19:1;       /* Channel 19 Pending Service Status */
                uint32_t SR18:1;       /* Channel 18 Pending Service Status */
                uint32_t SR17:1;       /* Channel 17 Pending Service Status */
                uint32_t SR16:1;       /* Channel 16 Pending Service Status */
                uint32_t SR15:1;       /* Channel 15 Pending Service Status */
                uint32_t SR14:1;       /* Channel 14 Pending Service Status */
                uint32_t SR13:1;       /* Channel 13 Pending Service Status */
                uint32_t SR12:1;       /* Channel 12 Pending Service Status */
                uint32_t SR11:1;       /* Channel 11 Pending Service Status */
                uint32_t SR10:1;       /* Channel 10 Pending Service Status */
                uint32_t SR9:1;        /* Channel 9 Pending Service Status */
                uint32_t SR8:1;        /* Channel 8 Pending Service Status */
                uint32_t SR7:1;        /* Channel 7 Pending Service Status */
                uint32_t SR6:1;        /* Channel 6 Pending Service Status */
                uint32_t SR5:1;        /* Channel 5 Pending Service Status */
                uint32_t SR4:1;        /* Channel 4 Pending Service Status */
                uint32_t SR3:1;        /* Channel 3 Pending Service Status */
                uint32_t SR2:1;        /* Channel 2 Pending Service Status */
                uint32_t SR1:1;        /* Channel 1 Pending Service Status */
                uint32_t SR0:1;        /* Channel 0 Pending Service Status */
            } B;
        } CPSSR_A;

        union {                 /* ETPU_B Channel Pending Service Status */
            uint32_t R;
            struct {
                uint32_t SR31:1;       /* Channel 31 Pending Service Status */
                uint32_t SR30:1;       /* Channel 30 Pending Service Status */
                uint32_t SR29:1;       /* Channel 29 Pending Service Status */
                uint32_t SR28:1;       /* Channel 28 Pending Service Status */
                uint32_t SR27:1;       /* Channel 27 Pending Service Status */
                uint32_t SR26:1;       /* Channel 26 Pending Service Status */
                uint32_t SR25:1;       /* Channel 25 Pending Service Status */
                uint32_t SR24:1;       /* Channel 24 Pending Service Status */
                uint32_t SR23:1;       /* Channel 23 Pending Service Status */
                uint32_t SR22:1;       /* Channel 22 Pending Service Status */
                uint32_t SR21:1;       /* Channel 21 Pending Service Status */
                uint32_t SR20:1;       /* Channel 20 Pending Service Status */
                uint32_t SR19:1;       /* Channel 19 Pending Service Status */
                uint32_t SR18:1;       /* Channel 18 Pending Service Status */
                uint32_t SR17:1;       /* Channel 17 Pending Service Status */
                uint32_t SR16:1;       /* Channel 16 Pending Service Status */
                uint32_t SR15:1;       /* Channel 15 Pending Service Status */
                uint32_t SR14:1;       /* Channel 14 Pending Service Status */
                uint32_t SR13:1;       /* Channel 13 Pending Service Status */
                uint32_t SR12:1;       /* Channel 12 Pending Service Status */
                uint32_t SR11:1;       /* Channel 11 Pending Service Status */
                uint32_t SR10:1;       /* Channel 10 Pending Service Status */
                uint32_t SR9:1;        /* Channel 9 Pending Service Status */
                uint32_t SR8:1;        /* Channel 8 Pending Service Status */
                uint32_t SR7:1;        /* Channel 7 Pending Service Status */
                uint32_t SR6:1;        /* Channel 6 Pending Service Status */
                uint32_t SR5:1;        /* Channel 5 Pending Service Status */
                uint32_t SR4:1;        /* Channel 4 Pending Service Status */
                uint32_t SR3:1;        /* Channel 3 Pending Service Status */
                uint32_t SR2:1;        /* Channel 2 Pending Service Status */
                uint32_t SR1:1;        /* Channel 1 Pending Service Status */
                uint32_t SR0:1;        /* Channel 0 Pending Service Status */
            } B;
        } CPSSR_B;

        uint32_t etpu_reserved20a[2];

        union {                 /* ETPU_A Channel Service Status */
            uint32_t R;
            struct {
                uint32_t SS31:1;       /* Channel 31 Service Status */
                uint32_t SS30:1;       /* Channel 30 Service Status */
                uint32_t SS29:1;       /* Channel 29 Service Status */
                uint32_t SS28:1;       /* Channel 28 Service Status */
                uint32_t SS27:1;       /* Channel 27 Service Status */
                uint32_t SS26:1;       /* Channel 26 Service Status */
                uint32_t SS25:1;       /* Channel 25 Service Status */
                uint32_t SS24:1;       /* Channel 24 Service Status */
                uint32_t SS23:1;       /* Channel 23 Service Status */
                uint32_t SS22:1;       /* Channel 22 Service Status */
                uint32_t SS21:1;       /* Channel 21 Service Status */
                uint32_t SS20:1;       /* Channel 20 Service Status */
                uint32_t SS19:1;       /* Channel 19 Service Status */
                uint32_t SS18:1;       /* Channel 18 Service Status */
                uint32_t SS17:1;       /* Channel 17 Service Status */
                uint32_t SS16:1;       /* Channel 16 Service Status */
                uint32_t SS15:1;       /* Channel 15 Service Status */
                uint32_t SS14:1;       /* Channel 14 Service Status */
                uint32_t SS13:1;       /* Channel 13 Service Status */
                uint32_t SS12:1;       /* Channel 12 Service Status */
                uint32_t SS11:1;       /* Channel 11 Service Status */
                uint32_t SS10:1;       /* Channel 10 Service Status */
                uint32_t SS9:1;        /* Channel 9 Service Status */
                uint32_t SS8:1;        /* Channel 8 Service Status */
                uint32_t SS7:1;        /* Channel 7 Service Status */
                uint32_t SS6:1;        /* Channel 6 Service Status */
                uint32_t SS5:1;        /* Channel 5 Service Status */
                uint32_t SS4:1;        /* Channel 4 Service Status */
                uint32_t SS3:1;        /* Channel 3 Service Status */
                uint32_t SS2:1;        /* Channel 2 Service Status */
                uint32_t SS1:1;        /* Channel 1 Service Status */
                uint32_t SS0:1;        /* Channel 0 Service Status */
            } B;
        } CSSR_A;

        union {                 /* ETPU_B Channel Service Status */
            uint32_t R;
            struct {
                uint32_t SS31:1;       /* Channel 31 Service Status */
                uint32_t SS30:1;       /* Channel 30 Service Status */
                uint32_t SS29:1;       /* Channel 29 Service Status */
                uint32_t SS28:1;       /* Channel 28 Service Status */
                uint32_t SS27:1;       /* Channel 27 Service Status */
                uint32_t SS26:1;       /* Channel 26 Service Status */
                uint32_t SS25:1;       /* Channel 25 Service Status */
                uint32_t SS24:1;       /* Channel 24 Service Status */
                uint32_t SS23:1;       /* Channel 23 Service Status */
                uint32_t SS22:1;       /* Channel 22 Service Status */
                uint32_t SS21:1;       /* Channel 21 Service Status */
                uint32_t SS20:1;       /* Channel 20 Service Status */
                uint32_t SS19:1;       /* Channel 19 Service Status */
                uint32_t SS18:1;       /* Channel 18 Service Status */
                uint32_t SS17:1;       /* Channel 17 Service Status */
                uint32_t SS16:1;       /* Channel 16 Service Status */
                uint32_t SS15:1;       /* Channel 15 Service Status */
                uint32_t SS14:1;       /* Channel 14 Service Status */
                uint32_t SS13:1;       /* Channel 13 Service Status */
                uint32_t SS12:1;       /* Channel 12 Service Status */
                uint32_t SS11:1;       /* Channel 11 Service Status */
                uint32_t SS10:1;       /* Channel 10 Service Status */
                uint32_t SS9:1;        /* Channel 9 Service Status */
                uint32_t SS8:1;        /* Channel 8 Service Status */
                uint32_t SS7:1;        /* Channel 7 Service Status */
                uint32_t SS6:1;        /* Channel 6 Service Status */
                uint32_t SS5:1;        /* Channel 5 Service Status */
                uint32_t SS4:1;        /* Channel 4 Service Status */
                uint32_t SS3:1;        /* Channel 3 Service Status */
                uint32_t SS2:1;        /* Channel 2 Service Status */
                uint32_t SS1:1;        /* Channel 1 Service Status */
                uint32_t SS0:1;        /* Channel 0 Service Status */
            } B;
        } CSSR_B;

        uint32_t etpu_reserved23[90];

/*****************************Channels********************************/

        struct {
            union {
                uint32_t R;    /* Channel Configuration Register */
                struct {
                    uint32_t CIE:1;    /* Channel Interruput Enable */
                    uint32_t DTRE:1;   /* Data Transfer Request Enable */
                    uint32_t CPR:2;    /* Channel Priority */
                      uint32_t:3;
                    uint32_t ETCS:1;   /* Entry Table Condition Select */
                      uint32_t:3;
                    uint32_t CFS:5;    /* Channel Function Select */
                    uint32_t ODIS:1;   /* Output disable */
                    uint32_t OPOL:1;   /* output polarity */
                      uint32_t:3;
                    uint32_t CPBA:11;  /* Channel Parameter Base Address */
                } B;
            } CR;
            union {
                uint32_t R;    /* Channel Status Control Register */
                struct {
                    uint32_t CIS:1;    /* Channel Interruput Status */
                    uint32_t CIOS:1;   /* Channel Interruput Overflow Status */
                      uint32_t:6;
                    uint32_t DTRS:1;   /* Data Transfer Status */
                    uint32_t DTROS:1;  /* Data Transfer Overflow Status */
                      uint32_t:6;
                    uint32_t IPS:1;    /* Input Pin State */
                    uint32_t OPS:1;    /* Output Pin State */
                    uint32_t OBE:1;    /* Output Buffer Enable */
                      uint32_t:11;
                    uint32_t FM1:1;    /* Function mode */
                    uint32_t FM0:1;    /* Function mode */
                } B;
            } SCR;
            union {
                uint32_t R;    /* Channel Host Service Request Register */
                struct {
                    uint32_t:29;       /* Host Service Request */
                    uint32_t HSR:3;
                } B;
            } HSRR;
            uint32_t etpu_reserved23;
        } CHAN[127];

    };
/****************************************************************************/
/*                          MODULE : XBAR CrossBar                          */
/****************************************************************************/
    struct XBAR_tag {
        union {
            uint32_t R;
            struct {
                uint32_t:4;

                uint32_t:4;

                uint32_t:4;

                uint32_t:4;

                uint32_t:4;

                uint32_t:1;
                uint32_t MSTR2:3;
                  uint32_t:1;
                uint32_t MSTR1:3;
                  uint32_t:1;
                uint32_t MSTR0:3;
            } B;
        } MPR0;                 /* Master Priority Register for Slave Port 0 */

        uint32_t xbar_reserved1[3];

        union {
            uint32_t R;
            struct {
                uint32_t RO:1;
                  uint32_t:21;
                uint32_t ARB:2;
                  uint32_t:2;
                uint32_t PCTL:2;
                  uint32_t:1;
                uint32_t PARK:3;
            } B;
        } SGPCR0;               /* General Purpose Control Register for Slave Port 0 */

        uint32_t xbar_reserved2[59];

        union {
            uint32_t R;
            struct {
                uint32_t:4;

                uint32_t:4;

                uint32_t:4;

                uint32_t:4;

                uint32_t:4;

                uint32_t:1;
                uint32_t MSTR2:3;
                  uint32_t:1;
                uint32_t MSTR1:3;
                  uint32_t:1;
                uint32_t MSTR0:3;
            } B;
        } MPR1;                 /* Master Priority Register for Slave Port 1 */

        uint32_t xbar_reserved3[3];

        union {
            uint32_t R;
            struct {
                uint32_t RO:1;
                  uint32_t:21;
                uint32_t ARB:2;
                  uint32_t:2;
                uint32_t PCTL:2;
                  uint32_t:1;
                uint32_t PARK:3;
            } B;
        } SGPCR1;               /* General Purpose Control Register for Slave Port 1 */

        uint32_t xbar_reserved4[123];

        union {
            uint32_t R;
            struct {
                uint32_t:4;

                uint32_t:4;

                uint32_t:4;

                uint32_t:4;

                uint32_t:4;

                uint32_t:1;
                uint32_t MSTR2:3;
                  uint32_t:1;
                uint32_t MSTR1:3;
                  uint32_t:1;
                uint32_t MSTR0:3;
            } B;
        } MPR3;                 /* Master Priority Register for Slave Port 3 */

        uint32_t xbar_reserved5[3];

        union {
            uint32_t R;
            struct {
                uint32_t RO:1;
                  uint32_t:21;
                uint32_t ARB:2;
                  uint32_t:2;
                uint32_t PCTL:2;
                  uint32_t:1;
                uint32_t PARK:3;
            } B;
        } SGPCR3;               /* General Purpose Control Register for Slave Port 3 */
        uint32_t xbar_reserved6[187];

        union {
            uint32_t R;
            struct {
                uint32_t:4;

                uint32_t:4;

                uint32_t:4;

                uint32_t:4;

                uint32_t:4;

                uint32_t:1;
                uint32_t MSTR2:3;
                  uint32_t:1;
                uint32_t MSTR1:3;
                  uint32_t:1;
                uint32_t MSTR0:3;
            } B;
        } MPR6;                 /* Master Priority Register for Slave Port 6 */

        uint32_t xbar_reserved7[3];

        union {
            uint32_t R;
            struct {
                uint32_t RO:1;
                  uint32_t:21;
                uint32_t ARB:2;
                  uint32_t:2;
                uint32_t PCTL:2;
                  uint32_t:1;
                uint32_t PARK:3;
            } B;
        } SGPCR6;               /* General Purpose Control Register for Slave Port 6 */

        uint32_t xbar_reserved8[59];

        union {
            uint32_t R;
            struct {
                uint32_t:4;

                uint32_t:4;

                uint32_t:4;

                uint32_t:4;

                uint32_t:4;

                uint32_t:1;
                uint32_t MSTR2:3;
                  uint32_t:1;
                uint32_t MSTR1:3;
                  uint32_t:1;
                uint32_t MSTR0:3;
            } B;
        } MPR7;                 /* Master Priority Register for Slave Port 7 */

        uint32_t xbar_reserved9[3];

        union {
            uint32_t R;
            struct {
                uint32_t RO:1;
                  uint32_t:21;
                uint32_t ARB:2;
                  uint32_t:2;
                uint32_t PCTL:2;
                  uint32_t:1;
                uint32_t PARK:3;
            } B;
        } SGPCR7;               /* General Purpose Control Register for Slave Port 7 */

    };
/****************************************************************************/
/*                     MODULE : ECSM                                        */
/****************************************************************************/
    struct ECSM_tag {

        uint32_t ecsm_reserved1[5];

        uint16_t ecsm_reserved2;

        union {
            uint16_t R;
        } SWTCR;                //Software Watchdog Timer Control

        uint8_t ecsm_reserved3[3];

        union {
            uint8_t R;
        } SWTSR;                //SWT Service Register

        uint8_t ecsm_reserved4[3];

        union {
            uint8_t R;
        } SWTIR;                //SWT Interrupt Register

        uint32_t ecsm_reserved5a[1];
        uint32_t ecsm_reserved5b[1];

        uint32_t ecsm_reserved5c[6];

        uint8_t ecsm_reserved6[3];

        union {
            uint8_t R;
            struct {
                uint8_t:6;
                uint8_t ERNCR:1;
                uint8_t EFNCR:1;
            } B;
        } ECR;                  //ECC Configuration Register

        uint8_t mcm_reserved8[3];

        union {
            uint8_t R;
            struct {
                uint8_t:6;
                uint8_t RNCE:1;
                uint8_t FNCE:1;
            } B;
        } ESR;                  //ECC Status Register

        uint16_t ecsm_reserved9;

        union {
            uint16_t R;
            struct {
                uint16_t:6;
                uint16_t FRCNCI:1;
                uint16_t FR1NCI:1;
                  uint16_t:1;
                uint16_t ERRBIT:7;
            } B;
        } EEGR;                 //ECC Error Generation Register

        uint32_t ecsm_reserved10;

        union {
            uint32_t R;
            struct {
                uint32_t FEAR:32;
            } B;
        } FEAR;                 //Flash ECC Address Register

        uint16_t ecsm_reserved11;

        union {
            uint8_t R;
            struct {
                uint8_t:4;
                uint8_t FEMR:4;
            } B;
        } FEMR;                 //Flash ECC Master Register

        union {
            uint8_t R;
            struct {
                uint8_t WRITE:1;
                uint8_t SIZE:3;
                uint8_t PROT0:1;
                uint8_t PROT1:1;
                uint8_t PROT2:1;
                uint8_t PROT3:1;
            } B;
        } FEAT;                 //Flash ECC Attributes Register

        union {
            uint32_t R;
            struct {
                uint32_t FEDH:32;
            } B;
        } FEDRH;                //Flash ECC Data High Register

        union {
            uint32_t R;
            struct {
                uint32_t FEDL:32;
            } B;
        } FEDRL;                //Flash ECC Data Low Register

        union {
            uint32_t R;
            struct {
                uint32_t REAR:32;
            } B;
        } REAR;                 //RAM ECC Address

        uint8_t ecsm_reserved12[2];

        union {
            uint8_t R;
            struct {
                uint8_t:4;
                uint8_t REMR:4;
            } B;
        } REMR;                 //RAM ECC Master

        union {
            uint8_t R;
            struct {
                uint8_t WRITE:1;
                uint8_t SIZE:3;
                uint8_t PROT0:1;
                uint8_t PROT1:1;
                uint8_t PROT2:1;
                uint8_t PROT3:1;
            } B;
        } REAT;                 // RAM ECC Attributes Register

        union {
            uint32_t R;
            struct {
                uint32_t REDH:32;
            } B;
        } REDRH;                //RAM ECC Data High Register

        union {
            uint32_t R;
            struct {
                uint32_t REDL:32;
            } B;
        } REDRL;                //RAMECC Data Low Register

    };
/****************************************************************************/
/*                          MODULE : INTC                                   */
/****************************************************************************/
    struct INTC_tag {
        union {
            uint32_t R;
            struct {
                uint32_t:26;
                uint32_t VTES:1;
                  uint32_t:4;
                uint32_t HVEN:1;
            } B;
        } MCR;                  /* Module Configuration Register */

        int32_t INTC_reserved00;

        union {
            uint32_t R;
            struct {
                uint32_t:28;
                uint32_t PRI:4;
            } B;
        } CPR;                  /* Current Priority Register */

        uint32_t intc_reserved1;

        union {
            uint32_t R;
            struct {
                uint32_t VTBA:21;
                uint32_t INTVEC:9;
                  uint32_t:2;
            } B;
        } IACKR;                /* Interrupt Acknowledge Register */

        uint32_t intc_reserved2;

        union {
            uint32_t R;
            struct {
                uint32_t:32;
            } B;
        } EOIR;                 /* End of Interrupt Register */

        uint32_t intc_reserved3;

        union {
            uint8_t R;
            struct {
                uint8_t:6;
                uint8_t SET:1;
                uint8_t CLR:1;
            } B;
        } SSCIR[8];             /* Software Set/Clear Interruput Register */

        uint32_t intc_reserved4[6];

        union {
            uint8_t R;
            struct {
                uint8_t:4;
                uint8_t PRI:4;
            } B;
        } PSR[358];             /* Software Set/Clear Interrupt Register */

    };
/****************************************************************************/
/*                          MODULE : EQADC                                  */
/****************************************************************************/
    struct EQADC_tag {
        union {
            uint32_t R;
            struct {
                uint32_t:27;
                uint32_t ESSIE:2;
                  uint32_t:1;
                uint32_t DBG:2;
            } B;
        } MCR;                  /* Module Configuration Register */

        int32_t EQADC_reserved00;

        union {
            uint32_t R;
            struct {
                uint32_t:6;
                uint32_t NMF:26;
            } B;
        } NMSFR;                /* Null Message Send Format Register */

        union {
            uint32_t R;
            struct {
                uint32_t:28;
                uint32_t DFL:4;
            } B;
        } ETDFR;                /* External Trigger Digital Filter Register */

        union {
            uint32_t R;
            struct {
                uint32_t CFPUSH:32;
            } B;
        } CFPR[6];              /* CFIFO Push Registers */

        uint32_t eqadc_reserved1;

        uint32_t eqadc_reserved2;

        union {
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t RFPOP:16;
            } B;
        } RFPR[6];              /* Result FIFO Pop Registers */

        uint32_t eqadc_reserved3;

        uint32_t eqadc_reserved4;

        union EQADC_CFCR_tag {
            uint16_t R;
            struct {
                uint16_t:5;
                uint16_t SSE:1;
                uint16_t CFINV:1;
                  uint16_t:1;
                uint16_t MODE:4;
                  uint16_t:4;
            } B;
        } CFCR[6];              /* CFIFO Control Registers */

        uint32_t eqadc_reserved5;

        union EQADC_IDCR_tag {
            uint16_t R;
            struct {
                uint16_t NCIE:1;
                uint16_t TORIE:1;
                uint16_t PIE:1;
                uint16_t EOQIE:1;
                uint16_t CFUIE:1;
                  uint16_t:1;
                uint16_t CFFE:1;
                uint16_t CFFS:1;
                  uint16_t:4;
                uint16_t RFOIE:1;
                  uint16_t:1;
                uint16_t RFDE:1;
                uint16_t RFDS:1;
            } B;
        } IDCR[6];              /* Interrupt and DMA Control Registers */

        uint32_t eqadc_reserved6;

        union EQADC_FISR_tag {
            uint32_t R;
            struct {
                uint32_t NCF:1;
                uint32_t TORF:1;
                uint32_t PF:1;
                uint32_t EOQF:1;
                uint32_t CFUF:1;
                uint32_t SSS:1;
                uint32_t CFFF:1;
                  uint32_t:5;
                uint32_t RFOF:1;
                  uint32_t:1;
                uint32_t RFDF:1;
                  uint32_t:1;
                uint32_t CFCTR:4;
                uint32_t TNXTPTR:4;
                uint32_t RFCTR:4;
                uint32_t POPNXTPTR:4;
            } B;
        } FISR[6];              /* FIFO and Interrupt Status Registers */

        uint32_t eqadc_reserved7;

        uint32_t eqadc_reserved8;

        union {
            uint16_t R;
            struct {
                uint16_t:5;
                uint16_t TCCF:11;
            } B;
        } CFTCR[6];             /* CFIFO Transfer Counter Registers */

        uint32_t eqadc_reserved9;

        union {
            uint32_t R;
            struct {
                uint32_t CFS0:2;
                uint32_t CFS1:2;
                uint32_t CFS2:2;
                uint32_t CFS3:2;
                uint32_t CFS4:2;
                uint32_t CFS5:2;
                  uint32_t:5;
                uint32_t LCFTCB0:4;
                uint32_t TC_LCFTCB0:11;
            } B;
        } CFSSR0;               /* CFIFO Status Register 0 */

        union {
            uint32_t R;
            struct {
                uint32_t CFS0:2;
                uint32_t CFS1:2;
                uint32_t CFS2:2;
                uint32_t CFS3:2;
                uint32_t CFS4:2;
                uint32_t CFS5:2;
                  uint32_t:5;
                uint32_t LCFTCB1:4;
                uint32_t TC_LCFTCB1:11;
            } B;
        } CFSSR1;               /* CFIFO Status Register 1 */

        union {
            uint32_t R;
            struct {
                uint32_t CFS0:2;
                uint32_t CFS1:2;
                uint32_t CFS2:2;
                uint32_t CFS3:2;
                uint32_t CFS4:2;
                uint32_t CFS5:2;
                  uint32_t:4;
                uint32_t ECBNI:1;
                uint32_t LCFTSSI:4;
                uint32_t TC_LCFTSSI:11;
            } B;
        } CFSSR2;               /* CFIFO Status Register 2 */

        union {
            uint32_t R;
            struct {
                uint32_t CFS0:2;
                uint32_t CFS1:2;
                uint32_t CFS2:2;
                uint32_t CFS3:2;
                uint32_t CFS4:2;
                uint32_t CFS5:2;
                  uint32_t:20;
            } B;
        } CFSR;

        uint32_t eqadc_reserved11;

        union {
            uint32_t R;
            struct {
                uint32_t:21;
                uint32_t MDT:3;
                  uint32_t:4;
                uint32_t BR:4;
            } B;
        } SSICR;                /* SSI Control Register */

        union {
            uint32_t R;
            struct {
                uint32_t RDV:1;
                  uint32_t:5;
                uint32_t RDATA:26;
            } B;
        } SSIRDR;               /* SSI Recieve Data Register */

        uint32_t eqadc_reserved12[17];

        struct {
            union {
                uint32_t R;
                struct {
                    uint32_t:32;
                } B;
            } R[4];

            uint32_t eqadc_reserved13[12];

        } CF[6];

        uint32_t eqadc_reserved14[32];

        struct {
            union {
                uint32_t R;
                struct {
                    uint32_t:32;
                } B;
            } R[4];

            uint32_t eqadc_reserved15[12];

        } RF[6];

    };

/* Message Formats for On-Chip ADC Operation
 */
union EQADC_CONVERSION_COMMAND_tag {
    uint32_t R;
    struct {
        uint32_t EOQ:1;
        uint32_t PAUSE:1;
            uint32_t:3;
        uint32_t EB:1;
        uint32_t BN:1;
        uint32_t CAL:1;
        uint32_t MESSAGE_TAG:4;
        uint32_t LST:2;
        uint32_t TSR:1;
        uint32_t FMT:1;
        uint32_t CHANNEL_NUMBER:8;
            uint32_t:8;
    } B;
}; /* Conversion command */

union EQADC_WRITE_CONFIGURATION_COMMAND_tag {
    uint32_t R;
    struct {
        uint32_t EOQ:1;
        uint32_t PAUSE:1;
            uint32_t:3;
        uint32_t EB:1;
        uint32_t BN:1;
        uint32_t RW:1;
        uint32_t VALUE:16;
        uint32_t ADDR:8;
    } B;
}; /* Write configuration command */

/****************************************************************************/
/*                          MODULE : DSPI                                   */
/****************************************************************************/
    struct DSPI_tag {
        union DSPI_MCR_tag {
            uint32_t R;
            struct {
                uint32_t MSTR:1;
                uint32_t CONT_SCKE:1;
                uint32_t DCONF:2;
                uint32_t FRZ:1;
                uint32_t MTFE:1;
                uint32_t PCSSE:1;
                uint32_t ROOE:1;
                  uint32_t:2;
                uint32_t PCSIS5:1;
                uint32_t PCSIS4:1;
                uint32_t PCSIS3:1;
                uint32_t PCSIS2:1;
                uint32_t PCSIS1:1;
                uint32_t PCSIS0:1;
                uint32_t DOZE:1;
                uint32_t MDIS:1;
                uint32_t DIS_TXF:1;
                uint32_t DIS_RXF:1;
                uint32_t CLR_TXF:1;
                uint32_t CLR_RXF:1;
                uint32_t SMPL_PT:2;
                  uint32_t:7;
                uint32_t HALT:1;
            } B;
        } MCR;                  /* Module Configuration Register */

        uint32_t dspi_reserved1;

        union {
            uint32_t R;
            struct {
                uint32_t TCNT:16;
                  uint32_t:16;
            } B;
        } TCR;

        union DSPI_CTAR_tag {
            uint32_t R;
            struct {
                uint32_t DBR:1;
                uint32_t FMSZ:4;
                uint32_t CPOL:1;
                uint32_t CPHA:1;
                uint32_t LSBFE:1;
                uint32_t PCSSCK:2;
                uint32_t PASC:2;
                uint32_t PDT:2;
                uint32_t PBR:2;
                uint32_t CSSCK:4;
                uint32_t ASC:4;
                uint32_t DT:4;
                uint32_t BR:4;
            } B;
        } CTAR[8];              /* Clock and Transfer Attributes Registers */

        union DSPI_SR_tag {
            uint32_t R;
            struct {
                uint32_t TCF:1;
                uint32_t TXRXS:1;
                  uint32_t:1;
                uint32_t EOQF:1;
                uint32_t TFUF:1;
                  uint32_t:1;
                uint32_t TFFF:1;
                  uint32_t:5;
                uint32_t RFOF:1;
                  uint32_t:1;
                uint32_t RFDF:1;
                  uint32_t:1;
                uint32_t TXCTR:4;
                uint32_t TXNXTPTR:4;
                uint32_t RXCTR:4;
                uint32_t POPNXTPTR:4;
            } B;
        } SR;                   /* Status Register */

        union DSPI_RSER_tag {
            uint32_t R;
            struct {
                uint32_t TCFRE:1;
                  uint32_t:2;
                uint32_t EOQFRE:1;
                uint32_t TFUFRE:1;
                  uint32_t:1;
                uint32_t TFFFRE:1;
                uint32_t TFFFDIRS:1;
                  uint32_t:4;
                uint32_t RFOFRE:1;
                  uint32_t:1;
                uint32_t RFDFRE:1;
                uint32_t RFDFDIRS:1;
                  uint32_t:16;
            } B;
        } RSER;                 /* DMA/Interrupt Request Select and Enable Register */

        union DSPI_PUSHR_tag {
            uint32_t R;
            struct {
                uint32_t CONT:1;
                uint32_t CTAS:3;
                uint32_t EOQ:1;
                uint32_t CTCNT:1;
                  uint32_t:4;
                uint32_t PCS5:1;
                uint32_t PCS4:1;
                uint32_t PCS3:1;
                uint32_t PCS2:1;
                uint32_t PCS1:1;
                uint32_t PCS0:1;
                uint32_t TXDATA:16;
            } B;
        } PUSHR;                /* PUSH TX FIFO Register */

        union DSPI_POPR_tag {
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t RXDATA:16;
            } B;
        } POPR;                 /* POP RX FIFO Register */

        union {
            uint32_t R;
            struct {
                uint32_t TXCMD:16;
                uint32_t TXDATA:16;
            } B;
        } TXFR[4];              /* Transmit FIFO Registers */

        uint32_t DSPI_reserved_txf[12];

        union {
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t RXDATA:16;
            } B;
        } RXFR[4];              /* Transmit FIFO Registers */

        uint32_t DSPI_reserved_rxf[12];

        union {
            uint32_t R;
            struct {
                uint32_t MTOE:1;
                  uint32_t:1;
                uint32_t MTOCNT:6;
                  uint32_t:4;
                uint32_t TXSS:1;
                uint32_t TPOL:1;
                uint32_t TRRE:1;
                uint32_t CID:1;
                uint32_t DCONT:1;
                uint32_t DSICTAS:3;
                  uint32_t:6;
                uint32_t DPCS5:1;
                uint32_t DPCS4:1;
                uint32_t DPCS3:1;
                uint32_t DPCS2:1;
                uint32_t DPCS1:1;
                uint32_t DPCS0:1;
            } B;
        } DSICR;                /* DSI Configuration Register */

        union {
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t SER_DATA:16;
            } B;
        } SDR;                  /* DSI Serialization Data Register */

        union {
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t ASER_DATA:16;
            } B;
        } ASDR;                 /* DSI Alternate Serialization Data Register */

        union {
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t COMP_DATA:16;
            } B;
        } COMPR;                /* DSI Transmit Comparison Register */

        union {
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t DESER_DATA:16;
            } B;
        } DDR;                  /* DSI deserialization Data Register */

    };
/****************************************************************************/
/*                          MODULE : eSCI                                   */
/****************************************************************************/
    struct ESCI_tag {
        union ESCI_CR1_tag {
            uint32_t R;
            struct {
                uint32_t:3;
                uint32_t SBR:13;
                uint32_t LOOPS:1;
                uint32_t SCISDOZ:1;
                uint32_t RSRC:1;
                uint32_t M:1;
                uint32_t WAKE:1;
                uint32_t ILT:1;
                uint32_t PE:1;
                uint32_t PT:1;
                uint32_t TIE:1;
                uint32_t TCIE:1;
                uint32_t RIE:1;
                uint32_t ILIE:1;
                uint32_t TE:1;
                uint32_t RE:1;
                uint32_t RWU:1;
                uint32_t SBK:1;
            } B;
        } CR1;                  /* Control Register 1 */

        union ESCI_CR2_tag {
            uint16_t R;
            struct {
                uint16_t MDIS:1;
                uint16_t FBR:1;
                uint16_t BSTP:1;
                uint16_t IEBERR:1;
                uint16_t RXDMA:1;
                uint16_t TXDMA:1;
                uint16_t BRK13:1;
                  uint16_t:1;
                uint16_t BESM13:1;
                uint16_t SBSTP:1;
                  uint16_t:2;
                uint16_t ORIE:1;
                uint16_t NFIE:1;
                uint16_t FEIE:1;
                uint16_t PFIE:1;
            } B;
        } CR2;                  /* Control Register 2 */

        union ESCI_DR_tag {
            uint16_t R;
            struct {
                uint16_t R8:1;
                uint16_t T8:1;
                  uint16_t:6;
                uint8_t D;
            } B;
        } DR;                   /* Data Register */

        union ESCI_SR_tag {
            uint32_t R;
            struct {
                uint32_t TDRE:1;
                uint32_t TC:1;
                uint32_t RDRF:1;
                uint32_t IDLE:1;
                uint32_t OR:1;
                uint32_t NF:1;
                uint32_t FE:1;
                uint32_t PF:1;
                  uint32_t:3;
                uint32_t BERR:1;
                  uint32_t:3;
                uint32_t RAF:1;
                uint32_t RXRDY:1;
                uint32_t TXRDY:1;
                uint32_t LWAKE:1;
                uint32_t STO:1;
                uint32_t PBERR:1;
                uint32_t CERR:1;
                uint32_t CKERR:1;
                uint32_t FRC:1;
                  uint32_t:7;
                uint32_t OVFL:1;
            } B;
        } SR;                   /* Status Register */

        union {
            uint32_t R;
            struct {
                uint32_t LRES:1;
                uint32_t WU:1;
                uint32_t WUD0:1;
                uint32_t WUD1:1;
                uint32_t LDBG:1;
                uint32_t DSF:1;
                uint32_t PRTY:1;
                uint32_t LIN:1;
                uint32_t RXIE:1;
                uint32_t TXIE:1;
                uint32_t WUIE:1;
                uint32_t STIE:1;
                uint32_t PBIE:1;
                uint32_t CIE:1;
                uint32_t CKIE:1;
                uint32_t FCIE:1;
                  uint32_t:7;
                uint32_t OFIE:1;
                  uint32_t:8;
            } B;
        } LCR;                  /* LIN Control Register */

        union {
            uint32_t R;
        } LTR;                  /* LIN Transmit Register */

        union {
            uint32_t R;
        } LRR;                  /* LIN Recieve Register */

        union {
            uint32_t R;
        } LPR;                  /* LIN CRC Polynom Register  */

    };
/****************************************************************************/
/*                          MODULE : FlexCAN                                */
/****************************************************************************/
    struct FLEXCAN2_tag {
        union {
            uint32_t R;
            struct {
                uint32_t MDIS:1;
                uint32_t FRZ:1;
                  uint32_t:1;
                uint32_t HALT:1;
                uint32_t NOTRDY:1;
                  uint32_t:1;
                uint32_t SOFTRST:1;
                uint32_t FRZACK:1;
                  uint32_t:1;
                  uint32_t:1;

                  uint32_t:1;

                uint32_t MDISACK:1;
                  uint32_t:1;
                  uint32_t:1;

                  uint32_t:12;

                uint32_t MAXMB:6;
            } B;
        } MCR;                  /* Module Configuration Register */

        union {
            uint32_t R;
            struct {
                uint32_t PRESDIV:8;
                uint32_t RJW:2;
                uint32_t PSEG1:3;
                uint32_t PSEG2:3;
                uint32_t BOFFMSK:1;
                uint32_t ERRMSK:1;
                uint32_t CLKSRC:1;
                uint32_t LPB:1;

                  uint32_t:4;

                uint32_t SMP:1;
                uint32_t BOFFREC:1;
                uint32_t TSYN:1;
                uint32_t LBUF:1;
                uint32_t LOM:1;
                uint32_t PROPSEG:3;
            } B;
        } CR;                   /* Control Register */

        union {
            uint32_t R;
        } TIMER;                /* Free Running Timer */
        int32_t FLEXCAN_reserved00;

        union {
            uint32_t R;
            struct {
                uint32_t:3;
                uint32_t MI:29;
            } B;
        } RXGMASK;              /* RX Global Mask */

        union {
            uint32_t R;
            struct {
                uint32_t:3;
                uint32_t MI:29;
            } B;
        } RX14MASK;             /* RX 14 Mask */

        union {
            uint32_t R;
            struct {
                uint32_t:3;
                uint32_t MI:29;
            } B;
        } RX15MASK;             /* RX 15 Mask */

        union {
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t RXECNT:8;
                uint32_t TXECNT:8;
            } B;
        } ECR;                  /* Error Counter Register */

        union {
            uint32_t R;
            struct {
                uint32_t:14;

                uint32_t:2;

                uint32_t BIT1ERR:1;
                uint32_t BIT0ERR:1;
                uint32_t ACKERR:1;
                uint32_t CRCERR:1;
                uint32_t FRMERR:1;
                uint32_t STFERR:1;
                uint32_t TXWRN:1;
                uint32_t RXWRN:1;
                uint32_t IDLE:1;
                uint32_t TXRX:1;
                uint32_t FLTCONF:2;
                  uint32_t:1;
                uint32_t BOFFINT:1;
                uint32_t ERRINT:1;
                  uint32_t:1;
            } B;
        } ESR;                  /* Error and Status Register */

        union {
            uint32_t R;
            struct {
                uint32_t BUF63M:1;
                uint32_t BUF62M:1;
                uint32_t BUF61M:1;
                uint32_t BUF60M:1;
                uint32_t BUF59M:1;
                uint32_t BUF58M:1;
                uint32_t BUF57M:1;
                uint32_t BUF56M:1;
                uint32_t BUF55M:1;
                uint32_t BUF54M:1;
                uint32_t BUF53M:1;
                uint32_t BUF52M:1;
                uint32_t BUF51M:1;
                uint32_t BUF50M:1;
                uint32_t BUF49M:1;
                uint32_t BUF48M:1;
                uint32_t BUF47M:1;
                uint32_t BUF46M:1;
                uint32_t BUF45M:1;
                uint32_t BUF44M:1;
                uint32_t BUF43M:1;
                uint32_t BUF42M:1;
                uint32_t BUF41M:1;
                uint32_t BUF40M:1;
                uint32_t BUF39M:1;
                uint32_t BUF38M:1;
                uint32_t BUF37M:1;
                uint32_t BUF36M:1;
                uint32_t BUF35M:1;
                uint32_t BUF34M:1;
                uint32_t BUF33M:1;
                uint32_t BUF32M:1;
            } B;
        } IMRH;                 /* Interruput Masks Register */

        union {
            uint32_t R;
            struct {
                uint32_t BUF31M:1;
                uint32_t BUF30M:1;
                uint32_t BUF29M:1;
                uint32_t BUF28M:1;
                uint32_t BUF27M:1;
                uint32_t BUF26M:1;
                uint32_t BUF25M:1;
                uint32_t BUF24M:1;
                uint32_t BUF23M:1;
                uint32_t BUF22M:1;
                uint32_t BUF21M:1;
                uint32_t BUF20M:1;
                uint32_t BUF19M:1;
                uint32_t BUF18M:1;
                uint32_t BUF17M:1;
                uint32_t BUF16M:1;
                uint32_t BUF15M:1;
                uint32_t BUF14M:1;
                uint32_t BUF13M:1;
                uint32_t BUF12M:1;
                uint32_t BUF11M:1;
                uint32_t BUF10M:1;
                uint32_t BUF09M:1;
                uint32_t BUF08M:1;
                uint32_t BUF07M:1;
                uint32_t BUF06M:1;
                uint32_t BUF05M:1;
                uint32_t BUF04M:1;
                uint32_t BUF03M:1;
                uint32_t BUF02M:1;
                uint32_t BUF01M:1;
                uint32_t BUF00M:1;
            } B;
        } IMRL;                 /* Interruput Masks Register */

        union {
            uint32_t R;
            struct {
                uint32_t BUF63I:1;
                uint32_t BUF62I:1;
                uint32_t BUF61I:1;
                uint32_t BUF60I:1;
                uint32_t BUF59I:1;
                uint32_t BUF58I:1;
                uint32_t BUF57I:1;
                uint32_t BUF56I:1;
                uint32_t BUF55I:1;
                uint32_t BUF54I:1;
                uint32_t BUF53I:1;
                uint32_t BUF52I:1;
                uint32_t BUF51I:1;
                uint32_t BUF50I:1;
                uint32_t BUF49I:1;
                uint32_t BUF48I:1;
                uint32_t BUF47I:1;
                uint32_t BUF46I:1;
                uint32_t BUF45I:1;
                uint32_t BUF44I:1;
                uint32_t BUF43I:1;
                uint32_t BUF42I:1;
                uint32_t BUF41I:1;
                uint32_t BUF40I:1;
                uint32_t BUF39I:1;
                uint32_t BUF38I:1;
                uint32_t BUF37I:1;
                uint32_t BUF36I:1;
                uint32_t BUF35I:1;
                uint32_t BUF34I:1;
                uint32_t BUF33I:1;
                uint32_t BUF32I:1;
            } B;
        } IFRH;                 /* Interruput Flag Register */

        union {
            uint32_t R;
            struct {
                uint32_t BUF31I:1;
                uint32_t BUF30I:1;
                uint32_t BUF29I:1;
                uint32_t BUF28I:1;
                uint32_t BUF27I:1;
                uint32_t BUF26I:1;
                uint32_t BUF25I:1;
                uint32_t BUF24I:1;
                uint32_t BUF23I:1;
                uint32_t BUF22I:1;
                uint32_t BUF21I:1;
                uint32_t BUF20I:1;
                uint32_t BUF19I:1;
                uint32_t BUF18I:1;
                uint32_t BUF17I:1;
                uint32_t BUF16I:1;
                uint32_t BUF15I:1;
                uint32_t BUF14I:1;
                uint32_t BUF13I:1;
                uint32_t BUF12I:1;
                uint32_t BUF11I:1;
                uint32_t BUF10I:1;
                uint32_t BUF09I:1;
                uint32_t BUF08I:1;
                uint32_t BUF07I:1;
                uint32_t BUF06I:1;
                uint32_t BUF05I:1;
                uint32_t BUF04I:1;
                uint32_t BUF03I:1;
                uint32_t BUF02I:1;
                uint32_t BUF01I:1;
                uint32_t BUF00I:1;
            } B;
        } IFRL;                 /* Interruput Flag Register */

        uint32_t flexcan2_reserved2[19];

        struct canbuf_t {
            union {
                uint32_t R;
                struct {
                    uint32_t:4;
                    uint32_t CODE:4;
                      uint32_t:1;
                    uint32_t SRR:1;
                    uint32_t IDE:1;
                    uint32_t RTR:1;
                    uint32_t LENGTH:4;
                    uint32_t TIMESTAMP:16;
                } B;
            } CS;

            union {
                uint32_t R;
                struct {
                    uint32_t:3;
                    uint32_t STD_ID:11;
                    uint32_t EXT_ID:18;
                } B;
            } ID;

            union {
                uint8_t B[8];  /* Data buffer in Bytes (8 bits) */
                uint16_t H[4]; /* Data buffer in Half-words (16 bits) */
                uint32_t W[2]; /* Data buffer in words (32 bits) */
                uint32_t R[2]; /* Data buffer in words (32 bits) */
            } DATA;

        } BUF[64];
    };

/* Define memories */

#define SRAM_START  0x40000000
#define SRAM_SIZE      0x10000
#define SRAM_END    0x4000FFFF

#define FLASH_START         0x0
#define FLASH_SIZE     0x200000
#define FLASH_END      0x1FFFFF

/* Define instances of modules */
#define PBRIDGE_A (*(volatile struct PBRIDGE_A_tag *) 0xC3F00000)
#define FMPLL     (*(volatile struct FMPLL_tag *)     0xC3F80000)
#define EBI       (*(volatile struct EBI_tag *)       0xC3F84000)
#define FLASH     (*(volatile struct FLASH_tag *)     0xC3F88000)
#define SIU       (*(volatile struct SIU_tag *)       0xC3F90000)

#define EMIOS     (*(volatile struct EMIOS_tag *)     0xC3FA0000)
#define ETPU      (*(volatile struct ETPU_tag *)      0xC3FC0000)
#define ETPU_DATA_RAM  (*( uint32_t *)        0xC3FC8000)
#define ETPU_DATA_RAM_EXT  (*( uint32_t *)    0xC3FCC000)
#define ETPU_DATA_RAM_END  0xC3FC8BFC
#define CODE_RAM       (*( uint32_t *)        0xC3FD0000)
#define ETPU_CODE_RAM  (*( uint32_t *)        0xC3FD0000)

#define PBRIDGE_B (*(volatile struct PBRIDGE_B_tag *) 0xFFF00000)
#define XBAR      (*(volatile struct XBAR_tag *)      0xFFF04000)
#define ECSM      (*(volatile struct ECSM_tag *)      0xFFF40000)
#define EDMA      (*(volatile struct EDMA_tag *)      0xFFF44000)
#define INTC      (*(volatile struct INTC_tag *)      0xFFF48000)

#define EQADC     (*(volatile struct EQADC_tag *)     0xFFF80000)

#define DSPI_A    (*(volatile struct DSPI_tag *)      0xFFF90000)
#define DSPI_B    (*(volatile struct DSPI_tag *)      0xFFF94000)
#define DSPI_C    (*(volatile struct DSPI_tag *)      0xFFF98000)
#define DSPI_D    (*(volatile struct DSPI_tag *)      0xFFF9C000)

#define ESCI_A    (*(volatile struct ESCI_tag *)      0xFFFB0000)
#define ESCI_B    (*(volatile struct ESCI_tag *)      0xFFFB4000)

#define CAN_A     (*(volatile struct FLEXCAN2_tag *)  0xFFFC0000)
#define CAN_B     (*(volatile struct FLEXCAN2_tag *)  0xFFFC4000)
#define CAN_C     (*(volatile struct FLEXCAN2_tag *)  0xFFFC8000)

#ifdef __MWERKS__
#pragma pop
#endif

#ifdef  __cplusplus
}
#endif
#endif /* ASM */
#endif                          /* ifdef _MPC5554_H */
/*********************************************************************
 *
 * Copyright:
 *	Freescale Semiconductor, INC. All Rights Reserved.
 *  You are hereby granted a copyright license to use, modify, and
 *  distribute the SOFTWARE so long as this entire notice is
 *  retained without alteration in any modified and/or redistributed
 *  versions, and that such modified versions are clearly identified
 *  as such. No licenses are granted by implication, estoppel or
 *  otherwise under any patents or trademarks of Freescale
 *  Semiconductor, Inc. This software is provided on an "AS IS"
 *  basis and without warranty.
 *
 *  To the maximum extent permitted by applicable law, Freescale
 *  Semiconductor DISCLAIMS ALL WARRANTIES WHETHER EXPRESS OR IMPLIED,
 *  INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A
 *  PARTICULAR PURPOSE AND ANY WARRANTY AGAINST INFRINGEMENT WITH
 *  REGARD TO THE SOFTWARE (INCLUDING ANY MODIFIED VERSIONS THEREOF)
 *  AND ANY ACCOMPANYING WRITTEN MATERIALS.
 *
 *  To the maximum extent permitted by applicable law, IN NO EVENT
 *  SHALL Freescale Semiconductor BE LIABLE FOR ANY DAMAGES WHATSOEVER
 *  (INCLUDING WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
 *  BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR OTHER
 *  PECUNIARY LOSS) ARISING OF THE USE OR INABILITY TO USE THE SOFTWARE.
 *
 *  Freescale Semiconductor assumes no responsibility for the
 *  maintenance and support of this software
 *
 ********************************************************************/

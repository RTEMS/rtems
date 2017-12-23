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
/* FILE NAME: mpc5510.h                      COPYRIGHT (c) Freescale 2008 */
/* REVISION:  2.2                                  All Rights Reserved    */
/*                                                                        */
/* DESCRIPTION:                                                           */
/* This file contain all of the register and bit field definitions for    */
/* MPC5510.                                                               */
/**************************************************************************/
/*>>>>NOTE! this file is auto-generated please do not edit it!<<<<*/

/*************************************************/
/* Example register & bit field write:           */
/*                                               */
/*  <MODULE>.<REGISTER>.B.<BIT> = 1;             */
/*  <MODULE>.<REGISTER>.R       = 0x10000000;    */
/*                                               */
/*************************************************/

#ifndef _MPC5510_H_
#define _MPC5510_H_

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
/*                     MODULE : CRP                                         */
/****************************************************************************/
    struct CRP_tag {

        union {
            uint32_t R;
            struct {
                uint32_t:12;
                uint32_t IRC32KEN:1;
                uint32_t XOSCEN:1;
                  uint32_t:1;
                uint32_t OSC32KEN:1;
                uint32_t TRIM32IRC:8;
                uint32_t TRIMIRC:8;
            } B;
        } CLKSRC;               /* Clock Source Register */

        uint32_t crp_reserved1[3];

        union {
            uint32_t R;
            struct {
                uint32_t CNTEN:1;
                uint32_t RTCIE:1;
                uint32_t RTCF:1;
                uint32_t ROVRF:1;
                uint32_t RTCVAL:12;
                uint32_t APIEN:1;
                uint32_t APIIE:1;
                uint32_t APIF:1;
                uint32_t CLKSEL:2;
                uint32_t ROVREN:1;
                uint32_t APIVAL:10;
            } B;
        } RTCSC;                /* RTC Status and Control Register */

        union {
            uint32_t R;
            struct {
                uint32_t RTCCNT:32;
            } B;
        } RTCCNT;               /* RTC Counter Register */

        uint32_t crp_reserved2[10];

        union {
            uint32_t R;
            struct {
                uint32_t:1;
                uint32_t WKPSEL7:3;
                  uint32_t:1;
                uint32_t WKPSEL6:3;
                  uint32_t:1;
                uint32_t WKPSEL5:3;
                  uint32_t:1;
                uint32_t WKPSEL4:3;
                  uint32_t:1;
                uint32_t WKPSEL3:3;
                  uint32_t:1;
                uint32_t WKPSEL2:3;
                  uint32_t:1;
                uint32_t WKPSEL1:3;
                  uint32_t:1;
                uint32_t WKPSEL0:3;
            } B;
        } WKPINSEL;             /* Wakeup Pin Source Select Register */

        union {
            uint32_t R;
            struct {
                uint32_t WKPDET7:2;
                uint32_t WKPDET6:2;
                uint32_t WKPDET5:2;
                uint32_t WKPDET4:2;
                uint32_t WKPDET3:2;
                uint32_t WKPDET2:2;
                uint32_t WKPDET1:2;
                uint32_t WKPDET0:2;
                  uint32_t:5;
                uint32_t RTCOVREN:1;
                uint32_t RTCWKEN:1;
                uint32_t APIWKEN:1;
                  uint32_t:7;
                uint32_t WKCLKSEL:1;
            } B;
        } WKSE;                 /* Wakeup Source Enable Register */

        uint32_t crp_reserved3[2];

        union {
            uint32_t R;
            struct {
                uint32_t Z1VEC:30;
                uint32_t Z1RST:1;
                uint32_t VLE:1;
            } B;
        } Z1VEC;                /* Z1 Reset Vector Register */

        union {
            uint32_t R;
            struct {
                uint32_t Z0VEC:30;
                uint32_t Z0RST:1;
                  uint32_t:1;
            } B;
        } Z0VEC;                /* Z0 Reset Vector Register */

        union {
            uint32_t R;
            struct {
                uint32_t RECPTR:30;
                uint32_t FASTREC:1;
                  uint32_t:1;
            } B;
        } RECPTR;               /* Reset Recovery Pointer Register */

        uint32_t crp_reserved4;

        union {
            uint32_t R;
            struct {
                uint32_t SLEEPF:1;
                uint32_t STOPF:1;
                  uint32_t:3;
                uint32_t WKRLLOVRF:1;
                uint32_t WKAPIF:1;
                uint32_t WKRTCF:1;
                uint32_t PWKSCRF:8;
                uint32_t SLEEP:1;
                uint32_t STOP:1;
                  uint32_t:1;
                uint32_t PKREL:1;
                uint32_t SLP12EN:1;
                uint32_t RAMSEL:3;
                uint32_t PWKSRIE:8;
            } B;
        } PSCR;                 /* Power Status and Control Register */

        uint32_t crp_reserved5[3];

        union {
            uint32_t R;
            struct {
                uint32_t LVI5IE:1;
                uint32_t LVI5HIE:1;
                uint32_t LVI5F:1;
                uint32_t LVI5HF:1;
                uint32_t LVI5LOCK:1;
                uint32_t LVI5RE:1;
                  uint32_t:9;
                uint32_t BYPDIS:1;
                  uint32_t:16;
            } B;
        } SOCSC;                /* LVI Status and Control Register */

    };
/****************************************************************************/
/*                          MODULE : DMAMUX                                 */
/****************************************************************************/
    struct DMAMUX_tag {
        union {
            uint8_t R;
            struct {
                uint8_t ENBL:1;
                uint8_t TRIG:1;
                uint8_t SOURCE:6;
            } B;
        } CHCONFIG[16];         /* DMA Channel Configuration Register */

    };
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
                  uint32_t:1;
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
                uint32_t SPI_TCNT:16;
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
                uint32_t TCF_RE:1;
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
                uint32_t:12;
                uint32_t TXSS:1;
                  uint32_t:2;
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
/*                     MODULE : External Bus Interface (EBI)                */
/****************************************************************************/

/* CS_tag instantiated within EBI_tag */
    struct EBI_CS_tag {
        union {                 /* Base Register Bank */
            uint32_t R;
            struct {
                uint32_t BA:17;
                  uint32_t:3;
                uint32_t PS:1;
                  uint32_t:3;
                uint32_t AD_MUX:1;
                uint32_t BL:1;
                uint32_t WEBS:1;
                uint32_t TBDIP:1;
                  uint32_t:1;
                uint32_t SETA:1;
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
                uint32_t:16;
                uint32_t ACGE:1;
                uint32_t EXTM:1;
                uint32_t EARB:1;
                  uint32_t:6;
                uint32_t MDIS:1;
                  uint32_t:3;
                uint32_t D16_31:1;
                uint32_t AD_MUX:1;
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

        /* Roll in 3x CS registers */
        struct EBI_CS_tag CS[4];

        uint32_t EBI_reserved2[4];

        struct EBI_CAL_CS_tag CAL_CS[4];
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
                  uint32_t:1;
                uint32_t GPREN:1;
                  uint32_t:10;
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
        } GFG;                  /* Global FLAG Register */

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

        union {
            uint32_t R;
            struct {
                uint32_t:8;
                uint32_t UC23:1;
                uint32_t UC22:1;
                uint32_t UC21:1;
                uint32_t UC20:1;
                uint32_t UC19:1;
                uint32_t UC18:1;
                uint32_t UC17:1;
                uint32_t UC16:1;
                uint32_t UC15:1;
                uint32_t UC14:1;
                uint32_t UC13:1;
                uint32_t UC12:1;
                uint32_t UC11:1;
                uint32_t UC10:1;
                uint32_t UC9:1;
                uint32_t UC8:1;
                uint32_t UC7:1;
                uint32_t UC6:1;
                uint32_t UC5:1;
                uint32_t UC4:1;
                uint32_t UC3:1;
                uint32_t UC2:1;
                uint32_t UC1:1;
                uint32_t UC0:1;
            } B;
        } UCDIS;                /* Disable Channel Register */

        uint32_t emios_reserved1[4];

        struct EMIOS_CH_tag {
            union {
                uint32_t R;
                struct {
                    uint32_t:16;
                    uint32_t A:16;     /* Channel A Data Register */
                } B;
            } CADR;

            union {
                uint32_t R;
                struct {
                    uint32_t:16;
                    uint32_t B:16;     /* Channel B Data Register */
                } B;
            } CBDR;

            union {
                uint32_t R;    /* Channel Counter Register */
                struct {
                    uint32_t:16;
                    uint32_t C:16;     /* Channel C Data Register */
                } B;
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

            union {
                uint32_t R;    /* Alternate Channel A Data Register */
            } ALTA;

            uint32_t emios_channel_reserved[2];

        } CH[24];

    };
/****************************************************************************/
/*                          MODULE : EQADC                                  */
/****************************************************************************/
    struct EQADC_tag {
        union {
            uint32_t R;
            struct {
                uint32_t:30;
                uint32_t DBG:2;
            } B;
        } MCR;                  /* Module Configuration Register */

        uint32_t eqadc_reserved0;

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
                uint32_t CF_PUSH:32;
            } B;
        } CFPR[6];              /* CFIFO Push Registers */

        uint32_t eqadc_reserved1;

        uint32_t eqadc_reserved2;

        union {
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t RF_POP:16;
            } B;
        } RFPR[6];              /* Result FIFO Pop Registers */

        uint32_t eqadc_reserved3;

        uint32_t eqadc_reserved4;

        union {
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

        union {
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

        union {
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
                uint16_t TC_CF:11;
            } B;
        } CFTCR[6];             /* CFIFO Transfer Counter Registers */

        uint32_t eqadc_reserved9;

        union {
            uint32_t R;
            struct {
                uint32_t CFS0_T0:2;
                uint32_t CFS1_T0:2;
                uint32_t CFS2_T0:2;
                uint32_t CFS3_T0:2;
                uint32_t CFS4_T0:2;
                uint32_t CFS5_T0:2;
                  uint32_t:5;
                uint32_t LCFT0:4;
                uint32_t TC_LCFT0:11;
            } B;
        } CFSSR0;               /* CFIFO Status Register 0 */

        uint32_t eqadc_reserved10[2];

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

        uint32_t eqadc_reserved11[20];

        struct {
            union {
                uint32_t R;
                struct {
                    uint32_t:32;
                } B;
            } R[4];

            uint32_t eqadc_reserved12[12];

        } CF[6];

        uint32_t eqadc_reserved13[32];

        struct {
            union {
                uint32_t R;
                struct {
                    uint32_t:32;
                } B;
            } R[4];

            uint32_t eqadc_reserved14[12];

        } RF[6];

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
                  uint32_t:1;
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
                uint16_t TXDIR:1;
                uint16_t BESM13:1;
                uint16_t SBSTP:1;
                  uint16_t:1;
                uint16_t PMSK:1;
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
                  uint32_t:6;
                uint32_t UREQ:1;
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
                  uint32_t:6;
                uint32_t UQIE:1;
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
/*                     MODULE : FLASH                                       */
/****************************************************************************/
    struct FLASH_tag {
        union {                 /* Module Configuration Register */
            uint32_t R;
            struct {
                uint32_t:3;
                uint32_t SFS:1;
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
                  uint32_t:1;
                uint32_t PRD:1;
                uint32_t STOP:1;
                  uint32_t:1;
                uint32_t PGM:1;
                uint32_t PSUS:1;
                uint32_t ERS:1;
                uint32_t ESUS:1;
                uint32_t EHV:1;
            } B;
        } MCR;

        union LMLR_tag {        /* Low/Mid Address Space Block Locking Register */
            uint32_t R;
            struct {
                uint32_t LME:1;
                  uint32_t:10;
                uint32_t SLOCK:1;
                  uint32_t:2;
                uint32_t MLOCK:2;
                  uint32_t:6;
                uint32_t LLOCK:10;
            } B;
        } LMLR; /* Legacy naming - refer to LML in Reference Manual */

        union HLR_tag {         /* High Address Space Block Locking Register */
            uint32_t R;
            struct {
                uint32_t HBE:1;
                  uint32_t:23;
                uint32_t HBLOCK:8;
            } B;
        } HLR; /* Legacy naming - refer to HBL in Reference Manual */

        union SLMLR_tag {       /* Secondary Low/Mid Block Locking Register */
            uint32_t R;
            struct {
                uint32_t SLE:1;
                  uint32_t:10;
                uint32_t SSLOCK:1;
                  uint32_t:2;
                uint32_t SMLOCK:2;
                  uint32_t:6;
                uint32_t SLLOCK:10;
            } B;
        } SLMLR; /* Legacy naming - refer to SLL in Reference Manual */

        union {                 /* Low/Mid Address Space Block Select Register */
            uint32_t R;
            struct {
                uint32_t:14;
                uint32_t MSEL:2;
                  uint32_t:6;
                uint32_t LSEL:10;
            } B;
        } LMSR; /* Legacy naming - refer to LMS in Reference Manual */

        union {                 /* High Address Space Block Select Register */
            uint32_t R;
            struct {
                uint32_t:28;
                uint32_t HBSEL:4;
            } B;
        } HSR; /* Legacy naming - refer to HBS in Reference Manual */

        union {                 /* Address Register */
            uint32_t R;
            struct {
                uint32_t:10;
                uint32_t ADDR:19;
                  uint32_t:3;
            } B;
        } AR; /* Legacy naming - refer to ADR in Reference Manual */

        union {                 /* Platform Flash Configuration Register for Port 0 */
            uint32_t R;
            struct {
                uint32_t LBCFG:4;
                uint32_t ARB:1;
                uint32_t PRI:1;
                  uint32_t:5;
                uint32_t M4PFE:1;
                uint32_t M3PFE:1;
                uint32_t M2PFE:1;
                uint32_t M1PFE:1;
                uint32_t M0PFE:1;
                uint32_t APC:3;
                uint32_t WWSC:2;
                uint32_t RWSC:3;
                  uint32_t:1;
                uint32_t DPFEN:1;
                  uint32_t:1;
                uint32_t IPFEN:1;
                  uint32_t:1;
                uint32_t PFLIM:2;
                uint32_t BFEN:1;
            } B;
        } PFCRP0;

        union {                 /* Platform Flash Configuration Register for Port 1 */
            uint32_t R;
            struct {
                uint32_t LBCFG:4;
                  uint32_t:7;
                uint32_t M4PFE:1;
                uint32_t M3PFE:1;
                uint32_t M2PFE:1;
                uint32_t M1PFE:1;
                uint32_t M0PFE:1;
                uint32_t APC:3;
                uint32_t WWSC:2;
                uint32_t RWSC:3;
                  uint32_t:1;
                uint32_t DPFEN:1;
                  uint32_t:1;
                uint32_t IPFEN:1;
                  uint32_t:1;
                uint32_t PFLIM:2;
                uint32_t BFEN:1;
            } B;
        } PFCRP1;

    };
/****************************************************************************/
/*                          MODULE : FlexCAN                                */
/****************************************************************************/
    struct FLEXCAN_tag {
        union {
            uint32_t R;
            struct {
                uint32_t MDIS:1;
                uint32_t FRZ:1;
                uint32_t FEN:1;
                uint32_t HALT:1;
                uint32_t NOTRDY:1;
                uint32_t WAKMSK:1;
                uint32_t SOFTRST:1;
                uint32_t FRZACK:1;
                uint32_t SUPV:1;
                uint32_t SLFWAK:1;
                uint32_t WRNEN:1;
                uint32_t LPMACK:1;
                uint32_t WAKSRC:1;
                uint32_t DOZE:1;
                uint32_t SRXDIS:1;
                uint32_t BCC:1;
                  uint32_t:2;
                uint32_t LPRIO_EN:1;
                uint32_t AEN:1;
                  uint32_t:2;
                uint32_t IDAM:2;
                  uint32_t:2;
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
                uint32_t TWRNMSK:1;
                uint32_t RWRNMSK:1;
                  uint32_t:2;
                uint32_t SMP:1;
                uint32_t BOFFREC:1;
                uint32_t TSYN:1;
                uint32_t LBUF:1;
                uint32_t LOM:1;
                uint32_t PROPSEG:3;
            } B;
        } CTRL;                 /* Control Register */

        union {
            uint32_t R;
        } TIMER;                /* Free Running Timer */

        uint32_t FLEXCAN_reserved1;

        union {
            uint32_t R;
            struct {
                uint32_t MI:32;
            } B;
        } RXGMASK;              /* RX Global Mask */

        union {
            uint32_t R;
            struct {
                uint32_t MI:32;
            } B;
        } RX14MASK;             /* RX 14 Mask */

        union {
            uint32_t R;
            struct {
                uint32_t MI:32;
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
                uint32_t TWRNINT:1;
                uint32_t RWRNINT:1;
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
                uint32_t WAKINT:1;
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
        } IMASK2;               /* Interruput Masks Register */

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
        } IMASK1;               /* Interruput Masks Register */

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
        } IFLAG2;               /* Interruput Flag Register */

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
        } IFLAG1;               /* Interruput Flag Register */

        uint32_t FLEXCAN_reserved2[19];

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
                    uint32_t PRIO:3;
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

        uint32_t FLEXCAN_reserved3[256];

        union {
            uint32_t R;
            struct {
                uint32_t MI:32;
            } B;
        } RXIMR[64];            /* RX Individual Mask Registers */

    };
/****************************************************************************/
/*                     MODULE : FMPLL                                       */
/****************************************************************************/
    struct FMPLL_tag {

        uint32_t FMPLL_reserved0;

        union FMPLL_SYNSR_tag { /* Synthesiser Status Register */
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

        union FMPLL_ESYNCR1_tag {
            uint32_t R;
            struct {
                uint32_t:1;
                uint32_t CLKCFG:3;
                  uint32_t:8;
                uint32_t EPREDIV:4;
                  uint32_t:8;
                uint32_t EMFD:8;
            } B;
        } ESYNCR1;

        union FMPLL_ESYNCR2_tag {
            uint32_t R;
            struct {
                uint32_t:8;
                uint32_t LOCEN:1;
                uint32_t LOLRE:1;
                uint32_t LOCRE:1;
                uint32_t LOLIRQ:1;
                uint32_t LOCIRQ:1;
                  uint32_t:1;
                uint32_t ERATE:2;
                  uint32_t:5;
                uint32_t EDEPTH:3;
                  uint32_t:2;
                uint32_t ERFD:6;
            } B;
        } ESYNCR2;

    };
/****************************************************************************/
/*                          MODULE : i2c                                    */
/****************************************************************************/
    struct I2C_tag {
        union {
            uint8_t R;
            struct {
                uint8_t AD:7;
                  uint8_t:1;
            } B;
        } IBAD;                 /* Module Bus Address Register */

        union {
            uint8_t R;
            struct {
                uint8_t MULT:2;
                uint8_t ICR:6;
            } B;
        } IBFD;                 /* Module Bus Frequency Register */

        union {
            uint8_t R;
            struct {
                uint8_t MDIS:1;
                uint8_t IBIE:1;
                uint8_t MS:1;
                uint8_t TX:1;
                uint8_t NOACK:1;
                uint8_t RSTA:1;
                uint8_t DMAEN:1;
                  uint8_t:1;
            } B;
        } IBCR;                 /* Module Bus Control Register */

        union {
            uint8_t R;
            struct {
                uint8_t TCF:1;
                uint8_t IAAS:1;
                uint8_t IBB:1;
                uint8_t IBAL:1;
                  uint8_t:1;
                uint8_t SRW:1;
                uint8_t IBIF:1;
                uint8_t RXAK:1;
            } B;
        } IBSR;                 /* Module Status Register */

        union {
            uint8_t R;
            struct {
                uint8_t DATA:8;
            } B;
        } IBDR;                 /* Module Data Register */

        union {
            uint8_t R;
            struct {
                uint8_t BIIE:1;
                  uint8_t:7;
            } B;
        } IBIC;                 /* Module Interrupt Configuration Register */

    };
/****************************************************************************/
/*                          MODULE : INTC                                   */
/****************************************************************************/
    struct INTC_tag {
        union {
            uint32_t R;
            struct {
                uint32_t:18;
                uint32_t VTES_PRC1:1;
                  uint32_t:4;
                uint32_t HVEN_PRC1:1;
                  uint32_t:2;
                uint32_t VTES:1;
                  uint32_t:4;
                uint32_t HVEN:1;
            } B;
        } MCR;                  /* Module Configuration Register */

        int32_t INTC_reserved1;

        union {
            uint32_t R;
            struct {
                uint32_t:28;
                uint32_t PRI:4;
            } B;
        } CPR;             /* Processor 0 Current Priority Register */

        union {
            uint32_t R;
            struct {
                uint32_t:28;
                uint32_t PRI:4;
            } B;
        } CPR_PRC1;             /* Processor 1 Current Priority Register */

        union {
            uint32_t R;
            struct {
                uint32_t VTBA:21;
                uint32_t INTVEC:9;
                  uint32_t:2;
            } B;
        } IACKR;           /* Processor 0 Interrupt Acknowledge Register */

        union {
            uint32_t R;
            struct {
                uint32_t VTBA_PRC1:21;
                uint32_t INTVEC_PRC1:9;
                  uint32_t:2;
            } B;
        } IACKR_PRC1;           /* Processor 1 Interrupt Acknowledge Register */

        union {
            uint32_t R;
            struct {
                uint32_t:32;
            } B;
        } EOIR;            /* Processor 0 End of Interrupt Register */

        union {
            uint32_t R;
            struct {
                uint32_t:32;
            } B;
        } EOIR_PRC1;            /* Processor 1 End of Interrupt Register */

        union {
            uint8_t R;
            struct {
                uint8_t:6;
                uint8_t SET:1;
                uint8_t CLR:1;
            } B;
        } SSCIR[8];             /* Software Set/Clear Interruput Register */

        uint32_t intc_reserved2[6];

        union {
            uint8_t R;
            struct {
                uint8_t PRC_SEL:2;
                  uint8_t:2;
                uint8_t PRI:4;
            } B;
        } PSR[294];             /* Software Set/Clear Interrupt Register */

    };
/****************************************************************************/
/*                     MODULE : MCM                                         */
/****************************************************************************/
    struct MCM_tag {

        uint32_t mcm_reserved1[5];

        uint16_t mcm_reserved2;

        union {
            uint16_t R;
            struct {
                uint16_t RO:1;
                  uint16_t:6;
                uint16_t SWRWH:1;
                uint16_t SWE:1;
                uint16_t SWRI:2;
                uint16_t SWT:5;
            } B;
        } SWTCR;                /* Software Watchdog Timer Control */

        uint8_t mcm_reserved3[3];

        union {
            uint8_t R;
        } SWTSR;                /* SWT Service Register */

        uint8_t mcm_reserved4[3];

        union {
            uint8_t R;
            struct {
                uint8_t:7;
                uint8_t SWTIC:1;
            } B;
        } SWTIR;                /* SWT Interrupt Register */

        uint32_t mcm_reserved5[1];

        union {
            uint32_t R;
            struct {
                uint32_t PRI:1;
                  uint32_t:31;
            } B;
        } MUDCR;                /* Misc. User Defined Control Register */

        uint32_t mcm_reserved6[6];
        uint8_t mcm_reserved7[3];

        union {
            uint8_t R;
            struct {
                uint8_t:6;
                uint8_t ERNCR:1;
                uint8_t EFNCR:1;
            } B;
        } ECR;                  /* ECC Configuration Register */

        uint8_t mcm_reserved8[3];

        union {
            uint8_t R;
            struct {
                uint8_t:6;
                uint8_t RNCE:1;
                uint8_t FNCE:1;
            } B;
        } ESR;                  /* ECC Status Register */

        uint16_t mcm_reserved9;

        union {
            uint16_t R;
            struct {
                uint16_t:6;
                uint16_t FRCNCI:1;
                uint16_t FR1NCI:1;
                  uint16_t:1;
                uint16_t ERRBIT:7;
            } B;
        } EEGR;                 /* ECC Error Generation Register */

        uint32_t mcm_reserved10;

        union {
            uint32_t R;
        } FEAR;                 /* Flash ECC Address Register */

        uint16_t mcm_reserved11;

        union {
            uint8_t R;
            struct {
                uint8_t:4;
                uint8_t FEMR:4;
            } B;
        } FEMR;                 /* Flash ECC Master Register */

        union {
            uint8_t R;
            struct {
                uint8_t WRITE:1;
                uint8_t SIZE:3;
                uint8_t PROTECTION:4;
            } B;
        } FEAT;                 /* Flash ECC Attributes Register */

        union {
            uint32_t R;
        } FEDRH;                /* Flash ECC Data High Register */

        union {
            uint32_t R;
        } FEDRL;                /* Flash ECC Data Low Register */

        union {
            uint32_t R;
            struct {
                uint32_t REAR:32;
            } B;
        } REAR;                 /* RAM ECC Address */

        uint16_t mcm_reserved12;

        union {
            uint8_t R;
            struct {
                uint8_t:4;
                uint8_t REMR:4;
            } B;
        } REMR;                 /* RAM ECC Master */

        union {
            uint8_t R;
            struct {
                uint8_t WRITE:1;
                uint8_t SIZE:3;
                uint8_t PROTECTION:4;
            } B;
        } REAT;                 /*  RAM ECC Attributes Register */

        union {
            uint32_t R;
        } REDRH;                /* RAM ECC Data High Register */

        union {
            uint32_t R;
        } REDRL;                /* RAMECC Data Low Register */

    };
/****************************************************************************/
/*                          MODULE : MPU                                    */
/****************************************************************************/
    struct MPU_tag {
        union {
            uint32_t R;
            struct {
                uint32_t SPERR:8;
                  uint32_t:4;
                uint32_t HRL:4;
                uint32_t NSP:4;
                uint32_t NGRD:4;
                  uint32_t:7;
                uint32_t VLD:1;
            } B;
        } CESR;                 /* Module Control/Error Status Register */

        uint32_t mpu_reserved1[3];

        union {
            uint32_t R;
            struct {
                uint32_t EADDR:32;
            } B;
        } EAR0;

        union {
            uint32_t R;
            struct {
                uint32_t EACD:16;
                uint32_t EPID:8;
                uint32_t EMN:4;
                uint32_t EATTR:3;
                uint32_t ERW:1;
            } B;
        } EDR0;

        union {
            uint32_t R;
            struct {
                uint32_t EADDR:32;
            } B;
        } EAR1;

        union {
            uint32_t R;
            struct {
                uint32_t EACD:16;
                uint32_t EPID:8;
                uint32_t EMN:4;
                uint32_t EATTR:3;
                uint32_t ERW:1;
            } B;
        } EDR1;

        union {
            uint32_t R;
            struct {
                uint32_t EADDR:32;
            } B;
        } EAR2;

        union {
            uint32_t R;
            struct {
                uint32_t EACD:16;
                uint32_t EPID:8;
                uint32_t EMN:4;
                uint32_t EATTR:3;
                uint32_t ERW:1;
            } B;
        } EDR2;

        uint32_t mpu_reserved2[246];

        struct {
            union {
                uint32_t R;
                struct {
                    uint32_t SRTADDR:27;
                      uint32_t:5;
                } B;
            } WORD0;            /* Region Descriptor n Word 0 */

            union {
                uint32_t R;
                struct {
                    uint32_t ENDADDR:27;
                      uint32_t:5;
                } B;
            } WORD1;            /* Region Descriptor n Word 1 */

            union {
                uint32_t R;
                struct {
                    uint32_t:6;
                    uint32_t M4RE0:1;
                    uint32_t M4WE:1;
                    uint32_t M3PE:1;
                    uint32_t M3SM:2;
                    uint32_t M3UM:3;
                    uint32_t M2PE:1;
                    uint32_t M2SM:2;
                    uint32_t M2UM:3;
                    uint32_t M1PE:1;
                    uint32_t M1SM:2;
                    uint32_t M1UM:3;
                    uint32_t M0PE:1;
                    uint32_t M0SM:2;
                    uint32_t M0UM:3;
                } B;
            } WORD2;            /* Region Descriptor n Word 2 */

            union {
                uint32_t R;
                struct {
                    uint32_t PID:8;
                    uint32_t PIDMASK:8;
                      uint32_t:15;
                    uint32_t VLD:1;
                } B;
            } WORD3;            /* Region Descriptor n Word 3 */

        } RGD[16];

        uint32_t mpu_reserved3[192];

        union {
            uint32_t R;
            struct {
                uint32_t:6;
                uint32_t M4RE:1;
                uint32_t M4WE:1;
                uint32_t M3PE:1;
                uint32_t M3SM:2;
                uint32_t M3UM:3;
                uint32_t M2PE:1;
                uint32_t M2SM:2;
                uint32_t M2UM:3;
                uint32_t M1PE:1;
                uint32_t M1SM:2;
                uint32_t M1UM:3;
                uint32_t M0PE:1;
                uint32_t M0SM:2;
                uint32_t M0UM:3;
            } B;
        } RGDAAC[16];           /* Region Descriptor Alternate Access Control n */
    };
/****************************************************************************/
/*                          MODULE : pit                                    */
/****************************************************************************/
    struct PIT_tag {
        union {
            uint32_t R;
            struct {
                uint32_t TSV:32;
            } B;
        } TLVAL[9];

        uint32_t pit_reserved1[23];

        union {
            uint32_t R;
            struct {
                uint32_t TVL:32;
            } B;
        } TVAL[9];

        uint32_t pit_reserved2[23];

        union {
            uint32_t R;
            struct {
                uint32_t:23;
                uint32_t TIF8:1;
                uint32_t TIF7:1;
                uint32_t TIF6:1;
                uint32_t TIF5:1;
                uint32_t TIF4:1;
                uint32_t TIF3:1;
                uint32_t TIF2:1;
                uint32_t TIF1:1;
                uint32_t RTIF:1;
            } B;
        } PITFLG;

        union {
            uint32_t R;
            struct {
                uint32_t:23;
                uint32_t TIE8:1;
                uint32_t TIE7:1;
                uint32_t TIE6:1;
                uint32_t TIE5:1;
                uint32_t TIE4:1;
                uint32_t TIE3:1;
                uint32_t TIE2:1;
                uint32_t TIE1:1;
                uint32_t RTIE:1;
            } B;
        } PITINTEN;

        union {
            uint32_t R;
            struct {
                uint32_t:23;
                uint32_t ISEL8:1;
                uint32_t ISEL7:1;
                uint32_t ISEL6:1;
                uint32_t ISEL5:1;
                uint32_t ISEL4:1;
                uint32_t ISEL3:1;
                uint32_t ISEL2:1;
                uint32_t ISEL1:1;
                  uint32_t:1;
            } B;
        } PITINTSEL;

        union {
            uint32_t R;
            struct {
                uint32_t:23;
                uint32_t PEN8:1;
                uint32_t PEN7:1;
                uint32_t PEN6:1;
                uint32_t PEN5:1;
                uint32_t PEN4:1;
                uint32_t PEN3:1;
                uint32_t PEN2:1;
                uint32_t PEN1:1;
                uint32_t PEN0:1;
            } B;
        } PITEN;

        union {
            uint32_t R;
            struct {
                uint32_t:7;
                uint32_t MDIS:1;
                  uint32_t:24;
            } B;
        } PITCTRL;

    };
/****************************************************************************/
/*                          MODULE : sem4                                   */
/****************************************************************************/
    struct SEMA4_tag {
        union {
            uint8_t R;
            struct {
                uint8_t:6;
                uint8_t GTFSM:2;
            } B;
        } GATE[16];             /* Gate n Register */

        uint32_t sema4_reserved1[12];   /* {0x40-0x10}/4 = 0x0C */

        union {
            uint16_t R;
            struct {
                uint16_t INE0:1;
                uint16_t INE1:1;
                uint16_t INE2:1;
                uint16_t INE3:1;
                uint16_t INE4:1;
                uint16_t INE5:1;
                uint16_t INE6:1;
                uint16_t INE7:1;
                uint16_t INE8:1;
                uint16_t INE9:1;
                uint16_t INE10:1;
                uint16_t INE11:1;
                uint16_t INE12:1;
                uint16_t INE13:1;
                uint16_t INE14:1;
                uint16_t INE15:1;
            } B;
        } CP0INE;

        uint16_t sema4_reserved2[3];    /* {0x48-0x42}/2 = 0x03 */

        union {
            uint16_t R;
            struct {
                uint16_t INE0:1;
                uint16_t INE1:1;
                uint16_t INE2:1;
                uint16_t INE3:1;
                uint16_t INE4:1;
                uint16_t INE5:1;
                uint16_t INE6:1;
                uint16_t INE7:1;
                uint16_t INE8:1;
                uint16_t INE9:1;
                uint16_t INE10:1;
                uint16_t INE11:1;
                uint16_t INE12:1;
                uint16_t INE13:1;
                uint16_t INE14:1;
                uint16_t INE15:1;
            } B;
        } CP1INE;

        uint16_t sema4_reserved3[27];   /* {0x80-0x4A}/2 = 0x1B */

        union {
            uint16_t R;
            struct {
                uint16_t GN0:1;
                uint16_t GN1:1;
                uint16_t GN2:1;
                uint16_t GN3:1;
                uint16_t GN4:1;
                uint16_t GN5:1;
                uint16_t GN6:1;
                uint16_t GN7:1;
                uint16_t GN8:1;
                uint16_t GN9:1;
                uint16_t GN10:1;
                uint16_t GN11:1;
                uint16_t GN12:1;
                uint16_t GN13:1;
                uint16_t GN14:1;
                uint16_t GN15:1;
            } B;
        } CP0NTF;

        uint16_t sema4_reserved4[3];    /* {0x88-0x82}/2 = 0x03 */

        union {
            uint16_t R;
            struct {
                uint16_t GN0:1;
                uint16_t GN1:1;
                uint16_t GN2:1;
                uint16_t GN3:1;
                uint16_t GN4:1;
                uint16_t GN5:1;
                uint16_t GN6:1;
                uint16_t GN7:1;
                uint16_t GN8:1;
                uint16_t GN9:1;
                uint16_t GN10:1;
                uint16_t GN11:1;
                uint16_t GN12:1;
                uint16_t GN13:1;
                uint16_t GN14:1;
                uint16_t GN15:1;
            } B;
        } CP1NTF;

        uint16_t sema4_reserved5[59];   /* {0x100-0x8A}/2 = 0x3B */

        union {
            uint16_t R;
            struct {
                uint16_t:2;
                uint16_t RSTGSM:2;
                  uint16_t:1;
                uint16_t RSTGMS:3;
                uint16_t RSTGTN:8;
            } B;
        } RSTGT;

        uint16_t sema4_reserved6;

        union {
            uint16_t R;
            struct {
                uint16_t:2;
                uint16_t RSTNSM:2;
                  uint16_t:1;
                uint16_t RSTNMS:3;
                uint16_t RSTNTN:8;
            } B;
        } RSTNTF;
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
                uint32_t CSP:1;
                uint32_t PKG:5;
                  uint32_t:2;
                uint32_t MASKNUM_MAJOR:4;
                uint32_t MASKNUM_MINOR:4;
            } B;
        } MIDR;

        int32_t SIU_reserved1;

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
                  uint32_t:15;
                uint32_t BOOTCFG:1;
                  uint32_t:1;
            } B;
        } RSR;

        union {                 /* System Reset Control Register */
            uint32_t R;
            struct {
                uint32_t SSR:1;
                  uint32_t:15;
                uint32_t CRE0:1;
                uint32_t CRE1:1;
                  uint32_t:6;
                uint32_t SSRL:1;
                  uint32_t:7;
            } B;
        } SRCR;

        union SIU_EISR_tag {    /* External Interrupt Status Register */
            uint32_t R;
            struct {
                uint32_t NMI0:1;
                uint32_t NMI1:1;
                  uint32_t:14;
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
                uint32_t:27;
                uint32_t DIRS4:1;
                uint32_t DIRS3:1;
                uint32_t DIRS2:1;
                uint32_t DIRS1:1;
                  uint32_t:1;
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
                uint32_t NREE0:1;
                uint32_t NREE1:1;
                  uint32_t:14;
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
                uint32_t NFEE0:1;
                uint32_t NFEE1:1;
                  uint32_t:14;
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

        union {                 /* External IRQ Filtered Input Register */
            uint32_t R;
            struct {
                uint32_t FNMI0:1;
                uint32_t FNMI1:1;
                  uint32_t:14;
                uint32_t FI15:1;
                uint32_t FI14:1;
                uint32_t FI13:1;
                uint32_t FI12:1;
                uint32_t FI11:1;
                uint32_t FI10:1;
                uint32_t FI9:1;
                uint32_t FI8:1;
                uint32_t FI7:1;
                uint32_t FI6:1;
                uint32_t FI5:1;
                uint32_t FI4:1;
                uint32_t FI3:1;
                uint32_t FI2:1;
                uint32_t FI1:1;
                uint32_t FI0:1;
            } B;
        } IFIR;

        int32_t SIU_reserved2[2];

        union SIU_PCR_tag {     /* Pad Configuration Registers */
            uint16_t R;
            struct {
                uint16_t:4;
                uint16_t PA:2;
                uint16_t OBE:1;
                uint16_t IBE:1;
                  uint16_t:2;
                uint16_t ODE:1;
                uint16_t HYS:1;
                uint16_t SRC:2;
                uint16_t WPE:1;
                uint16_t WPS:1;
            } B;
        } PCR[146];

        int32_t SIU_reserved3[295];

        union {                 /* GPIO Pin Data Output Registers */
            uint8_t R;
            struct {
                uint8_t:7;
                uint8_t PDO:1;
            } B;
        } GPDO[146];

        int32_t SIU_reserved4[91];

        union {                 /* GPIO Pin Data Input Registers */
            uint8_t R;
            struct {
                uint8_t:7;
                uint8_t PDI:1;
            } B;
        } GPDI[146];

        int32_t SIU_reserved5[27];

        union {                 /* IMUX Register */
            uint32_t R;
            struct {
                uint32_t TSEL3:2;
                uint32_t TSEL2:2;
                uint32_t TSEL1:2;
                uint32_t TSEL0:2;
                  uint32_t:24;
            } B;
        } ISEL0;

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
        } ISEL1;

        union {                 /* IMUX Register */
            uint32_t R;
            struct {
                uint32_t SELEMIOS15:2;
                uint32_t SELEMIOS14:2;
                uint32_t SELEMIOS13:2;
                uint32_t SELEMIOS12:2;
                uint32_t SELEMIOS11:2;
                uint32_t SELEMIOS10:2;
                uint32_t SELEMIOS9:2;
                uint32_t SELEMIOS8:2;
                uint32_t SELEMIOS7:2;
                uint32_t SELEMIOS6:2;
                uint32_t SELEMIOS5:2;
                uint32_t SELEMIOS4:2;
                uint32_t SELEMIOS3:2;
                uint32_t SELEMIOS2:2;
                uint32_t SELEMIOS1:2;
                uint32_t SELEMIOS0:2;
            } B;
        } ISEL2;

        int32_t SIU_reserved6[29];

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
                uint32_t:30;
                uint32_t EBDF:2;
            } B;
        } ECCR;

        union {                 /* Compare A High Register */
            uint32_t R;
        } CMPAH;

        union {                 /* Compare A Low Register */
            uint32_t R;
        } CMPAL;

        union {                 /* Compare B High Register */
            uint32_t R;
        } CMPBH;

        union {                 /* Compare B Low Register */
            uint32_t R;
        } CMPBL;

        int32_t SIU_reserved7[2];

        union {                 /* System CLock Register */
            uint32_t R;
            struct {
                uint32_t SYSCLKSEL:2;
                uint32_t SYSCLKDIV:2;
                uint32_t SWTCLKSEL:1;
                  uint32_t:11;
                uint32_t LPCLKDIV7:2;
                uint32_t LPCLKDIV6:2;
                uint32_t LPCLKDIV5:2;
                uint32_t LPCLKDIV4:2;
                uint32_t LPCLKDIV3:2;
                uint32_t LPCLKDIV2:2;
                uint32_t LPCLKDIV1:2;
                uint32_t LPCLKDIV0:2;
            } B;
        } SYSCLK;

        union {                 /* Halt Register */
            uint32_t R;
        } HLT;

        union {                 /* Halt Acknowledge Register */
            uint32_t R;
        } HLTACK;

        int32_t SIU_reserved8[149];

        union {                 /* Parallel GPIO Pin Data Output Register */
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t PB:16;
            } B;
        } PGPDO0;

        union {                 /* Parallel GPIO Pin Data Output Register */
            uint32_t R;
            struct {
                uint32_t PC:16;
                uint32_t PD:16;
            } B;
        } PGPDO1;

        union {                 /* Parallel GPIO Pin Data Output Register */
            uint32_t R;
            struct {
                uint32_t PE:16;
                uint32_t PF:16;
            } B;
        } PGPDO2;

        union {                 /* Parallel GPIO Pin Data Output Register */
            uint32_t R;
            struct {
                uint32_t PG:16;
                uint32_t PH:16;
            } B;
        } PGPDO3;

        union {                 /* Parallel GPIO Pin Data Output Register */
            uint32_t R;
            struct {
                uint32_t PJ:16;
                  uint32_t:16;
            } B;
        } PGPDO4;

        int32_t SIU_reserved9[11];

        union {                 /* Parallel GPIO Pin Data Input Register */
            uint32_t R;
            struct {
                uint32_t PA:16;
                uint32_t PB:16;
            } B;
        } PGPDI0;

        union {                 /* Parallel GPIO Pin Data Input Register */
            uint32_t R;
            struct {
                uint32_t PC:16;
                uint32_t PD:16;
            } B;
        } PGPDI1;

        union {                 /* Parallel GPIO Pin Data Input Register */
            uint32_t R;
            struct {
                uint32_t PE:16;
                uint32_t PF:16;
            } B;
        } PGPDI2;

        union {                 /* Parallel GPIO Pin Data Input Register */
            uint32_t R;
            struct {
                uint32_t PG:16;
                uint32_t PH:16;
            } B;
        } PGPDI3;

        union {                 /* Parallel GPIO Pin Data Input Register */
            uint32_t R;
            struct {
                uint32_t PJ:16;
                uint32_t PK:2;
                  uint32_t:14;
            } B;
        } PGPDI4;

        int32_t SIU_reserved10[12];

        union {                 /* Masked Parallel GPIO Pin Data Input Register */
            uint32_t R;
            struct {
                uint32_t PB_MASK:16;
                uint32_t PB:16;
            } B;
        } MPGPDO1;

        union {                 /* Masked Parallel GPIO Pin Data Input Register */
            uint32_t R;
            struct {
                uint32_t PC_MASK:16;
                uint32_t PC:16;
            } B;
        } MPGPDO2;

        union {                 /* Masked Parallel GPIO Pin Data Input Register */
            uint32_t R;
            struct {
                uint32_t PD_MASK:16;
                uint32_t PD:16;
            } B;
        } MPGPDO3;

        union {                 /* Masked Parallel GPIO Pin Data Input Register */
            uint32_t R;
            struct {
                uint32_t PE_MASK:16;
                uint32_t PE:16;
            } B;
        } MPGPDO4;

        union {                 /* Masked Parallel GPIO Pin Data Input Register */
            uint32_t R;
            struct {
                uint32_t PF_MASK:16;
                uint32_t PF:16;
            } B;
        } MPGPDO5;

        union {                 /* Masked Parallel GPIO Pin Data Input Register */
            uint32_t R;
            struct {
                uint32_t PG_MASK:16;
                uint32_t PG:16;
            } B;
        } MPGPDO6;

        union {                 /* Masked Parallel GPIO Pin Data Input Register */
            uint32_t R;
            struct {
                uint32_t PH_MASK:16;
                uint32_t PH:16;
            } B;
        } MPGPDO7;

        union {                 /* Masked Parallel GPIO Pin Data Input Register */
            uint32_t R;
            struct {
                uint32_t PJ_MASK:16;
                uint32_t PJ:16;
            } B;
        } MPGPDO8;

    };
/****************************************************************************/
/*                          MODULE : FlexRay                                */
/****************************************************************************/

    typedef union uMVR {
        uint16_t R;
        struct {
            uint16_t CHIVER:8; /* CHI Version Number */
            uint16_t PEVER:8;  /* PE Version Number */
        } B;
    } MVR_t;

    typedef union uMCR {
        uint16_t R;
        struct {
            uint16_t MEN:1;    /* module enable */
              uint16_t:1;
            uint16_t SCMD:1;   /* single channel mode */
            uint16_t CHB:1;    /* channel B enable */
            uint16_t CHA:1;    /* channel A enable */
            uint16_t SFFE:1;   /* synchronization frame filter enable */
              uint16_t:5;
            uint16_t CLKSEL:1; /* protocol engine clock source select */
            uint16_t PRESCALE:3;       /* protocol engine clock prescaler */
              uint16_t:1;
        } B;
    } MCR_t;
    typedef union uSTBSCR {
        uint16_t R;
        struct {
            uint16_t WMD:1;    /* write mode */
            uint16_t STBSSEL:7;        /* strobe signal select */
              uint16_t:3;
            uint16_t ENB:1;    /* strobe signal enable */
              uint16_t:2;
            uint16_t STBPSEL:2;        /* strobe port select */
        } B;
    } STBSCR_t;
    typedef union uMBDSR {
        uint16_t R;
        struct {
            uint16_t:1;
            uint16_t MBSEG2DS:7;       /* message buffer segment 2 data size */
              uint16_t:1;
            uint16_t MBSEG1DS:7;       /* message buffer segment 1 data size */
        } B;
    } MBDSR_t;

    typedef union uMBSSUTR {
        uint16_t R;
        struct {

            uint16_t:2;
            uint16_t LAST_MB_SEG1:6;   /* last message buffer control register for message buffer segment 1 */
              uint16_t:2;
            uint16_t LAST_MB_UTIL:6;   /* last message buffer utilized */
        } B;
    } MBSSUTR_t;

    typedef union uPOCR {
        uint16_t R;
        uint8_t byte[2];
        struct {
            uint16_t WME:1;    /* write mode external correction command */
              uint16_t:3;
            uint16_t EOC_AP:2; /* external offset correction application */
            uint16_t ERC_AP:2; /* external rate correction application */
            uint16_t BSY:1;    /* command write busy / write mode command */
              uint16_t:3;
            uint16_t POCCMD:4; /* protocol command */
        } B;
    } POCR_t;
/* protocol commands */
    typedef union uGIFER {
        uint16_t R;
        struct {
            uint16_t MIF:1;    /* module interrupt flag */
            uint16_t PRIF:1;   /* protocol interrupt flag */
            uint16_t CHIF:1;   /* CHI interrupt flag */
            uint16_t WKUPIF:1; /* wakeup interrupt flag */
            uint16_t FNEBIF:1; /* receive FIFO channel B not empty interrupt flag */
            uint16_t FNEAIF:1; /* receive FIFO channel A not empty interrupt flag */
            uint16_t RBIF:1;   /* receive message buffer interrupt flag */
            uint16_t TBIF:1;   /* transmit buffer interrupt flag */
            uint16_t MIE:1;    /* module interrupt enable */
            uint16_t PRIE:1;   /* protocol interrupt enable */
            uint16_t CHIE:1;   /* CHI interrupt enable */
            uint16_t WKUPIE:1; /* wakeup interrupt enable */
            uint16_t FNEBIE:1; /* receive FIFO channel B not empty interrupt enable */
            uint16_t FNEAIE:1; /* receive FIFO channel A not empty interrupt enable */
            uint16_t RBIE:1;   /* receive message buffer interrupt enable */
            uint16_t TBIE:1;   /* transmit buffer interrupt enable */
        } B;
    } GIFER_t;
    typedef union uPIFR0 {
        uint16_t R;
        struct {
            uint16_t FATLIF:1; /* fatal protocol error interrupt flag */
            uint16_t INTLIF:1; /* internal protocol error interrupt flag */
            uint16_t ILCFIF:1; /* illegal protocol configuration flag */
            uint16_t CSAIF:1;  /* cold start abort interrupt flag */
            uint16_t MRCIF:1;  /* missing rate correctio interrupt flag */
            uint16_t MOCIF:1;  /* missing offset correctio interrupt flag */
            uint16_t CCLIF:1;  /* clock correction limit reached interrupt flag */
            uint16_t MXSIF:1;  /* max sync frames detected interrupt flag */
            uint16_t MTXIF:1;  /* media access test symbol received flag */
            uint16_t LTXBIF:1; /* pdLatestTx violation on channel B interrupt flag */
            uint16_t LTXAIF:1; /* pdLatestTx violation on channel A interrupt flag */
            uint16_t TBVBIF:1; /* Transmission across boundary on channel B Interrupt Flag */
            uint16_t TBVAIF:1; /* Transmission across boundary on channel A Interrupt Flag */
            uint16_t TI2IF:1;  /* timer 2 expired interrupt flag */
            uint16_t TI1IF:1;  /* timer 1 expired interrupt flag */
            uint16_t CYSIF:1;  /* cycle start interrupt flag */
        } B;
    } PIFR0_t;
    typedef union uPIFR1 {
        uint16_t R;
        struct {
            uint16_t EMCIF:1;  /* error mode changed interrupt flag */
            uint16_t IPCIF:1;  /* illegal protocol command interrupt flag */
            uint16_t PECFIF:1; /* protocol engine communication failure interrupt flag */
            uint16_t PSCIF:1;  /* Protocol State Changed Interrupt Flag */
            uint16_t SSI3IF:1; /* slot status counter incremented interrupt flag */
            uint16_t SSI2IF:1; /* slot status counter incremented interrupt flag */
            uint16_t SSI1IF:1; /* slot status counter incremented interrupt flag */
            uint16_t SSI0IF:1; /* slot status counter incremented interrupt flag */
              uint16_t:2;
            uint16_t EVTIF:1;  /* even cycle table written interrupt flag */
            uint16_t ODTIF:1;  /* odd cycle table written interrupt flag */
              uint16_t:4;
        } B;
    } PIFR1_t;
    typedef union uPIER0 {
        uint16_t R;
        struct {
            uint16_t FATLIE:1; /* fatal protocol error interrupt enable */
            uint16_t INTLIE:1; /* internal protocol error interrupt interrupt enable  */
            uint16_t ILCFIE:1; /* illegal protocol configuration interrupt enable */
            uint16_t CSAIE:1;  /* cold start abort interrupt enable */
            uint16_t MRCIE:1;  /* missing rate correctio interrupt enable */
            uint16_t MOCIE:1;  /* missing offset correctio interrupt enable */
            uint16_t CCLIE:1;  /* clock correction limit reached interrupt enable */
            uint16_t MXSIE:1;  /* max sync frames detected interrupt enable */
            uint16_t MTXIE:1;  /* media access test symbol received interrupt enable */
            uint16_t LTXBIE:1; /* pdLatestTx violation on channel B interrupt enable */
            uint16_t LTXAIE:1; /* pdLatestTx violation on channel A interrupt enable */
            uint16_t TBVBIE:1; /* Transmission across boundary on channel B Interrupt enable */
            uint16_t TBVAIE:1; /* Transmission across boundary on channel A Interrupt enable */
            uint16_t TI2IE:1;  /* timer 2 expired interrupt enable */
            uint16_t TI1IE:1;  /* timer 1 expired interrupt enable */
            uint16_t CYSIE:1;  /* cycle start interrupt enable */
        } B;
    } PIER0_t;
    typedef union uPIER1 {
        uint16_t R;
        struct {
            uint16_t EMCIE:1;  /* error mode changed interrupt enable */
            uint16_t IPCIE:1;  /* illegal protocol command interrupt enable */
            uint16_t PECFIE:1; /* protocol engine communication failure interrupt enable */
            uint16_t PSCIE:1;  /* Protocol State Changed Interrupt enable */
            uint16_t SSI3IE:1; /* slot status counter incremented interrupt enable */
            uint16_t SSI2IE:1; /* slot status counter incremented interrupt enable */
            uint16_t SSI1IE:1; /* slot status counter incremented interrupt enable */
            uint16_t SSI0IE:1; /* slot status counter incremented interrupt enable */
              uint16_t:2;
            uint16_t EVTIE:1;  /* even cycle table written interrupt enable */
            uint16_t ODTIE:1;  /* odd cycle table written interrupt enable */
              uint16_t:4;
        } B;
    } PIER1_t;
    typedef union uCHIERFR {
        uint16_t R;
        struct {
            uint16_t FRLBEF:1; /* flame lost channel B error flag */
            uint16_t FRLAEF:1; /* frame lost channel A error flag */
            uint16_t PCMIEF:1; /* command ignored error flag */
            uint16_t FOVBEF:1; /* receive FIFO overrun channel B error flag */
            uint16_t FOVAEF:1; /* receive FIFO overrun channel A error flag */
            uint16_t MSBEF:1;  /* message buffer search error flag */
            uint16_t MBUEF:1;  /* message buffer utilization error flag */
            uint16_t LCKEF:1;  /* lock error flag */
            uint16_t DBLEF:1;  /* double transmit message buffer lock error flag */
            uint16_t SBCFEF:1; /* system bus communication failure error flag */
            uint16_t FIDEF:1;  /* frame ID error flag */
            uint16_t DPLEF:1;  /* dynamic payload length error flag */
            uint16_t SPLEF:1;  /* static payload length error flag */
            uint16_t NMLEF:1;  /* network management length error flag */
            uint16_t NMFEF:1;  /* network management frame error flag */
            uint16_t ILSAEF:1; /* illegal access error flag */
        } B;
    } CHIERFR_t;
    typedef union uMBIVEC {
        uint16_t R;
        struct {

            uint16_t:2;
            uint16_t TBIVEC:6; /* transmit buffer interrupt vector */
              uint16_t:2;
            uint16_t RBIVEC:6; /* receive buffer interrupt vector */
        } B;
    } MBIVEC_t;

    typedef union uPSR0 {
        uint16_t R;
        struct {
            uint16_t ERRMODE:2;        /* error mode */
            uint16_t SLOTMODE:2;       /* slot mode */
              uint16_t:1;
            uint16_t PROTSTATE:3;      /* protocol state */
            uint16_t SUBSTATE:4;       /* protocol sub state */
              uint16_t:1;
            uint16_t WAKEUPSTATUS:3;   /* wakeup status */
        } B;
    } PSR0_t;

/* protocol states */
/* protocol sub-states */
/* wakeup status */
    typedef union uPSR1 {
        uint16_t R;
        struct {
            uint16_t CSAA:1;   /* cold start attempt abort flag */
            uint16_t SCP:1;    /* cold start path */
              uint16_t:1;
            uint16_t REMCSAT:5;        /* remanining coldstart attempts */
            uint16_t CPN:1;    /* cold start noise path */
            uint16_t HHR:1;    /* host halt request pending */
            uint16_t FRZ:1;    /* freeze occured */
            uint16_t APTAC:5;  /* allow passive to active counter */
        } B;
    } PSR1_t;
    typedef union uPSR2 {
        uint16_t R;
        struct {
            uint16_t NBVB:1;   /* NIT boundary violation on channel B */
            uint16_t NSEB:1;   /* NIT syntax error on channel B */
            uint16_t STCB:1;   /* symbol window transmit conflict on channel B */
            uint16_t SBVB:1;   /* symbol window boundary violation on channel B */
            uint16_t SSEB:1;   /* symbol window syntax error on channel B */
            uint16_t MTB:1;    /* media access test symbol MTS received on channel B */
            uint16_t NBVA:1;   /* NIT boundary violation on channel A */
            uint16_t NSEA:1;   /* NIT syntax error on channel A */
            uint16_t STCA:1;   /* symbol window transmit conflict on channel A */
            uint16_t SBVA:1;   /* symbol window boundary violation on channel A */
            uint16_t SSEA:1;   /* symbol window syntax error on channel A */
            uint16_t MTA:1;    /* media access test symbol MTS received on channel A */
            uint16_t CLKCORRFAILCNT:4; /* clock correction failed counter */
        } B;
    } PSR2_t;
    typedef union uPSR3 {
        uint16_t R;
        struct {
            uint16_t:2;
            uint16_t WUB:1;    /* wakeup symbol received on channel B */
            uint16_t ABVB:1;   /* aggregated boundary violation on channel B */
            uint16_t AACB:1;   /* aggregated additional communication on channel B */
            uint16_t ACEB:1;   /* aggregated content error on channel B */
            uint16_t ASEB:1;   /* aggregated syntax error on channel B */
            uint16_t AVFB:1;   /* aggregated valid frame on channel B */
              uint16_t:2;
            uint16_t WUA:1;    /* wakeup symbol received on channel A */
            uint16_t ABVA:1;   /* aggregated boundary violation on channel A */
            uint16_t AACA:1;   /* aggregated additional communication on channel A */
            uint16_t ACEA:1;   /* aggregated content error on channel A */
            uint16_t ASEA:1;   /* aggregated syntax error on channel A */
            uint16_t AVFA:1;   /* aggregated valid frame on channel A */
        } B;
    } PSR3_t;
    typedef union uCIFRR {
        uint16_t R;
        struct {
            uint16_t:8;
            uint16_t MIFR:1;   /* module interrupt flag */
            uint16_t PRIFR:1;  /* protocol interrupt flag */
            uint16_t CHIFR:1;  /* CHI interrupt flag */
            uint16_t WUPIFR:1; /* wakeup interrupt flag */
            uint16_t FNEBIFR:1;        /* receive fifo channel B no empty interrupt flag */
            uint16_t FNEAIFR:1;        /* receive fifo channel A no empty interrupt flag */
            uint16_t RBIFR:1;  /* receive message buffer interrupt flag */
            uint16_t TBIFR:1;  /* transmit buffer interrupt flag */
        } B;
    } CIFRR_t;
    typedef union uSYMATOR {
        uint16_t R;
        struct {
            uint16_t:11;
            uint16_t TIMEOUT:5;        /* system memory time out value */
        } B;
    } SYMATOR_t;

    typedef union uSFCNTR {
        uint16_t R;
        struct {
            uint16_t SFEVB:4;  /* sync frames channel B, even cycle */
            uint16_t SFEVA:4;  /* sync frames channel A, even cycle */
            uint16_t SFODB:4;  /* sync frames channel B, odd cycle */
            uint16_t SFODA:4;  /* sync frames channel A, odd cycle */
        } B;
    } SFCNTR_t;

    typedef union uSFTCCSR {
        uint16_t R;
        struct {
            uint16_t ELKT:1;   /* even cycle tables lock and unlock trigger */
            uint16_t OLKT:1;   /* odd cycle tables lock and unlock trigger */
            uint16_t CYCNUM:6; /* cycle number */
            uint16_t ELKS:1;   /* even cycle tables lock status */
            uint16_t OLKS:1;   /* odd cycle tables lock status */
            uint16_t EVAL:1;   /* even cycle tables valid */
            uint16_t OVAL:1;   /* odd cycle tables valid */
              uint16_t:1;
            uint16_t OPT:1;    /*one pair trigger */
            uint16_t SDVEN:1;  /* sync frame deviation table enable */
            uint16_t SIDEN:1;  /* sync frame ID table enable */
        } B;
    } SFTCCSR_t;
    typedef union uSFIDRFR {
        uint16_t R;
        struct {
            uint16_t:6;
            uint16_t SYNFRID:10;       /* sync frame rejection ID */
        } B;
    } SFIDRFR_t;

    typedef union uTICCR {
        uint16_t R;
        struct {
            uint16_t:2;
            uint16_t T2CFG:1;  /* timer 2 configuration */
            uint16_t T2REP:1;  /* timer 2 repetitive mode */
              uint16_t:1;
            uint16_t T2SP:1;   /* timer 2 stop */
            uint16_t T2TR:1;   /* timer 2 trigger */
            uint16_t T2ST:1;   /* timer 2 state */
              uint16_t:3;
            uint16_t T1REP:1;  /* timer 1 repetitive mode */
              uint16_t:1;
            uint16_t T1SP:1;   /* timer 1 stop */
            uint16_t T1TR:1;   /* timer 1 trigger */
            uint16_t T1ST:1;   /* timer 1 state */

        } B;
    } TICCR_t;
    typedef union uTI1CYSR {
        uint16_t R;
        struct {
            uint16_t:2;
            uint16_t TI1CYCVAL:6;      /* timer 1 cycle filter value */
              uint16_t:2;
            uint16_t TI1CYCMSK:6;      /* timer 1 cycle filter mask */

        } B;
    } TI1CYSR_t;

    typedef union uSSSR {
        uint16_t R;
        struct {
            uint16_t WMD:1;    /* write mode */
              uint16_t:1;
            uint16_t SEL:2;    /* static slot number */
              uint16_t:1;
            uint16_t SLOTNUMBER:11;    /* selector */
        } B;
    } SSSR_t;

    typedef union uSSCCR {
        uint16_t R;
        struct {
            uint16_t WMD:1;    /* write mode */
              uint16_t:1;
            uint16_t SEL:2;    /* selector */
              uint16_t:1;
            uint16_t CNTCFG:2; /* counter configuration */
            uint16_t MCY:1;    /* multi cycle selection */
            uint16_t VFR:1;    /* valid frame selection */
            uint16_t SYF:1;    /* sync frame selection */
            uint16_t NUF:1;    /* null frame selection  */
            uint16_t SUF:1;    /* startup frame selection */
            uint16_t STATUSMASK:4;     /* slot status mask */
        } B;
    } SSCCR_t;
    typedef union uSSR {
        uint16_t R;
        struct {
            uint16_t VFB:1;    /* valid frame on channel B */
            uint16_t SYB:1;    /* valid sync frame on channel B */
            uint16_t NFB:1;    /* valid null frame on channel B */
            uint16_t SUB:1;    /* valid startup frame on channel B */
            uint16_t SEB:1;    /* syntax error on channel B */
            uint16_t CEB:1;    /* content error on channel B */
            uint16_t BVB:1;    /* boundary violation on channel B */
            uint16_t TCB:1;    /* tx conflict on channel B */
            uint16_t VFA:1;    /* valid frame on channel A */
            uint16_t SYA:1;    /* valid sync frame on channel A */
            uint16_t NFA:1;    /* valid null frame on channel A */
            uint16_t SUA:1;    /* valid startup frame on channel A */
            uint16_t SEA:1;    /* syntax error on channel A */
            uint16_t CEA:1;    /* content error on channel A */
            uint16_t BVA:1;    /* boundary violation on channel A */
            uint16_t TCA:1;    /* tx conflict on channel A */
        } B;
    } SSR_t;
    typedef union uMTSCFR {
        uint16_t R;
        struct {
            uint16_t MTE:1;    /* media access test symbol transmission enable */
              uint16_t:1;
            uint16_t CYCCNTMSK:6;      /* cycle counter mask */
              uint16_t:2;
            uint16_t CYCCNTVAL:6;      /* cycle counter value */
        } B;
    } MTSCFR_t;

    typedef union uRSBIR {
        uint16_t R;
        struct {
            uint16_t WMD:1;    /* write mode */
              uint16_t:1;
            uint16_t SEL:2;    /* selector */
              uint16_t:5;
            uint16_t RSBIDX:7; /* receive shadow buffer index */
        } B;
    } RSBIR_t;

    typedef union uRFDSR {
        uint16_t R;
        struct {
            uint16_t FIFODEPTH:8;      /* fifo depth */
              uint16_t:1;
            uint16_t ENTRYSIZE:7;      /* entry size */
        } B;
    } RFDSR_t;

    typedef union uRFRFCFR {
        uint16_t R;
        struct {
            uint16_t WMD:1;    /* write mode */
            uint16_t IBD:1;    /* interval boundary */
            uint16_t SEL:2;    /* filter number */
              uint16_t:1;
            uint16_t SID:11;   /* slot ID */
        } B;
    } RFRFCFR_t;

    typedef union uRFRFCTR {
        uint16_t R;
        struct {
            uint16_t:4;
            uint16_t F3MD:1;   /* filter mode */
            uint16_t F2MD:1;   /* filter mode */
            uint16_t F1MD:1;   /* filter mode */
            uint16_t F0MD:1;   /* filter mode */
              uint16_t:4;
            uint16_t F3EN:1;   /* filter enable */
            uint16_t F2EN:1;   /* filter enable */
            uint16_t F1EN:1;   /* filter enable */
            uint16_t F0EN:1;   /* filter enable */
        } B;
    } RFRFCTR_t;
    typedef union uPCR0 {
        uint16_t R;
        struct {
            uint16_t ACTION_POINT_OFFSET:6;
            uint16_t STATIC_SLOT_LENGTH:10;
        } B;
    } PCR0_t;

    typedef union uPCR1 {
        uint16_t R;
        struct {
            uint16_t:2;
            uint16_t MACRO_AFTER_FIRST_STATIC_SLOT:14;
        } B;
    } PCR1_t;

    typedef union uPCR2 {
        uint16_t R;
        struct {
            uint16_t MINISLOT_AFTER_ACTION_POINT:6;
            uint16_t NUMBER_OF_STATIC_SLOTS:10;
        } B;
    } PCR2_t;

    typedef union uPCR3 {
        uint16_t R;
        struct {
            uint16_t WAKEUP_SYMBOL_RX_LOW:6;
            uint16_t MINISLOT_ACTION_POINT_OFFSET:5;
            uint16_t COLDSTART_ATTEMPTS:5;
        } B;
    } PCR3_t;

    typedef union uPCR4 {
        uint16_t R;
        struct {
            uint16_t CAS_RX_LOW_MAX:7;
            uint16_t WAKEUP_SYMBOL_RX_WINDOW:9;
        } B;
    } PCR4_t;

    typedef union uPCR5 {
        uint16_t R;
        struct {
            uint16_t TSS_TRANSMITTER:4;
            uint16_t WAKEUP_SYMBOL_TX_LOW:6;
            uint16_t WAKEUP_SYMBOL_RX_IDLE:6;
        } B;
    } PCR5_t;

    typedef union uPCR6 {
        uint16_t R;
        struct {
            uint16_t:1;
            uint16_t SYMBOL_WINDOW_AFTER_ACTION_POINT:8;
            uint16_t MACRO_INITIAL_OFFSET_A:7;
        } B;
    } PCR6_t;

    typedef union uPCR7 {
        uint16_t R;
        struct {
            uint16_t DECODING_CORRECTION_B:9;
            uint16_t MICRO_PER_MACRO_NOM_HALF:7;
        } B;
    } PCR7_t;

    typedef union uPCR8 {
        uint16_t R;
        struct {
            uint16_t MAX_WITHOUT_CLOCK_CORRECTION_FATAL:4;
            uint16_t MAX_WITHOUT_CLOCK_CORRECTION_PASSIVE:4;
            uint16_t WAKEUP_SYMBOL_TX_IDLE:8;
        } B;
    } PCR8_t;

    typedef union uPCR9 {
        uint16_t R;
        struct {
            uint16_t MINISLOT_EXISTS:1;
            uint16_t SYMBOL_WINDOW_EXISTS:1;
            uint16_t OFFSET_CORRECTION_OUT:14;
        } B;
    } PCR9_t;

    typedef union uPCR10 {
        uint16_t R;
        struct {
            uint16_t SINGLE_SLOT_ENABLED:1;
            uint16_t WAKEUP_CHANNEL:1;
            uint16_t MACRO_PER_CYCLE:14;
        } B;
    } PCR10_t;

    typedef union uPCR11 {
        uint16_t R;
        struct {
            uint16_t KEY_SLOT_USED_FOR_STARTUP:1;
            uint16_t KEY_SLOT_USED_FOR_SYNC:1;
            uint16_t OFFSET_CORRECTION_START:14;
        } B;
    } PCR11_t;

    typedef union uPCR12 {
        uint16_t R;
        struct {
            uint16_t ALLOW_PASSIVE_TO_ACTIVE:5;
            uint16_t KEY_SLOT_HEADER_CRC:11;
        } B;
    } PCR12_t;

    typedef union uPCR13 {
        uint16_t R;
        struct {
            uint16_t FIRST_MINISLOT_ACTION_POINT_OFFSET:6;
            uint16_t STATIC_SLOT_AFTER_ACTION_POINT:10;
        } B;
    } PCR13_t;

    typedef union uPCR14 {
        uint16_t R;
        struct {
            uint16_t RATE_CORRECTION_OUT:11;
            uint16_t LISTEN_TIMEOUT_H:5;
        } B;
    } PCR14_t;

    typedef union uPCR15 {
        uint16_t R;
        struct {
            uint16_t LISTEN_TIMEOUT_L:16;
        } B;
    } PCR15_t;

    typedef union uPCR16 {
        uint16_t R;
        struct {
            uint16_t MACRO_INITIAL_OFFSET_B:7;
            uint16_t NOISE_LISTEN_TIMEOUT_H:9;
        } B;
    } PCR16_t;

    typedef union uPCR17 {
        uint16_t R;
        struct {
            uint16_t NOISE_LISTEN_TIMEOUT_L:16;
        } B;
    } PCR17_t;

    typedef union uPCR18 {
        uint16_t R;
        struct {
            uint16_t WAKEUP_PATTERN:6;
            uint16_t KEY_SLOT_ID:10;
        } B;
    } PCR18_t;

    typedef union uPCR19 {
        uint16_t R;
        struct {
            uint16_t DECODING_CORRECTION_A:9;
            uint16_t PAYLOAD_LENGTH_STATIC:7;
        } B;
    } PCR19_t;

    typedef union uPCR20 {
        uint16_t R;
        struct {
            uint16_t MICRO_INITIAL_OFFSET_B:8;
            uint16_t MICRO_INITIAL_OFFSET_A:8;
        } B;
    } PCR20_t;

    typedef union uPCR21 {
        uint16_t R;
        struct {
            uint16_t EXTERN_RATE_CORRECTION:3;
            uint16_t LATEST_TX:13;
        } B;
    } PCR21_t;

    typedef union uPCR22 {
        uint16_t R;
        struct {
            uint16_t:1;
            uint16_t COMP_ACCEPTED_STARTUP_RANGE_A:11;
            uint16_t MICRO_PER_CYCLE_H:4;
        } B;
    } PCR22_t;

    typedef union uPCR23 {
        uint16_t R;
        struct {
            uint16_t micro_per_cycle_l:16;
        } B;
    } PCR23_t;

    typedef union uPCR24 {
        uint16_t R;
        struct {
            uint16_t CLUSTER_DRIFT_DAMPING:5;
            uint16_t MAX_PAYLOAD_LENGTH_DYNAMIC:7;
            uint16_t MICRO_PER_CYCLE_MIN_H:4;
        } B;
    } PCR24_t;

    typedef union uPCR25 {
        uint16_t R;
        struct {
            uint16_t MICRO_PER_CYCLE_MIN_L:16;
        } B;
    } PCR25_t;

    typedef union uPCR26 {
        uint16_t R;
        struct {
            uint16_t ALLOW_HALT_DUE_TO_CLOCK:1;
            uint16_t COMP_ACCEPTED_STARTUP_RANGE_B:11;
            uint16_t MICRO_PER_CYCLE_MAX_H:4;
        } B;
    } PCR26_t;

    typedef union uPCR27 {
        uint16_t R;
        struct {
            uint16_t MICRO_PER_CYCLE_MAX_L:16;
        } B;
    } PCR27_t;

    typedef union uPCR28 {
        uint16_t R;
        struct {
            uint16_t DYNAMIC_SLOT_IDLE_PHASE:2;
            uint16_t MACRO_AFTER_OFFSET_CORRECTION:14;
        } B;
    } PCR28_t;

    typedef union uPCR29 {
        uint16_t R;
        struct {
            uint16_t EXTERN_OFFSET_CORRECTION:3;
            uint16_t MINISLOTS_MAX:13;
        } B;
    } PCR29_t;

    typedef union uPCR30 {
        uint16_t R;
        struct {
            uint16_t:12;
            uint16_t SYNC_NODE_MAX:4;
        } B;
    } PCR30_t;

    typedef struct uMSG_BUFF_CCS {
        union {
            uint16_t R;
            struct {
                uint16_t:1;
                uint16_t MCM:1;        /* message buffer commit mode */
                uint16_t MBT:1;        /* message buffer type */
                uint16_t MTD:1;        /* message buffer direction */
                uint16_t CMT:1;        /* commit for transmission */
                uint16_t EDT:1;        /* enable / disable trigger */
                uint16_t LCKT:1;       /* lock request trigger */
                uint16_t MBIE:1;       /* message buffer interrupt enable */
                  uint16_t:3;
                uint16_t DUP:1;        /* data updated  */
                uint16_t DVAL:1;       /* data valid */
                uint16_t EDS:1;        /* lock status */
                uint16_t LCKS:1;       /* enable / disable status */
                uint16_t MBIF:1;       /* message buffer interrupt flag */
            } B;
        } MBCCSR;
        union {
            uint16_t R;
            struct {
                uint16_t MTM:1;        /* message buffer transmission mode */
                uint16_t CHNLA:1;      /* channel assignement */
                uint16_t CHNLB:1;      /* channel assignement */
                uint16_t CCFE:1;       /* cycle counter filter enable */
                uint16_t CCFMSK:6;     /* cycle counter filter mask */
                uint16_t CCFVAL:6;     /* cycle counter filter value */
            } B;
        } MBCCFR;
        union {
            uint16_t R;
            struct {
                uint16_t:5;
                uint16_t FID:11;       /* frame ID */
            } B;
        } MBFIDR;

        union {
            uint16_t R;
            struct {
                uint16_t:9;
                uint16_t MBIDX:7;      /* message buffer index */
            } B;
        } MBIDXR;
    } MSG_BUFF_CCS_t;
    typedef union uSYSBADHR {
        uint16_t R;
    } SYSBADHR_t;
    typedef union uSYSBADLR {
        uint16_t R;
    } SYSBADLR_t;
    typedef union uPADR {
        uint16_t R;
    } PADR_t;
    typedef union uPDAR {
        uint16_t R;
    } PDAR_t;
    typedef union uCASERCR {
        uint16_t R;
    } CASERCR_t;
    typedef union uCBSERCR {
        uint16_t R;
    } CBSERCR_t;
    typedef union uCYCTR {
        uint16_t R;
    } CYCTR_t;
    typedef union uMTCTR {
        uint16_t R;
    } MTCTR_t;
    typedef union uSLTCTAR {
        uint16_t R;
    } SLTCTAR_t;
    typedef union uSLTCTBR {
        uint16_t R;
    } SLTCTBR_t;
    typedef union uRTCORVR {
        uint16_t R;
    } RTCORVR_t;
    typedef union uOFCORVR {
        uint16_t R;
    } OFCORVR_t;
    typedef union uSFTOR {
        uint16_t R;
    } SFTOR_t;
    typedef union uSFIDAFVR {
        uint16_t R;
    } SFIDAFVR_t;
    typedef union uSFIDAFMR {
        uint16_t R;
    } SFIDAFMR_t;
    typedef union uNMVR {
        uint16_t R;
    } NMVR_t;
    typedef union uNMVLR {
        uint16_t R;
    } NMVLR_t;
    typedef union uT1MTOR {
        uint16_t R;
    } T1MTOR_t;
    typedef union uTI2CR0 {
        uint16_t R;
    } TI2CR0_t;
    typedef union uTI2CR1 {
        uint16_t R;
    } TI2CR1_t;
    typedef union uSSCR {
        uint16_t R;
    } SSCR_t;
    typedef union uRFSR {
        uint16_t R;
    } RFSR_t;
    typedef union uRFSIR {
        uint16_t R;
    } RFSIR_t;
    typedef union uRFARIR {
        uint16_t R;
    } RFARIR_t;
    typedef union uRFBRIR {
        uint16_t R;
    } RFBRIR_t;
    typedef union uRFMIDAFVR {
        uint16_t R;
    } RFMIDAFVR_t;
    typedef union uRFMIAFMR {
        uint16_t R;
    } RFMIAFMR_t;
    typedef union uRFFIDRFVR {
        uint16_t R;
    } RFFIDRFVR_t;
    typedef union uRFFIDRFMR {
        uint16_t R;
    } RFFIDRFMR_t;
    typedef union uLDTXSLAR {
        uint16_t R;
    } LDTXSLAR_t;
    typedef union uLDTXSLBR {
        uint16_t R;
    } LDTXSLBR_t;

    typedef struct FR_tag {
        volatile MVR_t MVR;     /*module version register *//*0  */
        volatile MCR_t MCR;     /*module configuration register *//*2  */
        volatile SYSBADHR_t SYSBADHR;   /*system memory base address high register *//*4        */
        volatile SYSBADLR_t SYSBADLR;   /*system memory base address low register *//*6         */
        volatile STBSCR_t STBSCR;       /*strobe signal control register *//*8      */
        uint16_t reserved0[1]; /*A    */
        volatile MBDSR_t MBDSR; /*message buffer data size register *//*C  */
        volatile MBSSUTR_t MBSSUTR;     /*message buffer segment size and utilization register *//*E  */
        uint16_t reserved1[1]; /*10 */
        uint16_t reserved2[1]; /*12 */
        volatile POCR_t POCR;   /*Protocol operation control register *//*14 */
        volatile GIFER_t GIFER; /*global interrupt flag and enable register *//*16 */
        volatile PIFR0_t PIFR0; /*protocol interrupt flag register 0 *//*18 */
        volatile PIFR1_t PIFR1; /*protocol interrupt flag register 1 *//*1A */
        volatile PIER0_t PIER0; /*protocol interrupt enable register 0 *//*1C */
        volatile PIER1_t PIER1; /*protocol interrupt enable register 1 *//*1E */
        volatile CHIERFR_t CHIERFR;     /*CHI error flag register *//*20 */
        volatile MBIVEC_t MBIVEC;       /*message buffer interrupt vector register *//*22 */
        volatile CASERCR_t CASERCR;     /*channel A status error counter register *//*24 */
        volatile CBSERCR_t CBSERCR;     /*channel B status error counter register *//*26 */
        volatile PSR0_t PSR0;   /*protocol status register 0 *//*28 */
        volatile PSR1_t PSR1;   /*protocol status register 1 *//*2A */
        volatile PSR2_t PSR2;   /*protocol status register 2 *//*2C */
        volatile PSR3_t PSR3;   /*protocol status register 3 *//*2E */
        volatile MTCTR_t MTCTR; /*macrotick counter register *//*30 */
        volatile CYCTR_t CYCTR; /*cycle counter register *//*32 */
        volatile SLTCTAR_t SLTCTAR;     /*slot counter channel A register *//*34 */
        volatile SLTCTBR_t SLTCTBR;     /*slot counter channel B register *//*36 */
        volatile RTCORVR_t RTCORVR;     /*rate correction value register *//*38 */
        volatile OFCORVR_t OFCORVR;     /*offset correction value register *//*3A */
        volatile CIFRR_t CIFRR; /*combined interrupt flag register *//*3C */
        volatile SYMATOR_t SYMATOR;     /*system memory acess time-out register *//*3E */
        volatile SFCNTR_t SFCNTR;       /*sync frame counter register *//*40 */
        volatile SFTOR_t SFTOR; /*sync frame table offset register *//*42 */
        volatile SFTCCSR_t SFTCCSR;     /*sync frame table configuration, control, status register *//*44 */
        volatile SFIDRFR_t SFIDRFR;     /*sync frame ID rejection filter register *//*46 */
        volatile SFIDAFVR_t SFIDAFVR;   /*sync frame ID acceptance filter value regiater *//*48 */
        volatile SFIDAFMR_t SFIDAFMR;   /*sync frame ID acceptance filter mask register *//*4A */
        volatile NMVR_t NMVR[6];        /*network management vector registers (12 bytes) *//*4C */
        volatile NMVLR_t NMVLR; /*network management vector length register *//*58 */
        volatile TICCR_t TICCR; /*timer configuration and control register *//*5A */
        volatile TI1CYSR_t TI1CYSR;     /*timer 1 cycle set register *//*5C */
        volatile T1MTOR_t T1MTOR;       /*timer 1 macrotick offset register *//*5E */
        volatile TI2CR0_t TI2CR0;       /*timer 2 configuration register 0 *//*60 */
        volatile TI2CR1_t TI2CR1;       /*timer 2 configuration register 1 *//*62 */
        volatile SSSR_t SSSR;   /*slot status selection register *//*64 */
        volatile SSCCR_t SSCCR; /*slot status counter condition register *//*66 */
        volatile SSR_t SSR[8];  /*slot status registers 0-7 *//*68 */
        volatile SSCR_t SSCR[4];        /*slot status counter registers 0-3 *//*78 */
        volatile MTSCFR_t MTSACFR;      /*mts a config register *//*80 */
        volatile MTSCFR_t MTSBCFR;      /*mts b config register *//*82 */
        volatile RSBIR_t RSBIR; /*receive shadow buffer index register *//*84 */
        volatile RFSR_t RFSR;   /*receive fifo selection register *//*86 */
        volatile RFSIR_t RFSIR; /*receive fifo start index register *//*88 */
        volatile RFDSR_t RFDSR; /*receive fifo depth and size register *//*8A */
        volatile RFARIR_t RFARIR;       /*receive fifo a read index register *//*8C */
        volatile RFBRIR_t RFBRIR;       /*receive fifo b read index register *//*8E */
        volatile RFMIDAFVR_t RFMIDAFVR; /*receive fifo message ID acceptance filter value register *//*90 */
        volatile RFMIAFMR_t RFMIAFMR;   /*receive fifo message ID acceptance filter mask register *//*92 */
        volatile RFFIDRFVR_t RFFIDRFVR; /*receive fifo frame ID rejection filter value register *//*94 */
        volatile RFFIDRFMR_t RFFIDRFMR; /*receive fifo frame ID rejection filter mask register *//*96 */
        volatile RFRFCFR_t RFRFCFR;     /*receive fifo range filter configuration register *//*98 */
        volatile RFRFCTR_t RFRFCTR;     /*receive fifo range filter control register *//*9A */
        volatile LDTXSLAR_t LDTXSLAR;   /*last dynamic transmit slot channel A register *//*9C */
        volatile LDTXSLBR_t LDTXSLBR;   /*last dynamic transmit slot channel B register *//*9E */
        volatile PCR0_t PCR0;   /*protocol configuration register 0 *//*A0 */
        volatile PCR1_t PCR1;   /*protocol configuration register 1 *//*A2 */
        volatile PCR2_t PCR2;   /*protocol configuration register 2 *//*A4 */
        volatile PCR3_t PCR3;   /*protocol configuration register 3 *//*A6 */
        volatile PCR4_t PCR4;   /*protocol configuration register 4 *//*A8 */
        volatile PCR5_t PCR5;   /*protocol configuration register 5 *//*AA */
        volatile PCR6_t PCR6;   /*protocol configuration register 6 *//*AC */
        volatile PCR7_t PCR7;   /*protocol configuration register 7 *//*AE */
        volatile PCR8_t PCR8;   /*protocol configuration register 8 *//*B0 */
        volatile PCR9_t PCR9;   /*protocol configuration register 9 *//*B2 */
        volatile PCR10_t PCR10; /*protocol configuration register 10 *//*B4 */
        volatile PCR11_t PCR11; /*protocol configuration register 11 *//*B6 */
        volatile PCR12_t PCR12; /*protocol configuration register 12 *//*B8 */
        volatile PCR13_t PCR13; /*protocol configuration register 13 *//*BA */
        volatile PCR14_t PCR14; /*protocol configuration register 14 *//*BC */
        volatile PCR15_t PCR15; /*protocol configuration register 15 *//*BE */
        volatile PCR16_t PCR16; /*protocol configuration register 16 *//*C0 */
        volatile PCR17_t PCR17; /*protocol configuration register 17 *//*C2 */
        volatile PCR18_t PCR18; /*protocol configuration register 18 *//*C4 */
        volatile PCR19_t PCR19; /*protocol configuration register 19 *//*C6 */
        volatile PCR20_t PCR20; /*protocol configuration register 20 *//*C8 */
        volatile PCR21_t PCR21; /*protocol configuration register 21 *//*CA */
        volatile PCR22_t PCR22; /*protocol configuration register 22 *//*CC */
        volatile PCR23_t PCR23; /*protocol configuration register 23 *//*CE */
        volatile PCR24_t PCR24; /*protocol configuration register 24 *//*D0 */
        volatile PCR25_t PCR25; /*protocol configuration register 25 *//*D2 */
        volatile PCR26_t PCR26; /*protocol configuration register 26 *//*D4 */
        volatile PCR27_t PCR27; /*protocol configuration register 27 *//*D6 */
        volatile PCR28_t PCR28; /*protocol configuration register 28 *//*D8 */
        volatile PCR29_t PCR29; /*protocol configuration register 29 *//*DA */
        volatile PCR30_t PCR30; /*protocol configuration register 30 *//*DC */
        uint16_t reserved3[17];
        volatile MSG_BUFF_CCS_t MBCCS[128];     /* message buffer configuration, control & status registers 0-31 *//*100 */
    } FR_tag_t;

    typedef union uF_HEADER     /* frame header */
    {
        struct {
            uint16_t:5;
            uint16_t HDCRC:11; /* Header CRC */
              uint16_t:2;
            uint16_t CYCCNT:6; /* Cycle Count */
              uint16_t:1;
            uint16_t PLDLEN:7; /* Payload Length */
              uint16_t:1;
            uint16_t PPI:1;    /* Payload Preamble Indicator */
            uint16_t NUF:1;    /* Null Frame Indicator */
            uint16_t SYF:1;    /* Sync Frame Indicator */
            uint16_t SUF:1;    /* Startup Frame Indicator */
            uint16_t FID:11;   /* Frame ID */
        } B;
        uint16_t WORDS[3];
    } F_HEADER_t;
    typedef union uS_STSTUS     /* slot status */
    {
        struct {
            uint16_t VFB:1;    /* Valid Frame on channel B */
            uint16_t SYB:1;    /* Sync Frame Indicator channel B */
            uint16_t NFB:1;    /* Null Frame Indicator channel B */
            uint16_t SUB:1;    /* Startup Frame Indicator channel B */
            uint16_t SEB:1;    /* Syntax Error on channel B */
            uint16_t CEB:1;    /* Content Error on channel B */
            uint16_t BVB:1;    /* Boundary Violation on channel B */
            uint16_t CH:1;     /* Channel */
            uint16_t VFA:1;    /* Valid Frame on channel A */
            uint16_t SYA:1;    /* Sync Frame Indicator channel A */
            uint16_t NFA:1;    /* Null Frame Indicator channel A */
            uint16_t SUA:1;    /* Startup Frame Indicator channel A */
            uint16_t SEA:1;    /* Syntax Error on channel A */
            uint16_t CEA:1;    /* Content Error on channel A */
            uint16_t BVA:1;    /* Boundary Violation on channel A */
              uint16_t:1;
        } RX;
        struct {
            uint16_t VFB:1;    /* Valid Frame on channel B */
            uint16_t SYB:1;    /* Sync Frame Indicator channel B */
            uint16_t NFB:1;    /* Null Frame Indicator channel B */
            uint16_t SUB:1;    /* Startup Frame Indicator channel B */
            uint16_t SEB:1;    /* Syntax Error on channel B */
            uint16_t CEB:1;    /* Content Error on channel B */
            uint16_t BVB:1;    /* Boundary Violation on channel B */
            uint16_t TCB:1;    /* Tx Conflict on channel B */
            uint16_t VFA:1;    /* Valid Frame on channel A */
            uint16_t SYA:1;    /* Sync Frame Indicator channel A */
            uint16_t NFA:1;    /* Null Frame Indicator channel A */
            uint16_t SUA:1;    /* Startup Frame Indicator channel A */
            uint16_t SEA:1;    /* Syntax Error on channel A */
            uint16_t CEA:1;    /* Content Error on channel A */
            uint16_t BVA:1;    /* Boundary Violation on channel A */
            uint16_t TCA:1;    /* Tx Conflict on channel A */
        } TX;
        uint16_t R;
    } S_STATUS_t;

    typedef struct uMB_HEADER   /* message buffer header */
    {
        F_HEADER_t FRAME_HEADER;
        uint16_t DATA_OFFSET;
        S_STATUS_t SLOT_STATUS;
    } MB_HEADER_t;
/****************************************************************************/
/*                     MODULE : MLB                                         */
/****************************************************************************/
    struct MLB_tag {

        union {                 /* MLB Module Configuration Register */
            uint32_t R;
            struct {
                uint32_t MDIS:1;
                  uint32_t:15;
                uint32_t MDATOBSE:1;
                uint32_t MSIGOBS:1;
                uint32_t MSLOTE:1;
                  uint32_t:2;
                uint32_t MSVRQIE:1;
                uint32_t MDATRQE:1;
                  uint32_t:2;
                uint32_t MSVRQDL:3;
                uint32_t MSVRQCIE:1;
                uint32_t MIFSEL:1;
                uint32_t MSBFEPOL:1;
                uint32_t MDBFEPOL:1;
            } B;
        } MCR;

        union {                 /* MLB Blank Register */
            uint32_t R;
            struct {
                uint32_t:31;
                uint32_t BLANK:1;
            } B;
        } MBR;

        union {                 /* MLB Status Register */
            uint32_t R;
            struct {
                uint32_t:29;
                uint32_t MDATRQS:1;
                uint32_t MSYSS:1;
                uint32_t MSVRQS:1;
            } B;
        } MSR;

        union {                 /* RX Control Channel Address Register */
            uint32_t R;
            struct {
                uint32_t RXCCHA_ACEN:1;
                  uint32_t:25;
                uint32_t RXCCHA:5;
                  uint32_t:1;
            } B;
        } RXCCHAR;

        union {                 /* RX Async Channel Address Register */
            uint32_t R;
            struct {
                uint32_t RXACHA_ACEN:1;
                  uint32_t:25;
                uint32_t RXACHA:5;
                  uint32_t:1;
            } B;
        } RXACHAR;

        union {                 /* TX Control Channel Address Register */
            uint32_t R;
            struct {
                uint32_t TXCCHA_ACEN:1;
                  uint32_t:25;
                uint32_t TXCCHA:5;
                  uint32_t:1;
            } B;
        } TXCCHAR;

        union {                 /* TX Async Channel Address Register */
            uint32_t R;
            struct {
                uint32_t TXACHA_ACEN:1;
                  uint32_t:25;
                uint32_t TXACHA:5;
                  uint32_t:1;
            } B;
        } TXACHAR;

        union {                 /* TX Sync Channel Address Register */
            uint32_t R;
            struct {
                uint32_t TXSCHA_ACEN:1;
                  uint32_t:25;
                uint32_t TXSCHA:5;
                  uint32_t:1;
            } B;
        } TXSCHAR;

        union {                 /* TX Sync Channel Address Mask Register */
            uint32_t R;
            struct {
                uint32_t:26;
                uint32_t TXSCHAM:5;
                  uint32_t:1;
            } B;
        } TXSCHAMR;

        union {                 /* Clock Adjust Control Register */
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t PDLY:16;
            } B;
        } CLKACR;

        union {                 /* RX Isochronous Channel Address Register */
            uint32_t R;
            struct {
                uint32_t RXICHA_ACEN:1;
                  uint32_t:25;
                uint32_t RXICHA:5;
                  uint32_t:1;
            } B;
        } RXICHAR;

        union {                 /* TX Isochronous Channel Address Register */
            uint32_t R;
            struct {
                uint32_t TXICHA_ACEN:1;
                  uint32_t:25;
                uint32_t TXICHA:5;
                  uint32_t:1;
            } B;
        } TXICHAR;

    };

/* Define memories */

#define SRAM_START   0x40000000UL
#define SRAM_SIZE       0x14000UL
#define SRAM_END     0x40013FFFUL

#define FLASH_START         0x0UL
#define FLASH_SIZE     0x180000UL
#define FLASH_END      0x17FFFFUL

/* Define instances of modules */
#define SEMA4     (*(volatile struct SEMA4_tag *)     0xFFF10000UL)
#define MPU       (*(volatile struct MPU_tag *)       0xFFF14000UL)
#define MCM       (*(volatile struct MCM_tag *)       0xFFF40000UL)
#define EDMA      (*(volatile struct EDMA_tag *)      0xFFF44000UL)
#define INTC      (*(volatile struct INTC_tag *)      0xFFF48000UL)
#define EQADC     (*(volatile struct EQADC_tag *)     0xFFF80000UL)
#define MLB       (*(volatile struct MLB_tag *)       0xFFF84000UL)
#define I2C       (*(volatile struct I2C_tag *)       0xFFF88000UL)
#define DSPI_A    (*(volatile struct DSPI_tag *)      0xFFF90000UL)
#define DSPI_B    (*(volatile struct DSPI_tag *)      0xFFF94000UL)
#define DSPI_C    (*(volatile struct DSPI_tag *)      0xFFF98000UL)
#define DSPI_D    (*(volatile struct DSPI_tag *)      0xFFF9C000UL)
#define ESCI_A    (*(volatile struct ESCI_tag *)      0xFFFA0000UL)
#define ESCI_B    (*(volatile struct ESCI_tag *)      0xFFFA4000UL)
#define ESCI_C    (*(volatile struct ESCI_tag *)      0xFFFA8000UL)
#define ESCI_D    (*(volatile struct ESCI_tag *)      0xFFFAC000UL)
#define ESCI_E    (*(volatile struct ESCI_tag *)      0xFFFB0000UL)
#define ESCI_F    (*(volatile struct ESCI_tag *)      0xFFFB4000UL)
#define ESCI_G    (*(volatile struct ESCI_tag *)      0xFFFB8000UL)
#define ESCI_H    (*(volatile struct ESCI_tag *)      0xFFFBC000UL)
#define CAN_A     (*(volatile struct FLEXCAN_tag *)   0xFFFC0000UL)
#define CAN_B     (*(volatile struct FLEXCAN_tag *)   0xFFFC4000UL)
#define CAN_C     (*(volatile struct FLEXCAN_tag *)   0xFFFC8000UL)
#define CAN_D     (*(volatile struct FLEXCAN_tag *)   0xFFFCC000UL)
#define CAN_E     (*(volatile struct FLEXCAN_tag *)   0xFFFD0000UL)
#define CAN_F     (*(volatile struct FLEXCAN_tag *)   0xFFFD4000UL)
#define FR        (*(volatile struct FR_tag *)        0xFFFD8000UL)
#define DMAMUX    (*(volatile struct DMAMUX_tag *)    0xFFFDC000UL)
#define PIT       (*(volatile struct PIT_tag *)       0xFFFE0000UL)
#define EMIOS     (*(volatile struct EMIOS_tag *)     0xFFFE4000UL)
#define SIU       (*(volatile struct SIU_tag *)       0xFFFE8000UL)
#define CRP       (*(volatile struct CRP_tag *)       0xFFFEC000UL)
#define FMPLL     (*(volatile struct FMPLL_tag *)     0xFFFF0000UL)
#define EBI       (*(volatile struct EBI_tag *)       0xFFFF4000UL)
#define FLASH     (*(volatile struct FLASH_tag *)     0xFFFF8000UL)

#ifdef __MWERKS__
#pragma pop
#endif

#ifdef  __cplusplus
}
#endif
#endif /* ASM */
#endif                          /* ifdef _MPC5510_H */
/*********************************************************************
 *
 * Copyright:
 *  Freescale Semiconductor, INC. All Rights Reserved.
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

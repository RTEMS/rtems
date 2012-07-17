/**
 * @file
 *
 * @ingroup mpc55xx
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

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

#ifndef LIBCPU_POWERPC_MPC55XX_REGS_EDMA_H
#define LIBCPU_POWERPC_MPC55XX_REGS_EDMA_H

#include <stdint.h>

#include <bspopts.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************************************************/
/*                          MODULE : eDMA                                   */
/****************************************************************************/
    struct EDMA_tag {
        union EDMA_CR_tag {
            uint32_t R;
            struct {
#if MPC55XX_CHIP_TYPE / 10 == 567
                uint32_t:14;
                uint32_t CX:1;
                uint32_t ECX:1;
#else
                uint32_t:16;
#endif
                uint32_t GRP3PRI:2;
                uint32_t GRP2PRI:2;
                uint32_t GRP1PRI:2;
                uint32_t GRP0PRI:2;
#if MPC55XX_CHIP_TYPE / 10 == 567
                uint32_t EMLM:1;
                uint32_t CLM:1;
                uint32_t HALT:1;
                uint32_t HOE:1;
#else
                  uint32_t:4;
#endif
                uint32_t ERGA:1;
                uint32_t ERCA:1;
                uint32_t EDBG:1;
                uint32_t EBW:1;
            } B;
        } CR;                   /* Control Register */

        union {
            uint32_t R;
            struct {
                uint32_t VLD:1;
#if MPC55XX_CHIP_TYPE / 10 == 567
                uint32_t:14;
                uint32_t ECX:1;
#else
                  uint32_t:15;
#endif
                uint32_t GPE:1;
                uint32_t CPE:1;
                uint32_t ERRCHN:6;
                uint32_t SAE:1;
                uint32_t SOE:1;
                uint32_t DAE:1;
                uint32_t DOE:1;
                uint32_t NCE:1;
                uint32_t SGE:1;
                uint32_t SBE:1;
                uint32_t DBE:1;
            } B;
        } ESR;                  /* Error Status Register */

        union {
            uint32_t R;
            struct {
                uint32_t ERQ63:1;
                uint32_t ERQ62:1;
                uint32_t ERQ61:1;
                uint32_t ERQ60:1;
                uint32_t ERQ59:1;
                uint32_t ERQ58:1;
                uint32_t ERQ57:1;
                uint32_t ERQ56:1;
                uint32_t ERQ55:1;
                uint32_t ERQ54:1;
                uint32_t ERQ53:1;
                uint32_t ERQ52:1;
                uint32_t ERQ51:1;
                uint32_t ERQ50:1;
                uint32_t ERQ49:1;
                uint32_t ERQ48:1;
                uint32_t ERQ47:1;
                uint32_t ERQ46:1;
                uint32_t ERQ45:1;
                uint32_t ERQ44:1;
                uint32_t ERQ43:1;
                uint32_t ERQ42:1;
                uint32_t ERQ41:1;
                uint32_t ERQ40:1;
                uint32_t ERQ39:1;
                uint32_t ERQ38:1;
                uint32_t ERQ37:1;
                uint32_t ERQ36:1;
                uint32_t ERQ35:1;
                uint32_t ERQ34:1;
                uint32_t ERQ33:1;
                uint32_t ERQ32:1;
            } B;
        } ERQRH;                /* DMA Enable Request Register High */

        union {
            uint32_t R;
            struct {
                uint32_t ERQ31:1;
                uint32_t ERQ30:1;
                uint32_t ERQ29:1;
                uint32_t ERQ28:1;
                uint32_t ERQ27:1;
                uint32_t ERQ26:1;
                uint32_t ERQ25:1;
                uint32_t ERQ24:1;
                uint32_t ERQ23:1;
                uint32_t ERQ22:1;
                uint32_t ERQ21:1;
                uint32_t ERQ20:1;
                uint32_t ERQ19:1;
                uint32_t ERQ18:1;
                uint32_t ERQ17:1;
                uint32_t ERQ16:1;
                uint32_t ERQ15:1;
                uint32_t ERQ14:1;
                uint32_t ERQ13:1;
                uint32_t ERQ12:1;
                uint32_t ERQ11:1;
                uint32_t ERQ10:1;
                uint32_t ERQ09:1;
                uint32_t ERQ08:1;
                uint32_t ERQ07:1;
                uint32_t ERQ06:1;
                uint32_t ERQ05:1;
                uint32_t ERQ04:1;
                uint32_t ERQ03:1;
                uint32_t ERQ02:1;
                uint32_t ERQ01:1;
                uint32_t ERQ00:1;
            } B;
        } ERQRL;                /* DMA Enable Request Register Low */

        union {
            uint32_t R;
            struct {
                uint32_t EEI63:1;
                uint32_t EEI62:1;
                uint32_t EEI61:1;
                uint32_t EEI60:1;
                uint32_t EEI59:1;
                uint32_t EEI58:1;
                uint32_t EEI57:1;
                uint32_t EEI56:1;
                uint32_t EEI55:1;
                uint32_t EEI54:1;
                uint32_t EEI53:1;
                uint32_t EEI52:1;
                uint32_t EEI51:1;
                uint32_t EEI50:1;
                uint32_t EEI49:1;
                uint32_t EEI48:1;
                uint32_t EEI47:1;
                uint32_t EEI46:1;
                uint32_t EEI45:1;
                uint32_t EEI44:1;
                uint32_t EEI43:1;
                uint32_t EEI42:1;
                uint32_t EEI41:1;
                uint32_t EEI40:1;
                uint32_t EEI39:1;
                uint32_t EEI38:1;
                uint32_t EEI37:1;
                uint32_t EEI36:1;
                uint32_t EEI35:1;
                uint32_t EEI34:1;
                uint32_t EEI33:1;
                uint32_t EEI32:1;
            } B;
        } EEIRH;                /* DMA Enable Error Interrupt Register High */

        union {
            uint32_t R;
            struct {
                uint32_t EEI31:1;
                uint32_t EEI30:1;
                uint32_t EEI29:1;
                uint32_t EEI28:1;
                uint32_t EEI27:1;
                uint32_t EEI26:1;
                uint32_t EEI25:1;
                uint32_t EEI24:1;
                uint32_t EEI23:1;
                uint32_t EEI22:1;
                uint32_t EEI21:1;
                uint32_t EEI20:1;
                uint32_t EEI19:1;
                uint32_t EEI18:1;
                uint32_t EEI17:1;
                uint32_t EEI16:1;
                uint32_t EEI15:1;
                uint32_t EEI14:1;
                uint32_t EEI13:1;
                uint32_t EEI12:1;
                uint32_t EEI11:1;
                uint32_t EEI10:1;
                uint32_t EEI09:1;
                uint32_t EEI08:1;
                uint32_t EEI07:1;
                uint32_t EEI06:1;
                uint32_t EEI05:1;
                uint32_t EEI04:1;
                uint32_t EEI03:1;
                uint32_t EEI02:1;
                uint32_t EEI01:1;
                uint32_t EEI00:1;
            } B;
        } EEIRL;                /* DMA Enable Error Interrupt Register Low */

        union {                /* DMA Set Enable Request Register */
            uint8_t R;
            struct {
                uint8_t NOP:1;
                uint8_t SERQ:7;
            } B;
        } SERQR;

        union {                /* DMA Clear Enable Request Register */
            uint8_t R;
            struct {
                uint8_t NOP:1;
                uint8_t CERQ:7;
            } B;
        } CERQR;

        union {                /* DMA Set Enable Error Interrupt Register */
            uint8_t R;
            struct {
                uint8_t NOP:1;
                uint8_t SEEI:7;
            } B;
        } SEEIR;

        union {                /* DMA Clear Enable Error Interrupt Register */
            uint8_t R;
            struct {
                uint8_t NOP:1;
                uint8_t CEEI:7;
            } B;
        } CEEIR;

        union {                /* DMA Clear Interrupt Request Register */
            uint8_t R;
            struct {
                uint8_t NOP:1;
                uint8_t CINT:7;
            } B;
        } CIRQR;

        union {                  /* DMA Clear error Register */
            uint8_t R;
            struct {
                uint8_t NOP:1;
                uint8_t CERR:7;
            } B;
        } CER;

        union {                 /* Set Start Bit Register */
            uint8_t R;
            struct {
                uint8_t NOP:1;
                uint8_t SSB:7;
            } B;
        } SSBR;

        union {                /* Clear Done Status Bit Register */
            uint8_t R;
            struct {
                uint8_t NOP:1;
                uint8_t CDSB:7;
            } B;
        } CDSBR;

        union {
            uint32_t R;
            struct {
                uint32_t INT63:1;
                uint32_t INT62:1;
                uint32_t INT61:1;
                uint32_t INT60:1;
                uint32_t INT59:1;
                uint32_t INT58:1;
                uint32_t INT57:1;
                uint32_t INT56:1;
                uint32_t INT55:1;
                uint32_t INT54:1;
                uint32_t INT53:1;
                uint32_t INT52:1;
                uint32_t INT51:1;
                uint32_t INT50:1;
                uint32_t INT49:1;
                uint32_t INT48:1;
                uint32_t INT47:1;
                uint32_t INT46:1;
                uint32_t INT45:1;
                uint32_t INT44:1;
                uint32_t INT43:1;
                uint32_t INT42:1;
                uint32_t INT41:1;
                uint32_t INT40:1;
                uint32_t INT39:1;
                uint32_t INT38:1;
                uint32_t INT37:1;
                uint32_t INT36:1;
                uint32_t INT35:1;
                uint32_t INT34:1;
                uint32_t INT33:1;
                uint32_t INT32:1;
            } B;
        } IRQRH;                /* DMA Interrupt Request High */

        union {
            uint32_t R;
            struct {
                uint32_t INT31:1;
                uint32_t INT30:1;
                uint32_t INT29:1;
                uint32_t INT28:1;
                uint32_t INT27:1;
                uint32_t INT26:1;
                uint32_t INT25:1;
                uint32_t INT24:1;
                uint32_t INT23:1;
                uint32_t INT22:1;
                uint32_t INT21:1;
                uint32_t INT20:1;
                uint32_t INT19:1;
                uint32_t INT18:1;
                uint32_t INT17:1;
                uint32_t INT16:1;
                uint32_t INT15:1;
                uint32_t INT14:1;
                uint32_t INT13:1;
                uint32_t INT12:1;
                uint32_t INT11:1;
                uint32_t INT10:1;
                uint32_t INT09:1;
                uint32_t INT08:1;
                uint32_t INT07:1;
                uint32_t INT06:1;
                uint32_t INT05:1;
                uint32_t INT04:1;
                uint32_t INT03:1;
                uint32_t INT02:1;
                uint32_t INT01:1;
                uint32_t INT00:1;
            } B;
        } IRQRL;                /* DMA Interrupt Request Low */

        union {
            uint32_t R;
            struct {
                uint32_t ERR63:1;
                uint32_t ERR62:1;
                uint32_t ERR61:1;
                uint32_t ERR60:1;
                uint32_t ERR59:1;
                uint32_t ERR58:1;
                uint32_t ERR57:1;
                uint32_t ERR56:1;
                uint32_t ERR55:1;
                uint32_t ERR54:1;
                uint32_t ERR53:1;
                uint32_t ERR52:1;
                uint32_t ERR51:1;
                uint32_t ERR50:1;
                uint32_t ERR49:1;
                uint32_t ERR48:1;
                uint32_t ERR47:1;
                uint32_t ERR46:1;
                uint32_t ERR45:1;
                uint32_t ERR44:1;
                uint32_t ERR43:1;
                uint32_t ERR42:1;
                uint32_t ERR41:1;
                uint32_t ERR40:1;
                uint32_t ERR39:1;
                uint32_t ERR38:1;
                uint32_t ERR37:1;
                uint32_t ERR36:1;
                uint32_t ERR35:1;
                uint32_t ERR34:1;
                uint32_t ERR33:1;
                uint32_t ERR32:1;
            } B;
        } ERH;                  /* DMA Error High */

        union {
            uint32_t R;
            struct {
                uint32_t ERR31:1;
                uint32_t ERR30:1;
                uint32_t ERR29:1;
                uint32_t ERR28:1;
                uint32_t ERR27:1;
                uint32_t ERR26:1;
                uint32_t ERR25:1;
                uint32_t ERR24:1;
                uint32_t ERR23:1;
                uint32_t ERR22:1;
                uint32_t ERR21:1;
                uint32_t ERR20:1;
                uint32_t ERR19:1;
                uint32_t ERR18:1;
                uint32_t ERR17:1;
                uint32_t ERR16:1;
                uint32_t ERR15:1;
                uint32_t ERR14:1;
                uint32_t ERR13:1;
                uint32_t ERR12:1;
                uint32_t ERR11:1;
                uint32_t ERR10:1;
                uint32_t ERR09:1;
                uint32_t ERR08:1;
                uint32_t ERR07:1;
                uint32_t ERR06:1;
                uint32_t ERR05:1;
                uint32_t ERR04:1;
                uint32_t ERR03:1;
                uint32_t ERR02:1;
                uint32_t ERR01:1;
                uint32_t ERR00:1;
            } B;
        } ERL;                  /* DMA Error Low */

#if MPC55XX_CHIP_TYPE / 10 == 567
        union {                  /* hardware request status high */
            uint32_t R;
            struct {
                uint32_t HRS63:1;
                uint32_t HRS62:1;
                uint32_t HRS61:1;
                uint32_t HRS60:1;
                uint32_t HRS59:1;
                uint32_t HRS58:1;
                uint32_t HRS57:1;
                uint32_t HRS56:1;
                uint32_t HRS55:1;
                uint32_t HRS54:1;
                uint32_t HRS53:1;
                uint32_t HRS52:1;
                uint32_t HRS51:1;
                uint32_t HRS50:1;
                uint32_t HRS49:1;
                uint32_t HRS48:1;
                uint32_t HRS47:1;
                uint32_t HRS46:1;
                uint32_t HRS45:1;
                uint32_t HRS44:1;
                uint32_t HRS43:1;
                uint32_t HRS42:1;
                uint32_t HRS41:1;
                uint32_t HRS40:1;
                uint32_t HRS39:1;
                uint32_t HRS38:1;
                uint32_t HRS37:1;
                uint32_t HRS36:1;
                uint32_t HRS35:1;
                uint32_t HRS34:1;
                uint32_t HRS33:1;
                uint32_t HRS32:1;
            } B;
        } HRSH;

        union {                  /* hardware request status low */
            uint32_t R;
            struct {
                uint32_t HRS31:1;
                uint32_t HRS30:1;
                uint32_t HRS29:1;
                uint32_t HRS28:1;
                uint32_t HRS27:1;
                uint32_t HRS26:1;
                uint32_t HRS25:1;
                uint32_t HRS24:1;
                uint32_t HRS23:1;
                uint32_t HRS22:1;
                uint32_t HRS21:1;
                uint32_t HRS20:1;
                uint32_t HRS19:1;
                uint32_t HRS18:1;
                uint32_t HRS17:1;
                uint32_t HRS16:1;
                uint32_t HRS15:1;
                uint32_t HRS14:1;
                uint32_t HRS13:1;
                uint32_t HRS12:1;
                uint32_t HRS11:1;
                uint32_t HRS10:1;
                uint32_t HRS09:1;
                uint32_t HRS08:1;
                uint32_t HRS07:1;
                uint32_t HRS06:1;
                uint32_t HRS05:1;
                uint32_t HRS04:1;
                uint32_t HRS03:1;
                uint32_t HRS02:1;
                uint32_t HRS01:1;
                uint32_t HRS00:1;
            } B;
        } HRSL;

        uint32_t eDMA_reserved0038[50];  /* 0x0038-0x00FF */
#else
        uint32_t edma_reserved1[52];
#endif

        union {
            uint8_t R;
            struct {
                uint8_t ECP:1;
#if MPC55XX_CHIP_TYPE / 10 == 567
                uint8_t DPA:1;
#else
                  uint8_t:1;
#endif
                uint8_t GRPPRI:2;
                uint8_t CHPRI:4;
            } B;
        } CPR[64];

        uint32_t edma_reserved2[944];

/****************************************************************************/
/*       DMA2 Transfer Control Descriptor                                   */
/****************************************************************************/
        struct tcd_t {
            uint32_t SADDR;    /* source address */

            /* Source and destination fields */
            union tcd_SDF_tag {
                uint32_t R;
                struct {
                    uint16_t SMOD:5;   /* source address modulo */
                    uint16_t SSIZE:3;  /* source transfer size */
                    uint16_t DMOD:5;   /* destination address modulo */
                    uint16_t DSIZE:3;  /* destination transfer size */
                    int16_t SOFF;      /* signed source address offset */
                } B;
            } SDF;

            uint32_t NBYTES;   /* inner (minor) byte count */

            int32_t SLAST;     /* last destination address adjustment, or
                                   scatter/gather address (if e_sg = 1) */

            uint32_t DADDR;    /* destination address */

            /* CITER and destination fields */
            union tcd_CDF_tag {
                uint32_t R;
                struct {
                    uint16_t CITERE_LINK:1;
                    uint16_t CITER:15;
                    int16_t DOFF;      /* signed destination address offset */
                } B;
                struct {
		    uint16_t CITERE_LINK:1;
		    uint16_t CITERLINKCH:6;
		    uint16_t CITER:9;
                    int16_t DOFF;
                } B_ALT;
            } CDF;

            int32_t DLAST_SGA;

            /* BITER and misc fields */
            union tcd_BMF_tag {
                uint32_t R;
                struct {
                    uint32_t BITERE_LINK:1;    /* beginning ("major") iteration count */
                    uint32_t BITER:15;
                    uint32_t BWC:2;    /* bandwidth control */
                    uint32_t MAJORLINKCH:6;    /* enable channel-to-channel link */
                    uint32_t DONE:1;   /* channel done */
                    uint32_t ACTIVE:1; /* channel active */
                    uint32_t MAJORE_LINK:1;    /* enable channel-to-channel link */
                    uint32_t E_SG:1;   /* enable scatter/gather descriptor */
                    uint32_t D_REQ:1;  /* disable ipd_req when done */
                    uint32_t INT_HALF:1;       /* interrupt on citer = (biter >> 1) */
                    uint32_t INT_MAJ:1;        /* interrupt on major loop completion */
                    uint32_t START:1;  /* explicit channel start */
                } B;
                struct {
                    uint32_t BITERE_LINK:1;
                    uint32_t BITERLINKCH:6;
                    uint32_t BITER:9;
                    uint32_t BWC:2;
                    uint32_t MAJORLINKCH:6;
                    uint32_t DONE:1;
                    uint32_t ACTIVE:1;
                    uint32_t MAJORE_LINK:1;
                    uint32_t E_SG:1;
                    uint32_t D_REQ:1;
                    uint32_t INT_HALF:1;
                    uint32_t INT_MAJ:1;
                    uint32_t START:1;
                } B_ALT;
            } BMF;
        } TCD[64];              /* transfer_control_descriptor */
    };

#ifndef  __cplusplus
    static const struct tcd_t EDMA_TCD_DEFAULT = {
        .SADDR = 0,
        .SDF = { .R = 0 },
        .NBYTES = 0,
        .SLAST = 0,
        .DADDR = 0,
        .CDF = { .R = 0 },
        .DLAST_SGA = 0,
        .BMF = { .R = 0 }
    };
#endif /* __cplusplus */

#define EDMA_TCD_BITER_MASK 0x7fff

#define EDMA_TCD_BITER_SIZE (EDMA_TCD_BITER_MASK + 1)

#define EDMA_TCD_BITER_LINKED_MASK 0x1ff

#define EDMA_TCD_BITER_LINKED_SIZE (EDMA_TCD_BITER_LINKED_MASK + 1)

#define EDMA_TCD_LINK_AND_BITER(link, biter) \
  (((link) << 9) + ((biter) & EDMA_TCD_BITER_LINKED_MASK))

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCPU_POWERPC_MPC55XX_REGS_EDMA_H */

/*
 * Modifications of the original file provided by Freescale are:
 *
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
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

/**************************************************************************
 * FILE NAME: mpc5668.h                      COPYRIGHT (c) Freescale 2009 * 
 * REVISION:  1.1                                     All Rights Reserved *
 *                                                                        *
 * DESCRIPTION:                                                           *
 * This file contain all of the register and bit field definitions for    *
 * MPC5668.                                                               *
 **************************************************************************/
/*>>>>NOTE! this file is auto-generated please do not edit it!<<<<*/

/**************************************************************************
 * Example register & bit field write:                                    *
 *                                                                        *
 *  <MODULE>.<REGISTER>.B.<BIT> = 1;                                      *
 *  <MODULE>.<REGISTER>.R       = 0x10000000;                             *
 *                                                                        *
 **************************************************************************/

#ifndef _MPC5668_H_
#define _MPC5668_H_

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

/*************************************************************************/
/*                          MODULE : ADC                                 */
/*************************************************************************/
    struct ADC_tag {

        union {
            uint32_t R;
            struct {
                uint32_t OWREN:1;
                uint32_t WLSIDE:1;
                uint32_t MODE:1;
                uint32_t EDGLEV:1;
                uint32_t TRGEN:1;
                uint32_t EDGE:1;
                uint32_t XSTRTEN:1;
                uint32_t NSTART:1;
                  uint32_t:1;
                uint32_t JTRGEN:1;
                uint32_t JEDGE:1;
                uint32_t JSTART:1;
                  uint32_t:2;
                uint32_t CTUEN:1;
                  uint32_t:8;
                uint32_t ADCLKSEL:1;
                uint32_t ABORTCHAIN:1;
                uint32_t ABORT:1;
                uint32_t ACKO:1;
                uint32_t OFFREFRESH:1;
                uint32_t OFFCANC:1;
                  uint32_t:2;
                uint32_t PWDN:1;
            } B;
        } MCR;                  /* MAIN CONFIGURATION REGISTER */

        union {
            uint32_t R;
            struct {
                uint32_t:7;
                uint32_t NSTART:1;
                uint32_t JABORT:1;
                  uint32_t:2;
                uint32_t JSTART:1;
                  uint32_t:3;
                uint32_t CTUSTART:1;
                uint32_t CHADDR:7;
                  uint32_t:3;
                uint32_t ACKO:1;
                uint32_t OFFREFRESH:1;
                uint32_t OFFCANC:1;
                uint32_t ADCSTATUS:3;
            } B;
        } MSR;                  /* MAIN STATUS REGISTER */

        uint32_t adc_reserved1[2];

        union {
            uint32_t R;
            struct {
                uint32_t:25;
                uint32_t OFFCANCOVR:1;
                uint32_t EOFFSET:1;
                uint32_t EOCTU:1;
                uint32_t JEOC:1;
                uint32_t JECH:1;
                uint32_t EOC:1;
                uint32_t ECH:1;
            } B;
        } ISR;                  /* INTERRUPT STATUS REGISTER */

        union {
            uint32_t R;
            struct {
                uint32_t EOCCH31:1;
                uint32_t EOCCH30:1;
                uint32_t EOCCH29:1;
                uint32_t EOCCH28:1;
                uint32_t EOCCH27:1;
                uint32_t EOCCH26:1;
                uint32_t EOCCH25:1;
                uint32_t EOCCH24:1;
                uint32_t EOCCH23:1;
                uint32_t EOCCH22:1;
                uint32_t EOCCH21:1;
                uint32_t EOCCH20:1;
                uint32_t EOCCH19:1;
                uint32_t EOCCH18:1;
                uint32_t EOCCH17:1;
                uint32_t EOCCH16:1;
                uint32_t EOCCH15:1;
                uint32_t EOCCH14:1;
                uint32_t EOCCH13:1;
                uint32_t EOCCH12:1;
                uint32_t EOCCH11:1;
                uint32_t EOCCH10:1;
                uint32_t EOCCH9:1;
                uint32_t EOCCH8:1;
                uint32_t EOCCH7:1;
                uint32_t EOCCH6:1;
                uint32_t EOCCH5:1;
                uint32_t EOCCH4:1;
                uint32_t EOCCH3:1;
                uint32_t EOCCH2:1;
                uint32_t EOCCH1:1;
                uint32_t EOCCH0:1;
            } B;
        } CEOCFR0;              /* CHANNEL PENDING REGISTER 0 */

        union {
            uint32_t R;
            struct {
                uint32_t EOCCH63:1;
                uint32_t EOCCH62:1;
                uint32_t EOCCH61:1;
                uint32_t EOCCH60:1;
                uint32_t EOCCH59:1;
                uint32_t EOCCH58:1;
                uint32_t EOCCH57:1;
                uint32_t EOCCH56:1;
                uint32_t EOCCH55:1;
                uint32_t EOCCH54:1;
                uint32_t EOCCH53:1;
                uint32_t EOCCH52:1;
                uint32_t EOCCH51:1;
                uint32_t EOCCH50:1;
                uint32_t EOCCH49:1;
                uint32_t EOCCH48:1;
                uint32_t EOCCH47:1;
                uint32_t EOCCH46:1;
                uint32_t EOCCH45:1;
                uint32_t EOCCH44:1;
                uint32_t EOCCH43:1;
                uint32_t EOCCH42:1;
                uint32_t EOCCH41:1;
                uint32_t EOCCH40:1;
                uint32_t EOCCH39:1;
                uint32_t EOCCH38:1;
                uint32_t EOCCH37:1;
                uint32_t EOCCH36:1;
                uint32_t EOCCH35:1;
                uint32_t EOCCH34:1;
                uint32_t EOCCH33:1;
                uint32_t EOCCH32:1;
            } B;
        } CEOCFR1;              /* CHANNEL PENDING REGISTER 1 */

        union {
            uint32_t R;
            struct {
                uint32_t EOCCH95:1;
                uint32_t EOCCH94:1;
                uint32_t EOCCH93:1;
                uint32_t EOCCH92:1;
                uint32_t EOCCH91:1;
                uint32_t EOCCH90:1;
                uint32_t EOCCH89:1;
                uint32_t EOCCH88:1;
                uint32_t EOCCH87:1;
                uint32_t EOCCH86:1;
                uint32_t EOCCH85:1;
                uint32_t EOCCH84:1;
                uint32_t EOCCH83:1;
                uint32_t EOCCH82:1;
                uint32_t EOCCH81:1;
                uint32_t EOCCH80:1;
                uint32_t EOCCH79:1;
                uint32_t EOCCH78:1;
                uint32_t EOCCH77:1;
                uint32_t EOCCH76:1;
                uint32_t EOCCH75:1;
                uint32_t EOCCH74:1;
                uint32_t EOCCH73:1;
                uint32_t EOCCH72:1;
                uint32_t EOCCH71:1;
                uint32_t EOCCH70:1;
                uint32_t EOCCH69:1;
                uint32_t EOCCH68:1;
                uint32_t EOCCH67:1;
                uint32_t EOCCH66:1;
                uint32_t EOCCH65:1;
                uint32_t EOCCH64:1;
            } B;
        } CEOCFR2;              /* CHANNEL PENDING REGISTER 2 */

        union {
            uint32_t R;
            struct {
                uint32_t:25;
                uint32_t MSKOFFCANCOVR:1;
                uint32_t MSKEOFFSET:1;
                uint32_t MSKEOCTU:1;
                uint32_t MSKJEOC:1;
                uint32_t MSKJECH:1;
                uint32_t MSKEOC:1;
                uint32_t MSKECH:1;
            } B;
        } IMR;                  /* INTERRUPT MASK REGISTER */

        union {
            uint32_t R;
            struct {
                uint32_t CIM31:1;
                uint32_t CIM30:1;
                uint32_t CIM29:1;
                uint32_t CIM28:1;
                uint32_t CIM27:1;
                uint32_t CIM26:1;
                uint32_t CIM25:1;
                uint32_t CIM24:1;
                uint32_t CIM23:1;
                uint32_t CIM22:1;
                uint32_t CIM21:1;
                uint32_t CIM20:1;
                uint32_t CIM19:1;
                uint32_t CIM18:1;
                uint32_t CIM17:1;
                uint32_t CIM16:1;
                uint32_t CIM15:1;
                uint32_t CIM14:1;
                uint32_t CIM13:1;
                uint32_t CIM12:1;
                uint32_t CIM11:1;
                uint32_t CIM10:1;
                uint32_t CIM9:1;
                uint32_t CIM8:1;
                uint32_t CIM7:1;
                uint32_t CIM6:1;
                uint32_t CIM5:1;
                uint32_t CIM4:1;
                uint32_t CIM3:1;
                uint32_t CIM2:1;
                uint32_t CIM1:1;
                uint32_t CIM0:1;
            } B;
        } CIMR0;                /* CHANNEL INTERRUPT MASK REGISTER 0 */

        union {
            uint32_t R;
            struct {
                uint32_t CIM63:1;
                uint32_t CIM62:1;
                uint32_t CIM61:1;
                uint32_t CIM60:1;
                uint32_t CIM59:1;
                uint32_t CIM58:1;
                uint32_t CIM57:1;
                uint32_t CIM56:1;
                uint32_t CIM55:1;
                uint32_t CIM54:1;
                uint32_t CIM53:1;
                uint32_t CIM52:1;
                uint32_t CIM51:1;
                uint32_t CIM50:1;
                uint32_t CIM49:1;
                uint32_t CIM48:1;
                uint32_t CIM47:1;
                uint32_t CIM46:1;
                uint32_t CIM45:1;
                uint32_t CIM44:1;
                uint32_t CIM43:1;
                uint32_t CIM42:1;
                uint32_t CIM41:1;
                uint32_t CIM40:1;
                uint32_t CIM39:1;
                uint32_t CIM38:1;
                uint32_t CIM37:1;
                uint32_t CIM36:1;
                uint32_t CIM35:1;
                uint32_t CIM34:1;
                uint32_t CIM33:1;
                uint32_t CIM32:1;
            } B;
        } CIMR1;                /* CHANNEL INTERRUPT MASK REGISTER 1 */

        union {
            uint32_t R;
            struct {
                uint32_t CIM63:1;
                uint32_t CIM62:1;
                uint32_t CIM61:1;
                uint32_t CIM60:1;
                uint32_t CIM59:1;
                uint32_t CIM58:1;
                uint32_t CIM57:1;
                uint32_t CIM56:1;
                uint32_t CIM55:1;
                uint32_t CIM54:1;
                uint32_t CIM53:1;
                uint32_t CIM52:1;
                uint32_t CIM51:1;
                uint32_t CIM50:1;
                uint32_t CIM49:1;
                uint32_t CIM48:1;
                uint32_t CIM47:1;
                uint32_t CIM46:1;
                uint32_t CIM45:1;
                uint32_t CIM44:1;
                uint32_t CIM43:1;
                uint32_t CIM42:1;
                uint32_t CIM41:1;
                uint32_t CIM40:1;
                uint32_t CIM39:1;
                uint32_t CIM38:1;
                uint32_t CIM37:1;
                uint32_t CIM36:1;
                uint32_t CIM35:1;
                uint32_t CIM34:1;
                uint32_t CIM33:1;
                uint32_t CIM32:1;
            } B;
        } CIMR2;                /* CHANNEL INTERRUPT MASK REGISTER 2 */

        union {
            uint32_t R;
            struct {
                uint32_t:24;
                uint32_t WDG3H:1;
                uint32_t WDG2H:1;
                uint32_t WDG1H:1;
                uint32_t WDG0H:1;
                uint32_t WDG3L:1;
                uint32_t WDG2L:1;
                uint32_t WDG1L:1;
                uint32_t WDG0L:1;
            } B;
        } WTISR;                /* WATCHDOG INTERRUPT THRESHOLD REGISTER */

        union {
            uint32_t R;
            struct {
                uint32_t:24;
                uint32_t MSKWDG3H:1;
                uint32_t MSKWDG2H:1;
                uint32_t MSKWDG1H:1;
                uint32_t MSKWDG0H:1;
                uint32_t MSKWDG3L:1;
                uint32_t MSKWDG2L:1;
                uint32_t MSKWDG1L:1;
                uint32_t MSKWDG0L:1;
            } B;
        } WTIMR;                /* WATCHDOG INTERRUPT THRESHOLD MASK REGISTER */

        uint32_t adc_reserved2[2];

        union {
            uint32_t R;
            struct {
                uint32_t:30;
                uint32_t DCLR:1;
                uint32_t DMAEN:1;
            } B;
        } DMAE;                 /* DMA ENABLE REGISTER */

        union {
            uint32_t R;
            struct {
                uint32_t DMA31:1;
                uint32_t DMA30:1;
                uint32_t DMA29:1;
                uint32_t DMA28:1;
                uint32_t DMA27:1;
                uint32_t DMA26:1;
                uint32_t DMA25:1;
                uint32_t DMA24:1;
                uint32_t DMA23:1;
                uint32_t DMA22:1;
                uint32_t DMA21:1;
                uint32_t DMA20:1;
                uint32_t DMA19:1;
                uint32_t DMA18:1;
                uint32_t DMA17:1;
                uint32_t DMA16:1;
                uint32_t DMA15:1;
                uint32_t DMA14:1;
                uint32_t DMA13:1;
                uint32_t DMA12:1;
                uint32_t DMA11:1;
                uint32_t DMA10:1;
                uint32_t DMA9:1;
                uint32_t DMA8:1;
                uint32_t DMA7:1;
                uint32_t DMA6:1;
                uint32_t DMA5:1;
                uint32_t DMA4:1;
                uint32_t DMA3:1;
                uint32_t DMA2:1;
                uint32_t DMA1:1;
                uint32_t DMA0:1;
            } B;
        } DMAR0;                /* DMA CHANNEL SELECT REGISTER 0 */

        union {
            uint32_t R;
            struct {
                uint32_t DMA63:1;
                uint32_t DMA62:1;
                uint32_t DMA61:1;
                uint32_t DMA60:1;
                uint32_t DMA59:1;
                uint32_t DMA58:1;
                uint32_t DMA57:1;
                uint32_t DMA56:1;
                uint32_t DMA55:1;
                uint32_t DMA54:1;
                uint32_t DMA53:1;
                uint32_t DMA52:1;
                uint32_t DMA51:1;
                uint32_t DMA50:1;
                uint32_t DMA49:1;
                uint32_t DMA48:1;
                uint32_t DMA47:1;
                uint32_t DMA46:1;
                uint32_t DMA45:1;
                uint32_t DMA44:1;
                uint32_t DMA43:1;
                uint32_t DMA42:1;
                uint32_t DMA41:1;
                uint32_t DMA40:1;
                uint32_t DMA39:1;
                uint32_t DMA38:1;
                uint32_t DMA37:1;
                uint32_t DMA36:1;
                uint32_t DMA35:1;
                uint32_t DMA34:1;
                uint32_t DMA33:1;
                uint32_t DMA32:1;
            } B;
        } DMAR1;                /* DMA CHANNEL SELECT REGISTER 1 */

        union {
            uint32_t R;
            struct {
                uint32_t DMA95:1;
                uint32_t DMA94:1;
                uint32_t DMA93:1;
                uint32_t DMA92:1;
                uint32_t DMA91:1;
                uint32_t DMA90:1;
                uint32_t DMA89:1;
                uint32_t DMA88:1;
                uint32_t DMA87:1;
                uint32_t DMA86:1;
                uint32_t DMA85:1;
                uint32_t DMA84:1;
                uint32_t DMA83:1;
                uint32_t DMA82:1;
                uint32_t DMA81:1;
                uint32_t DMA80:1;
                uint32_t DMA79:1;
                uint32_t DMA78:1;
                uint32_t DMA77:1;
                uint32_t DMA76:1;
                uint32_t DMA75:1;
                uint32_t DMA74:1;
                uint32_t DMA73:1;
                uint32_t DMA72:1;
                uint32_t DMA71:1;
                uint32_t DMA70:1;
                uint32_t DMA69:1;
                uint32_t DMA68:1;
                uint32_t DMA67:1;
                uint32_t DMA66:1;
                uint32_t DMA65:1;
                uint32_t DMA64:1;
            } B;
        } DMAR2;                /* DMA CHANNEL SELECT REGISTER 2 */

        union {
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t THREN:1;
                uint32_t THRINV:1;
                uint32_t THROP:1;
                  uint32_t:6;
                uint32_t THRCH:7;
            } B;
        } TRC[4];               /* THRESHOLD CONTROL REGISTER */

        union {
            uint32_t R;
            struct {
                uint32_t:6;
                uint32_t THRH:10;
                  uint32_t:6;
                uint32_t THRL:10;
            } B;
        } THRHLR[4];            /* THRESHOLD REGISTER */

        union {
            uint32_t R;
            struct {
                uint32_t:6;
                uint32_t THRH:10;
                  uint32_t:6;
                uint32_t THRL:10;
            } B;
        } THRALT[4];            /* ALTERNATE THRESHOLD REGISTER */

        union {
            uint32_t R;
            struct {
                uint32_t:25;
                uint32_t PREVAL2:2;
                uint32_t PREVAL1:2;
                uint32_t PREVAL0:2;
                uint32_t PRECONV:1;
            } B;
        } PSCR;                 /* PRESAMPLING CONTROL REGISTER */

        union {
            uint32_t R;
            struct {
                uint32_t PSR31:1;
                uint32_t PSR30:1;
                uint32_t PSR29:1;
                uint32_t PSR28:1;
                uint32_t PSR27:1;
                uint32_t PSR26:1;
                uint32_t PSR25:1;
                uint32_t PSR24:1;
                uint32_t PSR23:1;
                uint32_t PSR22:1;
                uint32_t PSR21:1;
                uint32_t PSR20:1;
                uint32_t PSR19:1;
                uint32_t PSR18:1;
                uint32_t PSR17:1;
                uint32_t PSR16:1;
                uint32_t PSR15:1;
                uint32_t PSR14:1;
                uint32_t PSR13:1;
                uint32_t PSR12:1;
                uint32_t PSR11:1;
                uint32_t PSR10:1;
                uint32_t PSR9:1;
                uint32_t PSR8:1;
                uint32_t PSR7:1;
                uint32_t PSR6:1;
                uint32_t PSR5:1;
                uint32_t PSR4:1;
                uint32_t PSR3:1;
                uint32_t PSR2:1;
                uint32_t PSR1:1;
                uint32_t PSR0:1;
            } B;
        } PSR0;                 /* PRESAMPLING REGISTER 0 */

        union {
            uint32_t R;
            struct {
                uint32_t PSR63:1;
                uint32_t PSR62:1;
                uint32_t PSR61:1;
                uint32_t PSR60:1;
                uint32_t PSR59:1;
                uint32_t PSR58:1;
                uint32_t PSR57:1;
                uint32_t PSR56:1;
                uint32_t PSR55:1;
                uint32_t PSR54:1;
                uint32_t PSR53:1;
                uint32_t PSR52:1;
                uint32_t PSR51:1;
                uint32_t PSR50:1;
                uint32_t PSR49:1;
                uint32_t PSR48:1;
                uint32_t PSR47:1;
                uint32_t PSR46:1;
                uint32_t PSR45:1;
                uint32_t PSR44:1;
                uint32_t PSR43:1;
                uint32_t PSR42:1;
                uint32_t PSR41:1;
                uint32_t PSR40:1;
                uint32_t PSR39:1;
                uint32_t PSR38:1;
                uint32_t PSR37:1;
                uint32_t PSR36:1;
                uint32_t PSR35:1;
                uint32_t PSR34:1;
                uint32_t PSR33:1;
                uint32_t PSR32:1;
            } B;
        } PSR1;                 /* PRESAMPLING REGISTER 1 */

        union {
            uint32_t R;
            struct {
                uint32_t PSR95:1;
                uint32_t PSR94:1;
                uint32_t PSR93:1;
                uint32_t PSR92:1;
                uint32_t PSR91:1;
                uint32_t PSR90:1;
                uint32_t PSR89:1;
                uint32_t PSR88:1;
                uint32_t PSR87:1;
                uint32_t PSR86:1;
                uint32_t PSR85:1;
                uint32_t PSR84:1;
                uint32_t PSR83:1;
                uint32_t PSR82:1;
                uint32_t PSR81:1;
                uint32_t PSR80:1;
                uint32_t PSR79:1;
                uint32_t PSR78:1;
                uint32_t PSR77:1;
                uint32_t PSR76:1;
                uint32_t PSR75:1;
                uint32_t PSR74:1;
                uint32_t PSR73:1;
                uint32_t PSR72:1;
                uint32_t PSR71:1;
                uint32_t PSR70:1;
                uint32_t PSR69:1;
                uint32_t PSR68:1;
                uint32_t PSR67:1;
                uint32_t PSR66:1;
                uint32_t PSR65:1;
                uint32_t PSR64:1;
            } B;
        } PSR2;                 /* PRESAMPLING REGISTER 2 */

        uint32_t adc_reserved3;

        union {
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t INPLATCH:1;
                  uint32_t:1;
                uint32_t OFFSHIFT:2;
                  uint32_t:1;
                uint32_t INPCMP:2;
                  uint32_t:1;
                uint32_t INPSAMP:8;
            } B;
        } CTR0;                 /* CONVERSION TIMING REGISTER 0 */

        union {
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t INPLATCH:1;
                  uint32_t:4;
                uint32_t INPCMP:2;
                  uint32_t:1;
                uint32_t INPSAMP:8;
            } B;
        } CTR1;                 /* CONVERSION TIMING REGISTER 1 */

        union {
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t INPLATCH:1;
                  uint32_t:4;
                uint32_t INPCMP:2;
                  uint32_t:1;
                uint32_t INPSAMP:8;
            } B;
        } CTR2;                 /* CONVERSION TIMING REGISTER 2 */

        uint32_t adc_reserved4;

        union {
            uint32_t R;
            struct {
                uint32_t CH31:1;
                uint32_t CH30:1;
                uint32_t CH29:1;
                uint32_t CH28:1;
                uint32_t CH27:1;
                uint32_t CH26:1;
                uint32_t CH25:1;
                uint32_t CH24:1;
                uint32_t CH23:1;
                uint32_t CH22:1;
                uint32_t CH21:1;
                uint32_t CH20:1;
                uint32_t CH19:1;
                uint32_t CH18:1;
                uint32_t CH17:1;
                uint32_t CH16:1;
                uint32_t CH15:1;
                uint32_t CH14:1;
                uint32_t CH13:1;
                uint32_t CH12:1;
                uint32_t CH11:1;
                uint32_t CH10:1;
                uint32_t CH9:1;
                uint32_t CH8:1;
                uint32_t CH7:1;
                uint32_t CH6:1;
                uint32_t CH5:1;
                uint32_t CH4:1;
                uint32_t CH3:1;
                uint32_t CH2:1;
                uint32_t CH1:1;
                uint32_t CH0:1;
            } B;
        } NCMR0;                /* NORMAL CONVERSION MASK REGISTER 0 */

        union {
            uint32_t R;
            struct {
                uint32_t CH63:1;
                uint32_t CH62:1;
                uint32_t CH61:1;
                uint32_t CH60:1;
                uint32_t CH59:1;
                uint32_t CH58:1;
                uint32_t CH57:1;
                uint32_t CH56:1;
                uint32_t CH55:1;
                uint32_t CH54:1;
                uint32_t CH53:1;
                uint32_t CH52:1;
                uint32_t CH51:1;
                uint32_t CH50:1;
                uint32_t CH49:1;
                uint32_t CH48:1;
                uint32_t CH47:1;
                uint32_t CH46:1;
                uint32_t CH45:1;
                uint32_t CH44:1;
                uint32_t CH43:1;
                uint32_t CH42:1;
                uint32_t CH41:1;
                uint32_t CH40:1;
                uint32_t CH39:1;
                uint32_t CH38:1;
                uint32_t CH37:1;
                uint32_t CH36:1;
                uint32_t CH35:1;
                uint32_t CH34:1;
                uint32_t CH33:1;
                uint32_t CH32:1;
            } B;
        } NCMR1;                /* NORMAL CONVERSION MASK REGISTER 1 */

        union {
            uint32_t R;
            struct {
                uint32_t PSR95:1;
                uint32_t PSR94:1;
                uint32_t PSR93:1;
                uint32_t PSR92:1;
                uint32_t PSR91:1;
                uint32_t PSR90:1;
                uint32_t PSR89:1;
                uint32_t PSR88:1;
                uint32_t PSR87:1;
                uint32_t PSR86:1;
                uint32_t PSR85:1;
                uint32_t PSR84:1;
                uint32_t PSR83:1;
                uint32_t PSR82:1;
                uint32_t PSR81:1;
                uint32_t PSR80:1;
                uint32_t PSR79:1;
                uint32_t PSR78:1;
                uint32_t PSR77:1;
                uint32_t PSR76:1;
                uint32_t PSR75:1;
                uint32_t PSR74:1;
                uint32_t PSR73:1;
                uint32_t PSR72:1;
                uint32_t PSR71:1;
                uint32_t PSR70:1;
                uint32_t PSR69:1;
                uint32_t PSR68:1;
                uint32_t PSR67:1;
                uint32_t PSR66:1;
                uint32_t PSR65:1;
                uint32_t PSR64:1;
            } B;
        } NCMR2;                /* NORMAL CONVERSION MASK REGISTER 2 */

        uint32_t adc_reserved5;

        union {
            uint32_t R;
            struct {
                uint32_t CH31:1;
                uint32_t CH30:1;
                uint32_t CH29:1;
                uint32_t CH28:1;
                uint32_t CH27:1;
                uint32_t CH26:1;
                uint32_t CH25:1;
                uint32_t CH24:1;
                uint32_t CH23:1;
                uint32_t CH22:1;
                uint32_t CH21:1;
                uint32_t CH20:1;
                uint32_t CH19:1;
                uint32_t CH18:1;
                uint32_t CH17:1;
                uint32_t CH16:1;
                uint32_t CH15:1;
                uint32_t CH14:1;
                uint32_t CH13:1;
                uint32_t CH12:1;
                uint32_t CH11:1;
                uint32_t CH10:1;
                uint32_t CH9:1;
                uint32_t CH8:1;
                uint32_t CH7:1;
                uint32_t CH6:1;
                uint32_t CH5:1;
                uint32_t CH4:1;
                uint32_t CH3:1;
                uint32_t CH2:1;
                uint32_t CH1:1;
                uint32_t CH0:1;
            } B;
        } JCMR0;                /* INJECTED CONVERSION MASK REGISTER 0 */

        union {
            uint32_t R;
            struct {
                uint32_t CH63:1;
                uint32_t CH62:1;
                uint32_t CH61:1;
                uint32_t CH60:1;
                uint32_t CH59:1;
                uint32_t CH58:1;
                uint32_t CH57:1;
                uint32_t CH56:1;
                uint32_t CH55:1;
                uint32_t CH54:1;
                uint32_t CH53:1;
                uint32_t CH52:1;
                uint32_t CH51:1;
                uint32_t CH50:1;
                uint32_t CH49:1;
                uint32_t CH48:1;
                uint32_t CH47:1;
                uint32_t CH46:1;
                uint32_t CH45:1;
                uint32_t CH44:1;
                uint32_t CH43:1;
                uint32_t CH42:1;
                uint32_t CH41:1;
                uint32_t CH40:1;
                uint32_t CH39:1;
                uint32_t CH38:1;
                uint32_t CH37:1;
                uint32_t CH36:1;
                uint32_t CH35:1;
                uint32_t CH34:1;
                uint32_t CH33:1;
                uint32_t CH32:1;
            } B;
        } JCMR1;                /* INJECTED CONVERSION MASK REGISTER 1 */

        union {
            uint32_t R;
            struct {
                uint32_t PSR95:1;
                uint32_t PSR94:1;
                uint32_t PSR93:1;
                uint32_t PSR92:1;
                uint32_t PSR91:1;
                uint32_t PSR90:1;
                uint32_t PSR89:1;
                uint32_t PSR88:1;
                uint32_t PSR87:1;
                uint32_t PSR86:1;
                uint32_t PSR85:1;
                uint32_t PSR84:1;
                uint32_t PSR83:1;
                uint32_t PSR82:1;
                uint32_t PSR81:1;
                uint32_t PSR80:1;
                uint32_t PSR79:1;
                uint32_t PSR78:1;
                uint32_t PSR77:1;
                uint32_t PSR76:1;
                uint32_t PSR75:1;
                uint32_t PSR74:1;
                uint32_t PSR73:1;
                uint32_t PSR72:1;
                uint32_t PSR71:1;
                uint32_t PSR70:1;
                uint32_t PSR69:1;
                uint32_t PSR68:1;
                uint32_t PSR67:1;
                uint32_t PSR66:1;
                uint32_t PSR65:1;
                uint32_t PSR64:1;
            } B;
        } JCMR2;                /* INJECTED CONVERSION MASK REGISTER 2 */

        union {
            uint32_t R;
            struct {
                uint32_t:15;
                uint32_t OFFSETLOAD:1;
                  uint32_t:8;
                uint32_t OFFSET_WORD:8;
            } B;
        } OFFWR;                /* OFFSET WORD REGISTER */

        union {
            uint32_t R;
            struct {
                uint32_t:24;
                uint32_t DSD:8;
            } B;
        } DSDR;                 /* DECODE SIGNALS DELAY REGISTER */

        union {
            uint32_t R;
            struct {
                uint32_t:24;
                uint32_t PDED:8;
            } B;
        } PDEDR;                /* DECODE SIGNALS DELAY REGISTER */

        uint32_t adc_reserved6[9];

        union {
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t TEST_CTL:16;
            } B;
        } TCTLR;                /* TEST CONTROL REGISTER */

        uint32_t adc_reserved7[3];

        union {
            uint32_t R;
            struct {
                uint32_t:12;
                uint32_t VALID:1;
                uint32_t OVERW:1;
                uint32_t RESULT:2;
                  uint32_t:6;
                uint32_t CDATA:10;
            } B;
        } PRECDATAREG[32];      /* PRESISION DATA REGISTER */

        union {
            uint32_t R;
            struct {
                uint32_t:12;
                uint32_t VALID:1;
                uint32_t OVERW:1;
                uint32_t RESULT:2;
                  uint32_t:6;
                uint32_t CDATA:10;
            } B;
        } INTDATAREG[32];       /* PRESISION DATA REGISTER */

        union {
            uint32_t R;
            struct {
                uint32_t:12;
                uint32_t VALID:1;
                uint32_t OVERW:1;
                uint32_t RESULT:2;
                  uint32_t:6;
                uint32_t CDATA:10;
            } B;
        } EXTDATAREG[32];       /* PRESISION DATA REGISTER */

    };                          /* end of ADC_tag */
/**************************************************************************/
/*                  MODULE : AXBS Crossbar Switch (XBAR)                  */
/**************************************************************************/
    struct XBAR_tag {

        union {
            uint32_t R;
            struct {
                uint32_t:1;
                uint32_t MSTR7:3;
                  uint32_t:1;
                uint32_t MSTR6:3;
                  uint32_t:9;
                uint32_t MSTR5:3;
                  uint32_t:1;
                uint32_t MSTR3:3;
                  uint32_t:1;
                uint32_t MSTR2:3;
                  uint32_t:1;
                uint32_t MSTR1:3;
                  uint32_t:1;
                uint32_t MSTR0:1;
            } B;
        } MPR0;                 /* Master Priority Register 0 */

        uint32_t xbar_reserved1[3];

        union {
            uint32_t R;
            struct {
                uint32_t R0:1;
                  uint32_t:21;
                uint32_t ARB:2;
                  uint32_t:2;
                uint32_t PCTL:2;
                  uint32_t:1;
                uint32_t PARK:3;
            } B;
        } SGPCR0;               /* Master Priority Register 0 */

        uint32_t xbar_reserved2[58];

        union {
            uint32_t R;
            struct {
                uint32_t:1;
                uint32_t MSTR7:3;
                  uint32_t:1;
                uint32_t MSTR6:3;
                  uint32_t:9;
                uint32_t MSTR5:3;
                  uint32_t:1;
                uint32_t MSTR3:3;
                  uint32_t:1;
                uint32_t MSTR2:3;
                  uint32_t:1;
                uint32_t MSTR1:3;
                  uint32_t:1;
                uint32_t MSTR0:1;
            } B;
        } MPR1;                 /* Master Priority Register 1 */

        uint32_t xbar_reserved3[3];

        union {
            uint32_t R;
            struct {
                uint32_t R0:1;
                  uint32_t:21;
                uint32_t ARB:2;
                  uint32_t:2;
                uint32_t PCTL:2;
                  uint32_t:1;
                uint32_t PARK:3;
            } B;
        } SGPCR1;               /* Master Priority Register 1 */

        uint32_t xbar_reserved4[58];

        union {
            uint32_t R;
            struct {
                uint32_t:1;
                uint32_t MSTR7:3;
                  uint32_t:1;
                uint32_t MSTR6:3;
                  uint32_t:9;
                uint32_t MSTR5:3;
                  uint32_t:1;
                uint32_t MSTR3:3;
                  uint32_t:1;
                uint32_t MSTR2:3;
                  uint32_t:1;
                uint32_t MSTR1:3;
                  uint32_t:1;
                uint32_t MSTR0:1;
            } B;
        } MPR2;                 /* Master Priority Register 2 */

        uint32_t xbar_reserved5[3];

        union {
            uint32_t R;
            struct {
                uint32_t R0:1;
                  uint32_t:21;
                uint32_t ARB:2;
                  uint32_t:2;
                uint32_t PCTL:2;
                  uint32_t:1;
                uint32_t PARK:3;
            } B;
        } SGPCR2;               /* Master Priority Register 2 */

        uint32_t xbar_reserved6[58];

        union {
            uint32_t R;
            struct {
                uint32_t:1;
                uint32_t MSTR7:3;
                  uint32_t:1;
                uint32_t MSTR6:3;
                  uint32_t:9;
                uint32_t MSTR5:3;
                  uint32_t:1;
                uint32_t MSTR3:3;
                  uint32_t:1;
                uint32_t MSTR2:3;
                  uint32_t:1;
                uint32_t MSTR1:3;
                  uint32_t:1;
                uint32_t MSTR0:1;
            } B;
        } MPR3;                 /* Master Priority Register 3 */

        uint32_t xbar_reserved7[3];

        union {
            uint32_t R;
            struct {
                uint32_t R0:1;
                  uint32_t:21;
                uint32_t ARB:2;
                  uint32_t:2;
                uint32_t PCTL:2;
                  uint32_t:1;
                uint32_t PARK:3;
            } B;
        } SGPCR3;               /* Master Priority Register 3 */

        uint32_t xbar_reserved8[186];

        union {
            uint32_t R;
            struct {
                uint32_t:1;
                uint32_t MSTR7:3;
                  uint32_t:1;
                uint32_t MSTR6:3;
                  uint32_t:9;
                uint32_t MSTR5:3;
                  uint32_t:1;
                uint32_t MSTR3:3;
                  uint32_t:1;
                uint32_t MSTR2:3;
                  uint32_t:1;
                uint32_t MSTR1:3;
                  uint32_t:1;
                uint32_t MSTR0:1;
            } B;
        } MPR6;                 /* Master Priority Register 6 */

        uint32_t xbar_reserved9[3];

        union {
            uint32_t R;
            struct {
                uint32_t R0:1;
                  uint32_t:21;
                uint32_t ARB:2;
                  uint32_t:2;
                uint32_t PCTL:2;
                  uint32_t:1;
                uint32_t PARK:3;
            } B;
        } SGPCR6;               /* Master Priority Register 6 */

        uint32_t xbar_reserved10[58];

        union {
            uint32_t R;
            struct {
                uint32_t:1;
                uint32_t MSTR7:3;
                  uint32_t:1;
                uint32_t MSTR6:3;
                  uint32_t:9;
                uint32_t MSTR5:3;
                  uint32_t:1;
                uint32_t MSTR3:3;
                  uint32_t:1;
                uint32_t MSTR2:3;
                  uint32_t:1;
                uint32_t MSTR1:3;
                  uint32_t:1;
                uint32_t MSTR0:1;
            } B;
        } MPR7;                 /* Master Priority Register 7 */

        uint32_t xbar_reserved11[3];

        union {
            uint32_t R;
            struct {
                uint32_t R0:1;
                  uint32_t:21;
                uint32_t ARB:2;
                  uint32_t:2;
                uint32_t PCTL:2;
                  uint32_t:1;
                uint32_t PARK:3;
            } B;
        } SGPCR7;               /* Master Priority Register 7 */

        uint32_t xbar_reserved12[506];

        union {
            uint32_t R;
            struct {
                uint32_t R0:1;
                  uint32_t:21;
                uint32_t ARB:2;
                  uint32_t:2;
                uint32_t PCTL:2;
                  uint32_t:1;
                uint32_t PARK:3;
            } B;
        } MGPCR7;               /* Master General Purpose Register 7 */

    };
/*************************************************************************/
/*                            MODULE : CRP                               */
/*************************************************************************/
    struct CRP_tag {

        union {
            uint32_t R;
            struct {
                uint32_t IRCTRIMEN:1;
                  uint32_t:4;
                uint32_t PREDIV:3;
                  uint32_t:4;
                uint32_t EN128KIRC:1;
                uint32_t EN32KOSC:1;
                uint32_t ENLPOSC:1;
                uint32_t EN40MOSC:1;
                  uint32_t:3;
                uint32_t TRIM128IRC:5;
                  uint32_t:2;
                uint32_t TRIM16IRC:6;
            } B;
        } CLKSRC;               /* CLOCK SOURCE REGISTER */

        uint32_t crp_reserved1[3];

        union {
            uint32_t R;
            struct {
                uint32_t CNTEN:1;
                uint32_t RTCIE:1;
                uint32_t FRZEN:1;
                uint32_t ROVREN:1;
                uint32_t RTCVAL:12;
                uint32_t APIEN:1;
                uint32_t APIIE:1;
                uint32_t CLKSEL:2;
                uint32_t DIV512EN:1;
                uint32_t DIV32EN:1;
                uint32_t APIVAL:10;
            } B;
        } RTCC;                 /* RTC CONTROL REGISTER */

        union {
            uint32_t R;
            struct {
                uint32_t:2;
                uint32_t RTCF:1;
                  uint32_t:15;
                uint32_t APIF:1;
                  uint32_t:2;
                uint32_t ROVRF:1;
                  uint32_t:10;
            } B;
        } RTSC;                 /* RTC STATUS REGISTER */

        union {
            uint32_t R;
            struct {
                uint32_t RTCCNT:32;
            } B;
        } RTCCNT;               /* RTC Counter Register */

        uint32_t crp_reserved2[9];

        union {
            uint32_t R;
            struct {
                uint32_t PWK31:2;
                uint32_t PWK30:2;
                uint32_t PWK29:2;
                uint32_t PWK28:2;
                uint32_t PWK27:2;
                uint32_t PWK26:2;
                uint32_t PWK25:2;
                uint32_t PWK24:2;
                uint32_t PWK23:2;
                uint32_t PWK22:2;
                uint32_t PWK21:2;
                uint32_t PWK20:2;
                uint32_t PWK19:2;
                uint32_t PWK18:2;
                uint32_t PWK17:2;
                uint32_t PWK16:2;
            } B;
        } PWKENH;               /* PIN WAKEUP ENABLE HIGH REGISTER */

        union {
            uint32_t R;
            struct {
                uint32_t PWK15:2;
                uint32_t PWK14:2;
                uint32_t PWK13:2;
                uint32_t PWK12:2;
                uint32_t PWK11:2;
                uint32_t PWK10:2;
                uint32_t PWK9:2;
                uint32_t PWK8:2;
                uint32_t PWK7:2;
                uint32_t PWK6:2;
                uint32_t PWK5:2;
                uint32_t PWK4:2;
                uint32_t PWK3:2;
                uint32_t PWK2:2;
                uint32_t PWK1:2;
                uint32_t PWK0:2;
            } B;
        } PWKENL;               /* PIN WAKEUP ENABLE LOW REGISTER */

        union {
            uint32_t R;
            struct {
                uint32_t PWKSRCIE31:1;
                uint32_t PWKSRCIE30:1;
                uint32_t PWKSRCIE29:1;
                uint32_t PWKSRCIE28:1;
                uint32_t PWKSRCIE27:1;
                uint32_t PWKSRCIE26:1;
                uint32_t PWKSRCIE25:1;
                uint32_t PWKSRCIE24:1;
                uint32_t PWKSRCIE23:1;
                uint32_t PWKSRCIE22:1;
                uint32_t PWKSRCIE21:1;
                uint32_t PWKSRCIE20:1;
                uint32_t PWKSRCIE19:1;
                uint32_t PWKSRCIE18:1;
                uint32_t PWKSRCIE17:1;
                uint32_t PWKSRCIE16:1;
                uint32_t PWKSRCIE15:1;
                uint32_t PWKSRCIE14:1;
                uint32_t PWKSRCIE13:1;
                uint32_t PWKSRCIE12:1;
                uint32_t PWKSRCIE11:1;
                uint32_t PWKSRCIE10:1;
                uint32_t PWKSRCIE9:1;
                uint32_t PWKSRCIE8:1;
                uint32_t PWKSRCIE7:1;
                uint32_t PWKSRCIE6:1;
                uint32_t PWKSRCIE5:1;
                uint32_t PWKSRCIE4:1;
                uint32_t PWKSRCIE3:1;
                uint32_t PWKSRCIE2:1;
                uint32_t PWKSRCIE1:1;
                uint32_t PWKSRCIE0:1;
            } B;
        } PWKSRCIE;             /* PIN WAKEUP SOURCE INTERRUPT ENABLE REGISTER */

        union {
            uint32_t R;
            struct {
                uint32_t PWKSRCIE31:1;
                uint32_t PWKSRCIE30:1;
                uint32_t PWKSRCIE29:1;
                uint32_t PWKSRCIE28:1;
                uint32_t PWKSRCIE27:1;
                uint32_t PWKSRCIE26:1;
                uint32_t PWKSRCIE25:1;
                uint32_t PWKSRCIE24:1;
                uint32_t PWKSRCIE23:1;
                uint32_t PWKSRCIE22:1;
                uint32_t PWKSRCIE21:1;
                uint32_t PWKSRCIE20:1;
                uint32_t PWKSRCIE19:1;
                uint32_t PWKSRCIE18:1;
                uint32_t PWKSRCIE17:1;
                uint32_t PWKSRCIE16:1;
                uint32_t PWKSRCIE15:1;
                uint32_t PWKSRCIE14:1;
                uint32_t PWKSRCIE13:1;
                uint32_t PWKSRCIE12:1;
                uint32_t PWKSRCIE11:1;
                uint32_t PWKSRCIE10:1;
                uint32_t PWKSRCIE9:1;
                uint32_t PWKSRCIE8:1;
                uint32_t PWKSRCIE7:1;
                uint32_t PWKSRCIE6:1;
                uint32_t PWKSRCIE5:1;
                uint32_t PWKSRCIE4:1;
                uint32_t PWKSRCIE3:1;
                uint32_t PWKSRCIE2:1;
                uint32_t PWKSRCIE1:1;
                uint32_t PWKSRCIE0:1;
            } B;
        } PWKSRCF;              /* PIN WAKEUP SOURCE FLAG REGISTER */

        union {
            uint32_t R;
            struct {
                uint32_t Z6VECB:20;
                  uint32_t:10;
                uint32_t Z6RST:1;
                uint32_t VLE:1;
            } B;
        } Z6VEC;                /* Z6 RESET VECTOR REGISTER */

        union {
            uint32_t R;
            struct {
                uint32_t Z0VECB:30;
                uint32_t Z0RST:1;
                  uint32_t:1;
            } B;
        } Z0VEC;                /* Z0 RESET VECTOR REGISTER */

        union {
            uint32_t R;
            struct {
                uint32_t RECPTR:30;
                uint32_t FASTREC:1;
                  uint32_t:1;
            } B;
        } RECPTR;               /* RESET RECOVERY POINTER REGISTER */

        uint32_t crp_reserved3;

        union {
            uint32_t R;
            struct {
                uint32_t SLEEPF:1;
                  uint32_t:12;
                uint32_t RTCOVRWKF:1;
                uint32_t RTCWKF:1;
                uint32_t APIWKF:1;
                uint32_t SLEEP:1;
                  uint32_t:4;
                uint32_t RAMSEL:3;
                  uint32_t:4;
                uint32_t WKCLKSEL:1;
                uint32_t RTCOVRWKEN:1;
                uint32_t RTCWKEN:1;
                uint32_t APIWKEN:1;
            } B;
        } PSCR;                 /* POWER STATUS AND CONTROL REGISTER */

        uint32_t crp_reserved4[3];

        union {
            uint32_t R;
            struct {
                uint32_t LVI5LOCK:1;
                uint32_t LVI5RE:1;
                  uint32_t:7;
                uint32_t LVI5HIE:1;
                uint32_t LVI5NIE:1;
                uint32_t LVI5IE:1;
                  uint32_t:2;
                uint32_t FRIE:1;
                uint32_t FDIS:1;
                  uint32_t:9;
                uint32_t LVI5HIF:1;
                uint32_t LVI5NF:1;
                uint32_t LVI5F:1;
                  uint32_t:2;
                uint32_t FRF:1;
                uint32_t FRDY:1;
            } B;
        } SOCSC;                /* LVI Status and Control Register */

    };                          /* end of CRP_tag */
/*************************************************************************/
/*                          MODULE : CTU                                 */
/*************************************************************************/
    struct CTU_tag {

        union {
            uint32_t R;
            struct {
                uint32_t:24;
                uint32_t TRGIEN:1;
                uint32_t TRGI:1;
                  uint32_t:2;
                uint32_t PRESC_CONF:4;
            } B;
        } CSR;                  /* Control Status Register */

        union {
            uint32_t R;
            struct {
                uint32_t:23;
                uint32_t SV:9;
            } B;
        } SVR[7];               /* Start Value Register */

        union {
            uint32_t R;
            struct {
                uint32_t:23;
                uint32_t CV:9;
            } B;
        } CVR[4];               /* Current Value Register */

        union {
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t TM:1;
                  uint32_t:1;
                uint32_t COUNT_GROUP:2;
                  uint32_t:1;
                uint32_t DELAY_INDEX:3;
                uint32_t CLR_FG:1;
                  uint32_t:1;
                uint32_t CHANNEL_VALUE:6;
            } B;
        } EVTCFGR[33];          /* Event Configuration Register */

    };                          /* end of CTU_tag */
/*************************************************************************/
/*                          MODULE : DMAMUX                              */
/*************************************************************************/
    struct DMAMUX_tag {
        union {
            uint8_t R;
            struct {
                uint8_t ENBL:1;
                uint8_t TRIG:1;
                uint8_t SOURCE:6;
            } B;
        } CHCONFIG[32];         /* DMA Channel Configuration Register */

    };                          /* end of DMAMUX_tag */
/*************************************************************************/
/*                          MODULE : DSPI                                */
/*************************************************************************/
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
                uint32_t:11;
                uint32_t TSBC:1;
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
                uint32_t SER_DATA:32;
            } B;
        } SDR;                  /* DSI Serialization Data Register */

        union {
            uint32_t R;
            struct {
                uint32_t ASER_DATA:32;
            } B;
        } ASDR;                 /* DSI Alternate Serialization Data Register */

        union {
            uint32_t R;
            struct {
                uint32_t COMP_DATA:32;
            } B;
        } COMPR;                /* DSI Transmit Comparison Register */

        union {
            uint32_t R;
            struct {
                uint32_t DESER_DATA:32;
            } B;
        } DDR;                  /* DSI deserialization Data Register */

        union {
            uint32_t R;
            struct {
                uint32_t:3;
                uint32_t TSBCNT:5;
                  uint32_t:16;
                uint32_t DPCS1_7:1;
                uint32_t DPCS1_6:1;
                uint32_t DPCS1_5:1;
                uint32_t DPCS1_4:1;
                uint32_t DPCS1_3:1;
                uint32_t DPCS1_2:1;
                uint32_t DPCS1_1:1;
                uint32_t DPCS1_0:1;
            } B;
        } DSICR1;               /* DSI Configuration Register 1 */

    };                          /* end of DSPI_tag */
/*************************************************************************/
/*                           MODULE : ECSM                               */
/*************************************************************************/
    struct ECSM_tag {

        uint32_t ecsm_reserved1[9];

        union {
            uint32_t R;
            struct {
                uint32_t FXSBE0:1;
                uint32_t FXSBE1:1;
                uint32_t FXSBE2:1;
                uint32_t FXSBE3:1;
                  uint32_t:2;
                uint32_t FXSBE6:1;
                uint32_t FXSBE7:1;
                uint32_t RBEN:1;
                uint32_t WBEN:1;
                uint32_t ACCERR:1;
                  uint32_t:21;
            } B;
        } FBOMCR;               /* FEC Burst Optimisation Master Control Register */

        uint8_t ecsm_reserved2[27];

        union {
            uint8_t R;
            struct {
                uint8_t:2;
                uint8_t EPR1BR:1;
                uint8_t EPF1BR:1;
                  uint8_t:2;
                uint8_t EPRNCR:1;
                uint8_t EPFNCR:1;
            } B;
        } ECR;                  /* ECC Configuration Register */

        uint8_t ecsm_reserved3[3];

        union {
            uint8_t R;
            struct {
                uint8_t:2;
                uint8_t PR1BC:1;
                uint8_t PF1BC:1;
                  uint8_t:2;
                uint8_t PRNCE:1;
                uint8_t PFNCE:1;
            } B;
        } ESR;                  /* ECC Status Register */

        uint16_t ecsm_reserved4;

        union {
            uint16_t R;
            struct {
                uint16_t:2;
                uint16_t FRC1BI:1;
                uint16_t FR11BI:1;
                  uint16_t:2;
                uint16_t FRCNCI:1;
                uint16_t FR1NCI:1;
                uint16_t PREI_SEL:1;
                uint16_t ERRBIT:7;
            } B;
        } EEGR;                 /* ECC Error Generation Register */

        uint32_t ecsm_reserved5;

        union {
            uint32_t R;
            struct {
                uint32_t PFEAR:32;
            } B;
        } PFEAR;                /* Platform Flash ECC Address Register */

        uint16_t ecsm_reserved6;

        union {
            uint8_t R;
            struct {
                uint8_t:4;
                uint8_t PFEMR:4;
            } B;
        } PFEMR;                /* Platform Flash ECC Address Register */

        union {
            uint8_t R;
            struct {
                uint8_t WRITE:1;
                uint8_t SIZE:3;
                uint8_t PROTECTION:4;
            } B;
        } PFEAT;                /* Flash ECC Attributes Register */

        union {
            uint32_t R;
            struct {
                uint32_t PFEDRH:32;
            } B;
        } PFEDRH;               /* Flash ECC Data High Register */

        union {
            uint32_t R;
            struct {
                uint32_t PFEDRL:32;
            } B;
        } PFEDRL;               /* Flash ECC Data Low Register */

        union {
            uint32_t R;
            struct {
                uint32_t PREAR:32;
            } B;
        } PREAR;                /* Platform RAM ECC Address Register */

        uint16_t ecsm_reserved8;

        union {
            uint8_t R;
            struct {
                uint8_t:4;
                uint8_t PREMR:4;
            } B;
        } PREMR;                /* RAM ECC Attributes Register */

        union {
            uint8_t R;
            struct {
                uint8_t WRITE:1;
                uint8_t SIZE:3;
                uint8_t PROTECTION:4;
            } B;
        } PREAT;                /* Platform RAM ECC Attributes Register */

        union {
            uint32_t R;
            struct {
                uint32_t PREDR:32;
            } B;
        } PREDRH;               /* Platform RAM ECC Data Low Register High */

        union {
            uint32_t R;
            struct {
                uint32_t PREDR:32;
            } B;
        } PREDRL;               /* Platform RAM ECC Data Low Register Low */

    };                          /* end of ECSM_tag */
/*************************************************************************/
/*                          MODULE : EMIOS                               */
/*************************************************************************/
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
                uint32_t F31:1;
                uint32_t F30:1;
                uint32_t F29:1;
                uint32_t F28:1;
                uint32_t F27:1;
                uint32_t F26:1;
                uint32_t F25:1;
                uint32_t F24:1;
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
                uint32_t OU31:1;
                uint32_t OU30:1;
                uint32_t OU29:1;
                uint32_t OU28:1;
                uint32_t OU27:1;
                uint32_t OU26:1;
                uint32_t OU25:1;
                uint32_t OU24:1;
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
                uint32_t UC31:1;
                uint32_t UC30:1;
                uint32_t UC29:1;
                uint32_t UC28:1;
                uint32_t UC27:1;
                uint32_t UC26:1;
                uint32_t UC25:1;
                uint32_t UC24:1;
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

        } CH[32];

    };                          /* end of EMIOS_tag */
/*************************************************************************/
/*                             MODULE : eSCI                             */
/*************************************************************************/
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
                uint16_t RXPOL:1;
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
                uint16_t RN:1;
                uint16_t TN:1;
                uint16_t ERR:1;
                  uint16_t:1;
                uint16_t RD_11:4;
                uint16_t D:8;
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
                  uint32_t:2;
                uint32_t TACT:1;
                uint32_t RACT:1;
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
                  uint32_t:2;
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
            uint8_t R;
        } LTR;                  /* LIN Transmit Register */

        uint8_t eSCI_reserved1[3];

        union {
            uint8_t R;
        } LRR;                  /* LIN Recieve Register */

        uint8_t eSCI_reserved2[3];

        union {
            uint16_t R;
        } LPR;                  /* LIN CRC Polynom Register  */

        union {
            uint8_t R;
            struct {
                uint8_t:3;
                uint8_t SYNM:1;
                uint8_t EROE:1;
                uint8_t ERFE:1;
                uint8_t ERPE:1;
                uint8_t M2:1;
            } B;
        } CR3;                  /* Control Register 3 */

        uint8_t eSCI_reserved3[5];
    };                          /* end of ESCI_tag */
/*************************************************************************/
/*                             MODULE : FEC                              */
/*************************************************************************/
    struct FEC_tag {

        uint32_t fec_reserved_start;

        union {
            uint32_t R;
            struct {
                uint32_t HBERR:1;
                uint32_t BABR:1;
                uint32_t BABT:1;
                uint32_t GRA:1;
                uint32_t TXF:1;
                uint32_t TXB:1;
                uint32_t RXF:1;
                uint32_t RXB:1;
                uint32_t MII:1;
                uint32_t EBERR:1;
                uint32_t LC:1;
                uint32_t RL:1;
                uint32_t UN:1;
                  uint32_t:19;
            } B;
        } EIR;                  /*  Interrupt Event Register */

        union {
            uint32_t R;
            struct {
                uint32_t HBERR:1;
                uint32_t BABR:1;
                uint32_t BABT:1;
                uint32_t GRA:1;
                uint32_t TXF:1;
                uint32_t TXB:1;
                uint32_t RXF:1;
                uint32_t RXB:1;
                uint32_t MII:1;
                uint32_t EBERR:1;
                uint32_t LC:1;
                uint32_t RL:1;
                uint32_t UN:1;
                  uint32_t:19;
            } B;
        } EIMR;                 /*  Interrupt Mask Register  */

        uint32_t fec_reserved_eimr;

        union {
            uint32_t R;
            struct {
                uint32_t:7;
                uint32_t R_DES_ACTIVE:1;
                  uint32_t:24;
            } B;
        } RDAR;                 /*  Receive Descriptor Active Register  */

        union {
            uint32_t R;
            struct {
                uint32_t:7;
                uint32_t X_DES_ACTIVE:1;
                  uint32_t:24;
            } B;
        } TDAR;                 /*  Transmit Descriptor Active Register  */

        uint32_t fec_reserved_tdar[3];

        union {
            uint32_t R;
            struct {
                uint32_t:30;
                uint32_t ETHER_EN:1;
                uint32_t RESET:1;
            } B;
        } ECR;                  /*  Ethernet Control Register  */

        uint32_t fec_reserved_ecr[6];

        union {
            uint32_t R;
            struct {
                uint32_t ST:2;
                uint32_t OP:2;
                uint32_t PA:5;
                uint32_t RA:5;
                uint32_t TA:2;
                uint32_t DATA:16;
            } B;
        } MMFR;                 /* MII Data Register */

        union {
            uint32_t R;
            struct {
                uint32_t:24;
                uint32_t DIS_PREAMBLE:1;
                uint32_t MII_SPEED:6;
                  uint32_t:1;
            } B;
        } MSCR;                 /* MII Speed Control Register */

        uint32_t fec_reserved_mscr[7];

        union {
            uint32_t R;
            struct {
                uint32_t MIB_DISABLE:1;
                uint32_t MIB_IDLE:1;
                  uint32_t:30;
            } B;
        } MIBC;                 /* MIB Control Register */

        uint32_t fec_reserved_mibc[7];

        union {
            uint32_t R;
            struct {
                uint32_t:5;
                uint32_t MAX_FL:11;
                  uint32_t:10;
                uint32_t FCE:1;
                uint32_t BC_REJ:1;
                uint32_t PROM:1;
                uint32_t MII_MODE:1;
                uint32_t DRT:1;
                uint32_t LOOP:1;
            } B;
        } RCR;                  /* Receive Control Register */

        uint32_t fec_reserved_rcr[15];

        union {
            uint32_t R;
            struct {
                uint32_t:27;
                uint32_t RFC_PAUSE:1;
                uint32_t TFC_PAUSE:1;
                uint32_t FDEN:1;
                uint32_t HBC:1;
                uint32_t GTS:1;
            } B;
        } TCR;                  /* Transmit Control Register */

        uint32_t fec_reserved_tcr[7];

        union {
            uint32_t R;
            struct {
                uint32_t PADDR1:32;
            } B;
        } PALR;                 /* Physical Address Low Register */

        union {
            uint32_t R;
            struct {
                uint32_t PADDR2:16;
                uint32_t TYPE:16;
            } B;
        } PAUR;                 /* Physical Address High + Type Register */

        union {
            uint32_t R;
            struct {
                uint32_t OPCODE:16;
                uint32_t PAUSE_DUR:16;
            } B;
        } OPD;                  /* Opcode/Pause Duration Register */

        uint32_t fec_reserved_opd[10];

        union {
            uint32_t R;
            struct {
                uint32_t IADDR1:32;
            } B;
        } IAUR;                 /* Descriptor Individual Upper Address Register */

        union {
            uint32_t R;
            struct {
                uint32_t IADDR2:32;
            } B;
        } IALR;                 /* Descriptor Individual Lower Address Register */

        union {
            uint32_t R;
            struct {
                uint32_t GADDR1:32;
            } B;
        } GAUR;                 /* Descriptor Group Upper Address Register */

        union {
            uint32_t R;
            struct {
                uint32_t GADDR2:32;
            } B;
        } GALR;                 /* Descriptor Group Lower Address Register */

        uint32_t fec_reserved_galr[7];

        union {
            uint32_t R;
            struct {
                uint32_t:30;
                uint32_t X_WMRK:2;
            } B;
        } TFWR;                 /* FIFO Transmit FIFO Watermark Register */

        uint32_t fec_reserved_tfwr;

        union {
            uint32_t R;
            struct {
                uint32_t:22;
                uint32_t R_BOUND:8;
                  uint32_t:2;
            } B;
        } FRBR;                 /* FIFO Receive Bound Register */

        union {
            uint32_t R;
            struct {
                uint32_t:22;
                uint32_t R_FSTART:8;
                  uint32_t:2;
            } B;
        } FRSR;                 /* FIFO Receive Start Register */

        uint32_t fec_reserved_frsr[11];

        union {
            uint32_t R;
            struct {
                uint32_t R_DES_START:30;
                  uint32_t:2;
            } B;
        } ERDSR;                /* Receive Descriptor Ring Start Register */

        union {
            uint32_t R;
            struct {
                uint32_t X_DES_START:30;
                  uint32_t:2;
            } B;
        } ETDSR;                /* Transmit Descriptor Ring Start Register */

        union {
            uint32_t R;
            struct {
                uint32_t:21;
                uint32_t R_BUF_SIZE:7;
                  uint32_t:4;
            } B;
        } EMRBR;                /* Receive Buffer Size Register */

        uint32_t fec_reserved_emrbr[29];

        union {
            uint32_t R;
        } RMON_T_DROP;          /* Count of frames not counted correctly */

        union {
            uint32_t R;
        } RMON_T_PACKETS;       /* RMON Tx packet count */

        union {
            uint32_t R;
        } RMON_T_BC_PKT;        /* RMON Tx Broadcast Packets */

        union {
            uint32_t R;
        } RMON_T_MC_PKT;        /* RMON Tx Multicast Packets */

        union {
            uint32_t R;
        } RMON_T_CRC_ALIGN;     /* RMON Tx Packets w CRC/Align error */

        union {
            uint32_t R;
        } RMON_T_UNDERSIZE;     /* RMON Tx Packets < 64 bytes, good crc */

        union {
            uint32_t R;
        } RMON_T_OVERSIZE;      /* RMON Tx Packets > MAX_FL bytes, good crc */

        union {
            uint32_t R;
        } RMON_T_FRAG;          /* RMON Tx Packets < 64 bytes, bad crc */

        union {
            uint32_t R;
        } RMON_T_JAB;           /* RMON Tx Packets > MAX_FL bytes, bad crc */

        union {
            uint32_t R;
        } RMON_T_COL;           /* RMON Tx collision count */

        union {
            uint32_t R;
        } RMON_T_P64;           /* RMON Tx 64 byte packets */

        union {
            uint32_t R;
        } RMON_T_P65TO127;      /* RMON Tx 65 to 127 byte packets */

        union {
            uint32_t R;
        } RMON_T_P128TO255;     /* RMON Tx 128 to 255 byte packets */

        union {
            uint32_t R;
        } RMON_T_P256TO511;     /* RMON Tx 256 to 511 byte packets */

        union {
            uint32_t R;
        } RMON_T_P512TO1023;    /* RMON Tx 512 to 1023 byte packets */

        union {
            uint32_t R;
        } RMON_T_P1024TO2047;   /* RMON Tx 1024 to 2047 byte packets */

        union {
            uint32_t R;
        } RMON_T_P_GTE2048;     /* RMON Tx packets w > 2048 bytes */

        union {
            uint32_t R;
        } RMON_T_OCTETS;        /* RMON Tx Octets */

        union {
            uint32_t R;
        } IEEE_T_DROP;          /* Count of frames not counted correctly */

        union {
            uint32_t R;
        } IEEE_T_FRAME_OK;      /* Frames Transmitted OK */

        union {
            uint32_t R;
        } IEEE_T_1COL;          /* Frames Transmitted with Single Collision */

        union {
            uint32_t R;
        } IEEE_T_MCOL;          /* Frames Transmitted with Multiple Collisions */

        union {
            uint32_t R;
        } IEEE_T_DEF;           /* Frames Transmitted after Deferral Delay */

        union {
            uint32_t R;
        } IEEE_T_LCOL;          /* Frames Transmitted with Late Collision */

        union {
            uint32_t R;
        } IEEE_T_EXCOL;         /* Frames Transmitted with Excessive Collisions */

        union {
            uint32_t R;
        } IEEE_T_MACERR;        /* Frames Transmitted with Tx FIFO Underrun */

        union {
            uint32_t R;
        } IEEE_T_CSERR;         /* Frames Transmitted with Carrier Sense Error */

        union {
            uint32_t R;
        } IEEE_T_SQE;           /* Frames Transmitted with SQE Error */

        union {
            uint32_t R;
        } IEEE_T_FDXFC;         /* Flow Control Pause frames transmitted */

        union {
            uint32_t R;
        } IEEE_T_OCTETS_OK;     /* Octet count for Frames Transmitted w/o Error */

        uint32_t fec_reserved_rmon_t_octets_ok[2];

        union {
            uint32_t R;
        } RMON_R_DROP;          /*  Count of frames not counted correctly  */

        union {
            uint32_t R;
        } RMON_R_PACKETS;       /* RMON Rx packet count */

        union {
            uint32_t R;
        } RMON_R_BC_PKT;        /* RMON Rx Broadcast Packets */

        union {
            uint32_t R;
        } RMON_R_MC_PKT;        /* RMON Rx Multicast Packets */

        union {
            uint32_t R;
        } RMON_R_CRC_ALIGN;     /* RMON Rx Packets w CRC/Align error */

        union {
            uint32_t R;
        } RMON_R_UNDERSIZE;     /* RMON Rx Packets < 64 bytes, good crc */

        union {
            uint32_t R;
        } RMON_R_OVERSIZE;      /* RMON Rx Packets > MAX_FL bytes, good crc */

        union {
            uint32_t R;
        } RMON_R_FRAG;          /* RMON Rx Packets < 64 bytes, bad crc */

        union {
            uint32_t R;
        } RMON_R_JAB;           /* RMON Rx Packets > MAX_FL bytes, bad crc */

        uint32_t fec_reserved_rmon_r_jab;

        union {
            uint32_t R;
        } RMON_R_P64;           /* RMON Rx 64 byte packets */

        union {
            uint32_t R;
        } RMON_R_P65TO127;      /* RMON Rx 65 to 127 byte packets */

        union {
            uint32_t R;
        } RMON_R_P128TO255;     /* RMON Rx 128 to 255 byte packets */

        union {
            uint32_t R;
        } RMON_R_P256TO511;     /* RMON Rx 256 to 511 byte packets */

        union {
            uint32_t R;
        } RMON_R_P512TO1023;    /* RMON Rx 512 to 1023 byte packets */

        union {
            uint32_t R;
        } RMON_R_P1024TO2047;   /* RMON Rx 1024 to 2047 byte packets */

        union {
            uint32_t R;
        } RMON_R_P_GTE2048;     /* RMON Rx packets w > 2048 bytes */

        union {
            uint32_t R;
        } RMON_R_OCTETS;        /* RMON Rx Octets */

        union {
            uint32_t R;
        } IEEE_R_DROP;          /* Count of frames not counted correctly */

        union {
            uint32_t R;
        } IEEE_R_FRAME_OK;      /* Frames Received OK */

        union {
            uint32_t R;
        } IEEE_R_CRC;           /* Frames Received with CRC Error */

        union {
            uint32_t R;
        } IEEE_R_ALIGN;         /* Frames Received with Alignment Error */

        union {
            uint32_t R;
        } IEEE_R_MACERR;        /* Receive Fifo Overflow count */

        union {
            uint32_t R;
        } IEEE_R_FDXFC;         /* Flow Control Pause frames received */

        union {
            uint32_t R;
        } IEEE_R_OCTETS_OK;     /* Octet count for Frames Rcvd w/o Error */

    };                          /* end of FEC_tag */
/*************************************************************************/
/*                            MODULE : FLASH                             */
/*************************************************************************/
    struct FLASH_tag {
        union {
            uint32_t R;
            struct {
                uint32_t:5;
                uint32_t SIZE:3;
                  uint32_t:1;
                uint32_t LAS:3;
                  uint32_t:3;
                uint32_t MAS:1;
                uint32_t EER:1;
                uint32_t RWE:1;
                uint32_t SBC:1;
                  uint32_t:1;
                uint32_t PEAS:1;
                uint32_t DONE:1;
                uint32_t PEG:1;
                  uint32_t:4;
                uint32_t PGM:1;
                uint32_t PSUS:1;
                uint32_t ERS:1;
                uint32_t ESUS:1;
                uint32_t EHV:1;
            } B;
        } MCR;                  /* Module Configuration Register */

        union {
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
        } LML;                  /* Low/Mid-address space block locking Register */

        union {
            uint32_t R;
            struct {
                uint32_t HBE:1;
                  uint32_t:25;
                uint32_t HBLOCK:6;
            } B;
        } HBL;                  /* High-address space block locking Register */

        union {
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
        } SLL;                  /* Secondary low/mid-address space block locking Register */

        union {
            uint32_t R;
            struct {
                uint32_t:14;
                uint32_t MSEL:2;
                  uint32_t:6;
                uint32_t LSEL:10;
            } B;
        } LMS;                  /* Low/Mid-address space block locking Register */

        union {
            uint32_t R;
            struct {
                uint32_t:26;
                uint32_t HBSEL:6;
            } B;
        } HBS;                  /* High-address space block locking Register */

        union {
            uint32_t R;
            struct {
                uint32_t SAD:1;
                  uint32_t:10;
                uint32_t ADDR:18;
                  uint32_t:3;
            } B;
        } ADR;                  /* Address Register */

        union {
            uint32_t R;
            struct {
                uint32_t LBCFG:4;
                uint32_t ARB:1;
                uint32_t PRI:1;
                  uint32_t:1;
                uint32_t M8PFE:1;
                  uint32_t:1;
                uint32_t M6PFE:1;
                uint32_t M5PFE:1;
                uint32_t M4PFE:1;
                  uint32_t:1;
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
        } PFCRP0;               /* Platform Flash Configuration Register for Port 0 */

        union {
            uint32_t R;
            struct {
                uint32_t LBCFG:4;
                  uint32_t:3;
                uint32_t M8PFE:1;
                  uint32_t:1;
                uint32_t M6PFE:1;
                uint32_t M5PFE:1;
                uint32_t M4PFE:1;
                  uint32_t:1;
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
        } PFCRP1;               /* Platform Flash Configuration Register for Port 1 */

        union {
            uint32_t R;
            struct {
                uint32_t M7AP:2;
                uint32_t M6AP:2;
                uint32_t M5AP:2;
                uint32_t M4AP:2;
                uint32_t M3AP:2;
                uint32_t M2AP:2;
                uint32_t M1AP:2;
                uint32_t M0AP:2;
                uint32_t SHSACC:4;
                  uint32_t:4;
                uint32_t SHDACC:4;
                  uint32_t:4;
            } B;
        } PFAPR;                /* Platform Flash access protection Register */

        union {
            uint32_t R;
            struct {
                uint32_t:1;
                uint32_t SACC:31;
            } B;
        } PFSACC;               /* PFlash Supervisor Access Control Register */

        union {
            uint32_t R;
            struct {
                uint32_t:1;
                uint32_t DACC:31;
            } B;
        } PFDACC;               /* PFlash Data Access Control Register */

        uint32_t FLASH_reserved1[3];

        union {
            uint32_t R;
            struct {
                uint32_t UTE:1;
                uint32_t SCBE:1;
                  uint32_t:6;
                uint32_t DSI:8;
                  uint32_t:10;
                uint32_t MRE:1;
                uint32_t MRV:1;
                uint32_t EIE:1;
                uint32_t AIS:1;
                uint32_t AIE:1;
                uint32_t AID:1;
            } B;
        } UT0;                  /* User Test Register 0 */

        union {
            uint32_t R;
            struct {
                uint32_t DAI:32;
            } B;
        } UT1;                  /* User Test Register 1 */

        union {
            uint32_t R;
            struct {
                uint32_t DAI:32;
            } B;
        } UT2;                  /* User Test Register 2 */

        union {
            uint32_t R;
            struct {
                uint32_t MISR:32;
            } B;
        } MISR[5];              /* Multiple Input Signature Register */

    };                          /* end of FLASH_tag */
/*************************************************************************/
/*                          MODULE : FlexCAN                             */
/*************************************************************************/
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
                /* uint8_t  B[8];  Data buffer in Bytes (8 bits) */
                /* uint16_t H[4];  Data buffer in Half-words (16 bits) */
                uint32_t W[2]; /* Data buffer in words (32 bits) */
                /* uint32_t R[2];    Data buffer in words (32 bits) */
            } DATA;

        } BUF[64];

        uint32_t FLEXCAN_reserved3[256];

        union {
            uint32_t R;
            struct {
                uint32_t MI:32;
            } B;
        } RXIMR[64];            /* RX Individual Mask Registers */

    };                          /* end of CTU_tag */
/**************************************************************************/
/*                          MODULE : FlexRay                              */
/**************************************************************************/

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
/**************************************************************************/
/*                     MODULE : FMPLL                                     */
/**************************************************************************/
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
/*************************************************************************/
/*                          MODULE : i2c                                 */
/*************************************************************************/
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

    };                          /* end of i2c_tag */
/*************************************************************************/
/*                          MODULE : INTC                                */
/*************************************************************************/
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
        } CPR;                  /* Processor 0 (Z6) Current Priority Register */

        union {
            uint32_t R;
            struct {
                uint32_t:28;
                uint32_t PRI:4;
            } B;
        } CPR_PRC1;             /* Processor 1 (Z0) Current Priority Register */

        union {
            uint32_t R;
            struct {
                uint32_t VTBA:21;
                uint32_t INTVEC:9;
                  uint32_t:2;
            } B;
        } IACKR;                /* Processor 0 (Z6) Interrupt Acknowledge Register */

        union {
            uint32_t R;
            struct {
                uint32_t VTBA_PRC1:21;
                uint32_t INTVEC_PRC1:9;
                  uint32_t:2;
            } B;
        } IACKR_PRC1;           /* Processor 1 (Z0) Interrupt Acknowledge Register */

        union {
            uint32_t R;
            struct {
                uint32_t:32;
            } B;
        } EOIR;                 /* Processor 0 End of Interrupt Register */

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
        } PSR[316];             /* Software Set/Clear Interrupt Register */

    };                          /* end of INTC_tag */
/*************************************************************************/
/*                           MODULE : MLB                                */
/*************************************************************************/
    struct MLB_tag {

        union {
            uint32_t R;
            struct {
                uint32_t MDE:1;
                uint32_t LBM:1;
                uint32_t MCS:2;
                  uint32_t:1;
                uint32_t MLK:1;
                uint32_t MLE:1;
                uint32_t MHRE:1;
                uint32_t MRS:1;
                  uint32_t:15;
                uint32_t MDA:8;
            } B;
        } DCCR;                 /* Device Control Configuration Register */

        union {
            uint32_t R;
            struct {
                uint32_t:24;
                uint32_t SSRE:1;
                uint32_t SDMU:1;
                uint32_t SDML:1;
                uint32_t SDSC:1;
                uint32_t SDCS:1;
                uint32_t SDNU:1;
                uint32_t SDNL:1;
                uint32_t SDR:1;
            } B;
        } SSCR;                 /* MLB Blank Register */

        union {
            uint32_t R;
            struct {
                uint32_t MSD:32;
            } B;
        } SDCR;                 /* MLB Status Register */

        union {
            uint32_t R;
            struct {
                uint32_t:25;
                uint32_t SMMU:1;
                uint32_t SMML:1;
                uint32_t SMSC:1;
                uint32_t SMCS:1;
                uint32_t SMNU:1;
                uint32_t SMNL:1;
                uint32_t SMR:1;
            } B;
        } SMCR;                 /* RX Control Channel Address Register */

        uint32_t MLB_reserved1[3];

        union {
            uint32_t R;
            struct {
                uint32_t UMA:8;
                uint32_t UMI:8;
                uint32_t MMA:8;
                uint32_t MMI:8;
            } B;
        } VCCR;                 /* Version Control Configuration Register */

        union {
            uint32_t R;
            struct {
                uint32_t SRBA:16;
                uint32_t STBA:16;
            } B;
        } SBCR;                 /* Sync Base Address Config Register */

        union {
            uint32_t R;
            struct {
                uint32_t ARBA:16;
                uint32_t ATBA:16;
            } B;
        } ABCR;                 /* Async Base Address Channel Config Register */

        union {
            uint32_t R;
            struct {
                uint32_t CRBA:16;
                uint32_t CTBA:16;
            } B;
        } CBCR;                 /* Control Base Address Config Register */

        union {
            uint32_t R;
            struct {
                uint32_t IRBA:16;
                uint32_t ITBA:16;
            } B;
        } IBCR;                 /* Isochronous Base Address Config Register */

        union {
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t CSU:16;
            } B;
        } CICR;                 /* Channel Interrupt Config Register */

        uint32_t MLB_reserved2[3];

        struct mlbch_t {

            union {
                uint32_t R;
                struct {
                    uint32_t CE:1;
                    uint32_t TR:1;
                    uint32_t CT:2;
                    uint32_t FSE_FCE:1;
                    uint32_t MDS:2;
                      uint32_t:2;
                    uint32_t MLFS:1;
                      uint32_t:1;
                    uint32_t MBE:1;
                    uint32_t MBS:1;
                    uint32_t MBD:1;
                    uint32_t MDB:1;
                    uint32_t MPE:1;
                    uint32_t FSCD_IPL:1;
                    uint32_t IPL:2;
                    uint32_t FSPC_IPL:5;
                    uint32_t CA:8;
                } B;
            } CECR;             /* Channel Entry Config Register */

            union {
                uint32_t R;
                struct {
                    uint32_t BM:1;
                    uint32_t BF:1;
                      uint32_t:10;
                    uint32_t IVB:2;
                    uint32_t GIRB_GB:1;
                    uint32_t RDY:1;
                      uint32_t:4;
                    uint32_t PBS:1;
                    uint32_t PBD:1;
                    uint32_t PBDB:1;
                    uint32_t PBPE:1;
                      uint32_t:1;
                    uint32_t LFS:1;
                    uint32_t HBE:1;
                    uint32_t BE:1;
                    uint32_t CBS:1;
                    uint32_t CBD:1;
                    uint32_t CBDB:1;
                    uint32_t CBPE:1;
                } B;
            } CSCR;             /* Channel Status Config Register */

            union {
                uint32_t R;
                struct {
                    uint32_t BCA:16;
                    uint32_t BFA:16;
                } B;
            } CCBCR;            /* Channel Current Buffer Config Register */

            union {
                uint32_t R;
                struct {
                    uint32_t BSA:16;
                    uint32_t BEA:16;
                } B;
            } CNBCR;            /* Channel Next Buffer Config Register */

        } CH[16];

        uint32_t MLB_reserved3[80];

        union {
            uint32_t R;
            struct {
                uint32_t BSA:16;
                uint32_t BEA:16;
            } B;
        } LCBCR[16];            /* Local Channel Buffer Config Register */

    };                          /* end of MLB_tag */
/*************************************************************************/
/*                          MODULE : MPU                                 */
/*************************************************************************/
    struct MPU_tag {
        union {
            uint32_t R;
            struct {
                uint32_t MPERR:8;
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
        } EAR0;                 /* Error Address Register */

        union {
            uint32_t R;
            struct {
                uint32_t EACD:16;
                uint32_t EPID:8;
                uint32_t EMN:4;
                uint32_t EATTR:3;
                uint32_t ERW:1;
            } B;
        } EDR0;                 /* Error Detail Register */

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
        } EDR3;

        union {
            uint32_t R;
            struct {
                uint32_t EADDR:32;
            } B;
        } EAR3;

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

        uint32_t mpu_reserved2[244];

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
                    uint32_t:2;
                    uint32_t M6RE:1;
                    uint32_t M6WE:1;
                    uint32_t M5RE:1;
                    uint32_t M5WE:1;
                    uint32_t M4RE:1;
                    uint32_t M4WE:1;
                      uint32_t:6;
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
                uint32_t:2;
                uint32_t M6RE:1;
                uint32_t M6WE:1;
                uint32_t M5RE:1;
                uint32_t M5WE:1;
                uint32_t M4RE:1;
                uint32_t M4WE:1;
                  uint32_t:6;
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
/**************************************************************************/
/*                          MODULE : pit                                  */
/**************************************************************************/
    struct PIT_tag {

        union PIT_MCR_tag {
            uint32_t R;
            struct {
                uint32_t:30;
                uint32_t MDIS:1;
                uint32_t FRZ:1;
            } B;
        } PITMCR;

        uint32_t pit_reserved1[59];

        struct PIT_CHANNEL_tag {
            union {
                uint32_t R;
                struct {
                    uint32_t TSV:32;
                } B;
            } LDVAL;

            union {
                uint32_t R;
                struct {
                    uint32_t TVL:32;
                } B;
            } CVAL;

            union PIT_TCTRL_tag {
                uint32_t R;
                struct {
                    uint32_t:30;
                    uint32_t TIE:1;
                    uint32_t TEN:1;
                } B;
            } TCTRL;

            union PIT_TFLG_tag {
                uint32_t R;
                struct {
                    uint32_t:31;
                    uint32_t TIF:1;
                } B;
            } TFLG;
        } CHANNEL[9];
    };

    /* Compatibility with MPC5643L */
    typedef struct PIT_CHANNEL_tag PIT_RTI_CHANNEL_tag;
    typedef union PIT_MCR_tag PIT_RTI_PITMCR_32B_tag;
    typedef union PIT_TCTRL_tag PIT_RTI_TCTRL_32B_tag;
    typedef union PIT_TFLG_tag PIT_RTI_TFLG_32B_tag;
/**************************************************************************/
/*                          MODULE : sem4                                 */
/**************************************************************************/
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
/*************************************************************************/
/*                            MODULE : SIU                               */
/*************************************************************************/
    struct SIU_tag {

        int32_t SIU_reserved0;

        union {
            uint32_t R;
            struct {
                uint32_t PARTNUM:16;
                uint32_t CSP:1;
                uint32_t PKG:5;
                  uint32_t:2;
                uint32_t MASKNUM_MAJOR:4;
                uint32_t MASKNUM_MINOR:4;
            } B;
        } MIDR;                 /* MCU ID Register */

        int32_t SIU_reserved1;

        union {
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
        } RSR;                  /* Reset Status Register */

        union {
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
        } SRCR;                 /* System Reset Control Register */

        union {
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
        } EISR;                 /* External Interrupt Status Register */

        union {
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
        } DIRER;                /* DMA/Interrupt Request Enable Register */

        union {
            uint32_t R;
            struct {
                uint32_t:30;
                uint32_t DIRS1:1;
                uint32_t DIRS0:1;
            } B;
        } DIRSR;                /* DMA/Interrupt Select Register */

        union {
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
        } OSR;                  /* Overrun Status Register */

        union {
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
        } ORER;                 /* Overrun Request Enable Register */

        union {
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
        } IREER;                /* External IRQ Rising-Edge Event Enable Register */

        union {
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
        } IFEER;                /* External IRQ Falling-Edge Event Enable Register */

        union {
            uint32_t R;
            struct {
                uint32_t:28;
                uint32_t DFL:4;
            } B;
        } IDFR;                 /* External IRQ Digital Filter Register */

        union {
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
        } IFIR;                 /* External IRQ Filtered Input Register */

        int32_t SIU_reserved2[2];

        union SIU_PCR_tag {
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
        } PCR[155];             /* Pad Configuration Registers */

        int32_t SIU_reserved3[290];

        union {
            uint8_t R;
            struct {
                uint8_t:7;
                uint8_t PDO:1;
            } B;
        } GPDO[155];            /* GPIO Pin Data Output Registers */

        int8_t SIU_reserved4[357];

        union {
            uint8_t R;
            struct {
                uint8_t:7;
                uint8_t PDI:1;
            } B;
        } GPDI[155];            /* GPIO Pin Data Input Registers */

        int32_t SIU_reserved5[26];

        union {
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
        } ISEL1;                /* IMUX Register */

        union {
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
        } ISEL2;                /* IMUX Register */

        int32_t SIU_reserved6;

        union {
            uint32_t R;
            struct {
                uint32_t:17;
                uint32_t TSEL1:7;
                  uint32_t:1;
                uint32_t TSEL0:7;
            } B;
        } ISEL4;                /* IMUX Register */

        int32_t SIU_reserved7[27];

        union {
            uint32_t R;
            struct {
                uint32_t:14;
                uint32_t MATCH:1;
                uint32_t DISNEX:1;
                  uint32_t:8;
                uint32_t TESTLOCK:1;
                  uint32_t:7;
            } B;
        } CCR;                  /* Chip Configuration Register Register */

        union {
            uint32_t R;
            struct {
                uint32_t:28;
                uint32_t ECEN:1;
                  uint32_t:1;
                uint32_t ECDF:2;
            } B;
        } ECCR;                 /* External Clock Configuration Register Register */

        union {
            uint32_t R;
        } GPR0;                 /* General Purpose Register 0 */

        union {
            uint32_t R;
        } GPR1;                 /* General Purpose Register 1 */

        union {
            uint32_t R;
        } GPR2;                 /* General Purpose Register 2 */

        union {
            uint32_t R;
        } GPR3;                 /* General Purpose Register 3 */

        int32_t SIU_reserved8[2];

        union {
            uint32_t R;
            struct {
                uint32_t SYSCLKSEL:2;
                uint32_t SYSCLKDIV:3;
                  uint32_t:19;
                uint32_t LPCLKDIV3:2;
                uint32_t LPCLKDIV2:2;
                uint32_t LPCLKDIV1:2;
                uint32_t LPCLKDIV0:2;
            } B;
        } SYSCLK;               /* System CLock Register */

        union {
            uint32_t R;
            struct {
                uint32_t:6;
                uint32_t HLT6:1;
                uint32_t HLT7:1;
                  uint32_t:1;
                uint32_t HLT9:1;
                uint32_t HLT10:1;
                uint32_t HLT11:1;
                uint32_t HLT12:1;
                uint32_t HLT13:1;
                uint32_t HLT14:1;
                uint32_t HLT15:1;
                uint32_t HLT16:1;
                uint32_t HLT17:1;
                uint32_t HLT18:1;
                uint32_t HLT19:1;
                uint32_t HLT20:1;
                uint32_t HLT21:1;
                uint32_t HLT22:1;
                uint32_t HLT23:1;
                  uint32_t:2;
                uint32_t HLT26:1;
                uint32_t HLT27:1;
                uint32_t HLT28:1;
                uint32_t HLT29:1;
                  uint32_t:1;
                uint32_t HLT31:1;
            } B;
        } HLT0;                 /* Halt Register 0 */

        union {
            uint32_t R;
            struct {
                uint32_t:3;
                uint32_t HLT3:1;
                uint32_t HLT4:1;
                  uint32_t:15;
                uint32_t HLT20:1;
                uint32_t HLT21:1;
                uint32_t HLT22:1;
                uint32_t HLT23:1;
                  uint32_t:2;
                uint32_t HLT26:1;
                uint32_t HLT27:1;
                uint32_t HLT28:1;
                uint32_t HLT29:1;
                  uint32_t:2;
            } B;
        } HLT1;                 /* Halt Register 1 */

        union {
            uint32_t R;
            struct {
                uint32_t:6;
                uint32_t HLTACK6:1;
                uint32_t HLTACK7:1;
                  uint32_t:1;
                uint32_t HLTACK9:1;
                uint32_t HLTACK10:1;
                uint32_t HLTACK11:1;
                uint32_t HLTACK12:1;
                uint32_t HLTACK13:1;
                uint32_t HLTACK14:1;
                uint32_t HLTACK15:1;
                uint32_t HLTACK16:1;
                uint32_t HLTACK17:1;
                uint32_t HLTACK18:1;
                uint32_t HLTACK19:1;
                uint32_t HLTACK20:1;
                uint32_t HLTACK21:1;
                uint32_t HLTACK22:1;
                uint32_t HLTACK23:1;
                  uint32_t:2;
                uint32_t HLTACK26:1;
                uint32_t HLTACK27:1;
                uint32_t HLTACK28:1;
                uint32_t HLTACK29:1;
                  uint32_t:1;
                uint32_t HLTACK31:1;
            } B;
        } HLTACK0;              /* Halt Acknowledge Register 0 */

        union {
            uint32_t R;
            struct {
                uint32_t HLTACK0:1;
                uint32_t HLTACK1:1;
                  uint32_t:1;
                uint32_t HLTACK3:1;
                uint32_t HLTACK4:1;
                  uint32_t:11;
                uint32_t HLTACK20:1;
                uint32_t HLTACK21:1;
                uint32_t HLTACK22:1;
                uint32_t HLTACK23:1;
                  uint32_t:2;
                uint32_t HLTACK26:1;
                uint32_t HLTACK27:1;
                uint32_t HLTACK28:1;
                uint32_t HLTACK29:1;
                  uint32_t:2;
            } B;
        } HLTACK1;              /* Halt Acknowledge Register 0 */

        union {
            uint32_t R;
            struct {
                uint32_t EMIOSSEL31:4;
                uint32_t EMIOSSEL30:4;
                uint32_t EMIOSSEL29:4;
                uint32_t EMIOSSEL28:4;
                uint32_t EMIOSSEL27:4;
                uint32_t EMIOSSEL26:4;
                uint32_t EMIOSSEL25:4;
                uint32_t EMIOSSEL24:4;
            } B;
        } EMIOS_SEL0;           /* eMIOS Select Register 0 */

        union {
            uint32_t R;
            struct {
                uint32_t EMIOSSEL23:4;
                uint32_t EMIOSSEL22:4;
                uint32_t EMIOSSEL21:4;
                uint32_t EMIOSSEL20:4;
                uint32_t EMIOSSEL19:4;
                uint32_t EMIOSSEL18:4;
                uint32_t EMIOSSEL17:4;
                uint32_t EMIOSSEL16:4;
            } B;
        } EMIOS_SEL1;           /* eMIOS Select Register 1 */

        union {
            uint32_t R;
            struct {
                uint32_t EMIOSSEL15:4;
                uint32_t EMIOSSEL14:4;
                uint32_t EMIOSSEL13:4;
                uint32_t EMIOSSEL12:4;
                uint32_t EMIOSSEL11:4;
                uint32_t EMIOSSEL10:4;
                uint32_t EMIOSSEL9:4;
                uint32_t EMIOSSEL8:4;
            } B;
        } EMIOS_SEL2;           /* eMIOS Select Register 2 */

        union {
            uint32_t R;
            struct {
                uint32_t EMIOSSEL7:4;
                uint32_t EMIOSSEL6:4;
                uint32_t EMIOSSEL5:4;
                uint32_t EMIOSSEL4:4;
                uint32_t EMIOSSEL3:4;
                uint32_t EMIOSSEL2:4;
                uint32_t EMIOSSEL1:4;
                uint32_t EMIOSSEL0:4;
            } B;
        } EMIOS_SEL3;           /* eMIOS Select Register 3 */

        union {
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
        } ISEL2A;               /* External Interrupt Select Register 2A */

        int32_t SIU_reserved9[142];

        union {
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t PB:16;
            } B;
        } PGPDO0;               /* Parallel GPIO Pin Data Output Register */

        union {
            uint32_t R;
            struct {
                uint32_t PC:16;
                uint32_t PD:16;
            } B;
        } PGPDO1;               /* Parallel GPIO Pin Data Output Register */

        union {
            uint32_t R;
            struct {
                uint32_t PE:16;
                uint32_t PF:16;
            } B;
        } PGPDO2;               /* Parallel GPIO Pin Data Output Register */

        union {
            uint32_t R;
            struct {
                uint32_t PG:16;
                uint32_t PH:16;
            } B;
        } PGPDO3;               /* Parallel GPIO Pin Data Output Register */

        union {
            uint32_t R;
            struct {
                uint32_t PJ:16;
                uint32_t PK:11;
                  uint32_t:5;
            } B;
        } PGPDO4;               /* Parallel GPIO Pin Data Output Register */

        int32_t SIU_reserved10[11];

        union {
            uint32_t R;
            struct {
                uint32_t PA:16;
                uint32_t PB:16;
            } B;
        } PGPDI0;               /* Parallel GPIO Pin Data Input Register */

        union {
            uint32_t R;
            struct {
                uint32_t PC:16;
                uint32_t PD:16;
            } B;
        } PGPDI1;               /* Parallel GPIO Pin Data Input Register */

        union {
            uint32_t R;
            struct {
                uint32_t PE:16;
                uint32_t PF:16;
            } B;
        } PGPDI2;               /* Parallel GPIO Pin Data Input Register */

        union {
            uint32_t R;
            struct {
                uint32_t PG:16;
                uint32_t PH:16;
            } B;
        } PGPDI3;               /* Parallel GPIO Pin Data Input Register */

        union {
            uint32_t R;
            struct {
                uint32_t PJ:16;
                uint32_t PK:11;
                  uint32_t:5;
            } B;
        } PGPDI4;               /* Parallel GPIO Pin Data Input Register */

        int32_t SIU_reserved11[12];

        union {
            uint32_t R;
            struct {
                uint32_t PB_MASK:16;
                uint32_t PB:16;
            } B;
        } MPGPDO1;              /* Masked Parallel GPIO Pin Data Input Register */

        union {
            uint32_t R;
            struct {
                uint32_t PC_MASK:16;
                uint32_t PC:16;
            } B;
        } MPGPDO2;              /* Masked Parallel GPIO Pin Data Input Register */

        union {
            uint32_t R;
            struct {
                uint32_t PD_MASK:16;
                uint32_t PD:16;
            } B;
        } MPGPDO3;              /* Masked Parallel GPIO Pin Data Input Register */

        union {
            uint32_t R;
            struct {
                uint32_t PE_MASK:16;
                uint32_t PE:16;
            } B;
        } MPGPDO4;              /* Masked Parallel GPIO Pin Data Input Register */

        union {
            uint32_t R;
            struct {
                uint32_t PF_MASK:16;
                uint32_t PF:16;
            } B;
        } MPGPDO5;              /* Masked Parallel GPIO Pin Data Input Register */

        union {
            uint32_t R;
            struct {
                uint32_t PG_MASK:16;
                uint32_t PG:16;
            } B;
        } MPGPDO6;              /* Masked Parallel GPIO Pin Data Input Register */

        union {
            uint32_t R;
            struct {
                uint32_t PH_MASK:16;
                uint32_t PH:16;
            } B;
        } MPGPDO7;              /* Masked Parallel GPIO Pin Data Input Register */

        union {
            uint32_t R;
            struct {
                uint32_t PJ_MASK:16;
                uint32_t PJ:16;
            } B;
        } MPGPDO8;              /* Masked Parallel GPIO Pin Data Input Register */

        union {
            uint32_t R;
            struct {
                uint32_t PK_MASK:11;
                  uint32_t:5;
                uint32_t PK:11;
                  uint32_t:5;
            } B;
        } MPGPDO9;              /* Masked Parallel GPIO Pin Data Input Register */

        int32_t SIU_reserved12[22];

        union {
            uint32_t R;
            struct {
                uint32_t MASK31:1;
                uint32_t MASK30:1;
                uint32_t MASK29:1;
                uint32_t MASK28:1;
                uint32_t MASK27:1;
                uint32_t MASK26:1;
                uint32_t MASK25:1;
                uint32_t MASK24:1;
                uint32_t MASK23:1;
                uint32_t MASK22:1;
                uint32_t MASK21:1;
                uint32_t MASK20:1;
                uint32_t MASK19:1;
                uint32_t MASK18:1;
                uint32_t MASK17:1;
                uint32_t MASK16:1;
                uint32_t DATA31:1;
                uint32_t DATA30:1;
                uint32_t DATA29:1;
                uint32_t DATA28:1;
                uint32_t DATA27:1;
                uint32_t DATA26:1;
                uint32_t DATA25:1;
                uint32_t DATA24:1;
                uint32_t DATA23:1;
                uint32_t DATA22:1;
                uint32_t DATA21:1;
                uint32_t DATA20:1;
                uint32_t DATA19:1;
                uint32_t DATA18:1;
                uint32_t DATA17:1;
                uint32_t DATA16:1;
            } B;
        } DSPIAH;               /* Masked Serial GPO for DSPI_A High Register */

        union {
            uint32_t R;
            struct {
                uint32_t MASK15:1;
                uint32_t MASK14:1;
                uint32_t MASK13:1;
                uint32_t MASK12:1;
                uint32_t MASK11:1;
                uint32_t MASK10:1;
                uint32_t MASK9:1;
                uint32_t MASK8:1;
                uint32_t MASK7:1;
                uint32_t MASK6:1;
                uint32_t MASK5:1;
                uint32_t MASK4:1;
                uint32_t MASK3:1;
                uint32_t MASK2:1;
                uint32_t MASK1:1;
                uint32_t MASK0:1;
                uint32_t DATA15:1;
                uint32_t DATA14:1;
                uint32_t DATA13:1;
                uint32_t DATA12:1;
                uint32_t DATA11:1;
                uint32_t DATA10:1;
                uint32_t DATA9:1;
                uint32_t DATA8:1;
                uint32_t DATA7:1;
                uint32_t DATA6:1;
                uint32_t DATA5:1;
                uint32_t DATA4:1;
                uint32_t DATA3:1;
                uint32_t DATA2:1;
                uint32_t DATA1:1;
                uint32_t DATA0:1;
            } B;
        } DSPIAL;               /* Masked Serial GPO for DSPI_A Low Register */

        union {
            uint32_t R;
            struct {
                uint32_t MASK31:1;
                uint32_t MASK30:1;
                uint32_t MASK29:1;
                uint32_t MASK28:1;
                uint32_t MASK27:1;
                uint32_t MASK26:1;
                uint32_t MASK25:1;
                uint32_t MASK24:1;
                uint32_t MASK23:1;
                uint32_t MASK22:1;
                uint32_t MASK21:1;
                uint32_t MASK20:1;
                uint32_t MASK19:1;
                uint32_t MASK18:1;
                uint32_t MASK17:1;
                uint32_t MASK16:1;
                uint32_t DATA31:1;
                uint32_t DATA30:1;
                uint32_t DATA29:1;
                uint32_t DATA28:1;
                uint32_t DATA27:1;
                uint32_t DATA26:1;
                uint32_t DATA25:1;
                uint32_t DATA24:1;
                uint32_t DATA23:1;
                uint32_t DATA22:1;
                uint32_t DATA21:1;
                uint32_t DATA20:1;
                uint32_t DATA19:1;
                uint32_t DATA18:1;
                uint32_t DATA17:1;
                uint32_t DATA16:1;
            } B;
        } DSPIBH;               /* Masked Serial GPO for DSPI_B High Register */

        union {
            uint32_t R;
            struct {
                uint32_t MASK15:1;
                uint32_t MASK14:1;
                uint32_t MASK13:1;
                uint32_t MASK12:1;
                uint32_t MASK11:1;
                uint32_t MASK10:1;
                uint32_t MASK9:1;
                uint32_t MASK8:1;
                uint32_t MASK7:1;
                uint32_t MASK6:1;
                uint32_t MASK5:1;
                uint32_t MASK4:1;
                uint32_t MASK3:1;
                uint32_t MASK2:1;
                uint32_t MASK1:1;
                uint32_t MASK0:1;
                uint32_t DATA15:1;
                uint32_t DATA14:1;
                uint32_t DATA13:1;
                uint32_t DATA12:1;
                uint32_t DATA11:1;
                uint32_t DATA10:1;
                uint32_t DATA9:1;
                uint32_t DATA8:1;
                uint32_t DATA7:1;
                uint32_t DATA6:1;
                uint32_t DATA5:1;
                uint32_t DATA4:1;
                uint32_t DATA3:1;
                uint32_t DATA2:1;
                uint32_t DATA1:1;
                uint32_t DATA0:1;
            } B;
        } DSPIBL;               /* Masked Serial GPO for DSPI_B Low Register */

        union {
            uint32_t R;
            struct {
                uint32_t MASK31:1;
                uint32_t MASK30:1;
                uint32_t MASK29:1;
                uint32_t MASK28:1;
                uint32_t MASK27:1;
                uint32_t MASK26:1;
                uint32_t MASK25:1;
                uint32_t MASK24:1;
                uint32_t MASK23:1;
                uint32_t MASK22:1;
                uint32_t MASK21:1;
                uint32_t MASK20:1;
                uint32_t MASK19:1;
                uint32_t MASK18:1;
                uint32_t MASK17:1;
                uint32_t MASK16:1;
                uint32_t DATA31:1;
                uint32_t DATA30:1;
                uint32_t DATA29:1;
                uint32_t DATA28:1;
                uint32_t DATA27:1;
                uint32_t DATA26:1;
                uint32_t DATA25:1;
                uint32_t DATA24:1;
                uint32_t DATA23:1;
                uint32_t DATA22:1;
                uint32_t DATA21:1;
                uint32_t DATA20:1;
                uint32_t DATA19:1;
                uint32_t DATA18:1;
                uint32_t DATA17:1;
                uint32_t DATA16:1;
            } B;
        } DSPICH;               /* Masked Serial GPO for DSPI_C High Register */

        union {
            uint32_t R;
            struct {
                uint32_t MASK15:1;
                uint32_t MASK14:1;
                uint32_t MASK13:1;
                uint32_t MASK12:1;
                uint32_t MASK11:1;
                uint32_t MASK10:1;
                uint32_t MASK9:1;
                uint32_t MASK8:1;
                uint32_t MASK7:1;
                uint32_t MASK6:1;
                uint32_t MASK5:1;
                uint32_t MASK4:1;
                uint32_t MASK3:1;
                uint32_t MASK2:1;
                uint32_t MASK1:1;
                uint32_t MASK0:1;
                uint32_t DATA15:1;
                uint32_t DATA14:1;
                uint32_t DATA13:1;
                uint32_t DATA12:1;
                uint32_t DATA11:1;
                uint32_t DATA10:1;
                uint32_t DATA9:1;
                uint32_t DATA8:1;
                uint32_t DATA7:1;
                uint32_t DATA6:1;
                uint32_t DATA5:1;
                uint32_t DATA4:1;
                uint32_t DATA3:1;
                uint32_t DATA2:1;
                uint32_t DATA1:1;
                uint32_t DATA0:1;
            } B;
        } DSPICL;               /* Masked Serial GPO for DSPI_C Low Register */

        union {
            uint32_t R;
            struct {
                uint32_t MASK31:1;
                uint32_t MASK30:1;
                uint32_t MASK29:1;
                uint32_t MASK28:1;
                uint32_t MASK27:1;
                uint32_t MASK26:1;
                uint32_t MASK25:1;
                uint32_t MASK24:1;
                uint32_t MASK23:1;
                uint32_t MASK22:1;
                uint32_t MASK21:1;
                uint32_t MASK20:1;
                uint32_t MASK19:1;
                uint32_t MASK18:1;
                uint32_t MASK17:1;
                uint32_t MASK16:1;
                uint32_t DATA31:1;
                uint32_t DATA30:1;
                uint32_t DATA29:1;
                uint32_t DATA28:1;
                uint32_t DATA27:1;
                uint32_t DATA26:1;
                uint32_t DATA25:1;
                uint32_t DATA24:1;
                uint32_t DATA23:1;
                uint32_t DATA22:1;
                uint32_t DATA21:1;
                uint32_t DATA20:1;
                uint32_t DATA19:1;
                uint32_t DATA18:1;
                uint32_t DATA17:1;
                uint32_t DATA16:1;
            } B;
        } DSPIDH;               /* Masked Serial GPO for DSPI_D High Register */

        union {
            uint32_t R;
            struct {
                uint32_t MASK15:1;
                uint32_t MASK14:1;
                uint32_t MASK13:1;
                uint32_t MASK12:1;
                uint32_t MASK11:1;
                uint32_t MASK10:1;
                uint32_t MASK9:1;
                uint32_t MASK8:1;
                uint32_t MASK7:1;
                uint32_t MASK6:1;
                uint32_t MASK5:1;
                uint32_t MASK4:1;
                uint32_t MASK3:1;
                uint32_t MASK2:1;
                uint32_t MASK1:1;
                uint32_t MASK0:1;
                uint32_t DATA15:1;
                uint32_t DATA14:1;
                uint32_t DATA13:1;
                uint32_t DATA12:1;
                uint32_t DATA11:1;
                uint32_t DATA10:1;
                uint32_t DATA9:1;
                uint32_t DATA8:1;
                uint32_t DATA7:1;
                uint32_t DATA6:1;
                uint32_t DATA5:1;
                uint32_t DATA4:1;
                uint32_t DATA3:1;
                uint32_t DATA2:1;
                uint32_t DATA1:1;
                uint32_t DATA0:1;
            } B;
        } DSPIDL;               /* Masked Serial GPO for DSPI_D Low Register */

        int32_t SIU_reserved13[9];

        union {
            uint32_t R;
            struct {
                uint32_t EMIOS31:1;
                uint32_t EMIOS30:1;
                uint32_t EMIOS29:1;
                uint32_t EMIOS28:1;
                uint32_t EMIOS27:1;
                uint32_t EMIOS26:1;
                uint32_t EMIOS25:1;
                uint32_t EMIOS24:1;
                uint32_t EMIOS23:1;
                uint32_t EMIOS22:1;
                uint32_t EMIOS21:1;
                uint32_t EMIOS20:1;
                uint32_t EMIOS19:1;
                uint32_t EMIOS18:1;
                uint32_t EMIOS17:1;
                uint32_t EMIOS16:1;
                uint32_t EMIOS15:1;
                uint32_t EMIOS14:1;
                uint32_t EMIOS13:1;
                uint32_t EMIOS12:1;
                uint32_t EMIOS11:1;
                uint32_t EMIOS10:1;
                uint32_t EMIOS9:1;
                uint32_t EMIOS8:1;
                uint32_t EMIOS7:1;
                uint32_t EMIOS6:1;
                uint32_t EMIOS5:1;
                uint32_t EMIOS4:1;
                uint32_t EMIOS3:1;
                uint32_t EMIOS2:1;
                uint32_t EMIOS1:1;
                uint32_t EMIOS0:1;
            } B;
        } EMIOSA;               /* EMIOS A Select Register */

        union {
            uint32_t R;
            struct {
                uint32_t DSPIAH31:1;
                uint32_t DSPIAH30:1;
                uint32_t DSPIAH29:1;
                uint32_t DSPIAH28:1;
                uint32_t DSPIAH27:1;
                uint32_t DSPIAH26:1;
                uint32_t DSPIAH25:1;
                uint32_t DSPIAH24:1;
                uint32_t DSPIAH23:1;
                uint32_t DSPIAH22:1;
                uint32_t DSPIAH21:1;
                uint32_t DSPIAH20:1;
                uint32_t DSPIAH19:1;
                uint32_t DSPIAH18:1;
                uint32_t DSPIAH17:1;
                uint32_t DSPIAH16:1;
                uint32_t DSPIAL15:1;
                uint32_t DSPIAL14:1;
                uint32_t DSPIAL13:1;
                uint32_t DSPIAL12:1;
                uint32_t DSPIAL11:1;
                uint32_t DSPIAL10:1;
                uint32_t DSPIAL9:1;
                uint32_t DSPIAL8:1;
                uint32_t DSPIAL7:1;
                uint32_t DSPIAL6:1;
                uint32_t DSPIAL5:1;
                uint32_t DSPIAL4:1;
                uint32_t DSPIAL3:1;
                uint32_t DSPIAL2:1;
                uint32_t DSPIAL1:1;
                uint32_t DSPIAL0:1;
            } B;
        } DSPIAHLA;             /* DSPIAH/L Select Register for DSPI A */

        int32_t SIU_reserved14[2];

        union {
            uint32_t R;
            struct {
                uint32_t EMIOS31:1;
                uint32_t EMIOS30:1;
                uint32_t EMIOS29:1;
                uint32_t EMIOS28:1;
                uint32_t EMIOS27:1;
                uint32_t EMIOS26:1;
                uint32_t EMIOS25:1;
                uint32_t EMIOS24:1;
                uint32_t EMIOS23:1;
                uint32_t EMIOS22:1;
                uint32_t EMIOS21:1;
                uint32_t EMIOS20:1;
                uint32_t EMIOS19:1;
                uint32_t EMIOS18:1;
                uint32_t EMIOS17:1;
                uint32_t EMIOS16:1;
                uint32_t EMIOS15:1;
                uint32_t EMIOS14:1;
                uint32_t EMIOS13:1;
                uint32_t EMIOS12:1;
                uint32_t EMIOS11:1;
                uint32_t EMIOS10:1;
                uint32_t EMIOS9:1;
                uint32_t EMIOS8:1;
                uint32_t EMIOS7:1;
                uint32_t EMIOS6:1;
                uint32_t EMIOS5:1;
                uint32_t EMIOS4:1;
                uint32_t EMIOS3:1;
                uint32_t EMIOS2:1;
                uint32_t EMIOS1:1;
                uint32_t EMIOS0:1;
            } B;
        } EMIOSB;               /* EMIOS B Select Register */

        union {
            uint32_t R;
            struct {
                uint32_t DSPIBH31:1;
                uint32_t DSPIBH30:1;
                uint32_t DSPIBH29:1;
                uint32_t DSPIBH28:1;
                uint32_t DSPIBH27:1;
                uint32_t DSPIBH26:1;
                uint32_t DSPIBH25:1;
                uint32_t DSPIBH24:1;
                uint32_t DSPIBH23:1;
                uint32_t DSPIBH22:1;
                uint32_t DSPIBH21:1;
                uint32_t DSPIBH20:1;
                uint32_t DSPIBH19:1;
                uint32_t DSPIBH18:1;
                uint32_t DSPIBH17:1;
                uint32_t DSPIBH16:1;
                uint32_t DSPIBL15:1;
                uint32_t DSPIBL14:1;
                uint32_t DSPIBL13:1;
                uint32_t DSPIBL12:1;
                uint32_t DSPIBL11:1;
                uint32_t DSPIBL10:1;
                uint32_t DSPIBL9:1;
                uint32_t DSPIBL8:1;
                uint32_t DSPIBL7:1;
                uint32_t DSPIBL6:1;
                uint32_t DSPIBL5:1;
                uint32_t DSPIBL4:1;
                uint32_t DSPIBL3:1;
                uint32_t DSPIBL2:1;
                uint32_t DSPIBL1:1;
                uint32_t DSPIBL0:1;
            } B;
        } DSPIBHLB;             /* DSPIBH/L Select Register for DSPI B */

        int32_t SIU_reserved115[2];

        union {
            uint32_t R;
            struct {
                uint32_t EMIOS31:1;
                uint32_t EMIOS30:1;
                uint32_t EMIOS29:1;
                uint32_t EMIOS28:1;
                uint32_t EMIOS27:1;
                uint32_t EMIOS26:1;
                uint32_t EMIOS25:1;
                uint32_t EMIOS24:1;
                uint32_t EMIOS23:1;
                uint32_t EMIOS22:1;
                uint32_t EMIOS21:1;
                uint32_t EMIOS20:1;
                uint32_t EMIOS19:1;
                uint32_t EMIOS18:1;
                uint32_t EMIOS17:1;
                uint32_t EMIOS16:1;
                uint32_t EMIOS15:1;
                uint32_t EMIOS14:1;
                uint32_t EMIOS13:1;
                uint32_t EMIOS12:1;
                uint32_t EMIOS11:1;
                uint32_t EMIOS10:1;
                uint32_t EMIOS9:1;
                uint32_t EMIOS8:1;
                uint32_t EMIOS7:1;
                uint32_t EMIOS6:1;
                uint32_t EMIOS5:1;
                uint32_t EMIOS4:1;
                uint32_t EMIOS3:1;
                uint32_t EMIOS2:1;
                uint32_t EMIOS1:1;
                uint32_t EMIOS0:1;
            } B;
        } EMIOSC;               /* EMIOS C Select Register */

        union {
            uint32_t R;
            struct {
                uint32_t DSPICH31:1;
                uint32_t DSPICH30:1;
                uint32_t DSPICH29:1;
                uint32_t DSPICH28:1;
                uint32_t DSPICH27:1;
                uint32_t DSPICH26:1;
                uint32_t DSPICH25:1;
                uint32_t DSPICH24:1;
                uint32_t DSPICH23:1;
                uint32_t DSPICH22:1;
                uint32_t DSPICH21:1;
                uint32_t DSPICH20:1;
                uint32_t DSPICH19:1;
                uint32_t DSPICH18:1;
                uint32_t DSPICH17:1;
                uint32_t DSPICH16:1;
                uint32_t DSPICL15:1;
                uint32_t DSPICL14:1;
                uint32_t DSPICL13:1;
                uint32_t DSPICL12:1;
                uint32_t DSPICL11:1;
                uint32_t DSPICL10:1;
                uint32_t DSPICL9:1;
                uint32_t DSPICL8:1;
                uint32_t DSPICL7:1;
                uint32_t DSPICL6:1;
                uint32_t DSPICL5:1;
                uint32_t DSPICL4:1;
                uint32_t DSPICL3:1;
                uint32_t DSPICL2:1;
                uint32_t DSPICL1:1;
                uint32_t DSPICL0:1;
            } B;
        } DSPICHLC;             /* DSPIAH/L Select Register for DSPI C */

        int32_t SIU_reserved16[2];

        union {
            uint32_t R;
            struct {
                uint32_t EMIOS31:1;
                uint32_t EMIOS30:1;
                uint32_t EMIOS29:1;
                uint32_t EMIOS28:1;
                uint32_t EMIOS27:1;
                uint32_t EMIOS26:1;
                uint32_t EMIOS25:1;
                uint32_t EMIOS24:1;
                uint32_t EMIOS23:1;
                uint32_t EMIOS22:1;
                uint32_t EMIOS21:1;
                uint32_t EMIOS20:1;
                uint32_t EMIOS19:1;
                uint32_t EMIOS18:1;
                uint32_t EMIOS17:1;
                uint32_t EMIOS16:1;
                uint32_t EMIOS15:1;
                uint32_t EMIOS14:1;
                uint32_t EMIOS13:1;
                uint32_t EMIOS12:1;
                uint32_t EMIOS11:1;
                uint32_t EMIOS10:1;
                uint32_t EMIOS9:1;
                uint32_t EMIOS8:1;
                uint32_t EMIOS7:1;
                uint32_t EMIOS6:1;
                uint32_t EMIOS5:1;
                uint32_t EMIOS4:1;
                uint32_t EMIOS3:1;
                uint32_t EMIOS2:1;
                uint32_t EMIOS1:1;
                uint32_t EMIOS0:1;
            } B;
        } EMIOSD;               /* EMIOS D Select Register */

        union {
            uint32_t R;
            struct {
                uint32_t DSPIDH31:1;
                uint32_t DSPIDH30:1;
                uint32_t DSPIDH29:1;
                uint32_t DSPIDH28:1;
                uint32_t DSPIDH27:1;
                uint32_t DSPIDH26:1;
                uint32_t DSPIDH25:1;
                uint32_t DSPIDH24:1;
                uint32_t DSPIDH23:1;
                uint32_t DSPIDH22:1;
                uint32_t DSPIDH21:1;
                uint32_t DSPIDH20:1;
                uint32_t DSPIDH19:1;
                uint32_t DSPIDH18:1;
                uint32_t DSPIDH17:1;
                uint32_t DSPIDH16:1;
                uint32_t DSPIDL15:1;
                uint32_t DSPIDL14:1;
                uint32_t DSPIDL13:1;
                uint32_t DSPIDL12:1;
                uint32_t DSPIDL11:1;
                uint32_t DSPIDL10:1;
                uint32_t DSPIDL9:1;
                uint32_t DSPIDL8:1;
                uint32_t DSPIDL7:1;
                uint32_t DSPIDL6:1;
                uint32_t DSPIDL5:1;
                uint32_t DSPIDL4:1;
                uint32_t DSPIDL3:1;
                uint32_t DSPIDL2:1;
                uint32_t DSPIDL1:1;
                uint32_t DSPIDL0:1;
            } B;
        } DSPIDHLD;             /* DSPIAH/L Select Register for DSPI D */

    };                          /* end of SIU_tag */
/**************************************************************************/
/*                          MODULE : STM                                  */
/**************************************************************************/
    struct STM_tag {

        union {
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t CPS:8;
                  uint32_t:6;
                uint32_t FRZ:1;
                uint32_t TEN:1;
            } B;
        } CR;                   /* STM Control Register */

        union {
            uint32_t R;
        } CNT;                  /* STM Count Register */

        int32_t STM_reserved[2];

        union {
            uint32_t R;
            struct {
                uint32_t:31;
                uint32_t CEN:1;
            } B;
        } CCR0;                 /* STM Channel Control Register 0 */

        union {
            uint32_t R;
            struct {
                uint32_t:31;
                uint32_t CIF:1;
            } B;
        } CIR0;                 /* STM Channel Interrupt Register 0 */

        union {
            uint32_t R;
        } CMP0;                 /* STM Channel Compare Register 0 */

        int32_t STM_reserved1;

        union {
            uint32_t R;
            struct {
                uint32_t:31;
                uint32_t CEN:1;
            } B;
        } CCR1;                 /* STM Channel Control Register 1 */

        union {
            uint32_t R;
            struct {
                uint32_t:31;
                uint32_t CIF:1;
            } B;
        } CIR1;                 /* STM Channel Interrupt Register 1 */

        union {
            uint32_t R;
        } CMP1;                 /* STM Channel Compare Register 1 */

        int32_t STM_reserved2;

        union {
            uint32_t R;
            struct {
                uint32_t:31;
                uint32_t CEN:1;
            } B;
        } CCR2;                 /* STM Channel Control Register 2 */

        union {
            uint32_t R;
            struct {
                uint32_t:31;
                uint32_t CIF:1;
            } B;
        } CIR2;                 /* STM Channel Interrupt Register 2 */

        union {
            uint32_t R;
        } CMP2;                 /* STM Channel Compare Register 2 */

        int32_t STM_reserved3;

        union {
            uint32_t R;
            struct {
                uint32_t:31;
                uint32_t CEN:1;
            } B;
        } CCR3;                 /* STM Channel Control Register 3 */

        union {
            uint32_t R;
            struct {
                uint32_t:31;
                uint32_t CIF:1;
            } B;
        } CIR3;                 /* STM Channel Interrupt Register 3 */

        union {
            uint32_t R;
        } CMP3;                 /* STM Channel Compare Register 3 */

    };                          /* end of STM_tag */
/**************************************************************************/
/*                          MODULE : SWT                                  */
/**************************************************************************/
    struct SWT_tag {
        union {
            uint32_t R;
            struct {
                uint32_t MAP0:1;
                uint32_t MAP1:1;
                uint32_t MAP2:1;
                uint32_t MAP3:1;
                uint32_t MAP4:1;
                uint32_t MAP5:1;
                uint32_t MAP6:1;
                uint32_t MAP7:1;
                  uint32_t:14;
                uint32_t KEY:1;
                uint32_t RIA:1;
                uint32_t WND:1;
                uint32_t ITR:1;
                uint32_t HLK:1;
                uint32_t SLK:1;
                  uint32_t:2;
                uint32_t FRZ:1;
                uint32_t WEN:1;
            } B;
        } CR;                   /* SWT Control Register */

        union {
            uint32_t R;
            struct {
                uint32_t:31;
                uint32_t TIF:1;
            } B;
        } IR;                   /* SWT Interrupt Register */

        union {
            uint32_t R;
            struct {
                uint32_t WTO:32;
            } B;
        } TO;                   /* SWT Time-Out Register */

        union {
            uint32_t R;
            struct {
                uint32_t WST:32;
            } B;
        } WN;                   /* SWT Window Register */

        union {
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t WSC:16;
            } B;
        } SR;                   /* SWT Service Register */

        union {
            uint32_t R;
            struct {
                uint32_t CNT:32;
            } B;
        } CO;                   /* SWT Counter Output Register */

        union {
            uint32_t R;
            struct {
                uint32_t:16;
                uint32_t SK:16;
            } B;
        } SK;                   /* SWT Service Key Register */

    };                          /* end of SWT_tag */

/* Define memories */

#define SRAM0_START   0x40000000UL
#define SRAM0_SIZE       0x80000UL
#define SRAM0_END     0x4007FFFFUL

#define SRAM1_START   0x40080000UL
#define SRAM1_SIZE       0x14000UL
#define SRAM1_END     0x40093FFFUL

#define FLASH_START         0x0UL
#define FLASH_SIZE     0x200000UL
#define FLASH_END      0x1FFFFFUL

/* Define instances of modules AIPS_A */
#define MLB       (*(volatile struct MLB_tag *)       0xC3F84000UL)
#define I2C_C     (*(volatile struct I2C_tag *)       0xC3F88000UL)
#define I2C_D     (*(volatile struct I2C_tag *)       0xC3F8C000UL)
#define DSPI_C    (*(volatile struct DSPI_tag *)      0xC3F90000UL)
#define DSPI_D    (*(volatile struct DSPI_tag *)      0xC3F94000UL)
#define ESCI_J    (*(volatile struct ESCI_tag *)      0xC3FA0000UL)
#define ESCI_K    (*(volatile struct ESCI_tag *)      0xC3FA4000UL)
#define ESCI_L    (*(volatile struct ESCI_tag *)      0xC3FA8000UL)
#define ESCI_M    (*(volatile struct ESCI_tag *)      0xC3FAC000UL)
#define FR        (*(volatile struct FR_tag *)        0xC3FDC000UL)

/* Define instances of modules AIPS_B */
#define XBAR      (*(volatile struct XBAR_tag *)      0xFFF04000UL)
#define SEMA4     (*(volatile struct SEMA4_tag *)     0xFFF10000UL)
#define MPU       (*(volatile struct MPU_tag *)       0xFFF14000UL)
#define SWT       (*(volatile struct SWT_tag *)       0xFFF38000UL)
#define STM       (*(volatile struct STM_tag *)       0xFFF3C000UL)
#define ECSM      (*(volatile struct ECSM_tag *)      0xFFF40000UL)
#define EDMA      (*(volatile struct EDMA_tag *)      0xFFF44000UL)
#define INTC      (*(volatile struct INTC_tag *)      0xFFF48000UL)
#define FEC       (*(volatile struct FEC_tag *)       0xFFF4C000UL)
#define ADC       (*(volatile struct ADC_tag *)       0xFFF80000UL)
#define I2C_A     (*(volatile struct I2C_tag *)       0xFFF88000UL)
#define I2C_B     (*(volatile struct I2C_tag *)       0xFFF8C000UL)
#define DSPI_A    (*(volatile struct DSPI_tag *)      0xFFF90000UL)
#define DSPI_B    (*(volatile struct DSPI_tag *)      0xFFF94000UL)
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
#define CTU       (*(volatile struct CTU_tag *)       0xFFFD8000UL)
#define DMAMUX    (*(volatile struct DMAMUX_tag *)    0xFFFDC000UL)
#define PIT       (*(volatile struct PIT_tag *)       0xFFFE0000UL)
#define PIT_RTI   (*(volatile struct PIT_tag *)       0xFFFE0000UL)
#define EMIOS     (*(volatile struct EMIOS_tag *)     0xFFFE4000UL)
#define SIU       (*(volatile struct SIU_tag *)       0xFFFE8000UL)
#define CRP       (*(volatile struct CRP_tag *)       0xFFFEC000UL)
#define FMPLL     (*(volatile struct FMPLL_tag *)     0xFFFF0000UL)
#define FLASH     (*(volatile struct FLASH_tag *)     0xFFFF8000UL)

#ifdef __MWERKS__
#pragma pop
#endif

#ifdef  __cplusplus
}
#endif
#endif /* ASM */
#endif                          /* ifdef _MPC5668_H */

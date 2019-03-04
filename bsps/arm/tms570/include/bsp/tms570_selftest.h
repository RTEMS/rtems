/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief Definition of TMS570 selftest error codes, addresses and functions.
 */
/*
 * Copyright (c) 2016 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 *
 * Algorithms are based on Ti manuals and Ti HalCoGen generated
 * code available under following copyright.
 */
/*
 * Copyright (C) 2009-2015 Texas Instruments Incorporated - www.ti.com
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef LIBBSP_ARM_TMS570_SELFTEST_H
#define LIBBSP_ARM_TMS570_SELFTEST_H

#include <stdint.h>
#include <stdbool.h>

#define CCMSELFCHECK_FAIL1           1U
#define CCMSELFCHECK_FAIL2           2U
#define CCMSELFCHECK_FAIL3           3U
#define CCMSELFCHECK_FAIL4           4U
#define PBISTSELFCHECK_FAIL1         5U
#define EFCCHECK_FAIL1               6U
#define EFCCHECK_FAIL2               7U
#define FMCECCCHECK_FAIL1            8U
#define CHECKB0RAMECC_FAIL1          9U
#define CHECKB1RAMECC_FAIL1         10U
#define CHECKFLASHECC_FAIL1         11U
#define VIMPARITYCHECK_FAIL1        12U
#define DMAPARITYCHECK_FAIL1        13U
#define HET1PARITYCHECK_FAIL1       14U
#define HTU1PARITYCHECK_FAIL1       15U
#define HET2PARITYCHECK_FAIL1       16U
#define HTU2PARITYCHECK_FAIL1       17U
#define ADC1PARITYCHECK_FAIL1       18U
#define ADC2PARITYCHECK_FAIL1       19U
#define CAN1PARITYCHECK_FAIL1       20U
#define CAN2PARITYCHECK_FAIL1       21U
#define CAN3PARITYCHECK_FAIL1       22U
#define MIBSPI1PARITYCHECK_FAIL1    23U
#define MIBSPI3PARITYCHECK_FAIL1    24U
#define MIBSPI5PARITYCHECK_FAIL1    25U
#define CHECKRAMECC_FAIL1           26U
#define CHECKRAMECC_FAIL2           27U
#define CHECKCLOCKMONITOR_FAIL1     28U
#define CHECKFLASHEEPROMECC_FAIL1   29U
#define CHECKFLASHEEPROMECC_FAIL2   31U
#define CHECKFLASHEEPROMECC_FAIL3   32U
#define CHECKFLASHEEPROMECC_FAIL4   33U
#define CHECKPLL1SLIP_FAIL1         34U
#define CHECKRAMADDRPARITY_FAIL1    35U
#define CHECKRAMADDRPARITY_FAIL2    36U
#define CHECKRAMUERRTEST_FAIL1      37U
#define CHECKRAMUERRTEST_FAIL2      38U
#define FMCBUS1PARITYCHECK_FAIL1    39U
#define FMCBUS1PARITYCHECK_FAIL2    40U
#define PBISTSELFCHECK_FAIL2         41U
#define PBISTSELFCHECK_FAIL3         42U

/* PBIST and STC ROM - PBIST RAM GROUPING */
#define PBIST_ROM_PBIST_RAM_GROUP   1U
#define STC_ROM_PBIST_RAM_GROUP     2U

#define VIMRAMLOC       (*(volatile uint32_t *)0xFFF82000U)
#define VIMRAMPARLOC    (*(volatile uint32_t *)0xFFF82400U)

#define NHET1RAMPARLOC  (*(volatile uint32_t *)0xFF462000U)
#define NHET2RAMPARLOC  (*(volatile uint32_t *)0xFF442000U)
#define adcPARRAM1      (*(volatile uint32_t *)(0xFF3E0000U + 0x1000U))
#define adcPARRAM2      (*(volatile uint32_t *)(0xFF3A0000U + 0x1000U))
#define canPARRAM1      (*(volatile uint32_t *)(0xFF1E0000U + 0x10U))
#define canPARRAM2      (*(volatile uint32_t *)(0xFF1C0000U + 0x10U))
#define canPARRAM3      (*(volatile uint32_t *)(0xFF1A0000U + 0x10U))
#define HTU1PARLOC      (*(volatile uint32_t *)0xFF4E0200U)
#define HTU2PARLOC      (*(volatile uint32_t *)0xFF4C0200U)

#define NHET1RAMLOC     (*(volatile uint32_t *)0xFF460000U)
#define NHET2RAMLOC     (*(volatile uint32_t *)0xFF440000U)
#define HTU1RAMLOC      (*(volatile uint32_t *)0xFF4E0000U)
#define HTU2RAMLOC      (*(volatile uint32_t *)0xFF4C0000U)

#define adcRAM1 (*(volatile uint32_t *)0xFF3E0000U)
#define adcRAM2 (*(volatile uint32_t *)0xFF3A0000U)
#define canRAM1 (*(volatile uint32_t *)0xFF1E0000U)
#define canRAM2 (*(volatile uint32_t *)0xFF1C0000U)
#define canRAM3 (*(volatile uint32_t *)0xFF1A0000U)

#define DMARAMPARLOC    (*(volatile uint32_t *)(0xFFF80A00U))
#define DMARAMLOC       (*(volatile uint32_t *)(0xFFF80000U))

#define MIBSPI1RAMLOC   (*(volatile uint32_t *)(0xFF0E0000U))
#define MIBSPI3RAMLOC   (*(volatile uint32_t *)(0xFF0C0000U))
#define MIBSPI5RAMLOC   (*(volatile uint32_t *)(0xFF0A0000U))

#define mibspiPARRAM1 (*(volatile uint32_t *)(0xFF0E0000U + 0x00000400U))
#define mibspiPARRAM3 (*(volatile uint32_t *)(0xFF0C0000U + 0x00000400U))
#define mibspiPARRAM5 (*(volatile uint32_t *)(0xFF0A0000U + 0x00000400U))

/** @enum pbistPort
 *   @brief Alias names for pbist Port number
 *
 *   This enumeration is used to provide alias names for the pbist Port number
 *     - PBIST_PORT0
 *     - PBIST_PORT1
 *
 *   @Note Check the datasheet for the port avaiability
 */
enum pbistPort {
  PBIST_PORT0 = 0U,   /**< Alias for PBIST Port 0 */
  PBIST_PORT1 = 1U    /**< Alias for PBIST Port 1 < Check datasheet for Port 1 availability > */
};

enum {
  PBIST_TripleReadSlow     = 0x00000001U,  /**<TRIPLE_READ_SLOW_READ  for PBIST and STC ROM*/
  PBIST_TripleReadFast     = 0x00000002U,  /**<TRIPLE_READ_SLOW_READ  for PBIST and STC ROM*/
  PBIST_March13N_DP        = 0x00000004U,  /**< March13 N Algo for 2 Port mem */
};

uint32_t tms570_efc_check( void );

bool tms570_efc_check_self_test( void );

void bsp_selftest_fail_notification( uint32_t flag );

void tms570_memory_port0_fail_notification(
  uint32_t groupSelect,
  uint32_t dataSelect,
  uint32_t address,
  uint32_t data
);

void tms570_esm_channel_sr_clear(
  unsigned grp,
  unsigned chan
);

int tms570_esm_channel_sr_get(
  unsigned grp,
  unsigned chan
);

void tms570_pbist_self_check( void );

void tms570_pbist_run(
  uint32_t raminfoL,
  uint32_t algomask
);

bool tms570_pbist_is_test_completed( void );

bool tms570_pbist_is_test_passed( void );

void tms570_pbist_fail( void );

void tms570_pbist_stop( void );

void tms570_enable_parity( void );

void tms570_disable_parity( void );

bool tms570_efc_stuck_zero( void );

void tms570_efc_self_test( void );

bool tms570_pbist_port_test_status( uint32_t port );

void tms570_check_tcram_ecc( void );

#endif /*LIBBSP_ARM_TMS570_SELFTEST_H*/

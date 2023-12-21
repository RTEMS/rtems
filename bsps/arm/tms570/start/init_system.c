/* SPDX-License-Identifier: BSD-3-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This source file contains parts of the system initialization.
 */

/*
 * Copyright (C) 2022 Airbus U.S. Space & Defense, Inc
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

#include <stdint.h>
#include <stdbool.h>
#include <bsp/tms570.h>
#include <bsp/tms570-pinmux.h>
#include <bsp/tms570_selftest.h>
#include <bsp/tms570_hwinit.h>

/**
 * @brief Adjust Low-Frequency (LPO) oscilator (HCG:trimLPO)
 *
 */
/* SourceId : SYSTEM_SourceId_002 */
/* DesignId : SYSTEM_DesignId_002 */
/* Requirements : HL_SR468 */
void tms570_trim_lpo_init( void )
{
  /** @b Initialize Lpo: */
  /** Load TRIM values from OTP if present else load user defined values */
  /*SAFETYMCUSW 139 S MR:13.7 <APPROVED> "Hardware status bit read check" */
  TMS570_SYS1.LPOMONCTL = TMS570_SYS1_LPOMONCTL_BIAS_ENABLE |
                          TMS570_SYS1_LPOMONCTL_OSCFRQCONFIGCNT * 0 |
                          TMS570_SYS1_LPOMONCTL_HFTRIM( 16 ) |
                          16; /* LFTRIM  */
}

/* FIXME */
enum tms570_flash_power_modes {
  TMS570_FLASH_SYS_SLEEP = 0U,     /**< Alias for flash bank power mode sleep   */
  TMS570_FLASH_SYS_STANDBY = 1U,   /**< Alias for flash bank power mode standby */
  TMS570_FLASH_SYS_ACTIVE = 3U     /**< Alias for flash bank power mode active  */
};

/**
 * @brief Setup Flash memory parameters and timing (HCG:setupFlash)
 *
 */
/* SourceId : SYSTEM_SourceId_003 */
/* DesignId : SYSTEM_DesignId_003 */
/* Requirements : HL_SR457 */
void tms570_flash_init( void )
{
  /** - Setup flash read mode, address wait states and data wait states */
  TMS570_FLASH.FRDCNTL = TMS570_FLASH_FRDCNTL_RWAIT( 3 ) |
                         TMS570_FLASH_FRDCNTL_ASWSTEN |
                         TMS570_FLASH_FRDCNTL_ENPIPE;

  /** - Setup flash access wait states for bank 7 */
  TMS570_FLASH.FSMWRENA = TMS570_FLASH_FSMWRENA_WR_ENA( 0x5 );
  TMS570_FLASH.EEPROMCONFIG = TMS570_FLASH_EEPROMCONFIG_EWAIT( 3 ) |
                              TMS570_FLASH_EEPROMCONFIG_AUTOSUSP_EN * 0 |
                              TMS570_FLASH_EEPROMCONFIG_AUTOSTART_GRACE( 2 );

  /** - Disable write access to flash state machine registers */
  TMS570_FLASH.FSMWRENA = TMS570_FLASH_FSMWRENA_WR_ENA( 0xA );

  /** - Setup flash bank power modes */
  TMS570_FLASH.FBFALLBACK = TMS570_FLASH_FBFALLBACK_BANKPWR7(
    TMS570_FLASH_SYS_ACTIVE ) |
                            TMS570_FLASH_FBFALLBACK_BANKPWR1(
    TMS570_FLASH_SYS_ACTIVE ) |
                            TMS570_FLASH_FBFALLBACK_BANKPWR0(
    TMS570_FLASH_SYS_ACTIVE );
}

/**
 * @brief Power-up all peripherals and enable their clocks (HCG:periphInit)
 *
 */
/* SourceId : SYSTEM_SourceId_004 */
/* DesignId : SYSTEM_DesignId_004 */
/* Requirements : HL_SR470 */
void tms570_periph_init( void )
{
  /** - Disable Peripherals before peripheral powerup*/
  TMS570_SYS1.CLKCNTL &= ~TMS570_SYS1_CLKCNTL_PENA;

  /** - Release peripherals from reset and enable clocks to all peripherals */
  /** - Power-up all peripherals */
  TMS570_PCR1.PSPWRDWNCLR0 = 0xFFFFFFFFU;
  TMS570_PCR1.PSPWRDWNCLR1 = 0xFFFFFFFFU;
  TMS570_PCR1.PSPWRDWNCLR2 = 0xFFFFFFFFU;
  TMS570_PCR1.PSPWRDWNCLR3 = 0xFFFFFFFFU;

#if TMS570_VARIANT == 4357
  TMS570_PCR2.PSPWRDWNCLR0 = 0xFFFFFFFFU;
  TMS570_PCR2.PSPWRDWNCLR1 = 0xFFFFFFFFU;
  TMS570_PCR2.PSPWRDWNCLR2 = 0xFFFFFFFFU;
  TMS570_PCR2.PSPWRDWNCLR3 = 0xFFFFFFFFU;

  TMS570_PCR3.PSPWRDWNCLR0 = 0xFFFFFFFFU;
  TMS570_PCR3.PSPWRDWNCLR1 = 0xFFFFFFFFU;
  TMS570_PCR3.PSPWRDWNCLR2 = 0xFFFFFFFFU;
  TMS570_PCR3.PSPWRDWNCLR3 = 0xFFFFFFFFU;
#endif

  /** - Enable Peripherals */
  TMS570_SYS1.CLKCNTL |= TMS570_SYS1_CLKCNTL_PENA;
}

/**
 * @brief TMS570 system hardware initialization (HCG:systemInit)
 *
 */
/* SourceId : SYSTEM_SourceId_006 */
/* DesignId : SYSTEM_DesignId_006 */
/* Requirements : HL_SR471 */
void tms570_system_hw_init( void )
{
  uint32_t efc_check_status;

  /* Configure PLL control registers and enable PLLs.
   * The PLL takes (127 + 1024 * NR) oscillator cycles to acquire lock.
   * This initialization sequence performs all the tasks that are not
   * required to be done at full application speed while the PLL locks.
   */
  tms570_pll_init();

  /* Run eFuse controller start-up checks and start eFuse controller ECC self-test.
   * This includes a check for the eFuse controller error outputs to be stuck-at-zero.
   */
  efc_check_status = tms570_efc_check();

  /* Enable clocks to peripherals and release peripheral reset */
  tms570_periph_init();

  /* Configure device-level multiplexing and I/O multiplexing */
  tms570_pinmux_init();

  /* Enable external memory interface */
  TMS570_SYS1.GPREG1 |= TMS570_SYS1_GPREG1_EMIF_FUNC;

  if ( efc_check_status == 0U ) {
    /* Wait for eFuse controller self-test to complete and check results */
    if ( tms570_efc_check_self_test() == false ) { /* eFuse controller ECC logic self-test failed */
      bsp_selftest_fail_notification( EFCCHECK_FAIL1 );           /* device operation is not reliable */
    }
  } else if ( efc_check_status == 2U ) {
    /* Wait for eFuse controller self-test to complete and check results */
    if ( tms570_efc_check_self_test() == false ) { /* eFuse controller ECC logic self-test failed */
      bsp_selftest_fail_notification( EFCCHECK_FAIL1 );           /* device operation is not reliable */
    } else {
      bsp_selftest_fail_notification( EFCCHECK_FAIL2 );
    }
  } else {
    /* Empty */
  }

  /** - Set up flash address and data wait states based on the target CPU clock frequency
   * The number of address and data wait states for the target CPU clock frequency are specified
   * in the specific part's datasheet.
   */
  tms570_flash_init();

  /** - Configure the LPO such that HF LPO is as close to 10MHz as possible */
  tms570_trim_lpo_init();

  /** - Wait for PLLs to start up and map clock domains to desired clock sources */
  tms570_map_clock_init();

  /** - set ECLK pins functional mode */
  TMS570_SYS1.SYSPC1 = 0U;

  /** - set ECLK pins default output value */
  TMS570_SYS1.SYSPC4 = 0U;

  /** - set ECLK pins output direction */
  TMS570_SYS1.SYSPC2 = 1U;

  /** - set ECLK pins open drain enable */
  TMS570_SYS1.SYSPC7 = 0U;

  /** - set ECLK pins pullup/pulldown enable */
  TMS570_SYS1.SYSPC8 = 0U;

  /** - set ECLK pins pullup/pulldown select */
  TMS570_SYS1.SYSPC9 = 1U;

  /** - Setup ECLK */
  TMS570_SYS1.ECPCNTL = TMS570_SYS1_ECPCNTL_ECPSSEL * 0 |
                        TMS570_SYS1_ECPCNTL_ECPCOS * 0 |
                        TMS570_SYS1_ECPCNTL_ECPDIV( 8 - 1 );
}

#if 0
errata_PBIST_4
vimInit
#endif

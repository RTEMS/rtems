/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief TCRAM selftest function.
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

#include <stdint.h>
#include <bsp/tms570.h>
#include <bsp/tms570_selftest.h>
#include <bsp/tms570_hwinit.h>

#define tcramA1bitError (*(volatile uint32_t *)(0x08400000U))
#define tcramA2bitError (*(volatile uint32_t *)(0x08400010U))

#define tcramB1bitError (*(volatile uint32_t *)(0x08400008U))
#define tcramB2bitError (*(volatile uint32_t *)(0x08400018U))

#define tcramA1bit      (*(volatile uint64_t *)(0x08000000U))
#define tcramA2bit      (*(volatile uint64_t *)(0x08000010U))

#define tcramB1bit      (*(volatile uint64_t *)(0x08000008U))
#define tcramB2bit      (*(volatile uint64_t *)(0x08000018U))

/**
 * @brief Check TCRAM ECC error detection logic (HCG:checkRAMECC)
 *
 * This function checks TCRAM ECC error detection and correction logic.
 * The function does not return in case of TCRAM error.
 * It calls bsp_selftest_fail_notification() instead.
 *
 */
/* SourceId : SELFTEST_SourceId_034 */
/* DesignId : SELFTEST_DesignId_019 */
/* Requirements : HL_SR408 */
void  tms570_check_tcram_ecc( void )
{
  volatile uint64_t ramread;
  volatile uint32_t regread;
  uint32_t tcram1ErrStat, tcram2ErrStat = 0U;

  uint64_t tcramA1_bk = tcramA1bit;
  uint64_t tcramB1_bk = tcramB1bit;
  uint64_t tcramA2_bk = tcramA2bit;
  uint64_t tcramB2_bk = tcramB2bit;

  /* Clear RAMOCUUR before setting RAMTHRESHOLD register */
  TMS570_TCRAM1.RAMOCCUR = 0U;
  TMS570_TCRAM2.RAMOCCUR = 0U;

  /* Set Single-bit Error Threshold Count as 1 */
  TMS570_TCRAM1.RAMTHRESHOLD = 1U;
  TMS570_TCRAM2.RAMTHRESHOLD = 1U;

  /* Enable single bit error generation */
  TMS570_TCRAM1.RAMINTCTRL = 1U;
  TMS570_TCRAM2.RAMINTCTRL = 1U;

  /* Enable writes to ECC RAM, enable ECC error response */
  TMS570_TCRAM1.RAMCTRL = 0x0005010AU;
  TMS570_TCRAM2.RAMCTRL = 0x0005010AU;

  /* Force a single bit error in both the banks */
  _coreDisableRamEcc_();
  tcramA1bitError ^= 1U;
  tcramB1bitError ^= 1U;
  _coreEnableRamEcc_();

  /* Read the corrupted data to generate single bit error */
  ramread = tcramA1bit;
  ramread = tcramB1bit;
  (void)ramread;

  /* Check for error status */
  tcram1ErrStat = TMS570_TCRAM1.RAMERRSTATUS & 0x1U;
  tcram2ErrStat = TMS570_TCRAM2.RAMERRSTATUS & 0x1U;
  /*SAFETYMCUSW 139 S MR:13.7  <APPROVED> "LDRA Tool issue" */
  /*SAFETYMCUSW 139 S MR:13.7  <APPROVED> "LDRA Tool issue" */
  if ((tcram1ErrStat == 0U) || (tcram2ErrStat == 0U)) {
        /* TCRAM module does not reflect 1-bit error reported by CPU */
    bsp_selftest_fail_notification(CHECKRAMECC_FAIL1);
  } else {
    if (!tms570_esm_channel_sr_get(1, 26) || !tms570_esm_channel_sr_get(1, 28)) {
      /* TCRAM 1-bit error not flagged in ESM */
      bsp_selftest_fail_notification(CHECKRAMECC_FAIL2);
    } else {
      /* Clear single bit error flag in TCRAM module */
      TMS570_TCRAM1.RAMERRSTATUS = 0x1U;
      TMS570_TCRAM2.RAMERRSTATUS = 0x1U;

      /* Clear ESM status */
      tms570_esm_channel_sr_clear(1, 26);
      tms570_esm_channel_sr_clear(1, 28);
    }
  }

#if 0
  /*
   * This test sequence requires that data abort exception
   * handler checks for ECC test write enable in RAMCTR (bit 8)
   * and if the access abort is intended then it should clear
   * error status TCRAM status register and checks and clears
   * ESM group3 uncorrectable TCRAM error channels.
   *
   * More modifications in BSP and RTEMS ARM support are
   * required to make this code work.
   */

  /* Force a double bit error in both the banks */
  _coreDisableRamEcc_();
  tcramA2bitError ^= 3U;
  tcramB2bitError ^= 3U;
  _coreEnableRamEcc_();

  /* Read the corrupted data to generate double bit error */
  ramread = tcramA2bit;
  ramread = tcramB2bit;
  /* read from location with 2-bit ECC error this will cause a data abort to be generated */
  /* See HalCoGen support src/sys/asm/dabort.asm */
  /* _ARMV4_Exception_data_abort_default has to include solution for this special case for RTEMS */
#endif

  regread = TMS570_TCRAM1.RAMUERRADDR;
  regread = TMS570_TCRAM2.RAMUERRADDR;
  (void)regread;

  /* disable writes to ECC RAM */
  TMS570_TCRAM1.RAMCTRL = 0x0005000AU;
  TMS570_TCRAM2.RAMCTRL = 0x0005000AU;

  /* Compute correct ECC */
  tcramA1bit = tcramA1_bk;
  tcramB1bit = tcramB1_bk;
  tcramA2bit = tcramA2_bk;
  tcramB2bit = tcramB2_bk;
}

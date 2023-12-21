/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This source file contains the HETx, HTUx, ADC, DMA and VIM module
 *   parity based protection support.
 *
 * Algorithms are based on Ti manuals and Ti HalCoGen generated
 * code.
 */

/*
 * Copyright (C) 2016 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <stddef.h>
#include <bsp/tms570.h>
#include <bsp/tms570_selftest.h>
#include <bsp/tms570_selftest_parity.h>

/**
 * @brief run test to check that parity protection works for modules with common setup structure.
 *
 * @param[in] desc module registers addresses end ESM channels descriptor
 *
 * @return Void, in the case of error invokes bsp_selftest_fail_notification()
 *
 * The descriptor provides address of the module registers and address
 * of internal RAM memory and corresponding parity area test access window.
 * This test function is usable for HETx, HTUx, ADC, DMA and VIM TMS570
 * peripherals.
 */
void tms570_selftest_par_check_std( const tms570_selftest_par_desc_t *desc )
{
  volatile uint32_t test_read_data;
  uint32_t          par_cr_bak = *desc->par_cr_reg;
  int               perr;

  /* Set TEST mode and enable parity checking */
  *desc->par_cr_reg = desc->par_cr_test | TMS570_SELFTEST_PAR_CR_KEY;

  /* flip parity bit */
  *desc->par_loc ^= desc->par_xor;

  /* Disable TEST mode */
  *desc->par_cr_reg = TMS570_SELFTEST_PAR_CR_KEY;

  /* read to cause parity error */
  test_read_data = *desc->ram_loc;
  (void) test_read_data;

  /* check if ESM channel is flagged */
  perr = tms570_esm_channel_sr_get( desc->esm_prim_grp, desc->esm_prim_chan );

  if ( desc->esm_sec_grp )
    perr |= tms570_esm_channel_sr_get( desc->esm_sec_grp, desc->esm_sec_chan );

  if ( !perr ) {
    /* RAM parity error was not flagged to ESM. */
    bsp_selftest_fail_notification( desc->fail_code );
  } else {
    /* If periperal has it own parity status register, clear it */
    if ( desc->par_st_reg != NULL )
      *desc->par_st_reg = desc->par_st_clear;

    /* clear ESM flag */
    tms570_esm_channel_sr_clear( desc->esm_prim_grp, desc->esm_prim_chan );

    if ( desc->esm_sec_grp )
      tms570_esm_channel_sr_clear( desc->esm_sec_grp, desc->esm_sec_chan );

    /* Set TEST mode and enable parity checking */
    *desc->par_cr_reg = desc->par_cr_test | TMS570_SELFTEST_PAR_CR_KEY;

    /* Revert back to correct data by flipping parity location */
    *desc->par_loc ^= desc->par_xor;
  }

  /* Restore Parity comtrol register */
  *desc->par_cr_reg = par_cr_bak;
}

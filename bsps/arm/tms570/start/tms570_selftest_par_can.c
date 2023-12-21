/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This source file contains the CAN module parity based protection
 *   support.
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
 * @brief run test to check that parity protection works for CAN modules RAM
 *
 * @param[in] desc module registers addresses end ESM channels descriptor
 *
 * @return Void, in the case of error invokes bsp_selftest_fail_notification()
 *
 * The descriptor provides address of the module registers and address
 * of internal RAM memory and corresponding parity area test access window.
 */
void tms570_selftest_par_check_can( const tms570_selftest_par_desc_t *desc )
{
  volatile uint32_t       test_read_data;
  volatile tms570_dcan_t *can_regs = (volatile tms570_dcan_t *) desc->fnc_data;
  uint32_t                canctl_bak = can_regs->CTL;
  uint32_t                canctl_pmd;
  int                     perr;

  /* Set TEST mode and enable parity checking */
  /* Disable parity, init mode, TEST mode */
  canctl_pmd = TMS570_DCAN_CTL_PMD_SET( 0, 0x5 );
  can_regs->CTL = canctl_pmd | TMS570_DCAN_CTL_Test | TMS570_DCAN_CTL_Init;

  /* Enable RAM Direct Access mode */
  can_regs->TEST = TMS570_DCAN_TEST_RDA;

  /* flip parity bit */
  *desc->par_loc ^= desc->par_xor;

  /* Disable TEST mode */
  canctl_pmd = TMS570_DCAN_CTL_PMD_SET( 0, 0xA );
  can_regs->CTL = canctl_pmd | TMS570_DCAN_CTL_Test;

  /* read to cause parity error */
  test_read_data = *desc->ram_loc;
  (void) test_read_data;

  /* check if ESM channel is flagged */
  perr = tms570_esm_channel_sr_get( desc->esm_prim_grp, desc->esm_prim_chan );

  if ( !perr ) {
    /* RAM parity error was not flagged to ESM. */
    bsp_selftest_fail_notification( desc->fail_code );
  } else {
    /* clear ESM flag */
    tms570_esm_channel_sr_clear( desc->esm_prim_grp, desc->esm_prim_chan );

    /* Set TEST mode and enable parity checking */
    canctl_pmd = TMS570_DCAN_CTL_PMD_SET( 0, 0x5 );
    can_regs->CTL = canctl_pmd | TMS570_DCAN_CTL_Test | TMS570_DCAN_CTL_Init;

    /* Revert back to correct data by flipping parity location */
    *desc->par_loc ^= desc->par_xor;
  }

  /* Disable RAM Direct Access mode */
  can_regs->TEST = 0x00000000U;

  /* Restore CTL register */
  can_regs->CTL = canctl_bak;

  /* Read Error and Status register to clear Parity Error bit */
  test_read_data = can_regs->ES;
}

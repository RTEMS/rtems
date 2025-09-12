/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup BspSparcLeon3ValGr740
 */

/*
 * Copyright (C) 2023 embedded brains GmbH & Co. KG
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp.h>
#include <grlib/io.h>
#include <grlib/l2cache-regs.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup BspSparcLeon3ValGr740 spec:/bsp/sparc/leon3/val/gr740
 *
 * @ingroup TestsuitesBspsValidationBsp0
 *
 * @brief This test case collection provides validation test cases for the
 *   `sparc/gr740` BSP.
 *
 * This test case performs the following actions:
 *
 * - Validate the required L2C bootloader settings.
 *
 *   - Check that the bootloader did set the L2CACCC.128WF register field.
 *
 *   - Check that the bootloader did clear the L2CACCC.DBPF register field.
 *
 *   - Check that the bootloader did set the L2CACCC.SPLIT register field.
 *
 *   - Check that the bootloader did clear the L2CERR.COMP register field.
 *
 * @{
 */

/**
 * @brief Validate the required L2C bootloader settings.
 */
static void BspSparcLeon3ValGr740_Action_0( void )
{
  l2cache *regs;
  uint32_t accc;
  uint32_t err;

  regs = (l2cache *) LEON3_L2CACHE_BASE;
  accc = grlib_load_32( &regs->l2caccc );
  err = grlib_load_32( &regs->l2cerr );

  /*
   * Check that the bootloader did set the L2CACCC.128WF register field.
   */
  T_ne_u32( accc & L2CACHE_L2CACCC_128WF, 0 );

  /*
   * Check that the bootloader did clear the L2CACCC.DBPF register field.
   */
  T_eq_u32( accc & L2CACHE_L2CACCC_DBPF, 0 );

  /*
   * Check that the bootloader did set the L2CACCC.SPLIT register field.
   */
  T_ne_u32( accc & L2CACHE_L2CACCC_SPLIT, 0 );

  /*
   * Check that the bootloader did clear the L2CERR.COMP register field.
   */
  T_eq_u32( err & L2CACHE_L2CERR_COMP, 0 );
}

/**
 * @fn void T_case_body_BspSparcLeon3ValGr740( void )
 */
T_TEST_CASE( BspSparcLeon3ValGr740 )
{
  BspSparcLeon3ValGr740_Action_0();
}

/** @} */

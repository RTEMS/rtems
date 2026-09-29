/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup BspSparcLeon3ValIrqmap
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <bsp/irq.h>

#include <rtems/test.h>

/**
 * @defgroup BspSparcLeon3ValIrqmap spec:/bsp/sparc/leon3/val/irqmap
 *
 * @ingroup TestsuitesBspsValidationBsp0
 *
 * @brief Tests the handling of an interrupt map entry of zero in the interrupt
 *   map copy.
 *
 * No driver uses the bus line of TM27_IRQMAP_BUS_LINE, so the test can change
 * its interrupt map entry for the time of the check.
 *
 * This test case performs the following actions:
 *
 * - Set the interrupt map entry of a bus line which no driver uses in the
 *   interrupt map copy to zero.  Check whether the interrupt vector of the bus
 *   line is enabled.  Restore the interrupt map entry.
 *
 *   - Check that the number of the bus line is associated with no interrupt
 *     vector.
 *
 * @{
 */

#define _RTEMS_TMTEST27
#include <tm27.h>

/**
 * @brief Set the interrupt map entry of a bus line which no driver uses in the
 *   interrupt map copy to zero.  Check whether the interrupt vector of the bus
 *   line is enabled.  Restore the interrupt map entry.
 */
static void BspSparcLeon3ValIrqmap_Action_0( void )
{
  rtems_status_code sc;
  uint8_t           entry;
  bool              enabled;

  entry = LEON3_IrqCtrl_Mapping[ TM27_IRQMAP_BUS_LINE ];
  LEON3_IrqCtrl_Mapping[ TM27_IRQMAP_BUS_LINE ] = 0;
  enabled = true;
  sc = rtems_interrupt_vector_is_enabled( TM27_IRQMAP_BUS_LINE, &enabled );
  LEON3_IrqCtrl_Mapping[ TM27_IRQMAP_BUS_LINE ] = entry;

  /*
   * Check that the number of the bus line is associated with no interrupt
   * vector.
   */
  T_rsc( sc, RTEMS_INVALID_ID );
}

/**
 * @fn void T_case_body_BspSparcLeon3ValIrqmap( void )
 */
T_TEST_CASE( BspSparcLeon3ValIrqmap )
{
  BspSparcLeon3ValIrqmap_Action_0();
}

/** @} */

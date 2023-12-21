/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This source file contains the error signaling module initialization.
 */

/*
 * Copyright (C) 2022 Airbus U.S. Space & Defense, Inc
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
#include <bsp/tms570.h>
#include <bsp/tms570_hwinit.h>

/**
 * @brief Error signaling module initialization (HCG:esmInit)
 *
 */
void tms570_esm_init( void )
{
  /** - Disable error pin channels */
  TMS570_ESM.DEPAPR1 = 0xFFFFFFFFU;
  TMS570_ESM.IEPCR4 = 0xFFFFFFFFU;

  /** - Disable interrupts */
  TMS570_ESM.IECR1 = 0xFFFFFFFFU;
  TMS570_ESM.IECR4 = 0xFFFFFFFFU;

  /** - Clear error status flags */
  TMS570_ESM.SR[0U] = 0xFFFFFFFFU;
  TMS570_ESM.SR[1U] = 0xFFFFFFFFU;
  TMS570_ESM.SSR2   = 0xFFFFFFFFU;
  TMS570_ESM.SR[2U] = 0xFFFFFFFFU;
  TMS570_ESM.SR4    = 0xFFFFFFFFU;

  /** - Setup LPC preload */
  TMS570_ESM.LTCPR = 16384U - 1U;

  /** - Reset error pin */
  if (TMS570_ESM.EPSR == 0U) {
    /*
     * Per TMS570LC4x Errata DEVICE#60, the error pin cannot be cleared with a
     * normal EKR write upon system reset.  Put in diagnostic followed by
     * normal mode instead.  This sequence works also on other chip variants.
     */
    TMS570_ESM.EKR = 0x0000000AU;
    TMS570_ESM.EKR = 0x00000000U;
  }

  /** - Clear interrupt level */
  TMS570_ESM.ILCR1 = 0xFFFFFFFFU;
  TMS570_ESM.ILCR4 = 0xFFFFFFFFU;

  /** - Set interrupt level */
  TMS570_ESM.ILSR1 = 0x00000000;

  TMS570_ESM.ILSR4 = 0x00000000;

  /** - Enable error pin channels */
  TMS570_ESM.EEPAPR1 = 0x00000000;

  TMS570_ESM.IEPSR4 = 0x00000000;

  /** - Enable interrupts */
  TMS570_ESM.IESR1 = 0x00000000;

  TMS570_ESM.IESR4 = 0x00000000;
}

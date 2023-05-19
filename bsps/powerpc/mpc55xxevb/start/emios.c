/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMPC55XX
 *
 * @brief Enhanced Modular Input Output Subsystem (eMIOS).
 */

/*
 * Copyright (C) 2009, 2011 embedded brains GmbH & Co. KG
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

#include <mpc55xx/emios.h>

#ifdef MPC55XX_HAS_EMIOS

/**
 * @brief Initialize the eMIOS module.
 *
 * The module is enabled.  It is configured to use the internal clock.  The
 * global prescaler value is set to @a prescaler.  If the value is greater than
 * the maximum the maxium value will be used instead.  A prescaler value of
 * zero disables the clock.
 *
 * @note No protection against concurrent execution.
 */
void mpc55xx_emios_initialize( unsigned prescaler)
{
  union EMIOS_MCR_tag mcr = MPC55XX_ZERO_FLAGS;

  /* Enable module */
  mcr.B.MDIS = 0;

  /* Disable debug mode */
  mcr.B.FRZ = 1;

  /* Enable global time base */
  mcr.B.GTBE = 1;

  /* Disable global prescaler (= disable clock) */
  mcr.B.GPREN = 0;

  /* Set MCR */
  EMIOS.MCR.R = mcr.R;

  /* Set OUDR */
  EMIOS.OUDR.R = 0;

  /* Set global prescaler value */
  mpc55xx_emios_set_global_prescaler( prescaler);
}

/**
 * @brief Returns the global prescaler value of the eMIOS module.
 */
unsigned mpc55xx_emios_global_prescaler( void)
{
  union EMIOS_MCR_tag mcr = EMIOS.MCR;

  if (mcr.B.GPREN != 0) {
    return mcr.B.GPRE + 1;
  } else {
    return 0;
  }
}

/**
 * @brief Sets the global prescaler value of the eMIOS module.
 *
 * The global prescaler value is set to @a prescaler.  If the value is greater
 * than the maximum the maxium value will be used instead.  A prescaler value
 * of zero disables the clock.
 *
 * @note No protection against concurrent execution.
 */
void mpc55xx_emios_set_global_prescaler( unsigned prescaler)
{
  union EMIOS_MCR_tag mcr = EMIOS.MCR;

  /* Enable or disable the global prescaler */
  mcr.B.GPREN = prescaler > 0 ? 1 : 0;

  /* Set global prescaler value */
  if (prescaler > 256) {
    prescaler = 256;
  } else if (prescaler < 1) {
    prescaler = 1;
  }
  mcr.B.GPRE = prescaler - 1;

  /* Set MCR */
  EMIOS.MCR.R = mcr.R;
}

#endif /* MPC55XX_HAS_EMIOS */

/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief Enhanced Modular Input Output Subsystem (eMIOS).
 */

/*
 * Copyright (c) 2009-2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
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

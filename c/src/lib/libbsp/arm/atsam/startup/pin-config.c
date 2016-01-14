/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp/pin-config.h>

const Pin atsam_pin_config[] = {
  /* Console */
  {PIO_PA21A_RXD1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT},
  {PIO_PB4D_TXD1, PIOB, ID_PIOB, PIO_PERIPH_D, PIO_DEFAULT},

  /* SDRAM */

  /* D0_7   */ {0x000000FF, PIOC, ID_PIOC, PIO_PERIPH_A, PIO_DEFAULT},
  /* D8_13  */ {0x0000003F, PIOE, ID_PIOE, PIO_PERIPH_A, PIO_DEFAULT},
  /* D14_15 */ {0x00018000, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT},
  /* A0_9   */ {0x3FF00000, PIOC, ID_PIOC, PIO_PERIPH_A, PIO_DEFAULT},
  /* SDA10  */ {0x00002000, PIOD, ID_PIOD, PIO_PERIPH_C, PIO_DEFAULT},
  /* CAS    */ {0x00020000, PIOD, ID_PIOD, PIO_PERIPH_C, PIO_DEFAULT},
  /* RAS    */ {0x00010000, PIOD, ID_PIOD, PIO_PERIPH_C, PIO_DEFAULT},
  /* SDCKE  */ {0x00004000, PIOD, ID_PIOD, PIO_PERIPH_C, PIO_DEFAULT},
  /* SDCK   */ {0x00800000, PIOD, ID_PIOD, PIO_PERIPH_C, PIO_DEFAULT},
  /* SDSC   */ {0x00008000, PIOC, ID_PIOC, PIO_PERIPH_A, PIO_DEFAULT},
  /* NBS0   */ {0x00040000, PIOC, ID_PIOC, PIO_PERIPH_A, PIO_DEFAULT},
  /* NBS1   */ {0x00008000, PIOD, ID_PIOD, PIO_PERIPH_C, PIO_DEFAULT},
  /* SDWE   */ {0x20000000, PIOD, ID_PIOD, PIO_PERIPH_C, PIO_DEFAULT},
  /* BA0    */ {0x00100000, PIOA, ID_PIOA, PIO_PERIPH_C, PIO_DEFAULT}
};

const size_t atsam_pin_config_count = PIO_LISTSIZE(atsam_pin_config);

const uint32_t atsam_matrix_ccfg_sysio = 0x20400010;

/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
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

/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Console LINFlexD API.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_POWERPC_MPC55XXEVB_CONSOLE_LINFLEX_H
#define LIBBSP_POWERPC_MPC55XXEVB_CONSOLE_LINFLEX_H

#include "console-generic.h"

#undef CR0
#undef CR1
#undef CR2
#undef CR3

#include <mpc55xx/regs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef MPC55XX_HAS_LINFLEX

extern const console_generic_callbacks mpc55xx_linflex_callbacks;

typedef struct {
  volatile LINFLEX_tag *regs;
  struct rtems_termios_tty *tty;
  rtems_vector_number irq_rxi;
  rtems_vector_number irq_txi;
  rtems_vector_number irq_err;
  volatile SIU_PCR_tag *tx_pcr_register;
  uint8_t tx_pa_value:2;
  volatile SIU_PCR_tag *rx_pcr_register;
  volatile SIUL_PSMI_8B_tag *rx_psmi_register;
  uint8_t rx_padsel_value:4;
  int transmit_nest_level;
  bool transmit_in_progress;
} mpc55xx_linflex_context;

extern mpc55xx_linflex_context mpc55xx_linflex_devices [];

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_POWERPC_MPC55XXEVB_CONSOLE_LINFLEX_H */

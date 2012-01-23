/**
 * @file
 *
 * @brief Console LINFlexD API.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
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

/**
 * @file
 *
 * @brief Console ESCI API.
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

#ifndef LIBBSP_POWERPC_MPC55XXEVB_CONSOLE_ESCI_H
#define LIBBSP_POWERPC_MPC55XXEVB_CONSOLE_ESCI_H

#include "console-generic.h"

#undef CR0
#undef CR1
#undef CR2
#undef CR3

#include <mpc55xx/regs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef MPC55XX_HAS_ESCI

extern const console_generic_callbacks mpc55xx_esci_callbacks;

typedef struct {
  volatile struct ESCI_tag *regs;
  struct rtems_termios_tty *tty;
  int transmit_nest_level;
  bool transmit_in_progress;
  rtems_vector_number irq;
} mpc55xx_esci_context;

extern mpc55xx_esci_context mpc55xx_esci_devices [];

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_POWERPC_MPC55XXEVB_CONSOLE_ESCI_H */

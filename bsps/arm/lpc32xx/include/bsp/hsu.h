/**
 * @file
 *
 * @ingroup lpc32xx_hsu
 *
 * @brief HSU support API.
 */

/*
 * Copyright (c) 2010-2014 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_ARM_LPC32XX_HSU_H
#define LIBBSP_ARM_LPC32XX_HSU_H

#include <rtems/termiostypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup lpc32xx_hsu HSU Support
 *
 * @ingroup RTEMSBSPsARMLPC32XX
 *
 * @brief HSU Support
 *
 * @{
 */

typedef struct {
  uint32_t fifo;
  uint32_t level;
  uint32_t iir;
  uint32_t ctrl;
  uint32_t rate;
} lpc32xx_hsu;

typedef struct {
  rtems_termios_device_context base;
  volatile lpc32xx_hsu *hsu;
  size_t chars_in_transmission;
  rtems_vector_number irq;
  uint32_t initial_baud;
} lpc32xx_hsu_context;

extern const rtems_termios_device_handler lpc32xx_hsu_fns;

bool lpc32xx_hsu_probe(rtems_termios_device_context *base);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC32XX_HSU_H */

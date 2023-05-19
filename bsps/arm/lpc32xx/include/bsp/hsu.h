/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup lpc32xx_hsu
 *
 * @brief HSU support API.
 */

/*
 * Copyright (C) 2010, 2014 embedded brains GmbH & Co. KG
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

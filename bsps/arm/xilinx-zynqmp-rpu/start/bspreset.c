/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMZynqMPRPU
 *
 * @brief This source file contains the implementation of bsp_reset().
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
 * Copyright (C) 2023 Reflex Aerospace GmbH
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

#include <bsp.h>
#include <bsp/bootcard.h>

void bsp_reset( rtems_fatal_source source, rtems_fatal_code code )
{
  /* CRL_APB_RESET_CTRL */
  volatile uint32_t *reset_ctrl = (volatile uint32_t *) 0xff5e0218;

  (void) source;
  (void) code;

  zynqmp_debug_console_flush();

  /*
   * This is a workaround for:
   *
   * https://gcc.gnu.org/bugzilla/show_bug.cgi?id=108658
   */
  __asm__ volatile ("");

  while (true) {
    /*
     * Request a soft system reset.  This is a system-level reset which is the
     * equivalent to asserting the external PS_SRST_B reset signal pin.
     */
    *reset_ctrl |= UINT32_C(0x10);
  }
}

/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2011, 2012 embedded brains GmbH & Co. KG
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
#include <bsp/console-generic.h>
#include <bsp/console-esci.h>
#include <bsp/console-linflex.h>

CONSOLE_GENERIC_INFO_TABLE = {
  #ifdef MPC55XX_HAS_ESCI
    CONSOLE_GENERIC_INFO(mpc55xx_esci_devices + 0, &mpc55xx_esci_callbacks, "/dev/ttyS0")
    #ifdef ESCI_B
      , CONSOLE_GENERIC_INFO(mpc55xx_esci_devices + 1, &mpc55xx_esci_callbacks, "/dev/ttyS1")
    #endif
    #ifdef ESCI_C
      , CONSOLE_GENERIC_INFO(mpc55xx_esci_devices + 2, &mpc55xx_esci_callbacks, "/dev/ttyS2")
    #endif
    #ifdef ESCI_D
      , CONSOLE_GENERIC_INFO(mpc55xx_esci_devices + 3, &mpc55xx_esci_callbacks, "/dev/ttyS3")
    #endif
  #endif
  #ifdef MPC55XX_HAS_LINFLEX
    CONSOLE_GENERIC_INFO(mpc55xx_linflex_devices + 0, &mpc55xx_linflex_callbacks, "/dev/ttyS0"),
    CONSOLE_GENERIC_INFO(mpc55xx_linflex_devices + 1, &mpc55xx_linflex_callbacks, "/dev/ttyS1")
  #endif
};

CONSOLE_GENERIC_INFO_COUNT;

CONSOLE_GENERIC_MINOR(MPC55XX_CONSOLE_MINOR);

/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMPC55XX
 *
 * @brief Register definitions for the MPC55xx and MPC56xx microcontroller
 * family.
 */

/*
 * Copyright (C) 2008, 2013 embedded brains GmbH & Co. KG
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

#ifndef LIBCPU_POWERPC_MPC55XX_REGS_H
#define LIBCPU_POWERPC_MPC55XX_REGS_H

#include <bspopts.h>

#if MPC55XX_CHIP_FAMILY == 551
  #include <mpc55xx/fsl-mpc551x.h>
  #define MPC55XX_HAS_EBI
  #define MPC55XX_HAS_ESCI
  #define MPC55XX_HAS_EMIOS
  #define MPC55XX_HAS_FMPLL_ENHANCED
  #define MPC55XX_HAS_SIU
#elif MPC55XX_CHIP_FAMILY == 555
  #include <mpc55xx/fsl-mpc555x.h>
  #define MPC55XX_HAS_EBI
  #define MPC55XX_HAS_ESCI
  #define MPC55XX_HAS_EMIOS
  #define MPC55XX_HAS_FMPLL
  #define MPC55XX_HAS_UNIFIED_CACHE
  #define MPC55XX_HAS_SIU
#elif MPC55XX_CHIP_FAMILY == 556
  #include <mpc55xx/fsl-mpc556x.h>
  #define MPC55XX_HAS_EBI
  #define MPC55XX_HAS_ESCI
  #define MPC55XX_HAS_EMIOS
  #define MPC55XX_HAS_FMPLL
  #define MPC55XX_HAS_UNIFIED_CACHE
  #define MPC55XX_HAS_SIU
#elif MPC55XX_CHIP_FAMILY == 564
  #include <mpc55xx/fsl-mpc564xL.h>
  #define MPC55XX_HAS_STM
  #define MPC55XX_HAS_SWT
  #define MPC55XX_HAS_MODE_CONTROL
  #define MPC55XX_HAS_INSTRUCTION_CACHE
  #define MPC55XX_HAS_LINFLEX
  #define MPC55XX_HAS_SECOND_INTERNAL_RAM_AREA
  #define MPC55XX_HAS_SIU_LITE
#elif MPC55XX_CHIP_FAMILY == 566
  #include <mpc55xx/fsl-mpc5668.h>
  #define MPC55XX_HAS_ESCI
  #define MPC55XX_HAS_EMIOS
  #define MPC55XX_HAS_FMPLL_ENHANCED
  #define MPC55XX_HAS_UNIFIED_CACHE
  #define MPC55XX_HAS_SIU
  /*
   * TODO: This e200z650n3e core has a wait instruction, but it did not wake-up
   * from PIT interrupts.
   */
#elif MPC55XX_CHIP_FAMILY == 567
  #include <mpc55xx/fsl-mpc567x.h>
  #define MPC55XX_HAS_EBI
  #define MPC55XX_HAS_ESCI
  #define MPC55XX_HAS_EMIOS
  #define MPC55XX_HAS_FMPLL_ENHANCED
  #define MPC55XX_HAS_INSTRUCTION_CACHE
  #define MPC55XX_HAS_DATA_CACHE
  #define MPC55XX_HAS_SIU
  #define MPC55XX_HAS_WAIT_INSTRUCTION
#else
  #error "unsupported chip type"
#endif

#define MPC55XX_ZERO_FLAGS { .R = 0 }

#endif /* LIBCPU_POWERPC_MPC55XX_REGS_H */

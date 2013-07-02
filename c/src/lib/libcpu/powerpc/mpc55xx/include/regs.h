/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief Register definitions for the MPC55xx and MPC56xx microcontroller
 * family.
 */

/*
 * Copyright (c) 2008-2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
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

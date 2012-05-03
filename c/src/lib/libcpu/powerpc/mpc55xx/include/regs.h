/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief Register definitions for the MPC55xx and MPC56xx microcontroller
 * family.
 */

/*
 * Copyright (c) 2008-2012 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBCPU_POWERPC_MPC55XX_REGS_H
#define LIBCPU_POWERPC_MPC55XX_REGS_H

#include <bspopts.h>

#if MPC55XX_CHIP_TYPE / 10 == 551
  #include <mpc55xx/fsl-mpc551x.h>
  #define MPC55XX_HAS_EBI
  #define MPC55XX_HAS_ESCI
  #define MPC55XX_HAS_EMIOS
  #define MPC55XX_HAS_FMPLL_ENHANCED
  #define MPC55XX_HAS_SIU
#elif MPC55XX_CHIP_TYPE / 10 == 555
  #include <mpc55xx/fsl-mpc555x.h>
  #define MPC55XX_HAS_EBI
  #define MPC55XX_HAS_ESCI
  #define MPC55XX_HAS_EMIOS
  #define MPC55XX_HAS_FMPLL
  #define MPC55XX_HAS_UNIFIED_CACHE
  #define MPC55XX_HAS_SIU
#elif MPC55XX_CHIP_TYPE / 10 == 556
  #include <mpc55xx/fsl-mpc556x.h>
  #define MPC55XX_HAS_EBI
  #define MPC55XX_HAS_ESCI
  #define MPC55XX_HAS_EMIOS
  #define MPC55XX_HAS_FMPLL
  #define MPC55XX_HAS_UNIFIED_CACHE
  #define MPC55XX_HAS_SIU
#elif MPC55XX_CHIP_TYPE / 10 == 564
  #include <mpc55xx/fsl-mpc564xL.h>
  #define MPC55XX_HAS_STM
  #define MPC55XX_HAS_SWT
  #define MPC55XX_HAS_MODE_CONTROL
  #define MPC55XX_HAS_INSTRUCTION_CACHE
  #define MPC55XX_HAS_LINFLEX
  #define MPC55XX_HAS_SECOND_INTERNAL_RAM_AREA
  #define MPC55XX_HAS_SIU_LITE
#elif MPC55XX_CHIP_TYPE / 10 == 567
  #include <mpc55xx/fsl-mpc567x.h>
  #define MPC55XX_HAS_EBI
  #define MPC55XX_HAS_ESCI
  #define MPC55XX_HAS_EMIOS
  #define MPC55XX_HAS_FMPLL_ENHANCED
  #define MPC55XX_HAS_INSTRUCTION_CACHE
  #define MPC55XX_HAS_DATA_CACHE
  #define MPC55XX_HAS_SIU
#else
  #error "unsupported chip type"
#endif

#define MPC55XX_ZERO_FLAGS { .R = 0 }

#endif /* LIBCPU_POWERPC_MPC55XX_REGS_H */

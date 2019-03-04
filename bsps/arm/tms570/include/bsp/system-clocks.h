/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief System clocks.
 */

/*
 * Copyright (c) 2014 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * Based on LPC24xx and LPC1768 BSP
 * by embedded brains GmbH and others
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_TMS570_SYSTEM_CLOCKS_H
#define LIBBSP_ARM_TMS570_SYSTEM_CLOCKS_H

#include <bsp/tms570-rti.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup tms570_clock System Clocks
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief System clocks.
 *
 * @{
 */

/**
 * @brief Returns current standard timer value in microseconds.
 *
 * This function uses RTI module free running counter 0 used
 * which is used as system tick timebase as well.
 */
static inline unsigned tms570_timer(void)
{
  uint32_t actual_fcr0 = TMS570_RTI.CNT[0].FRCx;
  return actual_fcr0;
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_TMS570_SYSTEM_CLOCKS_H */

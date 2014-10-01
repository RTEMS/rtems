/**
 * @file
 *
 * @ingroup beagle_timer
 *
 * @brief Timer API.
 */

/*
 * Copyright (c) 2012 Claas Ziemke. All rights reserved.
 *
 *  Claas Ziemke
 *  Kernerstrasse 11
 *  70182 Stuttgart
 *  Germany
 *  <claas.ziemke@gmx.net>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_SHARED_BEAGLE_TIMER_H
#define LIBBSP_ARM_SHARED_BEAGLE_TIMER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup beagle_timer Timer Support
 *
 * @ingroup beagle
 *
 * @brief Timer support.
 *
 * @{
 */

/**
 * @name Interrupt Register Defines
 *
 * @{
 */

#define BEAGLE_TIMER_IR_MR0 0x1U
#define BEAGLE_TIMER_IR_MR1 0x2U
#define BEAGLE_TIMER_IR_MR2 0x4U
#define BEAGLE_TIMER_IR_MR3 0x8U
#define BEAGLE_TIMER_IR_CR0 0x10U
#define BEAGLE_TIMER_IR_CR1 0x20U
#define BEAGLE_TIMER_IR_CR2 0x40U
#define BEAGLE_TIMER_IR_CR3 0x80U
#define BEAGLE_TIMER_IR_ALL 0xffU

/** @} */

/**
 * @name Timer Control Register Defines
 *
 * @{
 */

#define BEAGLE_TIMER_TCR_EN 0x1U
#define BEAGLE_TIMER_TCR_RST 0x2U

/** @} */

/**
 * @name Match Control Register Defines
 *
 * @{
 */

#define BEAGLE_TIMER_MCR_MR0_INTR 0x1U
#define BEAGLE_TIMER_MCR_MR0_RST 0x2U
#define BEAGLE_TIMER_MCR_MR0_STOP 0x4U
#define BEAGLE_TIMER_MCR_MR1_INTR 0x8U
#define BEAGLE_TIMER_MCR_MR1_RST 0x10U
#define BEAGLE_TIMER_MCR_MR1_STOP 0x20U
#define BEAGLE_TIMER_MCR_MR2_INTR 0x40U
#define BEAGLE_TIMER_MCR_MR2_RST 0x80U
#define BEAGLE_TIMER_MCR_MR2_STOP 0x100U
#define BEAGLE_TIMER_MCR_MR3_INTR 0x200U
#define BEAGLE_TIMER_MCR_MR3_RST 0x400U
#define BEAGLE_TIMER_MCR_MR3_STOP 0x800U

/** @} */

/**
 * @name Capture Control Register Defines
 *
 * @{
 */

#define BEAGLE_TIMER_CCR_CAP0_RE 0x1U
#define BEAGLE_TIMER_CCR_CAP0_FE 0x2U
#define BEAGLE_TIMER_CCR_CAP0_INTR 0x4U
#define BEAGLE_TIMER_CCR_CAP1_RE 0x8U
#define BEAGLE_TIMER_CCR_CAP1_FE 0x10U
#define BEAGLE_TIMER_CCR_CAP1_INTR 0x20U
#define BEAGLE_TIMER_CCR_CAP2_RE 0x40U
#define BEAGLE_TIMER_CCR_CAP2_FE 0x80U
#define BEAGLE_TIMER_CCR_CAP2_INTR 0x100U
#define BEAGLE_TIMER_CCR_CAP3_RE 0x200U
#define BEAGLE_TIMER_CCR_CAP3_FE 0x400U
#define BEAGLE_TIMER_CCR_CAP3_INTR 0x800U

/** @} */

/**
 * @name External Match Register Defines
 *
 * @{
 */

#define BEAGLE_TIMER_EMR_EM0_RE 0x1U
#define BEAGLE_TIMER_EMR_EM1_FE 0x2U
#define BEAGLE_TIMER_EMR_EM2_INTR 0x4U
#define BEAGLE_TIMER_EMR_EM3_RE 0x8U
#define BEAGLE_TIMER_EMR_EMC0_FE 0x10U
#define BEAGLE_TIMER_EMR_EMC1_INTR 0x20U
#define BEAGLE_TIMER_EMR_EMC2_RE 0x40U
#define BEAGLE_TIMER_EMR_EMC3_FE 0x80U

/** @} */

/**
 * @brief Timer control block.
 */
typedef struct {
  uint32_t ir;
  uint32_t tcr;
  uint32_t tc;
  uint32_t pr;
  uint32_t pc;
  uint32_t mcr;
  uint32_t mr0;
  uint32_t mr1;
  uint32_t mr2;
  uint32_t mr3;
  uint32_t ccr;
  uint32_t cr0;
  uint32_t cr1;
  uint32_t cr2;
  uint32_t cr3;
  uint32_t emr;
  uint32_t ctcr;
} beagle_timer;

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_BEAGLE_TIMER_H */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMShared
 *
 * @brief Create #defines which state which erratas shall get applied
 */

/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#ifndef ARM_ERRATA_H_
#define ARM_ERRATA_H_

#include <bsp/arm-release-id.h>
#include <libcpu/arm-cp15.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static inline arm_release_id arm_errata_get_processor_release(void)
{
  const uint32_t MIDR          = arm_cp15_get_id_code();
  const uint8_t  REVISION      = (MIDR & 0xF00000U) >> 20;
  const uint8_t  PATCH_LEVEL   = (MIDR & 0xFU);

  return ARM_RELEASE_ID_FROM_NUMBER_AND_PATCH_LEVEL(
    REVISION,
    PATCH_LEVEL
  );
}

static inline bool arm_errata_is_applicable_processor_errata_764369(void)
{
#if defined(RTEMS_SMP)
  const arm_release_id RELEASE       = arm_errata_get_processor_release();
  bool                 is_applicable = false;

  /* Errata information for Cortex-A9 processors.
   * Information taken from ARMs
   * "Cortex-A series processors
   * - Cortex-A9
   * - Software Developers Errata Notice
   * - Revision r4 revisions
   * - ARM Cortex-A9 processors r4 release Software Developers Errata Notice"
   * The corresponding link is: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0360f/BABJFIBA.html
   * Please see this document for more information on these erratas */

  switch( RELEASE ) {
    case ARM_RELEASE_ID_R4_P1:
    case ARM_RELEASE_ID_R4_P4:
    case ARM_RELEASE_ID_R3_P0:
    case ARM_RELEASE_ID_R2_P10:
    case ARM_RELEASE_ID_R2_P8:
    case ARM_RELEASE_ID_R2_P6:
    case ARM_RELEASE_ID_R2_P4:
    case ARM_RELEASE_ID_R2_P3:
    case ARM_RELEASE_ID_R2_P2:
    case ARM_RELEASE_ID_R2_P0:
      is_applicable = true;
      break;
    default:
      is_applicable = false;
      break;
  }

  return is_applicable;
#else
  return false;
#endif
}

static inline bool arm_errata_is_applicable_processor_errata_775420(void)
{
  const arm_release_id RELEASE       = arm_errata_get_processor_release();
  bool                 is_applicable = false;

  /* Errata information for Cortex-A9 processors.
  * Information taken from ARMs
  * "Cortex-A series processors
  * - Cortex-A9
  * - Software Developers Errata Notice
  * - Revision r4 revisions
  * - ARM Cortex-A9 processors r4 release Software Developers Errata Notice"
  * The corresponding link is: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0360f/BABJFIBA.html
  * Please see this document for more information on these erratas */

  switch( RELEASE ) {
    case ARM_RELEASE_ID_R2_P10:
    case ARM_RELEASE_ID_R2_P8:
    case ARM_RELEASE_ID_R2_P6:
    case ARM_RELEASE_ID_R2_P4:
    case ARM_RELEASE_ID_R2_P3:
    case ARM_RELEASE_ID_R2_P2:
      is_applicable = true;
      break;
    default:
      is_applicable = false;
      break;
  }

  return is_applicable;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ARM_ERRATA_H_ */

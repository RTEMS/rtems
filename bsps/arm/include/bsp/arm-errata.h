/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMShared
 *
 * @brief Create #defines which state which erratas shall get applied
 */

/*
 * Copyright (c) 2014 embedded brains GmbH & Co. KG
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

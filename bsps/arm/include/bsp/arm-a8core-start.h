/**
 *  @file
 *
 *  @ingroup RTEMSBSPsARMShared
 *
 *  @brief A8CORE_START Support
 */

/*
 * Copyright (c) 2014 Chris Johns <chrisj@rtems.org>.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_SHARED_ARM_A8CORE_START_H
#define LIBBSP_ARM_SHARED_ARM_A8CORE_START_H

#include <libcpu/arm-cp15.h>

#include <bsp.h>
#include <bsp/start.h>
#include <bsp/arm-errata.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

BSP_START_TEXT_SECTION static inline void arm_a8core_start_set_vector_base(void)
{
  /*
   * Do not use bsp_vector_table_begin == 0, since this will get optimized away.
  */
  if (bsp_vector_table_end != bsp_vector_table_size) {
    uint32_t ctrl;

    arm_cp15_set_vector_base_address(bsp_vector_table_begin);

    ctrl = arm_cp15_get_control();
    ctrl &= ~ARM_CP15_CTRL_V;
    arm_cp15_set_control(ctrl);
  }
}

BSP_START_TEXT_SECTION static inline void arm_a8core_start_hook_1(void)
{
  arm_a8core_start_set_vector_base();
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_SHARED_ARM_A8CORE_START_H */

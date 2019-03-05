/**
 *  @file
 *
 *  @ingroup RTEMSBSPsARMShared
 *
 *  @brief ARM PL111 FB Support
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_SHARED_ARM_PL111_FB_H
#define LIBBSP_ARM_SHARED_ARM_PL111_FB_H

#include <bsp/arm-pl111-regs.h>

typedef struct pl111_fb_config {
  volatile pl111 *regs;
  uint32_t timing0;
  uint32_t timing1;
  uint32_t timing2;
  uint32_t timing3;
  uint32_t control;
  uint32_t power_delay_in_us;
  void (*set_up)(const struct pl111_fb_config *cfg);
  void (*pins_set_up)(const struct pl111_fb_config *cfg);
  void (*pins_tear_down)(const struct pl111_fb_config *cfg);
  void (*tear_down)(const struct pl111_fb_config *cfg);
} pl111_fb_config;

const pl111_fb_config *arm_pl111_fb_get_config(void);

#endif /* LIBBSP_ARM_SHARED_ARM_PL111_FB_H */

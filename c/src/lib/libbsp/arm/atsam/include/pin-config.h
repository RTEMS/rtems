/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

#ifndef LIBBSP_ARM_ATSAM_PIN_CONFIG_H
#define LIBBSP_ARM_ATSAM_PIN_CONFIG_H

#include <libchip/chip.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern const Pin atsam_pin_config[];

extern const size_t atsam_pin_config_count;

extern const uint32_t atsam_matrix_ccfg_sysio;

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_ATSAM_PIN_CONFIG_H */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMCycVI2C
 */

/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#ifndef XXX_H
#define XXX_H

#include <rtems.h>
#include <bsp/alt_i2c.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  ALT_I2C_CTLR_t controller;
  char *device_name;
  uint32_t speed;
} i2cdrv_configuration;

extern const i2cdrv_configuration i2cdrv_config[];

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XXX_H */

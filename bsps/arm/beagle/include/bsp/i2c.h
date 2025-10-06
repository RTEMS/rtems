/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup arm_beagle
 *
 * @brief I2C support API.
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

#ifndef LIBBSP_ARM_BEAGLE_I2C_H
#define LIBBSP_ARM_BEAGLE_I2C_H

#include <rtems.h>
#include <bsp.h>
#include <dev/i2c/i2c.h>
#include <ofw/ofw.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BBB_I2C_SYSCLK 48000000
#define BBB_I2C_INTERNAL_CLK 12000000

#define BBB_I2C_0_BUS_PATH "/dev/i2c-0"
#define BBB_I2C_1_BUS_PATH "/dev/i2c-1"
#define BBB_I2C_2_BUS_PATH "/dev/i2c-2"

typedef enum {
  I2C0,
  I2C1,
  I2C2,
  I2C_COUNT
} bbb_i2c_id_t;

typedef struct i2c_regs {
  uint32_t BBB_I2C_REVNB_LO;
  uint32_t BBB_I2C_REVNB_HI;
  uint32_t dummy1[ 2 ];
  uint32_t BBB_I2C_SYSC;
  uint32_t dummy2[ 4 ];
  uint32_t BBB_I2C_IRQSTATUS_RAW;
  uint32_t BBB_I2C_IRQSTATUS;
  uint32_t BBB_I2C_IRQENABLE_SET;
  uint32_t BBB_I2C_IRQENABLE_CLR;
  uint32_t BBB_I2C_WE;
  uint32_t BBB_I2C_DMARXENABLE_SET;
  uint32_t BBB_I2C_DMATXENABLE_SET;
  uint32_t BBB_I2C_DMARXENABLE_CLR;
  uint32_t BBB_I2C_DMATXENABLE_CLR;
  uint32_t BBB_I2C_DMARXWAKE_EN;
  uint32_t BBB_I2C_DMATXWAKE_EN;
  uint32_t dummy3[ 16 ];
  uint32_t BBB_I2C_SYSS;
  uint32_t BBB_I2C_BUF;
  uint32_t BBB_I2C_CNT;
  uint32_t BBB_I2C_DATA;
  uint32_t dummy4;
  uint32_t BBB_I2C_CON;
  uint32_t BBB_I2C_OA;
  uint32_t BBB_I2C_SA;
  uint32_t BBB_I2C_PSC;
  uint32_t BBB_I2C_SCLL;
  uint32_t BBB_I2C_SCLH;
  uint32_t BBB_I2C_SYSTEST;
  uint32_t BBB_I2C_BUFSTAT;
  uint32_t BBB_I2C_OA1;
  uint32_t BBB_I2C_OA2;
  uint32_t BBB_I2C_OA3;
  uint32_t BBB_I2C_ACTOA;
  uint32_t BBB_I2C_SBLOCK;
} bbb_i2c_regs;

void beagle_i2c_init( phandle_t node );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_BEAGLE_I2C_H */

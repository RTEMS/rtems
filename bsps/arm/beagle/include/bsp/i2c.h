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
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_BEAGLE_I2C_H
#define LIBBSP_ARM_BEAGLE_I2C_H

#include <rtems.h>
#include <bsp.h>
#include <dev/i2c/i2c.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BBB_I2C_SYSCLK 48000000
#define BBB_I2C_INTERNAL_CLK 12000000

#define BBB_I2C_0_BUS_PATH "/dev/i2c-0"
#define BBB_I2C_1_BUS_PATH "/dev/i2c-1"
#define BBB_I2C_2_BUS_PATH "/dev/i2c-2"

#define BBB_I2C0_IRQ 70
#define BBB_I2C1_IRQ 71
#define BBB_I2C2_IRQ 30

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

int am335x_i2c_bus_register(
  const char         *bus_path,
  uintptr_t           register_base,
  uint32_t            input_clock, /* FIXME: Unused. Left for compatibility. */
  rtems_vector_number irq
);

static inline int bbb_register_i2c_1( void )
{
  return am335x_i2c_bus_register(
    BBB_I2C_1_BUS_PATH,
    AM335X_I2C1_BASE,
    I2C_BUS_CLOCK_DEFAULT,
    BBB_I2C1_IRQ
  );
}

static inline int bbb_register_i2c_2( void )
{
  return am335x_i2c_bus_register(
    BBB_I2C_2_BUS_PATH,
    AM335X_I2C2_BASE,
    I2C_BUS_CLOCK_DEFAULT,
    BBB_I2C2_IRQ
  );
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_BEAGLE_I2C_H */

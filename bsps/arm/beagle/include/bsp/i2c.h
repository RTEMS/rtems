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
#include <dev/i2c/i2c.h>
#include <bsp.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* I2C Configuration Register (I2C_CON): */

#define BBB_I2C_CON_EN  (1 << 15)  /* I2C module enable */
#define BBB_I2C_CON_BE  (1 << 14)  /* Big endian mode */
#define BBB_I2C_CON_STB (1 << 11)  /* Start byte mode (master mode only) */
#define BBB_I2C_CON_MST (1 << 10)  /* Master/slave mode */
#define BBB_I2C_CON_TRX (1 << 9)   /* Transmitter/receiver mode */
           /* (master mode only) */
#define BBB_I2C_CON_XA  (1 << 8)   /* Expand address */
#define BBB_I2C_CON_STP (1 << 1)   /* Stop condition (master mode only) */
#define BBB_I2C_CON_STT (1 << 0)   /* Start condition (master mode only) */
#define BBB_I2C_CON_CLR 0x0  /* Clear configuration register */
/* I2C Status Register (I2C_STAT): */

#define BBB_I2C_STAT_SBD  (1 << 15) /* Single byte data */
#define BBB_I2C_STAT_BB (1 << 12) /* Bus busy */
#define BBB_I2C_STAT_ROVR (1 << 11) /* Receive overrun */
#define BBB_I2C_STAT_XUDF (1 << 10) /* Transmit underflow */
#define BBB_I2C_STAT_AAS  (1 << 9)  /* Address as slave */
#define BBB_I2C_STAT_GC (1 << 5)
#define BBB_I2C_STAT_XRDY (1 << 4)  /* Transmit data ready */
#define BBB_I2C_STAT_RRDY (1 << 3)  /* Receive data ready */
#define BBB_I2C_STAT_ARDY (1 << 2)  /* Register access ready */
#define BBB_I2C_STAT_NACK (1 << 1)  /* No acknowledgment interrupt enable */
#define BBB_I2C_STAT_AL (1 << 0)  /* Arbitration lost interrupt enable */

/* I2C Interrupt Enable Register (I2C_IE): */
#define BBB_I2C_IE_GC_IE  (1 << 5)
#define BBB_I2C_IE_XRDY_IE  (1 << 4) /* Transmit data ready interrupt enable */
#define BBB_I2C_IE_RRDY_IE  (1 << 3) /* Receive data ready interrupt enable */
#define BBB_I2C_IE_ARDY_IE  (1 << 2) /* Register access ready interrupt enable */
#define BBB_I2C_IE_NACK_IE  (1 << 1) /* No acknowledgment interrupt enable */
#define BBB_I2C_IE_AL_IE  (1 << 0) /* Arbitration lost interrupt enable */

/* I2C SYSC Register (I2C_SYSC): */
#define BBB_I2C_SYSC_SRST (1 << 1)

#define BBB_I2C_TIMEOUT 1000

#define BBB_I2C_SYSS_RDONE            (1 << 0)  /* Internel reset monitoring */

#define BBB_CONFIG_SYS_I2C_SPEED    100000
#define BBB_CONFIG_SYS_I2C_SLAVE    1
#define BBB_I2C_ALL_FLAGS 0x7FFF
#define BBB_I2C_ALL_IRQ_FLAGS 0xFFFF

#define BBB_I2C_SYSCLK 48000000
#define BBB_I2C_INTERNAL_CLK 12000000
#define BBB_I2C_SPEED_CLK 100000

#define BBB_I2C_IRQ_ERROR \
  ( AM335X_I2C_IRQSTATUS_NACK \
    | AM335X_I2C_IRQSTATUS_ROVR \
    | AM335X_I2C_IRQSTATUS_AL \
    | AM335X_I2C_IRQSTATUS_ARDY \
    | AM335X_I2C_IRQSTATUS_RRDY \
    | AM335X_I2C_IRQSTATUS_XRDY \
    | AM335X_I2C_IRQSTATUS_XUDF )

#define BBB_I2C_IRQ_USED \
  ( AM335X_I2C_IRQSTATUS_ARDY \
    | AM335X_I2C_IRQSTATUS_XRDY )

#define BBB_I2C_0_BUS_PATH "/dev/i2c-0"
#define BBB_I2C_1_BUS_PATH "/dev/i2c-1"
#define BBB_I2C_2_BUS_PATH "/dev/i2c-2"

#define BBB_I2C0_IRQ 70
#define BBB_I2C1_IRQ 71
#define BBB_I2C2_IRQ 30

#define BBB_MODE2 2
#define BBB_MODE3 3

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

typedef struct bbb_i2c_bus {
  i2c_bus base;
  volatile bbb_i2c_regs *regs;
  i2c_msg *msgs;
  uint32_t msg_todo;
  uint32_t current_msg_todo;
  uint8_t *current_msg_byte;
  uint32_t current_todo;
  bool read;
  bool hold;
  rtems_id task_id;
  rtems_vector_number irq;
  uint32_t input_clock;
  uint32_t already_transferred;
} bbb_i2c_bus;

int am335x_i2c_bus_register(
  const char         *bus_path,
  uintptr_t           register_base,
  uint32_t            input_clock,
  rtems_vector_number irq
);

static inline int bbb_register_i2c_0( void )
{
  return am335x_i2c_bus_register(
    BBB_I2C_0_BUS_PATH,
    AM335X_I2C0_BASE,
    I2C_BUS_CLOCK_DEFAULT,
    BBB_I2C0_IRQ
  );
}

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

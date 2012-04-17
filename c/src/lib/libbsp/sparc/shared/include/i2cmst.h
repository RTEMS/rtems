/*
 * Driver for GRLIB port of OpenCores I2C-master
 *
 * COPYRIGHT (c) 2007 Gaisler Research
 * with parts from the RTEMS MPC83xx I2C driver (c) 2007 Embedded Brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * This file contains the driver declarations
 */
#ifndef _I2CMST_H
#define _I2CMST_H

#include <rtems/libi2c.h>
#include <ambapp.h>

#ifdef __cplusplus
extern "C" {
#endif

/* I2C-master operational registers */

typedef struct gr_i2cmst_regs {
  volatile unsigned int prescl; /* Prescale register */
  volatile unsigned int ctrl;   /* Control register */
  volatile unsigned int tdrd;   /* Transmit and Receive registers */
  volatile unsigned int cmdsts; /* Command and Status registers */
} gr_i2cmst_regs_t;

/* Control (CTRL) register */
#define GRI2C_CTRL_EN         0x00000080 /* Enable core */
#define GRI2C_CTRL_IEN        0x00000040 /* Interrupt enable */

/* Command (CMD) register */
#define GRI2C_CMD_STA         0x00000080 /* Generate START condition */
#define GRI2C_CMD_STO         0x00000040 /* Generate STOP condition */
#define GRI2C_CMD_RD          0x00000020 /* Read from slave */
#define GRI2C_CMD_WR          0x00000010 /* Write to slave */
#define GRI2C_CMD_ACK         0x00000008 /* Acknowledge */
#define GRI2C_CMD_IACK        0x00000001 /* Interrupt acknowledge */

/* Status (STS) register */
#define GRI2C_STS_RXACK       0x00000080 /* Receive acknowledge */
#define GRI2C_STS_BUSY        0x00000040 /* I2C-bus busy */
#define GRI2C_STS_AL          0x00000020 /* Arbitration lost */
#define GRI2C_STS_TIP         0x00000002 /* Transfer in progress */
#define GRI2C_STS_IF          0x00000001 /* Interrupt flag */

#define GRI2C_STATUS_IDLE     0x00000000

/* The OC I2C core will perform a write after a start unless the RD bit
   in the command register has been set. Since the rtems framework has
   a send_start function we buffer that command and use it when the first
   data is written. The START is buffered in the sendstart member below */
typedef struct gr_i2cmst_prv {
  gr_i2cmst_regs_t    *reg_ptr;
  unsigned int        sysfreq;     /* System clock frequency in kHz */
  unsigned char       sendstart;   /* START events are buffered here */
  /* rtems_irq_number irq_number; */
  /* rtems_id         irq_sema_id; */
} gr_i2cmst_prv_t;

typedef struct gr_i2cmst_desc {
  rtems_libi2c_bus_t  bus_desc;
  gr_i2cmst_prv_t     prv;
} gr_i2cmst_desc_t;

/* Scans for I2CMST core and initalizes i2c library */
rtems_status_code leon_register_i2c(struct ambapp_bus *abus);

#ifdef __cplusplus
}
#endif

#endif /*  _I2CMST_H */

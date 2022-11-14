/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup i2c
 */

/*
 * COPYRIGHT (c) 2007 Cobham Gaisler AB
 * with parts from the RTEMS MPC83xx I2C driver (c) 2007 Embedded Brains GmbH.
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

#ifndef _I2CMST_H
#define _I2CMST_H

/**
 * @defgroup i2c I2C-master
 *
 * @ingroup RTEMSBSPsSharedGRLIB
 *
 * @brief Driver for GRLIB port of OpenCores I2C-master
 *
 * @{
 */

#include <rtems/libi2c.h>
#include "ambapp.h"

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

/* Register I2CMST driver to Driver Manager */
void i2cmst_register_drv (void);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /*  _I2CMST_H */

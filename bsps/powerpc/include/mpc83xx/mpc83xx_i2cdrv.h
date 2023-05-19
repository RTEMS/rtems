/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * RTEMS support for MPC83xx
 *
 * This file contains the MPC83xx I2C driver declarations.
 */

/*
 * Copyright (c) 2007 embedded brains GmbH & Co. KG
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

#ifndef _MPC83XX_I2CDRV_H
#define _MPC83XX_I2CDRV_H

#include <rtems/libi2c.h>
#include <rtems/irq.h>

#include <bsp.h>

#ifdef LIBBSP_POWERPC_GEN83XX_BSP_H
  #include <mpc83xx/mpc83xx.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LIBBSP_POWERPC_MPC55XXEVB_BSP_H
  typedef struct {
    volatile uint8_t i2cadr;
    volatile uint8_t i2cfdr;
    volatile uint8_t i2ccr;
    volatile uint8_t i2csr;
    volatile uint8_t i2cdr;
    volatile uint8_t i2cdfsrr;
  } m83xxI2CRegisters_t;
#endif

typedef struct mpc83xx_i2c_softc {
  m83xxI2CRegisters_t *reg_ptr;     /* ptr to HW registers             */
  int                  initialized; /* TRUE: module is initialized     */
  rtems_irq_number     irq_number;  /* IRQ number used for this module */
  uint32_t             base_frq;    /* input frq for baud rate divider */
  rtems_id             irq_sema_id; /* SEMA used for IRQ signalling    */
  void               (*probe)(struct mpc83xx_i2c_softc *self);
} mpc83xx_i2c_softc_t ;

typedef struct {
  rtems_libi2c_bus_t	   bus_desc;
  struct mpc83xx_i2c_softc softc;
} mpc83xx_i2c_desc_t;


extern rtems_libi2c_bus_ops_t mpc83xx_i2c_ops;

#ifdef __cplusplus
}
#endif


#endif /* _MPC83XX_I2CDRV_H */

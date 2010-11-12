/*===============================================================*\
| Project: RTEMS support for MPC83xx                              |
+-----------------------------------------------------------------+
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the MPC83xx I2C driver declarations          |
\*===============================================================*/
#ifndef _MPC83XX_I2CDRV_H
#define _MPC83XX_I2CDRV_H

#include <rtems/libi2c.h>
#include <rtems/irq.h>

#include <bsp.h>

#ifdef __GEN83xx_BSP_h
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

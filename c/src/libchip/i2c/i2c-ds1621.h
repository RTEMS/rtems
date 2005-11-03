/* $Id$ */
#ifndef I2C_DS_1621_DRIVER_H
#define I2C_DS_1621_DRIVER_H

/* Trivial i2c driver for the maxim DS1621 temperature sensor;
 * just implements reading constant conversions with 8-bit
 * resolution.
 * Demonstrates the implementation of a i2c high-level driver.
 */

/* Author: Till Straumann, 2005 */

#define DS1621_CMD_READ_TEMP 	0xaa
#define	DS1621_CMD_CSR_ACCESS	0xac
#define DS1621_CMD_START_CONV	0xee

/* CSR bits */
#define DS1621_CSR_DONE			(1<<7)
#define DS1621_CSR_TEMP_HI		(1<<6)  /* T >= hi register */
#define DS1621_CSR_TEMP_LO		(1<<5)  /* T <= lo register */
#define DS1621_CSR_NVMEM_BSY	(1<<4)  /* non-volatile memory busy */
#define DS1621_CSR_OUT_POL		(1<<1)  /* Thermostat output active polarity */
#define DS1621_CSR_1SHOT		(1<<0)  /* Oneshot mode     */

#include <rtems.h>
#include <rtems/libi2c.h>

#ifdef __cplusplus
extern "C" {
#endif

/* for registration with libi2c */
extern rtems_libi2c_drv_t *i2c_ds1621_driver_descriptor;

#ifdef __cplusplus
}
#endif

#endif

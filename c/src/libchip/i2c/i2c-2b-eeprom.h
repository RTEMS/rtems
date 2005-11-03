/* $Id$ */
#ifndef I2C_2B_EEPROM_DRIVER_H
#define I2C_2B_EEPROM_DRIVER_H

/* Trivial i2c driver for reading and writing "2-byte eeproms".
 * On 'open' the file-pointer is reset to 0, subsequent
 * read/write operations slurp/write data from there...
 */

/* Author: Till Straumann, 2005 */


#include <rtems.h>
#include <rtems/libi2c.h>

#ifdef __cplusplus
extern "C" {
#endif

/* pass one of these to rtems_libi2c_register_drv() */

/* These ops provide no write access */
extern rtems_libi2c_drv_t *i2c_2b_eeprom_ro_driver_descriptor;

/* Use these for writing and reading */
extern rtems_libi2c_drv_t *i2c_2b_eeprom_driver_descriptor;

#ifdef __cplusplus
}
#endif

#endif

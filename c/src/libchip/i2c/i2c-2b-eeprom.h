#ifndef I2C_2B_EEPROM_DRIVER_H
#define I2C_2B_EEPROM_DRIVER_H

/* Trivial i2c driver for reading and writing "2-byte eeproms".
 * On 'open' the file-pointer is reset to 0, subsequent
 * read/write operations slurp/write data from there...
 */

/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2005,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * This software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */


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

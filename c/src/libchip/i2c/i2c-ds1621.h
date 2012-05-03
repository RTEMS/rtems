#ifndef I2C_DS_1621_DRIVER_H
#define I2C_DS_1621_DRIVER_H

/* Trivial i2c driver for the maxim DS1621 temperature sensor;
 * just implements reading constant conversions with 8-bit
 * resolution.
 * Demonstrates the implementation of a i2c high-level driver.
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

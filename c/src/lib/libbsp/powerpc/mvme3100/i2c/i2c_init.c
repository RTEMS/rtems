/* Register i2c bus driver & devices */

/*
 * Authorship
 * ----------
 * This software ('mvme3100' RTEMS BSP) was created by
 *
 *     Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * The 'mvme3100' BSP was produced by
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
#include <bsp.h>
#include <rtems/bspIo.h>
#include <rtems/libi2c.h>
#include <libchip/i2c-2b-eeprom.h>
#include <libchip/i2c-ds1621.h>
#include <bsp/mpc8540_i2c_busdrv.h>
#include <rtems/libio.h>

#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdarg.h>

static void
safe_printf (const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	if ( _System_state_Is_up( _System_state_Get() ) )
		vfprintf( stderr, fmt, ap );
	else
		vprintk( fmt, ap );
	va_end(ap);
}

static void
safe_perror(const char *s)
{
	safe_printf("%s :%s\n", s, strerror(errno));
}


int
BSP_i2c_initialize(void)
{
int busno, succ = 0;

	/* Initialize the library */
	if ( rtems_libi2c_initialize() ) {
		safe_printf("Initializing I2C library failed\n");
		return -1;
	}

	/* Register our bus driver */
	if ( (busno=rtems_libi2c_register_bus(
					BSP_I2C_BUS0_NAME,
					BSP_I2C_BUS_DESCRIPTOR) ) < 0 ) {
		safe_perror("Registering mpc8540 i2c bus driver");
		return -1;
	}

	/* Now register higher level drivers; note that
	 * the i2c address in the manual is actually left-shifted
	 * by one bit, i.e., as it would go on the bus.
	 */

	/* Use read-only driver for VPD */
	if ( rtems_libi2c_register_drv(
				BSP_I2C_VPD_EEPROM_NAME,
				i2c_2b_eeprom_ro_driver_descriptor,
				busno,
				BSP_VPD_I2C_ADDR) < 0 ) {
		safe_perror("Registering i2c VPD eeprom driver failed");
	} else {
		succ++;
	}

	/* Use read-write driver for user eeprom -- you still might
	 * have to disable HW write-protection on your board.
	 */
	if ( rtems_libi2c_register_drv(
				BSP_I2C_USR_EEPROM_NAME,
				i2c_2b_eeprom_driver_descriptor,
				busno,
				BSP_USR0_I2C_ADDR) < 0 ) {
		safe_perror("Registering i2c 1st USR eeprom driver failed");
	} else {
		succ++;
	}

	/* Use read-write driver for user eeprom -- you still might
	 * have to disable HW write-protection on your board.
	 */
	if ( rtems_libi2c_register_drv(
				BSP_I2C_USR1_EEPROM_NAME,
				i2c_2b_eeprom_driver_descriptor,
				busno,
				BSP_USR1_I2C_ADDR) < 0 ) {
		safe_perror("Registering i2c 2nd USR eeprom driver failed");
	} else {
		succ++;
	}

	/* The thermostat */
	if ( rtems_libi2c_register_drv(
				BSP_I2C_DS1621_NAME,
				i2c_ds1621_driver_descriptor,
				busno,
				BSP_THM_I2C_ADDR) < 0 ) {
		safe_perror("Registering i2c ds1621 temp sensor. driver failed");
	} else {
		succ++;
	}

	/* Finally, as an example, register raw access to the
	 * ds1621. The driver above just reads the 8 msb of the
	 * temperature but doesn't support anything else. Using
	 * the raw device node you can write/read individual
	 * control bytes yourself and e.g., program the thermostat...
	 */

	if ( mknod(
			BSP_I2C_DS1621_RAW_DEV_NAME,
			0666 | S_IFCHR,
			rtems_filesystem_make_dev_t(rtems_libi2c_major,
				  RTEMS_LIBI2C_MAKE_MINOR(busno,BSP_THM_I2C_ADDR))) ) {
		safe_perror("Creating device node for raw ds1621 (temp. sensor) access failed");
	} else {
		succ++;
	}

	/* Raw access to RTC */
	if ( mknod(
			BSP_I2C_DS1375_RAW_DEV_NAME,
			0666 | S_IFCHR,
			rtems_filesystem_make_dev_t(rtems_libi2c_major,
				  RTEMS_LIBI2C_MAKE_MINOR(busno,BSP_RTC_I2C_ADDR))) ) {
		safe_perror("Creating device node for raw ds1375 (rtc) access failed");
	} else {
		succ++;
	}

	safe_printf("%i I2C devices registered\n", succ);
	return 0;
}

/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMVME3100
 *
 * @brief Register the I2C bus driver and devices.
 */

/*
 * Copyright (C) 2005 - 2007 Till Straumann <strauman@slac.stanford.edu>
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

#include <rtems.h>
#include <bsp.h>
#include <rtems/bspIo.h>
#include <rtems/libi2c.h>
#include <libchip/i2c-2b-eeprom.h>
#include <libchip/i2c-ds1621.h>
#include <bsp/mpc8540_i2c_busdrv.h>
#include <rtems/libio.h>
#include <rtems/score/sysstate.h>

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

	/* Register our bus driver */
	if (!rtems_libi2c_is_initialized){
		safe_printf("LIBI2C NOT INITIALIZED\n");
		return -1;
	}

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

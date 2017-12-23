#ifndef DS1375_I2C_RTC_H
#define DS1375_I2C_RTC_H

/* Driver for the Maxim 1375 i2c RTC (TOD only; very simple...) */

/*
 * Authorship
 * ----------
 * This software was created by
 *
 *     Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * The software was produced by
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
#include <libchip/rtc.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern rtc_fns rtc_ds1375_fns;

bool
rtc_ds1375_device_probe( int minor );

uint32_t
rtc_ds1375_get_register( uintptr_t port, uint8_t reg );

void
rtc_ds1375_set_register( uintptr_t port, uint8_t reg, uint32_t value );

/*
 * BSP must supply string constant argument 'i2cname' which matches
 * the registered device name of the raw i2c device (created with mknod).
 * E.g., "/dev/i2c.ds1375-raw"
 *
 * NOTE: The i2c bus driver must already be up and 'i2cname' already
 *       be available when this ENTRY is registered or initialized.
 *
 *       If you want to allow applications to add the RTC driver to
 *       the configuration table then the i2c subsystem must be
 *       initialized by the BSP from the predriver_hook.
 */
#define DS1375_RTC_TBL_ENTRY(i2cname) \
{                                              	      \
	sDeviceName:	"/dev/rtc",                       \
	deviceType:	RTC_CUSTOM,                       \
	pDeviceFns:	&rtc_ds1375_fns,                  \
	deviceProbe:	rtc_ds1375_device_probe,          \
	ulCtrlPort1:	(uintptr_t)(i2cname),             \
	ulDataPort:	0,                                \
	getRegister:	rtc_ds1375_get_register,          \
	setRegister:	rtc_ds1375_set_register,          \
}

#ifdef __cplusplus
}
#endif

#endif

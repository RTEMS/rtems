/* Miscellaneous small BSP routines; reboot, board CSR, ... */

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

void
bsp_reset(void)
{
uint8_t v;
	/*
	 * AFAIK, the hardest reset available; cleared
	 * some errors a VME-bus reset wouldn't (hung
	 * i2c bus)...
	 */
	v  = in_8( BSP_MVME3100_SYS_CR );
	v &= ~BSP_MVME3100_SYS_CR_RESET_MSK;
	v |=  BSP_MVME3100_SYS_CR_RESET;
	out_8( BSP_MVME3100_SYS_CR, v );
}

uint8_t
BSP_setSysReg(volatile uint8_t *r, uint8_t mask)
{
uint8_t               v;
rtems_interrupt_level l;

	if ( !mask )
		return in_8( r );

	rtems_interrupt_disable(l);
		v = in_8( r );
		if ( mask ) {
			out_8( r,  v | mask );
		}
	rtems_interrupt_enable(l);
	return v;
}

uint8_t
BSP_clrSysReg(volatile uint8_t *r, uint8_t mask)
{
uint8_t               v;
rtems_interrupt_level l;

	if ( !mask )
		return in_8( r );

	rtems_interrupt_disable(l);
		v = in_8( r );
		if ( mask ) {
			out_8( r,  v & ~mask );
		}
	rtems_interrupt_enable(l);
	return v;
}

uint8_t
BSP_setLEDs(uint8_t mask)
{
	return BSP_setSysReg( BSP_MVME3100_SYS_IND_REG, mask );
}

uint8_t
BSP_clrLEDs(uint8_t mask)
{
	return BSP_clrSysReg( BSP_MVME3100_SYS_IND_REG, mask );
}

uint8_t
BSP_eeprom_write_protect(void)
{
uint8_t           m = BSP_MVME3100_SYS_CR_EEPROM_WP;
volatile uint8_t *r = BSP_MVME3100_SYS_CR;

	return m & BSP_setSysReg( r, m );
}

uint8_t
BSP_eeprom_write_enable(void)
{
uint8_t           m = BSP_MVME3100_SYS_CR_EEPROM_WP;
volatile uint8_t *r = BSP_MVME3100_SYS_CR;

	return m & BSP_clrSysReg( r, m );
}

/* BSP-specific bits of flash programmer support */

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
#include <libcpu/spr.h>
#include <stdio.h>

#include <bsp/flashPgmPvt.h>

SPR_RO(TBRL)

#define STATIC	static

static struct bankdesc mvme3100Flash[] = {
	/*
	 * Bank is populated from the top; make max_size negative to
	 * indicate this
	 */
	{ 0xf8000000, 0, - 0x08000000, 2, BSP_flash_vendor_spansion, 0, 0, 0  },
};

STATIC struct bankdesc *
bankcheck(int bank, int quiet)
{
	if ( bank ) {
		if ( !quiet )
			fprintf(stderr,"Invalid flash bank #%i\n",bank);
		return 0;
	}
	return &mvme3100Flash[bank];
}

STATIC int
flash_wp(int bank, int enbl)
{
uint8_t mask = enbl < 0 ? 0 : BSP_MVME3100_FLASH_CSR_F_WP_SW;
uint8_t val;

	if ( bank != 0 ) {
		fprintf(stderr,"Invalid flash bank #%i\n",bank);
		return -1;
	}

	if ( enbl )
		val = BSP_setSysReg( BSP_MVME3100_FLASH_CSR, mask );
	else
		val = BSP_clrSysReg( BSP_MVME3100_FLASH_CSR, mask );

	if ( BSP_MVME3100_FLASH_CSR_F_WP_HW & val ) {
		fprintf(stderr,"Flash: hardware write-protection engaged (switch)\n");
		return -1;
	}
	if ( enbl < 0 )
		return val & (BSP_MVME3100_FLASH_CSR_F_WP_HW | BSP_MVME3100_FLASH_CSR_F_WP_SW );
	return 0;
}

STATIC uint32_t
read_us_timer(void)
{
uint32_t mhz = BSP_bus_frequency/BSP_time_base_divisor/1000;

	return _read_TBRL()/mhz;
}

/* BSP ops (detect banks, handle write-protection on board) */
struct flash_bsp_ops BSP_flashBspOps = {
	bankcheck: 	   bankcheck,
	flash_wp:  	   flash_wp,
	read_us_timer: read_us_timer,
};

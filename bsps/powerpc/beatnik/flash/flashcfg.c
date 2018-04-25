/* 
 * Authorship
 * ----------
 * This software ('beatnik' RTEMS BSP for MVME6100 and MVME5500) was
 *     created by Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 * 
 * Acknowledgement of sponsorship
 * ------------------------------
 * The 'beatnik' BSP was produced by
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

#include <bsp.h>
#include <stdio.h>
#include <inttypes.h>

#define STATIC static

#include <bsp/flashPgmPvt.h>

/* MVME Board Specifica; board status reg. 2 where write-enable is controlled... */

#define SYS_FLASHA_WP		(1<<5)
#define SYS_FBOOTB_WP		(1<<3)
#define SYS_FBA_WP_HDR		(1<<2)
#define SYS_FBOOTB_WP_HDR	(1<<1)

#define SYS_STATUS_2_REG	(1)

/* Forward Declarations */
STATIC struct bankdesc *
bankcheck(int bank, int quiet);

static int
flash_wp(int bank, int enbl);

STATIC uint32_t
read_us_timer(void);

/* Global Variables     */

/* motload memory map   */
static struct bankdesc mvme5500Flash[] = {
	{ 0, 2                   }, /* first entry gives number of entries */
	{ 0xf2000000, 0x08000000, 0x20000*2, 2, BSP_flash_vendor_intel, 0, 0, 0, },
	{ 0xff800000, 0x00800000, 0x20000*2, 2, BSP_flash_vendor_intel, 0, 0, 0, },
};

/* motload memory map */
static struct bankdesc mvme6100Flash[] = {
	{ 0, 2                   }, /* first entry gives number of entries */
	{ 0xf4000000, 0x04000000, 0x20000*2, 2, BSP_flash_vendor_intel, 0, 0, 0, },
	{ 0xf8000000, 0x04000000, 0x20000*2, 2, BSP_flash_vendor_intel, 0, 0, 0, },
};

struct flash_bsp_ops BSP_flashBspOps = {
	bankcheck    : bankcheck,
	flash_wp     : flash_wp,
	read_us_timer: read_us_timer,
};

/* set (enbl:1), clear (enbl:0) or query (enbl:-1) write protection
 * 
 * RETURNS 0 on success, nonzero on error.
 */
static int
flash_wp(int bank, int enbl)
{
BSP_BoardType b;
A8            p;
unsigned char	hwp = 0, swp;

	/* validate 'bank' argument */
	if ( !bankcheck( bank, 0 ) )
		return -1;

	switch ( (b=BSP_getBoardType()) ) {
		default:
			fprintf(stderr,"Unknown board type %i\n",b);
			return -1;
			
		case MVME5500:
			/* bit enables both banks; no readback of jumper available */
			p = (A8)(BSP_MV64x60_DEV1_BASE + SYS_STATUS_2_REG);
			swp = SYS_FLASHA_WP;
			break;

		case MVME6100:
			{

				p = (A8)(BSP_MV64x60_DEV1_BASE + SYS_STATUS_2_REG);
				if ( 0 == bank ) {
					hwp = SYS_FBA_WP_HDR;
					swp = SYS_FLASHA_WP;
				} else {
					hwp = SYS_FBOOTB_WP_HDR;
					swp = SYS_FBOOTB_WP;
				}
				if ( enbl && (*p & hwp) ) {
					fprintf(stderr,"HW write protection enabled (jumper)\n");
					return -1;
				}
			}
			break;
	}
	if ( -1 == enbl ) {
		/* query */
		return *p & (swp | hwp);
	} else {
		if ( enbl ) {
			*p |= swp;
		} else {
			*p &= ~swp;
		}
	}
	return 0;
}

/* Lookup bank description in table */
STATIC struct bankdesc *
bankcheck(int bank, int quiet)
{
struct bankdesc *b;
	switch ( BSP_getBoardType() ) {
		case MVME5500:	b = mvme5500Flash; break;
		case MVME6100:  b = mvme6100Flash; break;
		default:
			fprintf(stderr,"Unknown/unsupported board type\n");
			return 0;
	}
	if ( bank >= b->size || bank < 0 ) {
		if ( !quiet )
			fprintf(stderr,"Invalid flash bank #: %i; (too big)\n", bank);
		return 0;
	}
	return b + bank + 1;
}

STATIC uint32_t read_us_timer(void)
{
uint32_t now, mhz;

	/* we burn cycles anyways... */
	mhz = BSP_bus_frequency/BSP_time_base_divisor/1000;

	asm volatile("mftb %0":"=r"(now));

	return now/mhz;
}

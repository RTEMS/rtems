/*
 * Trivial driver for spansion flash present on the
 * MVME3100 board.
 *
 * For recognized devices, look for 'spansionDevs'.
 *
 * This driver has only been tested with stride=4
 * and in 16-bit mode (width=2).
 */

/*
 * Authorship
 * ----------
 * This software was created by
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
#include <stdio.h>
#include <inttypes.h>

#include <bsp/flashPgmPvt.h>

#define DEBUG			5
#undef  DEBUG

#ifdef DEBUG
#define STATIC
#else
#define STATIC          static
#endif

/* Manual says max erase time is 3.5 s */
#define ERASE_TIMEOUT	4	 /* seconds */
#define WRITE_TIMEOUT	1000 /* us; manual says: 240us typ. */

/* Assume flash-endianness == CPU endianness */

#ifdef __PPC__
#define IOSYNC(mem)	do { __asm__ volatile("eieio"); } while (0)
#else
#define IOSYNC(mem)	do { } while (0)
#endif

/********* Forward Declarations ****************/

STATIC int
flash_get_id_s160(struct bankdesc *, uint32_t , uint32_t *, uint32_t *);

STATIC void
flash_unlock_block_s160(struct bankdesc *, uint32_t);

STATIC void
flash_lock_block_s160(struct bankdesc *, uint32_t);

STATIC int
flash_erase_block_s160(struct bankdesc *, uint32_t);

STATIC uint32_t
flash_check_ready_s160(struct bankdesc *, uint32_t);

STATIC void
flash_print_stat_s160(struct bankdesc *, uint32_t, int);

STATIC void
flash_array_mode_s160(struct bankdesc *, uint32_t);

STATIC uint32_t
flash_write_line_s160(struct bankdesc *, uint32_t, char *, uint32_t);

/********* Global Variables ********************/

static struct flash_bank_ops spansionOps = {
	get_id      : flash_get_id_s160,
	unlock_block: flash_unlock_block_s160,
	lock_block  : flash_lock_block_s160,
	erase_block : flash_erase_block_s160,
	check_ready : flash_check_ready_s160,
	print_stat  : flash_print_stat_s160,
	array_mode  : flash_array_mode_s160,
	write_line  : flash_write_line_s160,
};

static struct devdesc spansionDevs[] = {
	{ 0x007e2101, "S29GL128N", 0x01000000, 32, 0x20000 }, /* 16MB */
	{ 0x007e2201, "S29GL256N", 0x02000000, 32, 0x20000 }, /* 32MB */
	{ 0x007e2301, "S29GL512N", 0x04000000, 32, 0x20000 }, /* 64MB */
	{ 0, 0, 0, 0}
};

struct vendesc BSP_flash_vendor_spansion[] = {
	{ 0x01, "Spansion/AMD", spansionDevs, &spansionOps },
	{ 0, 0}
};

/********* Register Definitions ****************/

#define UNLK1_ADDR_16      0x555
#define UNLK1_DATA         0xaa
#define UNLK2_ADDR_16      0x2aa
#define UNLK2_ADDR_8       0x555
#define UNLK2_DATA         0x55
#define ASEL_DATA          0x90
#define VEND_ID_ADDR_16    0x000
#define SPROT_ADDR_16      0x002
#define DEV1_ID_ADDR_16    0x001
#define DEV2_ID_ADDR_16    0x00e
#define DEV3_ID_ADDR_16    0x00f
#define ERASE_DATA         0x80
#define SECT_ERASE_DATA    0x30
#define DQ7_DATA           0x80
#define RESET_DATA         0xf0
#define WRBUF_DATA         0x25
#define PGBUF_DATA         0x29

#define DQ7_POLL_ALL       (-1)

/********* Helper Types ************************/

union bconv {
	uint32_t	u;
	uint16_t	s[2];
	char		c[4];
};

/********* Register Access Primitives **********/

/* All of these currently assume stride == 4, i.e.
 * two 16-bit devices or 4 8-bit devices in parallel.
 *
 * FIXME:
 *   8-bit mode and strides 1,2 untested.
 */

#define ADDR32(b, a, o)   ((a) + ((o)*FLASH_STRIDE(b)))

static inline uint32_t
fl_rd32(struct bankdesc *b, uint32_t a, uint32_t off)
{
volatile union bconv *p;
uint32_t              rval;

	if ( 1 == b->width )
		off <<= 1;

	a = ADDR32(b, a, off);

	p    = (volatile union bconv *)a;
	if ( 4 == FLASH_STRIDE(b) ) {
		rval = p->u;
		IOSYNC(p->u);
	} else if ( 2 == FLASH_STRIDE(b) ) {
		rval = p->s[0];
		IOSYNC(p->s[0]);
	} else {
		rval = p->c[0];
		IOSYNC(p->c[0]);
	}
	return rval;
}

static inline void
fl_wr32(struct bankdesc *b, uint32_t a, uint32_t v)
{
volatile union bconv *p = (volatile union bconv*)a;
	if ( 4 == FLASH_STRIDE(b) ) {
		p->u    = v;
		IOSYNC(p->u);
	} else if ( 2 == FLASH_STRIDE(b) ) {
		p->s[0] = v;
		IOSYNC(p->s[0]);
	} else {
		p->c[0] = v;
		IOSYNC(p->c[0]);
	}
}

static inline uint32_t
fl_splat32(struct bankdesc *b, uint32_t x)
{
	if ( 4 == FLASH_STRIDE(b) ) {
		if ( 1 == b->width ) {
			x = (x << 8) | x;
		}
		x = (x<<16) | x;
	} else if ( 2 == FLASH_STRIDE(b) ) {
		if ( 1 == b->width )
			x = (x << 8) | x;
	}
	return x;
}

static inline uint32_t
fl_x32(struct bankdesc *b, union bconv *pv)
{
	if ( 4 == FLASH_STRIDE(b) )
		return pv->u;
	else if ( 2 == FLASH_STRIDE(b) )
		return pv->s[0];
	else
		return pv->c[0];
}

static inline void
fl_wr32_cmd(struct bankdesc *b, uint32_t a, uint32_t off, uint32_t cmd)
{
	if ( 1 == b->width ) {
		if ( off == UNLK2_ADDR_16 )
			off = UNLK2_ADDR_8;
		else
			/* all others are simply left shifted */
			off <<= 1;
	}
	cmd = fl_splat32(b, cmd);
	a   = ADDR32(b, a, off);
	fl_wr32(b, a, cmd);
}

/* Send unlock sequence */
static inline void unlk(struct bankdesc *b, uint32_t a)
{
	a &= ~ ( ADDR32(b, 0,0x1000) - 1 );
	fl_wr32_cmd(b, a, UNLK1_ADDR_16, UNLK1_DATA);
	fl_wr32_cmd(b, a, UNLK2_ADDR_16, UNLK2_DATA);
}

/********* Helper Routines *********************/

STATIC int
sector_is_protected(struct bankdesc *b, uint32_t addr)
{
int rval;
	unlk(b, addr);
	fl_wr32_cmd(b, addr, UNLK1_ADDR_16, ASEL_DATA);
	rval = fl_rd32(b, addr, SPROT_ADDR_16);
	flash_array_mode_s160(b, addr);
	return rval;
}

STATIC int fl_dq7_poll(struct bankdesc *b, uint32_t addr, uint32_t d7_val)
{
	d7_val &= fl_splat32(b, DQ7_DATA);
	return ( (fl_rd32(b, addr, 0) & fl_splat32(b, DQ7_DATA)) == d7_val );
}

/* Do DQ7 polling until DQ7 reads the value passed in d7_val
 * or timeout
 */
STATIC int
flash_pend(struct bankdesc *b, uint32_t addr, uint32_t timeout_us, uint32_t d7_val)
{
uint32_t then = BSP_flashBspOps.read_us_timer();
uint32_t now  = then;

	do {
		if ( fl_dq7_poll(b, addr, d7_val) ) {
#if (DEBUG > 4)
			printf("Write buffer succeded after %"PRIi32"us\n", (now-then)*8/333);
#endif
			return 0;
		}
		now = BSP_flashBspOps.read_us_timer();
	} while ( now - then < timeout_us );

	return -1;
}


/********* Access Methods **********************/

STATIC void
flash_array_mode_s160(struct bankdesc *b, uint32_t addr)
{
	fl_wr32_cmd(b, addr, 0, RESET_DATA);
}

STATIC int
flash_get_id_s160(struct bankdesc *b, uint32_t addr, uint32_t *pVendorId, uint32_t *pDeviceId)
{
uint32_t dev_id[3], x, i;

	if ( 4 != FLASH_STRIDE(b) )
		fprintf(stderr,"Warning: strides other than 4 untested\n(%s at %d)\n",
			__FILE__,__LINE__);

	if ( 2 != b->width )
		fprintf(stderr,"Warning: device width other than 2 untested\n(%s at %d)\n",
			__FILE__,__LINE__);

	addr &= ~ (ADDR32(b, 0, 0x1000) - 1);
	unlk(b, addr);
	fl_wr32_cmd(b, addr, UNLK1_ADDR_16, ASEL_DATA);
	*pVendorId = fl_rd32(b, addr, VEND_ID_ADDR_16) & 0xff;
	dev_id [0] = fl_rd32(b, addr, DEV1_ID_ADDR_16);
	dev_id [1] = fl_rd32(b, addr, DEV2_ID_ADDR_16);
	dev_id [2] = fl_rd32(b, addr, DEV3_ID_ADDR_16);

#ifdef DEBUG
	printf("Vendor Id 0x%08"PRIx32", Dev Ids: 0x%08"PRIx32", 0x%08"PRIx32", 0x%08"PRIx32"\n",
		*pVendorId, dev_id[0], dev_id[1], dev_id[2]);
#endif

	flash_array_mode_s160(b, addr);

	for ( x=0, i=0; i<3; i++ ) {
		x = (x<<8) | (dev_id[i] & 0xff);
	}

	*pDeviceId = x;

	return 0;
}


STATIC void
flash_lock_block_s160(struct bankdesc *b, uint32_t addr)
{
}

STATIC void
flash_unlock_block_s160(struct bankdesc *b, uint32_t addr)
{
}

STATIC uint32_t
flash_check_ready_s160(struct bankdesc *b, uint32_t addr)
{
	flash_array_mode_s160(b, addr);
	return 0;
}

/* Erase single block holding 'addr'ess
 *
 * RETURNS: zero on error, device status on failure.
 *
 *   NOTES: - device switched back to array mode on exit.
 *          - 'addr' must be 32-bit aligned.
 */
STATIC int
flash_erase_block_s160(struct bankdesc *b, uint32_t addr)
{
rtems_interval p,i;

	addr &= ~ (b->fblksz-1);

	if ( sector_is_protected(b, addr) ) {
		fprintf(stderr,"Sector at 0x%08"PRIx32" is protected\n", addr);
		return -10;
	}

	unlk(b, addr);
	fl_wr32_cmd(b, addr, UNLK1_ADDR_16, ERASE_DATA);
	unlk(b, addr);
	fl_wr32_cmd(b, addr, 0, SECT_ERASE_DATA);

	p = rtems_clock_get_ticks_per_second();
	p *= ERASE_TIMEOUT;

	for ( i=p; i; i-- ) {
		rtems_task_wake_after(1);
		if ( fl_dq7_poll(b, addr, DQ7_POLL_ALL) ) {
			break;
		}
	}
#ifdef DEBUG
	printf("ERASE polled for %"PRIi32" ticks\n", p-i);
#endif
	flash_array_mode_s160(b, addr);

	if ( i ) {
		/* write successful; verify */
		for ( i = 0; i < b->fblksz; i++ ) {
			if ( 0xff != ((char*)addr)[i] ) {
				fprintf(stderr,"ERROR: Erase verification failed at %p\n",
					((char*)addr) + i);
				return -1;
			}
		}
		return 0;
	}
	return -1;
}

STATIC void
flash_print_stat_s160(struct bankdesc *b, uint32_t sta, int verbose)
{
	fprintf(stderr,"Flash Spansion 160 error %"PRIi32"\n", sta);
}

STATIC uint32_t
flash_write_line_s160(struct bankdesc *b, uint32_t a, char *s, uint32_t N)
{
uint32_t        sta, nxt, j, v;
union    bconv  buf;

	if ( 0 == N )
		return -11;

	if ( N & (FLASH_STRIDE(b) - 1) ) {
		fprintf(stderr,"flash_write_line_s160: invalid byte count (not multiple of stride\n");
		return -10;
	}

	unlk(b, a);

	/* address block */
	fl_wr32_cmd(b, a, 0, WRBUF_DATA);

	/* (16-bit) word count per device */
	N /= FLASH_STRIDE(b);

	fl_wr32_cmd(b, a, 0, N-1);

	/* silence compiler warning about uninitialized var (N > 0 at this point) */
	v = 0;

	/* fill buffer */
	for (nxt = a; N>0; N--) {
#if (DEBUG > 4)
		printf("Writing DAT *0x%08"PRIx32" = 0x%08"PRIx32"\n", nxt, *(uint32_t*)s);
#endif
		/* deal with misaligned sources */
		for ( j=0; j<FLASH_STRIDE(b); j++ ) {
			buf.c[j] = *s++;
		}
		v = fl_x32(b, &buf);
		fl_wr32(b, nxt, v);
		nxt += FLASH_STRIDE(b);
	}

	/* burn buffer */
	fl_wr32_cmd(b, a, 0, PGBUF_DATA);

	/* pend */

	sta = flash_pend(b, nxt - FLASH_STRIDE(b), WRITE_TIMEOUT, v);

	return sta;
}

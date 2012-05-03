/*
 * Trivial driver for 16-bit intel flash present on the
 * MVME5500/MVME6100 boards.
 *
 * For recognized devices, look for 'intelDevs'.
 *
 * This driver currently only supports stride=4 and 16-bit
 * mode (width=2).
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
#ifdef TESTING

#define TIMEOUT_US       100000
#define rtems_task_wake_after(t) sleep(t)
#define CLOCKRATE_GET(p) (*(p)=1)

#else

#include <rtems.h>
#define TIMEOUT_US       1000
#define CLOCKRATE_GET(p) rtems_clock_get( RTEMS_CLOCK_GET_TICKS_PER_SECOND, p )

#endif

#define ERASE_TIMEOUT_S 2

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

#include <bsp/flashPgmPvt.h>

#define  DEBUG		0
#undef   DEBUG

#ifdef   DEBUG
#define  STATIC
#else
#define  STATIC				static
#endif

#define NumberOf(arr)		(sizeof(arr)/sizeof(arr[0]))

/* This driver assumes two 16-bit devices in parallel */

/********* Register Definitions ****************/

/* command codes */
#define F_CMD_RD_ARR		0xffffffff	/* back to 'normal' read mode */
#define F_CMD_RD_ID			0x90909090	/* read from ID space         */
#define F_CMD_RD_STA		0x70707070	/* read status register       */
#define F_CMD_WR_STA		0x50505050	/* clear status register      */
#define F_CMD_WR_BUF		0xe8e8e8e8	/* write to buffer            */
#define F_CMD_WR_WRD		0x40404040	/* write word                 */
#define F_CMD_WR_ERA		0x20202020	/* block erase                */
#define F_CMD_WR_LCK		0x60606060	/* lock bit (1st cycle)       */
#define F_CMD_WR_CMD		0xd0d0d0d0	/* commit erase               */
#define F_CMD_WR_LCK_SET	0x01010101	/* lock block commit          */

/* Status codes (F_CMD_RD_STA result)        */
#define STA_RDY		(1<<7)	/* ready         */
#define STA_ES 		(1<<6)	/* erase suspend */
#define STA_EE		(1<<5)	/* erase error   */
#define STA_PE		(1<<4)	/* program error */
#define STA_VE		(1<<3)	/* VPEN < min    */
#define STA_PS		(1<<2)	/* program susp. */
#define STA_LE		(1<<1)	/* block locked  */
#define STA_EFP		(1<<0)	/* buf. EFP stat.*/

/* Any error                                 */
#define STA_ERROR	(STA_EE|STA_PE|STA_VE|STA_LE)

/* TODO: Code using RDYRDY assumes flash is 16-bit wide :-( */
#define STA_RDYRDY			0x00800080	/* ready status on both devices */

/********* Forward Declarations ****************/

STATIC int
flash_get_id_intel(struct bankdesc *, uint32_t, uint32_t *, uint32_t *);

STATIC void
flash_unlock_block_intel(struct bankdesc *, uint32_t);

STATIC void
flash_lock_block_intel(struct bankdesc *, uint32_t);

STATIC int
flash_erase_block_intel(struct bankdesc *, uint32_t);

STATIC uint32_t
flash_check_ready_intel(struct bankdesc *, uint32_t);

STATIC void
flash_print_stat_intel(struct bankdesc *, uint32_t, int);

STATIC void
flash_array_mode_intel(struct bankdesc *, uint32_t);

STATIC uint32_t
flash_write_line_intel(struct bankdesc *, uint32_t, char *, uint32_t);

/********* Global Variables ********************/

static struct flash_bank_ops intelOps = {
	get_id      : flash_get_id_intel,
	unlock_block: flash_unlock_block_intel,
	lock_block  : flash_lock_block_intel,
	erase_block : flash_erase_block_intel,
	check_ready : flash_check_ready_intel,
	print_stat  : flash_print_stat_intel,
	array_mode  : flash_array_mode_intel,
	write_line  : flash_write_line_intel,
};

static struct devdesc intelDevs[] = {
	{ 0x8801, "K3 64Mb", 		 8*1024*1024, 	0x40, 0x20000 },
	{ 0x8802, "K3 128Mb", 		16*1024*1024, 	0x40, 0x20000 },
	{ 0x8803, "K3 256Mb", 		32*1024*1024, 	0x40, 0x20000 },
	{ 0x8805, "K18 64Mb", 		 8*1024*1024, 	0x40, 0x20000 },
	{ 0x8806, "K18 128Mb", 		16*1024*1024, 	0x40, 0x20000 },
	{ 0x8807, "K18 256Mb", 		32*1024*1024, 	0x40, 0x20000 },
	{ 0x0016, "J3 32Mb",	     4*1024*1024,   0x20, 0x20000 },
	{ 0x0017, "J3 64Mb",	     8*1024*1024,   0x20, 0x20000 },
	{ 0x0018, "J3 128Mb",	    16*1024*1024,   0x20, 0x20000 },
	{ 0x001d, "J3 256Mb",	    32*1024*1024,   0x20, 0x20000 },
	{ 0, 0, 0, 0}
};

struct vendesc BSP_flash_vendor_intel[] =
{
	{ 0x89, "Intel", intelDevs, &intelOps },
	{ 0, 0}
};

/********* Helper Subroutines ******************/

/* Basic low-level access routine for writing a command to the
 * internal state machine.
 *
 * Flash is slow, so who cares if these access routines
 * are not extremely efficient...
 */
STATIC uint32_t
BSP_flashReadRaw(uint32_t cmd, uint32_t addr)
{
#if DEBUG > 4
	printf("Writing CMD *0x%08"PRIx32" = 0x%08"PRIx32"\n", addr, cmd);
#endif
#ifdef TESTING
	return STA_RDYRDY;
#else
	if ( cmd & 0xffff0000 ) {
		/* 32-bit access */
		addr &= ~(sizeof(uint32_t)-1);
		*(A32)addr = cmd;
		return *(A32)addr;
	} else if ( cmd & 0xffffff00 ) {
		/* 16-bit access */
		addr &= ~(sizeof(uint16_t)-1);
		*(A16)addr = cmd;
		return *(A16)addr;
	} else {
		*(A8)addr = cmd;
		return *(A8)addr;
	}
#endif
}

STATIC void
BSP_flashWriteRaw(uint32_t val, uint32_t addr)
{
#ifdef TESTING
	printf("Writing CNT *0x%08"PRIx32" = 0x%08"PRIx32"\n", addr, val);
#else
/* TODO implicitly assumes FLASH_WIDTH = 2, FLASH_NDEVS = 2 */
	/* 32-bit access */
	addr &= ~(sizeof(uint32_t)-1);
	*(A32)addr = val;
#endif
}

STATIC uint32_t
flash_pend(struct bankdesc *b, uint32_t a, uint32_t timeout_us)
{
uint32_t then, now, sta;

	then = BSP_flashBspOps.read_us_timer();

	do {
		sta = BSP_flashReadRaw(F_CMD_RD_STA, a);
		now = BSP_flashBspOps.read_us_timer();
		if ( now-then > timeout_us ) {
			/* timeout */
			sta = -1;
			break;
		}
	} while ( STA_RDYRDY != (STA_RDYRDY & sta) );

	/* switch back to normal mode */
	flash_array_mode_intel(b, a);

	return STA_RDYRDY == sta ? 0 : sta;
}


/********* Access Methods **********************/

STATIC void
flash_array_mode_intel(struct bankdesc *b, uint32_t a)
{
	BSP_flashReadRaw(F_CMD_RD_ARR, a);
}

/* Dump status bits (F_CMD_RD_STA results); 'verbose' prints non-error bits, too */
STATIC void
flash_print_stat_intel(struct bankdesc *b, uint32_t sta, int verbose)
{
int ch;
	if ( sta & STA_ERROR ) {
		ch = ':';
		fprintf(stderr,"Errors found");
		if ( STA_EE & sta ) {
			fprintf(stderr,"%c ERASE",ch);
			ch = ',';
		}
		if ( STA_PE & sta ) {
			fprintf(stderr,"%c PROGRAM",ch);
			ch = ',';
		}
		if ( STA_VE & sta ) {
			fprintf(stderr,"%c VPEN TOO LOW",ch);
			ch = ',';
		}
		if ( STA_LE & sta ) {
			fprintf(stderr,"%c BLOCK LOCKED",ch);
			ch = ',';
		}
		fprintf(stderr,"\n");
	}
	if ( verbose ) {
		fprintf(stderr,"%sREADY\n",STA_RDY & sta ? "" : "NOT ");
	}
}


/* Query the status of the device and assert it's readiness
 * leave off in array-reading mode.
 *
 * RETURNS: 0 on success, error status (result of status query) on error.
 *
 *   NOTES: - error message is printed to stderr.
 *          - device switched back to array mode on exit.
 *          - 'addr' must be 32-bit aligned.
 */
STATIC uint32_t
flash_check_ready_intel(struct bankdesc *b, uint32_t addr)
{
uint32_t sta;

	(void)BSP_flashReadRaw(F_CMD_WR_STA, addr);
	if ( STA_RDYRDY != (STA_RDYRDY & (sta=BSP_flashReadRaw(F_CMD_RD_STA, addr))) ) {
		fprintf(stderr,"Flash not ready (@0x%08"PRIx32")\n", addr);
		flash_print_stat_intel(b, sta, 0);
	} else {
		sta = 0;
	}
	/* switch back to normal mode */
	flash_array_mode_intel(b, addr);
	return sta;
}

/* Erase single block holding 'addr'ess
 *
 * RETURNS: zero on error, device status on failure.
 *
 *   NOTES: - device switched back to array mode on exit.
 *          - 'addr' must be 32-bit aligned.
 */
STATIC int
flash_erase_block_intel(struct bankdesc *b, uint32_t addr)
{
uint32_t       sta;
int            i;
rtems_interval p;

	if ( (sta = flash_check_ready_intel(b, addr)) )
		return sta;

	(void)BSP_flashReadRaw(F_CMD_WR_ERA, addr);
	(void)BSP_flashReadRaw(F_CMD_WR_CMD, addr);

	CLOCKRATE_GET( &p );
	i = p * ERASE_TIMEOUT_S;

	while ( STA_RDYRDY != (STA_RDYRDY & (sta = BSP_flashReadRaw(F_CMD_RD_STA, addr))) && --i > 0 ) {
		rtems_task_wake_after(1);
	}
	/* switch back to 'normal' mode */
	(void)flash_array_mode_intel(b, addr);
	if ( 0 == i ) {
		fprintf(stderr,"Flash erase block: timeout\n");
		return -1;
	}

	/* Verify */
	for ( i = 0; i<b->fblksz; i++ ) {
		if ( (char)0xff != ((char*)addr)[i] ) {
			fprintf(stderr,"ERROR: Erase verification failed at %p\n",
					((char*)addr) + i);
			return -1;
		}
	}
	return STA_RDYRDY == sta ? 0 : sta;
}

/* Unlock block holding 'addr'ess
 *
 *   NOTES: - device switched back to array mode on exit.
 *          - 'addr' must be 32-bit aligned.
 */

STATIC void
flash_unlock_block_intel(struct bankdesc *b, uint32_t addr)
{
#ifdef DEBUG
	printf("Unlocking block 0x%08"PRIx32"\n", addr);
#endif
	(void)BSP_flashReadRaw(F_CMD_WR_LCK, addr);
	(void)BSP_flashReadRaw(F_CMD_WR_CMD, addr);
	flash_pend(b, addr, TIMEOUT_US);
}

/* Lock block holding 'addr'ess
 *
 *   NOTES: - device switched back to array mode on exit.
 *          - 'addr' must be 32-bit aligned.
 */

STATIC void
flash_lock_block_intel(struct bankdesc *b, uint32_t addr)
{
#ifdef DEBUG
	printf("Locking block 0x%08"PRIx32"\n", addr);
#endif
	(void)BSP_flashReadRaw(F_CMD_WR_LCK, addr);
	(void)BSP_flashReadRaw(F_CMD_WR_LCK_SET, addr);
	flash_pend(b, addr, TIMEOUT_US);
}

STATIC uint32_t
flash_write_line_intel(struct bankdesc *b, uint32_t a, char *s, uint32_t N)
{
uint32_t sta, Nspla, nxt, j;
union	{
	uint32_t	u;
	char		c[sizeof(uint32_t)];
}		buf;

	/* address block */
	if ( STA_RDYRDY != (sta = BSP_flashReadRaw(F_CMD_WR_BUF, a)) ) {
		return sta;
	}

	/* count per device */
	N /= FLASH_STRIDE(b);

	/* splat out */
	Nspla = (N<<8)  | N;
	Nspla = (Nspla<<16) | Nspla;
	BSP_flashWriteRaw(Nspla - 0x01010101, a);

	/* fill buffer */
	for (nxt = a; N>0; N--) {
#if defined(TESTING) || (DEBUG > 4)
		printf("Writing DAT *0x%08"PRIx32" = 0x%08"PRIx32"\n", nxt, *(uint32_t*)s);
#endif
		/* deal with misaligned sources */
		for ( j=0; j<sizeof(buf.u); j++ ) {
			buf.c[j] = *s++;
		}
		*(A32)nxt = buf.u;
		nxt += FLASH_STRIDE(b);
	}

	BSP_flashReadRaw(F_CMD_WR_CMD, a);

	sta = flash_pend(b, a, TIMEOUT_US);

	return sta;
}

/* Query device for basic information verifying that we talk
 * to a 'known'/'supported' device.
 *
 *   NOTES: - device switched back to array mode on exit.
 *          - 'addr' must be 32-bit aligned.
 */

STATIC int
flash_get_id_intel(struct bankdesc *b, uint32_t addr, uint32_t *pVendorId, uint32_t *pDeviceId)
{
uint16_t v,d;

	if ( 4 != FLASH_STRIDE(b) ) {
		fprintf(stderr,"intel flash programmer: Strides other than 4 not implemented yet\n");
		return -1;
	}

	/* Try to read ID */
	v = BSP_flashReadRaw(F_CMD_RD_ID, addr);
	d = BSP_flashReadRaw(F_CMD_RD_ID, addr + FLASH_STRIDE(b));

	/* switch to array mode */
	flash_array_mode_intel(b, addr);

	*pVendorId = v;
	*pDeviceId = d;

	return 0;
}

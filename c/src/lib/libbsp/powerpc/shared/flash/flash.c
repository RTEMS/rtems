/* Trivial Flash Programmer                                  */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2006 */

/* To keep things simple, this driver makes a few assumptions about the
 * hardware:
 *
 * - no CFI
 * - devices operate with 16-bit data width
 * - two devices are used in parallel (stride 4) to
 *   provide 32-bit data. I.e., the devices are
 *   organized like this:
 *   unsigned short flash[FLASH_SIZE][2];
 * - no endianness issues (i.e., flash endianness ==  CPU endianness)
 * - fixed block size
 * - fixed buffer size
 * - all devices in a bank are identical
 * - NOT THREAD SAFE; no locking scheme is implemented.
 * - cannot copy within same flash bank.
 * - timeout uses polling/busy-wait.
 *
 * NOTE: some attempts have been made to remove the restrictions
 * on stride and 16-bit width with the goal to support widths 1/2 (bytes)
 * and strides 1/2/4 and all (legal) combinations thereof.
 * However, the intel chip driver only implements stride 4 / width 2
 * and other combinations are untested.
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
#ifndef TESTING
#include <rtems.h>
#include <bsp/flashPgm.h>
#include <bsp/flashPgmPvt.h>
#else
#include "flashPgm.h"
#include "flashPgmPvt.h"
#endif

#include <unistd.h>
#include <stdio.h>
#include <inttypes.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdlib.h>

#define  DEBUG		0
#undef   DEBUG

#ifdef   DEBUG
#define  STATIC
#else
#define  STATIC				static
#endif

/* Forward decls. */

STATIC uint32_t
BSP_flashProbeSize(struct bankdesc *b);

STATIC struct bankdesc *
bankValidate(int bank, int quiet);

static struct bankdesc *
argcheck(int bank, uint32_t offset, char *src, uint32_t size);

/* Type definitions */

union bconv	{
	uint32_t u;
	uint16_t s[2];
	char     c[4];
};

/* Little helpers... */

/* Read parallel devices */
static void
rd_par(struct bankdesc *b, union bconv *pv, uint32_t a)
{
	if ( 4 == FLASH_STRIDE(b) ) {
		pv->u    = *(_u32_a_t*)a;
	} else if ( 2 == FLASH_STRIDE(b) ) {
		pv->s[0] = *(_u16_a_t*)a;
	} else {
		pv->c[0] = *(_u8_a_t*)a;
	}
}

/* 'flush' input buffer and get an upper-case char from stdin */
STATIC int
getUc(void)
{
	fseek(stdin, 0, SEEK_END);
	return toupper(getchar());
}

/* Advance rotating progress indicator on stdout.
 * The idea is that the caller updates a state variable
 * using 'flip':
 *
 * unsigned f = 0;
 *
 *    // advance indicator
 *    f = flip(f);
 *
 *    ...
 *
 *    // erase indicator
 *    wipe(f);
 */
static unsigned flip(unsigned x)
{
static char bar[]= { '/', '-', '\\', '|' };
	putchar('\b');
	putchar(bar[ x & 3]);
	fflush(stdout);
	return x+1;
}

/* If f!=0 then erase one char on stdout.
 * ( If 'flip' was never called then f should still
 * be zero and no action is taken).
 */
static void wipe(unsigned f)
{
	if ( f ) {
		putchar('\b');
		putchar(' ');
		putchar('\b');
		fflush(stdout);
	}
}

/* lookup vendor ID in table of known vendors using ops
 * associated with the vendor.
 */
STATIC struct vendesc *
knownVendor(struct bankdesc *b, uint32_t addr, uint32_t *pd, unsigned quiet)
{
uint32_t        v;
struct vendesc *rval;

	for ( rval=b->knownVendors; rval->name; rval++ ) {

		if ( rval->ops->get_id(b, addr, &v, pd) ) {
			if ( quiet < 2 )
				fprintf(stderr,"Unable to read vendor/device info at 0x%08"PRIx32"\n", addr);
			return 0;
		}

		if ( rval->id == v ) {
			return rval;
		}
	}
	if ( quiet < 2 )
		fprintf(stderr,"Unknown vendor id (0x%04"PRIx32") at 0x%08"PRIx32"\n",v, addr);
	return 0;
}

/* lookup device ID in table of known devices */
STATIC struct devdesc *
knownDevice(struct vendesc *v, uint32_t d)
{
struct devdesc *rval;
	for ( rval=v->known_devs; rval->name; rval++ ) {
		if ( rval->id == d ) {
			return rval;
		}
	}
	return 0;
}

/* Write 'n_words' (32-bit) from 'src' to 'addr'ess on flash.
 * (src is a char* to emphasize that no src-alignment is required)
 *
 * RETURNS: 0 on success, (destination) address of failure on error.
 *
 *   NOTES: - device switched back to array mode on exit.
 *          - 'addr' must be 32-bit aligned.
 */

STATIC uint32_t
BSP_flashWriteDataRaw(struct bankdesc *b, uint32_t addr, char *src, uint32_t n_words, int quiet)
{
uint32_t sta;
uint32_t N;
uint32_t nxt, a, i, bufsz;
uint32_t then, now;
unsigned f;
char     *s;

#ifdef DEBUG
	printf("\nflashWriteDataRaw(0x%08"PRIx32", %p, 0x%"PRIx32")\n", addr, src, n_words);
#endif

	if ( 0 == n_words ) {
		return 0;
	}

	if ( !b ) {
		fprintf(stderr,"Missing bank descriptor argument\n");
		return -1;
	}
	if ( !b->dd ) {
		fprintf(stderr,"Bank descriptor not initialized\n");
		return -1;
	}

	if ( addr & (FLASH_STRIDE(b)-1) ) {
		fprintf(stderr,"Misaligned address (not on word boundary) 0x%08"PRIx32"\n", addr);
		return -1;
	}

	if ( (sta = b->ops->check_ready(b, addr)) ) {
		/* Error msgs have already been printed */
		return addr;
	}

	bufsz = FLASH_NDEVS(b) * b->dd->bufsz;

	then = BSP_flashBspOps.read_us_timer();

	for ( f = 0, a = addr, s=src, i = n_words; i ; s+=N ) {
		/* start of next buffer */
		nxt = (a + bufsz) & ~(bufsz-1);

		/* number of bytes */
		N = (nxt - a);

		if ( N > i * FLASH_STRIDE(b) )
			N = i * FLASH_STRIDE(b);

		i -= N/FLASH_STRIDE(b);

		if ( (sta = b->ops->write_line(b, a, s, N)) )
			goto bail;

		if ( ! quiet && (now = BSP_flashBspOps.read_us_timer()) - then > 500000 ) {
			f = flip(f);
			then = now;
		}

		a = nxt;
	}

	sta = 0;

	/* verify */
	for ( i=0, a=addr; i < n_words * FLASH_STRIDE(b); i++, a++ ) {
		if ( *(char*)a != src[i] ) {
			sta = -2;
			goto bail;
		}
	}

bail:
	if ( ! quiet ) {
		wipe(f);
	}
	if ( sta ) {
		switch ( sta ) {
			default:
				fprintf(stderr,"Error (flashWriteDataRaw): write error\n");
				b->ops->print_stat(b, sta,0);
			break;

			case -1:
				fprintf(stderr,"Error (flashWriteDataRaw): Timeout\n");
			break;

			case -2:
				fprintf(stderr,"Error (flashWriteDataRaw): write verification failed at 0x%08"PRIx32"\n", a);
			break;
		}
		b->ops->array_mode(b, a);
	} else {
		/* no errors */
		a = 0;
	}
	return a;
}


/* Query device for basic information verifying that we talk
 * to a 'known'/'supported' device.
 *
 * This is not really clean since (until we implement CFI)
 * we already need to know what kind of device it is to
 * be able to read its ID...
 *
 *   NOTES: - device switched back to array mode on exit.
 *          - 'addr' must be 32-bit aligned.
 */

STATIC struct devdesc *
BSP_flashCheckId(struct bankdesc *b, uint32_t addr, unsigned quiet)
{
uint8_t  x;
uint32_t d;
struct vendesc  *vd;
struct devdesc  *dd;

	/* check if it's flash at all: */
	x = *(A8)addr;
	*(A8)addr = ~x;
	if ( x != *(A8)addr ) {
		/* restore */
		*(A8)addr = x;
		if ( quiet < 3 )
			fprintf(stderr,"Addr 0x%08"PRIx32" seems to be RAM!\n", addr);
		return 0;
	}

	if ( !(vd = knownVendor(b, addr, &d, quiet)) ) {
		return 0;
	}

	/* Use the vendor ops for this bank */
	b->ops    = vd->ops;

	if ( !quiet )
		printf("Flash device, vendor: %s", vd->name);

	if ( !(dd = knownDevice(vd, d)) ) {
		if ( !quiet )
			printf("\n");
		if ( quiet < 2 )
			fprintf(stderr,"Unknown device id (0x%04"PRIx32") at 0x%08"PRIx32"\n",d, addr);
		return 0;
	}

	/* logical sector size is device sector size
	 * multiplied by # of devices in parallel
	 */
	b->fblksz = dd->fblksz * FLASH_NDEVS(b);

	if ( !quiet )
		printf(", device: %s -- size 0x%08"PRIx32" bytes\n", dd->name, dd->size);
	return dd;
}

/* We don't have device info yet so just
 * use 64k alignment
 */
#define SCAN_BACK_OFFSET	0x10000

/* Scan address range for flash devices and compute total size
 * of bank.
 *
 * RETURNS: size in bytes.
 */
STATIC uint32_t
BSP_flashProbeSize(struct bankdesc *b)
{
int             max = b->max_size;
uint32_t		rval;
struct devdesc	*dd;
unsigned		q;
	if ( max > 0 ) {
		for ( rval = 0, q=1; rval < max && (dd = BSP_flashCheckId(b, b->start + rval, q)); q=3 ) {
			rval += dd->size * FLASH_NDEVS(b);
		}
	} else {
		/* bank is populated from the top; scan backwards */
		max = -max;
		for ( rval = 0, q=1; rval < max && (dd = BSP_flashCheckId(b, b->start + max - SCAN_BACK_OFFSET - rval, q)); q=3 ) {
			rval += dd->size * FLASH_NDEVS(b);
		}
		b->start += max - rval;
	}
	return rval;
}

uint32_t
BSP_flashStart(int bank)
{
struct bankdesc *b;
	if ( ! ( b = argcheck(bank, 0, 0, 0) ) )
		return -1;
	return b->start;
}

uint32_t
BSP_flashSize(int bank)
{
struct bankdesc *b;
	if ( ! ( b = argcheck(bank, 0, 0, 0) ) )
		return -1;
	return b->size;
}

uint32_t
BSP_flashBlockSize(int bank)
{
struct bankdesc *b;
	if ( ! ( b = argcheck(bank, 0, 0, 0) ) )
		return -1;
	return b->fblksz;
}


#ifndef TESTING

/* Obtain bank description making sure it is initialized and not write protected */
STATIC struct bankdesc *
bankValidate(int bank, int quiet)
{
struct bankdesc *b = BSP_flashBspOps.bankcheck(bank, quiet);

	/* If flash is write-protected then we can't even talk to it... */
	if ( BSP_flashBspOps.flash_wp(bank, -1) ) {
		fprintf(stderr,"Flash bank #%i is write-protected; use 'BSP_flashWriteEnable(int bank)' and/or jumper\n", bank);
		return 0;
	}

	if ( !b->dd && !(b->dd = BSP_flashCheckId(b, b->start,1)) ) {
		fprintf(stderr,"Error: unable to detect flash device in bank #%i\n", bank);
		return 0;
	}
	return b;
}

/* Validate arguments and write-protection status of 'bank'.
 *
 *    'bank': 0..max bank supported by board.
 *  'offset': 0..bank size - 1
 *     'src': src .. src + size - 1 must not overlap bank; 'src' may be NULL
 *            (check is skipped in this case)
 *
 *      RETURNS: pointer to bank description on success, NULL on error (invalid args;
 *               error message is printed to stderr).
 *
 * SIDE EFFECTS: probes for bank size and stores result in bank description table.
 */

static struct bankdesc *
argcheck(int bank, uint32_t offset, char *src, uint32_t size)
{
struct bankdesc *b;

	if ( !(b=bankValidate(bank, 0)) ) {
		return 0;
	}

	if ( !b->size && !(b->size = BSP_flashProbeSize(b)) ) {
		fprintf(stderr,"Configuration Error - unable to determine flash size\n");
		return 0;
	}
	if ( offset + size > b->size ) {
		fprintf(stderr,"Error: requested size exceeds available flash (0x%08"PRIx32" bytes)\n", b->size);
		return 0;
	}

	if ( src && ( src + size > (char*)b->start && src < (char*)(b->start + b->size) ) ) {
		fprintf(stderr,"Error: cannot copy data within flash bank\n");
		return 0;
	}


	return b;
}

/* Calculate region that needs to be erased from 'offset' and 'n_bytes'
 * handling alignment and checking for blank areas that need not be
 * erased.
 * Ask for user confirmation and erase calculated region.
 *
 * RETURNS: 0 on success, -1 or destination address on error.
 *
 *   NOTES: - device switched back to array mode on exit.
 *          - prints progress/messages.
 */
STATIC int
regionCheckAndErase(int bank, uint32_t offset, char *src, uint32_t n_bytes, int quiet)
{
struct bankdesc *b;
uint32_t         i;
char            *p;
uint32_t		a,e;

	if ( ! (b = argcheck(bank, offset, src, n_bytes)) )
		return -1;

	a = offset & ~(b->fblksz - 1);
	e = (offset + n_bytes +  b->fblksz - 1) & ~ (b->fblksz - 1);

	/* If 'offset' is not block-aligned then rest of the block must
	 * be free.
	 */
	if ( a != offset ) {
		a += b->fblksz;
		i  = ( a > offset + n_bytes ) ? offset + n_bytes : a;
		for ( p = (char*)(b->start + offset); p < (char*)(b->start + i); p++ ) {
			if ( (char)0xff != *p ) {
				if ( ! quiet ) {
					fprintf(stderr,"Starting offset not block-aligned and destination area not empty.\n");
					fprintf(stderr,"I'll need to erase data below destination start\n");
				}
				a -= b->fblksz;
				break;
			}
		}
	}
	if ( e != offset + n_bytes ) {
		e -= b->fblksz;
		i  = ( e < offset ) ? offset : e;
		for ( p = (char*)(b->start + i); p < (char*)(b->start + offset + n_bytes); p++ ) {
			if ( (char)0xff != *p ) {
				if ( ! quiet ) {
					fprintf(stderr,"Ending offset not block-aligned and destination area not empty.\n");
					fprintf(stderr,"I'll need to erase data beyond destination end\n");
				}
				e += b->fblksz;
				break;
			}
		}
	}
	if ( ! quiet ) {
		if ( e > a )
			printf("ERASING 0x%08"PRIx32" .. 0x%08"PRIx32"\n", (b->start+a), (b->start + e - 1));
		printf("WRITING 0x%08"PRIx32" .. 0x%08"PRIx32"\n", (b->start+offset), (b->start + offset + n_bytes - 1));
		printf("OK to proceed y/[n]?"); fflush(stdout);
		if ( 'Y' != getUc() ) {
			printf("ABORTED\n");
			return -1;
		}
	}
	if ( e > a ) {
		if ( quiet < 2 ) {
			printf("ERASING  "); fflush(stdout);
		}

		if ( (i = BSP_flashErase(bank, a, e-a, quiet ? quiet : 1)) )
			return i;

		if ( quiet < 2 ) {
			printf("DONE\n");
		}
	}

	return 0;
}

/* Write to a flash region ('offset'..'offset'+'n_bytes'-1 on 'bank').
 *
 * RETURNS: 0 on success, -1 or destination address on error.
 *
 *   NOTES: - device switched back to array mode on exit.
 *          - no erase operation is performed.
 *          - written data is verified against source.
 */

STATIC int
BSP_flashWriteRegion(int bank, uint32_t offset, char *src, uint32_t n_bytes, int quiet)
{
struct bankdesc *b = BSP_flashBspOps.bankcheck(bank, 0);	/* caller did bankValidate() */
uint32_t     ab = offset & ~(b->fblksz - 1);
uint32_t     eb = (offset + n_bytes + b->fblksz - 1) & ~(b->fblksz - 1);
uint32_t     o,i,a,e;
int          err;
char        *p;
union bconv  buf;

	/* unlock */
	for ( i=ab; i<eb; i+=b->fblksz ) {
		b->ops->unlock_block(b, b->start + i );
	}

	err = 0;
	p = src;

	/* handle misaligned offset merging old contents */
	o = b->start + offset;
	a = o & ~(FLASH_STRIDE(b)-1);
	e = (o + n_bytes) & ~(FLASH_STRIDE(b)-1);

	if ( o > a ) {
		i = o - a;

		rd_par(b, &buf, a);

		while ( i < FLASH_STRIDE(b) && p < src + n_bytes ) {
			buf.c[i++] = *p++;
		}
		if ( (err = BSP_flashWriteDataRaw(b, a, buf.c, 1, quiet)) )
			goto bail;
		a += FLASH_STRIDE(b);
	}


	/* caution if misaligned data covering only one or two words */

	if ( e > a ) {
		i = (e-a);
		if ( (err = BSP_flashWriteDataRaw(b, a, p, i/FLASH_STRIDE(b), quiet)) )
			goto bail;
		p += i;
	}

	/* handle misaligned end */
	if ( o + n_bytes > e)  {
		rd_par(b, &buf, e);
		for ( i=0; p < src + n_bytes; ) {
			buf.c[i++] = *p++;
		}
		if ( (err = BSP_flashWriteDataRaw(b, e, buf.c, 1, quiet)) )
			goto bail;
	}

bail:
	/* lock area */
	for ( i=ab; i<eb; i+=b->fblksz ) {
		b->ops->lock_block(b, b->start + i );
	}

	/* final verification */
	if ( !err ) {
		for ( i=0; i<n_bytes; i++ ) {
			if ( ((char*)(b->start + offset))[i] != src[i] ) {
				fprintf(stderr,"Final verification failed at offset 0x%08"PRIx32"\n", (offset + i));
				return b->start + offset + i;
			}
		}
	}

	return err;
}

int
BSP_flashErase(int bank, uint32_t offset, uint32_t size, int quiet)
{
struct bankdesc *b;
uint32_t		 a,i;
int              f;

	if ( ! (b = argcheck(bank, offset, 0, size)) )
		return -1;

	if ( offset & (b->fblksz - 1) ) {
		fprintf(stderr,"Offset misaligned (needs to be multiple of 0x%08"PRIx32")\n", b->fblksz);
		return -1;
	}
	if ( size & (b->fblksz - 1) ) {
		fprintf(stderr,"Size misaligned (needs to be multiple of 0x%08"PRIx32")\n", b->fblksz);
		return -1;
	}

	a = b->start + offset;

	if ( !quiet ) {
		printf("ERASING Flash (Bank #%i)\n    from 0x%08"PRIx32" .. 0x%08"PRIx32"\nproceed y/[n]?",
			bank, a, (a+size-1));
		fflush(stdout);
		if ( 'Y' != getUc() ) {
			printf("ABORTED\n");
			return -1;
		}
	}
	f = 0;
	while ( size ) {
		/* work to do ? */
		for ( i = 0; i<b->fblksz; i++ ) {
			if ( (char)0xff != ((char*)a)[i] ) {
				b->ops->unlock_block(b, a);
				i = b->ops->erase_block(b, a);
				b->ops->lock_block(b, a);
				if (i) {
					wipe(f);
					return a;
				}
				break;
			}
		}

		if ( quiet < 2 ) {
			f = flip(f);
		}

		a    += b->fblksz;
		size -= b->fblksz;
	}
	b->ops->array_mode(b, a);
	if ( quiet < 2 ) {
		wipe(f);
	}
	return 0;
}

int
BSP_flashWrite(int bank, uint32_t offset, char *src, uint32_t n_bytes, int quiet)
{
int      rval;

	if ( !src ) {
		fprintf(stderr,"Error: Data source pointer is NULL\n");
		return -1;
	}

	if ( (rval = regionCheckAndErase(bank, offset, src, n_bytes, quiet)) )
		return rval;

	if ( ! quiet ) {
		printf("WRITING  "); fflush(stdout);
	}

	rval = BSP_flashWriteRegion(bank, offset, src, n_bytes, quiet);

	if ( !quiet && !rval ) {
		printf("DONE");
	}
	if ( !quiet )
		printf("\n");
	return rval;
}

static int
bfill(int fd, char *buf, int size)
{
int got, avail;
	for (avail = size; (got = read(fd, buf, avail)) > 0; avail-=got ) {
		buf  += got;
	}
	return size - avail;
}

int
BSP_flashWriteFile(int bank, uint32_t offset, char *fname, int quiet)
{
int             fd   = -1;
struct stat     sb;
uint32_t	    sz;
int			    rval = -1;
char		   *buf  = 0;
uint32_t	    got;
struct bankdesc *b;
unsigned        f    = 0;

	if ( ! (b = bankValidate(bank, 0)) )
		return -1;

	for ( sz = 0; -1 == fd ;  ) {
		if ( (fd = open(fname,O_RDONLY)) < 0 ) {
			perror("Opening file");
			return -1;
		}

		if ( sz )
			break;

		if ( fstat(fd, &sb) ) {
			fprintf(stderr,"Warning: fstat doesn't work; need to slurp file to determine size; please be patient.\n");
			FILE *f;
			close(fd); fd = -1;
			f = fopen(fname,"r");
			if ( !f ) {
				perror("fdopen");
				return -1;
			}
			while ( EOF != fgetc(f) )
				sz++;
			fclose(f);
			/* reopen */
		} else {
			sz = sb.st_size;
		}
		if ( 0 == sz ) {
			fprintf(stderr,"Error: zero file size (?)\n");
			goto bail;
		}
	}

	if ( !(buf = malloc(b->fblksz)) ) {
		perror("buffer allocation");
		goto bail;
	}

	/* See if we can erase the entire region */
	if ( (rval = regionCheckAndErase(bank, offset, buf, sz, quiet)) )
		goto bail;

	/* Proceed copying chunks */
	if ( quiet < 2 ) {
		printf("WRITING  "); fflush(stdout);
	}

	while ( (got = bfill(fd, buf, b->fblksz)) > 0 && sz ) {
		if ( (rval = BSP_flashWriteRegion(bank, offset, buf, got, 1)) ) {
			wipe(f);
			goto bail;
		}
		offset += got;
		sz     -= got;
		if ( quiet < 2 ) {
			f = flip(f);
		}
	}
	if ( got < 0 ) {
		perror("reading file");
		rval = offset;
		goto bail;
	}

	if ( quiet < 2 ) {
		wipe(f);
		printf("DONE");
	}
bail:
	if ( quiet < 2 ) {
		printf("\n");
	}
	if ( fd > -1 )
		close(fd);
	free(buf);
	return rval;
}

int
BSP_flashWriteEnable(int bank)
{
	return BSP_flashBspOps.flash_wp(bank,0);
}

int
BSP_flashWriteDisable(int bank)
{
	return BSP_flashBspOps.flash_wp(bank,1);
}

int
BSP_flashDumpInfo(FILE *f)
{
struct bankdesc *b;
int		        bank;

	if ( !f )
		f = stdout;

	/* use 'bankValidate()' with 'quiet' flag to suppress error message when
	 * we reach the end of the table.
	 */
	for ( bank = 0; BSP_flashBspOps.bankcheck(bank,1); bank++ ) {
		if ( (b=argcheck(bank,0,0,0)) ) {
			fprintf(f,"Flash Bank #%i; 0x%08"PRIx32" .. 0x%08"PRIx32" (%"PRId32" bytes)\n",
				bank, b->start, (b->start + b->size - 1), b->size);
			fprintf(f,"%i * %i-bit devices in parallel; block size 0x%"PRIx32"\n", FLASH_NDEVS(b), FLASH_WIDTH(b)*8, b->fblksz);
			BSP_flashCheckId(b, b->start, 0);
		}
	}

	return 0;
}

#else

int
main(int argc, char **argv)
{
uint32_t fla[1000];
uint32_t qqq[1000];
int i;
	for ( i=0; i<sizeof(qqq)/sizeof(qqq[0]); i++ )
		qqq[i] = 0xdada<<16 | i;
	BSP_flashWriteDataRaw(0, (uint32_t)fla, (char*)qqq, 32, 0);
}
#endif

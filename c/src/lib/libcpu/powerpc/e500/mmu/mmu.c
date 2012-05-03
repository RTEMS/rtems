/*
 * Routines to manipulate e500 TLBs; TLB0 (fixed 4k page size)
 * is not very useful so we mostly focus on TLB1 (variable page size).
 *
 * TLB0's 256 entries are 2-way set associative which means that
 *        only 2 entries for page index numbers with matching 7 LSBs
 *        are available.
 *
 *        E.g., look at EA = 0xAAAyy000. 0xAAAyy is the page index.
 *
 *        The least-significant 7 bits in 'yy' determine the 'way'
 *        in the TLB 0 array. At most two EAs with matching 'yy' bits
 *        (the 7 LSBs, that is) can be mapped with TLB0 since there
 *        are only two entries per 'way'.
 *
 *        Since this is a real-time OS we want to stay away from
 *        software TLB replacement.
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

/* 8450 MSR definitions; note that there are *substantial* differences
 * compared to classic powerpc; in particular, IS/DS are *different*
 * from IR/DR; the e500 MMU can not be switched off!
 *
 * Also: To disable/enable all external interrupts, CE and EE must both be
 *       controlled.
 */
#include <rtems.h>
#include <rtems/bspIo.h>
#include <inttypes.h>
#include <stdio.h>

#include "e500_mmu.h"

#define TLBIVAX_TLBSEL	(1<<(63-60))
#define TLBIVAX_INV_ALL	(1<<(63-61))

#define E500_TLB_ATTR_WIMGE(x)      ((x)&0x7f)				/* includes user bits */
#define E500_TLB_ATTR_WIMGE_GET(x)  ((x)&0x7f)
#define E500_TLB_ATTR_TS            (1<<7)
#define E500_TLB_ATTR_PERM(x)   	(((x)&0x3ff)<<8)
#define E500_TLB_ATTR_PERM_GET(x)   (((x)>>8)&0x3ff)
#define E500_TLB_ATTR_TID(x)        (((x)&0xfff)<<20)
#define E500_TLB_ATTR_TID_GET(x)    (((x)>>20)&0xfff)


#ifdef DEBUG
#define STATIC
#else
#define STATIC static
#endif

/* Factory to generate inline macros for accessing the MAS registers */
#define __RDWRMAS(mas,rmas)	\
	static inline uint32_t _read_MAS##mas(void)	                                 \
	{ uint32_t x; __asm__ volatile("mfspr %0, %1": "=r"(x):"i"(rmas)); return x; } \
	static inline void _write_MAS##mas(uint32_t x)                             \
	{             __asm__ volatile("mtspr %1, %0":: "r"(x),"i"(rmas)); }

__RDWRMAS(0,FSL_EIS_MAS0)
__RDWRMAS(1,FSL_EIS_MAS1)
__RDWRMAS(2,FSL_EIS_MAS2)
__RDWRMAS(3,FSL_EIS_MAS3)
__RDWRMAS(4,FSL_EIS_MAS4)
__RDWRMAS(6,FSL_EIS_MAS6)

#undef __RDWRMAS

static int initialized  = 0;

E500_tlb_va_cache_t rtems_e500_tlb_va_cache[16];

/* Since it is likely that these routines are used during
 * early initialization when stdio is not available yet
 * we provide a helper that resorts to 'printk()'
 */
static void
myprintf(FILE *f, char *fmt, ...)
{
va_list ap;
    va_start(ap, fmt);

    if (!f || !_impure_ptr->__sdidinit) {
        /*
		 * Might be called at an early stage when
         * stdio is not yet initialized.
         */
        vprintk(fmt,ap);
    } else {
        vfprintf(f,fmt,ap);
    }
    va_end(ap);
}


void
rtems_e500_dmptlbc(FILE *f)
{
int i;
	if ( !initialized ) {
		myprintf(stderr,"TLB cache not initialized\n");
		return;
	}
	for ( i=0; i<16; i++ ) {
		if ( !rtems_e500_tlb_va_cache[i].att.v )
			continue;
		myprintf(f,"#%2i: TID 0x%03x, TS %i, ea 0x%08x .. 0x%08x\n",
			i,
			rtems_e500_tlb_va_cache[i].va.va_tid,
			rtems_e500_tlb_va_cache[i].att.ts,
			rtems_e500_tlb_va_cache[i].va.va_epn<<12,
			(rtems_e500_tlb_va_cache[i].va.va_epn<<12) + (1024<<(2*rtems_e500_tlb_va_cache[i].att.sz))-1);
		myprintf(f,"PA 0x%08"PRIx32", PERM 0x%03x, WIMGE 0x%02x\n",
			rtems_e500_tlb_va_cache[i].rpn<<12,
			rtems_e500_tlb_va_cache[i].att.perm,
			rtems_e500_tlb_va_cache[i].att.wimge);
	}
}

#define E500_SELTLB_1	0x1000

static void seltlb(rtems_e500_tlb_idx key)
{
int idx = key & ~E500_SELTLB_1;

	if ( key & E500_SELTLB_1 ) {
		_write_MAS0( FSL_EIS_MAS0_TLBSEL | FSL_EIS_MAS0_ESEL(idx) );
	} else {
		_write_MAS0( (idx & 128) ? FSL_EIS_MAS0_ESEL(1) : FSL_EIS_MAS0_ESEL(0) );
		_write_MAS2( FSL_EIS_MAS2_EPN( idx & 127 ) );
	}
}

/*
 * Read a TLB entry from the hardware; if it is a TLB1 entry
 * then the current settings are stored in the
 * rtems_e500_tlb_va_cache[] structure.
 *
 * The routine can perform this operation quietly or
 * print information to a file.
 *
 *   'sel': which TLB array to use; TLB0 (4k) if zero,
 *          TLB1 (variable) if nonzero.
 *   'idx': which TLB entry to access.
 * 'quiet': perform operation silently (no info printed)
 *          if nonzero.
 *     'f': open FILE where to print information. May be
 *          NULL in which case 'stdout' is used.
 *
 * RETURNS:
 *       0: success; TLB entry is VALID
 *      +1: success but TLB entry is INVALID
 *     < 0: error (-1: invalid argument)
 */
int
rtems_e500_prtlb(rtems_e500_tlb_idx key, int quiet, FILE *f)
{
uint32_t               mas0, mas1, mas2, mas3;
rtems_interrupt_level  lvl;
E500_tlb_va_cache_t   *tlb;
E500_tlb_va_cache_t    buf;
int                    sel, idx;

	sel = (key &  E500_SELTLB_1) ? 1 : 0;
	idx = key & ~E500_SELTLB_1;

	if ( idx < 0 || idx > 255 || ( idx > 15 && sel ) )
		return -1;

	rtems_interrupt_disable(lvl);

	seltlb( key );

	asm volatile("tlbre");

	mas0 = _read_MAS0();
	mas1 = _read_MAS1();
	mas2 = _read_MAS2();
	mas3 = _read_MAS3();

	rtems_interrupt_enable(lvl);

	tlb = sel ? rtems_e500_tlb_va_cache + idx : &buf;

	if ( (tlb->att.v  = (FSL_EIS_MAS1_V & mas1) ? 1 : 0) ) {
		tlb->va.va_epn = FSL_EIS_MAS2_EPN_GET(mas2);
		tlb->rpn       = FSL_EIS_MAS3_RPN_GET(mas3);
		tlb->va.va_tid = FSL_EIS_MAS1_TID_GET(mas1);
		tlb->att.ts    = (FSL_EIS_MAS1_TS & mas1) ? 1 : 0;
		tlb->att.sz    = sel ? FSL_EIS_MAS1_TSIZE_GET(mas1) : 1 /* 4k size */;
		tlb->att.wimge = FSL_EIS_MAS2_ATTR_GET(mas2);
		tlb->att.perm  = FSL_EIS_MAS3_PERM_GET(mas3);
	}

	if ( tlb->att.v ) {
		if ( !quiet ) {
/*
			"TLB[1] Entry # 0 spans EA range     0x00000000 .. 0x00000000
			"Mapping:    VA     [TS 0/TID 0x00/EPN 0x00000] -> RPN 0x00000"
			"Size:       TSIZE  0x0 ( 4^ts KiB = 000000 KiB = 0x00000000 B)
			"Attributes: PERM 0x000 (ux/sx/uw/sw/ur/sr) WIMGE 0x00 IPROT 0"
*/
			myprintf(f,
				"TLB[%i] Entry # %d spans EA range     0x%08x .. 0x%08x\r\n",
				sel,
				idx,
				(tlb->va.va_epn << 12),
				(tlb->va.va_epn << 12) + (1024<<(2*tlb->att.sz)) - 1
			);

			myprintf(f,
				"Mapping:    VA     [TS %d/TID 0x%02x/EPN 0x%05x] -> RPN 0x%05"PRIx32"\r\n",
				tlb->att.ts, tlb->va.va_tid, tlb->va.va_epn, tlb->rpn
			);
			myprintf(f,
				"Size:       TSIZE  0x%x ( 4^ts KiB = %6d KiB = 0x%08x B)\r\n",
				tlb->att.sz, (1<<(2*tlb->att.sz)), (1024<<(2*tlb->att.sz))
			);
			myprintf(f,
				"Attributes: PERM 0x%03x (ux/sx/uw/sw/ur/sr) WIMGE 0x%02x IPROT %i\r\n",
				tlb->att.perm, tlb->att.wimge, (sel && (mas1 & FSL_EIS_MAS1_IPROT) ? 1 : 0)
			);
			myprintf(f,
				"EA range 0x%08x .. 0x%08x\r\n",
				(tlb->va.va_epn << 12),
				(tlb->va.va_epn << 12) + (1024<<(2*tlb->att.sz)) - 1
			);
		}
	} else {
		if ( !quiet ) {
			myprintf(f, "TLB[%i] Entry #%i <OFF> (size 0x%x = 0x%xb)\n", sel, idx, tlb->att.sz, (1024<<(2*tlb->att.sz)));
		}
		return 1;
	}
	return 0;
}

/* Initialize cache; verify that TLB0 is unused;
 *
 * RETURNS: zero on success, nonzero on error (TLB0
 *          seems to be in use); in this case the
 *          driver will refuse to change TLB1 entries
 *          (other than disabling them).
 */
int rtems_e500_initlb()
{
int i;
int rval = 0;
	for (i=0; i<16; i++)
		rtems_e500_prtlb(E500_SELTLB_1 | i, 1, 0);
	for (i=0; i<256; i++) {
		/* refuse to enable operations that change TLB entries
         * if anything in TLB[0] is valid (because we currently
		 * don't check against overlap with TLB[0] when we
		 * write a new entry).
		 */
		if ( rtems_e500_prtlb(E500_SELTLB_0 | i, 1, 0) <=0 ) {
			myprintf(stderr,"WARNING: 4k TLB #%i seems to be valid; UNSUPPORTED configuration\n", i);
			rval = -1;
		}
	}
	if ( !rval )
		initialized = 1;
	return rval;
}

/*
 * Write TLB1 entry (can also be used to disable an entry).
 *
 * The routine checks against the cached data in
 * rtems_e500_tlb_va[] to prevent the user from generating
 * overlapping entries.
 *
 *   'idx': TLB 1 entry # to manipulate
 *    'ea': Effective address (must be page aligned)
 *    'pa': Physical  address (must be page aligned)
 *    'sz': Page size selector; page size is
 *          1024 * 2^(2*sz) bytes.
 *          'sz' may also be one of the following:
 *          - page size in bytes ( >= 1024 ); the selector
 *            value is then computed by this routine.
 *            However, 'sz' must be a valid page size
 *            or -1 will be returned.
 *          - a value < 0 to invalidate/disable the
 *            TLB entry.
 *  'attr': Page attributes; ORed combination of WIMGE,
 *          PERMissions, TID and TS. Use ATTR_xxx macros
 *
 * RETURNS: 0 on success, nonzero on error:
 *
 *         >0: requested mapping would overlap with
 *             existing mapping in other entry. Return
 *             value gives conflicting entry + 1; i.e.,
 *             if a value of 4 is returned then the request
 *             conflicts with existing mapping in entry 3.
 *         -1: invalid argument
 *         -3: driver not initialized (or initialization
 *             failed because TLB0 is in use).
 *         <0: other error
 *
 */
#define E500_TLB_ATTR_WIMGE(x)      ((x)&0x7f)				/* includes user bits */
#define E500_TLB_ATTR_WIMGE_GET(x)  ((x)&0x7f)
#define E500_TLB_ATTR_TS            (1<<7)
#define E500_TLB_ATTR_PERM(x)   	   (((x)&0x3ff)<<8)
#define E500_TLB_ATTR_PERM_GET(x)   (((x)>>8)&0x3ff)
#define E500_TLB_ATTR_TID(x)        (((x)&0xfff)<<20)
#define E500_TLB_ATTR_TID_GET(x)    (((x)>>20)&0xfff)

int
rtems_e500_wrtlb(int idx, uint32_t ea, uint32_t pa, int sz, uint32_t attr)
{
uint32_t              mas1, mas2, mas3, mas4;
uint32_t              tid, msk;
int                   lkup;
rtems_interrupt_level lvl;

	if ( sz >= 1024 ) {
		/* Assume they literally specify a size */
		msk = sz;
		sz  = 0;
		while ( msk != (1024<<(2*sz)) ) {
			if ( ++sz > 15 ) {
				return -1;
			}
		}
		/* OK, acceptable */
	}

	msk = sz > 0 ? (1024<<(2*sz)) - 1 : 0;

	if ( !initialized && sz > 0 ) {
		myprintf(stderr,"TLB driver not initialized; refuse to enable any entry\n");
		return -3;
	}

	if ( (ea & msk) || (pa & msk) ) {
		myprintf(stderr,"Misaligned ea or pa\n");
		return -1;
	}

	if ( idx < 0 || idx > 15 )
		return -1;

	if ( sz > 15 ) {
		/* but e500v1 doesn't support all 16 sizes!! */
		/* FIXME: we should inquire about this CPU's
		 *        capabilities...
		 */
		return -1;
	}

	tid = E500_TLB_ATTR_TID_GET(attr);

	mas1 = (attr & E500_TLB_ATTR_TS) ? FSL_EIS_MAS1_TS : 0;

	if ( sz >=0 ) {
		lkup = rtems_e500_matchtlb(ea, tid, mas1, sz);

		if ( lkup < -1 ) {
			/* some error */
			return lkup;
		}

		if ( lkup >= 0 && lkup != idx ) {
			myprintf(stderr,"TLB[1] #%i overlaps with requested mapping\n", lkup);
			rtems_e500_prtlb( E500_SELTLB_1 | lkup, 0, stderr);
			return lkup+1;
		}
	}

	/* OK to proceed */
	mas1 |= FSL_EIS_MAS1_IPROT | FSL_EIS_MAS1_TID(tid);

	if ( sz >= 0 )
		mas1 |= FSL_EIS_MAS1_V | FSL_EIS_MAS1_TSIZE(sz);

	mas2 = FSL_EIS_MAS2_EPN( ea>>12 ) | E500_TLB_ATTR_WIMGE(attr);
	mas3 = FSL_EIS_MAS3_RPN( pa>>12 ) | E500_TLB_ATTR_PERM_GET(attr);
	/* mas4 is not really relevant; we don't use TLB replacement */
	mas4 = FSL_EIS_MAS4_TLBSELD | FSL_EIS_MAS4_TIDSELD(0) | FSL_EIS_MAS4_TSIZED(9) | FSL_EIS_MAS4_ID | FSL_EIS_MAS4_GD;

	rtems_interrupt_disable(lvl);

	seltlb(idx | E500_SELTLB_1);

	_write_MAS1(mas1);
	_write_MAS2(mas2);
	_write_MAS3(mas3);
	_write_MAS4(mas4);

	asm volatile(
		"	sync		\n"
		"	isync		\n"
		"	tlbwe       \n"
		"	sync        \n"
		"	isync		\n"
	);

	rtems_interrupt_enable(lvl);

	/* update cache */
	rtems_e500_prtlb( E500_SELTLB_1 | idx, 1, 0);

	return 0;
}

/*
 * Check if a ts/tid/ea/sz mapping overlaps
 * with an existing entry.
 *
 * ASSUMPTION: all TLB0 (fixed 4k pages) are invalid and always unused.
 *
 * NOTE: 'sz' is the 'logarithmic' size selector; the page size
 *       is 1024*2^(2*sz).
 *
 * RETURNS:
 *     >= 0: index of TLB1 entry that already provides a mapping
 *           which overlaps within the ea range.
 *       -1: SUCCESS (no conflicting entry found)
 *     <=-2: ERROR (invalid input)
 */
int rtems_e500_matchtlb(uint32_t ea, uint32_t tid, int ts, int sz)
{
int                  i;
uint32_t             m,a;
E500_tlb_va_cache_t *tlb;

	if ( sz < 0 || sz > 15 )
		return -4;

	sz = (1024<<(2*sz));

	if ( !initialized ) {
		/* cache not initialized */
		return -3;
	}

	if ( ea & (sz-1) ) {
		/* misaligned ea */
		return -2;
	}

	if ( ts )
		ts = 1;

	for ( i=0, tlb=rtems_e500_tlb_va_cache; i<16; i++, tlb++ ) {
		if ( ! tlb->att.v )
			continue;
		if ( tlb->att.ts != ts )
			continue;
		if ( tlb->va.va_tid && tlb->va.va_tid != tid )
			continue;
		/* TID and TS match a valid entry */
		m  = (1024<<(2*tlb->att.sz)) - 1;
		/* calculate starting address of this entry */
		a  = tlb->va.va_epn<<12;
		if ( ea <= a + m && ea + sz -1 >= a ) {
			/* overlap */
			return i;
		}
	}
	return -1;
}

/* Find TLB index that maps 'ea/as' combination
 *
 * RETURNS: index 'key'; i.e., the index number plus
 *          a bit (E500_SELTLB_1) which indicates whether
 *          the mapping was found in TLB0 (4k fixed page
 *          size) or in TLB1 (variable page size).
 *
 *          On error (no mapping) -1 is returned.
 */
rtems_e500_tlb_idx
rtems_e500_ftlb(uint32_t ea, int as)
{
uint32_t              pid, mas0, mas1;
int                   i, rval = -1;
rtems_interrupt_level lvl;

	rtems_interrupt_disable(lvl);

	for ( i=0; i<3; i++ ) {
		switch (i) {
			case 0:	asm volatile("mfspr %0, %1":"=r"(pid):"i"(FSL_EIS_PID0)); break;
			case 1:	asm volatile("mfspr %0, %1":"=r"(pid):"i"(FSL_EIS_PID1)); break;
			case 2:	asm volatile("mfspr %0, %1":"=r"(pid):"i"(FSL_EIS_PID2)); break;
			default:
				goto bail;
		}

		_write_MAS6( FSL_EIS_MAS6_SPID0(pid) | (as ? FSL_EIS_MAS6_SAS : 0 ) );

		asm volatile("tlbsx 0, %0"::"r"(ea));

		mas1 = _read_MAS1();

		if ( (FSL_EIS_MAS1_V & mas1) ) {
			mas0 = _read_MAS0();
			if ( FSL_EIS_MAS0_TLBSEL & mas0 ) {
				/* TLB1 */
				rval = FSL_EIS_MAS0_ESEL_GET(mas0) | E500_SELTLB_1;
			} else {
				rval = (ea >> (63-51)) | (( FSL_EIS_MAS0_NV & mas0 ) ? 180 : 0 ) ;
			}
			break;
		}
	}

bail:
	rtems_interrupt_enable(lvl);
	return rval;
}

/* Mark TLB entry as invalid ('disabled'). Unlike
 * rtems_e500_wrtlb() with a negative size argument
 * this routine also can disable TLB0 entries.
 *
 * 'key': TLB entry (index) ORed with selector bit
 *        (0 for TLB0, E500_SELTLB_1 for TLB1).
 *
 * RETURNS: zero on success, nonzero on error (TLB
 *          unchanged).
 *
 * NOTE:  If a TLB1 entry is disabled the associated
 *        entry in rtems_e500_va_cache[] is also
 *        marked as disabled.
 */
int
rtems_e500_clrtlb(rtems_e500_tlb_idx key)
{
rtems_e500_tlb_idx    k0;
rtems_interrupt_level lvl;

	/* minimal guard against bad key */
	if ( key < 0 )
		return -1;

	if ( (key & E500_SELTLB_1) ) {
		if ( (key & ~E500_SELTLB_1) > 15 ) {
			myprintf(stderr,"Invalid TLB index; TLB1 index must be < 16\n");
			return -1;
		}
	} else if ( key > 255 ) {
			myprintf(stderr,"Invalid TLB index; TLB0 index must be < 256\n");
			return -1;
	}

	/* Must not invalidate page 0 which holds vectors, text etc...  */
	k0 = rtems_e500_ftlb(0, 0);
	if ( -1 == k0 ) {
		myprintf(stderr,"tlbivax; something's fishy - I don't find mapping for addr. 0\n");
		return -1;
	}

	/* NOTE: we assume PID is ignored, and AS is 0 */
	if ( k0 == key ) {
		myprintf(stderr,"Refuse to invalidate page holding addr 0 (always needed)\n");
		return -1;
	}

	rtems_interrupt_disable(lvl);

	seltlb(key);

	asm volatile("tlbre");

	/* read old entries */
	_write_MAS1( _read_MAS1() & ~FSL_EIS_MAS1_V );

	asm volatile(
		"	sync		\n"
		"	isync		\n"
		"	tlbwe       \n"
		"	sync        \n"
		"	isync		\n"
	);

	/* update cache */
	if ( E500_SELTLB_1 & key )
		rtems_e500_tlb_va_cache[ (~E500_SELTLB_1 & key) ].att.v = 0;

	rtems_interrupt_enable(lvl);

	return 0;
}

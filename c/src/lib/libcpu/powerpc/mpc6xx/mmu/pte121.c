/* $Id$ */

/* Trivial page table setup for RTEMS
 * Purpose: allow write protection of text/ro-data
 *
 * Author: Till Straumann <strauman@slac.stanford.edu>, 4/2002
 */

/* Chose debugging options */
#undef  DEBUG_MAIN	/* create a standalone (host) program for basic testing */
#undef  DEBUG		/* target debugging and consistency checking */
#undef  DEBUG_EXC	/* add exception handler which reenables BAT0 and recovers from a page fault */

#ifdef	DEBUG_MAIN
#undef	DEBUG		/* must not use these together with DEBUG_MAIN */
#undef	DEBUG_EXC
#endif

/***************************** INCLUDE HEADERS ****************************/

#ifndef DEBUG_MAIN
#include <rtems.h>
#include <rtems/bspIo.h>
#include <libcpu/cpuIdent.h>
#ifdef	DEBUG_EXC
#include <bsp.h>
#include <bsp/vectors.h>
#include <libcpu/raw_exception.h>
#endif
#endif

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "pte121.h"

/************************** CONSTANT DEFINITIONS **************************/

/* Base 2 logs of some sizes */

#ifndef DEBUG_MAIN

#define	LD_PHYS_SIZE	32		/* physical address space */
#define LD_PG_SIZE		12		/* page size */
#define LD_PTEG_SIZE	6		/* PTEG size */
#define LD_PTE_SIZE		3		/* PTE size  */
#define LD_SEG_SIZE		28		/* segment size */
#define LD_MIN_PT_SIZE	16		/* minimal size of a page table */
#define LD_HASH_SIZE	19		/* lengh of a hash */

#else /* DEBUG_MAIN */

/* Reduced 'fantasy' sizes for testing */
#define	LD_PHYS_SIZE	32		/* physical address space */
#define LD_PG_SIZE		6		/* page size */
#define LD_PTEG_SIZE	5		/* PTEG size */
#define LD_PTE_SIZE		3		/* PTE size  */
#define LD_SEG_SIZE		28		/* segment size */
#define LD_MIN_PT_SIZE	7		/* minimal size of a page table */
#define LD_HASH_SIZE	19		/* lengh of a hash */

#endif /* DEBUG_MAIN */

/* Derived sizes */

/* Size of a page index */
#define LD_PI_SIZE		((LD_SEG_SIZE) - (LD_PG_SIZE)) 

/* Number of PTEs in a PTEG */
#define PTE_PER_PTEG	(1<<((LD_PTEG_SIZE)-(LD_PTE_SIZE)))

/* Segment register bits */
#define KEY_SUP			(1<<30)	/* supervisor mode key */
#define KEY_USR			(1<<29)	/* user mode key */

/* The range of effective addresses to scan with 'tlbie'
 * instructions in order to flush all TLBs.
 * On the 750 and 7400, there are 128 two way I and D TLBs,
 * indexed by EA[14:19]. Hence calling
 *	  tlbie rx
 * where rx scans 0x00000, 0x01000, 0x02000, ... 0x3f000
 * is sufficient to do the job
 */
#define NUM_TLB_PER_WAY 64 /* 750 and 7400 have 128 two way TLBs */
#define FLUSH_EA_RANGE	(NUM_TLB_PER_WAY<<LD_PG_SIZE)

/*************************** MACRO DEFINITIONS ****************************/

/* Macros to split a (32bit) 'effective' address into
 * VSID (virtual segment id) and PI (page index)
 * using a 1:1 mapping of 'effective' to 'virtual'
 * addresses.
 *
 * For 32bit addresses this looks like follows
 * (each 'x' or '0' stands for a 'nibble' [4bits]):
 *
 *         32bit effective address (EA)
 *
 *              x x x x x x x x
 *               |       |
 *    0 0 0 0 0 x|x x x x|x x x
 *       VSID    |  PI   |  PO (page offset)
 *               |       |
 */
/* 1:1 VSID of an EA  */
#define VSID121(ea) (((ea)>>LD_SEG_SIZE) & ((1<<(LD_PHYS_SIZE-LD_SEG_SIZE))-1))
/* page index of an EA */
#define PI121(ea)	(((ea)>>LD_PG_SIZE) & ((1<<LD_PI_SIZE)-1))


/* Primary and secondary PTE hash functions */

/* Compute the primary hash from a VSID and a PI */
#define PTE_HASH1(vsid, pi) (((vsid)^(pi))&((1<<LD_HASH_SIZE)-1))

/* Compute the secondary hash from a primary hash */
#define PTE_HASH2(hash1) ((~(hash1))&((1<<LD_HASH_SIZE)-1))

/* Extract the abbreviated page index (which is the
 * part of the PI which does not go into the hash
 * under all circumstances [10 bits to -> 6bit API])
 */
#define API(pi)	((pi)>>((LD_MIN_PT_SIZE)-(LD_PTEG_SIZE)))


/* Horrible Macros */
#ifdef __rtems__
/* must not use printf until multitasking is up */
typedef void (*PrintF)(char *,...);
static PrintF whatPrintf(void)
{
		return _Thread_Executing ?
				(PrintF)printf :
				printk;
}

#define PRINTF(args...) ((void)(whatPrintf())(args))
#else
#define PRINTF(args...) printf(args)
#endif

#ifdef DEBUG
unsigned long
triv121PgTblConsistency(Triv121PgTbl pt, int pass, int expect);

static int consistencyPass=0;
#define CONSCHECK(expect) triv121PgTblConsistency(&pgTbl,consistencyPass++,(expect))
#else
#define CONSCHECK(expect) do {} while (0)
#endif

/**************************** TYPE DEFINITIONS ****************************/

/* A PTE entry */
typedef struct PTERec_ {
		unsigned long v:1,    vsid:24, h:1, api: 6;
		unsigned long rpn:20, pad: 3, r:1, c:1, wimg:4, marked:1, pp:2;
} PTERec, *PTE;

/* internal description of a trivial page table */
typedef struct Triv121PgTblRec_ {
		PTE				base;
		unsigned long	size;
		int				active;
} Triv121PgTblRec;


/************************** FORWARD DECLARATIONS *************************/

#ifdef DEBUG_EXC
static void
myhdl(BSP_Exception_frame* excPtr);
#endif

#if defined(DEBUG_MAIN) || defined(DEBUG)
static void
dumpPte(PTE pte);
#endif

#ifdef DEBUG
static void
dumpPteg(unsigned long vsid, unsigned long pi, unsigned long hash);
unsigned long
triv121IsRangeMapped(unsigned long start, unsigned long end);
#endif

/**************************** STATIC VARIABLES ****************************/

/* dont malloc - we might have to use this before
 * we have malloc or even RTEMS workspace available
 */
static Triv121PgTblRec pgTbl={0};

#ifdef DEBUG_EXC
static void *ohdl;			/* keep a pointer to the original handler */
#endif

/*********************** INLINES & PRIVATE ROUTINES ***********************/

/* compute the page table entry group (PTEG) of a hash */
static inline PTE
ptegOf(Triv121PgTbl pt, unsigned long hash)
{
	hash &= ((1<<LD_HASH_SIZE)-1);
	return (PTE)(((unsigned long)pt->base) | ((hash<<LD_PTEG_SIZE) & (pt->size-1)));
}

/* see if a vsid/pi combination is already mapped
 *
 * RETURNS: PTE of mapping / NULL if none exists
 *
 * NOTE: a vsid<0 is legal and will tell this
 *       routine that 'pi' is actually an EA to
 *       be split into vsid and pi...
 */
static PTE
alreadyMapped(Triv121PgTbl pt, long vsid, unsigned long pi)
{
int				i;
unsigned long	hash,api;
PTE				pte;

	if (!pt->size)
		return 0;

	if (vsid<0) {
		vsid=VSID121(pi);
		pi=PI121(pi);
	}

	hash = PTE_HASH1(vsid,pi);
	api=API(pi);
	for (i=0, pte=ptegOf(pt,hash); i<PTE_PER_PTEG; i++,pte++)
		if (pte->v && pte->vsid==vsid && pte->api==api && 0==pte->h)
			return pte;
	/* try the secondary hash table */
	hash = PTE_HASH2(hash);
	for (i=0, pte=ptegOf(pt,hash); i<PTE_PER_PTEG; i++,pte++)
		if (pte->v && pte->vsid==vsid && pte->api==api && 1==pte->h)
			return pte;
	return 0;
}

/* find the first available slot for  vsid/pi
 *
 * NOTE: it is NOT legal to pass a vsid<0 / EA combination.
 *
 * RETURNS free slot with the 'marked' field set. The 'h'
 *         field is set to 0 or one, depending on whether
 *         the slot was allocated by using the primary or
 *         the secondary hash, respectively.
 */
static PTE
slotFor(Triv121PgTbl pt, unsigned long vsid, unsigned long pi)
{
int				i;
unsigned long	hash,api;
PTE				pte;

	/* primary hash */
	hash = PTE_HASH1(vsid,pi);
	api=API(pi);
	/* linear search thru all buckets for this hash */
	for (i=0, pte=ptegOf(pt,hash); i<PTE_PER_PTEG; i++,pte++) {
		if (!pte->v && !pte->marked) {
			/* found a free PTE; mark it as potentially used and return */
			pte->h=0;	/* found by the primary hash fn */
			pte->marked=1;
			return pte;
		}
	}

#ifdef DEBUG
	/* Strange: if the hash table was allocated big enough,
	 *          this should not happen (when using a 1:1 mapping)
	 *          Give them some information...
	 */
	PRINTF("## First hash bucket full - ");
	dumpPteg(vsid,pi,hash);
#endif

	hash = PTE_HASH2(hash);
#ifdef DEBUG
	PRINTF("   Secondary pteg is 0x%08x\n", (unsigned)ptegOf(pt,hash));
#endif
	for (i=0, pte=ptegOf(pt,hash); i<PTE_PER_PTEG; i++,pte++) {
		if (!pte->v && !pte->marked) {
			/* mark this pte as potentially used */
			pte->marked=1;
			pte->h=1;
			return pte;
		}
	}
#ifdef DEBUG
	/* Even more strange - most likely, something is REALLY messed up */
	PRINTF("## Second hash bucket full - ");
	dumpPteg(vsid,pi,hash);
#endif
	return 0;
}

/* unmark all entries */
static void
unmarkAll(Triv121PgTbl pt)
{
unsigned long	n=pt->size / sizeof(PTERec);
unsigned long	i;
PTE				pte;
	for (i=0,pte=pt->base; i<n; i++,pte++)
		pte->marked=0;

}

/* calculate the minimal size of a page/hash table
 * to map a range of 'size' bytes in EA space.
 *
 * RETURNS: size in 'number of bits', i.e. the
 *          integer part of LOGbase2(minsize)
 *          is returned.
 * NOTE:	G3/G4 machines need at least 16 bits
 *          (64k).
 */
unsigned long
triv121PgTblLdMinSize(unsigned long size)
{
unsigned long i;
	/* round 'size' up to the next page boundary */
	size += (1<<LD_PG_SIZE)-1;
	size &= ~((1<<LD_PG_SIZE)-1);
	/* divide by number of PTEs  and multiply
	 * by the size of a PTE.
	 */
	size >>= LD_PG_SIZE - LD_PTE_SIZE;
	/* find the next power of 2 >= size */
	for (i=0; i<LD_PHYS_SIZE; i++) {
		if ((1<<i) >= size)
			break;
	}
	/* pop up to the allowed minimum, if necessary */
	if (i<LD_MIN_PT_SIZE)
			i=LD_MIN_PT_SIZE;
	return i;
}

/* initialize a trivial page table of 2^ldSize bytes
 * at 'base' in memory.
 *
 * RETURNS:	OPAQUE HANDLE (not the hash table address)
 *          or NULL on failure.
 */
Triv121PgTbl
triv121PgTblInit(unsigned long base, unsigned ldSize)
{
	if (pgTbl.size) {
		/* already initialized */
		return 0;
	}

	if (ldSize < LD_MIN_PT_SIZE)
		return 0; /* too small */

	if (base & ((1<<ldSize)-1))
		return 0; /* misaligned */

	/* This was tested on 604r, 750 and 7400.
	 * On other CPUs, verify that the TLB invalidation works
	 * for a new CPU variant and that it has hardware PTE lookup/
	 * TLB replacement before adding it to this list.
	 *
	 * NOTE: The 603 features no hardware PTE lookup - and 
	 *       hence the page tables should NOT be used.
	 *		 Although lookup could be implemented in
	 *		 software this is probably not desirable
	 *		 as it could have an impact on hard realtime
	 *		 performance, screwing deterministic latency!
	 *		 (Could still be useful for debugging, though)
	 */
	if	(	PPC_604		!=current_ppc_cpu &&
  			PPC_604e	!=current_ppc_cpu &&
  			PPC_604r	!=current_ppc_cpu &&
  			PPC_750		!=current_ppc_cpu &&
  			PPC_7400	!=current_ppc_cpu )
		return 0;	/* unsupported by this CPU */

	pgTbl.base=(PTE)base;
	pgTbl.size=1<<ldSize;
	/* clear all page table entries */
	memset(pgTbl.base, 0, pgTbl.size);

	CONSCHECK(0);

	/* map the page table itself 'm' and 'readonly' */
	if (triv121PgTblMap(&pgTbl,
						TRIV121_121_VSID,
						base,
						(pgTbl.size >> LD_PG_SIZE),
						TRIV121_ATTR_M,
						TRIV121_PP_RO_PAGE) >= 0)
		return 0;

	CONSCHECK((pgTbl.size>>LD_PG_SIZE));

	return &pgTbl;
}

/* return the handle of the (one and only) page table
 * or NULL if none has been initialized yet.
 */
Triv121PgTbl
triv121PgTblGet(void)
{
	return pgTbl.size ? &pgTbl : 0;
}

/* NOTE: this routine returns -1 on success;
 *       on failure, the page table index for
 *       which no PTE could be allocated is returned
 *
 * (Consult header about argument/return value
 * description)
 */
long
triv121PgTblMap(
				Triv121PgTbl	pt,
				long			vsid,
				unsigned long	start,
				unsigned long	numPages,
				unsigned		attributes,
				unsigned		protection
				)
{
int				i,pass;
unsigned long	pi;
PTE				pte;

	/* already activated - no change allowed */
	if (pt->active)
			return -1;

	if (vsid < 0) {
			/* use 1:1 mapping */
			vsid = VSID121(start);
	}

#ifdef DEBUG
	PRINTF("Mapping %i (0x%x) pages at 0x%08x for VSID 0x%08x\n",
		(unsigned)numPages, (unsigned)numPages,
		(unsigned)start, (unsigned)vsid);
#endif

	/* map in two passes. During the first pass, we try
	 * to claim entries as needed. The 'slotFor()' routine
	 * will 'mark' the claimed entries without 'valid'ating
	 * them.
	 * If the mapping fails, all claimed entries are unmarked
	 * and we return the PI for which allocation failed.
	 *
	 * Once we know that the allocation would succeed, we
	 * do a second pass; during the second pass, the PTE
	 * is actually written.
	 *
	 */
	for (pass=0; pass<2; pass++) {
		/* check if we would succeed during the first pass */
		for (i=0, pi=PI121(start); i<numPages; i++,pi++) {
			/* leave alone existing mappings for this EA */
			if (!alreadyMapped(pt, vsid, pi)) {
				if (!(pte=slotFor(pt, vsid, pi))) {
					/* no free slot found for page index 'pi' */
					unmarkAll(pt);
					return pi;
				} else {
					/* have a free slot; marked by slotFor() */
					if (pass) {
						/* second pass; do the real work */
						pte->vsid=vsid;
						/* H was set by slotFor() */
						pte->api =API(pi);
						/* set up 1:1 mapping */
						pte->rpn =((((unsigned long)vsid)&((1<<(LD_PHYS_SIZE-LD_SEG_SIZE))-1))<<LD_PI_SIZE) | pi;
						pte->wimg=attributes & 0xf;
						pte->pp=protection&0x3;
						/* mark it valid */
						pte->v=1;
						pte->marked=0;
#ifdef DEBUG
						/* add paranoia */
						assert(alreadyMapped(pt, vsid, pi) == pte);
#endif
					}
				}
			}
		}
		unmarkAll(pt);
	}
#ifdef DEBUG
	{
	unsigned long failedat;
	CONSCHECK(-1);
	/* double check that the requested range is mapped */
	failedat=triv121IsRangeMapped(start, start + (1<<LD_PG_SIZE)*numPages);
	if (0x0C0C != failedat) {
		PRINTF("triv121 mapping failed at 0x%08x\n",(unsigned)failedat);
		return PI121(failedat);
	}
	}
#endif
	return TRIV121_MAP_SUCCESS; /* -1 !! */
}

unsigned long
triv121PgTblSDR1(Triv121PgTbl pt)
{
	return (((unsigned long)pt->base) & ~(LD_MIN_PT_SIZE-1)) |
		   ( ((pt->size-1) >> LD_MIN_PT_SIZE) &
			 ((1<<(LD_HASH_SIZE-(LD_MIN_PT_SIZE-LD_PTEG_SIZE)))-1)
		   );
}

void
triv121PgTblActivate(Triv121PgTbl pt)
{
#ifndef DEBUG_MAIN
unsigned long sdr1=triv121PgTblSDR1(pt);
#endif
	pt->active=1;

#ifndef DEBUG_MAIN
#ifdef DEBUG_EXC
	/* install our exception handler */
	ohdl=globalExceptHdl;
	globalExceptHdl=myhdl;
	__asm__ __volatile__ ("sync");
#endif

	/* This section of assembly code takes care of the
	 * following:
	 * - get MSR and switch interrupts + MMU off
	 *
	 * - load up the segment registers with a
	 *   1:1 effective <-> virtual mapping;
	 *   give user & supervisor keys
	 *
	 * - flush all TLBs;
	 *   NOTE: the TLB flushing code is probably
	 *         CPU dependent!
	 *
	 * - setup SDR1
	 *
	 * - restore original MSR
	 */
	__asm__ __volatile(
		"	mtctr	%0\n"
		/* Get MSR and switch interrupts off - just in case.
		 * Also switch the MMU off; the book
		 * says that SDR1 must not be changed with either
		 * MSR_IR or MSR_DR set. I would guess that it could
		 * be safe as long as the IBAT & DBAT mappings override
		 * the page table...
		 */
		"	mfmsr	%0\n"
		"	andc	%6, %0, %6\n"
		"	mtmsr	%6\n"
		"	isync	\n"
		/* set up the segment registers */
		"	li		%6, 0\n"
		"1:	mtsrin	%1, %6\n"
		"	addis	%6, %6, 0x1000\n"	/* address next SR */
		"	addi	%1, %1, 1\n"		/* increment VSID  */
		"	bdnz	1b\n"
		/* Now flush all TLBs, starting with the topmost index */
		"	lis		%6, %2@h\n"	
		"2:	addic.	%6, %6, -%3\n"		/* address the next one (decrementing) */
		"	tlbie	%6\n"				/* invalidate & repeat */
		"	bgt		2b\n"
		"	tlbsync\n"
		"	sync\n"
		/* set up SDR1 */
		"   mtspr	%4, %5\n"
		/* restore original MSR  */
		"	mtmsr	%0\n"
		"	isync	\n"
		::"r"(16), "b"(KEY_USR | KEY_SUP),
		  "i"(FLUSH_EA_RANGE), "i"(1<<LD_PG_SIZE),
		  "i"(SDR1), "r"(sdr1),
		  "b"(MSR_EE | MSR_IR | MSR_DR)
		: "ctr","cc");

	/* At this point, BAT0 is probably still active; it's the
	 * caller's job to deactivate it...
	 */
#endif
}

/**************************  DEBUGGING ROUTINES  *************************/

/* Exception handler to catch page faults */
#ifdef DEBUG_EXC

#define BAT_VALID_BOTH	3	/* allow user + super access */

static void
myhdl(BSP_Exception_frame* excPtr)
{
if (3==excPtr->_EXC_number) {
	unsigned long dsisr;

	/* reactivate DBAT0 and read DSISR */
	__asm__ __volatile__(
			"mfspr %0, %1\n"
			"ori	%0,%0,3\n"
			"mtspr %1, %0\n"
			"sync\n"
			"mfspr %0, %2\n"
			:"=r"(dsisr)
			:"i"(DBAT0U),"i"(DSISR),"i"(BAT_VALID_BOTH)
	);

	printk("Data Access Exception (DSI) # 3\n");
	printk("Reactivated DBAT0 mapping\n");


	printk("DSISR 0x%08x\n",dsisr);

	printk("revectoring to prevent default handler panic().\n");
	printk("NOTE: exception number %i below is BOGUS\n",
			ASM_DEC_VECTOR);
	/* make this exception 'recoverable' for
	 * the default handler by faking a decrementer
	 * exception.
	 * Note that the default handler's message will be
	 * wrong about the exception number.
	 */
	excPtr->_EXC_number = ASM_DEC_VECTOR;
}
/* now call the original handler */
((void(*)())ohdl)(excPtr);
}
#endif


#ifdef DEBUG

/* test the consistency of the page table
 *
 * 'pass' is merely a number which will be printed
 * by this routine, so the caller may give some
 * context information.
 *
 * 'expected' is the number of valid (plus 'marked')
 * entries the caller believes the page table should
 * have. This routine complains if its count differs.
 *
 * It basically verifies that the topmost 20bits
 * of all VSIDs as well as the unused bits are all
 * zero. Then it counts all valid and all 'marked'
 * entries, adding them up and comparing them to the
 * 'expected' number of occupied slots.
 *
 * RETURNS: total number of valid plus 'marked' slots.
 */
unsigned long
triv121PgTblConsistency(Triv121PgTbl pt, int pass, int expected)
{
PTE pte;
int i;
unsigned v,m;
int warn=0;
static int maxw=20;	/* mute after detecting this many errors */

	PRINTF("Checking page table at 0x%08x (size %i==0x%x)\n",
			(unsigned)pt->base, (unsigned)pt->size, (unsigned)pt->size);

	if (!pt->base || !pt->size) {
		PRINTF("Uninitialized Page Table!\n");
		return 0;
	}

	v=m=0;
#if 1
	/* 10/9/2002: I had machine checks crashing after this loop
	 *            terminated. Maybe caused by speculative loads
	 *            from beyond the valid memory area (since the
	 *            page hash table sits at the top of physical
	 *            memory).
	 *            Very bizarre - the other loops in this file
	 *            seem to be fine. Maybe there is a compiler bug??
	 *            For the moment, I let the loop run backwards...
	 *
	 * 			  Also see the comment a couple of lines down.
	 */
 	for (i=pt->size/sizeof(PTERec)-1, pte=pt->base + i; i>=0; i--,pte--)
#else
	for (i=0, pte=pt->base; i<pt->size/sizeof(PTERec); i++,pte++)
#endif
	{
		int				err=0;
		char			buf[500];
		unsigned long	*lp=(unsigned long*)pte;
#if 0
		/* If I put this bogus while statement here (the body is
		 * never reached), the original loop works OK
		 */
		while (pte >= pt->base + pt->size/sizeof(PTERec))
				/* never reached */;
#endif

		if ( (*lp & (0xfffff0<<7)) || *(lp+1) & 0xe00 || (pte->v && pte->marked)) {
			/* check for vsid (without segment bits) == 0, unused bits == 0, valid && marked */
			sprintf(buf,"invalid VSID , unused bits or v && m");
			err=1;
		} else {
			if (pte->v) v++;
			if (pte->marked) m++;
		}
		if (err && maxw) {
			PRINTF("Pass %i -- strange PTE at 0x%08x found for page index %i == 0x%08x:\n",
				pass,(unsigned)pte,i,i);
			PRINTF("Reason: %s\n",buf);
			dumpPte(pte);
			warn++;
			maxw--;
		}
	}
	if (warn) {
		PRINTF("%i errors found; currently %i entries marked, %i are valid\n",
			warn, m, v);
	}
	v+=m;
	if (maxw && expected>=0 && expected != v) {
		/* number of occupied slots not what they expected */
		PRINTF("Wrong # of occupied slots detected during pass");
	    PRINTF("%i; should be %i (0x%x) is %i (0x%x)\n",
				pass, expected, (unsigned)expected, v, (unsigned)v);
		maxw--;
	}
	return v;
}

/* Find the PTE for a EA and print its contents
 * RETURNS: pte for EA or NULL if no entry was found.
 */
PTE
triv121DumpPte(unsigned long ea)
{
PTE pte;

	pte=alreadyMapped(&pgTbl,TRIV121_121_VSID,ea);

	if (pte)
		dumpPte(pte);
	return pte;
}

/* Dump an entire PTEG */

static void
dumpPteg(unsigned long vsid, unsigned long pi, unsigned long hash)
{
PTE pte=ptegOf(&pgTbl,hash);
int i;
	PRINTF("hash 0x%08x, pteg 0x%08x (vsid 0x%08x, pi 0x%08x)\n",
			(unsigned)hash, (unsigned)pte,
			(unsigned)vsid, (unsigned)pi);
	for (i=0; i<PTE_PER_PTEG; i++,pte++) {
		PRINTF("pte 0x%08x is 0x%08x : 0x%08x\n",
			(unsigned)pte,
			(unsigned)*(unsigned long*)pte,
			(unsigned)*(((unsigned long*)pte)+1));
	}
}
 
/* Verify that a range of EAs is mapped the page table
 *
 * RETURNS: address of the first page for which no
 *          PTE was found (i.e. page index * page size)
 *          
 *          ON SUCCESS, the special value 0x0C0C ("OKOK")
 *          [which is not page aligned and hence is not
 *          a valid page address].
 */
unsigned long
triv121IsRangeMapped(unsigned long start, unsigned long end)
{
	start&=~((1<<LD_PG_SIZE)-1);
	while (start < end) {
		if (!alreadyMapped(&pgTbl,TRIV121_121_VSID,start))
			return start;
		start+=1<<LD_PG_SIZE;
	}
	return 0x0C0C; /* OKOK - not on a page boundary */
}

#endif


#if defined(DEBUG_MAIN) || defined(DEBUG)
#include <stdlib.h>

/* print a PTE */
static void
dumpPte(PTE pte)
{
	if (0==((unsigned long)pte & ((1<<LD_PTEG_SIZE)-1)))
		PRINTF("PTEG--");
	else
		PRINTF("......");
	if (pte->v) {
		PRINTF("VSID: 0x%08x H:%1i API: 0x%02x\n",
					pte->vsid, pte->h, pte->api);
		PRINTF("      ");
		PRINTF("RPN:  0x%08x WIMG: 0x%1x, (m %1i), pp: 0x%1x\n",
					pte->rpn, pte->wimg, pte->marked, pte->pp);
	} else {
		PRINTF("xxxxxx\n");
		PRINTF("      ");
		PRINTF("xxxxxx\n");
	}
}


/* dump page table entries from index 'from' to 'to'
 * The special values (unsigned)-1 are allowed which
 * cause the routine to dump the entire table.
 *
 * RETURNS 0
 */
int
triv121PgTblDump(Triv121PgTbl pt, unsigned from, unsigned to)
{
int i;
PTE	pte;
	PRINTF("Dumping PT [size 0x%08x == %i] at 0x%08x\n",
			(unsigned)pt->size, (unsigned)pt->size, (unsigned)pt->base);
	if (from> pt->size>>LD_PTE_SIZE)
		from=0;
	if (to  > pt->size>>LD_PTE_SIZE)
		to=(pt->size>>LD_PTE_SIZE);
	for (i=from,pte=pt->base+from; i<(long)to; i++, pte++) {
		dumpPte(pte);
	}
	return 0;
}


#if defined(DEBUG_MAIN)

#define LD_DBG_PT_SIZE	LD_MIN_PT_SIZE

int
main(int argc, char **argv)
{
unsigned long	base,start,numPages;
unsigned long	size=1<<LD_DBG_PT_SIZE;
Triv121PgTbl	pt;

	base=(unsigned long)malloc(size<<1);

	assert(base);

	/* align pt */
	base += size-1;
	base &= ~(size-1);

	assert(pt=triv121PgTblInit(base,LD_DBG_PT_SIZE));

	triv121PgTblDump(pt,(unsigned)-1, (unsigned)-1);
	do {
		do {
		PRINTF("Start Address:"); fflush(stdout);
		} while (1!=scanf("%i",&start));
		do {
		PRINTF("# pages:"); fflush(stdout);
		} while (1!=scanf("%i",&numPages));
	} while (TRIV121_MAP_SUCCESS==triv121PgTblMap(pt,TRIV121_121_VSID,start,numPages,
							TRIV121_ATTR_IO_PAGE,2) &&
			0==triv121PgTblDump(pt,(unsigned)-1,(unsigned)-1));
}
#endif
#endif

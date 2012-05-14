/* Trivial page table setup for RTEMS
 * Purpose: allow write protection of text/ro-data
 */

/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 4/2002, 2003, 2004,
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

/* Chose debugging options */
#undef  DEBUG_MAIN              /* create a standalone (host) program for basic testing */
#undef  DEBUG                   /* target debugging and consistency checking */
#undef  DEBUG_EXC               /* add exception handler which reenables BAT0 and recovers from a page fault */

#ifdef	DEBUG_MAIN
#undef	DEBUG                   /* must not use these together with DEBUG_MAIN */
#undef	DEBUG_EXC
#endif

/***************************** INCLUDE HEADERS ****************************/

#ifndef DEBUG_MAIN
#include <rtems.h>
#include <rtems/bspIo.h>
#include <libcpu/cpuIdent.h>
#include <libcpu/spr.h>
#ifdef	DEBUG_EXC
#include <bsp.h>
#include <bsp/vectors.h>
#endif
#endif

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "pte121.h"

/************************** CONSTANT DEFINITIONS **************************/

/* Base 2 logs of some sizes */

#ifndef DEBUG_MAIN

#define LD_PHYS_SIZE	32      /* physical address space */
#define LD_PG_SIZE		12      /* page size */
#define LD_PTEG_SIZE	6       /* PTEG size */
#define LD_PTE_SIZE		3       /* PTE size  */
#define LD_SEG_SIZE		28      /* segment size */
#define LD_MIN_PT_SIZE	16      /* minimal size of a page table */
#define LD_HASH_SIZE	19      /* lengh of a hash */
#define LD_VSID_SIZE	24      /* vsid bits in seg. register */

#else /* DEBUG_MAIN */

/* Reduced 'fantasy' sizes for testing */
#define LD_PHYS_SIZE	32      /* physical address space */
#define LD_PG_SIZE		6       /* page size */
#define LD_PTEG_SIZE	5       /* PTEG size */
#define LD_PTE_SIZE		3       /* PTE size  */
#define LD_SEG_SIZE		28      /* segment size */
#define LD_MIN_PT_SIZE	7       /* minimal size of a page table */
#define LD_HASH_SIZE	19      /* lengh of a hash */

#endif /* DEBUG_MAIN */

/* Derived sizes */

/* Size of a page index */
#define LD_PI_SIZE		((LD_SEG_SIZE) - (LD_PG_SIZE))

/* Number of PTEs in a PTEG */
#define PTE_PER_PTEG	(1<<((LD_PTEG_SIZE)-(LD_PTE_SIZE)))

/* Segment register bits */
#define KEY_SUP			(1<<30) /* supervisor mode key */
#define KEY_USR			(1<<29) /* user mode key */

/* The range of effective addresses to scan with 'tlbie'
 * instructions in order to flush all TLBs.
 * On the 750 and 7400, there are 128 two way I and D TLBs,
 * indexed by EA[14:19]. Hence calling
 *   tlbie rx
 * where rx scans 0x00000, 0x01000, 0x02000, ... 0x3f000
 * is sufficient to do the job
 */
#define NUM_TLB_PER_WAY 64      /* 750 and 7400 have 128 two way TLBs */
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

/* read VSID from segment register */
#ifndef DEBUG_MAIN
static uint32_t
seg2vsid (uint32_t ea)
{
  __asm__ volatile ("mfsrin %0, %0":"=r" (ea):"0" (ea));
  return ea & ((1 << LD_VSID_SIZE) - 1);
}
#else
#define seg2vsid(ea) VSID121(ea)
#endif

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
typedef void (*PrintF) (const char *, ...);
static PrintF
whatPrintf (void)
{
  return _Thread_Executing ? (PrintF) printf : printk;
}

#define PRINTF(args...) ((void)(whatPrintf())(args))
#else
#define PRINTF(args...) printf(args)
#endif

#ifdef DEBUG
unsigned long triv121PgTblConsistency (Triv121PgTbl pt, int pass, int expect);

static int consistencyPass = 0;
#define CONSCHECK(expect) triv121PgTblConsistency(&pgTbl,consistencyPass++,(expect))
#else
#define CONSCHECK(expect) do {} while (0)
#endif

/**************************** TYPE DEFINITIONS ****************************/

/* internal description of a trivial page table */
typedef struct Triv121PgTblRec_
{
  APte base;
  unsigned long size;
  int active;
} Triv121PgTblRec;


/************************** FORWARD DECLARATIONS *************************/

#ifdef DEBUG_EXC
static void myhdl (BSP_Exception_frame * excPtr);
#endif

static void dumpPte (APte pte);

#ifdef DEBUG
static void
dumpPteg (unsigned long vsid, unsigned long pi, unsigned long hash);
#endif

unsigned long
triv121IsRangeMapped (long vsid, unsigned long start, unsigned long end);

static void do_dssall (void);

/**************************** STATIC VARIABLES ****************************/

/* dont malloc - we might have to use this before
 * we have malloc or even RTEMS workspace available
 */
static Triv121PgTblRec pgTbl = { 0 };

#ifdef DEBUG_EXC
static void *ohdl;              /* keep a pointer to the original handler */
#endif

/*********************** INLINES & PRIVATE ROUTINES ***********************/

/* compute the page table entry group (PTEG) of a hash */
static inline APte
ptegOf (Triv121PgTbl pt, unsigned long hash)
{
  hash &= ((1 << LD_HASH_SIZE) - 1);
  return (APte) (((unsigned long) pt->
                  base) | ((hash << LD_PTEG_SIZE) & (pt->size - 1)));
}

/* see if a vsid/pi combination is already mapped
 *
 * RETURNS: PTE of mapping / NULL if none exists
 *
 * NOTE: a vsid<0 is legal and will tell this
 *       routine that 'pi' is actually an EA to
 *       be split into vsid and pi...
 */
static APte
alreadyMapped (Triv121PgTbl pt, long vsid, unsigned long pi)
{
  int i;
  unsigned long hash, api;
  APte pte;

  if (!pt->size)
    return 0;

  if (TRIV121_121_VSID == vsid) {
    vsid = VSID121 (pi);
    pi = PI121 (pi);
  } else if (TRIV121_SEG_VSID == vsid) {
    vsid = seg2vsid (pi);
    pi = PI121 (pi);
  }

  hash = PTE_HASH1 (vsid, pi);
  api = API (pi);
  for (i = 0, pte = ptegOf (pt, hash); i < PTE_PER_PTEG; i++, pte++)
    if (pte->v && pte->vsid == vsid && pte->api == api && 0 == pte->h)
      return pte;
  /* try the secondary hash table */
  hash = PTE_HASH2 (hash);
  for (i = 0, pte = ptegOf (pt, hash); i < PTE_PER_PTEG; i++, pte++)
    if (pte->v && pte->vsid == vsid && pte->api == api && 1 == pte->h)
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
static APte
slotFor (Triv121PgTbl pt, unsigned long vsid, unsigned long pi)
{
  int i;
  unsigned long hash, api;
  APte pte;

  /* primary hash */
  hash = PTE_HASH1 (vsid, pi);
  api = API (pi);
  /* linear search thru all buckets for this hash */
  for (i = 0, pte = ptegOf (pt, hash); i < PTE_PER_PTEG; i++, pte++) {
    if (!pte->v && !pte->marked) {
      /* found a free PTE; mark it as potentially used and return */
      pte->h = 0;               /* found by the primary hash fn */
      pte->marked = 1;
      return pte;
    }
  }

#ifdef DEBUG
  /* Strange: if the hash table was allocated big enough,
   *          this should not happen (when using a 1:1 mapping)
   *          Give them some information...
   */
  PRINTF ("## First hash bucket full - ");
  dumpPteg (vsid, pi, hash);
#endif

  hash = PTE_HASH2 (hash);
#ifdef DEBUG
  PRINTF ("   Secondary pteg is 0x%08x\n", (unsigned) ptegOf (pt, hash));
#endif
  for (i = 0, pte = ptegOf (pt, hash); i < PTE_PER_PTEG; i++, pte++) {
    if (!pte->v && !pte->marked) {
      /* mark this pte as potentially used */
      pte->marked = 1;
      pte->h = 1;
      return pte;
    }
  }
#ifdef DEBUG
  /* Even more strange - most likely, something is REALLY messed up */
  PRINTF ("## Second hash bucket full - ");
  dumpPteg (vsid, pi, hash);
#endif
  return 0;
}

/* unmark all entries */
static void
unmarkAll (Triv121PgTbl pt)
{
  unsigned long n = pt->size / sizeof (PTERec);
  unsigned long i;
  APte pte;
  for (i = 0, pte = pt->base; i < n; i++, pte++)
    pte->marked = 0;

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
triv121PgTblLdMinSize (unsigned long size)
{
  unsigned long i;
  /* round 'size' up to the next page boundary */
  size += (1 << LD_PG_SIZE) - 1;
  size &= ~((1 << LD_PG_SIZE) - 1);
  /* divide by number of PTEs  and multiply
   * by the size of a PTE.
   */
  size >>= LD_PG_SIZE - LD_PTE_SIZE;
  /* find the next power of 2 >= size */
  for (i = 0; i < LD_PHYS_SIZE; i++) {
    if ((1 << i) >= size)
      break;
  }
  /* pop up to the allowed minimum, if necessary */
  if (i < LD_MIN_PT_SIZE)
    i = LD_MIN_PT_SIZE;
  return i;
}

/* initialize a trivial page table of 2^ldSize bytes
 * at 'base' in memory.
 *
 * RETURNS:	OPAQUE HANDLE (not the hash table address)
 *          or NULL on failure.
 */
Triv121PgTbl
triv121PgTblInit (unsigned long base, unsigned ldSize)
{
  if (pgTbl.size) {
    /* already initialized */
    return 0;
  }

  if (ldSize < LD_MIN_PT_SIZE)
    return 0;                   /* too small */

  if (base & ((1 << ldSize) - 1))
    return 0;                   /* misaligned */

  /* This was tested on 604r, 750 and 7400.
   * On other CPUs, verify that the TLB invalidation works
   * for a new CPU variant and that it has hardware PTE lookup/
   * TLB replacement before adding it to this list.
   *
   * NOTE: The 603 features no hardware PTE lookup - and
   *       hence the page tables should NOT be used.
   *               Although lookup could be implemented in
   *               software this is probably not desirable
   *               as it could have an impact on hard realtime
   *               performance, screwing deterministic latency!
   *               (Could still be useful for debugging, though)
   */
  if ( ! ppc_cpu_has_hw_ptbl_lkup() )
    return 0;                   /* unsupported by this CPU */

  pgTbl.base = (APte) base;
  pgTbl.size = 1 << ldSize;
  /* clear all page table entries */
  memset (pgTbl.base, 0, pgTbl.size);

  CONSCHECK (0);

  /* map the page table itself 'm' and 'readonly' */
  if (triv121PgTblMap (&pgTbl,
                       TRIV121_121_VSID,
                       base,
                       (pgTbl.size >> LD_PG_SIZE),
                       TRIV121_ATTR_M, TRIV121_PP_RO_PAGE) >= 0)
    return 0;

  CONSCHECK ((pgTbl.size >> LD_PG_SIZE));

  return &pgTbl;
}

/* return the handle of the (one and only) page table
 * or NULL if none has been initialized yet.
 */
Triv121PgTbl
triv121PgTblGet (void)
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
triv121PgTblMap (Triv121PgTbl pt,
                 long ovsid,
                 unsigned long start,
                 unsigned long numPages,
                 unsigned attributes, unsigned protection)
{
  int i, pass;
  unsigned long pi;
  APte pte;
  long vsid;
#ifdef DEBUG
  long saved_vsid = ovsid;
#endif

  if (TRIV121_121_VSID == ovsid) {
    /* use 1:1 mapping */
    ovsid = VSID121 (start);
  } else if (TRIV121_SEG_VSID == ovsid) {
    ovsid = seg2vsid (start);
  }

#ifdef DEBUG
  PRINTF ("Mapping %i (0x%x) pages at 0x%08x for VSID 0x%08x\n",
          (unsigned) numPages, (unsigned) numPages,
          (unsigned) start, (unsigned) ovsid);
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
  for (pass = 0; pass < 2; pass++) {
    /* check if we would succeed during the first pass */
    for (i = 0, pi = PI121 (start), vsid = ovsid; i < numPages; i++, pi++) {
      if (pi >= 1 << LD_PI_SIZE) {
        vsid++;
        pi = 0;
      }
      /* leave alone existing mappings for this EA */
      if (!alreadyMapped (pt, vsid, pi)) {
        if (!(pte = slotFor (pt, vsid, pi))) {
          /* no free slot found for page index 'pi' */
          unmarkAll (pt);
          return pi;
        } else {
          /* have a free slot; marked by slotFor() */
          if (pass) {
            /* second pass; do the real work */
            pte->vsid = vsid;
            /* H was set by slotFor() */
            pte->api = API (pi);
            /* set up 1:1 mapping */
            pte->rpn =
              ((((unsigned long) vsid) &
                ((1 << (LD_PHYS_SIZE - LD_SEG_SIZE)) -
                 1)) << LD_PI_SIZE) | pi;
            pte->wimg = attributes & 0xf;
            pte->pp = protection & 0x3;
            /* mark it valid */
            pte->marked = 0;
            if (pt->active) {
              uint32_t flags;
              rtems_interrupt_disable (flags);
              /* order setting 'v' after writing everything else */
              __asm__ volatile ("eieio":::"memory");
              pte->v = 1;
              __asm__ volatile ("sync":::"memory");
              rtems_interrupt_enable (flags);
            } else {
              pte->v = 1;
            }

#ifdef DEBUG
            /* add paranoia */
            assert (alreadyMapped (pt, vsid, pi) == pte);
#endif
          }
        }
      }
    }
    unmarkAll (pt);
  }
#ifdef DEBUG
  {
    unsigned long failedat;
    CONSCHECK (-1);
    /* double check that the requested range is mapped */
    failedat =
      triv121IsRangeMapped (saved_vsid, start,
                            start + (1 << LD_PG_SIZE) * numPages);
    if (0x0C0C != failedat) {
      PRINTF ("triv121 mapping failed at 0x%08x\n", (unsigned) failedat);
      return PI121 (failedat);
    }
  }
#endif
  return TRIV121_MAP_SUCCESS;   /* -1 !! */
}

unsigned long
triv121PgTblSDR1 (Triv121PgTbl pt)
{
  return (((unsigned long) pt->base) & ~((1 << LD_MIN_PT_SIZE) - 1)) |
    (((pt->size - 1) >> LD_MIN_PT_SIZE) &
     ((1 << (LD_HASH_SIZE - (LD_MIN_PT_SIZE - LD_PTEG_SIZE))) - 1)
    );
}

void
triv121PgTblActivate (Triv121PgTbl pt)
{
#ifndef DEBUG_MAIN
  unsigned long          sdr1 = triv121PgTblSDR1 (pt);
  register unsigned long tmp0 = 16;	/* initial counter value (#segment regs) */
  register unsigned long tmp1 = (KEY_USR | KEY_SUP);
  register unsigned long tmp2 = (MSR_EE | MSR_IR | MSR_DR);
#endif
  pt->active = 1;

#ifndef DEBUG_MAIN
#ifdef DEBUG_EXC
  /* install our exception handler */
  ohdl = globalExceptHdl;
  globalExceptHdl = myhdl;
  __asm__ __volatile__ ("sync"::"memory");
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
  __asm__ __volatile (
    "	mtctr	%[tmp0]\n"
    /* Get MSR and switch interrupts off - just in case.
     * Also switch the MMU off; the book
     * says that SDR1 must not be changed with either
     * MSR_IR or MSR_DR set. I would guess that it could
     * be safe as long as the IBAT & DBAT mappings override
     * the page table...
     */
    "	mfmsr	%[tmp0]\n"
    "	andc	%[tmp2], %[tmp0], %[tmp2]\n"
    "	mtmsr	%[tmp2]\n"
    "	isync	\n"
    /* set up the segment registers */
    "	li		%[tmp2], 0\n"
    "1:	mtsrin	%[tmp1], %[tmp2]\n"
    "	addis	%[tmp2], %[tmp2], 0x1000\n" /* address next SR */
    "	addi	%[tmp1], %[tmp1], 1\n"      /* increment VSID  */
    "	bdnz	1b\n"
    /* Now flush all TLBs, starting with the topmost index */
    "	lis		%[tmp2], %[ea_range]@h\n"
    "2:	addic.	%[tmp2], %[tmp2], -%[pg_sz]\n"    /* address the next one (decrementing) */
    "	tlbie	%[tmp2]\n"             /* invalidate & repeat */
    "	bgt		2b\n"
    "	eieio	\n"
    "	tlbsync \n"
    "	sync    \n"
    /* set up SDR1 */
    "   mtspr	%[sdr1], %[sdr1val]\n"
    /* restore original MSR  */
    "	mtmsr	%[tmp0]\n"
    "	isync	\n"
      :[tmp0]"+r&"(tmp0), [tmp1]"+b&"(tmp1), [tmp2]"+b&"(tmp2)
      :[ea_range]"i"(FLUSH_EA_RANGE), [pg_sz]"i" (1 << LD_PG_SIZE),
       [sdr1]"i"(SDR1), [sdr1val]"r" (sdr1)
      :"ctr", "cc", "memory"
  );

  /* At this point, BAT0 is probably still active; it's the
   * caller's job to deactivate it...
   */
#endif
}

/**************************  DEBUGGING ROUTINES  *************************/

/* Exception handler to catch page faults */
#ifdef DEBUG_EXC

#define BAT_VALID_BOTH	3       /* allow user + super access */

static void
myhdl (BSP_Exception_frame * excPtr)
{
  if (3 == excPtr->_EXC_number) {
    unsigned long dsisr;

    /* reactivate DBAT0 and read DSISR */
    __asm__ __volatile__ (
      "mfspr %0, %1   \n"
      "ori   %0, %0, 3\n"
      "mtspr %1, %0   \n"
      "sync\n"
      "mfspr %0, %2\n"
        :"=&r" (dsisr)
        :"i" (DBAT0U), "i" (DSISR), "i" (BAT_VALID_BOTH)
    );

    printk ("Data Access Exception (DSI) # 3\n");
    printk ("Reactivated DBAT0 mapping\n");


    printk ("DSISR 0x%08x\n", dsisr);

    printk ("revectoring to prevent default handler panic().\n");
    printk ("NOTE: exception number %i below is BOGUS\n", ASM_DEC_VECTOR);
    /* make this exception 'recoverable' for
     * the default handler by faking a decrementer
     * exception.
     * Note that the default handler's message will be
     * wrong about the exception number.
     */
    excPtr->_EXC_number = ASM_DEC_VECTOR;
  }
/* now call the original handler */
  ((void (*)()) ohdl) (excPtr);
}
#endif



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
triv121PgTblConsistency (Triv121PgTbl pt, int pass, int expected)
{
  APte pte;
  int i;
  unsigned v, m;
  int warn = 0;
  int errs = 0;
  static int maxw = 20;         /* mute after detecting this many errors */

  PRINTF ("Checking page table at 0x%08x (size %i==0x%x)\n",
          (unsigned) pt->base, (unsigned) pt->size, (unsigned) pt->size);

  if (!pt->base || !pt->size) {
    PRINTF ("Uninitialized Page Table!\n");
    return 0;
  }

  v = m = 0;
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
   *                        Also see the comment a couple of lines down.
   */
  for (i = pt->size / sizeof (PTERec) - 1, pte = pt->base + i; i >= 0;
       i--, pte--)
#else
  for (i = 0, pte = pt->base; i < pt->size / sizeof (PTERec); i++, pte++)
#endif
  {
    int err = 0;
    char buf[500];
    unsigned long *lp = (unsigned long *) pte;
#if 0
    /* If I put this bogus while statement here (the body is
     * never reached), the original loop works OK
     */
    while (pte >= pt->base + pt->size / sizeof (PTERec))
      /* never reached */ ;
#endif

    if ( /* T.S: allow any VSID... (*lp & (0xfffff0 << 7)) || */ (*(lp + 1) & 0xe00)
        || (pte->v && pte->marked)) {
      /* check for vsid (without segment bits) == 0, unused bits == 0, valid && marked */
      sprintf (buf, "unused bits or v && m");
      err = 1;
    } else {
      if ( (*lp & (0xfffff0 << 7)) ) {
        sprintf(buf,"(warning) non-1:1 VSID found");
        err = 2;
      }
      if (pte->v)
        v++;
      if (pte->marked)
        m++;
    }
    if (err && maxw) {
      PRINTF
        ("Pass %i -- strange PTE at 0x%08x found for page index %i == 0x%08x:\n",
         pass, (unsigned) pte, i, i);
      PRINTF ("Reason: %s\n", buf);
      dumpPte (pte);
      if ( err & 2 ) {
         warn++;
      } else {
         errs++;
      }
      maxw--;
    }
  }
  if (errs) {
    PRINTF ("%i errors %s", errs, warn ? "and ":"");
  }
  if (warn) {
    PRINTF ("%i warnings ",warn);
  }
  if (errs || warn) {
    PRINTF ("found; currently %i entries marked, %i are valid\n",
            m, v);
  }
  v += m;
  if (maxw && expected >= 0 && expected != v) {
    /* number of occupied slots not what they expected */
    PRINTF ("Wrong # of occupied slots detected during pass");
    PRINTF ("%i; should be %i (0x%x) is %i (0x%x)\n",
            pass, expected, (unsigned) expected, v, (unsigned) v);
    maxw--;
  }
  return v;
}

/* Find the PTE for a EA and print its contents
 * RETURNS: pte for EA or NULL if no entry was found.
 */
APte
triv121DumpEa (unsigned long ea)
{
  APte pte;

  pte =
    alreadyMapped (&pgTbl, pgTbl.active ? TRIV121_SEG_VSID : TRIV121_121_VSID,
                   ea);

  if (pte)
    dumpPte (pte);
  return pte;
}

APte
triv121FindPte (unsigned long vsid, unsigned long pi)
{
  return alreadyMapped (&pgTbl, vsid, pi);
}

APte
triv121UnmapEa (unsigned long ea)
{
  uint32_t flags;
  APte pte;

  if (!pgTbl.active) {
    pte = alreadyMapped (&pgTbl, TRIV121_121_VSID, ea);
    if (pte)                    /* alreadyMapped checks for pte->v */
      pte->v = 0;
    return pte;
  }

  pte = alreadyMapped (&pgTbl, TRIV121_SEG_VSID, ea);

  if (!pte)
    return 0;

  rtems_interrupt_disable (flags);
  pte->v = 0;
  do_dssall ();
  __asm__ volatile ("	sync		\n\t"
                "	tlbie %0	\n\t"
                "	eieio		\n\t"
                "	tlbsync		\n\t"
                "	sync		\n\t"::"r" (ea):"memory");
  rtems_interrupt_enable (flags);
  return pte;
}

/* A context synchronizing jump */
#define SYNC_LONGJMP(msr)				\
	asm volatile(						\
		"	mtsrr1	%0			\n\t"	\
		"	bl		1f			\n\t"	\
		"1:	mflr	3			\n\t"	\
		"	addi	3,3,1f-1b	\n\t"	\
		"	mtsrr0	3			\n\t"	\
		"	rfi					\n\t"	\
		"1:						\n\t"	\
		:								\
		:"r"(msr)						\
		:"3","lr","memory")

/* The book doesn't mention dssall when changing PTEs
 * but they require it for BAT changes and I guess
 * it makes sense in the case of PTEs as well.
 * Just do it to be on the safe side...
 */
static void
do_dssall (void)
{
  /* Before changing BATs, 'dssall' must be issued.
   * We check MSR for MSR_VE and issue a 'dssall' if
   * MSR_VE is set hoping that
   *  a) on non-altivec CPUs MSR_VE reads as zero
   *  b) all altivec CPUs use the same bit
   *
   * NOTE: psim doesn't implement dssall so we skip if we run on psim
   */
  if ( (_read_MSR () & MSR_VE) && PPC_PSIM != get_ppc_cpu_type() ) {
    /* this construct is needed because we don't know
     * if this file is compiled with -maltivec.
     * (I plan to add altivec support outside of
     * RTEMS core and hence I'd rather not
     * rely on consistent compiler flags).
     */
#define DSSALL	0x7e00066c      /* dssall opcode */
    __asm__ volatile ("	.long %0"::"i" (DSSALL));
#undef  DSSALL
  }
}

APte
triv121ChangeEaAttributes (unsigned long ea, int wimg, int pp)
{
  APte pte;
  unsigned long msr;

  if (!pgTbl.active) {
    pte = alreadyMapped (&pgTbl, TRIV121_121_VSID, ea);
    if (!pte)
      return 0;
    if (wimg > 0)
      pte->wimg = wimg;
    if (pp > 0)
      pte->pp = pp;
    return pte;
  }

  pte = alreadyMapped (&pgTbl, TRIV121_SEG_VSID, ea);

  if (!pte)
    return 0;

  if (wimg < 0 && pp < 0)
    return pte;

  __asm__ volatile ("mfmsr %0":"=r" (msr));

  /* switch MMU and IRQs off */
  SYNC_LONGJMP (msr & ~(MSR_EE | MSR_DR | MSR_IR));

  pte->v = 0;
  do_dssall ();
  __asm__ volatile ("sync":::"memory");
  if (wimg >= 0)
    pte->wimg = wimg;
  if (pp >= 0)
    pte->pp = pp;
  __asm__ volatile ("tlbie %0; eieio"::"r" (ea):"memory");
  pte->v = 1;
  __asm__ volatile ("tlbsync; sync":::"memory");

  /* restore, i.e., switch MMU and IRQs back on */
  SYNC_LONGJMP (msr);

  return pte;
}

static void
pgtblChangePP (Triv121PgTbl pt, int pp)
{
  unsigned long n = pt->size >> LD_PG_SIZE;
  unsigned long b, i;

  for (i = 0, b = (unsigned long) pt->base; i < n;
       i++, b += (1 << LD_PG_SIZE)) {
    triv121ChangeEaAttributes (b, -1, pp);
  }
}

void
triv121MakePgTblRW ()
{
  pgtblChangePP (&pgTbl, TRIV121_PP_RW_PAGE);
}

void
triv121MakePgTblRO ()
{
  pgtblChangePP (&pgTbl, TRIV121_PP_RO_PAGE);
}

long
triv121DumpPte (APte pte)
{
  if (pte)
    dumpPte (pte);
  return 0;
}


#ifdef DEBUG
/* Dump an entire PTEG */

static void
dumpPteg (unsigned long vsid, unsigned long pi, unsigned long hash)
{
  APte pte = ptegOf (&pgTbl, hash);
  int i;
  PRINTF ("hash 0x%08x, pteg 0x%08x (vsid 0x%08x, pi 0x%08x)\n",
          (unsigned) hash, (unsigned) pte, (unsigned) vsid, (unsigned) pi);
  for (i = 0; i < PTE_PER_PTEG; i++, pte++) {
    PRINTF ("pte 0x%08x is 0x%08x : 0x%08x\n",
            (unsigned) pte,
            (unsigned) *(unsigned long *) pte,
            (unsigned) *(((unsigned long *) pte) + 1));
  }
}
#endif

/* Verify that a range of addresses is mapped the page table.
 * start/end are segment offsets or EAs (if vsid has one of
 * the special values), respectively.
 *
 * RETURNS: address of the first page for which no
 *          PTE was found (i.e. page index * page size)
 *
 *          ON SUCCESS, the special value 0x0C0C ("OKOK")
 *          [which is not page aligned and hence is not
 *          a valid page address].
 */

unsigned long
triv121IsRangeMapped (long vsid, unsigned long start, unsigned long end)
{
unsigned pi;

  start &= ~((1 << LD_PG_SIZE) - 1);
  while (start < end) {
    if ( TRIV121_SEG_VSID != vsid && TRIV121_121_VSID != vsid )
      pi = PI121(start);
    else
      pi = start;
    if (!alreadyMapped (&pgTbl, vsid, pi))
      return start;
    start += 1 << LD_PG_SIZE;
  }
  return 0x0C0C;                /* OKOK - not on a page boundary */
}


#include <stdlib.h>

/* print a PTE */
static void
dumpPte (APte pte)
{
  if (0 == ((unsigned long) pte & ((1 << LD_PTEG_SIZE) - 1)))
    PRINTF ("PTEG--");
  else
    PRINTF ("......");
  if (pte->v) {
    PRINTF ("VSID: 0x%08x H:%1i API: 0x%02x\n", pte->vsid, pte->h, pte->api);
    PRINTF ("      ");
    PRINTF ("RPN:  0x%08x WIMG: 0x%1x, (m %1i), pp: 0x%1x\n",
            pte->rpn, pte->wimg, pte->marked, pte->pp);
  } else {
    PRINTF ("xxxxxx\n");
    PRINTF ("      ");
    PRINTF ("xxxxxx\n");
  }
}


/* dump page table entries from index 'from' to 'to'
 * The special values (unsigned)-1 are allowed which
 * cause the routine to dump the entire table.
 *
 * RETURNS 0
 */
int
triv121PgTblDump (Triv121PgTbl pt, unsigned from, unsigned to)
{
  int i;
  APte pte;
  PRINTF ("Dumping PT [size 0x%08x == %i] at 0x%08x\n",
          (unsigned) pt->size, (unsigned) pt->size, (unsigned) pt->base);
  if (from > pt->size >> LD_PTE_SIZE)
    from = 0;
  if (to > pt->size >> LD_PTE_SIZE)
    to = (pt->size >> LD_PTE_SIZE);
  for (i = from, pte = pt->base + from; i < (long) to; i++, pte++) {
    dumpPte (pte);
  }
  return 0;
}


#if defined(DEBUG_MAIN)

#define LD_DBG_PT_SIZE	LD_MIN_PT_SIZE

int
main (int argc, char **argv)
{
  unsigned long base, start, numPages;
  unsigned long size = 1 << LD_DBG_PT_SIZE;
  Triv121PgTbl pt;

  base = (unsigned long) malloc (size << 1);

  assert (base);

  /* align pt */
  base += size - 1;
  base &= ~(size - 1);

  assert (pt = triv121PgTblInit (base, LD_DBG_PT_SIZE));

  triv121PgTblDump (pt, (unsigned) -1, (unsigned) -1);
  do {
    do {
      PRINTF ("Start Address:");
      fflush (stdout);
    } while (1 != scanf ("%i", &start));
    do {
      PRINTF ("# pages:");
      fflush (stdout);
    } while (1 != scanf ("%i", &numPages));
  } while (TRIV121_MAP_SUCCESS ==
           triv121PgTblMap (pt, TRIV121_121_VSID, start, numPages,
                            TRIV121_ATTR_IO_PAGE, 2)
           && 0 == triv121PgTblDump (pt, (unsigned) -1, (unsigned) -1));
}
#endif

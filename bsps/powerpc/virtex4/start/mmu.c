/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCVirtex4MMU
 *
 * @brief Implementation of routines to manipulate the PPC 405 mmu.
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
 * and was transcribed for the PPC 405 by
 *     R. Claus <claus@slac.stanford.edu>, 2012,
 *       Stanford Linear Accelerator Center, Stanford University,
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

/* 405 MSR definitions; note that there are *substantial* differences
 * compared to classic powerpc; in particular, IS/DS are *different*
 * from IR/DR.
 *
 * Also: To disable/enable all external interrupts, CE and EE must both be
 *       controlled.
 */
#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/powerpc/powerpc.h>
#include <inttypes.h>
#include <stdio.h>

#include <bsp/mmu.h>


#ifdef DEBUG
#define STATIC
#else
#define STATIC static
#endif


bsp_tlb_entry_t* bsp_mmu_cache = 0;


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
    /* Might be called at an early stage when stdio is not yet initialized. */
    vprintk(fmt,ap);
  } else {
    vfprintf(f,fmt,ap);
  }
  va_end(ap);
}


void
bsp_mmu_dump_cache(FILE *f)
{
  bsp_tlb_idx_t idx;
  if ( !bsp_mmu_cache ) {
    myprintf(stderr,"MMU TLB cache not initialized\n");
    return;
  }
  for ( idx=0; idx<NTLBS; idx++ ) {
    bsp_tlb_entry_t *tlb = bsp_mmu_cache + idx;
    if ( !tlb->hi.v )
      continue;
    myprintf(f, "#%2i: EA 0x%08x .. 0x%08x, TID  0x%03x, EU0  0x%01x\n",
             idx,
             tlb->hi.epn << 10,
             (tlb->hi.epn << 10) + (1024<<(2*tlb->hi.size))-1,
             tlb->id.tid,
             tlb->hi.att);
    myprintf(f, "     PA 0x%08"PRIx32" .. 0x%08"PRIx32", PERM 0x%03x, WIMG 0x%02x\n",
             tlb->lo.rpn << 10,
             (tlb->lo.rpn << 10) + (1024<<(2*tlb->hi.size))-1,
             tlb->lo.perm,
             tlb->lo.wimg);
  }
}

static void
fetch(bsp_tlb_idx_t key, bsp_tlb_entry_t* tlb)
{
  register uint32_t tmp;
  __asm__ volatile ("mfpid   %[tmp]           \n\t"
                    "stw     %[tmp],0(%[tlb]) \n\t"
                    "tlbrehi %[tmp],%[key]    \n\t"
                    "stw     %[tmp],4(%[tlb]) \n\t"
                    "tlbrelo %[tmp],%[key]    \n\t"
                    "stw     %[tmp],8(%[tlb]) \n\t"
                    "sync                     \n\t"
                    : [tmp]"=&r"(tmp)
                    : [key]"r"(key),
                      [tlb]"b"(tlb)
                    );
}


static void
store(bsp_tlb_idx_t key, bsp_tlb_entry_t* tlb)
{
  register uint32_t tmp;
  __asm__ volatile ("lwz     %[tmp],0(%[tlb]) \n\t"
                    "mtpid   %[tmp]           \n\t"
                    "lwz     %[tmp],4(%[tlb]) \n\t"
                    "tlbwehi %[tmp],%[key]    \n\t"
                    "lwz     %[tmp],8(%[tlb]) \n\t"
                    "tlbwelo %[tmp],%[key]    \n\t"
                    : [tmp]"=&r"(tmp)
                    : [tlb]"b"(tlb),
                      [key]"r"(key)
                    );
}


static void
commit(void)
{
  __asm__ volatile("isync           \n\t");
}


/*
 * Read a TLB entry from the hardware and store the current settings in the
 * bsp_mmu_cache[] structure.
 *
 * The routine can perform this operation quietly or
 * print information to a file.
 *
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
 *                (-2: driver not initialized)
 */
int
bsp_mmu_update(bsp_tlb_idx_t key, bool quiet, FILE *f)
{
  rtems_interrupt_level  lvl;
  bsp_tlb_entry_t*       tlb;
  bsp_tlb_idx_t          idx;

  idx = key;

  if ( idx < 0 || idx > NTLBS-1 )
    return -1;

  if (!bsp_mmu_cache)
    return -2;

  tlb = bsp_mmu_cache + idx;

  rtems_interrupt_disable(lvl);

  fetch(idx, tlb);

  rtems_interrupt_enable(lvl);

  if ( tlb->hi.v ) {
    if ( !quiet ) {
/*
               "TLB Entry #  0 spans EA range     0x00000000 - 0x00000000
               "Mapping:     VA     [TID 0x00 / EPN 0x00000] -> RPN 0x00000"
               "Size:        TSIZE 0x0  (4^sz KB = 000000 KB = 0x00000000 B)
               "Attributes:  PERM  0x00 (ex/wr/zsel) WIMG 0x00 EU0 0x0"
*/
      myprintf(f,
               "TLB Entry # %2d spans EA range     0x%08x - 0x%08x\n",
               idx,
               (tlb->hi.epn << 10),
               (tlb->hi.epn << 10) + (1024<<(2*tlb->hi.size)) - 1
               );

      myprintf(f,
               "Mapping:     VA     [TID 0x%02x / EPN 0x%05x] -> RPN 0x%05"PRIx32"\n",
               tlb->id.tid, tlb->hi.epn, tlb->lo.rpn
               );
      myprintf(f,
               "Size:        TSIZE 0x%x  (4^sz KB = %6d KB = 0x%08x B)\n",
               tlb->hi.size, (1<<(2*tlb->hi.size)), (1024<<(2*tlb->hi.size))
               );
      myprintf(f,
               "Attributes:  PERM  0x%02x (ex/wr/zsel) WIMG 0x%02x EU0 0x%01x\n",
               tlb->lo.perm, tlb->lo.wimg, tlb->hi.att
               );
    }
  } else {
    if ( !quiet ) {
      myprintf(f,
               "TLB Entry # %2d <OFF> (size 0x%x = 0x%xb)\n",
               idx, tlb->hi.size, (1024<<(2*tlb->hi.size))
               );
    }
    return 1;
  }
  return 0;
}

/* Initialize cache.  Should be done only once although this is not enforced.
 *
 * RETURNS: zero on success, nonzero on error; in this case the driver will
 *          refuse to change TLB entries (other than disabling them).
 */
int
bsp_mmu_initialize()
{
  static bsp_tlb_entry_t mmu_cache[NTLBS];
  bsp_tlb_entry_t*       tlb = mmu_cache;  /* Should malloc if it's not too early */
  rtems_interrupt_level  lvl;

  bsp_tlb_idx_t idx;
  rtems_interrupt_disable(lvl);
  for (idx=0; idx<NTLBS; tlb++, idx++)
  {
    fetch(idx, tlb);
  }
  rtems_interrupt_enable(lvl);

  bsp_mmu_cache = mmu_cache;
  return 0;
}

/* Find first free TLB entry by examining all entries' valid bit.  The first
 * entry without the valid bit set is returned.
 *
 * RETURNS: A free TLB entry number.  -1 if no entry can be found.
 */
bsp_tlb_idx_t
bsp_mmu_find_first_free()
{
  bsp_tlb_idx_t   idx;
  bsp_tlb_entry_t entry;

  for (idx=0; idx<NTLBS; idx++) {
    register uint32_t tmp;
    __asm__ volatile ("tlbrehi %[tmp],%[idx]    \n\t"
                      "stw     %[tmp],4(%[tlb]) \n\t" /* entry.hi */
                      "sync                     \n\t"
                      : [tmp]"=&r"(tmp)
                      : [idx]"r"(idx),
                        [tlb]"b"(&entry)
                      : "memory"
                      );
    if (!(entry.hi.v))
      break;
  }
  return (idx < NTLBS) ? idx : -1;
}

/*
 * Write TLB entry (can also be used to disable an entry).
 *
 * The routine checks against the cached data in
 * bsp_mmu_cache[] to prevent the user from generating
 * overlapping entries.
 *
 *   'idx': TLB entry # to manipulate
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
 *  'flgs': Page's little-endian & user-defined flags, permissions and attributes
 *   'tid': Translation ID
 *
 * RETURNS: 0 on success, nonzero on error:
 *
 *         >0: requested mapping would overlap with
 *             existing mapping in other entry. Return
 *             value gives conflicting entry + 1; i.e.,
 *             if a value of 4 is returned then the request
 *             conflicts with existing mapping in entry 3.
 *         -1: invalid argument
 *         -3: driver not initialized (or initialization failed).
 *         <0: other error
 */
bsp_tlb_idx_t
bsp_mmu_write(bsp_tlb_idx_t idx, uint32_t ea, uint32_t pa, uint sz,
              uint32_t flgs, uint32_t tid)
{
  bsp_tlb_entry_t       tlb;
  uint32_t              msk;
  bsp_tlb_idx_t         lkup;
  rtems_interrupt_level lvl;

  if ( sz >= 1024 ) {
    /* Assume they literally specify a size */
    msk = sz;
    sz  = 0;
    while ( msk != (1024u<<(2*sz)) ) {
      if ( ++sz > 7 ) {
        return -1;
      }
    }
    /* OK, acceptable */
  }

  msk = sz > 0 ? (1024u<<(2*sz)) - 1 : 0;

  if ( !bsp_mmu_cache && sz > 0 ) {
    myprintf(stderr,"MMU driver not initialized; refusing to enable any entry\n");
    return -3;
  }

  if ( (ea & msk) || (pa & msk) ) {
    myprintf(stderr,"Misaligned EA (%08x) or PA (%08x) (mask is %08x)\n", ea, pa, msk);
    return -1;
  }

  if ( idx < 0 || idx > NTLBS-1 )
    return -1;

  if ( sz > 7 ) {
    myprintf(stderr,"Invalid size %u = %08x = %u KB\n", sz, 1024u<<(2*sz), (1024u<<(2*sz))/1024);
    return -1;
  }

  if ( sz >=0 ) {
    lkup = bsp_mmu_match(ea, sz, tid);

    if ( lkup < -1 ) {
      /* some error */
      return lkup;
    }
    if ( (lkup >= 0) && (lkup != idx) && (bsp_mmu_cache[lkup].hi.v != 0) ) {
      myprintf(stderr,"TLB #%i overlaps with requested mapping\n", lkup);
      bsp_mmu_update( lkup, false, stderr);
      return lkup+1;
    }
  }

  /* OK to proceed */
  tlb.id.tid  = tid;
  tlb.hi.v    = sz >= 0;
  tlb.hi.size = sz;
  tlb.hi.epn  = (ea & (0xfffffc00 << (sz + sz))) >> 10;
  tlb.lo.rpn  = (pa & (0xfffffc00 << (sz + sz))) >> 10;
  tlb.hi.att  = (flgs & MMU_M_ATTR) >> MMU_V_ATTR;
  tlb.lo.perm = (flgs & MMU_M_PERM) >> MMU_V_PERM;
  tlb.lo.wimg = (flgs & MMU_M_PROP) >> MMU_V_PROP;

  rtems_interrupt_disable(lvl);

  store(idx, &tlb);

  commit();

  rtems_interrupt_enable(lvl);

  /* update cache */
  bsp_mmu_update(idx, true, 0);

  return 0;
}

/*
 * Check if a ea/sz/tid mapping overlaps with an existing entry.
 *
 *    'ea': The Effective Address to match against
 *    'sz': The 'logarithmic' size selector; the page size
 *          is 1024*2^(2*sz).
 *   'tid': The TID to match against
 *
 * RETURNS:
 *     >= 0: index of the TLB entry that already provides a mapping
 *           which overlaps within the ea range.
 *       -1: SUCCESS (no conflicting entry found)
 *     <=-2: ERROR (invalid input)
 */
bsp_tlb_idx_t
bsp_mmu_match(uint32_t ea, int sz, uint32_t tid)
{
  bsp_tlb_idx_t    idx;
  uint32_t         m,a;
  bsp_tlb_entry_t* tlb;

  if ( sz < 0 || sz > 7 )
    return -4;

  sz = (1024<<(2*sz));

  if ( !bsp_mmu_cache ) {
    /* cache not initialized */
    return -3;
  }

  if ( ea & (sz-1) ) {
    /* misaligned ea */
    return -2;
  }

  for ( idx=0, tlb=bsp_mmu_cache; idx<NTLBS; idx++, tlb++ ) {
    if ( ! tlb->hi.v )
      continue;
    if ( tlb->id.tid && tlb->id.tid != tid )
      continue;
    /* TID matches a valid entry */
    m  = (1024<<(2*tlb->hi.size)) - 1;
    /* calculate starting address of this entry */
    a  = tlb->hi.epn << 10;
    if ( ea <= a + m && ea + sz -1 >= a ) {
      /* overlap */
      return idx;
    }
  }
  return -1;
}

/* Find TLB index that maps 'ea/tid' combination
 *
 *    'ea': Effective address to match against
 *   'tid': The TID to match against
 *
 * RETURNS: index 'key' which indicates whether
 *          the mapping was found.
 *
 *          On error (no mapping) -1 is returned.
 */
bsp_tlb_idx_t
bsp_mmu_find(uint32_t ea, uint32_t tid)
{
  rtems_interrupt_level  lvl;
  register uint32_t      pid;
  register bsp_tlb_idx_t idx;
  register int           failure;

  rtems_interrupt_disable(lvl);

  __asm__ volatile ("mfpid  %[pid]         \n\t" /* Save PID */
                    "mtpid  %[tid]         \n\t"
                    "tlbsx. %[idx],0,%[ea] \n\t" /* Failure changes the index reg randomly. */
                    "mfcr   %[failure]     \n\t"
                    "mtpid  %[pid]         \n\t" /* Restore PID */
                    : [pid]"=r"(pid),
                      [idx]"=&r"(idx),
                      [failure]"=&r"(failure)
                    : [tid]"r"(tid),
                      [ea]"r"(ea)
                    : "cc"
                    );

  rtems_interrupt_enable(lvl);

  return (failure & 0x20000000) ? idx : -1;
}

/* Mark TLB entry as invalid ('disabled').
 *
 * 'key': TLB entry (index).
 *
 * RETURNS: zero on success, nonzero on error (TLB unchanged).
 *
 * NOTE:  If a TLB entry is disabled the associated
 *        entry in bsp_mmu_cache[] is also
 *        marked as disabled.
 */
int
bsp_mmu_invalidate(bsp_tlb_idx_t key)
{
  bsp_tlb_idx_t         k0;
  rtems_interrupt_level lvl;
  bsp_tlb_entry_t       tlb;
  uint32_t              msr;

  /* Minimal guard against bad key */
  if ( key < 0 || key > NTLBS-1 )
    return -1;

  _CPU_MSR_GET(msr);

  /* While address translation is enabled... */
  if (msr & (PPC_MSR_IR | PPC_MSR_DR))
  {
    /* Must not invalidate page 0 which holds vectors, text etc...  */
    k0 = bsp_mmu_find(0, 0);
    if ( -1 == k0 ) {
      myprintf(stderr,"No mapping for address 0 found\n");
      return -2;
    }

    /* NOTE: we assume PID is ignored */
    if ( k0 == key ) {
      myprintf(stderr,"Cannot invalidate page holding address 0 (always needed)\n");
      return -3;
    }
  }

  rtems_interrupt_disable(lvl);

  fetch(key, &tlb);

  /* Invalidate old entries */
  tlb.hi.v = 0;

  store(key, &tlb);

  commit();

  /* update cache */
  bsp_mmu_cache[ key ].hi.v = tlb.hi.v;

  rtems_interrupt_enable(lvl);

  return 0;
}

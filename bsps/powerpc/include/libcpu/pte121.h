#ifndef _LIBCPU_PTE121_H
#define _LIBCPU_PTE121_H

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

/* Rudimentary page/hash table support for Powerpc
 *
 * A simple, static (i.e. no 'per-process' virtual
 * address space etc.) page table providing
 * one-to-one effective <-> virtual <-> physical
 * address mapping.
 *
 * PURPOSE:
 *    1) allow write-protection of text/read-only data areas
 *    2) provide more effective-address space in case
 *       the BATs are not enough
 *    3) allow 'alias' mappings. Such aliases can only use
 *       the upper bits of the VSID since VSID & 0xf and the
 *       PI are always mapped 1:1 to the RPN.
 * LIMITATIONS:
 *    -  no PTE replacement (makes no sense in a real-time
 *       environment, anyway) -> the page table just MUST
 *       be big enough!.
 *    -  only one page table supported.
 *    -  no locking implemented. If multiple threads modify
 *       the page table, it is the user's responsibility to
 *       implement exclusive access.
 */


/* I don't include mmu.h here because it says it's derived from linux
 * and I want to avoid licensing problems
 */

/* Abstract handle for a page table */
typedef struct Triv121PgTblRec_ *Triv121PgTbl;

/* A PTE entry */
typedef struct PTERec_ {
  volatile unsigned long v:1,    vsid:24, h:1, api: 6;
  volatile unsigned long rpn:20, pad: 3, r:1, c:1, wimg:4, marked:1, pp:2;
} PTERec, *APte;

/* Initialize a trivial page table
 * using 2^ldSize bytes of memory starting at
 * 'base'.
 *
 * RETURNS: a handle to the internal data structure
 *          used to manage the page table. NULL on
 *          error.
 *
 * NOTES:   - 'base' must be aligned to the size
 *          - minimal ldSize is 16 (== 64k)
 *          - this routine maps the page table itself
 *            with read-only access. While this prevents
 *            the CPU from overwriting the page table,
 *            it can still be corrupted by PCI bus masters
 *            (like DMA engines, [VME] bridges etc.) and
 *            even by this CPU if either the MMU is off
 *            or if there is a DBAT mapping granting write
 *            access...
 */
Triv121PgTbl
triv121PgTblInit(unsigned long base, unsigned ldSize);

/* get the log2 of the minimal page table size needed
 * for mapping 'size' bytes.
 *
 * EXAMPLE: create a page table which maps the entire
 *          physical memory. The page table itself shall
 *          be allocated at the top of the available
 *          memory (assuming 'memsize' is a power of two):
 *
 *  ldSize = triv121PgTblLdMinSize(memsize);
 *  memsize -= (1<<ldSize);  / * reduce memory available to RTEMS * /
 *  pgTbl  = triv121PgTblInit(memsize,ldSize);
 *
 */
unsigned long
triv121PgTblLdMinSize(unsigned long size);

/* Map an address range 1:1 in pgTbl with the given protection;
 *
 * RETURNS: -1 (TRIV121_MAP_SUCCESS) on success; the page index
 *          for which no PTE could be allocated, on failure.
 *
 * NOTES:   - This routine returns MINUS ONE ON SUCCESS
 *          - (parts) of a mapping which overlap with
 *            already existing PTEs are silently ignored.
 *
 *            Therefore, you can e.g. first create
 *            a couple of write protected maps and
 *            finally map the entire memory r/w. This
 *            will leave the write protected maps
 *            intact.
 */
long
triv121PgTblMap(
  Triv121PgTbl  pgTbl,     /* handle, returned by Init or Get */

  long          vsid,      /* vsid for this mapping (contains topmost 4 bits of EA);
                            *
                            * NOTE: it is allowed to pass a VSID < 0 to tell this
                            *       routine it should use a VSID corresponding to a
                            *       1:1:1  effective - virtual - physical  mapping
                            */

  unsigned long start,     /* segment offset (lowermost 28 bits of EA) of address range
                            *
                            * NOTE: if VSID < 0 (TRIV121_121_VSID), 'start' is inter-
                            *       preted as an effective address (EA), i.e. all 32
                            *       bits are used - the most significant four going into
                            *       to the VSID...
                            */

  unsigned long numPages,  /* number of pages to map */

  unsigned wimgAttr,       /* 'wimg' attributes
                            * (Write thru, cache Inhibit, coherent Memory,
                            *  Guarded memory)
                            */

  unsigned protection      /* 'pp' access protection: Super      User
                            *
                            *   0                      r/w       none
                            *   1                      r/w       ro
                            *   2                      r/w       r/w
                            *   3                      ro        ro
                            */
);

#define TRIV121_ATTR_W  8
#define TRIV121_ATTR_I  4
#define TRIV121_ATTR_M  2
#define TRIV121_ATTR_G  1

/* for I/O pages (e.g. PCI, VME addresses) use cache inhibited
 * and guarded pages. RTM about the 'eieio' instruction!
 */
#define TRIV121_ATTR_IO_PAGE    (TRIV121_ATTR_I|TRIV121_ATTR_G)

#define TRIV121_PP_RO_PAGE      (1)  /* read-only for key = 1, unlocked by key=0 */
#define TRIV121_PP_RW_PAGE      (2)  /* read-write for key = 1/0                 */

#define TRIV121_121_VSID        (-1) /* use 1:1 effective<->virtual address mapping */
#define TRIV121_SEG_VSID        (-2) /* lookup VSID in the segment register         */

#define TRIV121_MAP_SUCCESS     (-1) /* triv121PgTblMap() returns this on SUCCESS */

/* get a handle to the one and only page table
 * (must have been initialized/allocated)
 *
 * RETURNS: NULL if the page table has not been initialized/allocated.
 */
Triv121PgTbl
triv121PgTblGet(void);

/*
 * compute the SDR1 register value for the page table
 */

unsigned long
triv121PgTblSDR1(Triv121PgTbl pgTbl);

/*
 * Activate the page table:
 *  - set up the segment registers for a 1:1 effective <-> virtual address mapping,
 *    give user and supervisor keys.
 *  - set up the SDR1 register
 *  - flush all tlbs
 *  - 'lock' pgTbl, i.e. prevent all further modifications.
 *
 * NOTE: This routine does not change any BATs. Since these
 *       have priority over the page table, the user
 *       may have to switch overlapping BATs OFF in order
 *       for the page table mappings to take effect.
 */
void triv121PgTblActivate(Triv121PgTbl pgTbl);

/* Find the PTE for a EA and print its contents to stdout
 * RETURNS: pte for EA or NULL if no entry was found.
 */
APte triv121DumpEa(unsigned long ea);

/* Find and return a PTE for a vsid/pi combination
 * RETURNS: pte or NULL if no entry was found
 */
APte triv121FindPte(unsigned long vsid, unsigned long pi);

/*
 * Unmap an effective address
 *
 * RETURNS: pte that mapped the ea or NULL if no
 *          mapping existed.
 */
APte triv121UnmapEa(unsigned long ea);

/*
 * Change the WIMG and PP attributes of the page containing 'ea'
 *
 * NOTES:   The 'wimg' and 'pp' may be <0 to indicate that no
 *          change is desired.
 *
 * RETURNS: Pointer to modified PTE or NULL if 'ea' is not mapped.
 */
APte triv121ChangeEaAttributes(unsigned long ea, int wimg, int pp);

/* Make the whole page table writable
 * NOTES:   If the page table has not been initialized yet,
 *          this routine has no effect (i.e., after
 *          initialization the page table will still be read-only).
 */
void triv121MakePgTblRW(void);

/* Make the whole page table read-only
 */
void triv121MakePgTblRO(void);

/* Dump a pte to stdout */
long triv121DumpPte(APte pte);

#endif

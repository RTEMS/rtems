#ifndef RTEMS_E500_MMU_DRIVER_H
#define RTEMS_E500_MMU_DRIVER_H

/*
 * Routines to manipulate e500 TLBs; TLB0 (fixed 4k page size)
 * is not very useful so we mostly focus on TLB1 (variable page size)
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

#include <rtems.h>
#include <inttypes.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Some routines require or return a index 'key'. This
 * is simply the TLB entry # ORed with E500_SELTLB_0
 * or E500_SELTLB_1 specifying an entry in TLB0 or TLB1,
 * respectively.
 */
typedef int rtems_e500_tlb_idx;
#define E500_SELTLB_0	0x0000
#define E500_SELTLB_1	0x1000

/* Cache the relevant TLB1 entries so that we can
 * make sure the user cannot create conflicting
 * (overlapping) entries.
 * Keep them public for informational purposes.
 */
typedef struct {
	struct {
		uint32_t va_epn: 20;
		uint32_t va_tid: 12;
	}         va;
	uint32_t rpn;
	struct {
		uint32_t     sz:  4;
		uint32_t     ts:  1;
		uint32_t      v:  1;
		uint32_t   perm: 10;
		uint32_t  wimge:  7;
	}        att;
} E500_tlb_va_cache_t;

extern E500_tlb_va_cache_t rtems_e500_tlb_va_cache[16];

/*
 * Dump (cleartext) content info from cached TLB entries
 * to a file (stdout if f==NULL).
 */
void
rtems_e500_dmptlbc(FILE *f);

/*
 * Read a TLB entry from the hardware; if it is a TLB1 entry
 * then the current settings are stored in the
 * rtems_e500_tlb_va_cache[] structure.
 *
 * The routine can perform this operation quietly or
 * print information to a file.
 *
 *   'key': TLB entry index ORed with selector bit
 *          (E500_SELTLB_0 for TLB0, E500_SELTLB_1 for TLB1).
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
rtems_e500_prtlb(rtems_e500_tlb_idx key, int quiet, FILE *f);

/* Initialize cache; verify that TLB0 is unused;
 *
 * RETURNS: zero on success, nonzero on error (TLB0
 *          seems to be in use); in this case the
 *          driver will refuse to change TLB1 entries
 *          (other than disabling them).
 */
int
rtems_e500_initlb(void);

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
#define E500_TLB_ATTR_PERM(x)   	(((x)&0x3ff)<<8)
#define E500_TLB_ATTR_PERM_GET(x)   (((x)>>8)&0x3ff)
#define E500_TLB_ATTR_TID(x)        (((x)&0xfff)<<20)
#define E500_TLB_ATTR_TID_GET(x)    (((x)>>20)&0xfff)

int
rtems_e500_wrtlb(int idx, uint32_t ea, uint32_t pa, int sz, uint32_t attr);

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
int
rtems_e500_matchtlb(uint32_t ea, uint32_t tid, int ts, int sz);

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
rtems_e500_ftlb(uint32_t ea, int as);

/* Mark TLB entry as invalid ('disabled'). Unlike
 * rtems_e500_wrtlb() with a negative size argument
 * this routine also can disable TLB0 entries.
 *
 * 'key': TLB entry index ORed with selector bit
 *        (E500_SELTLB_0 for TLB0, E500_SELTLB_1 for TLB1).
 *
 * RETURNS: zero on success, nonzero on error (TLB
 *          unchanged).
 *
 * NOTE:  If a TLB1 entry is disabled the associated
 *        entry in rtems_e500_va_cache[] is also
 *        marked as disabled.
 */
int
rtems_e500_clrtlb(rtems_e500_tlb_idx key);

#ifdef __cplusplus
};
#endif

#endif

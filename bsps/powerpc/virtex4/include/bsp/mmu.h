#ifndef RTEMS_VIRTEX4_MMU_H
#define RTEMS_VIRTEX4_MMU_H
/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCVirtex4MMU
 *
 * @brief Routines to manipulate the PPC 405 MMU.
 */
/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
 *       Stanford Linear Accelerator Center, Stanford University.
 * and was transcribed for the PPC 405 by
 *     R. Claus <claus@slac.stanford.edu>, 2012,
 *       Stanford Linear Accelerator Center, Stanford University,
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * This software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 *     under Contract DE-AC03-76SFO0515 with the Department of Energy.
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

/**
 * @defgroup Virtex4MMU Virtex 4 - MMU Support
 *
 * @ingroup RTEMSBSPsPowerPCVirtex4
 *
 * @brief MMU support.
 *
 * @{
 */

/* Some routines require or return a index 'key'.
 */
typedef int bsp_tlb_idx_t;

/* Cache the relevant TLB entries so that we can make sure the user cannot
 * create conflicting (overlapping) entries. Keep them public for informational
 * purposes.
 */
typedef struct {
  struct {
    uint32_t pad:24;
    uint32_t tid:8;             /** Translation ID */
  }        id;
  struct {
    uint32_t epn:22;            /** Effective page number */
    uint32_t size:3;            /** Page size */
    uint32_t v:1;               /** Valid */
    uint32_t att:2;             /** Little-endian, User-defined */
    uint32_t pad:4;
  }        hi;                  /** High word*/
  struct {
    uint32_t rpn:22;            /** Real page number */
    uint32_t perm:6;            /** Execute enable, Write-enable, Zone select */
    uint32_t wimg:4;            /** Write-through, Caching inhibited, Mem coherent, Guarded */
  }        lo;                  /** Low word */
} bsp_tlb_entry_t;

#define NTLBS  64

extern bsp_tlb_entry_t* bsp_mmu_cache;


// These constants will have to be shifted right by 20 bits before
// being inserted the high word of the TLB.

#define MMU_M_SIZE_1K               (0x00000000U)
#define MMU_M_SIZE_4K               (0x08000000U)
#define MMU_M_SIZE_16K              (0x10000000U)
#define MMU_M_SIZE_64K              (0x18000000U)
#define MMU_M_SIZE_256K             (0x20000000U)
#define MMU_M_SIZE_1M               (0x28000000U)
#define MMU_M_SIZE_4M               (0x30000000U)
#define MMU_M_SIZE_16M              (0x38000000U)
#define MMU_M_SIZE_MIN              (MMU_M_SIZE_1K)
#define MMU_M_SIZE_MAX              (MMU_M_SIZE_16M)
#define MMU_M_SIZE                  (0x38000000U)
#define MMU_V_SIZE                  (27)

#define MMU_M_ATTR_LITTLE_ENDIAN    (0x02000000U)
#define MMU_M_ATTR_USER0            (0x01000000U)
#define MMU_M_ATTR                  (0x03000000U)
#define MMU_V_ATTR                  (24)

// These constants have the same bit positions they'll occupy
// in low word of the TLB.

#define MMU_M_PERM_EXEC             (0x00000200U)
#define MMU_M_PERM_DATA_WRITE       (0x00000100U)
#define MMU_M_PERM_ZONE_SELECT      (0x000000f0U)
#define MMU_M_PERM                  (0x000003f0U)
#define MMU_V_PERM                  (4)

#define MMU_M_PROP_WRITE_THROUGH    (0x00000008U)
#define MMU_M_PROP_UNCACHED         (0x00000004U)
#define MMU_M_PROP_MEM_COHERENT     (0x00000002U)
#define MMU_M_PROP_GUARDED          (0x00000001U)
#define MMU_M_PROP                  (0x0000000fU)
#define MMU_V_PROP                  (0)


/*
 * Dump (cleartext) content info from cached TLB entries
 * to a file (stdout if f==NULL).
 */
void
bsp_mmu_dump_cache(FILE *f);

/* Read a TLB entry from the hardware and store the settings in the
 * bsp_mmu_cache[] structure.
 *
 * The routine can perform this operation quietly or
 * print information to a file.
 *
 *   'key': TLB entry index.
 * 'quiet': perform operation silently (no info printed) if nonzero.
 *     'f': open FILE where to print information. May be NULL, in
 *          which case 'stdout' is used.
 *
 * RETURNS:
 *       0: success; TLB entry is VALID
 *      +1: success but TLB entry is INVALID
 *     < 0: error (-1: invalid argument)
 *                (-2: driver not initialized)
 */
int
bsp_mmu_update(bsp_tlb_idx_t key, bool quiet, FILE *f);

/* Initialize cache.  Should be done only once although this is not enforced.
 *
 * RETURNS: zero on success, nonzero on error; in this case the driver will
 *          refuse to change TLB entries (other than disabling them).
 */
int
bsp_mmu_initialize(void);

/* Find first free TLB entry by examining all entries' valid bit.  The first
 * entry without the valid bit set is returned.
 *
 * RETURNS: A free TLB entry number.  -1 if no entry can be found.
 */
bsp_tlb_idx_t
bsp_mmu_find_first_free(void);

/* Write a TLB entry (can also be used to disable an entry).
 *
 * The routine checks against the cached data in bsp_mmu_cache[]
 * to prevent the user from generating overlapping entries.
 *
 *   'idx': TLB entry # to manipulate
 *    'ea': Effective address (must be page aligned)
 *    'pa': Physical  address (must be page aligned)
 *    'sz': Page size selector; page size is 1024 * 2^(2*sz) bytes.
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
 *             existing mapping in another entry. Return
 *             value gives conflicting entry + 1; i.e.,
 *             if a value of 4 is returned then the request
 *             conflicts with existing mapping in entry 3.
 *         -1: invalid argument
 *         -3: driver not initialized (or initialization failed).
 *         <0: other error
 */
bsp_tlb_idx_t
bsp_mmu_write(bsp_tlb_idx_t idx, uint32_t ea, uint32_t pa, uint sz,
              uint32_t flgs, uint32_t tid);

/* Check if a ea/tid/sz mapping overlaps with an existing entry.
 *
 *    'ea': The Effective Address to match against
 *    'sz': The 'logarithmic' size selector; the page size
 *          is 1024*2^(2*sz).
 *   'tid': The TID to match against
 *
 * RETURNS:
 *     >= 0: index of TLB entry that already provides a mapping
 *           which overlaps within the ea range.
 *       -1: SUCCESS (no conflicting entry found)
 *     <=-2: ERROR (invalid input)
 */
bsp_tlb_idx_t
bsp_mmu_match(uint32_t ea, int sz, uint32_t tid);

/* Find TLB index that maps 'ea/tid' combination
 *
 *    'ea': Effective address to match against
 *   'tid': The TID to match against
 *
 * RETURNS: index 'key'; i.e., the index number.
 *
 *          On error (no mapping) -1 is returned.
 */
bsp_tlb_idx_t
bsp_mmu_find(uint32_t ea, uint32_t tid);

/* Mark TLB entry as invalid ('disabled').
 *
 * 'key': TLB entry index.
 *
 * RETURNS: zero on success, nonzero on error (TLB unchanged).
 *
 * NOTE:  If a TLB entry is disabled the associated
 *        entry in bsp_mmu_cache[] is also marked as disabled.
 */
int
bsp_mmu_invalidate(bsp_tlb_idx_t key);

/** @} */

#ifdef __cplusplus
};
#endif

#endif

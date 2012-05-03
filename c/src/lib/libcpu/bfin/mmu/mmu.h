/*  Blackfin MMU Support
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */


/* NOTE: this currently only implements a static table.  It should be made
   to handle more regions than fit in the CPLBs, with an exception handler
   to do replacements as needed.  This would of course require great care
   to insure any storage required by the exception handler, including any
   stack space, the exception handler itself, and the region descriptors
   it needs to update the CPLBs, are in regions that will never be
   replaced. */

#ifndef _mmu_h_
#define _mmu_h_

#include <libcpu/mmuRegs.h>


#define INSTR_NOCACHE   (ICPLB_DATA_CPLB_USER_RD | \
                         ICPLB_DATA_CPLB_VALID)

#define INSTR_CACHEABLE (ICPLB_DATA_CPLB_L1_CHBL | \
                         ICPLB_DATA_CPLB_USER_RD | \
                         ICPLB_DATA_CPLB_VALID)

#define DATA_NOCACHE   (DCPLB_DATA_CPLB_DIRTY | \
                        DCPLB_DATA_CPLB_SUPV_WR | \
                        DCPLB_DATA_CPLB_USER_WR | \
                        DCPLB_DATA_CPLB_USER_RD | \
                        DCPLB_DATA_CPLB_VALID)

#define DATA_WRITEBACK (DCPLB_DATA_CPLB_L1_AOW | \
                        DCPLB_DATA_CPLB_L1_CHBL | \
                        DCPLB_DATA_CPLB_DIRTY | \
                        DCPLB_DATA_CPLB_SUPV_WR | \
                        DCPLB_DATA_CPLB_USER_WR | \
                        DCPLB_DATA_CPLB_USER_RD | \
                        DCPLB_DATA_CPLB_VALID)


#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
  struct {
    void *address;
    uint32_t flags;
  } instruction[ICPLB_COUNT];
  struct {
    void *address;
    uint32_t flags;
  } data[DCPLB_COUNT];
} bfin_mmu_config_t;


void bfin_mmu_init(bfin_mmu_config_t *config);


#ifdef __cplusplus
}
#endif

#endif /* _mmu_h_ */


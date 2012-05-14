/*  Blackfin MMU Registers
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _mmuRegs_h_
#define _mmuRegs_h_

/* register addresses */
#define DCPLB_ADDR0                               0xffe00100
#define DCPLB_DATA0                               0xffe00200
#define DCPLB_COUNT                                       16
#define DCPLB_ADDR_PITCH                                   4
#define DCPLB_DATA_PITCH                                   4
#define ICPLB_ADDR0                               0xffe01100
#define ICPLB_DATA0                               0xffe01200
#define ICPLB_COUNT                                       16
#define ICPLB_ADDR_PITCH                                   4
#define ICPLB_DATA_PITCH                                   4


/* register fields */
#define DCPLB_DATA_PAGE_SIZE_MASK                 0x00030000
#define DCPLB_DATA_PAGE_SIZE_1KB                  0x00000000
#define DCPLB_DATA_PAGE_SIZE_4KB                  0x00010000
#define DCPLB_DATA_PAGE_SIZE_1MB                  0x00020000
#define DCPLB_DATA_PAGE_SIZE_4MB                  0x00030000
#define DCPLB_DATA_CPLB_L1_AOW                    0x00008000
#define DCPLB_DATA_CPLB_WT                        0x00004000
#define DCPLB_DATA_CPLB_L1_CHBL                   0x00001000
#define DCPLB_DATA_CPLB_DIRTY                     0x00000080
#define DCPLB_DATA_CPLB_SUPV_WR                   0x00000010
#define DCPLB_DATA_CPLB_USER_WR                   0x00000008
#define DCPLB_DATA_CPLB_USER_RD                   0x00000004
#define DCPLB_DATA_CPLB_LOCK                      0x00000002
#define DCPLB_DATA_CPLB_VALID                     0x00000001

#define ICPLB_DATA_PAGE_SIZE_MASK                 0x00030000
#define ICPLB_DATA_PAGE_SIZE_1KB                  0x00000000
#define ICPLB_DATA_PAGE_SIZE_4KB                  0x00010000
#define ICPLB_DATA_PAGE_SIZE_1MB                  0x00020000
#define ICPLB_DATA_PAGE_SIZE_4MB                  0x00030000
#define ICPLB_DATA_CPLB_L1_CHBL                   0x00001000
#define ICPLB_DATA_CPLB_LRUPRIO                   0x00000100
#define ICPLB_DATA_CPLB_USER_RD                   0x00000004
#define ICPLB_DATA_CPLB_LOCK                      0x00000002
#define ICPLB_DATA_CPLB_VALID                     0x00000001

#endif /* _mmuRegs_h_ */


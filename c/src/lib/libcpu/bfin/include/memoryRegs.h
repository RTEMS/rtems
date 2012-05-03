/*  Blackfin Memory Registers
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _memoryRegs_h_
#define _memoryRegs_h_

/* register addresses */
#define DMEM_CONTROL                              0xffe00004
#define DTEST_COMMAND                             0xffe00300
#define DTEST_DATA0                               0xffe00400
#define DTEST_DATA1                               0xffe00404

#define IMEM_CONTROL                              0xffe01004


/* register fields */
#define DMEM_CONTROL_PORT_PREF1                   0x00002000
#define DMEM_CONTROL_PORT_PREF0                   0x00001000
#define DMEM_CONTROL_DCBS                         0x00000010
#define DMEM_CONTROL_DMC_MASK                     0x0000000c
#define DMEM_CONTROL_DMC_SHIFT                             2
#define DMEM_CONTROL_ENDCPLB                      0x00000002

#define DTEST_COMMAND_ACCESS_WAY1                 0x02000000
#define DTEST_COMMAND_ACCESS_INSTRUCTION          0x01000000
#define DTEST_COMMAND_ACCESS_BANKB                0x00800000
#define DTEST_COMMAND_SRAM_ADDR_13_12_MASK        0x00030000
#define DTEST_COMMAND_SRAM_ADDR_13_12_SHIFT               16
#define DTEST_COMMAND_DATA_CACHE_SELECT           0x00004000
#define DTEST_COMMAND_SET_INDEX_MASK              0x000007e0
#define DTEST_COMMAND_SET_INDEX_SHIFT                      5
#define DTEST_COMMAND_DOUBLE_WORD_INDEX_MASK      0x00000018
#define DTEST_COMMAND_DOUBLE_WORD_INDEX_SHIFT              3
#define DTEST_COMMAND_ACCESS_DATA_ARRAY           0x00000004
#define DTEST_COMMAND_WRITE_ACCESS                0x00000002

#define DTEST_DATA0_TAG_19_2_MASK                 0xffffc000
#define DTEST_DATA0_TAG_19_2_SHIFT                        14
#define DTEST_DATA0_TAG                           0x00000800
#define DTEST_DATA0_LRU                           0x00000004
#define DTEST_DATA0_DIRTY                         0x00000002
#define DTEST_DATA0_VALID                         0x00000001

#define IMEM_CONTROL_LRUPRIORST                   0x00002000
#define IMEM_CONTROL_ILOC_MASK                    0x00000078
#define IMEM_CONTROL_ILOC_SHIFT                            3
#define IMEM_CONTROL_IMC                          0x00000004
#define IMEM_CONTROL_ENICPLB                      0x00000002


#endif /* _memoryRegs_h_ */


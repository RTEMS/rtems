/*  Blackfin External Peripheral Interface Registers
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _ebiuRegs_h_
#define _ebiuRegs_h_

/* register addresses */

#define EBIU_AMGCTL             (EBIU_BASE_ADDRESS + 0x0000)
#define EBIU_AMBCTL0            (EBIU_BASE_ADDRESS + 0x0004)
#define EBIU_AMBCTL1            (EBIU_BASE_ADDRESS + 0x0008)
#define EBIU_SDGCTL             (EBIU_BASE_ADDRESS + 0x0010)
#define EBIU_SDBCTL             (EBIU_BASE_ADDRESS + 0x0014)
#define EBIU_SDRRC              (EBIU_BASE_ADDRESS + 0x0018)
#define EBIU_SDSTAT             (EBIU_BASE_ADDRESS + 0x001c)

/* register fields */

#define EBIU_AMGCTL_CDPRIO                            0x0100
#define EBIU_AMGCTL_AMBEN_MASK                        0x000e
#define EBIU_AMGCTL_AMBEN_SHIFT                            1
#define EBIU_AMGCTL_AMCKEN                            0x0001

#define EBIU_AMBCTL0_B1WAT_MASK                   0xf0000000
#define EBIU_AMBCTL0_B1WAT_SHIFT                          28
#define EBIU_AMBCTL0_B1RAT_MASK                   0x0f000000
#define EBIU_AMBCTL0_B1RAT_SHIFT                          24
#define EBIU_AMBCTL0_B1HT_MASK                    0x00c00000
#define EBIU_AMBCTL0_B1HT_SHIFT                           22
#define EBIU_AMBCTL0_B1ST_MASK                    0x00300000
#define EBIU_AMBCTL0_B1ST_SHIFT                           20
#define EBIU_AMBCTL0_B1TT_MASK                    0x000c0000
#define EBIU_AMBCTL0_B1TT_SHIFT                           18
#define EBIU_AMBCTL0_B1RDYPOL                     0x00020000
#define EBIU_AMBCTL0_B1RDYEN                      0x00010000
#define EBIU_AMBCTL0_B0WAT_MASK                   0x0000f000
#define EBIU_AMBCTL0_B0WAT_SHIFT                          12
#define EBIU_AMBCTL0_B0RAT_MASK                   0x00000f00
#define EBIU_AMBCTL0_B0RAT_SHIFT                           8
#define EBIU_AMBCTL0_B0HT_MASK                    0x000000c0
#define EBIU_AMBCTL0_B0HT_SHIFT                            6
#define EBIU_AMBCTL0_B0ST_MASK                    0x00000030
#define EBIU_AMBCTL0_B0ST_SHIFT                            4
#define EBIU_AMBCTL0_B0TT_MASK                    0x0000000c
#define EBIU_AMBCTL0_B0TT_SHIFT                            2
#define EBIU_AMBCTL0_B0RDYPOL                     0x00000002
#define EBIU_AMBCTL0_B0RDYEN                      0x00000001

#define EBIU_AMBCTL1_B3WAT_MASK                   0xf0000000
#define EBIU_AMBCTL1_B3WAT_SHIFT                          28
#define EBIU_AMBCTL1_B3RAT_MASK                   0x0f000000
#define EBIU_AMBCTL1_B3RAT_SHIFT                          24
#define EBIU_AMBCTL1_B3HT_MASK                    0x00c00000
#define EBIU_AMBCTL1_B3HT_SHIFT                           22
#define EBIU_AMBCTL1_B3ST_MASK                    0x00300000
#define EBIU_AMBCTL1_B3ST_SHIFT                           20
#define EBIU_AMBCTL1_B3TT_MASK                    0x000c0000
#define EBIU_AMBCTL1_B3TT_SHIFT                           18
#define EBIU_AMBCTL1_B3RDYPOL                     0x00020000
#define EBIU_AMBCTL1_B3RDYEN                      0x00010000
#define EBIU_AMBCTL1_B2WAT_MASK                   0x0000f000
#define EBIU_AMBCTL1_B2WAT_SHIFT                          12
#define EBIU_AMBCTL1_B2RAT_MASK                   0x00000f00
#define EBIU_AMBCTL1_B2RAT_SHIFT                           8
#define EBIU_AMBCTL1_B2HT_MASK                    0x000000c0
#define EBIU_AMBCTL1_B2HT_SHIFT                            6
#define EBIU_AMBCTL1_B2ST_MASK                    0x00000030
#define EBIU_AMBCTL1_B2ST_SHIFT                            4
#define EBIU_AMBCTL1_B2TT_MASK                    0x0000000c
#define EBIU_AMBCTL1_B2TT_SHIFT                            2
#define EBIU_AMBCTL1_B2RDYPOL                     0x00000002
#define EBIU_AMBCTL1_B2RDYEN                      0x00000001

#define EBIU_SDGCTL_CDDBG                         0x40000000
#define EBIU_SDGCTL_TCSR                          0x20000000
#define EBIU_SDGCTL_EMREN                         0x10000000
#define EBIU_SDGCTL_FBBRW                         0x04000000
#define EBIU_SDGCTL_EBUFE                         0x02000000
#define EBIU_SDGCTL_SRFS                          0x01000000
#define EBIU_SDGCTL_PSSE                          0x00800000
#define EBIU_SDGCTL_PSM                           0x00400000
#define EBIU_SDGCTL_PUPSD                         0x00200000
#define EBIU_SDGCTL_TWR_MASK                      0x00180000
#define EBIU_SDGCTL_TWR_SHIFT                             19
#define EBIU_SDGCTL_TRCD_MASK                     0x00038000
#define EBIU_SDGCTL_TRCD_SHIFT                            15
#define EBIU_SDGCTL_TRP_MASK                      0x00003800
#define EBIU_SDGCTL_TRP_SHIFT                             11
#define EBIU_SDGCTL_TRAS_MASK                     0x000003c0
#define EBIU_SDGCTL_TRAS_SHIFT                             6
#define EBIU_SDGCTL_PASR_MASK                     0x00000030
#define EBIU_SDGCTL_PASR_ALL                      0x00000000
#define EBIU_SDGCTL_PASR_0_1                      0x00000010
#define EBIU_SDGCTL_PASR_0                        0x00000020
#define EBIU_SDGCTL_CL_MASK                       0x0000000c
#define EBIU_SDGCTL_CL_SHIFT                               2
#define EBIU_SDGCTL_SCTLE                         0x00000001

#define EBIU_SDBCTL_EBCAW_MASK                        0x0030
#define EBIU_SDBCTL_SHIFT                                  4
#define EBIU_SDBCTL_EBCAW_8                           0x0000
#define EBIU_SDBCTL_EBCAW_9                           0x0010
#define EBIU_SDBCTL_EBCAW_10                          0x0020
#define EBIU_SDBCTL_EBCAW_11                          0x0030
#define EBIU_SDBCTL_EBSZ_MASK                         0x000e
#define EBIU_SDBCTL_EBSZ_SHIFT                             1
#define EBIU_SDBCTL_EBSZ_16M                          0x0000
#define EBIU_SDBCTL_EBSZ_32M                          0x0002
#define EBIU_SDBCTL_EBSZ_64M                          0x0004
#define EBIU_SDBCTL_EBSZ_128M                         0x0006
#define EBIU_SDBCTL_EBSZ_256M                         0x0008
#define EBIU_SDBCTL_EBSZ_512M                         0x000a
#define EBIU_SDBCTL_EBE                               0x0001

#define EBIU_SDRRC_RDIV_MASK                          0x0fff
#define EBIU_SDRRC_RDIV_SHIFT                              0

#define EBIU_SDSTAT_BGSTAT                            0x0020
#define EBIU_SDSTAT_SDEASE                            0x0010
#define EBIU_SDSTAT_SDRS                              0x0008
#define EBIU_SDSTAT_SDPUA                             0x0004
#define EBIU_SDSTAT_SDSRA                             0x0002
#define EBIU_SDSTAT_SDCI                              0x0001


#endif /* _ebiuRegs_h_ */


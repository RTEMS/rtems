/*  Blackfin Ethernet Registers
 *
 *  Copyright (c) 2008 Kallisti Labs, Los Gatos, CA, USA
 *             written by Allan Hessenflow <allanh@kallisti.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _ethernetRegs_h_
#define _ethernetRegs_h_

/* register addresses */

#define EMAC_OPMODE_OFFSET                            0x0000
#define EMAC_ADDRLO_OFFSET                            0x0004
#define EMAC_ADDRHI_OFFSET                            0x0008
#define EMAC_HASHLO_OFFSET                            0x000c
#define EMAC_HASHHI_OFFSET                            0x0010
#define EMAC_STAADD_OFFSET                            0x0014
#define EMAC_STADAT_OFFSET                            0x0018
#define EMAC_FLC_OFFSET                               0x001c
#define EMAC_VLAN1_OFFSET                             0x0020
#define EMAC_VLAN2_OFFSET                             0x0024
#define EMAC_WKUP_CTL_OFFSET                          0x002c
#define EMAC_WKUP_FFMSK0_OFFSET                       0x0030
#define EMAC_WKUP_FFMSK1_OFFSET                       0x0034
#define EMAC_WKUP_FFMSK2_OFFSET                       0x0038
#define EMAC_WKUP_FFMSK3_OFFSET                       0x003c
#define EMAC_WKUP_FFCMD_OFFSET                        0x0040
#define EMAC_WKUP_FFOFF_OFFSET                        0x0044
#define EMAC_WKUP_FFCRC01_OFFSET                      0x0048
#define EMAC_WKUP_FFCRC23_OFFSET                      0x004c
#define EMAC_SYSCTL_OFFSET                            0x0060
#define EMAC_SYSTAT_OFFSET                            0x0064
#define EMAC_RX_STAT_OFFSET                           0x0068
#define EMAC_RX_STKY_OFFSET                           0x006c
#define EMAC_RX_IRQE_OFFSET                           0x0070
#define EMAC_TX_STAT_OFFSET                           0x0074
#define EMAC_TX_STKY_OFFSET                           0x0078
#define EMAC_TX_IRQE_OFFSET                           0x007c
#define EMAC_MMC_CTL_OFFSET                           0x0080
#define EMAC_MMC_RIRQS_OFFSET                         0x0084
#define EMAC_MMC_RIRQE_OFFSET                         0x0088
#define EMAC_MMC_TIRQS_OFFSET                         0x008c
#define EMAC_MMC_TIRQE_OFFSET                         0x0090

#define EMAC_RXC_OK_OFFSET                            0x0100
#define EMAC_RXC_FCS_OFFSET                           0x0104
#define EMAC_RXC_ALIGN_OFFSET                         0x0108
#define EMAC_RXC_OCTET_OFFSET                         0x010c
#define EMAC_RXC_DMAOVF_OFFSET                        0x0110
#define EMAC_RXC_UNICST_OFFSET                        0x0114
#define EMAC_RXC_MULTI_OFFSET                         0x0118
#define EMAC_RXC_BROAD_OFFSET                         0x011c
#define EMAC_RXC_LNERRI_OFFSET                        0x0120
#define EMAC_RXC_LNERRO_OFFSET                        0x0124
#define EMAC_RXC_LONG_OFFSET                          0x0128
#define EMAC_RXC_MACCTL_OFFSET                        0x012c
#define EMAC_RXC_OPCODE_OFFSET                        0x0130
#define EMAC_RXC_PAUSE_OFFSET                         0x0134
#define EMAC_RXC_ALLFRM_OFFSET                        0x0138
#define EMAC_RXC_ALLOCT_OFFSET                        0x013c
#define EMAC_RXC_TYPED_OFFSET                         0x0140
#define EMAC_RXC_SHORT_OFFSET                         0x0144
#define EMAC_RXC_EQ64_OFFSET                          0x0148
#define EMAC_RXC_LT128_OFFSET                         0x014c
#define EMAC_RXC_LT256_OFFSET                         0x0150
#define EMAC_RXC_LT512_OFFSET                         0x0154
#define EMAC_RXC_LT1024_OFFSET                        0x0158
#define EMAC_RXC_GE1024_OFFSET                        0x015c

#define EMAC_TXC_OK_OFFSET                            0x0180
#define EMAC_TXC_1COL_OFFSET                          0x0184
#define EMAC_TXC_GT1COL_OFFSET                        0x0188
#define EMAC_TXC_OCTET_OFFSET                         0x018c
#define EMAC_TXC_DEFER_OFFSET                         0x0190
#define EMAC_TXC_LATECL_OFFSET                        0x0194
#define EMAC_TXC_XS_COL_OFFSET                        0x0198
#define EMAC_TXC_DMAUND_OFFSET                        0x019c
#define EMAC_TXC_CRSERR_OFFSET                        0x01a0
#define EMAC_TXC_UNICST_OFFSET                        0x01a4
#define EMAC_TXC_MULTI_OFFSET                         0x01a8
#define EMAC_TXC_BROAD_OFFSET                         0x01ac
#define EMAC_TXC_ES_DFR_OFFSET                        0x01b0
#define EMAC_TXC_MACCTL_OFFSET                        0x01b4
#define EMAC_TXC_ALLFRM_OFFSET                        0x01b8
#define EMAC_TXC_ALLOCT_OFFSET                        0x01bc
#define EMAC_TXC_EQ64_OFFSET                          0x01c0
#define EMAC_TXC_LT128_OFFSET                         0x01c4
#define EMAC_TXC_LT256_OFFSET                         0x01c8
#define EMAC_TXC_LT512_OFFSET                         0x01cc
#define EMAC_TXC_LT1024_OFFSET                        0x01d0
#define EMAC_TXC_GE1024_OFFSET                        0x01d4
#define EMAC_TXC_ABORT_OFFSET                         0x01d8


/* register fields */

#define EMAC_OPMODE_DRO                           0x10000000
#define EMAC_OPMODE_LB                            0x08000000
#define EMAC_OPMODE_FDMODE                        0x04000000
#define EMAC_OPMODE_RMII_10                       0x02000000
#define EMAC_OPMODE_RMII                          0x01000000
#define EMAC_OPMODE_LCTRE                         0x00800000
#define EMAC_OPMODE_DRTY                          0x00400000
#define EMAC_OPMODE_BOLMT_MASK                    0x00300000
#define EMAC_OPMODE_BOLMT_1023                    0x00000000
#define EMAC_OPMODE_BOLMT_255                     0x00100000
#define EMAC_OPMODE_BOLMT_15                      0x00200000
#define EMAC_OPMODE_BOLMT_1                       0x00300000
#define EMAC_OPMODE_DC                            0x00080000
#define EMAC_OPMODE_DTXCRC                        0x00040000
#define EMAC_OPMODE_DTXPAD                        0x00020000
#define EMAC_OPMODE_TE                            0x00010000
#define EMAC_OPMODE_RAF                           0x00001000
#define EMAC_OPMODE_PSF                           0x00000800
#define EMAC_OPMODE_PBF                           0x00000400
#define EMAC_OPMODE_DBF                           0x00000200
#define EMAC_OPMODE_IFE                           0x00000100
#define EMAC_OPMODE_PR                            0x00000080
#define EMAC_OPMODE_PAM                           0x00000040
#define EMAC_OPMODE_HM                            0x00000020
#define EMAC_OPMODE_HU                            0x00000010
#define EMAC_OPMODE_ASTP                          0x00000002
#define EMAC_OPMODE_RE                            0x00000001

#define EMAC_STAADD_PHYAD_MASK                    0x0000f800
#define EMAC_STAADD_PHYAD_SHIFT                           11
#define EMAC_STAADD_REGAD_MASK                    0x000007c0
#define EMAC_STAADD_REGAD_SHIFT                            6
#define EMAC_STAADD_STAIE                         0x00000008
#define EMAC_STAADD_STADISPRE                     0x00000004
#define EMAC_STAADD_STAOP                         0x00000002
#define EMAC_STAADD_STABUSY                       0x00000001

#define EMAC_FLC_FLCPAUSE_MASK                    0xffff0000
#define EMAC_FLC_FLCPAUSE_SHIFT                           16
#define EMAC_FLC_BKPRSEN                          0x00000008
#define EMAC_FLC_PCF                              0x00000004
#define EMAC_FLC_FLCE                             0x00000002
#define EMAC_FLC_FLCBUSY                          0x00000001

#define EMAC_WKUP_CTL_RWKS_MASK                   0x00000f00
#define EMAC_WKUP_CTL_RWKS_SHIFT                           8
#define EMAC_WKUP_CTL_MPKS                        0x00000020
#define EMAC_WKUP_CTL_GUWKE                       0x00000008
#define EMAC_WKUP_CTL_RWKE                        0x00000004
#define EMAC_WKUP_CTL_MPKE                        0x00000002
#define EMAC_WKUP_CTL_CAPWKFRM                    0x00000001

#define EMAC_WKUP_FFCMD_3_TYPE                    0x08000000
#define EMAC_WKUP_FFCMD_3_EN                      0x01000000
#define EMAC_WKUP_FFCMD_2_TYPE                    0x00080000
#define EMAC_WKUP_FFCMD_2_EN                      0x00010000
#define EMAC_WKUP_FFCMD_1_TYPE                    0x00000800
#define EMAC_WKUP_FFCMD_1_EN                      0x00000100
#define EMAC_WKUP_FFCMD_0_TYPE                    0x00000008
#define EMAC_WKUP_FFCMD_0_EN                      0x00000001

#define EMAC_WKUP_FFOFF_3_MASK                    0xff000000
#define EMAC_WKUP_FFOFF_3_SHIFT                           24
#define EMAC_WKUP_FFOFF_2_MASK                    0x00ff0000
#define EMAC_WKUP_FFOFF_2_SHIFT                           16
#define EMAC_WKUP_FFOFF_1_MASK                    0x0000ff00
#define EMAC_WKUP_FFOFF_1_SHIFT                            8
#define EMAC_WKUP_FFOFF_0_MASK                    0x000000ff
#define EMAC_WKUP_FFOFF_0_SHIFT                            0

#define EMAC_WKUP_FFCRC01_1_MASK                  0xffff0000
#define EMAC_WKUP_FFCRC01_1_SHIFT                         16
#define EMAC_WKUP_FFCRC01_0_MASK                  0x0000ffff
#define EMAC_WKUP_FFCRC01_0_SHIFT                          0

#define EMAC_WKUP_FFCRC23_3_MASK                  0xffff0000
#define EMAC_WKUP_FFCRC23_3_SHIFT                         16
#define EMAC_WKUP_FFCRC23_2_MASK                  0x0000ffff
#define EMAC_WKUP_FFCRC23_2_SHIFT                          0

#define EMAC_SYSCTL_MDCDIV_MASK                   0x00003f00
#define EMAC_SYSCTL_MDCDIV_SHIFT                           8
#define EMAC_SYSCTL_TXDWA                         0x00000010
#define EMAC_SYSCTL_RXCKS                         0x00000004
#define EMAC_SYSCTL_RXDWA                         0x00000002
#define EMAC_SYSCTL_PHYIE                         0x00000001

#define EMAC_SYSTAT_STMDONE                       0x00000080
#define EMAC_SYSTAT_TXDMAERR                      0x00000040
#define EMAC_SYSTAT_RXDMAERR                      0x00000020
#define EMAC_SYSTAT_WAKEDET                       0x00000010
#define EMAC_SYSTAT_TXFSINT                       0x00000008
#define EMAC_SYSTAT_RXFSINT                       0x00000004
#define EMAC_SYSTAT_MMCINT                        0x00000002
#define EMAC_SYSTAT_PHYINT                        0x00000001

#define EMAC_RX_STAT_RX_ACCEPT                    0x80000000
#define EMAC_RX_STAT_RX_VLAN2                     0x40000000
#define EMAC_RX_STAT_RX_VLAN1                     0x20000000
#define EMAC_RX_STAT_RX_TYPE                      0x10000000
#define EMAC_RX_STAT_RX_UCTL                      0x08000000
#define EMAC_RX_STAT_RX_CTL                       0x04000000
#define EMAC_RX_STAT_RX_BROAD_MULTI_MASK          0x03000000
#define EMAC_RX_STAT_RX_BROAD_MULTI_ILLEGAL       0x03000000
#define EMAC_RX_STAT_RX_BROAD_MULTI_BROADCAST     0x02000000
#define EMAC_RX_STAT_RX_BROAD_MULTI_GROUP         0x01000000
#define EMAC_RX_STAT_RX_BROAD_MULTI_UNICAST       0x00000000
#define EMAC_RX_STAT_RX_RANGE                     0x00800000
#define EMAC_RX_STAT_RX_LATE                      0x00400000
#define EMAC_RX_STAT_RX_PHY                       0x00200000
#define EMAC_RX_STAT_RX_DMAO                      0x00100000
#define EMAC_RX_STAT_RX_ADDR                      0x00080000
#define EMAC_RX_STAT_RX_FRAG                      0x00040000
#define EMAC_RX_STAT_RX_LEN                       0x00020000
#define EMAC_RX_STAT_RX_CRC                       0x00010000
#define EMAC_RX_STAT_RX_ALIGN                     0x00008000
#define EMAC_RX_STAT_RX_LONG                      0x00004000
#define EMAC_RX_STAT_RX_OK                        0x00002000
#define EMAC_RX_STAT_RX_COMP                      0x00001000
#define EMAC_RX_STAT_RX_FRLEN_MASK                0x000007ff
#define EMAC_RX_STAT_RX_FRLEN_SHIFT                        0

#define EMAC_RX_STKY_RX_ACCEPT                    0x80000000
#define EMAC_RX_STKY_RX_VLAN2                     0x40000000
#define EMAC_RX_STKY_RX_VLAN1                     0x20000000
#define EMAC_RX_STKY_RX_TYPE                      0x10000000
#define EMAC_RX_STKY_RX_UCTL                      0x08000000
#define EMAC_RX_STKY_RX_CTL                       0x04000000
#define EMAC_RX_STKY_RX_BROAD                     0x02000000
#define EMAC_RX_STKY_RX_MULTI                     0x01000000
#define EMAC_RX_STKY_RX_RANGE                     0x00800000
#define EMAC_RX_STKY_RX_LATE                      0x00400000
#define EMAC_RX_STKY_RX_PHY                       0x00200000
#define EMAC_RX_STKY_RX_DMAO                      0x00100000
#define EMAC_RX_STKY_RX_ADDR                      0x00080000
#define EMAC_RX_STKY_RX_FRAG                      0x00040000
#define EMAC_RX_STKY_RX_LEN                       0x00020000
#define EMAC_RX_STKY_RX_CRC                       0x00010000
#define EMAC_RX_STKY_RX_ALIGN                     0x00008000
#define EMAC_RX_STKY_RX_LONG                      0x00004000
#define EMAC_RX_STKY_RX_OK                        0x00002000
#define EMAC_RX_STKY_RX_COMP                      0x00001000

#define EMAC_RX_IRQE_RX_ACCEPT                    0x80000000
#define EMAC_RX_IRQE_RX_VLAN2                     0x40000000
#define EMAC_RX_IRQE_RX_VLAN1                     0x20000000
#define EMAC_RX_IRQE_RX_TYPE                      0x10000000
#define EMAC_RX_IRQE_RX_UCTL                      0x08000000
#define EMAC_RX_IRQE_RX_CTL                       0x04000000
#define EMAC_RX_IRQE_RX_BROAD                     0x02000000
#define EMAC_RX_IRQE_RX_MULTI                     0x01000000
#define EMAC_RX_IRQE_RX_RANGE                     0x00800000
#define EMAC_RX_IRQE_RX_LATE                      0x00400000
#define EMAC_RX_IRQE_RX_PHY                       0x00200000
#define EMAC_RX_IRQE_RX_DMAO                      0x00100000
#define EMAC_RX_IRQE_RX_ADDR                      0x00080000
#define EMAC_RX_IRQE_RX_FRAG                      0x00040000
#define EMAC_RX_IRQE_RX_LEN                       0x00020000
#define EMAC_RX_IRQE_RX_CRC                       0x00010000
#define EMAC_RX_IRQE_RX_ALIGN                     0x00008000
#define EMAC_RX_IRQE_RX_LONG                      0x00004000
#define EMAC_RX_IRQE_RX_OK                        0x00002000
#define EMAC_RX_IRQE_RX_COMP                      0x00001000

#define EMAC_TX_STAT_TX_FRLEN_MASK                0x07ff0000
#define EMAC_TX_STAT_TX_FRLEN_SHIFT                       16
#define EMAC_TX_STAT_TX_RETRY                     0x00008000
#define EMAC_TX_STAT_TX_LOSS                      0x00004000
#define EMAC_TX_STAT_TX_CRS                       0x00002000
#define EMAC_TX_STAT_TX_DEFER                     0x00001000
#define EMAC_TX_STAT_TX_CCNT_MASK                 0x00000f00
#define EMAC_TX_STAT_TX_CCNT_SHIFT                         8
#define EMAC_TX_STAT_TX_MULTI_BROAD_MASK          0x000000c0
#define EMAC_TX_STAT_TX_MULTI_BROAD_ILLEGAL       0x000000c0
#define EMAC_TX_STAT_TX_MULTI_BROAD_GROUP         0x00000080
#define EMAC_TX_STAT_TX_MULTI_BROAD_BROADCAST     0x00000040
#define EMAC_TX_STAT_TX_MULTI_BROAD_UNICAST       0x00000000
#define EMAC_TX_STAT_TX_EDEFER                    0x00000020
#define EMAC_TX_STAT_TX_DMAU                      0x00000010
#define EMAC_TX_STAT_TX_LATE                      0x00000008
#define EMAC_TX_STAT_TX_ECOLL                     0x00000004
#define EMAC_TX_STAT_TX_OK                        0x00000002
#define EMAC_TX_STAT_TX_COMP                      0x00000001

#define EMAC_TX_STKY_TX_RETRY                     0x00008000
#define EMAC_TX_STKY_TX_LOSS                      0x00004000
#define EMAC_TX_STKY_TX_CRS                       0x00002000
#define EMAC_TX_STKY_TX_DEFER                     0x00001000
#define EMAC_TX_STKY_TX_CCNT_MASK                 0x00000f00
#define EMAC_TX_STKY_TX_CCNT_SHIFT                         8
#define EMAC_TX_STKY_TX_MULTI                     0x00000080
#define EMAC_TX_STKY_TX_BROAD                     0x00000040
#define EMAC_TX_STKY_TX_EDEFER                    0x00000020
#define EMAC_TX_STKY_TX_DMAU                      0x00000010
#define EMAC_TX_STKY_TX_LATE                      0x00000008
#define EMAC_TX_STAT_TX_ECOLL                     0x00000004
#define EMAC_TX_STAT_TX_OK                        0x00000002
#define EMAC_TX_STAT_TX_COMP                      0x00000001

#define EMAC_TX_IRQE_TX_RETRY                     0x00008000
#define EMAC_TX_IRQE_TX_LOSS                      0x00004000
#define EMAC_TX_IRQE_TX_CRS                       0x00002000
#define EMAC_TX_IRQE_TX_DEFER                     0x00001000
#define EMAC_TX_IRQE_TX_CCNT_MASK                 0x00000f00
#define EMAC_TX_IRQE_TX_CCNT_SHIFT                         8
#define EMAC_TX_IRQE_TX_MULTI                     0x00000080
#define EMAC_TX_IRQE_TX_BROAD                     0x00000040
#define EMAC_TX_IRQE_TX_EDEFER                    0x00000020
#define EMAC_TX_IRQE_TX_DMAU                      0x00000010
#define EMAC_TX_IRQE_TX_LATE                      0x00000008
#define EMAC_TX_IRQE_TX_ECOLL                     0x00000004
#define EMAC_TX_IRQE_TX_OK                        0x00000002
#define EMAC_TX_IRQE_TX_COMP                      0x00000001

#define EMAC_MMC_RIRQS_RX_GE1024_CNT              0x00800000
#define EMAC_MMC_RIRQS_RX_LT1024_CNT              0x00400000
#define EMAC_MMC_RIRQS_RX_LT512_CNT               0x00200000
#define EMAC_MMC_RIRQS_RX_LT256_CNT               0x00100000
#define EMAC_MMC_RIRQS_RX_LT128_CNT               0x00080000
#define EMAC_MMC_RIRQS_RX_EQ64_CNT                0x00040000
#define EMAC_MMC_RIRQS_RX_SHORT_CNT               0x00020000
#define EMAC_MMC_RIRQS_RX_TYPED_CNT               0x00010000
#define EMAC_MMC_RIRQS_RX_ALLO_CNT                0x00008000
#define EMAC_MMC_RIRQS_RX_ALLF_CNT                0x00004000
#define EMAC_MMC_RIRQS_RX_PAUSE_CNT               0x00002000
#define EMAC_MMC_RIRQS_RX_OPCODE_CNT              0x00001000
#define EMAC_MMC_RIRQS_RX_MACCTL_CNT              0x00000800
#define EMAC_MMC_RIRQS_RX_LONG_CNT                0x00000400
#define EMAC_MMC_RIRQS_RX_ORL_CNT                 0x00000200
#define EMAC_MMC_RIRQS_RX_IRL_CNT                 0x00000100
#define EMAC_MMC_RIRQS_RX_BROAD_CNT               0x00000080
#define EMAC_MMC_RIRQS_RX_MULTI_CNT               0x00000040
#define EMAC_MMC_RIRQS_RX_UNI_CNT                 0x00000020
#define EMAC_MMC_RIRQS_RX_LOST_CNT                0x00000010
#define EMAC_MMC_RIRQS_RX_OCTET_CNT               0x00000008
#define EMAC_MMC_RIRQS_RX_ALIGN_CNT               0x00000004
#define EMAC_MMC_RIRQS_RX_FCS_CNT                 0x00000002
#define EMAC_MMC_RIRQS_RX_OK_CNT                  0x00000001

#define EMAC_MMC_RIRQE_RX_GE1024_CNT              0x00800000
#define EMAC_MMC_RIRQE_RX_LT1024_CNT              0x00400000
#define EMAC_MMC_RIRQE_RX_LT512_CNT               0x00200000
#define EMAC_MMC_RIRQE_RX_LT256_CNT               0x00100000
#define EMAC_MMC_RIRQE_RX_LT128_CNT               0x00080000
#define EMAC_MMC_RIRQE_RX_EQ64_CNT                0x00040000
#define EMAC_MMC_RIRQE_RX_SHORT_CNT               0x00020000
#define EMAC_MMC_RIRQE_RX_TYPED_CNT               0x00010000
#define EMAC_MMC_RIRQE_RX_ALLO_CNT                0x00008000
#define EMAC_MMC_RIRQE_RX_ALLF_CNT                0x00004000
#define EMAC_MMC_RIRQE_RX_PAUSE_CNT               0x00002000
#define EMAC_MMC_RIRQE_RX_OPCODE_CNT              0x00001000
#define EMAC_MMC_RIRQE_RX_MACCTL_CNT              0x00000800
#define EMAC_MMC_RIRQE_RX_LONG_CNT                0x00000400
#define EMAC_MMC_RIRQE_RX_ORL_CNT                 0x00000200
#define EMAC_MMC_RIRQE_RX_IRL_CNT                 0x00000100
#define EMAC_MMC_RIRQE_RX_BROAD_CNT               0x00000080
#define EMAC_MMC_RIRQE_RX_MULTI_CNT               0x00000040
#define EMAC_MMC_RIRQE_RX_UNI_CNT                 0x00000020
#define EMAC_MMC_RIRQE_RX_LOST_CNT                0x00000010
#define EMAC_MMC_RIRQE_RX_OCTET_CNT               0x00000008
#define EMAC_MMC_RIRQE_RX_ALIGN_CNT               0x00000004
#define EMAC_MMC_RIRQE_RX_FCS_CNT                 0x00000002
#define EMAC_MMC_RIRQE_RX_OK_CNT                  0x00000001

#define EMAC_MMC_TIRQS_TX_ABORT_CNT               0x00400000
#define EMAC_MMC_TIRQS_TX_GE1024_CNT              0x00200000
#define EMAC_MMC_TIRQS_TX_LT1024_CNT              0x00100000
#define EMAC_MMC_TIRQS_TX_LT512_CNT               0x00080000
#define EMAC_MMC_TIRQS_TX_LT256_CNT               0x00040000
#define EMAC_MMC_TIRQS_TX_LT128_CNT               0x00020000
#define EMAC_MMC_TIRQS_TX_EQ64_CNT                0x00010000
#define EMAC_MMC_TIRQS_TX_ALLO_CNT                0x00008000
#define EMAC_MMC_TIRQS_TX_ALLF_CNT                0x00004000
#define EMAC_MMC_TIRQS_TX_MACCTL_CNT              0x00002000
#define EMAC_MMC_TIRQS_TX_EXDEF_CNT               0x00001000
#define EMAC_MMC_TIRQS_TX_BROAD_CNT               0x00000800
#define EMAC_MMC_TIRQS_TX_MULTI_CNT               0x00000400
#define EMAC_MMC_TIRQS_TX_UNI_CNT                 0x00000200
#define EMAC_MMC_TIRQS_TX_CRS_CNT                 0x00000100
#define EMAC_MMC_TIRQS_TX_LOST_CNT                0x00000080
#define EMAC_MMC_TIRQS_TX_ABORTC_CNT              0x00000040
#define EMAC_MMC_TIRQS_TX_LATE_CNT                0x00000020
#define EMAC_MMC_TIRQS_TX_DEFER_CNT               0x00000010
#define EMAC_MMC_TIRQS_TX_OCTET_CNT               0x00000008
#define EMAC_MMC_TIRQS_TX_MCOLL_CNT               0x00000004
#define EMAC_MMC_TIRQS_TX_SCOLL_CNT               0x00000002
#define EMAC_MMC_TIRQS_TX_OK_CNT                  0x00000001

#define EMAC_MMC_TIRQE_TX_ABORT_CNT               0x00400000
#define EMAC_MMC_TIRQE_TX_GE1024_CNT              0x00200000
#define EMAC_MMC_TIRQE_TX_LT1024_CNT              0x00100000
#define EMAC_MMC_TIRQE_TX_LT512_CNT               0x00080000
#define EMAC_MMC_TIRQE_TX_LT256_CNT               0x00040000
#define EMAC_MMC_TIRQE_TX_LT128_CNT               0x00020000
#define EMAC_MMC_TIRQE_TX_EQ64_CNT                0x00010000
#define EMAC_MMC_TIRQE_TX_ALLO_CNT                0x00008000
#define EMAC_MMC_TIRQE_TX_ALLF_CNT                0x00004000
#define EMAC_MMC_TIRQE_TX_MACCTL_CNT              0x00002000
#define EMAC_MMC_TIRQE_TX_EXDEF_CNT               0x00001000
#define EMAC_MMC_TIRQE_TX_BROAD_CNT               0x00000800
#define EMAC_MMC_TIRQE_TX_MULTI_CNT               0x00000400
#define EMAC_MMC_TIRQE_TX_UNI_CNT                 0x00000200
#define EMAC_MMC_TIRQE_TX_CRS_CNT                 0x00000100
#define EMAC_MMC_TIRQE_TX_LOST_CNT                0x00000080
#define EMAC_MMC_TIRQE_TX_ABORTC_CNT              0x00000040
#define EMAC_MMC_TIRQE_TX_LATE_CNT                0x00000020
#define EMAC_MMC_TIRQE_TX_DEFER_CNT               0x00000010
#define EMAC_MMC_TIRQE_TX_OCTET_CNT               0x00000008
#define EMAC_MMC_TIRQE_TX_MCOLL_CNT               0x00000004
#define EMAC_MMC_TIRQE_TX_SCOLL_CNT               0x00000002
#define EMAC_MMC_TIRQE_TX_OK_CNT                  0x00000001

#define EMAC_MMC_CTL_MMCE                         0x00000008
#define EMAC_MMC_CTL_CCOR                         0x00000004
#define EMAC_MMC_CTL_CROLL                        0x00000002
#define EMAC_MMC_CTL_RSTC                         0x00000001


#endif /* _ethernetRegs_h_ */


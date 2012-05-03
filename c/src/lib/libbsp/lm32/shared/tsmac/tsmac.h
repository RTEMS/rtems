/*
 *  This file contains definitions for LatticeMico32 TSMAC (Tri-Speed MAC)
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Jukka Pietarinen <jukka.pietarinen@mrf.fi>, 2008,
 *  Micro-Research Finland Oy
 */

#ifndef _BSPTSMAC_H
#define _BSPTSMAC_H

/* FIFO Registers */

#define LM32_TSMAC_RX_LEN_FIFO        (0x000)
#define LM32_TSMAC_RX_DATA_FIFO       (0x004)
#define LM32_TSMAC_TX_LEN_FIFO        (0x008)
#define LM32_TSMAC_TX_DATA_FIFO       (0x00C)

/* Control and Status Registers */

#define LM32_TSMAC_VERID              (0x100)
#define LM32_TSMAC_INTR_SRC           (0x104)
#define INTR_RX_SMRY                  (0x00020000)
#define INTR_TX_SMRY                  (0x00010000)
#define INTR_RX_FIFO_FULL             (0x00001000)
#define INTR_RX_ERROR                 (0x00000800)
#define INTR_RX_FIFO_ERROR            (0x00000400)
#define INTR_RX_FIFO_ALMOST_FULL      (0x00000200)
#define INTR_RX_PKT_RDY               (0x00000100)
#define INTR_TX_FIFO_FULL             (0x00000010)
#define INTR_TX_DISCFRM               (0x00000008)
#define INTR_TX_FIFO_ALMOST_EMPTY     (0x00000004)
#define INTR_TX_FIFO_ALMOST_FULL      (0x00000002)
#define INTR_TX_PKT_SENT              (0x00000001)
#define LM32_TSMAC_INTR_ENB           (0x108)
#define INTR_ENB                      (0x00040000)
#define LM32_TSMAC_RX_STATUS          (0x10C)
#define STAT_RX_FIFO_FULL             (0x00000010)
#define STAT_RX_ERROR                 (0x00000008)
#define STAT_RX_FIFO_ERROR            (0x00000004)
#define STAT_RX_FIFO_ALMOST_FULL      (0x00000002)
#define STAT_RX_PKT_RDY               (0x00000001)
#define LM32_TSMAC_TX_STATUS          (0x110)
#define STAT_TX_FIFO_FULL             (0x00000010)
#define STAT_TX_DISCFRM               (0x00000008)
#define STAT_TX_FIFO_ALMOST_EMPTY     (0x00000004)
#define STAT_TX_FIFO_ALMOST_FULL      (0x00000002)
#define STAT_TX_PKT_SENT              (0x00000001)
#define LM32_TSMAC_RX_FRAMES_CNT      (0x114)
#define LM32_TSMAC_TX_FRAMES_CNT      (0x118)
#define LM32_TSMAC_RX_FIFO_TH         (0x11C)
#define LM32_TSMAC_TX_FIFO_TH         (0x120)
#define LM32_TSMAC_SYS_CTL            (0x124)
#define SYS_CTL_TX_FIFO_FLUSH         (0x00000010)
#define SYS_CTL_RX_FIFO_FLUSH         (0x00000008)
#define SYS_CTL_TX_SNDPAUSREQ         (0x00000004)
#define SYS_CTL_TX_FIFOCTRL           (0x00000002)
#define SYS_CTL_IGNORE_NEXT_PKT       (0x00000001)
#define LM32_TSMAC_PAUSE_TMR          (0x128)

/* Tri-Speed MAC Registers */

#define LM32_TSMAC_MAC_REGS_DATA      (0x200)
#define LM32_TSMAC_MAC_REGS_ADDR_RW   (0x204)
#define REGS_ADDR_WRITE               (0x80000000)
#define LM32_TSMAC_MODE_BYTE0         (0x000)
#define MODE_TX_EN                    (1<<3)
#define MODE_RX_EN                    (1<<2)
#define MODE_FC_EN                    (1<<1)
#define MODE_GBIT_EN                  (1<<0)
#define LM32_TSMAC_TX_RX_CTL_BYTE0    (0x002)
#define TX_RX_CTL_RECEIVE_SHORT       (1<<8)
#define TX_RX_CTL_RECEIVE_BRDCST      (1<<7)
#define TX_RX_CTL_DIS_RTRY            (1<<6)
#define TX_RX_CTL_HDEN                (1<<5)
#define TX_RX_CTL_RECEIVE_MLTCST      (1<<4)
#define TX_RX_CTL_RECEIVE_PAUSE       (1<<3)
#define TX_RX_CTL_TX_DIS_FCS          (1<<2)
#define TX_RX_CTL_DISCARD_FCS         (1<<1)
#define TX_RX_CTL_PRMS                (1<<0)
#define LM32_TSMAC_MAX_PKT_SIZE_BYTE0 (0x004)
#define LM32_TSMAC_IPG_VAL_BYTE0      (0x008)
#define LM32_TSMAC_MAC_ADDR_0_BYTE0   (0x00A)
#define LM32_TSMAC_MAC_ADDR_1_BYTE0   (0x00C)
#define LM32_TSMAC_MAC_ADDR_2_BYTE0   (0x00E)
#define LM32_TSMAC_TX_RX_STS_BYTE0    (0x012)
#define TX_RX_STS_RX_IDLE             (1<<10)
#define TX_RX_STS_TAGGED_FRAME        (1<<9)
#define TX_RX_STS_BRDCST_FRAME        (1<<8)
#define TX_RX_STS_MULTCST_FRAME       (1<<7)
#define TX_RX_STS_IPG_SHRINK          (1<<6)
#define TX_RX_STS_SHORT_FRAME         (1<<5)
#define TX_RX_STS_LONG_FRAME          (1<<4)
#define TX_RX_STS_ERROR_FRAME         (1<<3)
#define TX_RX_STS_CRC                 (1<<2)
#define TX_RX_STS_PAUSE_FRAME         (1<<1)
#define TX_RX_STS_TX_IDLE             (1<<0)
#define LM32_TSMAC_GMII_MNG_CTL_BYTE0 (0x014)
#define GMII_MNG_CTL_CMD_FIN          (1<<14)
#define GMII_MNG_CTL_READ_PHYREG      (0)
#define GMII_MNG_CTL_WRITE_PHYREG     (1<<13)
#define GMII_MNG_CTL_PHY_ADD_MASK     (0x001f)
#define GMII_MNG_CTL_PHY_ADD_SHIFT    (8)
#define GMII_MNG_CTL_REG_ADD_MASK     (0x001f)
#define GMII_MNG_CTL_REG_ADD_SHIFT    (0)
#define LM32_TSMAC_GMII_MNG_DAT_BYTE0 (0x016)
#define LM32_TSMAC_MLT_TAB_0_BYTE0    (0x022)
#define LM32_TSMAC_MLT_TAB_1_BYTE0    (0x024)
#define LM32_TSMAC_MLT_TAB_2_BYTE0    (0x026)
#define LM32_TSMAC_MLT_TAB_3_BYTE0    (0x028)
#define LM32_TSMAC_MLT_TAB_4_BYTE0    (0x02A)
#define LM32_TSMAC_MLT_TAB_5_BYTE0    (0x02C)
#define LM32_TSMAC_MLT_TAB_6_BYTE0    (0x02E)
#define LM32_TSMAC_MLT_TAB_7_BYTE0    (0x030)
#define LM32_TSMAC_VLAN_TAG_BYTE0     (0x032)
#define LM32_TSMAC_PAUS_OP_BYTE0      (0x034)

/* Receive Statistics Counters */

#define LM32_TSMAC_RX_PKT_IGNR_CNT    (0x300)
#define LM32_TSMAC_RX_LEN_CHK_ERR_CNT (0x304)
#define LM32_TSMAC_RX_LNG_FRM_CNT     (0x308)
#define LM32_TSMAC_RX_SHRT_FRM_CNT    (0x30C)
#define LM32_TSMAC_RX_IPG_VIOL_CNT    (0x310)
#define LM32_TSMAC_RX_CRC_ERR_CNT     (0x314)
#define LM32_TSMAC_RX_OK_PKT_CNT      (0x318)
#define LM32_TSMAC_RX_CTL_FRM_CNT     (0x31C)
#define LM32_TSMAC_RX_PAUSE_FRM_CNT   (0x320)
#define LM32_TSMAC_RX_MULTICAST_CNT   (0x324)
#define LM32_TSMAC_RX_BRDCAST_CNT     (0x328)
#define LM32_TSMAC_RX_VLAN_TAG_CNT    (0x32C)
#define LM32_TSMAC_RX_PRE_SHRINK_CNT  (0x330)
#define LM32_TSMAC_RX_DRIB_NIB_CNT    (0x334)
#define LM32_TSMAC_RX_UNSUP_OPCD_CNT  (0x338)
#define LM32_TSMAC_RX_BYTE_CNT        (0x33C)

/* Transmit Statistics Counters */

#define LM32_TSMAC_TX_UNICAST_CNT     (0x400)
#define LM32_TSMAC_TX_PAUSE_FRM_CNT   (0x404)
#define LM32_TSMAC_TX_MULTICAST_CNT   (0x408)
#define LM32_TSMAC_TX_BRDCAST_CNT     (0x40C)
#define LM32_TSMAC_TX_VLAN_TAG_CNT    (0x410)
#define LM32_TSMAC_TX_BAD_FCS_CNT     (0x414)
#define LM32_TSMAC_TX_JUMBO_CNT       (0x418)
#define LM32_TSMAC_TX_BYTE_CNT        (0x41C)

#ifdef CPU_U32_FIX
void ipalign(struct mbuf *m);
#endif

#endif /* _BSPTSMAC_H */

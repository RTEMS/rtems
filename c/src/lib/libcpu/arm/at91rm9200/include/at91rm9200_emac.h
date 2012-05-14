/*
 * Atmel AT91RM9200 EMAC Register definitions
 *
 * Copyright (c) 2003 by Cogent Computer Systems
 * Written by Mike Kelly <mike@cogcomp.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#ifndef __AT91RM9200_EMAC_H__
#define __AT91RM9200_EMAC_H__

#include <bits.h>

/*Register offsets */
#define EMAC_CTL        0x00          /* Network Control Register */
#define EMAC_CFG        0x04          /* Network Configuration Register */
#define EMAC_SR         0x08          /* Network Status Register */
#define EMAC_TAR        0x0C          /* Transmit Address Register */
#define EMAC_TCR        0x10          /* Transmit Control Register */
#define EMAC_TSR        0x14          /* Transmit Status Register */
#define EMAC_RBQP       0x18          /* Receive Buffer Queue Pointer */
#define EMAC_RSR        0x20          /* Receive Status Register */
#define EMAC_ISR        0x24          /* Interrupt Enable Register */
#define EMAC_IER        0x28          /* Interrupt Enable Register */
#define EMAC_IDR        0x2C          /* Interrupt Disable Register */
#define EMAC_IMR        0x30          /* Interrupt Mask Register */
#define EMAC_MAN        0x34          /* PHY Maintenance Register */
#define EMAC_FRA        0x40          /* Frames Transmitted OK Register */
#define EMAC_SCOL       0x44          /* Single Collision Frame Register */
#define EMAC_MCOL       0x48          /* Multiple Collision Frame Register */
#define EMAC_OK         0x4C          /* Frames Received OK Register */
#define EMAC_SEQE       0x50          /* Frame Check Sequence Error Register */
#define EMAC_ALE        0x54          /* Alignment Error Register */
#define EMAC_DTE        0x58          /* Deferred Transmission Frame Register */
#define EMAC_LCOL       0x5C          /* Late Collision Register */
#define EMAC_ECOL       0x60          /* Excessive Collision Register */
#define EMAC_CSE        0x64          /* Carrier Sense Error Register */
#define EMAC_TUE        0x68          /* Transmit Underrun Error Register */
#define EMAC_CDE        0x6C          /* Code Error Register */
#define EMAC_ELR        0x70          /* Excessive Length Error Register */
#define EMAC_RJB        0x74          /* Receive Jabber Register */
#define EMAC_USF        0x78          /* Undersize Frame Register */
#define EMAC_SQEE       0x7C          /* SQE Test Error Register */
#define EMAC_DRFC       0x80          /* Discarded RX Frame Register */
#define EMAC_HSH        0x90          /* Hash Address High[63:32] */
#define EMAC_HSL        0x94          /* Hash Address Low[31:0] */
#define EMAC_SA1L       0x98          /* Specific Addr 1 Low, First 4 bytes */
#define EMAC_SA1H       0x9C          /* Specific Addr 1 High, Last 2 bytes */
#define EMAC_SA2L       0xA0          /* Specific Addr 2 Low, First 4 bytes */
#define EMAC_SA2H       0xA4          /* Specific Addr 2 High, Last 2 bytes */
#define EMAC_SA3L       0xA8          /* Specific Addr 3 Low, First 4 bytes */
#define EMAC_SA3H       0xAC          /* Specific Addr 3 High, Last 2 bytes */
#define EMAC_SA4L       0xB0          /* Specific Addr 4 Low, First 4 bytes */
#define EMAC_SA4H       0xB4          /* Specific Addr 4 High, Last 2 bytesr */

/* Control Register, EMAC_CTL, Offset 0x0 */
#define EMAC_CTL_LB     BIT0          /* 1 = Set Loopback output signal */
#define EMAC_CTL_LBL    BIT1          /* 1 = Loopback local.  */
#define EMAC_CTL_RE     BIT2          /* 1 = Receive enable.  */
#define EMAC_CTL_TE     BIT3          /* 1 = Transmit enable.  */
#define EMAC_CTL_MPE    BIT4          /* 1 = Management port enable.  */
#define EMAC_CTL_CSR    BIT5          /* Write 1 to clear stats registers.  */
#define EMAC_CTL_ISR    BIT6          /* Write to increment stats registers */
#define EMAC_CTL_WES    BIT7          /* 1 = Enable writing to stats regs */
#define EMAC_CTL_BP     BIT8          /* 1 = Force collision on all RX frames */

/* Configuration Register, EMAC_CFG, Offset 0x4 */
#define EMAC_CFG_SPD    BIT0          /* 1 = 10/100 Speed (not functional?) */
#define EMAC_CFG_FD     BIT1          /* 1 = Full duplex.  */
#define EMAC_CFG_BR     BIT2          /* write 0  */
#define EMAC_CFG_CAF    BIT4          /* 1 = accept all frames */
#define EMAC_CFG_NBC    BIT5          /* 1 = disable reception of bcast frms */
#define EMAC_CFG_MTI    BIT6          /* 1 = Multicast hash enable */
#define EMAC_CFG_UNI    BIT7          /* 1 = Unicast hash enable.  */
#define EMAC_CFG_BIG    BIT8          /* 1 = enable reception 1522 byte frms */
#define EMAC_CFG_EAE    BIT9          /* write 0 */
#define EMAC_CFG_CLK_8  (0 << 10)     /* MII Clock = HCLK divided by 8 */
#define EMAC_CFG_CLK_16 (1 << 10)     /* MII Clock = HCLK divided by 16 */
#define EMAC_CFG_CLK_32 (2 << 10)     /* MII Clock = HCLK divided by 32 */
#define EMAC_CFG_CLK_64 (3 << 10)     /* MII Clock = HCLK divided by 64 */
#define EMAC_CFG_CLK_MASK (3 << 10)   /* MII Clock mask */
#define EMAC_CFG_RTY    BIT12         /* Retry Test Mode - Must be 0  */
#define EMAC_CFG_RMII   BIT13         /* Reduced MII Mode Enable */

/* Status Register, EMAC_SR, Offset 0x8 */
#define EMAC_LINK       BIT0          /* Link pin  */
#define EMAC_MDIO       BIT1          /* Real Time state of MDIO pin */
#define EMAC_IDLE       BIT2          /* 0 = PHY Logic is idle */

/* Transmit Control Register, EMAC_TCR, Offset 0x10 */
#define EMAC_TCR_LEN(_x_)  ((_x_ & 0x7FF) <<  0) /* Tx frame len minus CRC */
#define EMAC_TCR_NCRC   BIT15                    /* Do'nt append CRC on Tx */

/* Transmit Status Register, EMAC_TSR, Offset 0x14 */
#define EMAC_TSR_OVR    BIT0          /* 1 = Transmit buffer overrun */
#define EMAC_TSR_COL    BIT1          /* 1 = Collision occured */
#define EMAC_TSR_RLE    BIT2          /* 1 = Retry lmimt exceeded */
#define EMAC_TSR_TXIDLE BIT3          /* 1 = Transmitter is idle */
#define EMAC_TSR_BNQ    BIT4          /* 1 = Transmit buffer not queued */
#define EMAC_TSR_COMP   BIT5          /* 1 = Transmit complete */
#define EMAC_TSR_UND    BIT6          /* 1 = Transmit underrun */

/* Receive Status Register, EMAC_RSR, Offset 0x20 */
#define EMAC_RSR_BNA    BIT0          /* 1 = Buffer not available */
#define EMAC_RSR_REC    BIT1          /* 1 = Frame received */
#define EMAC_RSR_OVR    BIT2          /* 1 = Receive overrun */

/*
 * Interrupt Status Register, EMAC_ISR, Offsen 0x24
 * Interrupt Enable Register, EMAC_IER, Offset 0x28
 * Interrupt Disable Register, EMAC_IDR, Offset 0x2c
 * Interrupt Mask Register, EMAC_IMR, Offset 0x30
 */
#define EMAC_INT_DONE   BIT0          /* Phy management done  */
#define EMAC_INT_RCOM   BIT1          /* Receive complete */
#define EMAC_INT_RBNA   BIT2          /* Receive buffer not available */
#define EMAC_INT_TOVR   BIT3          /* Transmit buffer overrun */
#define EMAC_INT_TUND   BIT4          /* Transmit buffer underrun */
#define EMAC_INT_RTRY   BIT5          /* Transmit Retry limt */
#define EMAC_INT_TBRE   BIT6          /* Transmit buffer register empty */
#define EMAC_INT_TCOM   BIT7          /* Transmit complete */
#define EMAC_INT_TIDLE  BIT8          /* Transmit idle */
#define EMAC_INT_LINK   BIT9          /* Link pin changed value */
#define EMAC_INT_ROVR   BIT10         /* Receive overrun */
#define EMAC_INT_ABT    BIT11         /* Abort on DMA transfer */

/* PHY Maintenance Register, EMAC_MAN, Offset 0x34 */
#define EMAC_MAN_DATA(_x_)      ((_x_ & 0xFFFF) <<  0)/* PHY data register */
#define EMAC_MAN_CODE           (0x2 << 16)           /* IEEE Code */
#define EMAC_MAN_REGA(_x_)      ((_x_ & 0x1F) << 18)  /* PHY register address */
#define EMAC_MAN_PHYA(_x_)      ((_x_ & 0x1F) << 23)  /* PHY address */
#define EMAC_MAN_WRITE          (0x1 << 28)           /* Transfer is a write */
#define EMAC_MAN_READ           (0x2 << 28)           /* Transfer is a read */
#define EMAC_MAN_HIGH           BIT30                 /* Must be set */
#define EMAC_MAN_LOW            BIT31

/*
 * Bit assignments for Receive Buffer Descriptor
 * Address - Word 0
 */
#define RXBUF_ADD_BASE_MASK     0xfffffffc    /* Base addr of the rx buf */
#define RXBUF_ADD_WRAP          BIT1          /* set indicates last buf  */
#define RXBUF_ADD_OWNED         BIT0          /* 1 = SW owns the pointer */

/* Status - Word 1 */
#define RXBUF_STAT_BCAST        BIT31         /* Global bcast addr detected */
#define RXBUF_STAT_MULTI        BIT30         /* Multicast hash match */
#define RXBUF_STAT_UNI          BIT29         /* Unicast hash match */
#define RXBUF_STAT_EXT          BIT28         /* External address (optional) */
#define RXBUF_STAT_UNK          BIT27         /* Unknown source address  */
#define RXBUF_STAT_LOC1         BIT26         /* Local address 1 match */
#define RXBUF_STAT_LOC2         BIT25         /* Local address 2 match */
#define RXBUF_STAT_LOC3         BIT24         /* Local address 3 match */
#define RXBUF_STAT_LOC4         BIT23         /* Local address 4 match  */
#define RXBUF_STAT_LEN_MASK     0x7ff         /* Len of frame including FCS */

#endif /* __AT91RM9200_EMAC_H__ */


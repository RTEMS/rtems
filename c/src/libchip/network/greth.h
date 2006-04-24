/*
 * Gaisler Research ethernet MAC driver
 * adapted from Opencores driver by Marko Isomaki
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef _GR_ETH_
#define _GR_ETH_


/* Configuration Information */

typedef struct {
  unsigned32              base_address;
  unsigned32              vector;
  unsigned32              txd_count;
  unsigned32              rxd_count;
} greth_configuration_t;

/* Ethernet configuration registers */

typedef struct _greth_regs {
   volatile unsigned32 ctrl;         /* Ctrl Register */
   volatile unsigned32 status;       /* Status Register */
   volatile unsigned32 mac_addr_msb; /* Bit 47-32 of MAC address */
   volatile unsigned32 mac_addr_lsb; /* Bit 31-0 of MAC address */
   volatile unsigned32 mdio_ctrl;    /* MDIO control and status */
   volatile unsigned32 txdesc;       /* Transmit descriptor pointer */
   volatile unsigned32 rxdesc;       /* Receive descriptor pointer */
} greth_regs;

#define GRETH_TOTAL_BD           128
#define GRETH_MAXBUF_LEN         1520
                                
/* Tx BD */                     
#define GRETH_TXD_ENABLE      0x0800 /* Tx BD Enable */
#define GRETH_TXD_WRAP        0x1000 /* Tx BD Wrap (last BD) */
#define GRETH_TXD_IRQ         0x2000 /* Tx BD IRQ Enable */

#define GRETH_TXD_UNDERRUN    0x4000 /* Tx BD Underrun Status */
#define GRETH_TXD_RETLIM      0x8000 /* Tx BD Retransmission Limit Status */
#define GRETH_TXD_STATS       (GRETH_TXD_UNDERRUN            | \
                               GRETH_TXD_RETLIM)
                                
/* Rx BD */                     
#define GRETH_RXD_ENABLE      0x0800 /* Rx BD Enable */
#define GRETH_RXD_WRAP        0x1000 /* Rx BD Wrap (last BD) */
#define GRETH_RXD_IRQ         0x2000 /* Rx BD IRQ Enable */

#define GRETH_RXD_DRIBBLE     0x4000 /* Rx BD Dribble Nibble Status */                                
#define GRETH_RXD_TOOLONG     0x8000 /* Rx BD Too Long Status */
#define GRETH_RXD_CRCERR      0x10000 /* Rx BD CRC Error Status */
#define GRETH_RXD_OVERRUN     0x20000 /* Rx BD Overrun Status */
#define GRETH_RXD_STATS       (GRETH_RXD_OVERRUN             | \
                               GRETH_RXD_DRIBBLE             | \
                               GRETH_RXD_TOOLONG             | \
                               GRETH_RXD_CRCERR)

/* CTRL Register */
#define GRETH_CTRL_TXEN         0x00000001 /* Transmit Enable */
#define GRETH_CTRL_RXEN         0x00000002 /* Receive Enable  */
#define GRETH_CTRL_TXIRQ        0x00000004 /* Transmit Enable */
#define GRETH_CTRL_RXIRQ        0x00000008 /* Receive Enable  */
#define GRETH_CTRL_FULLD        0x00000010 /* Full Duplex */
#define GRETH_CTRL_PRO          0x00000020 /* Promiscuous (receive all) */
#define GRETH_CTRL_RST          0x00000040 /* Reset MAC */

/* Status Register */
#define GRETH_STATUS_RXERR      0x00000001 /* Receive Error */
#define GRETH_STATUS_TXERR      0x00000002 /* Transmit Error IRQ */
#define GRETH_STATUS_RXIRQ      0x00000004 /* Receive Frame IRQ */
#define GRETH_STATUS_TXIRQ      0x00000008 /* Transmit Error IRQ */
#define GRETH_STATUS_RXAHBERR   0x00000010 /* Receiver AHB Error */
#define GRETH_STATUS_TXAHBERR   0x00000020 /* Transmitter AHB Error */

/* MDIO Control  */
#define GRETH_MDIO_WRITE        0x00000001 /* MDIO Write */
#define GRETH_MDIO_READ         0x00000002 /* MDIO Read */
#define GRETH_MDIO_LINKFAIL     0x00000004 /* MDIO Link failed */
#define GRETH_MDIO_BUSY         0x00000008 /* MDIO Link Busy */
#define GRETH_MDIO_REGADR       0x000007C0 /* Register Address */
#define GRETH_MDIO_PHYADR       0x0000F800 /* PHY address */
#define GRETH_MDIO_DATA         0xFFFF0000 /* MDIO DATA */
 
/* Attach routine */

int rtems_greth_driver_attach (
    struct rtems_bsdnet_ifconfig *config,
    greth_configuration_t *chip
);

/*
#ifdef CPU_U32_FIX
void ipalign(struct mbuf *m);
#endif
*/

#endif


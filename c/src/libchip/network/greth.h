/*
 * Gaisler Research ethernet MAC driver
 * adapted from Opencores driver by Marko Isomaki
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */


#ifndef _GR_ETH_
#define _GR_ETH_


/* Configuration Information */

typedef struct {
  void                 *base_address;
  rtems_vector_number   vector;
  uint32_t              txd_count;
  uint32_t              rxd_count;
} greth_configuration_t;

/* Ethernet configuration registers */

typedef struct _greth_regs {
   volatile uint32_t ctrl;         /* Ctrl Register */
   volatile uint32_t status;       /* Status Register */
   volatile uint32_t mac_addr_msb; /* Bit 47-32 of MAC address */
   volatile uint32_t mac_addr_lsb; /* Bit 31-0 of MAC address */
   volatile uint32_t mdio_ctrl;    /* MDIO control and status */
   volatile uint32_t txdesc;       /* Transmit descriptor pointer */
   volatile uint32_t rxdesc;       /* Receive descriptor pointer */
} greth_regs;

#define GRETH_TOTAL_BD           128
#define GRETH_MAXBUF_LEN         1520

/* Tx BD */
#define GRETH_TXD_ENABLE      0x0800 /* Tx BD Enable */
#define GRETH_TXD_WRAP        0x1000 /* Tx BD Wrap (last BD) */
#define GRETH_TXD_IRQ         0x2000 /* Tx BD IRQ Enable */
#define GRETH_TXD_MORE        0x20000 /* Tx BD More (more descs for packet) */
#define GRETH_TXD_IPCS        0x40000 /* Tx BD insert ip chksum */
#define GRETH_TXD_TCPCS       0x80000 /* Tx BD insert tcp chksum */
#define GRETH_TXD_UDPCS       0x100000 /* Tx BD insert udp chksum */

#define GRETH_TXD_UNDERRUN    0x4000 /* Tx BD Underrun Status */
#define GRETH_TXD_RETLIM      0x8000 /* Tx BD Retransmission Limit Status */
#define GRETH_TXD_LATECOL     0x10000 /* Tx BD Late Collision */

#define GRETH_TXD_STATS       (GRETH_TXD_UNDERRUN            | \
                               GRETH_TXD_RETLIM              | \
                               GRETH_TXD_LATECOL)

#define GRETH_TXD_CS          (GRETH_TXD_IPCS            | \
                               GRETH_TXD_TCPCS           | \
                               GRETH_TXD_UDPCS)

/* Rx BD */
#define GRETH_RXD_ENABLE      0x0800 /* Rx BD Enable */
#define GRETH_RXD_WRAP        0x1000 /* Rx BD Wrap (last BD) */
#define GRETH_RXD_IRQ         0x2000 /* Rx BD IRQ Enable */

#define GRETH_RXD_DRIBBLE     0x4000 /* Rx BD Dribble Nibble Status */
#define GRETH_RXD_TOOLONG     0x8000 /* Rx BD Too Long Status */
#define GRETH_RXD_CRCERR      0x10000 /* Rx BD CRC Error Status */
#define GRETH_RXD_OVERRUN     0x20000 /* Rx BD Overrun Status */
#define GRETH_RXD_LENERR      0x40000 /* Rx BD Length Error */
#define GRETH_RXD_ID          0x40000 /* Rx BD IP Detected */
#define GRETH_RXD_IR          0x40000 /* Rx BD IP Chksum Error */
#define GRETH_RXD_UD          0x40000 /* Rx BD UDP Detected*/
#define GRETH_RXD_UR          0x40000 /* Rx BD UDP Chksum Error */
#define GRETH_RXD_TD          0x40000 /* Rx BD TCP Detected */
#define GRETH_RXD_TR          0x40000 /* Rx BD TCP Chksum Error */


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


/* MII registers */
#define GRETH_MII_EXTADV_1000FD 0x00000200
#define GRETH_MII_EXTADV_1000HD 0x00000100
#define GRETH_MII_EXTPRT_1000FD 0x00000800
#define GRETH_MII_EXTPRT_1000HD 0x00000400

#define GRETH_MII_100T4         0x00000200
#define GRETH_MII_100TXFD       0x00000100
#define GRETH_MII_100TXHD       0x00000080
#define GRETH_MII_10FD          0x00000040
#define GRETH_MII_10HD          0x00000020



/* Attach routine */

int rtems_greth_driver_attach (
    struct rtems_bsdnet_ifconfig *config,
    greth_configuration_t *chip
);

/* PHY data */
struct phy_device_info
{
   int vendor;
   int device;
   int rev;

   int adv;
   int part;

   int extadv;
   int extpart;
};

/*
#ifdef CPU_U32_FIX
void ipalign(struct mbuf *m);
#endif

*/
#endif


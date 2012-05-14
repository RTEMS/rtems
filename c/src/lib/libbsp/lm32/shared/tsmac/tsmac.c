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

#define _KERNEL

#include <rtems.h>
#include <bsp.h>
#include <stdio.h>
#include <errno.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>

#include <net/if.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>

#include "../include/system_conf.h"
#include "tsmac.h"
#include "dp83848phy.h"

struct tsmac_softc {
  struct arpcom arpcom;
  void *ioaddr;
  rtems_id rxDaemonTid;
  rtems_id txDaemonTid;

  /*
   * Statistics
   */
  int rxInterrupts;
  int rxPktIgnore;
  int rxLenCheckError;
  int rxLongFrame;
  int rxShortFrame;
  int rxIPGViolation;
  int rxCRCError;
  int rxOKPackets;
  int rxControlFrame;
  int rxPauseFrame;
  int rxMulticast;
  int rxBroadcast;
  int rxVLANTag;
  int rxPreShrink;
  int rxDribNib;
  int rxUnsupOPCD;
  int rxByteCnt;
  int rxFifoFull;

  int txInterrupts;
  int txUnicast;
  int txPauseFrame;
  int txMulticast;
  int txBroadcast;
  int txVLANTag;
  int txBadFCS;
  int txJumboCnt;
  int txByteCnt;
  int txLostCarrier;
  int txFifoFull;
};

/*
 * Macros to access tsmac wrapper registers.
 */

static inline uint32_t tsmacread(unsigned int reg)
{
  return *((uint32_t *)(TS_MAC_CORE_BASE_ADDRESS + reg));
}

static inline void tsmacwrite(unsigned int reg, uint32_t value)
{
  *((uint32_t *)(TS_MAC_CORE_BASE_ADDRESS + reg)) = value;
}

/*
 * tsmac is a wishbone to MAC wrapper.
 * The macros below access to MAC registers.
 */

static inline uint16_t tsmacregread(unsigned int reg)
{
  tsmacwrite(LM32_TSMAC_MAC_REGS_ADDR_RW, reg);
  return *((uint16_t *)(TS_MAC_CORE_BASE_ADDRESS + LM32_TSMAC_MAC_REGS_DATA + 2));
}

static inline void tsmacregwrite(unsigned int reg, uint16_t value)
{
  *((uint16_t *)(TS_MAC_CORE_BASE_ADDRESS + LM32_TSMAC_MAC_REGS_DATA + 2)) = value;
  tsmacwrite(LM32_TSMAC_MAC_REGS_ADDR_RW, REGS_ADDR_WRITE | reg);
}

/*
#define DEBUG 1
*/

/* We support one interface */

#define TSMAC_NUM  1
#define TSMAC_NAME "TSMAC"
#define TSMAC_MAC0 0x00
#define TSMAC_MAC1 0x0E
#define TSMAC_MAC2 0xB2
#define TSMAC_MAC3 0x00
#define TSMAC_MAC4 0x00
#define TSMAC_MAC5 0x01

/*
 *  The interrupt vector number associated with the tsmac device
 *  driver.
 */

#define TSMAC_VECTOR    ( TS_MAC_CORE_IRQ )
#define TSMAC_IRQMASK   ( 1 << TSMAC_VECTOR )

rtems_isr tsmac_interrupt_handler(rtems_vector_number vector);

extern rtems_isr_entry set_vector(rtems_isr_entry handler,
				 rtems_vector_number vector, int type);

/*
 * Macros to access PHY registers through the (G)MII
 */

uint16_t tsmacphyread(unsigned int reg)
{
  tsmacregwrite(LM32_TSMAC_GMII_MNG_CTL_BYTE0,
		((DEFAULT_PHY_ADDRESS & GMII_MNG_CTL_PHY_ADD_MASK) <<
		 GMII_MNG_CTL_PHY_ADD_SHIFT) |
		((reg & GMII_MNG_CTL_REG_ADD_MASK) <<
		 GMII_MNG_CTL_REG_ADD_SHIFT) |
		GMII_MNG_CTL_READ_PHYREG);

  /* Wait for management interface to be ready */
  while(!(tsmacregread(LM32_TSMAC_GMII_MNG_CTL_BYTE0) & GMII_MNG_CTL_CMD_FIN));

  return tsmacregread(LM32_TSMAC_GMII_MNG_DAT_BYTE0);
}

void tsmacphywrite(unsigned int reg, uint16_t value)
{
  tsmacregwrite(LM32_TSMAC_GMII_MNG_DAT_BYTE0, value);
  tsmacregwrite(LM32_TSMAC_GMII_MNG_CTL_BYTE0,
		((DEFAULT_PHY_ADDRESS & GMII_MNG_CTL_PHY_ADD_MASK) <<
		 GMII_MNG_CTL_PHY_ADD_SHIFT) |
		((reg & GMII_MNG_CTL_REG_ADD_MASK) <<
		 GMII_MNG_CTL_REG_ADD_SHIFT) |
		GMII_MNG_CTL_WRITE_PHYREG);

  /* Wait for management interface to be ready */
  while(!(tsmacregread(LM32_TSMAC_GMII_MNG_CTL_BYTE0) & GMII_MNG_CTL_CMD_FIN));
}

/*
 * Event definitions
 */
#define INTERRUPT_EVENT      RTEMS_EVENT_1
#define START_TRANSMIT_EVENT RTEMS_EVENT_2

static struct tsmac_softc tsmac_softc[TSMAC_NUM];

#ifdef CPU_U32_FIX

/*
 * Routine to align the received packet so that the ip header
 * is on a 32-bit boundary. Necessary for cpu's that do not
 * allow unaligned loads and stores and when the 32-bit DMA
 * mode is used.
 *
 * Transfers are done on word basis to avoid possibly slow byte
 * and half-word writes.
 *
 * Copied over from sonic.c driver
 */

void ipalign(struct mbuf *m)
{
  unsigned int *first, *last, data;
  unsigned int tmp = 0;

  if ((((int) m->m_data) & 2) && (m->m_len)) {
    last = (unsigned int *) ((((int) m->m_data) + m->m_len + 8) & ~3);
    first = (unsigned int *) (((int) m->m_data) & ~3);
    tmp = *first << 16;
    first++;
    do {
      data = *first;
      *first = tmp | (data >> 16);
      tmp = data << 16;
      first++;
    } while (first <= last);

    m->m_data = (caddr_t)(((int) m->m_data) + 2);
  }
}
#endif

/*
 * Receive task
 */
static void tsmac_rxDaemon(void *arg)
{
  struct tsmac_softc *tsmac = (struct tsmac_softc *) arg;
  struct ifnet *ifp = &tsmac->arpcom.ac_if;
  rtems_event_set events;
  int rxq, count, len, data;

#ifdef DEBUG
  printk(TSMAC_NAME ": tsmac_rxDaemon\n");
#endif

  for(;;)
    {
      rtems_bsdnet_event_receive( RTEMS_ALL_EVENTS,
				  RTEMS_WAIT | RTEMS_EVENT_ANY,
				  RTEMS_NO_TIMEOUT,
				  &events);

#ifdef DEBUG
      printk(TSMAC_NAME ": tsmac_rxDaemon wakeup\n");
#endif

      for (;;)
	{
	  struct mbuf* m;
	  struct ether_header* eh;
	  uint32_t *buf;

	  /* Get number of RX frames in RX FIFO */
	  rxq = tsmacread(LM32_TSMAC_RX_FRAMES_CNT);

	  if (rxq == 0)
	    break;

	  /* Get lenght of frame */
	  len = tsmacread(LM32_TSMAC_RX_LEN_FIFO);
#ifdef DEBUG
	  printk(TSMAC_NAME ": Frames %d, len 0x%04x (%d)\n",
		 rxq, len, len);
#endif

	  /*
	   * Get memory for packet
	   */
	  MGETHDR(m, M_WAIT, MT_DATA);
	  MCLGET(m, M_WAIT);

	  m->m_pkthdr.rcvif = ifp;

	  buf = (uint32_t *) mtod(m, uint32_t*);
	  for (count = 0; count < len; count += 4)
	    {
	      data = tsmacread(LM32_TSMAC_RX_DATA_FIFO);
	      *buf++ = data;
#ifdef DEBUG
	      printk("%08x ", data);
#endif
	    }
#ifdef DEBUG
	  printk("\n");
#endif

	  m->m_len = m->m_pkthdr.len =
	    len - sizeof(uint32_t) - sizeof(struct ether_header);
	  eh = mtod(m, struct ether_header*);
	  m->m_data += sizeof(struct ether_header);

#ifdef CPU_U32_FIX
      ipalign(m);	/* Align packet on 32-bit boundary */
#endif

	  /* Notify the ip stack that there is a new packet */
	  ether_input(ifp, eh, m);

	  /*
	   * Release RX frame
	   */
	}
    }
}

static unsigned char tsmac_txbuf[2048];

static void tsmac_sendpacket(struct ifnet *ifp, struct mbuf *m)
{
  struct mbuf *nm = m;
  int len = 0, i;
  uint32_t *buf;

#ifdef DEBUG
  printk(TSMAC_NAME ": tsmac_sendpacket\n");
#endif

  do
    {
#ifdef DEBUG
      printk("mbuf: 0x%08x len %03x: ", nm->m_data, nm->m_len);
      for (i = 0; i < nm->m_len; i++)
	{
	  printk("%02x", mtod(nm, unsigned char*)[i]);
	  if (i & 1)
	    printk(" ");
	}
      printk("\n");
#endif

      if (nm->m_len > 0)
	{
	  memcpy(&tsmac_txbuf[len], (char *)nm->m_data, nm->m_len);
	  len += nm->m_len;
	}
    }
  while ((nm = nm->m_next) != 0);

  buf = (uint32_t *) tsmac_txbuf;
  for (i = 0; i < len; i += 4)
    {
#ifdef DEBUG
      printk("%08x", *buf);
#endif
      tsmacwrite(LM32_TSMAC_TX_DATA_FIFO, *buf++);
    }
#ifdef DEBUG
  printk("\n");
#endif

  /*
   * Enqueue TX frame
   */
  tsmacwrite(LM32_TSMAC_TX_LEN_FIFO, len);
}

/*
 * Transmit task
 */
static void tsmac_txDaemon(void *arg)
{
  struct tsmac_softc *tsmac = (struct tsmac_softc *) arg;
  struct ifnet *ifp = &tsmac->arpcom.ac_if;
  struct mbuf *m;
  rtems_event_set events;
  int txq;

#ifdef DEBUG
  printk(TSMAC_NAME ": tsmac_txDaemon\n");
#endif

  for (;;)
    {
      /*
       * Wait for packet
       */
      rtems_bsdnet_event_receive (START_TRANSMIT_EVENT | INTERRUPT_EVENT,
				  RTEMS_EVENT_ANY | RTEMS_WAIT,
				  RTEMS_NO_TIMEOUT, &events);
#ifdef DEBUG
      printk(TSMAC_NAME ": tsmac_txDaemon event\n");
#endif
      for (;;)
	{
	  /*
	   * Here we should read amount of transmit memory available
	   */

	  txq = 2048;

	  if (txq < ifp->if_mtu)
	    {
	      /*
	       * Here we need to enable transmit done IRQ
	       */
#ifdef DEBUG
	      printk(TSMAC_NAME ": TXMA %d < MTU + CW%d\n", txq,
		     ifp->if_mtu);
#endif
	      break;
	    }

	  /*
	   * Get the next mbuf chain to transmit.
	   */
	  IF_DEQUEUE(&ifp->if_snd, m);
#ifdef DEBUG
	  printk(TSMAC_NAME ": mbuf %08x\n", (int) m);
#endif
	  if (!m)
	    break;
	  tsmac_sendpacket(ifp, m);

	  m_freem(m);
	}
      ifp->if_flags &= ~IFF_OACTIVE;
    }
}

/*
 * Initialize TSMAC hardware
 */
void tsmac_init_hardware(struct tsmac_softc *tsmac)
{
  unsigned char *mac_addr;
  int version_id, phyid, stat;

#ifdef DEBUG
  printk(TSMAC_NAME ": tsmac_init_hardware\n");
#endif

  version_id = tsmacread(LM32_TSMAC_VERID);
#ifdef DEBUG
  printk(TSMAC_NAME ": Version ID %08x\n", version_id);
#endif

#ifdef DEBUG
  printk(TSMAC_NAME ": MAC MODE %04x\n", tsmacregread(LM32_TSMAC_MODE_BYTE0));
  printk(TSMAC_NAME ": MAC TX_RX_CTL %04x\n", tsmacregread(LM32_TSMAC_TX_RX_CTL_BYTE0));
  printk(TSMAC_NAME ": MAC MAX_PKT_SIZE %04x\n", tsmacregread(LM32_TSMAC_MAX_PKT_SIZE_BYTE0));
  printk(TSMAC_NAME ": MAC IPG_VAL %04x\n", tsmacregread(LM32_TSMAC_IPG_VAL_BYTE0));
  printk(TSMAC_NAME ": MAC MAC_ADDR0 %04x\n",
	 tsmacregread(LM32_TSMAC_MAC_ADDR_0_BYTE0));
  printk(TSMAC_NAME ": MAC MAC_ADDR1 %04x\n",
	 tsmacregread(LM32_TSMAC_MAC_ADDR_1_BYTE0));
  printk(TSMAC_NAME ": MAC MAC_ADDR2 %04x\n",
	 tsmacregread(LM32_TSMAC_MAC_ADDR_2_BYTE0));
  printk(TSMAC_NAME ": MAC TX_RX_STS %04x\n",
	 tsmacregread(LM32_TSMAC_TX_RX_STS_BYTE0));
#endif

  /*
   * Set our physical address
   */
  mac_addr = tsmac->arpcom.ac_enaddr;
  tsmacregwrite(LM32_TSMAC_MAC_ADDR_0_BYTE0, (mac_addr[0] << 8) | mac_addr[1]);
  tsmacregwrite(LM32_TSMAC_MAC_ADDR_1_BYTE0, (mac_addr[2] << 8) | mac_addr[3]);
  tsmacregwrite(LM32_TSMAC_MAC_ADDR_2_BYTE0, (mac_addr[4] << 8) | mac_addr[5]);

#ifdef DEBUG
  printk(TSMAC_NAME ": After setting MAC address.\n");
  printk(TSMAC_NAME ": MAC MAC_ADDR0 %04x\n",
	 tsmacregread(LM32_TSMAC_MAC_ADDR_0_BYTE0));
  printk(TSMAC_NAME ": MAC MAC_ADDR1 %04x\n",
	 tsmacregread(LM32_TSMAC_MAC_ADDR_1_BYTE0));
  printk(TSMAC_NAME ": MAC MAC_ADDR2 %04x\n",
	 tsmacregread(LM32_TSMAC_MAC_ADDR_2_BYTE0));
#endif

  /*
   * Configure PHY
   */

  phyid = tsmacphyread(PHY_PHYIDR1);
#ifdef DEBUG
  printk(TSMAC_NAME ": PHYIDR1 %08x\n", phyid);
#endif
  phyid = tsmacphyread(PHY_PHYIDR2);
#ifdef DEBUG
  printk(TSMAC_NAME ": PHYIDR2 %08x\n", phyid);
#endif

#ifdef TSMAC_FORCE_10BASET
  /* Force 10baseT mode, no AN, full duplex */
  tsmacphywrite(PHY_BMCR, PHY_BMCR_DUPLEX_MODE);
  stat = tsmacphyread(PHY_BMCR);
#ifdef DEBUG
  printk(TSMAC_NAME ": PHY BMCR %04x, wrote %04x\n", stat,
	 PHY_BMCR_DUPLEX_MODE);
#endif
  stat = tsmacphyread(PHY_BMSR);
#ifdef DEBUG
  printk(TSMAC_NAME ": PHY BMSR %04x\n", stat);
#endif
  /* Support for 10baseT modes only */
  tsmacphywrite(PHY_ANAR, PHY_ANAR_10_FD | PHY_ANAR_10 | PHY_ANAR_SEL_DEF);
  stat = tsmacphyread(PHY_ANAR);
#ifdef DEBUG
  printk(TSMAC_NAME ": PHY ANAR %04x, wrote %04x\n", stat,
	 PHY_ANAR_10_FD | PHY_ANAR_10 | PHY_ANAR_SEL_DEF);
#endif
#endif /* TSMAC_FORCE_10BASET */
  stat = tsmacphyread(PHY_PHYSTS);
#ifdef DEBUG
  printk(TSMAC_NAME ": PHY PHYSTS %04x\n", stat);
#endif

  /* Enable receive and transmit interrupts */
  tsmacwrite(LM32_TSMAC_INTR_ENB, INTR_ENB |
	     INTR_RX_SMRY | INTR_TX_SMRY |
	     INTR_RX_PKT_RDY | INTR_TX_PKT_SENT);
}

/*
 * Initialize and start the device
 */
void tsmac_init(void *arg)
{
  struct tsmac_softc *tsmac = &tsmac_softc[0];
  struct ifnet *ifp = &tsmac->arpcom.ac_if;

#ifdef DEBUG
  printk(TSMAC_NAME ": tsmac_init, tsmac->txDaemonTid = 0x%x\n",
	 tsmac->txDaemonTid);
#endif

  if (tsmac->txDaemonTid == 0)
    {
      /*
       * Initialize hardware
       */
      tsmac_init_hardware(tsmac);

      /*
       * Start driver tasks
       */
      tsmac->txDaemonTid = rtems_bsdnet_newproc ("TSMACtx", 4096,
					       tsmac_txDaemon, tsmac);
      tsmac->rxDaemonTid = rtems_bsdnet_newproc ("TSMACrx", 4096,
					       tsmac_rxDaemon, tsmac);
      /*
       * Setup interrupt handler
       */
      set_vector( tsmac_interrupt_handler, TSMAC_VECTOR, 1 );

      /* Interrupt line for TSMAC */
      lm32_interrupt_unmask(TSMAC_IRQMASK);
   }

  ifp->if_flags |= IFF_RUNNING;

  /*
   * Receive broadcast
   */

  tsmacregwrite(LM32_TSMAC_TX_RX_CTL_BYTE0, TX_RX_CTL_RECEIVE_BRDCST |
		TX_RX_CTL_RECEIVE_PAUSE);

  /*
   * Enable transmitter
   * Flow control enable
   * Enable receiver
   */

  tsmacregwrite(LM32_TSMAC_MODE_BYTE0, MODE_TX_EN | MODE_RX_EN | MODE_FC_EN);

  /*
   * Wake up receive task to receive packets in queue
   */
  rtems_event_send(tsmac->rxDaemonTid, INTERRUPT_EVENT);
}

void tsmac_stop(struct ifnet *ifp)
{
  /*
   * Mask tsmac interrupts
   */
  lm32_interrupt_mask(TSMAC_IRQMASK);

  ifp->if_flags &= ~IFF_RUNNING;

  /*
   * Disable transmitter and receiver
   */
  tsmacregwrite(LM32_TSMAC_MODE_BYTE0, 0);
}

/*
 * Send packet
 */
void tsmac_start(struct ifnet *ifp)
{
  struct tsmac_softc *tsmac = ifp->if_softc;

  rtems_event_send (tsmac->txDaemonTid, START_TRANSMIT_EVENT);
  ifp->if_flags |= IFF_OACTIVE;
}

void tsmac_stats(struct tsmac_softc *tsmac)
{
  /*
   * Update counters from TSMAC MIB counters
   */

  tsmac->rxPktIgnore = tsmacread(LM32_TSMAC_RX_PKT_IGNR_CNT);
  tsmac->rxLenCheckError = tsmacread(LM32_TSMAC_RX_LEN_CHK_ERR_CNT);
  tsmac->rxLongFrame = tsmacread(LM32_TSMAC_RX_LNG_FRM_CNT);
  tsmac->rxShortFrame = tsmacread(LM32_TSMAC_RX_SHRT_FRM_CNT);
  tsmac->rxIPGViolation = tsmacread(LM32_TSMAC_RX_IPG_VIOL_CNT);
  tsmac->rxCRCError = tsmacread(LM32_TSMAC_RX_CRC_ERR_CNT);
  tsmac->rxOKPackets = tsmacread(LM32_TSMAC_RX_OK_PKT_CNT);
  tsmac->rxControlFrame = tsmacread(LM32_TSMAC_RX_CTL_FRM_CNT);
  tsmac->rxPauseFrame = tsmacread(LM32_TSMAC_RX_PAUSE_FRM_CNT);
  tsmac->rxMulticast = tsmacread(LM32_TSMAC_RX_MULTICAST_CNT);
  tsmac->rxBroadcast = tsmacread(LM32_TSMAC_RX_BRDCAST_CNT);
  tsmac->rxVLANTag = tsmacread(LM32_TSMAC_RX_VLAN_TAG_CNT);
  tsmac->rxPreShrink = tsmacread(LM32_TSMAC_RX_PRE_SHRINK_CNT);
  tsmac->rxDribNib = tsmacread(LM32_TSMAC_RX_DRIB_NIB_CNT);
  tsmac->rxUnsupOPCD = tsmacread(LM32_TSMAC_RX_UNSUP_OPCD_CNT);
  tsmac->rxByteCnt = tsmacread(LM32_TSMAC_RX_BYTE_CNT);

  tsmac->txUnicast = tsmacread(LM32_TSMAC_TX_UNICAST_CNT);
  tsmac->txPauseFrame = tsmacread(LM32_TSMAC_TX_PAUSE_FRM_CNT);
  tsmac->txMulticast = tsmacread(LM32_TSMAC_TX_MULTICAST_CNT);
  tsmac->txBroadcast = tsmacread(LM32_TSMAC_TX_BRDCAST_CNT);
  tsmac->txVLANTag = tsmacread(LM32_TSMAC_TX_VLAN_TAG_CNT);
  tsmac->txBadFCS = tsmacread(LM32_TSMAC_TX_BAD_FCS_CNT);
  tsmac->txJumboCnt = tsmacread(LM32_TSMAC_TX_JUMBO_CNT);
  tsmac->txByteCnt = tsmacread(LM32_TSMAC_TX_BYTE_CNT);

  printk("RX Interrupts:    %8d", tsmac->rxInterrupts);
  printk(" RX Len Chk Error: %8d", tsmac->rxLenCheckError);
  printk(" RX Long Frame:    %8d\n", tsmac->rxLongFrame);
  printk("RX Short Frame:   %8d", tsmac->rxShortFrame);
  printk(" RX IPG Violation: %8d", tsmac->rxIPGViolation);
  printk(" RX CRC Errors:    %8d\n", tsmac->rxCRCError);
  printk("RX OK Packets:    %8d", tsmac->rxOKPackets);
  printk(" RX Control Frame: %8d", tsmac->rxControlFrame);
  printk(" RX Pause Frame:   %8d\n", tsmac->rxPauseFrame);
  printk("RX Multicast:     %8d", tsmac->rxMulticast);
  printk(" RX Broadcast:     %8d", tsmac->rxBroadcast);
  printk(" RX VLAN Tag:      %8d\n", tsmac->rxVLANTag);
  printk("RX Pre Shrink:    %8d", tsmac->rxPreShrink);
  printk(" RX Dribb. Nibble: %8d", tsmac->rxDribNib);
  printk(" RX Unsupp. OPCD:  %8d\n", tsmac->rxUnsupOPCD);
  printk("RX Byte Count:    %8d", tsmac->rxByteCnt);
  printk(" RX FIFO Full:     %8d\n", tsmac->rxFifoFull);

  printk("TX Interrupts:    %8d", tsmac->txInterrupts);
  printk(" TX Unicast:       %8d", tsmac->txUnicast);
  printk(" TX Pause Frame:   %8d\n", tsmac->txPauseFrame);
  printk("TX Multicast:     %8d", tsmac->txMulticast);
  printk(" TX Broadcast:     %8d", tsmac->txBroadcast);
  printk(" TX VLAN Tag:      %8d\n", tsmac->txVLANTag);
  printk("TX Bad FSC:       %8d", tsmac->txBadFCS);
  printk(" TX Jumbo Frame:   %8d", tsmac->txJumboCnt);
  printk(" TX Byte Count:    %8d\n", tsmac->txByteCnt);
  printk("TX FIFO Full:     %8d\n", tsmac->txFifoFull);
}

/*
 * TSMAC ioctl handler
 */

int tsmac_ioctl(struct ifnet *ifp, ioctl_command_t command, caddr_t data)
{
  struct tsmac_softc *tsmac = ifp->if_softc;
  int error = 0;

  switch (command) {
  case SIOCGIFADDR:
  case SIOCSIFADDR:
    ether_ioctl (ifp, command, data);
    break;

  case SIOCSIFFLAGS:
    switch (ifp->if_flags & (IFF_UP | IFF_RUNNING)) {
    case IFF_RUNNING:
      tsmac_stop ((struct ifnet *) tsmac);
      break;

    case IFF_UP:
      tsmac_init ((struct ifnet *) tsmac);
      break;

    case IFF_UP | IFF_RUNNING:
      tsmac_stop ((struct ifnet *) tsmac);
      tsmac_init ((struct ifnet *) tsmac);
      break;

    default:
      break;
    }
    break;

  case SIO_RTEMS_SHOW_STATS:
    tsmac_stats (tsmac);
    break;

  default:
    error = EINVAL;
    break;
  }
  return error;
}

/*
 * Attach a TSMAC driver
 */
int rtems_tsmac_driver_attach(struct rtems_bsdnet_ifconfig *config, int attaching)
{
  struct tsmac_softc *tsmac;
  struct ifnet *ifp;
  int mtu, i;
  int unit;
  char *unitName;

  if ((unit = rtems_bsdnet_parse_driver_name(config, &unitName)) < 0)
    {
      printk(TSMAC_NAME ": Driver name parsing failed.\n");
      return 0;
    }

  if ((unit < 0) || (unit >= TSMAC_NUM))
    {
      printk(TSMAC_NAME ": Bad unit number %d.\n", unit);
      return 0;
    }

  tsmac = &tsmac_softc[unit];

  ifp = &tsmac->arpcom.ac_if;
  if (ifp->if_softc != NULL)
    {
      printk(TSMAC_NAME ": Driver already in use.\n");
      return 0;
    }

  /* Base address for TSMAC */
  if (config->bpar == 0)
    {
      printk(TSMAC_NAME ": Using default base address 0x%08x.\n", TS_MAC_CORE_BASE_ADDRESS);
      config->bpar = TS_MAC_CORE_BASE_ADDRESS;
    }
  tsmac->ioaddr = config->bpar;

  /* Hardware address for TSMAC */
  if (config->hardware_address == 0)
    {
      printk(TSMAC_NAME ": Using default hardware address.\n");
      tsmac->arpcom.ac_enaddr[0] = TSMAC_MAC0;
      tsmac->arpcom.ac_enaddr[1] = TSMAC_MAC1;
      tsmac->arpcom.ac_enaddr[2] = TSMAC_MAC2;
      tsmac->arpcom.ac_enaddr[3] = TSMAC_MAC3;
      tsmac->arpcom.ac_enaddr[4] = TSMAC_MAC4;
      tsmac->arpcom.ac_enaddr[5] = TSMAC_MAC5;
    }
  else
    memcpy(tsmac->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);

  printk(TSMAC_NAME ": MAC address ");
  for (i = 0; i < ETHER_ADDR_LEN; i++)
    {
      printk("%02x", tsmac->arpcom.ac_enaddr[i]);
      if (i != ETHER_ADDR_LEN-1)
	printk(":");
      else
	printk("\n");
    }

  if (config->mtu)
    mtu = config->mtu;
  else
    mtu = ETHERMTU;

  /*
   * Set up network interface values
   */
  ifp->if_softc = tsmac;
  ifp->if_unit = unit;
  ifp->if_name = unitName;
  ifp->if_mtu = mtu;
  ifp->if_init = tsmac_init;
  ifp->if_ioctl = tsmac_ioctl;
  ifp->if_start = tsmac_start;
  ifp->if_output = ether_output;
  ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX;

  if (ifp->if_snd.ifq_maxlen == 0)
    ifp->if_snd.ifq_maxlen = ifqmaxlen;

  if_attach(ifp);
  ether_ifattach(ifp);

  return 1;
}

rtems_isr tsmac_interrupt_handler(rtems_vector_number vector)
{
  struct tsmac_softc *tsmac = &tsmac_softc[0];
  uint32_t irq_stat, rx_stat, tx_stat;

  irq_stat = tsmacread(LM32_TSMAC_INTR_SRC);
  if (irq_stat & INTR_RX_PKT_RDY)
    {
      tsmac->rxInterrupts++;
      rtems_event_send(tsmac->rxDaemonTid, INTERRUPT_EVENT);
    }

  if (irq_stat & INTR_TX_PKT_SENT)
    {
      tsmac->txInterrupts++;
      rtems_event_send(tsmac->txDaemonTid, INTERRUPT_EVENT);
    }

  rx_stat = tsmacread(LM32_TSMAC_RX_STATUS);
  if (rx_stat & STAT_RX_FIFO_FULL)
    tsmac->rxFifoFull++;

  tx_stat = tsmacread(LM32_TSMAC_TX_STATUS);
  if (tx_stat & STAT_TX_FIFO_FULL)
    tsmac->txFifoFull++;

  lm32_interrupt_ack(TSMAC_IRQMASK);
}


/*
 *By Yang Xi <hiyangxi@gmail.com>.
 *RTL8019 ethernet driver for Gumstix on SKYEYE simulator
 *Based on NE2000 driver for pc386 bsp
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/irq.h>
#include "wd80x3.h"

#include <stdio.h>
#include <assert.h>
#include <errno.h>

#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>

#include <net/if.h>
#include <net/if_arp.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>


/* Define this to force byte-wide data transfers with the NIC. This
   is needed for boards like the TS-1325 386EX PC, which support only
   an 8-bit PC/104 bus.  Undefine this on a normal PC.*/

#define NE2000_BYTE_TRANSFERS

/* Define this to print debugging messages with printk.  */

/*#define DEBUG_NE2000
  #define DEBUG_NE*/

/* We expect to be able to read a complete packet into an mbuf.  */

#if (MCLBYTES < 1520)
# error "Driver must have MCLBYTES >= 1520"
#endif

/* The 8390 macro definitions in wd80x3.h expect RO to be defined.  */
#define RO 0

/* Minimum size of Ethernet packet.  */
#define ET_MINLEN 60

/* The number of NE2000 devices supported by this driver.  */

#define NNEDRIVER 1

/* RTEMS event number used by the interrupt handler to signal the
   driver task.  This must not be any of the events used by the
   network task synchronization.  */
#define INTERRUPT_EVENT RTEMS_EVENT_1

/* RTEMS event number used to start the transmit daemon.  This must
   not be the same as INTERRUPT_EVENT.  */
#define START_TRANSMIT_EVENT RTEMS_EVENT_2

/* Interrupts we want to handle from the device.  */

#define NE_INTERRUPTS \
  (MSK_PRX | MSK_PTX | MSK_RXE | MSK_TXE | MSK_OVW | MSK_CNT)

/* The size of a page in device memory.  */

#define NE_PAGE_SIZE (256)

/* The first page address in device memory.  */

#define NE_START_PAGE (0x40)

/* The last page address, plus 1.  */

#define NE_STOP_PAGE (0x80)

/* The number of pages used for a single transmit buffer.  This is
   1536 bytes, enough for a full size packet.  */

#define NE_TX_PAGES (6)

/* The number of transmit buffers.  We use two, so we can load one
   packet while the other is being sent.  */

#define NE_TX_BUFS (2)

/* We use the first pages in memory as transmit buffers, and the
   remaining ones as receive buffers.  */

#define NE_FIRST_TX_PAGE (NE_START_PAGE)

#define NE_FIRST_RX_PAGE (NE_FIRST_TX_PAGE + NE_TX_PAGES * NE_TX_BUFS)

/* Data we store for each NE2000 device.  */

struct ne_softc {
  /* The bsdnet information structure.  */
  struct arpcom arpcom;

  /* The interrupt request number.  */
  unsigned int irno;
  /* The base IO port number.  */
  unsigned int port;

  /* Whether we accept broadcasts.  */
  int accept_broadcasts;

  /* The thread ID of the transmit task.   */
  rtems_id tx_daemon_tid;
  /* The thread ID of the receive task.  */
  rtems_id rx_daemon_tid;

  /* Whether we use byte-transfers with the device. */
  bool byte_transfers;

  /* The number of memory buffers which the transmit daemon has loaded
     with data to be sent, but which have not yet been completely
     sent.  */
  int inuse;
  /* The index of the next available transmit memory buffer.  */
  int nextavail;
  /* The index of the next transmit buffer to send.  */
  int nextsend;
  /* Nonzero if the device is currently transmitting a packet.  */
  int transmitting;
  /* The length of the data stored in each transmit buffer.  */
  int sendlen[NE_TX_BUFS];

  /* Set if we have a packet overrun while receiving.  */
  int overrun;
  /* Set if we should resend after an overrun.  */
  int resend;

  /* Statistics.  */
  struct {
    /* Number of packets received.  */
    unsigned long rx_packets;
    /* Number of packets sent.  */
    unsigned long tx_packets;
    /* Number of interrupts.  */
    unsigned long interrupts;
    /* Number of receive acknowledgements.  */
    unsigned long rx_acks;
    /* Number of transmit acknowledgements.  */
    unsigned long tx_acks;
    /* Number of packet overruns.  */
    unsigned long overruns;
    /* Number of frame errors.  */
    unsigned long rx_frame_errors;
    /* Number of CRC errors.  */
    unsigned long rx_crc_errors;
    /* Number of missed packets.  */
    unsigned long rx_missed_errors;
  } stats;
};

/* The list of NE2000 devices on this system.  */

static struct ne_softc ne_softc[NNEDRIVER];

/*
 * receive ring descriptor
 *
 * The National Semiconductor DS8390 Network interface controller uses
 * the following receive ring headers.  The way this works is that the
 * memory on the interface card is chopped up into 256 bytes blocks.
 * A contiguous portion of those blocks are marked for receive packets
 * by setting start and end block #'s in the NIC.  For each packet that
 * is put into the receive ring, one of these headers (4 bytes each) is
 * tacked onto the front. The first byte is a copy of the receiver status
 * register at the time the packet was received.
 */
struct ne_ring
{
	unsigned char rsr;    /* receiver status */
	unsigned char next;   /* pointer to next packet	*/
	unsigned short count; /* bytes in packet (length + 4)	*/
};

/* Forward declarations to avoid warnings */

static void ne_init_irq_handler (struct ne_softc *sc);
static void ne_stop (struct ne_softc *sc);
static void ne_stop_hardware (struct ne_softc *sc);
static void ne_init (void *arg);
static void ne_init_hardware (struct ne_softc *sc);

static void ne_reset(struct ne_softc *sc);
#ifdef DEBUG_NE
static void ne_dump(struct ne_softc *sc);
#endif

/* Read data from an NE2000 device.  Read LEN bytes at ADDR, storing
   them into P.  */

static void
ne_read_data (struct ne_softc *sc, int addr, int len, unsigned char *p)
{
  unsigned int port = sc->port;
  unsigned int dport = port + DATAPORT;
  outport_byte (port + CMDR, MSK_PG0 | MSK_RD2 | MSK_STA);
  outport_byte (port + RBCR0, len);
  outport_byte (port + RBCR1, len >> 8);
  outport_byte (port + RSAR0, addr);
  outport_byte (port + RSAR1, addr >> 8);
  outport_byte (port + CMDR, MSK_PG0 | MSK_RRE | MSK_STA);

  if (sc->byte_transfers)
  {
    unsigned char d;
    while (len > 0)
    {
      inport_byte(dport, d);
      *p++ = d;
      len--;
    }
  }
  else  /* word transfers */
  {
    unsigned short d;
    while (len > 1)
    {
      inport_word(dport, d);
      *p++ = d;
      *p++ = d >> 8;
      len -= 2;
    }
    if (len)
    {
      inport_word(dport, d);
      *p++ = d;
    }
  }

  outport_byte (port + ISR, MSK_RDC);
}

/* Handle the current NE2000 status.  This is called when the device
   signals an interrupt.  It is also called at other times while
   NE2000 interrupts have been disabled.  */

static void
ne_check_status (struct ne_softc *sc, int from_irq_handler)
{
  struct ifnet *ifp = &sc->arpcom.ac_if;
  unsigned int port = sc->port;
  unsigned char status;

  /* It seems that we need to use a loop here, because if the NE2000
     signals an interrupt because packet transmission is complete, and
     then receives a packet while interrupts are disabled, it seems to
     sometimes fail to signal the interrupt for the received packet
     when interrupts are reenabled.  (Based on the behaviour of the
     Realtek 8019AS chip).  */

  /* int count = 0; */
  while (1)
  {
    inport_byte (port + ISR, status);
#ifdef DEBUG_NE2000
    printk ("NE2000 status 0x%x\n", status);
#endif
    if (status == 0)
      break;

    /* ack */
    outport_byte (port + ISR, status);

    /* Check for incoming packet overwrite.  */
    if (status & MSK_OVW)
    {
      ifp->if_timer = 0;
#ifdef DEBUG_NE
      printk("^\n");
#endif
      ++sc->stats.overruns;
      ne_reset(sc);
      /* Reenable device interrupts.  */
      if (from_irq_handler)
        outport_byte(port + IMR, NE_INTERRUPTS);
      return;
    }

    /* Check for transmitted packet.  The transmit daemon may now be
       able to send another packet to the device.  */
    if ((status & (MSK_PTX | MSK_TXE)) != 0)
    {
      ifp->if_timer = 0;
      ++sc->stats.tx_acks;
      --sc->inuse;
      sc->transmitting = 0;
      if (sc->inuse > 0 || (sc->arpcom.ac_if.if_flags & IFF_OACTIVE) != 0)
        rtems_event_send (sc->tx_daemon_tid, START_TRANSMIT_EVENT);
    }

    /* Check for received packet.  */
    if ((status & (MSK_PRX | MSK_RXE)) != 0)
    {
      ++sc->stats.rx_acks;
      rtems_event_send (sc->rx_daemon_tid, INTERRUPT_EVENT);
    }

    /* Check for counter change.  */
    if ((status & MSK_CNT) != 0)
    {
      unsigned char add;
      inport_byte (port + CNTR0, add);
      sc->stats.rx_frame_errors += add;
      inport_byte (port + CNTR1, add);
      sc->stats.rx_crc_errors += add;
      inport_byte (port + CNTR2, add);
      sc->stats.rx_missed_errors += add;
    }

    break;
  }

  outport_byte (port + CMDR, MSK_PG0 | MSK_STA | MSK_RD2);

}

/* Handle an NE2000 interrupt.  */

static void
ne_interrupt_handler (rtems_irq_hdl_param handle)
{
  struct ne_softc *sc = handle;

  if (sc == NULL)
    return;

  ++sc->stats.interrupts;

#ifdef DEBUG_NE
  printk("!");
#endif
  ne_check_status(sc, 1);
}

/* Turn NE2000 interrupts on.  */

static void
ne_interrupt_on (const rtems_irq_connect_data *irq)
{
  struct ne_softc *sc = irq->handle;

#ifdef DEBUG_NE
  printk ("ne_interrupt_on()\n");
#endif
  if (sc != NULL)
    outport_byte (sc->port + IMR, NE_INTERRUPTS);
}

/* Turn NE2000 interrupts off.  See ne_interrupt_on.  */

static void
ne_interrupt_off (const rtems_irq_connect_data *irq)
{
  struct ne_softc *sc = irq->handle;

#ifdef DEBUG_NE
  printk ("ne_interrupt_off()\n");
#endif
  if (sc != NULL)
    outport_byte (sc->port + IMR, 0);
}

/*
 *Return whether NE2000 interrupts are on.
 *If it is eanbled, return 1
*/
static int
ne_interrupt_is_on (const rtems_irq_connect_data *irq)
{
  struct ne_softc *sc = irq->handle;
  unsigned char imr;
#ifdef DEBUG_NE
  printk("ne_interrupt_is_on()\n");
#endif
  if(sc != NULL){
    /*Read IMR in Page2*/
    outport_byte (sc->port + CMDR, MSK_PG2 | MSK_RD2 | MSK_STP);
    inport_byte(sc->port + IMR, imr);
    /*Back Page 0*/
    outport_byte (sc->port + CMDR, MSK_PG0 | MSK_RD2 | MSK_STP);
    if(imr == NE_INTERRUPTS)
      return 1;
  }
  return 0;
}

/* Initialize the NE2000 hardware.  */

static void
ne_init_hardware (struct ne_softc *sc)
{
  unsigned int port = sc->port;
  int i;

#ifdef DEBUG_NE2000
  printk ("ne_init_hardware()\n");
#endif

  /* Initialize registers.  */

  /* Set interface for page 0, Remote DMA complete, Stopped */
  outport_byte (port + CMDR, MSK_PG0 | MSK_RD2 | MSK_STP);

  /* Set FIFO threshold to 8, No auto-init Remote DMA, byte order=80x86 */
  /* byte-wide DMA xfers */
  if (sc->byte_transfers)
    outport_byte (port + DCR, MSK_FT10 | MSK_BMS);
  /* word-wide DMA xfers */
  else
    outport_byte (port + DCR, MSK_FT10 | MSK_BMS | MSK_WTS);

  /* Clear Remote Byte Count Registers */
  outport_byte (port + RBCR0, 0);
  outport_byte (port + RBCR1, 0);

  /* For the moment, don't store incoming packets in memory. */
  outport_byte (port + RCR, MSK_MON);

  /* Place NIC in internal loopback mode */
  outport_byte (port + TCR, MSK_LOOP);

  /* Initialize transmit/receive (ring-buffer) Page Start */
  outport_byte (port + TPSR, NE_FIRST_TX_PAGE);
  outport_byte (port + PSTART, NE_FIRST_RX_PAGE);

  /* Initialize Receiver (ring-buffer) Page Stop and Boundary */
  outport_byte (port + PSTOP, NE_STOP_PAGE);
  outport_byte (port + BNRY, NE_STOP_PAGE - 1);

  /* Clear all interrupts */
  outport_byte (port + ISR, 0xff);
  /* Disable all interrupts */
  outport_byte (port + IMR, 0);

  /* Program Command Register for page 1 */
  outport_byte (port + CMDR, MSK_PG1 | MSK_RD2 | MSK_STP);

  /* Set the Ethernet hardware address.  */
  for (i = 0; i < ETHER_ADDR_LEN; ++i)
    outport_byte (port + PAR + i, sc->arpcom.ac_enaddr[i]);

  /* Set Current Page pointer to next_packet */
  outport_byte (port + CURR, NE_FIRST_RX_PAGE);

  /* Clear the multicast address.  */
  for (i = 0; i < MARsize; ++i)
    outport_byte (port + MAR + i, 0);

  /* Set page 0 registers */
  outport_byte (port + CMDR, MSK_PG0 | MSK_RD2 | MSK_STP);

  /* accept broadcast */
  outport_byte (port + RCR, (sc->accept_broadcasts ? MSK_AB : 0));

  /* Start interface */
  outport_byte (port + CMDR, MSK_PG0 | MSK_RD2 | MSK_STA);

  /* Take interface out of loopback */
  outport_byte (port + TCR, 0);
}

/* Set up interrupts.
*/
static void
ne_init_irq_handler(struct ne_softc *sc)
{
  rtems_irq_connect_data irq;

#ifdef DEBUG_NE
  printk("ne_init_irq_handler(%d)\n", sc->irno);
#endif
  irq.name = sc->irno;
  irq.hdl = ne_interrupt_handler;
  irq.handle = sc;
  irq.on = ne_interrupt_on;
  irq.off = ne_interrupt_off;
  irq.isOn = ne_interrupt_is_on;

  if (!BSP_install_rtems_irq_handler (&irq))
    rtems_panic ("Can't attach NE interrupt handler for irq %d\n", sc->irno);
}

/* The NE2000 packet receive daemon.  This task is started when the
   NE2000 driver is initialized.  */

#ifdef DEBUG_NE
static int ccc = 0; /* experinent! */
#endif

static void
ne_rx_daemon (void *arg)
{
  struct ne_softc *sc = (struct ne_softc *) arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  unsigned int port = sc->port;

  while (1)
  {
    rtems_event_set events;

    /* Wait for the interrupt handler to tell us that there is a
       packet ready to receive.  */
    rtems_bsdnet_event_receive (INTERRUPT_EVENT,
                                RTEMS_WAIT | RTEMS_EVENT_ANY,
                                RTEMS_NO_TIMEOUT,
                                &events);

    /* Don't let the device interrupt us now.  */
    outport_byte (port + IMR, 0);

    while (1)
    {
      unsigned char startpage, currpage;
      unsigned short len;
      unsigned char next, stat, cnt1, cnt2;
      struct mbuf *m = NULL;
      unsigned char *p;
      int startaddr;
      int toend;
      struct ether_header *eh;
      struct ne_ring hdr; /* ring buffer header */
      int reset;

      inport_byte (port + BNRY, startpage);

      outport_byte (port + CMDR, MSK_PG1 | MSK_RD2);
      inport_byte (port + CURR, currpage);
      outport_byte (port + CMDR, MSK_PG0 | MSK_RD2);

      ++startpage;
      if (startpage >= NE_STOP_PAGE)
        startpage = NE_FIRST_RX_PAGE;

      if (startpage == currpage)
        break;

#ifdef DEBUG_NE2000
      printk ("ne_rx_daemon: start page %x; current page %x\n",
              startpage, currpage);
#endif

      reset = 0;

      /* Read the buffer header */
      startaddr = startpage * NE_PAGE_SIZE;
      ne_read_data(sc, startaddr, sizeof(hdr), (unsigned char *)&hdr);
      next = hdr.next;

      if (next >= NE_STOP_PAGE)
        next = NE_FIRST_RX_PAGE;

      /* check packet length */
      len = hdr.count;
      if (currpage < startpage)
        cnt1 = currpage + (NE_STOP_PAGE - NE_FIRST_RX_PAGE) - startpage;
      else
        cnt1 = currpage - startpage;
      cnt2 = len / NE_PAGE_SIZE;
      if (len % NE_PAGE_SIZE)
        cnt2++;
      if (cnt1 < cnt2)
      {
#ifdef DEBUG_NE
        printk("(%x<%x:%x)\n", cnt1, cnt2, len);
/*
        printk("start page 0x%x; current page 0x%x\n",
                startpage, currpage);
        printk("cnt1 < cnt2 (0x%x, 0x%x); len 0x%x\n",
                cnt1, cnt2, len);
*/
#endif
        reset = 1;
      }
      if (len > (ETHER_MAX_LEN - ETHER_CRC_LEN + sizeof(struct ne_ring)) ||
          len < (ETHER_MIN_LEN - ETHER_CRC_LEN + sizeof(struct ne_ring)) ||
          len > MCLBYTES)
      {
#ifdef DEBUG_NE
        printk("(%x)", len);

        printk("start page 0x%x; current page 0x%x\n",
                startpage, currpage);
        printk("len out of range: 0x%x\n", len);
        printk("stat: 0x%x, next: 0x%x\n", hdr.rsr, hdr.next);
#endif
        reset = 1;
      }
#ifdef DEBUG_NE
      if (++ccc == 100)
      { ccc = 0; reset = 1;
        printk("T");
      }
#endif

      /* reset interface */
      if (reset)
      {
	printk("Reset in RX\n");
        ne_reset(sc);
        goto Next;
      }

      stat = hdr.rsr;

      /* The first four bytes of the length are the buffer header
       * Just decrease them by 2 since in ARM, we have to make sure
       * 4bytes memory access align on 4bytes
       */
      len -= 2;
      startaddr += 2;

      MGETHDR (m, M_WAIT, MT_DATA);
      MCLGET (m, M_WAIT);

      if (m == NULL)
	panic ("ne_rx_daemon");

      m->m_pkthdr.rcvif = ifp;
      m->m_nextpkt = 0;

      p = mtod (m, unsigned char *);
      m->m_len = m->m_pkthdr.len = len - (sizeof(struct ether_header) + 2);

      toend = NE_STOP_PAGE * NE_PAGE_SIZE - startaddr;
      if (toend < len)
	{
	  ne_read_data (sc, startaddr, toend, p);
	  p += toend;
	  len -= toend;
	  startaddr = NE_FIRST_RX_PAGE * NE_PAGE_SIZE;
	}

      if (len > 0)
        ne_read_data (sc, startaddr, len, p);

      m->m_data +=2;
      eh = mtod(m, struct ether_header *);
      m->m_data += sizeof (struct ether_header);

#ifdef DEBUG_NE
      /*printk("[r%d]", hdr.count - sizeof(hdr));*/
      printk("<\n");
#endif
      ether_input (ifp, eh, m);
      ++sc->stats.rx_packets;
      outport_byte (port + BNRY, next - 1);
    }

    if (sc->overrun) {
      outport_byte (port + ISR, MSK_OVW);
      outport_byte (port + TCR, 0);
      if (sc->resend)
        outport_byte (port + CMDR, MSK_PG0 | MSK_TXP | MSK_RD2 | MSK_STA);
      sc->resend = 0;
      sc->overrun = 0;
    }

  Next:
    /* Reenable device interrupts.  */
    outport_byte (port + IMR, NE_INTERRUPTS);
  }
}

/* Load an NE2000 packet onto the device.  */

static void
ne_loadpacket (struct ne_softc *sc, struct mbuf *m)
{
  unsigned int port = sc->port;
  unsigned int dport = port + DATAPORT;
  struct mbuf *mhold = m;
  int leftover;
  unsigned char leftover_data;
  int timeout;
  int send_cnt = 0;

#ifdef DEBUG_NE2000
  printk ("Uploading NE2000 packet\n");
#endif

  /* Reset remote DMA complete flag.  */
  outport_byte (port + ISR, MSK_RDC);

  /* Write out the count.  */
  outport_byte (port + RBCR0, m->m_pkthdr.len);
  outport_byte (port + RBCR1, m->m_pkthdr.len >> 8);

  sc->sendlen[sc->nextavail] = m->m_pkthdr.len;

  /* Tell the device which address we want to write to.  */
  outport_byte (port + RSAR0, 0);
  outport_byte (port + RSAR1,
                NE_FIRST_TX_PAGE + (sc->nextavail * NE_TX_PAGES));

  /* Set up the write.  */
  outport_byte (port + CMDR, MSK_PG0 | MSK_RWR | MSK_STA);

  /* Transfer the mbuf chain to device memory.  NE2000 devices require
     that the data be transferred as words, so we need to handle odd
     length mbufs.  Never occurs if we force byte transfers. */

  leftover = 0;
  leftover_data = '\0';

  for (; m != NULL; m = m->m_next) {
    int len;
    unsigned char *data;

    len = m->m_len;
    if (len == 0)
      continue;

    data = mtod (m, unsigned char *);

    if (leftover) {
      unsigned char next;

      /* Data left over from previous mbuf in chain.  */
      next = *data++;
      --len;
      outport_word (dport, leftover_data | (next << 8));
      send_cnt += 2;
      leftover = 0;
    }

    /* If using byte transfers, len always ends up as zero so
       there are no leftovers. */

    if (sc->byte_transfers)
      while (len > 0) {
        outport_byte (dport, *data++);
        len--;
      }
    else
      while (len > 1) {
        outport_word (dport, data[0] | (data[1] << 8));
        data += 2;
        len -= 2;
        send_cnt += 2;
      }

    if (len > 0)
      {
        leftover = 1;
        leftover_data = *data++;
      }
  }

  if (leftover)
  {
    outport_word (dport, leftover_data);
    send_cnt += 2;
  }

#ifdef DEBUG_NE
  /* printk("{l%d|%d}", send_cnt, sc->nextavail); */
  printk("v");
#endif
  m_freem (mhold);

  /* Wait for the device to complete accepting the data, with a
     limiting counter so that we don't wait too long.  */
  for (timeout = 0; timeout < 1000; ++timeout)
    {
      unsigned char status;

      inport_byte (port + ISR, status);

#ifdef DEBUG_NE2000
      if ((status &~ MSK_RDC) != 0)
        printk ("Status 0x%x while waiting for acknowledgement of uploaded packet\n",
                status);
#endif

      if ((status & MSK_RDC) != 0) {
        outport_byte (port + ISR, MSK_RDC);
        break;
      }
    }

  if (timeout >= 1000)
    printk ("Timed out waiting for acknowledgement of uploaded NE2000 packet\n");

  ++sc->nextavail;
  if (sc->nextavail == NE_TX_BUFS)
    sc->nextavail = 0;
}

/* Tell the NE2000 to transmit a buffer whose contents we have already
   loaded onto the device.  */

static void
ne_transmit (struct ne_softc *sc)
{
  struct ifnet *ifp = &sc->arpcom.ac_if;
  unsigned int port = sc->port;
  int len;

#ifdef DEBUG_NE2000
  printk ("Transmitting NE2000 packet\n");
#endif

  len = sc->sendlen[sc->nextsend];
  if (len < ET_MINLEN)
    len = ET_MINLEN;
  outport_byte (port + TBCR0, len);
  outport_byte (port + TBCR1, len >> 8);

  outport_byte (port + TPSR, NE_FIRST_TX_PAGE + (sc->nextsend * NE_TX_PAGES));

  outport_byte (port + CMDR, MSK_PG0 | MSK_TXP | MSK_RD2 | MSK_STA);

#ifdef DEBUG_NE
  /* printk("{s%d|%d}", len, sc->nextsend); */
  printk(">");
#endif
  ++sc->nextsend;
  if (sc->nextsend == NE_TX_BUFS)
    sc->nextsend = 0;

  ++sc->stats.tx_packets;

  /* set watchdog timer */
  ifp->if_timer = 2;
}

/* The NE2000 packet transmit daemon.  This task is started when the
   NE2000 driver is initialized.  */

static void
ne_tx_daemon (void *arg)
{
  struct ne_softc *sc = (struct ne_softc *) arg;
  unsigned int port = sc->port;
  struct ifnet *ifp = &sc->arpcom.ac_if;

  while (1) {
    rtems_event_set events;

    /* Wait for a packet to be ready for sending, or for there to be
       room for another packet in the device memory.  */
    rtems_bsdnet_event_receive (START_TRANSMIT_EVENT,
                                RTEMS_EVENT_ANY | RTEMS_WAIT,
                                RTEMS_NO_TIMEOUT,
                                &events);

#ifdef DEBUG_NE2000
    printk ("ne_tx_daemon\n");
#endif

    /* This daemon handles both uploading data onto the device and
       telling the device to transmit data which has been uploaded.
       These are separate tasks, because while the device is
       transmitting one buffer we will upload another.  */

    /* Don't let the device interrupt us now.  */
    outport_byte (port + IMR, 0);

    while (1) {
      struct mbuf *m;

      /* If the device is not transmitting a packet, and we have
         uploaded a packet, tell the device to transmit it.  */
      if (! sc->transmitting && sc->inuse > 0) {
        sc->transmitting = 1;
        ne_transmit (sc);
      }

      /* If we don't have any more buffers to send, quit now.  */
      if (ifp->if_snd.ifq_head == NULL) {
        ifp->if_flags &= ~IFF_OACTIVE;
        break;
      }

      /* Allocate a buffer to load data into.  If there are none
         available, quit until a buffer has been transmitted.  */
      if (sc->inuse >= NE_TX_BUFS)
        break;

      ++sc->inuse;

      IF_DEQUEUE (&ifp->if_snd, m);
      if (m == NULL)
        panic ("ne_tx_daemon");

      ne_loadpacket (sc, m);

      /* Check the device status.  It may have finished transmitting
         the last packet.  */
      ne_check_status(sc, 0);
    }

    /* Reenable device interrupts.  */
    outport_byte (port + IMR, NE_INTERRUPTS);
  }
}

/* Start sending an NE2000 packet.  */

static void
ne_start (struct ifnet *ifp)
{
  struct ne_softc *sc = ifp->if_softc;

#ifdef DEBUG_NE
  printk("S");
#endif
  /* Tell the transmit daemon to wake up and send a packet.  */
  rtems_event_send (sc->tx_daemon_tid, START_TRANSMIT_EVENT);
  ifp->if_flags |= IFF_OACTIVE;
}

/* Initialize and start and NE2000.  */

static void
ne_init (void *arg)
{
  struct ne_softc *sc = (struct ne_softc *) arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;

#ifdef DEBUG_NE
  printk("ne_init()\n");
  ne_dump(sc);
#endif

  /* only once... */
  if (sc->tx_daemon_tid == 0)
  {
    sc->inuse = 0;
    sc->nextavail = 0;
    sc->nextsend = 0;
    sc->transmitting = 0;

    ne_init_hardware (sc);

    sc->tx_daemon_tid = rtems_bsdnet_newproc ("SCtx", 4096, ne_tx_daemon, sc);
    sc->rx_daemon_tid = rtems_bsdnet_newproc ("SCrx", 4096, ne_rx_daemon, sc);

    /* install rtems irq handler */
    ne_init_irq_handler(sc);
  }

  ifp->if_flags |= IFF_RUNNING;
}

/* Stop an NE2000.  */

static void
ne_stop (struct ne_softc *sc)
{
  sc->arpcom.ac_if.if_flags &= ~IFF_RUNNING;

  ne_stop_hardware(sc);

  sc->inuse = 0;
  sc->nextavail = 0;
  sc->nextsend = 0;
  sc->transmitting = 0;
  sc->overrun = 0;
  sc->resend = 0;
}

static void
ne_stop_hardware (struct ne_softc *sc)
{
  unsigned int port = sc->port;
  int i;

  /* Stop everything.  */
  outport_byte (port + CMDR, MSK_STP | MSK_RD2);

  /* Wait for the interface to stop, using I as a time limit.  */
  for (i = 0; i < 5000; ++i)
    {
      unsigned char status;

      inport_byte (port + ISR, status);
      if ((status & MSK_RST) != 0)
        break;
    }
}

/* reinitializing interface
*/
static void
ne_reset(struct ne_softc *sc)
{
  ne_stop(sc);
  ne_init_hardware(sc);
  sc->arpcom.ac_if.if_flags |= IFF_RUNNING;
  sc->arpcom.ac_if.if_flags &= ~IFF_OACTIVE;
#ifdef DEBUG_NE
  printk("*");
#endif
}

#ifdef DEBUG_NE
/* show anything about ne
*/
static void
ne_dump(struct ne_softc *sc)
{
  int i;
  printk("\nne configuration:\n");
  printk("ethernet addr:");
  for (i=0; i<ETHER_ADDR_LEN; i++)
    printk(" %x", sc->arpcom.ac_enaddr[i]);
  printk("\n");
  printk("irq = %d\n", sc->irno);
  printk("port = 0x%x\n", sc->port);
  printk("accept_broadcasts = %d\n", sc->accept_broadcasts);
  printk("byte_transfers = %d\n", sc->byte_transfers);
}
#endif

/* Show NE2000 interface statistics.  */

static void
ne_stats (struct ne_softc *sc)
{
  printf ("    Received packets: %-8lu", sc->stats.rx_packets);
  printf (" Transmitted packets: %-8lu\n", sc->stats.tx_packets);
  printf ("        Receive acks: %-8lu", sc->stats.rx_acks);
  printf ("       Transmit acks: %-8lu\n", sc->stats.tx_acks);
  printf ("     Packet overruns: %-8lu", sc->stats.overruns);
  printf ("        Frame errors: %-8lu\n", sc->stats.rx_frame_errors);
  printf ("          CRC errors: %-8lu", sc->stats.rx_crc_errors);
  printf ("      Missed packets: %-8lu\n", sc->stats.rx_missed_errors);
  printf ("          Interrupts: %-8lu\n", sc->stats.interrupts);
}

/* NE2000 driver ioctl handler.  */

static int
ne_ioctl (struct ifnet *ifp, ioctl_command_t command, caddr_t data)
{
  struct ne_softc *sc = ifp->if_softc;
  int error = 0;

  switch (command) {
  case SIOCGIFADDR:
  case SIOCSIFADDR:
    error = ether_ioctl (ifp, command, data);
    break;

  case SIOCSIFFLAGS:
    switch (ifp->if_flags & (IFF_UP | IFF_RUNNING)) {
    case IFF_RUNNING:
      ne_stop (sc);
      break;

    case IFF_UP:
      printk("IFF_UP\n");
      ne_init (sc);
      break;

    case IFF_UP | IFF_RUNNING:
      ne_stop (sc);
      ne_init (sc);
      break;

    default:
      break;
    }
    break;

  case SIO_RTEMS_SHOW_STATS:
    ne_stats (sc);
    break;

    /* FIXME: Multicast commands must be added here.  */

  default:
    error = EINVAL;
    break;
  }

  return error;
}

/*
 * Device timeout/watchdog routine. Entered if the device neglects to
 *	generate an interrupt after a transmit has been started on it.
 */
static void
ne_watchdog(struct ifnet *ifp)
{
  struct ne_softc *sc = ifp->if_softc;

  printk("ne2000: device timeout\n");
    ifp->if_oerrors++;

  ne_reset(sc);
}

static void
print_byte(unsigned char b)
{
  printk("%x%x", b >> 4, b & 0x0f);
}

/* Attach an NE2000 driver to the system.  */

int
rtems_ne_driver_attach (struct rtems_bsdnet_ifconfig *config, int attach)
{
  int i;
  struct ne_softc *sc;
  struct ifnet *ifp;
  int mtu;

  /* dettach ... */
  if (!attach)
    return 0;

  /* Find a free driver.  */
  sc = NULL;
  for (i = 0; i < NNEDRIVER; ++i) {
    sc = &ne_softc[i];
    ifp = &sc->arpcom.ac_if;
    if (ifp->if_softc == NULL)
      break;
  }

  if (sc == NULL) {
    printf ("Too many NE2000 drivers.\n");
    return 0;
  }

  memset (sc, 0, sizeof *sc);

  /* Check whether we do byte-wide or word-wide transfers.  */

#ifdef NE2000_BYTE_TRANSFERS
  sc->byte_transfers = true;
#else
  sc->byte_transfers = false;
#endif

  /* Handle the options passed in by the caller.  */

  if (config->mtu != 0)
    mtu = config->mtu;
  else
    mtu = ETHERMTU;

    /* We use 16 as the default IRQ.  */
  sc->irno = XSCALE_IRQ_NETWORK;



  /*IO prts are mapped to 0X40000600 */
  sc->port = 0x40000600;

  sc->accept_broadcasts = ! config->ignore_broadcast;

  if (config->hardware_address != NULL)
    memcpy (sc->arpcom.ac_enaddr, config->hardware_address,
            ETHER_ADDR_LEN);
  else
    {
      unsigned char prom[16];
      int ia;

      /* Read the PROM to get the Ethernet hardware address.  */

      outport_byte (sc->port + CMDR, MSK_PG0 | MSK_RD2 | MSK_STP);

      if (sc->byte_transfers) {
        outport_byte (sc->port + DCR, MSK_FT10 | MSK_BMS);
      }
      else {
        outport_byte (sc->port + DCR, MSK_FT10 | MSK_BMS | MSK_WTS);
      }

      outport_byte (sc->port + RBCR0, 0);
      outport_byte (sc->port + RBCR1, 0);
      outport_byte (sc->port + RCR, MSK_MON);
      outport_byte (sc->port + TCR, MSK_LOOP);
      outport_byte (sc->port + IMR, 0);
      outport_byte (sc->port + ISR, 0xff);

      ne_read_data (sc, 0, sizeof prom, prom);

      outport_byte (sc->port + CMDR, MSK_PG0 | MSK_RD2 | MSK_STP);

      for (ia = 0; ia < ETHER_ADDR_LEN; ++ia)
        sc->arpcom.ac_enaddr[ia] = prom[ia * 2];
    }

  /* Set up the network interface.  */

  ifp->if_softc = sc;
  ifp->if_unit = i + 1;
  ifp->if_name = "ne";
  ifp->if_mtu = mtu;
  ifp->if_init = ne_init;
  ifp->if_ioctl = ne_ioctl;
  ifp->if_watchdog = ne_watchdog;
  ifp->if_start = ne_start;
  ifp->if_output = ether_output;
  ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX;
  if (ifp->if_snd.ifq_maxlen == 0)
    ifp->if_snd.ifq_maxlen = ifqmaxlen;

  /* Attach the interface.  */

  if_attach (ifp);
  ether_ifattach (ifp);

  printk("network device '%s' <", config->name);
  print_byte(sc->arpcom.ac_enaddr[0]);
  for (i=1; i<ETHER_ADDR_LEN; i++)
  { printk(":");
    print_byte(sc->arpcom.ac_enaddr[i]);
  }
  printk("> initialized on port 0x%x, irq %d\n", sc->port, sc->irno);

  return 1;
}

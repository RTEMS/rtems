/*
 * RTEMS/TCPIP driver for MCF5329 Fast Ethernet Controller
 *
 * TO DO: Check network stack code -- force longword alignment of all tx mbufs?
 */

#include <bsp.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <rtems.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>

#include <net/ethernet.h>
#include <net/if.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>

/*
 * Number of interfaces supported by this driver
 */
#define NIFACES 1

#define FEC_INTC0_TX_VECTOR (64+36)
#define FEC_INTC0_RX_VECTOR (64+40)

/*
 * Default number of buffer descriptors set aside for this driver.
 * The number of transmit buffer descriptors has to be quite large
 * since a single frame often uses three or more buffer descriptors.
 */
#define RX_BUF_COUNT     32
#define TX_BUF_COUNT     20
#define TX_BD_PER_BUF    3

#define INET_ADDR_MAX_BUF_SIZE (sizeof "255.255.255.255")

/*
 * RTEMS event used by interrupt handler to signal daemons.
 * This must *not* be the same event used by the TCP/IP task synchronization.
 */
#define TX_INTERRUPT_EVENT RTEMS_EVENT_1
#define RX_INTERRUPT_EVENT RTEMS_EVENT_1

/*
 * RTEMS event used to start transmit daemon.
 * This must not be the same as INTERRUPT_EVENT.
 */
#define START_TRANSMIT_EVENT RTEMS_EVENT_2

/*
 * Receive buffer size -- Allow for a full ethernet packet plus CRC (1518).
 * Round off to nearest multiple of RBUF_ALIGN.
 */
#define MAX_MTU_SIZE    1518
#define RBUF_ALIGN      4
#define RBUF_SIZE       ((MAX_MTU_SIZE + RBUF_ALIGN) & ~RBUF_ALIGN)

#if (MCLBYTES < RBUF_SIZE)
#  error "Driver must have MCLBYTES > RBUF_SIZE"
#endif

typedef struct mcf5329BufferDescriptor_
{
  volatile uint16_t status;
  uint16_t length;
  volatile void *buffer;
} mcf5329BufferDescriptor_t;

/*
 * Per-device data
 */
struct mcf5329_enet_struct
{
  struct arpcom arpcom;
  struct mbuf **rxMbuf;
  struct mbuf **txMbuf;
  int acceptBroadcast;
  int rxBdCount;
  int txBdCount;
  int txBdHead;
  int txBdTail;
  int txBdActiveCount;
  mcf5329BufferDescriptor_t *rxBdBase;
  mcf5329BufferDescriptor_t *txBdBase;
  rtems_id rxDaemonTid;
  rtems_id txDaemonTid;

  /*
   * Statistics
   */
  unsigned long rxInterrupts;
  unsigned long txInterrupts;
  unsigned long txRawWait;
  unsigned long txRealign;
};
static struct mcf5329_enet_struct enet_driver[NIFACES];

static rtems_isr mcf5329_fec_rx_interrupt_handler(rtems_vector_number v)
{
  MCF_FEC_EIR = MCF_FEC_EIR_RXF;
  MCF_FEC_EIMR &= ~MCF_FEC_EIMR_RXF;
  enet_driver[0].rxInterrupts++;
  rtems_event_send(enet_driver[0].rxDaemonTid, RX_INTERRUPT_EVENT);
}

static rtems_isr mcf5329_fec_tx_interrupt_handler(rtems_vector_number v)
{
  MCF_FEC_EIR = MCF_FEC_EIR_TXF;
  MCF_FEC_EIMR &= ~MCF_FEC_EIMR_TXF;
  enet_driver[0].txInterrupts++;
  rtems_event_send(enet_driver[0].txDaemonTid, TX_INTERRUPT_EVENT);
}

extern char _CoreSRamBase[];

/*
 * Allocate buffer descriptors from (non-cached) on-chip static RAM
 * Ensure 128-bit (16-byte) alignment
 */
static mcf5329BufferDescriptor_t *mcf5329_bd_allocate(unsigned int count)
{
  static mcf5329BufferDescriptor_t *bdp =
    (mcf5329BufferDescriptor_t *) _CoreSRamBase;
  mcf5329BufferDescriptor_t *p = bdp;

  bdp += count;
  if ((int) bdp & 0xF)
    bdp =
      (mcf5329BufferDescriptor_t *) ((char *) bdp + (16 - ((int) bdp & 0xF)));
  return p;
}

#if UNUSED

/*
 * Read MII register
 * Busy-waits, but transfer time should be short!
 */
static int getMII(int phyNumber, int regNumber)
{
  MCF_FEC_MMFR = (0x1 << 30) |
    (0x2 << 28) | (phyNumber << 23) | (regNumber << 18) | (0x2 << 16);
  while ((MCF_FEC_EIR & MCF_FEC_EIR_MII) == 0) ;
  MCF_FEC_EIR = MCF_FEC_EIR_MII;
  return MCF_FEC_MMFR & 0xFFFF;
}
#endif

/*
 * Write MII register
 * Busy-waits, but transfer time should be short!
 */
static void setMII(int phyNumber, int regNumber, int value)
{
  MCF_FEC_MMFR = (0x1 << 30) |
    (0x1 << 28) |
    (phyNumber << 23) | (regNumber << 18) | (0x2 << 16) | (value & 0xFFFF);
  while ((MCF_FEC_EIR & MCF_FEC_EIR_MII) == 0) ;
  MCF_FEC_EIR = MCF_FEC_EIR_MII;
}

static void mcf5329_fec_initialize_hardware(struct mcf5329_enet_struct *sc)
{
  int i;
  const unsigned char *hwaddr = 0;
  rtems_status_code status;
  rtems_isr_entry old_handler;
  uint32_t clock_speed = bsp_get_BUS_clock_speed();

  /*
   * Issue reset to FEC
   */
  MCF_FEC_ECR = MCF_FEC_ECR_RESET;
  rtems_task_wake_after(1);
  MCF_FEC_ECR = 0;

  /*
   * Configuration of I/O ports is done outside of this function
   */
#if 0
  imm->gpio.pbcnt |= MCF_GPIO_PBCNT_SET_FEC;      /* Set up port b FEC pins */
#endif

  /*
   * Set our physical address
   */
  hwaddr = sc->arpcom.ac_enaddr;
  MCF_FEC_PALR = (hwaddr[0] << 24) | (hwaddr[1] << 16) |
    (hwaddr[2] << 8) | (hwaddr[3] << 0);
  MCF_FEC_PAUR = (hwaddr[4] << 24) | (hwaddr[5] << 16);

  /*
   * Clear the hash table
   */
  MCF_FEC_GAUR = 0;
  MCF_FEC_GALR = 0;

  /*
   * Set up receive buffer size
   */
  MCF_FEC_EMRBR = 1520;                           /* Standard Ethernet */

  /*
   * Allocate mbuf pointers
   */
  sc->rxMbuf = malloc(sc->rxBdCount * sizeof *sc->rxMbuf, M_MBUF, M_NOWAIT);
  sc->txMbuf = malloc(sc->txBdCount * sizeof *sc->txMbuf, M_MBUF, M_NOWAIT);
  if (!sc->rxMbuf || !sc->txMbuf)
    rtems_panic("No memory for mbuf pointers");

  /*
   * Set receiver and transmitter buffer descriptor bases
   */
  sc->rxBdBase = mcf5329_bd_allocate(sc->rxBdCount);
  sc->txBdBase = mcf5329_bd_allocate(sc->txBdCount);
  MCF_FEC_ERDSR = (int) sc->rxBdBase;
  MCF_FEC_ETDSR = (int) sc->txBdBase;

  /*
   * Set up Receive Control Register:
   *   Not promiscuous
   *   MII mode
   *   Full duplex
   *   No loopback
   */
  MCF_FEC_RCR = MCF_FEC_RCR_MAX_FL(MAX_MTU_SIZE) | MCF_FEC_RCR_MII_MODE;

  /*
   * Set up Transmit Control Register:
   *   Full duplex
   *   No heartbeat
   */
  MCF_FEC_TCR = MCF_FEC_TCR_FDEN;

  /*
   * Initialize statistic counters
   */
  MCF_FEC_MIBC = MCF_FEC_MIBC_MIB_DISABLE;
  {
    vuint32 *vuip = &MCF_FEC_RMON_T_DROP;

    while (vuip <= &MCF_FEC_IEEE_R_OCTETS_OK)
      *vuip++ = 0;
  }
  MCF_FEC_MIBC = 0;

  /*
   * Set MII speed to <= 2.5 MHz
   */
  i = (clock_speed + 5000000 - 1) / 5000000;
  MCF_FEC_MSCR = MCF_FEC_MSCR_MII_SPEED(i);

  /*
   * Set PHYS to 100 Mb/s, full duplex
   */
  setMII(1, 0, 0x2100);

  /*
   * Set up receive buffer descriptors
   */
  for (i = 0; i < sc->rxBdCount; i++)
    (sc->rxBdBase + i)->status = 0;

  /*
   * Set up transmit buffer descriptors
   */
  for (i = 0; i < sc->txBdCount; i++) {
    sc->txBdBase[i].status = 0;
    sc->txMbuf[i] = NULL;
  }
  sc->txBdHead = sc->txBdTail = 0;
  sc->txBdActiveCount = 0;

  /*
   * Set up interrupts
   */
  status =
    rtems_interrupt_catch(mcf5329_fec_tx_interrupt_handler,
                          FEC_INTC0_TX_VECTOR, &old_handler);
  if (status != RTEMS_SUCCESSFUL)
    rtems_panic("Can't attach MCF FEC TX interrupt handler: %s\n",
                rtems_status_text(status));
  status =
    rtems_interrupt_catch(mcf5329_fec_rx_interrupt_handler,
                          FEC_INTC0_RX_VECTOR, &old_handler);
  if (status != RTEMS_SUCCESSFUL)
    rtems_panic("Can't attach MCF FEC RX interrupt handler: %s\n",
                rtems_status_text(status));
  MCF_INTC0_ICR36 = MCF_INTC_ICR_IL(FEC_IRQ_LEVEL);
  MCF_INTC0_IMRH &= ~(MCF_INTC_IMRH_INT_MASK36);
  MCF_INTC0_ICR40 = MCF_INTC_ICR_IL(FEC_IRQ_LEVEL);
  MCF_INTC0_IMRH &= ~(MCF_INTC_IMRH_INT_MASK40);
}

/*
 * Get the MAC address from the hardware.
 */
static void
fec_get_mac_address(volatile struct mcf5329_enet_struct *sc,
                    unsigned char *hwaddr)
{
  unsigned long addr;

  addr = MCF_FEC_PALR;

  hwaddr[0] = (addr >> 24) & 0xff;
  hwaddr[1] = (addr >> 16) & 0xff;
  hwaddr[2] = (addr >> 8) & 0xff;
  hwaddr[3] = (addr >> 0) & 0xff;

  addr = MCF_FEC_PAUR;

  hwaddr[4] = (addr >> 24) & 0xff;
  hwaddr[5] = (addr >> 16) & 0xff;
}

/*
 * Soak up buffer descriptors that have been sent.
 */
static void fec_retire_tx_bd(volatile struct mcf5329_enet_struct *sc)
{
  struct mbuf *m, *n;

  while ((sc->txBdActiveCount != 0)
         && ((sc->txBdBase[sc->txBdTail].status & MCF_FEC_TxBD_R) == 0)) {
    m = sc->txMbuf[sc->txBdTail];
    MFREE(m, n);
    if (++sc->txBdTail == sc->txBdCount)
      sc->txBdTail = 0;
    sc->txBdActiveCount--;
  }
}

static void fec_rxDaemon(void *arg)
{
  volatile struct mcf5329_enet_struct *sc =
    (volatile struct mcf5329_enet_struct *) arg;
  struct ifnet *ifp = (struct ifnet *) &sc->arpcom.ac_if;
  struct mbuf *m;
  volatile uint16_t status;
  volatile mcf5329BufferDescriptor_t *rxBd;
  int rxBdIndex;

  /*
   * Allocate space for incoming packets and start reception
   */
  for (rxBdIndex = 0;;) {
    rxBd = sc->rxBdBase + rxBdIndex;
    MGETHDR(m, M_WAIT, MT_DATA);
    MCLGET(m, M_WAIT);
    m->m_pkthdr.rcvif = ifp;
    sc->rxMbuf[rxBdIndex] = m;
    rxBd->buffer = mtod(m, void *);

    rxBd->status = MCF_FEC_RxBD_E;
    if (++rxBdIndex == sc->rxBdCount) {
      rxBd->status |= MCF_FEC_RxBD_W;
      break;
    }
  }

  /*
   * Input packet handling loop
   */
  /* Indicate we have some ready buffers available */
  MCF_FEC_RDAR = MCF_FEC_RDAR_R_DES_ACTIVE;

  rxBdIndex = 0;
  for (;;) {
    rxBd = sc->rxBdBase + rxBdIndex;

    /*
     * Wait for packet if there's not one ready
     */
    if ((status = rxBd->status) & MCF_FEC_RxBD_E) {
      /*
       * Clear old events.
       */
      MCF_FEC_EIR = MCF_FEC_EIR_RXF;

      /*
       * Wait for packet to arrive.
       * Check the buffer descriptor before waiting for the event.
       * This catches the case when a packet arrives between the
       * `if' above, and the clearing of the RXF bit in the EIR.
       */
      while ((status = rxBd->status) & MCF_FEC_RxBD_E) {
        rtems_event_set events;
        int level;

        rtems_interrupt_disable(level);
        MCF_FEC_EIMR |= MCF_FEC_EIMR_RXF;
        rtems_interrupt_enable(level);
        rtems_bsdnet_event_receive(RX_INTERRUPT_EVENT,
                                   RTEMS_WAIT | RTEMS_EVENT_ANY,
                                   RTEMS_NO_TIMEOUT, &events);
      }
    }

    /*
     * Check that packet is valid
     */
    if (status & MCF_FEC_RxBD_L) {
      /*
       * Pass the packet up the chain.
       * FIXME: Packet filtering hook could be done here.
       */
      struct ether_header *eh;
      int len = rxBd->length - sizeof(uint32_t);

      m = sc->rxMbuf[rxBdIndex];

      rtems_cache_invalidate_multiple_data_lines(m->m_data, len);

      m->m_len = m->m_pkthdr.len = len - sizeof(struct ether_header);
      eh = mtod(m, struct ether_header *);
      m->m_data += sizeof(struct ether_header);
      ether_input(ifp, eh, m);

      /*
       * Allocate a new mbuf
       */
      MGETHDR(m, M_WAIT, MT_DATA);
      MCLGET(m, M_WAIT);
      m->m_pkthdr.rcvif = ifp;
      sc->rxMbuf[rxBdIndex] = m;
      rxBd->buffer = mtod(m, void *);
    }

    /*
     * Reenable the buffer descriptor
     */
    rxBd->status = (status & MCF_FEC_RxBD_W) | MCF_FEC_RxBD_E;
    MCF_FEC_RDAR = MCF_FEC_RDAR_R_DES_ACTIVE;

    /*
     * Move to next buffer descriptor
     */
    if (++rxBdIndex == sc->rxBdCount)
      rxBdIndex = 0;
  }
}

static void fec_sendpacket(struct ifnet *ifp, struct mbuf *m)
{
  struct mcf5329_enet_struct *sc = ifp->if_softc;
  volatile mcf5329BufferDescriptor_t *firstTxBd, *txBd;
  uint16_t status;
  int nAdded;

  /*
   * Free up buffer descriptors
   */
  fec_retire_tx_bd(sc);

  /*
   * Set up the transmit buffer descriptors.
   * No need to pad out short packets since the
   * hardware takes care of that automatically.
   * No need to copy the packet to a contiguous buffer
   * since the hardware is capable of scatter/gather DMA.
   */
  nAdded = 0;
  firstTxBd = sc->txBdBase + sc->txBdHead;

  for (;;) {
    /*
     * Wait for buffer descriptor to become available
     */
    if ((sc->txBdActiveCount + nAdded) == sc->txBdCount) {
      /*
       * Clear old events.
       */
      MCF_FEC_EIR = MCF_FEC_EIR_TXF;

      /*
       * Wait for buffer descriptor to become available.
       * Check for buffer descriptors before waiting for the event.
       * This catches the case when a buffer became available between
       * the `if' above, and the clearing of the TXF bit in the EIR.
       */
      fec_retire_tx_bd(sc);
      while ((sc->txBdActiveCount + nAdded) == sc->txBdCount) {
        rtems_event_set events;
        int level;

        rtems_interrupt_disable(level);
        MCF_FEC_EIMR |= MCF_FEC_EIMR_TXF;
        rtems_interrupt_enable(level);
        sc->txRawWait++;
        rtems_bsdnet_event_receive(TX_INTERRUPT_EVENT,
                                   RTEMS_WAIT | RTEMS_EVENT_ANY,
                                   RTEMS_NO_TIMEOUT, &events);
        fec_retire_tx_bd(sc);
      }
    }

    /*
     * Don't set the READY flag on the first fragment
     * until the whole packet has been readied.
     */
    status = nAdded ? MCF_FEC_TxBD_R : 0;

    /*
     * The IP fragmentation routine in ip_output
     * can produce fragments with zero length.
     */
    txBd = sc->txBdBase + sc->txBdHead;
    if (m->m_len) {
      char *p = mtod(m, char *);

      /*
       * Stupid FEC can't handle misaligned data!
       * Given the way that mbuf's are layed out it should be
       * safe to shuffle the data down like this.....
       * Perhaps this code could be improved with a "Duff's Device".
       */
      if ((int) p & 0x3) {
        int l = m->m_len;
        char *dest = p - ((int) p & 0x3);
        uint16_t *o = (uint16_t *) dest, *i = (uint16_t *) p;

        while (l > 0) {
          *o++ = *i++;
          l -= sizeof(uint16_t);
        }
        p = dest;
        sc->txRealign++;
      }

      txBd->buffer = p;
      txBd->length = m->m_len;

      rtems_cache_flush_multiple_data_lines(txBd->buffer, txBd->length);

      sc->txMbuf[sc->txBdHead] = m;
      nAdded++;
      if (++sc->txBdHead == sc->txBdCount) {
        status |= MCF_FEC_TxBD_W;
        sc->txBdHead = 0;
      }
      m = m->m_next;
    } else {
      /*
       * Just toss empty mbufs
       */
      struct mbuf *n;

      MFREE(m, n);
      m = n;
    }
    if (m == NULL) {
      if (nAdded) {
        txBd->status = status | MCF_FEC_TxBD_R
          | MCF_FEC_TxBD_L | MCF_FEC_TxBD_TC;
        if (nAdded > 1)
          firstTxBd->status |= MCF_FEC_TxBD_R;
        MCF_FEC_TDAR = MCF_FEC_TDAR_X_DES_ACTIVE;
        sc->txBdActiveCount += nAdded;
      }
      break;
    }
    txBd->status = status;
  }
}

void fec_txDaemon(void *arg)
{
  struct mcf5329_enet_struct *sc = (struct mcf5329_enet_struct *) arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  struct mbuf *m;
  rtems_event_set events;

  for (;;) {
    /*
     * Wait for packet
     */
    rtems_bsdnet_event_receive(START_TRANSMIT_EVENT,
                               RTEMS_EVENT_ANY | RTEMS_WAIT,
                               RTEMS_NO_TIMEOUT, &events);

    /*
     * Send packets till queue is empty
     */
    for (;;) {
      /*
       * Get the next mbuf chain to transmit.
       */
      IF_DEQUEUE(&ifp->if_snd, m);
      if (!m)
        break;
      fec_sendpacket(ifp, m);
    }
    ifp->if_flags &= ~IFF_OACTIVE;
  }
}

/*
 * Send packet (caller provides header).
 */
static void mcf5329_enet_start(struct ifnet *ifp)
{
  struct mcf5329_enet_struct *sc = ifp->if_softc;

  rtems_event_send(sc->txDaemonTid, START_TRANSMIT_EVENT);
  ifp->if_flags |= IFF_OACTIVE;
}

static void fec_init(void *arg)
{
  struct mcf5329_enet_struct *sc = arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;

  if (sc->txDaemonTid == 0) {
    /*
     * Set up hardware
     */
    mcf5329_fec_initialize_hardware(sc);

    /*
     * Start driver tasks
     */
    sc->txDaemonTid = rtems_bsdnet_newproc("FECtx", 4096, fec_txDaemon, sc);
    sc->rxDaemonTid = rtems_bsdnet_newproc("FECrx", 4096, fec_rxDaemon, sc);
  }

  /*
   * Set flags appropriately
   */
  if (ifp->if_flags & IFF_PROMISC)
    MCF_FEC_RCR |= MCF_FEC_RCR_PROM;
  else
    MCF_FEC_RCR &= ~MCF_FEC_RCR_PROM;

  /*
   * Tell the world that we're running.
   */
  ifp->if_flags |= IFF_RUNNING;

  /*
   * Enable receiver and transmitter
   */
  MCF_FEC_ECR = MCF_FEC_ECR_ETHER_EN;
}

static void fec_stop(struct mcf5329_enet_struct *sc)
{
  struct ifnet *ifp = &sc->arpcom.ac_if;

  ifp->if_flags &= ~IFF_RUNNING;

  /*
   * Shut down receiver and transmitter
   */
  MCF_FEC_ECR = 0x0;
}

/*
 * Show interface statistics
 */
static void enet_stats(struct mcf5329_enet_struct *sc)
{
  printf("  Rx Interrupts:%-10lu", sc->rxInterrupts);
  printf("Rx Packet Count:%-10lu", MCF_FEC_RMON_R_PACKETS);
  printf("   Rx Broadcast:%-10lu\n", MCF_FEC_RMON_R_BC_PKT);
  printf("   Rx Multicast:%-10lu", MCF_FEC_RMON_R_MC_PKT);
  printf("CRC/Align error:%-10lu", MCF_FEC_RMON_R_CRC_ALIGN);
  printf("   Rx Undersize:%-10lu\n", MCF_FEC_RMON_R_UNDERSIZE);
  printf("    Rx Oversize:%-10lu", MCF_FEC_RMON_R_OVERSIZE);
  printf("    Rx Fragment:%-10lu", MCF_FEC_RMON_R_FRAG);
  printf("      Rx Jabber:%-10lu\n", MCF_FEC_RMON_R_JAB);
  printf("          Rx 64:%-10lu", MCF_FEC_RMON_R_P64);
  printf("      Rx 65-127:%-10lu", MCF_FEC_RMON_R_P65TO127);
  printf("     Rx 128-255:%-10lu\n", MCF_FEC_RMON_R_P128TO255);
  printf("     Rx 256-511:%-10lu", MCF_FEC_RMON_R_P256TO511);
  printf("    Rx 511-1023:%-10lu", MCF_FEC_RMON_R_512TO1023);
  printf("   Rx 1024-2047:%-10lu\n", MCF_FEC_RMON_R_1024TO2047);
  printf("      Rx >=2048:%-10lu", MCF_FEC_RMON_R_P_GTE2048);
  printf("      Rx Octets:%-10lu", MCF_FEC_RMON_R_OCTETS);
  printf("     Rx Dropped:%-10lu\n", MCF_FEC_IEEE_R_DROP);
  printf("    Rx frame OK:%-10lu", MCF_FEC_IEEE_R_FRAME_OK);
  printf("   Rx CRC error:%-10lu", MCF_FEC_IEEE_R_CRC);
  printf(" Rx Align error:%-10lu\n", MCF_FEC_IEEE_R_ALIGN);
  printf("  FIFO Overflow:%-10lu", MCF_FEC_IEEE_R_MACERR);
  printf("Rx Pause Frames:%-10lu", MCF_FEC_IEEE_R_FDXFC);
  printf("   Rx Octets OK:%-10lu\n", MCF_FEC_IEEE_R_OCTETS_OK);
  printf("  Tx Interrupts:%-10lu", sc->txInterrupts);
  printf("Tx Output Waits:%-10lu", sc->txRawWait);
  printf("Tx Realignments:%-10lu\n", sc->txRealign);
  printf(" Tx Unaccounted:%-10lu", MCF_FEC_RMON_T_DROP);
  printf("Tx Packet Count:%-10lu", MCF_FEC_RMON_T_PACKETS);
  printf("   Tx Broadcast:%-10lu\n", MCF_FEC_RMON_T_BC_PKT);
  printf("   Tx Multicast:%-10lu", MCF_FEC_RMON_T_MC_PKT);
  printf("CRC/Align error:%-10lu", MCF_FEC_RMON_T_CRC_ALIGN);
  printf("   Tx Undersize:%-10lu\n", MCF_FEC_RMON_T_UNDERSIZE);
  printf("    Tx Oversize:%-10lu", MCF_FEC_RMON_T_OVERSIZE);
  printf("    Tx Fragment:%-10lu", MCF_FEC_RMON_T_FRAG);
  printf("      Tx Jabber:%-10lu\n", MCF_FEC_RMON_T_JAB);
  printf("  Tx Collisions:%-10lu", MCF_FEC_RMON_T_COL);
  printf("          Tx 64:%-10lu", MCF_FEC_RMON_T_P64);
  printf("      Tx 65-127:%-10lu\n", MCF_FEC_RMON_T_P65TO127);
  printf("     Tx 128-255:%-10lu", MCF_FEC_RMON_T_P128TO255);
  printf("     Tx 256-511:%-10lu", MCF_FEC_RMON_T_P256TO511);
  printf("    Tx 511-1023:%-10lu\n", MCF_FEC_RMON_T_P512TO1023);
  printf("   Tx 1024-2047:%-10lu", MCF_FEC_RMON_T_P1024TO2047);
  printf("      Tx >=2048:%-10lu", MCF_FEC_RMON_T_P_GTE2048);
  printf("      Tx Octets:%-10lu\n", MCF_FEC_RMON_T_OCTETS);
  printf("     Tx Dropped:%-10lu", MCF_FEC_IEEE_T_DROP);
  printf("    Tx Frame OK:%-10lu", MCF_FEC_IEEE_T_FRAME_OK);
  printf(" Tx 1 Collision:%-10lu\n", MCF_FEC_IEEE_T_1COL);
  printf("Tx >1 Collision:%-10lu", MCF_FEC_IEEE_T_MCOL);
  printf("    Tx Deferred:%-10lu", MCF_FEC_IEEE_T_DEF);
  printf(" Late Collision:%-10lu\n", MCF_FEC_IEEE_T_LCOL);
  printf(" Excessive Coll:%-10lu", MCF_FEC_IEEE_T_EXCOL);
  printf("  FIFO Underrun:%-10lu", MCF_FEC_IEEE_T_MACERR);
  printf("  Carrier Error:%-10lu\n", MCF_FEC_IEEE_T_CSERR);
  printf("   Tx SQE Error:%-10lu", MCF_FEC_IEEE_T_SQE);
  printf("Tx Pause Frames:%-10lu", MCF_FEC_IEEE_T_FDXFC);
  printf("   Tx Octets OK:%-10lu\n", MCF_FEC_IEEE_T_OCTETS_OK);
}

static int fec_ioctl(struct ifnet *ifp, ioctl_command_t command, caddr_t data)
{
  struct mcf5329_enet_struct *sc = ifp->if_softc;
  int error = 0;

  switch (command) {
    case SIOCGIFADDR:
    case SIOCSIFADDR:
      ether_ioctl(ifp, command, data);
      break;

    case SIOCSIFFLAGS:
      switch (ifp->if_flags & (IFF_UP | IFF_RUNNING)) {
        case IFF_RUNNING:
          fec_stop(sc);
          break;

        case IFF_UP:
          fec_init(sc);
          break;

        case IFF_UP | IFF_RUNNING:
          fec_stop(sc);
          fec_init(sc);
          break;

        default:
          break;
      }
      break;

    case SIO_RTEMS_SHOW_STATS:
      enet_stats(sc);
      break;

      /*
       * FIXME: All sorts of multicast commands need to be added here!
       */
    default:
      error = EINVAL;
      break;
  }
  return error;
}

int
rtems_fec_driver_attach(struct rtems_bsdnet_ifconfig *config, int attaching)
{
  struct mcf5329_enet_struct *sc;
  struct ifnet *ifp;
  int mtu;
  int unitNumber;
  char *unitName;
  unsigned char *hwaddr;

  /*
   * Parse driver name
   */
  if ((unitNumber = rtems_bsdnet_parse_driver_name(config, &unitName)) < 0)
    return 0;

  /*
   * Is driver free?
   */
  if ((unitNumber < 0) || (unitNumber >= NIFACES)) {
    printf("mcf5329: bad FEC unit number.\n");
    return 0;
  }
  sc = &enet_driver[unitNumber];
  ifp = &sc->arpcom.ac_if;
  if (ifp->if_softc != NULL) {
    printf("mcf5329: driver already in use.\n");
    return 0;
  }

  /*
   * Process options
   */
  if (config->hardware_address)
    memcpy(sc->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);
  else
    fec_get_mac_address(sc, sc->arpcom.ac_enaddr);

  hwaddr = config->hardware_address;
  printf("%s%d: mac: %02x:%02x:%02x:%02x:%02x:%02x\n",
         unitName, unitNumber,
         hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4], hwaddr[5]);

  if (config->mtu)
    mtu = config->mtu;
  else
    mtu = ETHERMTU;
  if (config->rbuf_count)
    sc->rxBdCount = config->rbuf_count;
  else
    sc->rxBdCount = RX_BUF_COUNT;
  if (config->xbuf_count)
    sc->txBdCount = config->xbuf_count;
  else
    sc->txBdCount = TX_BUF_COUNT * TX_BD_PER_BUF;

  sc->acceptBroadcast = !config->ignore_broadcast;

  /*
   * Set up network interface values
   */
  ifp->if_softc = sc;
  ifp->if_unit = unitNumber;
  ifp->if_name = unitName;
  ifp->if_mtu = mtu;
  ifp->if_init = fec_init;
  ifp->if_ioctl = fec_ioctl;
  ifp->if_start = mcf5329_enet_start;
  ifp->if_output = ether_output;
  ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX;
  if (ifp->if_snd.ifq_maxlen == 0)
    ifp->if_snd.ifq_maxlen = ifqmaxlen;

  /*
   * Attach the interface
   */
  if_attach(ifp);
  ether_ifattach(ifp);
  return 1;
};

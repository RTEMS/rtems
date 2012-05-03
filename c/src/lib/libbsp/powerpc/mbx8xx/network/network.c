/*
 * RTEMS/TCPIP driver for MPC8xx SCC1 Ethernet
 *
 *  Modified for MPC860 by Jay Monkman (jmonkman@frasca.com)
 *
 *  This supports Ethernet on either SCC1 or the FEC of the MPC860T.
 *  Right now, we only do 10 Mbps, even with the FEC. The function
 *  rtems_enet_driver_attach determines which one to use. Currently,
 *  only one may be used at a time.
 *
 *  Based on the MC68360 network driver by
 *  W. Eric Norum
 *  Saskatchewan Accelerator Laboratory
 *  University of Saskatchewan
 *  Saskatoon, Saskatchewan, CANADA
 *  eric@skatter.usask.ca
 *
 *  This supports ethernet on SCC1. Right now, we only do 10 Mbps.
 *
 *  Modifications by Darlene Stewart <Darlene.Stewart@iit.nrc.ca>
 *  and Charles-Antoine Gauthier <charles.gauthier@iit.nrc.ca>
 *  Copyright (c) 1999, National Research Council of Canada
 */
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
#include <bsp/irq.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/*
 * Number of interfaces supported by this driver
 */
#define NIFACES 1

/*
 * Default number of buffer descriptors set aside for this driver.
 * The number of transmit buffer descriptors has to be quite large
 * since a single frame often uses four or more buffer descriptors.
 */
#define RX_BUF_COUNT     32
#define TX_BUF_COUNT     8
#define TX_BD_PER_BUF    4

#define INET_ADDR_MAX_BUF_SIZE (sizeof "255.255.255.255")

/*
 * RTEMS event used by interrupt handler to signal daemons.
 * This must *not* be the same event used by the TCP/IP task synchronization.
 */
#define INTERRUPT_EVENT RTEMS_EVENT_1

/*
 * RTEMS event used to start transmit daemon.
 * This must not be the same as INTERRUPT_EVENT.
 */
#define START_TRANSMIT_EVENT RTEMS_EVENT_2

/*
 * Receive buffer size -- Allow for a full ethernet packet plus CRC (1518).
 * Round off to nearest multiple of RBUF_ALIGN.
 */
#define MAX_MTU_SIZE	1518
#define RBUF_ALIGN		4
#define RBUF_SIZE       ((MAX_MTU_SIZE + RBUF_ALIGN) & ~RBUF_ALIGN)

#if (MCLBYTES < RBUF_SIZE)
# error "Driver must have MCLBYTES > RBUF_SIZE"
#endif

/*
 * Per-device data
 */
struct m8xx_enet_struct {
        struct arpcom           arpcom;
        struct mbuf             **rxMbuf;
        struct mbuf             **txMbuf;
        int                     acceptBroadcast;
        int                     rxBdCount;
        int                     txBdCount;
        int                     txBdHead;
        int                     txBdTail;
        int                     txBdActiveCount;
        m8xxBufferDescriptor_t  *rxBdBase;
        m8xxBufferDescriptor_t  *txBdBase;
        rtems_id                rxDaemonTid;
        rtems_id                txDaemonTid;

        /*
         * Statistics
         */
        unsigned long   rxInterrupts;
        unsigned long   rxNotFirst;
        unsigned long   rxNotLast;
        unsigned long   rxGiant;
        unsigned long   rxNonOctet;
        unsigned long   rxRunt;
        unsigned long   rxBadCRC;
        unsigned long   rxOverrun;
        unsigned long   rxCollision;

        unsigned long   txInterrupts;
        unsigned long   txDeferred;
        unsigned long   txHeartbeat;
        unsigned long   txLateCollision;
        unsigned long   txRetryLimit;
        unsigned long   txUnderrun;
        unsigned long   txLostCarrier;
        unsigned long   txRawWait;
};
static struct m8xx_enet_struct enet_driver[NIFACES];

static void  m8xx_scc1_ethernet_on(const rtems_irq_connect_data* ptr)
{
}

static void  m8xx_scc1_ethernet_off(const rtems_irq_connect_data* ptr)
{
  /*
   * Please put relevant code there
   */
}

static void  m8xx_scc1_ethernet_isOn(const rtems_irq_connect_data* ptr)
{
  BSP_irq_enabled_at_cpm (ptr->name);
}

/*
 * SCC1 interrupt handler
 */
static void m8xx_scc1_interrupt_handler (void *unused)
{
    /* Frame received? */
	if ((m8xx.scc1.sccm & 0x8) && (m8xx.scc1.scce & 0x8)) {
		m8xx.scc1.scce = 0x8;		/* Clear receive frame int */
		m8xx.scc1.sccm &= ~0x8; 	/* Disable receive frame ints */
		enet_driver[0].rxInterrupts++; /* Rx int has occurred */
		rtems_event_send (enet_driver[0].rxDaemonTid, INTERRUPT_EVENT);
	}

	/* Buffer transmitted or transmitter error? */
	if ((m8xx.scc1.sccm & 0x12) && (m8xx.scc1.scce & 0x12)) {
		m8xx.scc1.scce = 0x12;		/* Clear Tx int */
		m8xx.scc1.sccm &= ~0x12; 	/* Disable Tx ints */
		enet_driver[0].txInterrupts++; /* Tx int has occurred */
		rtems_event_send (enet_driver[0].txDaemonTid, INTERRUPT_EVENT);
	}
}

#ifdef MPC860T
/*
 * FEC interrupt handler
 */
static void m860_fec_interrupt_handler ()
{
  /*
   * Frame received?
   */
  if (m8xx.fec.ievent & M8xx_FEC_IEVENT_RFINT) {
    m8xx.fec.ievent = M8xx_FEC_IEVENT_RFINT;
    enet_driver[0].rxInterrupts++;
    rtems_event_send (enet_driver[0].rxDaemonTid, INTERRUPT_EVENT);
  }

  /*
   * Buffer transmitted or transmitter error?
   */
  if (m8xx.fec.ievent & M8xx_FEC_IEVENT_TFINT) {
    m8xx.fec.ievent = M8xx_FEC_IEVENT_TFINT;
    enet_driver[0].txInterrupts++;
    rtems_event_send (enet_driver[0].txDaemonTid, INTERRUPT_EVENT);
  }
}
#endif

static rtems_irq_connect_data ethernetSCC1IrqData = {
  BSP_CPM_IRQ_SCC1,
  (rtems_irq_hdl) m8xx_scc1_interrupt_handler,
  0,
  (rtems_irq_enable) m8xx_scc1_ethernet_on,
  (rtems_irq_disable) m8xx_scc1_ethernet_off,
  (rtems_irq_is_enabled)m8xx_scc1_ethernet_isOn
};

/*
 * Initialize the ethernet hardware
 */
static void
m8xx_enet_initialize (struct m8xx_enet_struct *sc)
{
  int i;
  unsigned char *hwaddr;

  /*
   * Configure port A
   * PA15 is enet RxD. Set PAPAR(15) to 1, PADIR(15) to 0.
   * PA14 is enet TxD. Set PAPAR(14) to 1, PADIR(14) to 0, PAODR(14) to 0.
   * PA7 is input CLK1. Set PAPAR(7) to 1, PADIR(7) to 0.
   * PA6 is input CLK2. Set PAPAR(6) to 1, PADIR(6) to 0.
   */
  m8xx.papar |=  0x303;
  m8xx.padir &= ~0x303;
  m8xx.paodr &= ~0x2;

  /*
   * Configure port C
   * PC11 is CTS1*. Set PCPAR(11) to 0, PCDIR(11) to 0, and PCSO(11) to 1.
   * PC10 is CD1*. Set PCPAR(10) to 0, PCDIR(10) to 0, and PCSO(10) to 1.
   */
  m8xx.pcpar &= ~0x30;
  m8xx.pcdir &= ~0x30;
  m8xx.pcso  |=  0x30;

  /*
   * Connect CLK1 and CLK2 to SCC1 in the SICR.
   * CLK1 is TxClk, CLK2 is RxClk. No grant mechanism, SCC1 is directly
   * connected to the NMSI pins.
   * R1CS = 0b101 (CLK2)
   * T1CS = 0b100 (CLK1)
   */
  m8xx.sicr |= 0x2C;

  /*
   * Initialize SDMA configuration register
   */
  m8xx.sdcr = 1;

  /*
   * Allocate mbuf pointers
   */
  sc->rxMbuf = malloc (sc->rxBdCount * sizeof *sc->rxMbuf,
		       M_MBUF, M_NOWAIT);
  sc->txMbuf = malloc (sc->txBdCount * sizeof *sc->txMbuf,
		       M_MBUF, M_NOWAIT);
  if (!sc->rxMbuf || !sc->txMbuf)
    rtems_panic ("No memory for mbuf pointers");

  /*
   * Set receiver and transmitter buffer descriptor bases
   */
  sc->rxBdBase = m8xx_bd_allocate(sc->rxBdCount);
  sc->txBdBase = m8xx_bd_allocate(sc->txBdCount);
  m8xx.scc1p.rbase = (char *)sc->rxBdBase - (char *)&m8xx;
  m8xx.scc1p.tbase = (char *)sc->txBdBase - (char *)&m8xx;

  /*
   * Send "Init parameters" command
   */
  m8xx_cp_execute_cmd (M8xx_CR_OP_INIT_RX_TX | M8xx_CR_CHAN_SCC1);

  /*
   * Set receive and transmit function codes
   */
  m8xx.scc1p.rfcr = M8xx_RFCR_MOT | M8xx_RFCR_DMA_SPACE(0);
  m8xx.scc1p.tfcr = M8xx_TFCR_MOT | M8xx_TFCR_DMA_SPACE(0);

  /*
   * Set maximum receive buffer length
   */
  m8xx.scc1p.mrblr = RBUF_SIZE;

  /*
   * Set CRC parameters
   */
  m8xx.scc1p.un.ethernet.c_pres = 0xFFFFFFFF;
  m8xx.scc1p.un.ethernet.c_mask = 0xDEBB20E3;

  /*
   * Clear diagnostic counters
   */
  m8xx.scc1p.un.ethernet.crcec = 0;
  m8xx.scc1p.un.ethernet.alec = 0;
  m8xx.scc1p.un.ethernet.disfc = 0;

  /*
   * Set pad value
   */
  m8xx.scc1p.un.ethernet.pads = 0x8888;

  /*
   * Set retry limit
   */
  m8xx.scc1p.un.ethernet.ret_lim = 15;

  /*
   * Set maximum and minimum frame length
   */
  m8xx.scc1p.un.ethernet.mflr = 1518;
  m8xx.scc1p.un.ethernet.minflr = 64;
  m8xx.scc1p.un.ethernet.maxd1 = MAX_MTU_SIZE;
  m8xx.scc1p.un.ethernet.maxd2 = MAX_MTU_SIZE;

  /*
   * Clear group address hash table
   */
  m8xx.scc1p.un.ethernet.gaddr1 = 0;
  m8xx.scc1p.un.ethernet.gaddr2 = 0;
  m8xx.scc1p.un.ethernet.gaddr3 = 0;
  m8xx.scc1p.un.ethernet.gaddr4 = 0;

  /*
   * Set our physical address
   */
  hwaddr = sc->arpcom.ac_enaddr;

  m8xx.scc1p.un.ethernet.paddr_h = (hwaddr[5] << 8) | hwaddr[4];
  m8xx.scc1p.un.ethernet.paddr_m = (hwaddr[3] << 8) | hwaddr[2];
  m8xx.scc1p.un.ethernet.paddr_l = (hwaddr[1] << 8) | hwaddr[0];

  /*
   * Aggressive retry
   */
  m8xx.scc1p.un.ethernet.p_per = 0;

  /*
   * Clear individual address hash table
   */
  m8xx.scc1p.un.ethernet.iaddr1 = 0;
  m8xx.scc1p.un.ethernet.iaddr2 = 0;
  m8xx.scc1p.un.ethernet.iaddr3 = 0;
  m8xx.scc1p.un.ethernet.iaddr4 = 0;

  /*
   * Clear temp address
   */
  m8xx.scc1p.un.ethernet.taddr_l = 0;
  m8xx.scc1p.un.ethernet.taddr_m = 0;
  m8xx.scc1p.un.ethernet.taddr_h = 0;

  /*
   * Set up receive buffer descriptors
   */
  for (i = 0 ; i < sc->rxBdCount ; i++) {
    (sc->rxBdBase + i)->status = 0;
  }

  /*
   * Set up transmit buffer descriptors
   */
  for (i = 0 ; i < sc->txBdCount ; i++) {
    (sc->txBdBase + i)->status = 0;
    sc->txMbuf[i] = NULL;
  }
  sc->txBdHead = sc->txBdTail = 0;
  sc->txBdActiveCount = 0;

  /*
   * Clear any outstanding events
   */
  m8xx.scc1.scce = 0xFFFF;

  /*
   * Set up interrupts
   */
  if (!BSP_install_rtems_irq_handler (&ethernetSCC1IrqData)) {
    rtems_panic ("Can't attach M8xx SCC1 interrupt handler\n");
  }
  m8xx.scc1.sccm = 0;     /* No interrupts unmasked till necessary */

  /*
   * Set up General SCC Mode Register
   * Ethernet configuration
   */
  m8xx.scc1.gsmr_h = 0x0;
  m8xx.scc1.gsmr_l = 0x1088000c;

  /*
   * Set up data synchronization register
   * Ethernet synchronization pattern
   */
  m8xx.scc1.dsr = 0xd555;

  /*
   * Set up protocol-specific mode register
   *      No Heartbeat check
   *      No force collision
   *      Discard short frames
   *      Individual address mode
   *      Ethernet CRC
   *      Not promisuous
   *      Ignore/accept broadcast packets as specified
   *      Normal backoff timer
   *      No loopback
   *      No input sample at end of frame
   *      64-byte limit for late collision
   *      Wait 22 bits before looking for start of frame delimiter
   *      Disable full-duplex operation
   */
  m8xx.scc1.psmr = 0x080A | (sc->acceptBroadcast ? 0 : 0x100);

  /*
   * Enable the TENA (RTS1*) pin
   */
  m8xx.pcpar |=  0x1;
  m8xx.pcdir &= ~0x1;

  /*
   * Enable receiver and transmitter
   */
  m8xx.scc1.gsmr_l = 0x1088003c;
}

#ifdef MPC860T
/*
 * Please organize FEC controller code better by moving code from
 * m860_fec_initialize_hardware to m8xx_fec_ethernet_on
 */
static void m8xx_fec_ethernet_on(){};
static void m8xx_fec_ethernet_off(){};
static int m8xx_fec_ethernet_isOn (const rtems_irq_connect_data* ptr)
{
  return BSP_irq_enabled_at_siu (ptr->name);
}

static rtems_irq_connect_data ethernetFECIrqData = {
  BSP_FAST_ETHERNET_CTRL,
  (rtems_irq_hdl) m8xx_fec_interrupt_handler,
  (rtems_irq_enable) m8xx_fec_ethernet_on,
  (rtems_irq_disable) m8xx_fec_ethernet_off,
  (rtems_irq_is_enabled)m8xx_fec_ethernet_isOn
};

static void
m860_fec_initialize_hardware (struct m860_enet_struct *sc)
{
  int i;
  unsigned char *hwaddr;
  rtems_status_code status;

  /*
   * Issue reset to FEC
   */
  m8xx.fec.ecntrl=0x1;

  /*
   * Put ethernet transciever in reset
   */
  m8xx.pgcra |= 0x80;

  /*
   * Configure I/O ports
   */
  m8xx.pdpar = 0x1fff;
  m8xx.pddir = 0x1c58;

  /*
   * Take ethernet transciever out of reset
   */
  m8xx.pgcra &= ~0x80;

  /*
   * Set SIU interrupt level to LVL2
   *
   */
  m8xx.fec.ivec = ((((unsigned) BSP_FAST_ETHERNET_CTRL)/2) << 29);

  /*
   * Set the TX and RX fifo sizes. For now, we'll split it evenly
   */
  /* If you uncomment these, the FEC will not work right.
  m8xx.fec.r_fstart = ((m8xx.fec.r_bound & 0x3ff) >> 2) & 0x3ff;
  m8xx.fec.x_fstart = 0;
  */

  /*
   * Set our physical address
   */
  hwaddr = sc->arpcom.ac_enaddr;

  m8xx.fec.addr_low = (hwaddr[0] << 24) | (hwaddr[1] << 16) |
                      (hwaddr[2] << 8)  | (hwaddr[3] << 0);
  m8xx.fec.addr_high = (hwaddr[4] << 24) | (hwaddr[5] << 16);

  /*
   * Clear the hash table
   */
  m8xx.fec.hash_table_high = 0;
  m8xx.fec.hash_table_low  = 0;

  /*
   * Set up receive buffer size
   */
  m8xx.fec.r_buf_size = 0x5f0; /* set to 1520 */

  /*
   * Allocate mbuf pointers
   */
  sc->rxMbuf = malloc (sc->rxBdCount * sizeof *sc->rxMbuf,
                       M_MBUF, M_NOWAIT);
  sc->txMbuf = malloc (sc->txBdCount * sizeof *sc->txMbuf,
                       M_MBUF, M_NOWAIT);
  if (!sc->rxMbuf || !sc->txMbuf)
    rtems_panic ("No memory for mbuf pointers");

  /*
   * Set receiver and transmitter buffer descriptor bases
   */
  sc->rxBdBase = m8xx_bd_allocate(sc->rxBdCount);
  sc->txBdBase = m8xx_bd_allocate(sc->txBdCount);
  m8xx.fec.r_des_start = (int)sc->rxBdBase;
  m8xx.fec.x_des_start = (int)sc->txBdBase;

  /*
   * Set up Receive Control Register:
   *   Not promiscuous mode
   *   MII mode
   *   Half duplex
   *   No loopback
   */
  m8xx.fec.r_cntrl = 0x00000006;

  /*
   * Set up Transmit Control Register:
   *   Half duplex
   *   No heartbeat
   */
  m8xx.fec.x_cntrl = 0x00000000;

  /*
   * Set up DMA function code:
   *   Big-endian
   *   DMA functino code = 0
   */
  m8xx.fec.fun_code = 0x78000000;

  /*
   * Initialize SDMA configuration register
   *   SDMA ignores FRZ
   *   FEC not aggressive
   *   FEC arbitration ID = 0 => U-bus arbitration = 6
   *   RISC arbitration ID = 1 => U-bus arbitration = 5
   */
  m8xx.sdcr = 1;

  /*
   * Set MII speed to 2.5 MHz for 25 Mhz system clock
   */
  m8xx.fec.mii_speed = 0x0a;
  m8xx.fec.mii_data = 0x58021000;

  /*
   * Set up receive buffer descriptors
   */
  for (i = 0 ; i < sc->rxBdCount ; i++)
    (sc->rxBdBase + i)->status = 0;

  /*
   * Set up transmit buffer descriptors
   */
  for (i = 0 ; i < sc->txBdCount ; i++) {
    (sc->txBdBase + i)->status = 0;
    sc->txMbuf[i] = NULL;
  }
  sc->txBdHead = sc->txBdTail = 0;
  sc->txBdActiveCount = 0;

  /*
   * Mask all FEC interrupts and clear events
   */
  m8xx.fec.imask = M8xx_FEC_IEVENT_TFINT |
                   M8xx_FEC_IEVENT_RFINT;
  m8xx.fec.ievent = ~0;

  /*
   * Set up interrupts
   */
  if (!BSP_install_rtems_irq_handler (&ethernetFECIrqData))
    rtems_panic ("Can't attach M860 FEC interrupt handler\n");

}
#endif

/*
 * Soak up buffer descriptors that have been sent.
 * Note that a buffer descriptor can't be retired as soon as it becomes
 * ready. The MPC860 manual (MPC860UM/AD 07/98 Rev.1) and the MPC821
 * manual state that, "If an Ethernet frame is made up of multiple
 * buffers, the user should not reuse the first buffer descriptor until
 * the last buffer descriptor of the frame has had its ready bit cleared
 * by the CPM".
 */
static void
m8xx_Enet_retire_tx_bd (struct m8xx_enet_struct *sc)
{
  uint16_t   status;
  int i;
  int nRetired;
  struct mbuf *m, *n;

  i = sc->txBdTail;
  nRetired = 0;
  while ((sc->txBdActiveCount != 0)
	&&  (((status = (sc->txBdBase + i)->status) & M8xx_BD_READY) == 0)) {
    /*
     * See if anything went wrong
     */
    if (status & (M8xx_BD_DEFER |
                  M8xx_BD_HEARTBEAT |
                  M8xx_BD_LATE_COLLISION |
                  M8xx_BD_RETRY_LIMIT |
                  M8xx_BD_UNDERRUN |
                  M8xx_BD_CARRIER_LOST)) {
      /*
       * Check for errors which stop the transmitter.
       */
      if (status & (M8xx_BD_LATE_COLLISION |
                    M8xx_BD_RETRY_LIMIT |
                    M8xx_BD_UNDERRUN)) {
        if (status & M8xx_BD_LATE_COLLISION)
          enet_driver[0].txLateCollision++;
        if (status & M8xx_BD_RETRY_LIMIT)
          enet_driver[0].txRetryLimit++;
        if (status & M8xx_BD_UNDERRUN)
          enet_driver[0].txUnderrun++;

        /*
         * Restart the transmitter
         */
        /* FIXME: this should get executed only if using the SCC */
        m8xx_cp_execute_cmd (M8xx_CR_OP_RESTART_TX | M8xx_CR_CHAN_SCC1);
      }
      if (status & M8xx_BD_DEFER)
        enet_driver[0].txDeferred++;
      if (status & M8xx_BD_HEARTBEAT)
        enet_driver[0].txHeartbeat++;
      if (status & M8xx_BD_CARRIER_LOST)
        enet_driver[0].txLostCarrier++;
    }
    nRetired++;
    if (status & M8xx_BD_LAST) {
      /*
       * A full frame has been transmitted.
       * Free all the associated buffer descriptors.
       */
      sc->txBdActiveCount -= nRetired;
      while (nRetired) {
        nRetired--;
        m = sc->txMbuf[sc->txBdTail];
        MFREE (m, n);
        if (++sc->txBdTail == sc->txBdCount)
          sc->txBdTail = 0;
      }
    }
    if (++i == sc->txBdCount)
      i = 0;
  }
}

/*
 * reader task
 */
static void
scc_rxDaemon (void *arg)
{
  struct m8xx_enet_struct *sc = (struct m8xx_enet_struct *)arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  struct mbuf *m;
  uint16_t   status;
  m8xxBufferDescriptor_t *rxBd;
  int rxBdIndex;

  /*
   * Allocate space for incoming packets and start reception
   */
  for (rxBdIndex = 0 ; ;) {
    rxBd = sc->rxBdBase + rxBdIndex;
    MGETHDR (m, M_WAIT, MT_DATA);
    MCLGET (m, M_WAIT);
    m->m_pkthdr.rcvif = ifp;
    sc->rxMbuf[rxBdIndex] = m;
    rxBd->buffer = mtod (m, void *);
    rxBd->status = M8xx_BD_EMPTY | M8xx_BD_INTERRUPT;
    if (++rxBdIndex == sc->rxBdCount) {
      rxBd->status |= M8xx_BD_WRAP;
      break;
    }
  }

  /*
   * Input packet handling loop
   */
  rxBdIndex = 0;
  for (;;) {
    rxBd = sc->rxBdBase + rxBdIndex;

    /*
     * Wait for packet if there's not one ready
     */
    if ((status = rxBd->status) & M8xx_BD_EMPTY) {
      /*
       * Clear old events
       */
      m8xx.scc1.scce = 0x8;

      /*
       * Wait for packet
       * Note that the buffer descriptor is checked
       * *before* the event wait -- this catches the
       * possibility that a packet arrived between the
       * `if' above, and the clearing of the event register.
       */
      while ((status = rxBd->status) & M8xx_BD_EMPTY) {
        rtems_event_set events;

        /*
         * Unmask RXF (Full frame received) event
         */
        m8xx.scc1.sccm |= 0x8;

        rtems_bsdnet_event_receive (INTERRUPT_EVENT,
                                    RTEMS_WAIT|RTEMS_EVENT_ANY,
                                    RTEMS_NO_TIMEOUT,
                                    &events);
      }
    }

    /*
     * Check that packet is valid
     */
    if ((status & (M8xx_BD_LAST |
                   M8xx_BD_FIRST_IN_FRAME |
                   M8xx_BD_LONG |
                   M8xx_BD_NONALIGNED |
                   M8xx_BD_SHORT |
                   M8xx_BD_CRC_ERROR |
                   M8xx_BD_OVERRUN |
                   M8xx_BD_COLLISION)) ==
        (M8xx_BD_LAST |
         M8xx_BD_FIRST_IN_FRAME)) {
      /*
       * Pass the packet up the chain.
       * FIXME: Packet filtering hook could be done here.
       */
      struct ether_header *eh;

   	/*
	 * Invalidate the buffer for this descriptor
	 */
	rtems_cache_invalidate_multiple_data_lines((const void *)rxBd->buffer, rxBd->length);

      m = sc->rxMbuf[rxBdIndex];
      m->m_len = m->m_pkthdr.len = rxBd->length -
        sizeof(uint32_t) -
        sizeof(struct ether_header);
      eh = mtod (m, struct ether_header *);
      m->m_data += sizeof(struct ether_header);
      ether_input (ifp, eh, m);

      /*
       * Allocate a new mbuf
       */
      MGETHDR (m, M_WAIT, MT_DATA);
      MCLGET (m, M_WAIT);
      m->m_pkthdr.rcvif = ifp;
      sc->rxMbuf[rxBdIndex] = m;
      rxBd->buffer = mtod (m, void *);
    }
    else {
      /*
       * Something went wrong with the reception
       */
      if (!(status & M8xx_BD_LAST))
        sc->rxNotLast++;
      if (!(status & M8xx_BD_FIRST_IN_FRAME))
        sc->rxNotFirst++;
      if (status & M8xx_BD_LONG)
        sc->rxGiant++;
      if (status & M8xx_BD_NONALIGNED)
        sc->rxNonOctet++;
      if (status & M8xx_BD_SHORT)
        sc->rxRunt++;
      if (status & M8xx_BD_CRC_ERROR)
        sc->rxBadCRC++;
      if (status & M8xx_BD_OVERRUN)
        sc->rxOverrun++;
      if (status & M8xx_BD_COLLISION)
        sc->rxCollision++;
    }

    /*
     * Reenable the buffer descriptor
     */
    rxBd->status = (status & (M8xx_BD_WRAP | M8xx_BD_INTERRUPT)) |
                    M8xx_BD_EMPTY;

    /*
     * Move to next buffer descriptor
     */
    if (++rxBdIndex == sc->rxBdCount)
      rxBdIndex = 0;
  }
}

#ifdef MPC860T
static void
fec_rxDaemon (void *arg)
{
  struct m8xx_enet_struct *sc = (struct m8xx_enet_struct *)arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  struct mbuf *m;
  uint16_t   status;
  m8xxBufferDescriptor_t *rxBd;
  int rxBdIndex;

  /*
   * Allocate space for incoming packets and start reception
   */
  for (rxBdIndex = 0 ; ;) {
    rxBd = sc->rxBdBase + rxBdIndex;
    MGETHDR (m, M_WAIT, MT_DATA);
    MCLGET (m, M_WAIT);
    m->m_pkthdr.rcvif = ifp;
    sc->rxMbuf[rxBdIndex] = m;
    rxBd->buffer = mtod (m, void *);
    rxBd->status = M8xx_BD_EMPTY;
    m8xx.fec.r_des_active = 0x1000000;
    if (++rxBdIndex == sc->rxBdCount) {
      rxBd->status |= M8xx_BD_WRAP;
      break;
    }
  }

  /*
   * Input packet handling loop
   */
  rxBdIndex = 0;
  for (;;) {
    rxBd = sc->rxBdBase + rxBdIndex;

    /*
     * Wait for packet if there's not one ready
     */
    if ((status = rxBd->status) & M8xx_BD_EMPTY) {
      /*
       * Clear old events
       */
      m8xx.fec.ievent = M8xx_FEC_IEVENT_RFINT;

      /*
       * Wait for packet
       * Note that the buffer descriptor is checked
       * *before* the event wait -- this catches the
       * possibility that a packet arrived between the
       * `if' above, and the clearing of the event register.
       */
      while ((status = rxBd->status) & M8xx_BD_EMPTY) {
        rtems_event_set events;

        /*
         * Unmask RXF (Full frame received) event
         */
        m8xx.fec.ievent |= M8xx_FEC_IEVENT_RFINT;

        rtems_bsdnet_event_receive (INTERRUPT_EVENT,
                                    RTEMS_WAIT|RTEMS_EVENT_ANY,
                                    RTEMS_NO_TIMEOUT,
                                    &events);
      }
    }

    /*
     * Check that packet is valid
     */
    if (status & M8xx_BD_LAST) {
      /*
       * Pass the packet up the chain.
       * FIXME: Packet filtering hook could be done here.
       */
      struct ether_header *eh;

    	/*
			 * Invalidate the buffer for this descriptor
			 */
			rtems_cache_invalidate_multiple_data_lines((const void *)rxBd->buffer, rxBd->length);

      m = sc->rxMbuf[rxBdIndex];
      m->m_len = m->m_pkthdr.len = rxBd->length -
        sizeof(uint32_t) -
        sizeof(struct ether_header);
      eh = mtod (m, struct ether_header *);
      m->m_data += sizeof(struct ether_header);
      ether_input (ifp, eh, m);

      /*
       * Allocate a new mbuf
       */
      MGETHDR (m, M_WAIT, MT_DATA);
      MCLGET (m, M_WAIT);
      m->m_pkthdr.rcvif = ifp;
      sc->rxMbuf[rxBdIndex] = m;
      rxBd->buffer = mtod (m, void *);
    }
    else {
      /*
       * Something went wrong with the reception
       */
      if (!(status & M8xx_BD_LAST))
        sc->rxNotLast++;
      if (status & M8xx_BD_LONG)
        sc->rxGiant++;
      if (status & M8xx_BD_NONALIGNED)
        sc->rxNonOctet++;
      if (status & M8xx_BD_SHORT)
        sc->rxRunt++;
      if (status & M8xx_BD_CRC_ERROR)
        sc->rxBadCRC++;
      if (status & M8xx_BD_OVERRUN)
        sc->rxOverrun++;
      if (status & M8xx_BD_COLLISION)
        sc->rxCollision++;
    }
    /*
     * Reenable the buffer descriptor
     */
    rxBd->status = (status & M8xx_BD_WRAP) |
                    M8xx_BD_EMPTY;
    m8xx.fec.r_des_active = 0x1000000;
    /*
     * Move to next buffer descriptor
     */
    if (++rxBdIndex == sc->rxBdCount)
      rxBdIndex = 0;
  }
}
#endif

static void
scc_sendpacket (struct ifnet *ifp, struct mbuf *m)
{
  struct m8xx_enet_struct *sc = ifp->if_softc;
  volatile m8xxBufferDescriptor_t *firstTxBd, *txBd;
  struct mbuf *l = NULL;
  uint16_t   status;
  int nAdded;

  /*
   * Free up buffer descriptors
   */
  m8xx_Enet_retire_tx_bd (sc);

  /*
   * Set up the transmit buffer descriptors.
   * No need to pad out short packets since the
   * hardware takes care of that automatically.
   * No need to copy the packet to a contiguous buffer
   * since the hardware is capable of scatter/gather DMA.
   */
  nAdded = 0;
  txBd = firstTxBd = sc->txBdBase + sc->txBdHead;
  for (;;) {
    /*
     * Wait for buffer descriptor to become available.
     */
    if ((sc->txBdActiveCount + nAdded) == sc->txBdCount) {
      /*
       * Clear old events
       */
      m8xx.scc1.scce = 0x12;

      /*
       * Wait for buffer descriptor to become available.
       * Note that the buffer descriptors are checked
       * *before* * entering the wait loop -- this catches
       * the possibility that a buffer descriptor became
       * available between the `if' above, and the clearing
       * of the event register.
       * This is to catch the case where the transmitter
       * stops in the middle of a frame -- and only the
       * last buffer descriptor in a frame can generate
       * an interrupt.
       */
      m8xx_Enet_retire_tx_bd (sc);
      while ((sc->txBdActiveCount + nAdded) == sc->txBdCount) {
        rtems_event_set events;

                                /*
                                 * Unmask TXB (buffer transmitted) and
                                 * TXE (transmitter error) events.
                                 */
        m8xx.scc1.sccm |= 0x12;
        rtems_bsdnet_event_receive (INTERRUPT_EVENT,
                                    RTEMS_WAIT|RTEMS_EVENT_ANY,
                                    RTEMS_NO_TIMEOUT,
                                    &events);
        m8xx_Enet_retire_tx_bd (sc);
      }
    }

    /*
     * Don't set the READY flag till the
     * whole packet has been readied.
     */
    status = nAdded ? M8xx_BD_READY : 0;

    /*
     *  FIXME: Why not deal with empty mbufs at at higher level?
     * The IP fragmentation routine in ip_output
     * can produce packet fragments with zero length.
     * I think that ip_output should be changed to get
     * rid of these zero-length mbufs, but for now,
     * I'll deal with them here.
     */
    if (m->m_len) {
      /*
       * Fill in the buffer descriptor
       */
      txBd->buffer = mtod (m, void *);
      txBd->length = m->m_len;

      /*
       * Flush the buffer for this descriptor
       */
      rtems_cache_flush_multiple_data_lines((const void *)txBd->buffer, txBd->length);

      sc->txMbuf[sc->txBdHead] = m;
      nAdded++;
      if (++sc->txBdHead == sc->txBdCount) {
        status |= M8xx_BD_WRAP;
        sc->txBdHead = 0;
      }
      l = m;
      m = m->m_next;
    }
    else {
      /*
       * Just toss empty mbufs
       */
      struct mbuf *n;
      MFREE (m, n);
      m = n;
      if (l != NULL)
        l->m_next = m;
    }

    /*
     * Set the transmit buffer status.
     * Break out of the loop if this mbuf is the last in the frame.
     */
    if (m == NULL) {
      if (nAdded) {
        status |= M8xx_BD_PAD | M8xx_BD_LAST | M8xx_BD_TX_CRC | M8xx_BD_INTERRUPT;
        txBd->status = status;
        firstTxBd->status |= M8xx_BD_READY;
        sc->txBdActiveCount += nAdded;
      }
      break;
    }
    txBd->status = status;
    txBd = sc->txBdBase + sc->txBdHead;
  }
}

#ifdef MPC860T
static void
fec_sendpacket (struct ifnet *ifp, struct mbuf *m)
{
  struct m8xx_enet_struct *sc = ifp->if_softc;
  volatile m8xxBufferDescriptor_t *firstTxBd, *txBd;
  /*  struct mbuf *l = NULL; */
  uint16_t   status;
  int nAdded;

  /*
   * Free up buffer descriptors
   */
  m8xx_Enet_retire_tx_bd (sc);

  /*
   * Set up the transmit buffer descriptors.
   * No need to pad out short packets since the
   * hardware takes care of that automatically.
   * No need to copy the packet to a contiguous buffer
   * since the hardware is capable of scatter/gather DMA.
   */
  nAdded = 0;
  txBd = firstTxBd = sc->txBdBase + sc->txBdHead;
  for (;;) {
    /*
     * Wait for buffer descriptor to become available.
     */
    if ((sc->txBdActiveCount + nAdded) == sc->txBdCount) {
      /*
       * Clear old events
       */
      m8xx.fec.ievent = M8xx_FEC_IEVENT_TFINT;

      /*
       * Wait for buffer descriptor to become available.
       * Note that the buffer descriptors are checked
       * *before* * entering the wait loop -- this catches
       * the possibility that a buffer descriptor became
       * available between the `if' above, and the clearing
       * of the event register.
       * This is to catch the case where the transmitter
       * stops in the middle of a frame -- and only the
       * last buffer descriptor in a frame can generate
       * an interrupt.
       */
      m8xx_Enet_retire_tx_bd (sc);
      while ((sc->txBdActiveCount + nAdded) == sc->txBdCount) {
        rtems_event_set events;

        /*
         * Unmask TXB (buffer transmitted) and
         * TXE (transmitter error) events.
         */
        m8xx.fec.ievent |= M8xx_FEC_IEVENT_TFINT;
        rtems_bsdnet_event_receive (INTERRUPT_EVENT,
                                    RTEMS_WAIT|RTEMS_EVENT_ANY,
                                    RTEMS_NO_TIMEOUT,
                                    &events);
        m8xx_Enet_retire_tx_bd (sc);
      }
    }

    /*
     * Don't set the READY flag till the
     * whole packet has been readied.
     */
    status = nAdded ? M8xx_BD_READY : 0;

    /*
     *  FIXME: Why not deal with empty mbufs at at higher level?
     * The IP fragmentation routine in ip_output
     * can produce packet fragments with zero length.
     * I think that ip_output should be changed to get
     * rid of these zero-length mbufs, but for now,
     * I'll deal with them here.
     */
    if (m->m_len) {
      /*
       * Fill in the buffer descriptor
       */
      txBd->buffer = mtod (m, void *);
      txBd->length = m->m_len;

      /*
       * Flush the buffer for this descriptor
       */
      rtems_cache_flush_multiple_data_lines(txBd->buffer, txBd->length);

      sc->txMbuf[sc->txBdHead] = m;
      nAdded++;
      if (++sc->txBdHead == sc->txBdCount) {
        status |= M8xx_BD_WRAP;
        sc->txBdHead = 0;
      }
      /*      l = m;*/
      m = m->m_next;
    }
    else {
      /*
       * Just toss empty mbufs
       */
      struct mbuf *n;
      MFREE (m, n);
      m = n;
      /*
      if (l != NULL)
        l->m_next = m;
      */
    }

    /*
     * Set the transmit buffer status.
     * Break out of the loop if this mbuf is the last in the frame.
     */
    if (m == NULL) {
      if (nAdded) {
        status |= M8xx_BD_LAST | M8xx_BD_TX_CRC;
        txBd->status = status;
        firstTxBd->status |= M8xx_BD_READY;
        m8xx.fec.x_des_active = 0x1000000;
        sc->txBdActiveCount += nAdded;
      }
      break;
    }
    txBd->status = status;
    txBd = sc->txBdBase + sc->txBdHead;
  }
}
#endif

/*
 * Driver transmit daemon
 */
void
scc_txDaemon (void *arg)
{
  struct m8xx_enet_struct *sc = (struct m8xx_enet_struct *)arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  struct mbuf *m;
  rtems_event_set events;

  for (;;) {
    /*
     * Wait for packet
     */
    rtems_bsdnet_event_receive (START_TRANSMIT_EVENT, RTEMS_EVENT_ANY | RTEMS_WAIT, RTEMS_NO_TIMEOUT, &events);

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
      scc_sendpacket (ifp, m);
    }
    ifp->if_flags &= ~IFF_OACTIVE;
  }
}

#ifdef MPC860T
void
fec_txDaemon (void *arg)
{
  struct m8xx_enet_struct *sc = (struct m8xx_enet_struct *)arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  struct mbuf *m;
  rtems_event_set events;

  for (;;) {
    /*
     * Wait for packet
     */
    rtems_bsdnet_event_receive (START_TRANSMIT_EVENT,
                                RTEMS_EVENT_ANY | RTEMS_WAIT,
                                RTEMS_NO_TIMEOUT,
                                &events);

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
      fec_sendpacket (ifp, m);
    }
    ifp->if_flags &= ~IFF_OACTIVE;
  }
}
#endif

/*
 * Send packet (caller provides header).
 */
static void
m8xx_enet_start (struct ifnet *ifp)
{
  struct m8xx_enet_struct *sc = ifp->if_softc;

  rtems_event_send (sc->txDaemonTid, START_TRANSMIT_EVENT);
  ifp->if_flags |= IFF_OACTIVE;
}

/*
 * Initialize and start the device
 */
static void
scc_init (void *arg)
{
  struct m8xx_enet_struct *sc = arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;

  if (sc->txDaemonTid == 0) {

    /*
     * Set up SCC hardware
     */
    m8xx_enet_initialize (sc);

    /*
     * Start driver tasks
     */
    sc->txDaemonTid = rtems_bsdnet_newproc ("SCtx", 4096, scc_txDaemon, sc);
    sc->rxDaemonTid = rtems_bsdnet_newproc ("SCrx", 4096, scc_rxDaemon, sc);

  }

  /*
   * Set flags appropriately
   */
  if (ifp->if_flags & IFF_PROMISC)
    m8xx.scc1.psmr |= 0x200;
  else
    m8xx.scc1.psmr &= ~0x200;

  /*
   * Tell the world that we're running.
   */
  ifp->if_flags |= IFF_RUNNING;

  /*
   * Enable receiver and transmitter
   */
  m8xx.scc1.gsmr_l |= 0x30;
}

#ifdef MPC860T
static void
fec_init (void *arg)
{
  struct m8xx_enet_struct *sc = arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;

  if (sc->txDaemonTid == 0) {

    /*
     * Set up SCC hardware
     */
    m8xx_fec_initialize_hardware (sc);

    /*
     * Start driver tasks
     */
    sc->txDaemonTid = rtems_bsdnet_newproc ("SCtx", 4096, fec_txDaemon, sc);
    sc->rxDaemonTid = rtems_bsdnet_newproc ("SCrx", 4096, fec_rxDaemon, sc);

  }

  /*
   * Set flags appropriately
   */
  if (ifp->if_flags & IFF_PROMISC)
    m8xx.fec.r_cntrl |= 0x8;
  else
    m8xx.fec.r_cntrl &= ~0x8;

  /*
   * Tell the world that we're running.
   */
  ifp->if_flags |= IFF_RUNNING;

  /*
   * Enable receiver and transmitter
   */
  m8xx.fec.ecntrl = 0x2;
}
#endif

/*
 * Stop the device
 */
static void
scc_stop (struct m8xx_enet_struct *sc)
{
  struct ifnet *ifp = &sc->arpcom.ac_if;

  ifp->if_flags &= ~IFF_RUNNING;

  /*
   * Shut down receiver and transmitter
   */
  m8xx.scc1.gsmr_l &= ~0x30;
}

#ifdef MPC860T
static void
fec_stop (struct m8xx_enet_struct *sc)
{
  struct ifnet *ifp = &sc->arpcom.ac_if;

  ifp->if_flags &= ~IFF_RUNNING;

  /*
   * Shut down receiver and transmitter
   */
  m8xx.fec.ecntrl = 0x0;
}
#endif

/*
 * Show interface statistics
 */
static void
enet_stats (struct m8xx_enet_struct *sc)
{
  printf ("      Rx Interrupts:%-8lu", sc->rxInterrupts);
  printf ("       Not First:%-8lu", sc->rxNotFirst);
  printf ("        Not Last:%-8lu\n", sc->rxNotLast);
  printf ("              Giant:%-8lu", sc->rxGiant);
  printf ("            Runt:%-8lu", sc->rxRunt);
  printf ("       Non-octet:%-8lu\n", sc->rxNonOctet);
  printf ("            Bad CRC:%-8lu", sc->rxBadCRC);
  printf ("         Overrun:%-8lu", sc->rxOverrun);
  printf ("       Collision:%-8lu\n", sc->rxCollision);
  printf ("          Discarded:%-8lu\n", (unsigned long)m8xx.scc1p.un.ethernet.disfc);

  printf ("      Tx Interrupts:%-8lu", sc->txInterrupts);
  printf ("        Deferred:%-8lu", sc->txDeferred);
  printf (" Missed Hearbeat:%-8lu\n", sc->txHeartbeat);
  printf ("         No Carrier:%-8lu", sc->txLostCarrier);
  printf ("Retransmit Limit:%-8lu", sc->txRetryLimit);
  printf ("  Late Collision:%-8lu\n", sc->txLateCollision);
  printf ("           Underrun:%-8lu", sc->txUnderrun);
  printf (" Raw output wait:%-8lu\n", sc->txRawWait);
}

/*
 * Driver ioctl handler
 */
static int
scc_ioctl (struct ifnet *ifp, ioctl_command_t command, caddr_t data)
{
  struct m8xx_enet_struct *sc = ifp->if_softc;
  int error = 0;

  switch (command) {
  case SIOCGIFADDR:
  case SIOCSIFADDR:
    ether_ioctl (ifp, command, data);
    break;

  case SIOCSIFFLAGS:
    switch (ifp->if_flags & (IFF_UP | IFF_RUNNING)) {
    case IFF_RUNNING:
      scc_stop (sc);
      break;

    case IFF_UP:
      scc_init (sc);
      break;

    case IFF_UP | IFF_RUNNING:
      scc_stop (sc);
      scc_init (sc);
      break;

    default:
      break;
    }
    break;

  case SIO_RTEMS_SHOW_STATS:
    enet_stats (sc);
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

#ifdef MPC860T
static int
fec_ioctl (struct ifnet *ifp, int command, caddr_t data)
{
  struct m8xx_enet_struct *sc = ifp->if_softc;
  int error = 0;

  switch (command) {
  case SIOCGIFADDR:
  case SIOCSIFADDR:
    ether_ioctl (ifp, command, data);
    break;

  case SIOCSIFFLAGS:
    switch (ifp->if_flags & (IFF_UP | IFF_RUNNING)) {
    case IFF_RUNNING:
      fec_stop (sc);
      break;

    case IFF_UP:
      fec_init (sc);
      break;

    case IFF_UP | IFF_RUNNING:
      fec_stop (sc);
      fec_init (sc);
      break;

    default:
      break;
    }
    break;

  case SIO_RTEMS_SHOW_STATS:
    enet_stats (sc);
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
#endif

/*
 * Attach an SCC driver to the system
 */
int
rtems_scc1_driver_attach (struct rtems_bsdnet_ifconfig *config)
{
  struct m8xx_enet_struct *sc;
  struct ifnet *ifp;
  int mtu;
  int unitNumber;
  char *unitName;
#if NVRAM_CONFIGURE == 1
  char *pAddr;
  unsigned long addr;
#endif

	/*
	 * Parse driver name
	 */
	if ((unitNumber = rtems_bsdnet_parse_driver_name (config, &unitName)) < 0)
		return 0;

	/*
	 * Is driver free?
	 */
	if ((unitNumber <= 0) || (unitNumber > NIFACES)) {
		printf ("Bad SCC unit number.\n");
		return 0;
	}
	sc = &enet_driver[unitNumber - 1];
	ifp = &sc->arpcom.ac_if;
	if (ifp->if_softc != NULL) {
		printf ("Driver already in use.\n");
		return 0;
	}

  /*
   * Process options
   */
#if NVRAM_CONFIGURE == 1

  /* Configure from NVRAM */
  if ( (addr = nvram->ipaddr) ) {
    /* We have a non-zero entry, copy the value */
    if ( (pAddr = malloc ( INET_ADDR_MAX_BUF_SIZE, 0, M_NOWAIT )) )
      config->ip_address = (char *)inet_ntop(AF_INET, &addr, pAddr, INET_ADDR_MAX_BUF_SIZE -1 );
    else
      rtems_panic("Can't allocate ip_address buffer!\n");
  }

  if ( (addr = nvram->netmask) ) {
    /* We have a non-zero entry, copy the value */
    if ( (pAddr = malloc ( INET_ADDR_MAX_BUF_SIZE, 0, M_NOWAIT )) )
      config->ip_netmask = (char *)inet_ntop(AF_INET, &addr, pAddr, INET_ADDR_MAX_BUF_SIZE -1 );
    else
      rtems_panic("Can't allocate ip_netmask buffer!\n");
  }

  /* Ethernet address requires special handling -- it must be copied into
   * the arpcom struct. The following if construct serves only to give the
   * User Area NVRAM parameter the highest priority.
   *
   * If the ethernet address is specified in NVRAM, go ahead and copy it.
   * (ETHER_ADDR_LEN = 6 bytes).
   */
  if ( nvram->enaddr[0] || nvram->enaddr[1] || nvram->enaddr[2] ) {
    /* Anything in the first three bytes indicates a non-zero entry, copy value */
  	memcpy ((void *)sc->arpcom.ac_enaddr, &nvram->enaddr, ETHER_ADDR_LEN);
  }
  else if ( config->hardware_address ) {
    /* There is no entry in NVRAM, but there is in the ifconfig struct, so use it. */
    memcpy ((void *)sc->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);
  }
  else {
    /* There is no ethernet address provided, so it could be read
     * from the Ethernet protocol block of SCC1 in DPRAM.
     */
    rtems_panic("No Ethernet address specified!\n");
  }

#else /* NVRAM_CONFIGURE != 1 */

  if (config->hardware_address) {
    memcpy (sc->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);
  }
  else {
    /* There is no ethernet address provided, so it could be read
     * from the Ethernet protocol block of SCC1 in DPRAM.
     */
    rtems_panic("No Ethernet address specified!\n");
  }

#endif /* NVRAM_CONFIGURE != 1 */

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
  ifp->if_init = scc_init;
  ifp->if_ioctl = scc_ioctl;
  ifp->if_start = m8xx_enet_start;
  ifp->if_output = ether_output;
  ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX;
  if (ifp->if_snd.ifq_maxlen == 0)
    ifp->if_snd.ifq_maxlen = ifqmaxlen;

  /*
   * Attach the interface
   */
  if_attach (ifp);
  ether_ifattach (ifp);
  return 1;
};

#ifdef MPC860T
int
rtems_fec_driver_attach (struct rtems_bsdnet_ifconfig *config)
{
  struct m8xx_enet_struct *sc;
  struct ifnet *ifp;
  int mtu;
  int unitNumber;
  char *unitName;

	/*
	 * Parse driver name
	 */
	if ((unitNumber = rtems_bsdnet_parse_driver_name (config, &unitName)) < 0)
		return 0;

	/*
	 * Is driver free?
	 */
	if ((unitNumber <= 0) || (unitNumber > NIFACES)) {
		printf ("Bad SCC unit number.\n");
		return 0;
	}
	sc = &enet_driver[unitNumber - 1];
	ifp = &sc->arpcom.ac_if;
	if (ifp->if_softc != NULL) {
		printf ("Driver already in use.\n");
		return 0;
	}

  /*
   * Process options
   */
  if (config->hardware_address) {
    memcpy (sc->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);
  }
  else {
		/* FIXME to read the enaddr from NVRAM */
  }
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
  ifp->if_start = m8xx_enet_start;
  ifp->if_output = ether_output;
  ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX;
  if (ifp->if_snd.ifq_maxlen == 0)
    ifp->if_snd.ifq_maxlen = ifqmaxlen;

  /*
   * Attach the interface
   */
  if_attach (ifp);
  ether_ifattach (ifp);
  return 1;
};
#endif

int
rtems_enet_driver_attach(struct rtems_bsdnet_ifconfig *config, int attaching)
{

#ifdef MPC860T
  if ((m8xx.fec.mii_data & 0xffff) == 0x2000) {
/*    rtems_scc1_driver_attach(config);*/
    return rtems_fec_driver_attach(config);
  }
  else {
#endif
    return rtems_scc1_driver_attach(config);
#ifdef MPC860T
  }
#endif
}

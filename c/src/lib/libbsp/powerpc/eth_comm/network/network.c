/*
 * RTEMS/TCPIP driver for MPC860 SCC1 Ethernet
 *
 * Modified for MPC860 by Jay Monkman (jmonkman@frasca.com)
 *
 * This supports ethernet on either SCC1 or the FEC of the MPC860T.
 *  Right now, we only do 10 Mbps, even with the FEC. The function
 *  rtems_m860_enet_driver_attach determines which one to use. Currently,
 *  only one may be used at a time.
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 *
 *  $Id$
 */
#include <bsp.h>
#include <mpc860.h>
#include <stdio.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>

#include <net/if.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>

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

/*
 * RTEMS event used by interrupt handler to signal daemons.
 * This must *not* be the same event used by the TCP/IP task synchronization.
 */
#define INTERRUPT_EVENT RTEMS_EVENT_1

/*
 * RTEMS event used to start transmit daemon.
 * This must not be the same as INTERRUPT_EVENT.
 */
#define START_TRANSMIT_EVENT    RTEMS_EVENT_2

/*
 * Receive buffer size -- Allow for a full ethernet packet plus a pointer
 */
#define RBUF_SIZE       1520

#if (MCLBYTES < RBUF_SIZE)
# error "Driver must have MCLBYTES > RBUF_SIZE"
#endif

/*
 * Per-device data
 */
struct m860_enet_struct {
        struct arpcom           arpcom;
        struct mbuf             **rxMbuf;
        struct mbuf             **txMbuf;
        int                     acceptBroadcast;
        int                     rxBdCount;
        int                     txBdCount;
        int                     txBdHead;
        int                     txBdTail;
        int                     txBdActiveCount;
        m860BufferDescriptor_t  *rxBdBase;
        m860BufferDescriptor_t  *txBdBase;
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
static struct m860_enet_struct enet_driver[NIFACES];

/*
 * SCC1 interrupt handler
 */
static rtems_isr
m860_scc1_interrupt_handler (rtems_vector_number v)
{
        /*
         * Frame received?
         */
        if ((m860.scc1.sccm & 0x8) && (m860.scc1.scce & 0x8)) {
                m860.scc1.scce = 0x8;
                /* I don't think the next line is needed. It was in 
                 * the 68360 stuff, though.
                 *   m860.scc1.sccm &= ~0x8; 
                 */
                enet_driver[0].rxInterrupts++;
                rtems_event_send (enet_driver[0].rxDaemonTid, INTERRUPT_EVENT);
        }

        /*
         * Buffer transmitted or transmitter error?
         */
        if ((m860.scc1.sccm & 0x12) && (m860.scc1.scce & 0x12)) {
                m860.scc1.scce = 0x12;
                /* I don't think the next line is needed. It was in 
                 * the 68360 stuff, though.
                 *   m860.scc1.sccm &= ~0x12; 
                 */
                enet_driver[0].txInterrupts++;
                rtems_event_send (enet_driver[0].txDaemonTid, INTERRUPT_EVENT);
        }
        m860.cisr = 1UL << 30;  /* Clear SCC1 interrupt-in-service bit */
}

/*
 * FEC interrupt handler
 */
static rtems_isr
m860_fec_interrupt_handler (rtems_vector_number v)
{
  /*
   * Frame received?
   */
  if (m860.fec.ievent & M860_FEC_IEVENT_RFINT) {
    m860.fec.ievent = M860_FEC_IEVENT_RFINT;
    enet_driver[0].rxInterrupts++;
    rtems_event_send (enet_driver[0].rxDaemonTid, INTERRUPT_EVENT);
  }
  
  /*
   * Buffer transmitted or transmitter error?
   */
  if (m860.fec.ievent & M860_FEC_IEVENT_TFINT) {
    m860.fec.ievent = M860_FEC_IEVENT_TFINT;
    enet_driver[0].txInterrupts++;
    rtems_event_send (enet_driver[0].txDaemonTid, INTERRUPT_EVENT);
  }
}

/*
 * Initialize the ethernet hardware
 */
static void
m860_scc_initialize_hardware (struct m860_enet_struct *sc)
{
  int i;
  unsigned char *hwaddr;
  rtems_status_code status;
  rtems_isr_entry old_handler;
  
  /*
   * Configure port A CLK1, CLK2, TXD1 and RXD1 pins
   */
  m860.papar |=  0x303;
  m860.padir &= ~0x303;
  m860.paodr &= ~0x303;
  
  /*
   * Configure port C CTS1* and CD1* pins, and PC4-PC7
   *  
   */
  m860.pcpar &= ~0x30;
  m860.pcdir |= 0x0f00;
  m860.pcdir &= ~0x30;
  m860.pcso  |=  0x30;
  m860.pcdat &= ~0x0f00;  /* Clear LOOP */
  m860.pcdat |= 0x0700;   /* Set FULDL, TPSQEL, TPAPCE */
  
  /*
   * Connect CLK1 and CLK2 to SCC1
   */
  m860.sicr &= ~0xFF;
  m860.sicr |= (5 << 3) | 4;
  
  /*
   * Initialize SDMA configuration register
   */
  m860.sdcr = 1;
  
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
  sc->rxBdBase = M860AllocateBufferDescriptors(sc->rxBdCount);
  sc->txBdBase = M860AllocateBufferDescriptors(sc->txBdCount);
  m860.scc1p.rbase = (char *)sc->rxBdBase - (char *)&m860;
  m860.scc1p.tbase = (char *)sc->txBdBase - (char *)&m860;
  
  /*
   * Send "Init parameters" command
   */
  M860ExecuteRISC (M860_CR_OP_INIT_RX_TX | M860_CR_CHAN_SCC1);
  
  /*
   * Set receive and transmit function codes
   */
  m860.scc1p.rfcr = M860_RFCR_MOT | M860_RFCR_DMA_SPACE(0);
  m860.scc1p.tfcr = M860_TFCR_MOT | M860_TFCR_DMA_SPACE(0);
  
  /*
   * Set maximum receive buffer length
   */
  m860.scc1p.mrblr = RBUF_SIZE;
  
  /*
   * Set CRC parameters
   */
  m860.scc1p.un.ethernet.c_pres = 0xFFFFFFFF;
  m860.scc1p.un.ethernet.c_mask = 0xDEBB20E3;
  
  /*
   * Clear diagnostic counters
   */
  m860.scc1p.un.ethernet.crcec = 0;
  m860.scc1p.un.ethernet.alec = 0;
  m860.scc1p.un.ethernet.disfc = 0;
  
  /*
   * Set pad value
   */
  m860.scc1p.un.ethernet.pads = 0x8888;
  
  /*
   * Set retry limit
   */
  m860.scc1p.un.ethernet.ret_lim = 15;
  
  /*
   * Set maximum and minimum frame length
   */
  m860.scc1p.un.ethernet.mflr = 1518;
  m860.scc1p.un.ethernet.minflr = 64;
  m860.scc1p.un.ethernet.maxd1 = RBUF_SIZE;
  m860.scc1p.un.ethernet.maxd2 = RBUF_SIZE;
  
  /*
   * Clear group address hash table
   */
  m860.scc1p.un.ethernet.gaddr1 = 0;
  m860.scc1p.un.ethernet.gaddr2 = 0;
  m860.scc1p.un.ethernet.gaddr3 = 0;
  m860.scc1p.un.ethernet.gaddr4 = 0;
  
  /*
   * Set our physical address
   */
  hwaddr = sc->arpcom.ac_enaddr;
  
  m860.scc1p.un.ethernet.paddr_h = (hwaddr[5] << 8) | hwaddr[4];
  m860.scc1p.un.ethernet.paddr_m = (hwaddr[3] << 8) | hwaddr[2];
  m860.scc1p.un.ethernet.paddr_l = (hwaddr[1] << 8) | hwaddr[0];
  
  /*
   * Aggressive retry
   */
  m860.scc1p.un.ethernet.p_per = 0;
  
  /*
   * Clear individual address hash table
   */
  m860.scc1p.un.ethernet.iaddr1 = 0;
  m860.scc1p.un.ethernet.iaddr2 = 0;
  m860.scc1p.un.ethernet.iaddr3 = 0;
  m860.scc1p.un.ethernet.iaddr4 = 0;
  
  /*
   * Clear temp address
   */
  m860.scc1p.un.ethernet.taddr_l = 0;
  m860.scc1p.un.ethernet.taddr_m = 0;
  m860.scc1p.un.ethernet.taddr_h = 0;
  
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
  m860.scc1.scce = 0xFFFF;
  
  /*
   * Set up interrupts
   */
  status = rtems_interrupt_catch (m860_scc1_interrupt_handler,
				  PPC_IRQ_CPM_SCC1,
				  &old_handler);
  if (status != RTEMS_SUCCESSFUL) {
    rtems_panic ("Can't attach M860 SCC1 interrupt handler: %s\n",
		 rtems_status_text (status));
  }
  m860.scc1.sccm = 0;     /* No interrupts unmasked till necessary */
  m860.cimr |= (1UL << 30);       /* Enable SCC1 interrupt */
  
  /*
   * Set up General SCC Mode Register
   * Ethernet configuration
   */
  m860.scc1.gsmr_h = 0x0;
  m860.scc1.gsmr_l = 0x1088000c;
  
  /*
   * Set up data synchronization register
   * Ethernet synchronization pattern
   */
  m860.scc1.dsr = 0xd555;
  
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
  m860.scc1.psmr = 0x080A | (sc->acceptBroadcast ? 0 : 0x100);
  
  /*
   * Enable the TENA (RTS1*) pin
   */
  m860.pcpar |=  0x1;
  m860.pcdir &= ~0x1;
  
  
  /*
   * Set up interrupts
   * FIXME: DANGER: WARNING:
   * CICR and SIMASK must be set in any module that uses
   *   the CPM. Currently those are console-generic.c and
   *   network.c. If the registers are not set the same
   *   in both places, strange things may happen.
   *   If they are only set in one place, then an application
   *   that used the other module won't work correctly.
   *   Put this comment in each module that sets these 2 registers
   */
  m860.cicr = 0x00e43e80;   /* SCaP=SCC1, SCbP=SCC2, SCcP=SCC3, 
			       SCdP=SCC4, IRL=1, HP=SCC1, IEN=1 */
  m860.simask |= M860_SIMASK_LVM1;
  
  /*
   * Enable receiver and transmitter
   */
  m860.scc1.gsmr_l = 0x1088003c;
}

static void
m860_fec_initialize_hardware (struct m860_enet_struct *sc)
{
  int i;
  unsigned char *hwaddr;
  rtems_status_code status;
  rtems_isr_entry old_handler;

  /*
   * Issue reset to FEC
   */
  m860.fec.ecntrl=0x1;

  /* 
   * Put ethernet transciever in reset
   */
  m860.pgcra |= 0x80;

  /*
   * Configure I/O ports
   */
  m860.pdpar = 0x1fff;
  m860.pddir = 0x1c58;

  /*
   * Take ethernet transciever out of reset
   */
  m860.pgcra &= ~0x80;


  /*
   * Set SIU interrupt level to LVL2
   *  
   */
  m860.fec.ivec = 0x02 << 29;
  
  /*
   * Set the TX and RX fifo sizes. For now, we'll split it evenly
   */
  /* If you uncomment these, the FEC will not work right.
  m860.fec.r_fstart = ((m860.fec.r_bound & 0x3ff) >> 2) & 0x3ff;
  m860.fec.x_fstart = 0;
  */

  /*
   * Set our physical address
   */
  hwaddr = sc->arpcom.ac_enaddr;
  
  m860.fec.addr_low = (hwaddr[0] << 24) | (hwaddr[1] << 16) |
                      (hwaddr[2] << 8)  | (hwaddr[3] << 0);
  m860.fec.addr_high = (hwaddr[4] << 24) | (hwaddr[5] << 16);

  /*
   * Clear the hash table
   */
  m860.fec.hash_table_high = 0;
  m860.fec.hash_table_low  = 0;

  /*
   * Set up receive buffer size
   */
  m860.fec.r_buf_size = 0x5f0; /* set to 1520 */

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
  sc->rxBdBase = M860AllocateBufferDescriptors(sc->rxBdCount);
  sc->txBdBase = M860AllocateBufferDescriptors(sc->txBdCount);
  m860.fec.r_des_start = (int)sc->rxBdBase;
  m860.fec.x_des_start = (int)sc->txBdBase;
  
  /*
   * Set up Receive Control Register:
   *   Not promiscuous mode
   *   MII mode
   *   Half duplex
   *   No loopback
   */
  m860.fec.r_cntrl = 0x00000006;

  /*
   * Set up Transmit Control Register:
   *   Half duplex
   *   No heartbeat
   */
  m860.fec.x_cntrl = 0x00000000;

  /*
   * Set up DMA function code:
   *   Big-endian
   *   DMA functino code = 0
   */
  m860.fec.fun_code = 0x78000000;

  /*
   * Initialize SDMA configuration register
   *   SDMA ignores FRZ
   *   FEC not aggressive
   *   FEC arbitration ID = 0 => U-bus arbitration = 6
   *   RISC arbitration ID = 1 => U-bus arbitration = 5
   */
  m860.sdcr = 1;

  /*
   * Set MII speed to 2.5 MHz for 25 Mhz system clock  
   */
  m860.fec.mii_speed = 0x0a;
  m860.fec.mii_data = 0x58021000;
  
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
  m860.fec.imask = M860_FEC_IEVENT_TFINT | 
                   M860_FEC_IEVENT_RFINT;
  m860.fec.ievent = ~0;

  /*
   * Set up interrupts
   */
  status = rtems_interrupt_catch (m860_fec_interrupt_handler,
                                  PPC_IRQ_LVL2,
                                  &old_handler);
  if (status != RTEMS_SUCCESSFUL)
    rtems_panic ("Can't attach M860 FEC interrupt handler: %s\n",
                 rtems_status_text (status));

  m860.simask |= M860_SIMASK_LVM2;

}


/*
 * Soak up buffer descriptors that have been sent
 * Note that a buffer descriptor can't be retired as soon as it becomes
 * ready.  The MC68360 Errata (May 96) says that, "If an Ethernet frame is
 *  made up of multiple buffers, the user should not reuse the first buffer
 * descriptor until the last buffer descriptor of the frame has had its
 * ready bit cleared by the CPM".
 */
static void
m860Enet_retire_tx_bd (struct m860_enet_struct *sc)
{
  rtems_unsigned16 status;
  int i;
  int nRetired;
  struct mbuf *m, *n;
  
  i = sc->txBdTail;
  nRetired = 0;
  while ((sc->txBdActiveCount != 0)
         &&  (((status = (sc->txBdBase + i)->status) & M860_BD_READY) == 0)) {
    /*
     * See if anything went wrong
     */
    if (status & (M860_BD_DEFER |
                  M860_BD_HEARTBEAT |
                  M860_BD_LATE_COLLISION |
                  M860_BD_RETRY_LIMIT |
                  M860_BD_UNDERRUN |
                  M860_BD_CARRIER_LOST)) {
      /*
       * Check for errors which stop the transmitter.
       */
      if (status & (M860_BD_LATE_COLLISION |
                    M860_BD_RETRY_LIMIT |
                    M860_BD_UNDERRUN)) {
        if (status & M860_BD_LATE_COLLISION)
          enet_driver[0].txLateCollision++;
        if (status & M860_BD_RETRY_LIMIT)
          enet_driver[0].txRetryLimit++;
        if (status & M860_BD_UNDERRUN)
          enet_driver[0].txUnderrun++;
        
        /*
         * Restart the transmitter
         */
        /* FIXME: this should get executed only if using the SCC */
        M860ExecuteRISC (M860_CR_OP_RESTART_TX | M860_CR_CHAN_SCC1);
      }
      if (status & M860_BD_DEFER)
        enet_driver[0].txDeferred++;
      if (status & M860_BD_HEARTBEAT)
        enet_driver[0].txHeartbeat++;
      if (status & M860_BD_CARRIER_LOST)
        enet_driver[0].txLostCarrier++;
    }
    nRetired++;
    if (status & M860_BD_LAST) {
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
  struct m860_enet_struct *sc = (struct m860_enet_struct *)arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  struct mbuf *m;
  rtems_unsigned16 status;
  m860BufferDescriptor_t *rxBd;
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
    rxBd->status = M860_BD_EMPTY | M860_BD_INTERRUPT;
    if (++rxBdIndex == sc->rxBdCount) {
      rxBd->status |= M860_BD_WRAP;
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
    if ((status = rxBd->status) & M860_BD_EMPTY) {
      /*
       * Clear old events
       */
      m860.scc1.scce = 0x8;
      
      /*
       * Wait for packet
       * Note that the buffer descriptor is checked
       * *before* the event wait -- this catches the
       * possibility that a packet arrived between the
       * `if' above, and the clearing of the event register.
       */
      while ((status = rxBd->status) & M860_BD_EMPTY) {
        rtems_event_set events;
        
        /*
         * Unmask RXF (Full frame received) event
         */
        m860.scc1.sccm |= 0x8;
        
        rtems_bsdnet_event_receive (INTERRUPT_EVENT,
                                    RTEMS_WAIT|RTEMS_EVENT_ANY,
                                    RTEMS_NO_TIMEOUT,
                                    &events);
      }
    }
    
    /*
     * Check that packet is valid
     */
    if ((status & (M860_BD_LAST |
                   M860_BD_FIRST_IN_FRAME |
                   M860_BD_LONG |
                   M860_BD_NONALIGNED |
                   M860_BD_SHORT |
                   M860_BD_CRC_ERROR |
                   M860_BD_OVERRUN |
                   M860_BD_COLLISION)) ==
        (M860_BD_LAST |
         M860_BD_FIRST_IN_FRAME)) {
      /*
       * Pass the packet up the chain.
       * FIXME: Packet filtering hook could be done here.
       */
      struct ether_header *eh;
      
      m = sc->rxMbuf[rxBdIndex];
      m->m_len = m->m_pkthdr.len = rxBd->length -
        sizeof(rtems_unsigned32) -
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
      if (!(status & M860_BD_LAST))
        sc->rxNotLast++;
      if (!(status & M860_BD_FIRST_IN_FRAME))
        sc->rxNotFirst++;
      if (status & M860_BD_LONG)
        sc->rxGiant++;
      if (status & M860_BD_NONALIGNED)
        sc->rxNonOctet++;
      if (status & M860_BD_SHORT)
        sc->rxRunt++;
      if (status & M860_BD_CRC_ERROR)
        sc->rxBadCRC++;
      if (status & M860_BD_OVERRUN)
        sc->rxOverrun++;
      if (status & M860_BD_COLLISION)
        sc->rxCollision++;
    }
    
    /*
     * Reenable the buffer descriptor
     */
    rxBd->status = (status & (M860_BD_WRAP | M860_BD_INTERRUPT)) |
                    M860_BD_EMPTY;
    
    /*
     * Move to next buffer descriptor
     */
    if (++rxBdIndex == sc->rxBdCount)
      rxBdIndex = 0;
  }
}

static void
fec_rxDaemon (void *arg)
{
  struct m860_enet_struct *sc = (struct m860_enet_struct *)arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  struct mbuf *m;
  rtems_unsigned16 status;
  m860BufferDescriptor_t *rxBd;
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
    rxBd->status = M860_BD_EMPTY;
    m860.fec.r_des_active = 0x1000000;
    if (++rxBdIndex == sc->rxBdCount) {
      rxBd->status |= M860_BD_WRAP;
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
    if ((status = rxBd->status) & M860_BD_EMPTY) {
      /*
       * Clear old events
       */
      m860.fec.ievent = M860_FEC_IEVENT_RFINT;
      
      /*
       * Wait for packet
       * Note that the buffer descriptor is checked
       * *before* the event wait -- this catches the
       * possibility that a packet arrived between the
       * `if' above, and the clearing of the event register.
       */
      while ((status = rxBd->status) & M860_BD_EMPTY) {
        rtems_event_set events;
        
        /*
         * Unmask RXF (Full frame received) event
         */
        m860.fec.ievent |= M860_FEC_IEVENT_RFINT;
        
        rtems_bsdnet_event_receive (INTERRUPT_EVENT,
                                    RTEMS_WAIT|RTEMS_EVENT_ANY,
                                    RTEMS_NO_TIMEOUT,
                                    &events);
      }
    }
    
    /*
     * Check that packet is valid
     */
    if (status & M860_BD_LAST) {
      /*
       * Pass the packet up the chain.
       * FIXME: Packet filtering hook could be done here.
       */
      struct ether_header *eh;
      
      m = sc->rxMbuf[rxBdIndex];
      m->m_len = m->m_pkthdr.len = rxBd->length -
        sizeof(rtems_unsigned32) -
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
      if (!(status & M860_BD_LAST))
        sc->rxNotLast++;
      if (status & M860_BD_LONG)
        sc->rxGiant++;
      if (status & M860_BD_NONALIGNED)
        sc->rxNonOctet++;
      if (status & M860_BD_SHORT)
        sc->rxRunt++;
      if (status & M860_BD_CRC_ERROR)
        sc->rxBadCRC++;
      if (status & M860_BD_OVERRUN)
        sc->rxOverrun++;
      if (status & M860_BD_COLLISION)
        sc->rxCollision++;
    }
    /*
     * Reenable the buffer descriptor
     */
    rxBd->status = (status & M860_BD_WRAP) |
                    M860_BD_EMPTY;
    m860.fec.r_des_active = 0x1000000;
    /*
     * Move to next buffer descriptor
     */
    if (++rxBdIndex == sc->rxBdCount)
      rxBdIndex = 0;
  }
}

static void
scc_sendpacket (struct ifnet *ifp, struct mbuf *m)
{
  struct m860_enet_struct *sc = ifp->if_softc;
  volatile m860BufferDescriptor_t *firstTxBd, *txBd;
  struct mbuf *l = NULL;
  rtems_unsigned16 status;
  int nAdded;
  
  /*
   * Free up buffer descriptors
   */
  m860Enet_retire_tx_bd (sc);
  
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
      m860.scc1.scce = 0x12;
      
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
      m860Enet_retire_tx_bd (sc);
      while ((sc->txBdActiveCount + nAdded) == sc->txBdCount) {
        rtems_event_set events;
        
                                /*
                                 * Unmask TXB (buffer transmitted) and
                                 * TXE (transmitter error) events.
                                 */
        m860.scc1.sccm |= 0x12;
        rtems_bsdnet_event_receive (INTERRUPT_EVENT,
                                    RTEMS_WAIT|RTEMS_EVENT_ANY,
                                    RTEMS_NO_TIMEOUT,
                                    &events);
        m860Enet_retire_tx_bd (sc);
      }
    }
    
    /*
     * Don't set the READY flag till the
     * whole packet has been readied.
     */
    status = nAdded ? M860_BD_READY : 0;
    
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
      sc->txMbuf[sc->txBdHead] = m;
      nAdded++;
      if (++sc->txBdHead == sc->txBdCount) {
        status |= M860_BD_WRAP;
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
        status |= M860_BD_PAD | M860_BD_LAST | M860_BD_TX_CRC | M860_BD_INTERRUPT;
        txBd->status = status;
        firstTxBd->status |= M860_BD_READY;
        sc->txBdActiveCount += nAdded;
      }
      break;
    }
    txBd->status = status;
    txBd = sc->txBdBase + sc->txBdHead;
  }
}

static void
fec_sendpacket (struct ifnet *ifp, struct mbuf *m)
{
  struct m860_enet_struct *sc = ifp->if_softc;
  volatile m860BufferDescriptor_t *firstTxBd, *txBd;
  /*  struct mbuf *l = NULL; */
  rtems_unsigned16 status;
  int nAdded;
  
  /*
   * Free up buffer descriptors
   */
  m860Enet_retire_tx_bd (sc);
  
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
      m860.fec.ievent = M860_FEC_IEVENT_TFINT;
      
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
      m860Enet_retire_tx_bd (sc);
      while ((sc->txBdActiveCount + nAdded) == sc->txBdCount) {
        rtems_event_set events;
        
        /*
         * Unmask TXB (buffer transmitted) and
         * TXE (transmitter error) events.
         */
        m860.fec.ievent |= M860_FEC_IEVENT_TFINT;
        rtems_bsdnet_event_receive (INTERRUPT_EVENT,
                                    RTEMS_WAIT|RTEMS_EVENT_ANY,
                                    RTEMS_NO_TIMEOUT,
                                    &events);
        m860Enet_retire_tx_bd (sc);
      }
    }
    
    /*
     * Don't set the READY flag till the
     * whole packet has been readied.
     */
    status = nAdded ? M860_BD_READY : 0;
    
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
      sc->txMbuf[sc->txBdHead] = m;
      nAdded++;
      if (++sc->txBdHead == sc->txBdCount) {
        status |= M860_BD_WRAP;
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
        status |= M860_BD_LAST | M860_BD_TX_CRC;
        txBd->status = status;
        firstTxBd->status |= M860_BD_READY;
        m860.fec.x_des_active = 0x1000000;
        sc->txBdActiveCount += nAdded;
      }
      break;
    }
    txBd->status = status;
    txBd = sc->txBdBase + sc->txBdHead;
  }
}

/*
 * Driver transmit daemon
 */
void
scc_txDaemon (void *arg)
{
  struct m860_enet_struct *sc = (struct m860_enet_struct *)arg;
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

void
fec_txDaemon (void *arg)
{
  struct m860_enet_struct *sc = (struct m860_enet_struct *)arg;
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

/*
 * Send packet (caller provides header).
 */
static void
m860_enet_start (struct ifnet *ifp)
{
  struct m860_enet_struct *sc = ifp->if_softc;
  
  rtems_event_send (sc->txDaemonTid, START_TRANSMIT_EVENT);
  ifp->if_flags |= IFF_OACTIVE;
}

/*
 * Initialize and start the device
 */
static void
scc_init (void *arg)
{
  struct m860_enet_struct *sc = arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  
  if (sc->txDaemonTid == 0) {
    
    /*
     * Set up SCC hardware
     */
    m860_scc_initialize_hardware (sc);
    
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
    m860.scc1.psmr |= 0x200;
  else
    m860.scc1.psmr &= ~0x200;
  
  /*
   * Tell the world that we're running.
   */
  ifp->if_flags |= IFF_RUNNING;
  
  /*
   * Enable receiver and transmitter
   */
  m860.scc1.gsmr_l |= 0x30;
}

static void
fec_init (void *arg)
{
  struct m860_enet_struct *sc = arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;
  
  if (sc->txDaemonTid == 0) {
    
    /*
     * Set up SCC hardware
     */
    m860_fec_initialize_hardware (sc);
    
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
    m860.fec.r_cntrl |= 0x8;
  else
    m860.fec.r_cntrl &= ~0x8;

  
  /*
   * Tell the world that we're running.
   */
  ifp->if_flags |= IFF_RUNNING;
  
  /*
   * Enable receiver and transmitter
   */
  m860.fec.ecntrl = 0x2;
}


/*
 * Stop the device
 */
static void
scc_stop (struct m860_enet_struct *sc)
{
  struct ifnet *ifp = &sc->arpcom.ac_if;
  
  ifp->if_flags &= ~IFF_RUNNING;
  
  /*
   * Shut down receiver and transmitter
   */
  m860.scc1.gsmr_l &= ~0x30;
}

static void
fec_stop (struct m860_enet_struct *sc)
{
  struct ifnet *ifp = &sc->arpcom.ac_if;
  
  ifp->if_flags &= ~IFF_RUNNING;
  
  /*
   * Shut down receiver and transmitter
   */
  m860.fec.ecntrl = 0x0;
}

/*
 * Show interface statistics
 */
static void
enet_stats (struct m860_enet_struct *sc)
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
  printf ("          Discarded:%-8lu\n", (unsigned long)m860.scc1p.un.ethernet.disfc);
  
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
scc_ioctl (struct ifnet *ifp, int command, caddr_t data)
{
  struct m860_enet_struct *sc = ifp->if_softc;
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

static int
fec_ioctl (struct ifnet *ifp, int command, caddr_t data)
{
  struct m860_enet_struct *sc = ifp->if_softc;
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

/*
 * Attach an SCC driver to the system
 */
int
rtems_scc1_driver_attach (struct rtems_bsdnet_ifconfig *config)
{
  struct m860_enet_struct *sc;
  struct ifnet *ifp;
  int mtu;
  int i;
  
  /*
   * Find a free driver
   */
  for (i = 0 ; i < NIFACES ; i++) {
    sc = &enet_driver[i];
    ifp = &sc->arpcom.ac_if;
    if (ifp->if_softc == NULL)
      break;
  }
  if (i >= NIFACES) {
    printf ("Too many SCC drivers.\n");
    return 0;
  }
  
  /*
   * Process options
   */
  if (config->hardware_address) {
    memcpy (sc->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);
  }
  else {
    sc->arpcom.ac_enaddr[0] = 0x44;
    sc->arpcom.ac_enaddr[1] = 0x22;
    sc->arpcom.ac_enaddr[2] = 0x33;
    sc->arpcom.ac_enaddr[3] = 0x33;
    sc->arpcom.ac_enaddr[4] = 0x22;
    sc->arpcom.ac_enaddr[5] = 0x44;
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
  ifp->if_unit = i + 1;
  ifp->if_name = "eth";
  ifp->if_mtu = mtu;
  ifp->if_init = scc_init;
  ifp->if_ioctl = scc_ioctl;
  ifp->if_start = m860_enet_start;
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

int
rtems_fec_driver_attach (struct rtems_bsdnet_ifconfig *config)
{
  struct m860_enet_struct *sc;
  struct ifnet *ifp;
  int mtu;
  
  /*
   * Find a free driver
   */
  sc = &enet_driver[0];
  ifp = &sc->arpcom.ac_if;
  if (ifp->if_softc != NULL)
    return 0;

  
  /*
   * Process options
   */
  if (config->hardware_address) {
    memcpy (sc->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);
  }
  else {
    sc->arpcom.ac_enaddr[0] = 0x44;
    sc->arpcom.ac_enaddr[1] = 0x22;
    sc->arpcom.ac_enaddr[2] = 0x33;
    sc->arpcom.ac_enaddr[3] = 0x33;
    sc->arpcom.ac_enaddr[4] = 0x22;
    sc->arpcom.ac_enaddr[5] = 0x44;
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
  ifp->if_unit = 1;
  ifp->if_name = "eth";
  ifp->if_mtu = mtu;
  ifp->if_init = fec_init;
  ifp->if_ioctl = fec_ioctl;
  ifp->if_start = m860_enet_start;
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

int
rtems_enet_driver_attach (struct rtems_bsdnet_ifconfig *config)
{
  int i;

  if ((m860.fec.mii_data & 0xffff) == 0x2000) {
/*    rtems_scc1_driver_attach(config);*/
    return rtems_fec_driver_attach(config);
  }
  else {
    return rtems_scc1_driver_attach(config);
  }
}

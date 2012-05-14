/*
 * RTEMS/TCPIP driver for MPC8260 SCC
 *
 * Modified for MPC8260 by Andy Dachs <a.dachs@sstl.co.uk>
 * Surrey Satellite Technology Limited
 *
 *   On the ADS board the ethernet interface is connected to FCC2
 *   but in my application I want TCP over HDLC (see README)
 *   so will use SCC3 as the network interface. I have other plans
 *   for the FCCs so am unlikely to add true ethernet support to
 *   this BSP.  Contributions welcome!
 *
 * Modified for MPC860 by Jay Monkman (jmonkman@frasca.com)
 *
 *  This supports ethernet on either SCC1 or the FEC of the MPC860T.
 *  Right now, we only do 10 Mbps, even with the FEC. The function
 *  rtems_m860_enet_driver_attach determines which one to use. Currently,
 *  only one may be used at a time.
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 */
#include <bsp.h>
#include <bsp/irq.h>
#include <mpc8260.h>
#include <mpc8260/cpm.h>
#include <stdio.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/bspIo.h>

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <errno.h>

#include <net/if.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>

#include "if_hdlcsubr.h"

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

extern void m8xx_dump_brgs( void );

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
 * Receive buffer size -- Allow for a full ethernet packet plus CRC (1518).
 * Round off to nearest multiple of RBUF_ALIGN.
 */
#define MAX_MTU_SIZE	1518
/*#define MAX_MTU_SIZE	2050*/
#define RBUF_ALIGN	4
#define RBUF_SIZE       ((MAX_MTU_SIZE + RBUF_ALIGN) & ~RBUF_ALIGN)

#if (MCLBYTES < RBUF_SIZE)
# error "Driver must have MCLBYTES > RBUF_SIZE"
#endif

/*
 * Per-device data
 */
struct m8260_hdlc_struct {
        struct ifnet            ac_if;
        struct mbuf             **rxMbuf;
        struct mbuf             **txMbuf;
        int                     acceptBroadcast;
        int                     rxBdCount;
        int                     txBdCount;
        int                     txBdHead;
        int                     txBdTail;
        int                     txBdActiveCount;
        m8260BufferDescriptor_t  *rxBdBase;
        m8260BufferDescriptor_t  *txBdBase;
        rtems_id                rxDaemonTid;
        rtems_id                txDaemonTid;

        /*
         * Statistics
         */
        unsigned long   rxNotFirst;
        unsigned long   rxNotLast;
        unsigned long   rxInterrupts;
        unsigned long   rxGiant;
        unsigned long   rxNonOctet;
	unsigned long	rxAbort;
        unsigned long   rxBadCRC;
        unsigned long   rxOverrun;
        unsigned long   rxLostCarrier;
        unsigned long   txInterrupts;
        unsigned long   txUnderrun;
        unsigned long   txLostCarrier;
        unsigned long   txRawWait;
};
static struct m8260_hdlc_struct hdlc_driver[NIFACES];

static void  m8xx_scc3_hdlc_on(const rtems_irq_connect_data* ptr)
{
}

static void  m8xx_scc3_hdlc_off(const rtems_irq_connect_data* ptr)
{
  /*
   * Please put relevant code there
   */
}

static int  m8xx_scc3_hdlc_isOn(const rtems_irq_connect_data* ptr)
{
  return BSP_irq_enabled_at_cpm (ptr->name);
}

/*
 * SCC interrupt handler
 * TBD: Can we work out which SCC generated the interrupt from the
 *      value of v? If so we can use the same handler for multiple
 *      SCCs.
 */
static void
m8xx_scc3_interrupt_handler (rtems_irq_hdl_param unused)
{
  /*
   * Frame received?
   */
  if ((m8260.scc3.sccm & M8260_SCCE_RXF) &&
      (m8260.scc3.scce & M8260_SCCE_RXF) ) {
    m8260.scc3.scce = M8260_SCCE_RXF;
/*    m8260.scc3.sccm &= ~M8260_SCCE_RXF; */
    hdlc_driver[0].rxInterrupts++;
    rtems_event_send (hdlc_driver[0].rxDaemonTid, INTERRUPT_EVENT);
/*
    printk( "Rx " );
*/
  }

  /*
   * Buffer transmitted or transmitter error?
   */
  if ((m8260.scc3.sccm & (M8260_SCCE_TX | M8260_SCCE_TXE) ) &&
      (m8260.scc3.scce & (M8260_SCCE_TX | M8260_SCCE_TXE) )) {
    m8260.scc3.scce = M8260_SCCE_TX | M8260_SCCE_TXE;
/*    m8260.scc3.sccm &= ~(M8260_SCCE_TX | M8260_SCCE_TXE); */
    hdlc_driver[0].txInterrupts++;
    rtems_event_send (hdlc_driver[0].txDaemonTid, INTERRUPT_EVENT);
/*
    printk( "Tx " );
*/
  }

#if 0
  m8260.sipnr_l = M8260_SIMASK_SCC3; /* Clear SCC3 interrupt-in-service bit */
#endif
}

static rtems_irq_connect_data hdlcSCC3IrqData = {
  BSP_CPM_IRQ_SCC3,
  (rtems_irq_hdl) m8xx_scc3_interrupt_handler,
  NULL,
  (rtems_irq_enable) m8xx_scc3_hdlc_on,
  (rtems_irq_disable) m8xx_scc3_hdlc_off,
  (rtems_irq_is_enabled)m8xx_scc3_hdlc_isOn
};

/*
 * Initialize the SCC hardware
 * Configure I/O ports for SCC3
 * Internal Tx clock, External Rx clock
 */
static void
m8260_scc_initialize_hardware (struct m8260_hdlc_struct *sc)
{
  int i;
  int brg;

  rtems_status_code status;

  /* RxD PB14 */
  m8260.pparb |=  0x00020000;
  m8260.psorb &= ~0x00020000;
  m8260.pdirb &= ~0x00020000;

  /* RxC (CLK5) PC27 */
  m8260.pparc |=  0x00000010;
  m8260.psorc &= ~0x00000010;
  m8260.pdirc &= ~0x00000010;

  /* TxD PD24 and TxC PD10 (BRG4) */
  m8260.ppard |=  0x00200080;
  m8260.psord |=  0x00200000;
  m8260.psord &= ~0x00000080;
  m8260.pdird |=  0x00200080;

  /* External Rx Clock from CLK5 */
  if( m8xx_get_clk( M8xx_CLK_5 ) == -1 )
    printk( "Error allocating CLK5 for network device.\n" );
  else
    m8260.cmxscr |= 0x00002000;

  /* Internal Tx Clock from BRG4 */
  if( (brg = m8xx_get_brg(M8xx_BRG_4, 8000000 )) == -1 )
    printk( "Error allocating BRG for network device\n" );
  else
    m8260.cmxscr |= ((unsigned)brg << 8);

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
  sc->rxBdBase = m8xx_bd_allocate (sc->rxBdCount);
  sc->txBdBase = m8xx_bd_allocate (sc->txBdCount);

  m8260.scc3p.rbase = (char *)sc->rxBdBase - (char *)&m8260;
  m8260.scc3p.tbase = (char *)sc->txBdBase - (char *)&m8260;

  /*
   * Send "Init parameters" command
   */

  m8xx_cp_execute_cmd (M8260_CR_OP_INIT_RX_TX | M8260_CR_SCC3 );

  /*
   * Set receive and transmit function codes
   */
  m8260.scc3p.rfcr = M8260_RFCR_MOT | M8260_RFCR_60X_BUS;
  m8260.scc3p.tfcr = M8260_TFCR_MOT | M8260_TFCR_60X_BUS;

  /*
   * Set maximum receive buffer length
   */
  m8260.scc3p.mrblr = RBUF_SIZE;

  m8260.scc3p.un.hdlc.c_mask = 0xF0B8;
  m8260.scc3p.un.hdlc.c_pres = 0xFFFF;
  m8260.scc3p.un.hdlc.disfc  = 0;
  m8260.scc3p.un.hdlc.crcec  = 0;
  m8260.scc3p.un.hdlc.abtsc  = 0;
  m8260.scc3p.un.hdlc.nmarc  = 0;
  m8260.scc3p.un.hdlc.retrc  = 0;
  m8260.scc3p.un.hdlc.rfthr  = 1;
  m8260.scc3p.un.hdlc.mflr   = RBUF_SIZE;

  m8260.scc3p.un.hdlc.hmask  = 0x0000;	/* promiscuous */

  m8260.scc3p.un.hdlc.haddr1 = 0xFFFF;	/* Broadcast address */
  m8260.scc3p.un.hdlc.haddr2 = 0xFFFF;	/* Station address */
  m8260.scc3p.un.hdlc.haddr3 = 0xFFFF;	/* Dummy */
  m8260.scc3p.un.hdlc.haddr4 = 0xFFFF;	/* Dummy */

  /*
   * Send "Init parameters" command
   */
/*
  m8xx_cp_execute_cmd (M8260_CR_OP_INIT_RX_TX | M8260_CR_SCC3 );
*/

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

  m8260.scc3.sccm = 0;     /* No interrupts unmasked till necessary */

  /*
   * Clear any outstanding events
   */
  m8260.scc3.scce = 0xFFFF;

  /*
   * Set up interrupts
   */
  status = BSP_install_rtems_irq_handler (&hdlcSCC3IrqData);
/*
  printk( "status = %d, Success = %d\n", status, RTEMS_SUCCESSFUL );
*/
  if (status != 1 /*RTEMS_SUCCESSFUL*/ ) {
    rtems_panic ("Can't attach M8260 SCC3 interrupt handler: %s\n",
		 rtems_status_text (status));
  }
  m8260.scc3.sccm = 0;     /* No interrupts unmasked till necessary */

  m8260.scc3.gsmr_h  = 0;
  m8260.scc3.gsmr_l  = 0x10000000;
  m8260.scc3.dsr     = 0x7E7E;	/* flag character */
  m8260.scc3.psmr    = 0x2000;	/* 2 flags between Tx'd frames */

/*  printk("scc3 init\n" ); */

  m8260.scc3.gsmr_l |=  0x00000030;  /* Set ENR and ENT to enable Rx and Tx */

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
m8260Enet_retire_tx_bd (struct m8260_hdlc_struct *sc)
{
  uint16_t         status;
  int i;
  int nRetired;
  struct mbuf *m, *n;

  i = sc->txBdTail;
  nRetired = 0;
  while ((sc->txBdActiveCount != 0)
         &&  (((status = (sc->txBdBase + i)->status) & M8260_BD_READY) == 0)) {
    /*
     * See if anything went wrong
     */
    if (status & (M8260_BD_UNDERRUN |
                  M8260_BD_CTS_LOST)) {
      /*
       * Check for errors which stop the transmitter.
       */
      if( status & M8260_BD_UNDERRUN ) {
          hdlc_driver[0].txUnderrun++;

        /*
         * Restart the transmitter
         */
        /* FIXME: this should get executed only if using the SCC */
        m8xx_cp_execute_cmd (M8260_CR_OP_RESTART_TX | M8260_CR_SCC3);
      }
      if (status & M8260_BD_CTS_LOST)
        hdlc_driver[0].txLostCarrier++;
    }
    nRetired++;
    if (status & M8260_BD_LAST) {
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
  struct m8260_hdlc_struct *sc = (struct m8260_hdlc_struct *)arg;
  struct ifnet *ifp = &sc->ac_if;
  struct mbuf *m;
  uint16_t         status;
  m8260BufferDescriptor_t *rxBd;
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
    rxBd->status = M8260_BD_EMPTY | M8260_BD_INTERRUPT;
    if (++rxBdIndex == sc->rxBdCount) {
      rxBd->status |= M8260_BD_WRAP;
      break;
    }
  }

/*
  m8260.scc3.sccm |= M8260_SCCE_RXF;
*/

  /*
   * Input packet handling loop
   */
  rxBdIndex = 0;
  for (;;) {
    rxBd = sc->rxBdBase + rxBdIndex;

    /*
     * Wait for packet if there's not one ready
     */
    if ((status = rxBd->status) & M8260_BD_EMPTY) {
      /*
       * Clear old events
       */

      m8260.scc3.scce = M8260_SCCE_RXF;

      /*
       * Wait for packet
       * Note that the buffer descriptor is checked
       * *before* the event wait -- this catches the
       * possibility that a packet arrived between the
       * `if' above, and the clearing of the event register.
       */
      while ((status = rxBd->status) & M8260_BD_EMPTY) {
        rtems_event_set events;

        /*
         * Unmask RXF (Full frame received) event
         */
        m8260.scc3.sccm |= M8260_SCCE_RXF;

/*        printk( "Rxdwait "); */

        rtems_bsdnet_event_receive (INTERRUPT_EVENT,
                                    RTEMS_WAIT|RTEMS_EVENT_ANY,
                                    RTEMS_NO_TIMEOUT,
                                    &events);

/*        printk( "Rxd " ); */
      }
    }

    /*
     * Check that packet is valid
     */
    if ((status & (M8260_BD_LAST |
                   M8260_BD_FIRST_IN_FRAME |
    		   M8260_BD_LONG |
                   M8260_BD_NONALIGNED |
                   M8260_BD_ABORT |
                   M8260_BD_CRC_ERROR |
                   M8260_BD_OVERRUN /*|
                   M8260_BD_CARRIER_LOST*/)) ==
                   (M8260_BD_LAST |
                   M8260_BD_FIRST_IN_FRAME ) ) {

/*      printk( "RxV " ); */

/*
 * Invalidate the buffer for this descriptor
 */

      rtems_cache_invalidate_multiple_data_lines((void *)rxBd->buffer, rxBd->length);

      m = sc->rxMbuf[rxBdIndex];

      /* strip off HDLC CRC */
      m->m_len = m->m_pkthdr.len = rxBd->length - sizeof(uint16_t);

      hdlc_input( ifp, m );

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
      printk( "RxErr[%04X,%d]", status, rxBd->length );
      /*
       * Something went wrong with the reception
       */
      if (!(status & M8260_BD_LAST))
        sc->rxNotLast++;
      if (!(status & M8260_BD_FIRST_IN_FRAME))
        sc->rxNotFirst++;

      if (status & M8260_BD_LONG)
        sc->rxGiant++;
      if (status & M8260_BD_NONALIGNED)
        sc->rxNonOctet++;
      if (status & M8260_BD_ABORT)
        sc->rxAbort++;
      if (status & M8260_BD_CRC_ERROR)
        sc->rxBadCRC++;
      if (status & M8260_BD_OVERRUN)
        sc->rxOverrun++;
      if (status & M8260_BD_CARRIER_LOST)
        sc->rxLostCarrier++;
    }

    /*
     * Reenable the buffer descriptor
     */
    rxBd->status = (status & (M8260_BD_WRAP | M8260_BD_INTERRUPT)) |
                    M8260_BD_EMPTY;

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
  struct m8260_hdlc_struct *sc = ifp->if_softc;
  volatile m8260BufferDescriptor_t *firstTxBd, *txBd;
  struct mbuf *l = NULL;
  uint16_t         status;
  int nAdded;

  /*
   * Free up buffer descriptors
   */
  m8260Enet_retire_tx_bd (sc);

  /*
   * Set up the transmit buffer descriptors.
   * No need to pad out short packets since the
   * hardware takes care of that automatically.
   * No need to copy the packet to a contiguous buffer
   * since the hardware is capable of scatter/gather DMA.
   */
  nAdded = 0;
  txBd = firstTxBd = sc->txBdBase + sc->txBdHead;

/*
  m8260.scc3.sccm |= (M8260_SCCE_TX | M8260_SCCE_TXE);
*/

  for (;;) {
    /*
     * Wait for buffer descriptor to become available.
     */
    if ((sc->txBdActiveCount + nAdded) == sc->txBdCount) {
      /*
       * Clear old events
       */
      m8260.scc3.scce = M8260_SCCE_TX | M8260_SCCE_TXE;

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
      m8260Enet_retire_tx_bd (sc);
      while ((sc->txBdActiveCount + nAdded) == sc->txBdCount) {
        rtems_event_set events;

        /*
         * Unmask TX (buffer transmitted) event
         */
        m8260.scc3.sccm |= (M8260_SCCE_TX | M8260_SCCE_TXE);

        rtems_bsdnet_event_receive (INTERRUPT_EVENT,
                                    RTEMS_WAIT|RTEMS_EVENT_ANY,
                                    RTEMS_NO_TIMEOUT,
                                    &events);
        m8260Enet_retire_tx_bd (sc);
      }
    }

    /*
     * Don't set the READY flag till the
     * whole packet has been readied.
     */
    status = nAdded ? M8260_BD_READY : 0;

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

      rtems_cache_flush_multiple_data_lines((void *)txBd->buffer, txBd->length);

/* throw off the header for Ethernet Emulation mode */
/*
      txBd->buffer = mtod (m, void *);
      txBd->buffer += sizeof( struct ether_header ) + 2;
      txBd->length = m->m_len - sizeof( struct ether_header ) - 2;
*/
      sc->txMbuf[sc->txBdHead] = m;
      nAdded++;
      if (++sc->txBdHead == sc->txBdCount) {
        status |= M8260_BD_WRAP;
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
        status |= M8260_BD_LAST | M8260_BD_TX_CRC | M8260_BD_INTERRUPT;
        txBd->status = status;
        firstTxBd->status |= M8260_BD_READY;
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
  struct m8260_hdlc_struct *sc = (struct m8260_hdlc_struct *)arg;
  struct ifnet *ifp = &sc->ac_if;
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

/*
 * Send packet (caller provides header).
 */
static void
m8260_hdlc_start (struct ifnet *ifp)
{
  struct m8260_hdlc_struct *sc = ifp->if_softc;

  rtems_event_send (sc->txDaemonTid, START_TRANSMIT_EVENT);
  ifp->if_flags |= IFF_OACTIVE;
}

/*
 * Initialize and start the device
 */
static void
scc_init (void *arg)
{
  struct m8260_hdlc_struct *sc = arg;
  struct ifnet *ifp = &sc->ac_if;

  if (sc->txDaemonTid == 0) {

    /*
     * Set up SCC hardware
     */
    m8260_scc_initialize_hardware (sc);

    /*
     * Start driver tasks
     */
    sc->txDaemonTid = rtems_bsdnet_newproc ("SCtx", 4096, scc_txDaemon, sc);
    sc->rxDaemonTid = rtems_bsdnet_newproc ("SCrx", 4096, scc_rxDaemon, sc);

  }

#if 0
  /*
   * Set flags appropriately
   */
  if (ifp->if_flags & IFF_PROMISC)
    m8260.scc3.psmr |= 0x200;
  else
    m8260.scc3.psmr &= ~0x200;
#endif

  /*
   * Tell the world that we're running.
   */
  ifp->if_flags |= IFF_RUNNING;

  /*
   * Enable receiver and transmitter
   */
  m8260.scc3.gsmr_l |= 0x30;
}

/*
 * Stop the device
 */
static void
scc_stop (struct m8260_hdlc_struct *sc)
{
  struct ifnet *ifp = &sc->ac_if;

  ifp->if_flags &= ~IFF_RUNNING;

  /*
   * Shut down receiver and transmitter
   */
  m8260.scc3.gsmr_l &= ~0x30;
}

/*
 * Show interface statistics
 */
static void
hdlc_stats (struct m8260_hdlc_struct *sc)
{
  printf ("      Rx Interrupts:%-8lu", sc->rxInterrupts);
  printf ("              Giant:%-8lu", sc->rxGiant);
  printf ("          Non-octet:%-8lu\n", sc->rxNonOctet);
  printf ("            Bad CRC:%-8lu", sc->rxBadCRC);
  printf ("            Overrun:%-8lu", sc->rxOverrun);
  printf ("         No Carrier:%-8lu\n", sc->rxLostCarrier);
  printf ("          Discarded:%-8lu\n", (unsigned long)m8260.scc3p.un.hdlc.disfc);

  printf ("      Tx Interrupts:%-8lu", sc->txInterrupts);
  printf ("         No Carrier:%-8lu", sc->txLostCarrier);
  printf ("           Underrun:%-8lu\n", sc->txUnderrun);
  printf ("    Raw output wait:%-8lu\n", sc->txRawWait);
}

/*
 * Driver ioctl handler
 */
static int
scc_ioctl (struct ifnet *ifp, ioctl_command_t command, caddr_t data)
{
  struct m8260_hdlc_struct *sc = ifp->if_softc;
  int error = 0;

  switch (command) {
  case SIOCGIFADDR:
  case SIOCSIFADDR:
    hdlc_ioctl (ifp, command, data);
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
    hdlc_stats (sc);
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
rtems_scc3_driver_attach (struct rtems_bsdnet_ifconfig *config)
{
  struct m8260_hdlc_struct *sc;
  struct ifnet *ifp;
  int mtu;
  int i;

  /*
   * Find a free driver
   */
  for (i = 0 ; i < NIFACES ; i++) {
    sc = &hdlc_driver[i];
    ifp = &sc->ac_if;
    if (ifp->if_softc == NULL)
      break;
  }
  if (i >= NIFACES) {
    printf ("Too many SCC drivers.\n");
    return 0;
  }

#if 0
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
#endif

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
  ifp->if_start = m8260_hdlc_start;
  ifp->if_output = hdlc_output;
  ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX | /*IFF_PROMISC |*/ IFF_NOARP;
  if (ifp->if_snd.ifq_maxlen == 0)
    ifp->if_snd.ifq_maxlen = ifqmaxlen;

  /*
   * Attach the interface
   */
  if_attach (ifp);
  hdlc_ifattach (ifp);
  return 1;
};

int
rtems_enet_driver_attach(struct rtems_bsdnet_ifconfig *config, int attaching)
{
  return rtems_scc3_driver_attach( config );

/*
  if ((m8260.fec.mii_data & 0xffff) == 0x2000) {
    return rtems_fec_driver_attach(config);
  }
  else {
    return rtems_scc1_driver_attach(config);
  }
*/
}

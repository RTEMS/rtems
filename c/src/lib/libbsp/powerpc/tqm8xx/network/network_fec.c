/*===============================================================*\
| Project: RTEMS TQM8xx BSP                                       |
+-----------------------------------------------------------------+
| This file has been adapted to MPC8xx by                         |
|    Thomas Doerfler <Thomas.Doerfler@embedded-brains.de>         |
|                    Copyright (c) 2008                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
|                                                                 |
| See the other copyright notice below for the original parts.    |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the console driver                           |
\*===============================================================*/
/* derived from: */
/*
 * RTEMS/TCPIP driver for MPC8xx Ethernet
 *
 * split into separate driver files for SCC and FEC by
 * Thomas Doerfler <Thomas.Doerfler@embedded-brains.de>
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
#include <rtems/rtems_mii_ioctl.h>

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

#define FEC_WATCHDOG_TIMEOUT 5 /* check media every 5 seconds */
/*
 * Per-device data
 */
struct m8xx_fec_enet_struct {
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
   * MDIO/Phy info
   */
  struct rtems_mdio_info mdio_info;
  int phy_default;
  int media_state; /* (last detected) state of media */
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
static struct m8xx_fec_enet_struct enet_driver[NIFACES];

/* declare ioctl function for internal use */
static int fec_ioctl (struct ifnet *ifp,
		      ioctl_command_t command, caddr_t data);
/***************************************************************************\
|  MII Management access functions                                          |
\***************************************************************************/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void fec_mdio_init
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   initialize the MII interface                                            |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct m8xx_fec_enet_struct *sc     /* control structure                */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/
{

  /* Set FEC registers for MDIO communication */
  /*
   * set clock divider
   */
  m8xx.fec.mii_speed = BSP_bus_frequency / 25000000 / 2 + 1;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int fec_mdio_read
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   read register of a phy                                                  |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int phy,                              /* PHY number to access or -1       */
 void *uarg,                           /* unit argument                    */
 unsigned reg,                         /* register address                 */
 uint32_t *pval                        /* ptr to read buffer               */
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    0, if ok, else error                                                   |
\*=========================================================================*/
{
  struct m8xx_fec_enet_struct *sc = uarg;/* control structure            */

  /*
   * make sure we work with a valid phy
   */
  if (phy == -1) {
    /*
     * set default phy number: 0
     */
    phy = sc->phy_default;
  }
  if ( (phy < 0) || (phy > 31)) {
    /*
     * invalid phy number
     */
    return EINVAL;
  }
  /*
   * clear MII transfer event bit
   */
  m8xx.fec.ievent = M8xx_FEC_IEVENT_MII;
  /*
   * set access command, data, start transfer
   */
  m8xx.fec.mii_data = (M8xx_FEC_MII_DATA_ST    |
		       M8xx_FEC_MII_DATA_OP_RD |
		       M8xx_FEC_MII_DATA_PHYAD(phy) |
		       M8xx_FEC_MII_DATA_PHYRA(reg) |
		       M8xx_FEC_MII_DATA_WDATA(0));

  /*
   * wait for cycle to terminate
   */
  do {
    rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
  }  while (0 == (m8xx.fec.ievent & M8xx_FEC_IEVENT_MII));

  /*
   * fetch read data, if available
   */
  if (pval != NULL) {
    *pval = M8xx_FEC_MII_DATA_RDATA(m8xx.fec.mii_data);
  }
  return 0;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int fec_mdio_write
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   write register of a phy                                                 |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int phy,                              /* PHY number to access or -1       */
 void *uarg,                           /* unit argument                    */
 unsigned reg,                         /* register address                 */
 uint32_t val                          /* write value                      */
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    0, if ok, else error                                                   |
\*=========================================================================*/
{
  struct m8xx_fec_enet_struct *sc = uarg;/* control structure            */

  /*
   * make sure we work with a valid phy
   */
  if (phy == -1) {
    /*
     * set default phy number: 0
     */
    phy = sc->phy_default;
  }
  if ( (phy < 0) || (phy > 31)) {
    /*
     * invalid phy number
     */
    return EINVAL;
  }
  /*
   * clear MII transfer event bit
   */
  m8xx.fec.ievent = M8xx_FEC_IEVENT_MII;
  /*
   * set access command, data, start transfer
   */
  m8xx.fec.mii_data = (M8xx_FEC_MII_DATA_ST    |
		       M8xx_FEC_MII_DATA_OP_WR |
		       M8xx_FEC_MII_DATA_PHYAD(phy) |
		       M8xx_FEC_MII_DATA_PHYRA(reg) |
		       M8xx_FEC_MII_DATA_WDATA(val));

  /*
   * wait for cycle to terminate
   */
  do {
    rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
  }  while (0 == (m8xx.fec.ievent & M8xx_FEC_IEVENT_MII));

  return 0;
}

/*
 * FEC interrupt handler
 */
static void m8xx_fec_interrupt_handler (void *unused)
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

/*
 * Please organize FEC controller code better by moving code from
 * m860_fec_initialize_hardware to m8xx_fec_ethernet_on
 */
static void m8xx_fec_ethernet_on(const rtems_irq_connect_data* ptr){};
static void m8xx_fec_ethernet_off(const rtems_irq_connect_data* ptr){};
static int m8xx_fec_ethernet_isOn (const rtems_irq_connect_data* ptr)
{
  return 1;
}

static rtems_irq_connect_data ethernetFECIrqData = {
  BSP_FAST_ETHERNET_CTRL,
  m8xx_fec_interrupt_handler,
  NULL,
  m8xx_fec_ethernet_on,
  m8xx_fec_ethernet_off,
  m8xx_fec_ethernet_isOn
};

static void
m8xx_fec_initialize_hardware (struct m8xx_fec_enet_struct *sc)
{
  int i;
  unsigned char *hwaddr;

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
   *   Full duplex
   *   No heartbeat
   */
  m8xx.fec.x_cntrl = M8xx_FEC_X_CNTRL_FDEN;

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
static void fec_rxDaemon (void *arg)
{
  struct m8xx_fec_enet_struct *sc = (struct m8xx_fec_enet_struct *)arg;
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
m8xx_fec_Enet_retire_tx_bd (struct m8xx_fec_enet_struct *sc)
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

static void fec_sendpacket (struct ifnet *ifp, struct mbuf *m)
{
  struct m8xx_fec_enet_struct *sc = ifp->if_softc;
  volatile m8xxBufferDescriptor_t *firstTxBd, *txBd;
  /*  struct mbuf *l = NULL; */
  uint16_t   status;
  int nAdded;

  /*
   * Free up buffer descriptors
   */
  m8xx_fec_Enet_retire_tx_bd (sc);

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
      m8xx_fec_Enet_retire_tx_bd (sc);
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
        m8xx_fec_Enet_retire_tx_bd (sc);
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
      rtems_cache_flush_multiple_data_lines((void *)txBd->buffer, txBd->length);

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
void fec_txDaemon (void *arg)
{
  struct m8xx_fec_enet_struct *sc = (struct m8xx_fec_enet_struct *)arg;
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
static void fec_init (void *arg)
{
  struct m8xx_fec_enet_struct *sc = arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;

  if (sc->txDaemonTid == 0) {

    /*
     * Set up FEC hardware
     */
    m8xx_fec_initialize_hardware (sc);

    /*
     * init access to phy
     */
    fec_mdio_init(sc);

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
   * init timer so the "watchdog function gets called periodically
   */
  ifp->if_timer    = 1;

  /*
   * Tell the world that we're running.
   */
  ifp->if_flags |= IFF_RUNNING;

  /*
   * Enable receiver and transmitter
   */
  m8xx.fec.ecntrl = 0x2;
}

/*
 * Send packet (caller provides header).
 */
static void
m8xx_fec_enet_start (struct ifnet *ifp)
{
  struct m8xx_fec_enet_struct *sc = ifp->if_softc;

  rtems_event_send (sc->txDaemonTid, START_TRANSMIT_EVENT);
  ifp->if_flags |= IFF_OACTIVE;
}

static void fec_stop (struct m8xx_fec_enet_struct *sc)
{
  struct ifnet *ifp = &sc->arpcom.ac_if;

  ifp->if_flags &= ~IFF_RUNNING;

  /*
   * Shut down receiver and transmitter
   */
  m8xx.fec.ecntrl = 0x0;
}

/*
 * Show interface statistics
 */
static void fec_enet_stats (struct m8xx_fec_enet_struct *sc)
{
  int media;
  int result;
  /*
   * fetch/print media info
   */
  media = IFM_MAKEWORD(0,0,0,sc->phy_default); /* fetch from default phy */

  result = fec_ioctl(&(sc->arpcom.ac_if),
		     SIOCGIFMEDIA,
		     (caddr_t)&media);
  if (result == 0) {
    rtems_ifmedia2str(media,NULL,0);
    printf ("\n");
  }

  printf ("      Rx Interrupts:%-8lu", sc->rxInterrupts);
  printf ("       Not First:%-8lu", sc->rxNotFirst);
  printf ("        Not Last:%-8lu\n", sc->rxNotLast);
  printf ("              Giant:%-8lu", sc->rxGiant);
  printf ("            Runt:%-8lu", sc->rxRunt);
  printf ("       Non-octet:%-8lu\n", sc->rxNonOctet);
  printf ("            Bad CRC:%-8lu", sc->rxBadCRC);
  printf ("         Overrun:%-8lu", sc->rxOverrun);
  printf ("       Collision:%-8lu\n", sc->rxCollision);

  printf ("      Tx Interrupts:%-8lu", sc->txInterrupts);
  printf ("        Deferred:%-8lu", sc->txDeferred);
  printf (" Missed Hearbeat:%-8lu\n", sc->txHeartbeat);
  printf ("         No Carrier:%-8lu", sc->txLostCarrier);
  printf ("Retransmit Limit:%-8lu", sc->txRetryLimit);
  printf ("  Late Collision:%-8lu\n", sc->txLateCollision);
  printf ("           Underrun:%-8lu", sc->txUnderrun);
  printf (" Raw output wait:%-8lu\n", sc->txRawWait);
}

static int fec_ioctl (struct ifnet *ifp,
		      ioctl_command_t command, caddr_t data)
{
  struct m8xx_fec_enet_struct *sc = ifp->if_softc;
  int error = 0;

  switch (command) {
    /*
     * access PHY via MII
     */
  case SIOCGIFMEDIA:
  case SIOCSIFMEDIA:
    rtems_mii_ioctl (&(sc->mdio_info),sc,command,(void *)data);
    break;
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
    fec_enet_stats (sc);
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

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int fec_mode_adapt
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   init the PHY and adapt FEC settings                                     |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct ifnet *ifp
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    0, if success                                                          |
\*=========================================================================*/
{
  int result = 0;
  struct m8xx_fec_enet_struct *sc = ifp->if_softc;
  int media = IFM_MAKEWORD( 0, 0, 0, sc->phy_default);

#ifdef DEBUG
  printf("c");
#endif
  /*
   * fetch media status
   */
  result = fec_ioctl(ifp,SIOCGIFMEDIA,(caddr_t)&media);
  if (result != 0) {
    return result;
  }
#ifdef DEBUG
  printf("C");
#endif
  /*
   * status is unchanged? then do nothing
   */
  if (media == sc->media_state) {
    return 0;
  }
  /*
   * otherwise: for the first call, try to negotiate mode
   */
  if (sc->media_state == 0) {
    /*
     * set media status: set auto negotiation -> start auto-negotiation
     */
    media = IFM_MAKEWORD(0,IFM_AUTO,0,sc->phy_default);
    result = fec_ioctl(ifp,SIOCSIFMEDIA,(caddr_t)&media);
    if (result != 0) {
      return result;
    }
    /*
     * wait for auto-negotiation to terminate
     */
    do {
      media = IFM_MAKEWORD(0,0,0,sc->phy_default);
      result = fec_ioctl(ifp,SIOCGIFMEDIA,(caddr_t)&media);
      if (result != 0) {
	return result;
      }
    } while (IFM_NONE == IFM_SUBTYPE(media));
  }

  /*
   * now set HW according to media results:
   */
  /*
   * if we are half duplex then switch to half duplex
   */
  if (0 == (IFM_FDX & IFM_OPTIONS(media))) {
    m8xx.fec.x_cntrl &= ~M8xx_FEC_X_CNTRL_FDEN;
  }
  else {
    m8xx.fec.x_cntrl |=  M8xx_FEC_X_CNTRL_FDEN;
  }
  /*
   * store current media state for future compares
   */
  sc->media_state = media;

  return 0;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static void fec_watchdog
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   periodically poll the PHY. if mode has changed,                         |
|  then adjust the FEC settings                                             |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 struct ifnet *ifp
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    1, if success                                                          |
\*=========================================================================*/
{
  fec_mode_adapt(ifp);
  ifp->if_timer    = FEC_WATCHDOG_TIMEOUT;
}

int rtems_fec_driver_attach (struct rtems_bsdnet_ifconfig *config)
{
  struct m8xx_fec_enet_struct *sc;
  struct ifnet *ifp;
  int mtu;
  int unitNumber;
  char *unitName;
  static const uint8_t maczero[] ={0,0,0,0,0,0};

  /*
   * Parse driver name
   */
  if ((unitNumber = rtems_bsdnet_parse_driver_name (config, &unitName)) < 0)
    return 0;

  /*
   * Is driver free?
   */
  if ((unitNumber <= 0) || (unitNumber > NIFACES)) {
    printk ("Bad FEC unit number.\n");
    return 0;
  }
  sc = &enet_driver[unitNumber - 1];
  ifp = &sc->arpcom.ac_if;
  if (ifp->if_softc != NULL) {
    printk ("Driver already in use.\n");
    return 0;
  }

  /*
   * Process options
   */
  if (config->hardware_address) {
    memcpy (sc->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);
  }
#ifdef BSP_HAS_TQMMON
  else if(0 != memcmp(maczero,TQM_BD_INFO.eth_addr,ETHER_ADDR_LEN)) {
    memcpy (sc->arpcom.ac_enaddr, TQM_BD_INFO.eth_addr, ETHER_ADDR_LEN);
  }
#endif
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
   * setup info about mdio interface
   */
  sc->mdio_info.mdio_r   = fec_mdio_read;
  sc->mdio_info.mdio_w   = fec_mdio_write;
  sc->mdio_info.has_gmii = 0; /* we do not support gigabit IF */
  /*
   * assume: IF 1 -> PHY 0
   */
  sc->phy_default = unitNumber-1;

  /*
   * Set up network interface values
   */
  ifp->if_softc = sc;
  ifp->if_unit = unitNumber;
  ifp->if_name = unitName;
  ifp->if_mtu = mtu;
  ifp->if_init = fec_init;
  ifp->if_ioctl = fec_ioctl;
  ifp->if_start = m8xx_fec_enet_start;
  ifp->if_output = ether_output;
  ifp->if_watchdog =  fec_watchdog; /* XXX: timer is set in "init" */
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

int rtems_fec_enet_driver_attach(struct rtems_bsdnet_ifconfig *config,
				 int attaching)
{
  /*
   * enable FEC functionality at hardware pins*
   * PD[3-15] are FEC pins
   */
  if (attaching) {
    m8xx.pdpar |= 0x1fff;
    m8xx.pddir |= 0x1fff;
  }
  return rtems_fec_driver_attach(config);
}

/*
 * RTEMS/TCPIP driver for MCF5272 Ethernet
 *
 *  Modified for MPC860 by Jay Monkman (jmonkman@lopingdog.com)
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
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>

#include <net/if.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>

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
#define TX_BUF_COUNT     16
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

typedef struct {
    uint16_t status;
    uint16_t length;
    void *buffer;
} bd_t;
#define MCF5272_BD_READY       (bit(15))
#define MCF5272_BD_TO1         (bit(14))
#define MCF5272_BD_WRAP        (bit(13))
#define MCF5272_BD_TO2         (bit(12))
#define MCF5272_BD_LAST        (bit(11))
#define MCF5272_BD_TX_CRC          (bit(10))
#define MCF5272_BD_DEFER       (bit(9))
#define MCF5272_BD_HEARTBEAT   (bit(8))
#define MCF5272_BD_LATE_COLLISION       (bit(7))
#define MCF5272_BD_RETRY_LIMIT          (bit(6))
#define MCF5272_BD_UNDERRUN          (bit(1))
#define MCF5272_BD_CARRIER_LOST         (bit(0))

#define MCF5272_BD_EMPTY           (bit(15))
#define MCF5272_BD_RO1         (bit(14))
#define MCF5272_BD_WRAP        (bit(13))
#define MCF5272_BD_RO2         (bit(12))
#define MCF5272_BD_M           (bit(8))
#define MCF5272_BD_BC          (bit(7))
#define MCF5272_BD_MC          (bit(6))
#define MCF5272_BD_LONG          (bit(5))
#define MCF5272_BD_NONALIGNED          (bit(4))
#define MCF5272_BD_SHORT          (bit(3))
#define MCF5272_BD_CRC_ERROR          (bit(2))
#define MCF5272_BD_OVERRUN          (bit(1))
#define MCF5272_BD_TRUNCATED          (bit(0))


/*
 * Per-device data
 */
struct mcf5272_enet_struct {
    struct arpcom           arpcom;
    struct mbuf             **rxMbuf;
    struct mbuf             **txMbuf;
    int                     acceptBroadcast;
    int                     rxBdCount;
    int                     txBdCount;
    int                     txBdHead;
    int                     txBdTail;
    int                     txBdActiveCount;
    bd_t                   *rxBdBase;
    bd_t                   *txBdBase;
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
    unsigned long   rxTruncated;

    unsigned long   txInterrupts;
    unsigned long   txDeferred;
    unsigned long   txHeartbeat;
    unsigned long   txLateCollision;
    unsigned long   txRetryLimit;
    unsigned long   txUnderrun;
    unsigned long   txLostCarrier;
    unsigned long   txRawWait;
};
static struct mcf5272_enet_struct enet_driver[NIFACES];


void dump_enet_regs(void)
{
    printf("**************************************************************\n");
   printf("ecr:   0x%08x  eir:   0x%08x  eimr:  0x%08x  ivsr:  0x%08x\n\r",
          g_enet_regs->ecr, g_enet_regs->eir,
          g_enet_regs->eimr, g_enet_regs->ivsr);
   printf("rdar:  0x%08x  tdar:  0x%08x  mmfr:  0x%08x  mscr:  0x%08x\n\r",
          g_enet_regs->rdar, g_enet_regs->tdar,
          g_enet_regs->mmfr, g_enet_regs->mscr);
   printf("frbr:  0x%08x  frsr:  0x%08x  tfwr:  0x%08x  tfsr:  0x%08x\n\r",
          g_enet_regs->frbr, g_enet_regs->frsr,
          g_enet_regs->tfwr, g_enet_regs->tfsr);
   printf("rcr:   0x%08x  mflr:  0x%08x  tcr:   0x%08x  malr:  0x%08x\n\r",
          g_enet_regs->rcr, g_enet_regs->mflr,
          g_enet_regs->tcr, g_enet_regs->malr);
   printf("maur:  0x%08x  htur:  0x%08x  htlr:  0x%08x  erdsr: 0x%08x\n\r",
          g_enet_regs->maur, g_enet_regs->htur,
          g_enet_regs->htlr, g_enet_regs->erdsr);
   printf("etdsr: 0x%08x  emrbr: 0x%08x\n\r",
          g_enet_regs->etdsr, g_enet_regs->emrbr);
}




/*#define cp printk("%s:%d\n\r", __FUNCTION__, __LINE__) */
#define cp
#define mcf5272_bd_allocate(_n_) malloc((_n_) * sizeof(bd_t), 0, M_NOWAIT)



rtems_isr enet_rx_isr(rtems_vector_number vector)
{
    cp;
    /*
     * Frame received?
     */
    if (g_enet_regs->eir & MCF5272_ENET_EIR_RXF) {
        cp;
        g_enet_regs->eir = MCF5272_ENET_EIR_RXF;
        enet_driver[0].rxInterrupts++;
        rtems_event_send (enet_driver[0].rxDaemonTid, INTERRUPT_EVENT);
    }
    cp;
}

rtems_isr enet_tx_isr(rtems_vector_number vector)
{
    cp;
    /*
     * Buffer transmitted or transmitter error?
     */
    if (g_enet_regs->eir & MCF5272_ENET_EIR_TXF) {
        cp;
        g_enet_regs->eir = MCF5272_ENET_EIR_TXF;
        enet_driver[0].txInterrupts++;
        rtems_event_send (enet_driver[0].txDaemonTid, INTERRUPT_EVENT);
    }
    cp;
}


/*
 * Initialize the ethernet hardware
 */


static void
mcf5272_enet_initialize_hardware (struct mcf5272_enet_struct *sc)
{
    int i;
    unsigned char *hwaddr;
    uint32_t icr;
    /*
     * Issue reset to FEC
     */
    g_enet_regs->ecr=0x1;

    /*
     * Set the TX and RX fifo sizes. For now, we'll split it evenly
     */
    /* If you uncomment these, the FEC will not work right.
       g_enet_regs->r_fstart = ((g_enet_regs->r_bound & 0x3ff) >> 2) & 0x3ff;
       g_enet_regs->x_fstart = 0;
    */

    /* Copy mac address to device */

    hwaddr = sc->arpcom.ac_enaddr;

    g_enet_regs->malr = (hwaddr[0] << 24 |
                         hwaddr[1] << 16 |
                         hwaddr[2] << 8 |
                         hwaddr[3]);
    g_enet_regs->maur = (hwaddr[4] << 24 |
                         hwaddr[5] << 16);

    /*
     * Clear the hash table
     */
    g_enet_regs->htlr = 0;
    g_enet_regs->htur  = 0;

    /*
     * Set up receive buffer size
     */
    g_enet_regs->emrbr = 0x5f0; /* set to 1520 */

    /*
     * Allocate mbuf pointers
     */
    sc->rxMbuf = malloc (sc->rxBdCount * sizeof *sc->rxMbuf,
                         M_MBUF, M_NOWAIT);
    sc->txMbuf = malloc (sc->txBdCount * sizeof *sc->txMbuf,
                         M_MBUF, M_NOWAIT);
    if (!sc->rxMbuf || !sc->txMbuf) {
        rtems_panic ("No memory for mbuf pointers");
    }

    /*
     * Set receiver and transmitter buffer descriptor bases
     */
    sc->rxBdBase = mcf5272_bd_allocate(sc->rxBdCount);
    sc->txBdBase = mcf5272_bd_allocate(sc->txBdCount);
    g_enet_regs->erdsr = (int)sc->rxBdBase;
    g_enet_regs->etdsr = (int)sc->txBdBase;

    /*
     * Set up Receive Control Register:
     *   Not promiscuous mode
     *   MII mode
     *   Full duplex
     *   No loopback
     */
    g_enet_regs->rcr = 0x00000004;

    /*
     * Set up Transmit Control Register:
     *   Full duplex
     *   No heartbeat
     */
    g_enet_regs->tcr = 0x00000004;

    /*
     * Set MII speed to 2.5 MHz for 25 Mhz system clock
     */
    g_enet_regs->mscr = 0x0a;
    g_enet_regs->mmfr = 0x58021000;

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
     * Mask all FEC interrupts and clear events
     */
    g_enet_regs->eimr = (MCF5272_ENET_EIR_TXF |
                        MCF5272_ENET_EIR_RXF);
    g_enet_regs->eir = ~0;

    /*
     * Set up interrupts
     */
    set_vector(enet_rx_isr, BSP_INTVEC_ERX, 1);
    set_vector(enet_tx_isr, BSP_INTVEC_ETX, 1);

    /* Configure ethernet interrupts */
    icr = g_intctrl_regs->icr3;
    icr = icr & ~((MCF5272_ICR3_ERX_MASK | MCF5272_ICR3_ERX_PI) |
                  (MCF5272_ICR3_ETX_MASK | MCF5272_ICR3_ETX_PI));
    icr |= ((MCF5272_ICR3_ERX_IPL(BSP_INTLVL_ERX) | MCF5272_ICR3_ERX_PI)|
            (MCF5272_ICR3_ETX_IPL(BSP_INTLVL_ETX) | MCF5272_ICR3_ETX_PI));
    g_intctrl_regs->icr3 = icr;

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
mcf5272_enet_retire_tx_bd (struct mcf5272_enet_struct *sc)
{
    uint16_t status;
    int i;
    int nRetired;
    struct mbuf *m, *n;

    i = sc->txBdTail;
    nRetired = 0;
    while ((sc->txBdActiveCount != 0) &&
           (((status = sc->txBdBase[i].status) & MCF5272_BD_READY) == 0)) {
        /*
         * See if anything went wrong
         */
        if (status & (MCF5272_BD_DEFER |
                      MCF5272_BD_HEARTBEAT |
                      MCF5272_BD_LATE_COLLISION |
                      MCF5272_BD_RETRY_LIMIT |
                      MCF5272_BD_UNDERRUN |
                      MCF5272_BD_CARRIER_LOST)) {
            /*
             * Check for errors which stop the transmitter.
             */
            if (status & (MCF5272_BD_LATE_COLLISION |
                          MCF5272_BD_RETRY_LIMIT |
                          MCF5272_BD_UNDERRUN)) {
                if (status & MCF5272_BD_LATE_COLLISION) {
                    enet_driver[0].txLateCollision++;
                }
                if (status & MCF5272_BD_RETRY_LIMIT) {
                    enet_driver[0].txRetryLimit++;
                }
                if (status & MCF5272_BD_UNDERRUN) {
                    enet_driver[0].txUnderrun++;
                }
            }
            if (status & MCF5272_BD_DEFER) {
                enet_driver[0].txDeferred++;
            }
            if (status & MCF5272_BD_HEARTBEAT) {
                enet_driver[0].txHeartbeat++;
            }
            if (status & MCF5272_BD_CARRIER_LOST) {
                enet_driver[0].txLostCarrier++;
            }
        }
        nRetired++;
        if (status & MCF5272_BD_LAST) {
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
        if (++i == sc->txBdCount) {
            i = 0;
        }
    }
}

static void
mcf5272_enet_rxDaemon (void *arg)
{
    struct mcf5272_enet_struct *sc = (struct mcf5272_enet_struct *)arg;
    struct ifnet *ifp = &sc->arpcom.ac_if;
    struct mbuf *m;
    uint16_t status;
    bd_t *rxBd;
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
        rxBd->status = MCF5272_BD_EMPTY;
        g_enet_regs->rdar = 0x1000000;
        if (++rxBdIndex == sc->rxBdCount) {
            rxBd->status |= MCF5272_BD_WRAP;
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
        if ((status = rxBd->status) & MCF5272_BD_EMPTY) {
            /*
             * Clear old events
             */
            g_enet_regs->eir = MCF5272_ENET_EIR_RXF;

            /*
             * Wait for packet
             * Note that the buffer descriptor is checked
             * *before* the event wait -- this catches the
             * possibility that a packet arrived between the
             * `if' above, and the clearing of the event register.
             */
            while ((status = rxBd->status) & MCF5272_BD_EMPTY) {
                rtems_event_set events;

                /*
                 * Unmask RXF (Full frame received) event
                 */
                g_enet_regs->eir |= MCF5272_ENET_EIR_RXF;

                rtems_bsdnet_event_receive (INTERRUPT_EVENT,
                                            RTEMS_WAIT|RTEMS_EVENT_ANY,
                                            RTEMS_NO_TIMEOUT,
                                            &events);
                cp;
            }
        }
    cp;

        /*
         * Check that packet is valid
         */
        if (status & MCF5272_BD_LAST) {
            /*
             * Pass the packet up the chain.
             * FIXME: Packet filtering hook could be done here.
             */
            struct ether_header *eh;

            m = sc->rxMbuf[rxBdIndex];
            m->m_len = m->m_pkthdr.len = (rxBd->length -
                                          sizeof(uint32_t) -
                                          sizeof(struct ether_header));
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
            if (!(status & MCF5272_BD_LAST)) {
                sc->rxNotLast++;
            }
            if (status & MCF5272_BD_LONG) {
                sc->rxGiant++;
            }
            if (status & MCF5272_BD_NONALIGNED) {
                sc->rxNonOctet++;
            }
            if (status & MCF5272_BD_SHORT) {
                sc->rxRunt++;
            }
            if (status & MCF5272_BD_CRC_ERROR) {
                sc->rxBadCRC++;
            }
            if (status & MCF5272_BD_OVERRUN) {
                sc->rxOverrun++;
            }
            if (status & MCF5272_BD_TRUNCATED) {
                sc->rxTruncated++;
            }
        }
        /*
         * Reenable the buffer descriptor
         */
        rxBd->status = (status & MCF5272_BD_WRAP) | MCF5272_BD_EMPTY;
        g_enet_regs->rdar = 0x1000000;
        /*
         * Move to next buffer descriptor
         */
        if (++rxBdIndex == sc->rxBdCount) {
            rxBdIndex = 0;
        }
    }
}

static void
mcf5272_enet_sendpacket (struct ifnet *ifp, struct mbuf *m)
{
    struct mcf5272_enet_struct *sc = ifp->if_softc;
    volatile bd_t *firstTxBd, *txBd;
    /*  struct mbuf *l = NULL; */
    uint16_t status;
    int nAdded;
    cp;

    /*
     * Free up buffer descriptors
     */
    mcf5272_enet_retire_tx_bd (sc);

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
    cp;
        /*
         * Wait for buffer descriptor to become available.
         */
        if ((sc->txBdActiveCount + nAdded) == sc->txBdCount) {
            /*
             * Clear old events
             */
            g_enet_regs->eir = MCF5272_ENET_EIR_TXF;

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
            mcf5272_enet_retire_tx_bd (sc);
            while ((sc->txBdActiveCount + nAdded) == sc->txBdCount) {
                rtems_event_set events;

                cp;
                /*
                 * Unmask TXB (buffer transmitted) and
                 * TXE (transmitter error) events.
                 */
                g_enet_regs->eir |= MCF5272_ENET_EIR_TXF;
                rtems_bsdnet_event_receive (INTERRUPT_EVENT,
                                            RTEMS_WAIT|RTEMS_EVENT_ANY,
                                            RTEMS_NO_TIMEOUT,
                                            &events);
                cp;
                mcf5272_enet_retire_tx_bd (sc);
            }
        }

        /*
         * Don't set the READY flag till the
         * whole packet has been readied.
         */
        status = nAdded ? MCF5272_BD_READY : 0;
        cp;

        /*
         *  FIXME: Why not deal with empty mbufs at at higher level?
         * The IP fragmentation routine in ip_output
         * can produce packet fragments with zero length.
         * I think that ip_output should be changed to get
         * rid of these zero-length mbufs, but for now,
         * I'll deal with them here.
         */
        if (m->m_len) {
            cp;
            /*
             * Fill in the buffer descriptor
             */
            txBd->buffer = mtod (m, void *);
            txBd->length = m->m_len;

            sc->txMbuf[sc->txBdHead] = m;
            nAdded++;
            if (++sc->txBdHead == sc->txBdCount) {
                status |= MCF5272_BD_WRAP;
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
            cp;
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
            cp;
            if (nAdded) {
                cp;
                status |= MCF5272_BD_LAST | MCF5272_BD_TX_CRC;
                txBd->status = status;
                firstTxBd->status |= MCF5272_BD_READY;
                g_enet_regs->tdar = 0x1000000;
                sc->txBdActiveCount += nAdded;
            }
            break;
        }
        txBd->status = status;
        txBd = sc->txBdBase + sc->txBdHead;
    }
    cp;
/*
    dump_enet_regs();
    dump_intctrl;
*/

}


void
mcf5272_enet_txDaemon (void *arg)
{
    struct mcf5272_enet_struct *sc = (struct mcf5272_enet_struct *)arg;
    struct ifnet *ifp = &sc->arpcom.ac_if;
    struct mbuf *m;
    rtems_event_set events;

    cp;
    for (;;) {
        /*
         * Wait for packet
         */
        rtems_bsdnet_event_receive (START_TRANSMIT_EVENT,
                                    RTEMS_EVENT_ANY | RTEMS_WAIT,
                                    RTEMS_NO_TIMEOUT,
                                    &events);
        cp;
        /*
         * Send packets till queue is empty
         */
    cp;
        for (;;) {
    cp;
            /*
             * Get the next mbuf chain to transmit.
             */
            IF_DEQUEUE(&ifp->if_snd, m);
            if (!m)
                break;
            mcf5272_enet_sendpacket (ifp, m);
        }
        ifp->if_flags &= ~IFF_OACTIVE;
    }
}


/*
 * Send packet (caller provides header).
 */
static void
mcf5272_enet_start (struct ifnet *ifp)
{
    struct mcf5272_enet_struct *sc = ifp->if_softc;

    cp;
    rtems_event_send (sc->txDaemonTid, START_TRANSMIT_EVENT);
    cp;
    ifp->if_flags |= IFF_OACTIVE;
}


static void
mcf5272_enet_init (void *arg)
{
    struct mcf5272_enet_struct *sc = arg;
    struct ifnet *ifp = &sc->arpcom.ac_if;

    if (sc->txDaemonTid == 0) {

        /*
         * Set up SCC hardware
         */
        mcf5272_enet_initialize_hardware (sc);

        /*
         * Start driver tasks
         */
        sc->txDaemonTid = rtems_bsdnet_newproc("SCtx",
                                               4096,
                                               mcf5272_enet_txDaemon,
                                               sc);
        sc->rxDaemonTid = rtems_bsdnet_newproc("SCrx",
                                               4096,
                                               mcf5272_enet_rxDaemon,
                                               sc);

    }

    /*
     * Set flags appropriately
     */
    if (ifp->if_flags & IFF_PROMISC) {
        g_enet_regs->rcr |= 0x8;
    } else {
        g_enet_regs->rcr &= ~0x8;
    }

    /*
     * Tell the world that we're running.
     */
    ifp->if_flags |= IFF_RUNNING;

    /*
     * Enable receiver and transmitter
     */
    g_enet_regs->ecr = 0x2;
}


static void
mcf5272_enet_stop (struct mcf5272_enet_struct *sc)
{
    struct ifnet *ifp = &sc->arpcom.ac_if;

    ifp->if_flags &= ~IFF_RUNNING;

    /*
     * Shut down receiver and transmitter
     */
    g_enet_regs->ecr = 0x0;
}


/*
 * Show interface statistics
 */
static void
enet_stats (struct mcf5272_enet_struct *sc)
{
    printf ("      Rx Interrupts:%-8lu", sc->rxInterrupts);
    printf ("       Not First:%-8lu", sc->rxNotFirst);
    printf ("        Not Last:%-8lu\n", sc->rxNotLast);
    printf ("              Giant:%-8lu", sc->rxGiant);
    printf ("            Runt:%-8lu", sc->rxRunt);
    printf ("       Non-octet:%-8lu\n", sc->rxNonOctet);
    printf ("            Bad CRC:%-8lu", sc->rxBadCRC);
    printf ("         Overrun:%-8lu", sc->rxOverrun);
    printf ("       Truncated:%-8lu\n", sc->rxTruncated);
/*    printf ("          Discarded:%-8lu\n", (unsigned long)mcf5272.scc1p.un.ethernet.disfc); */

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
mcf5272_enet_ioctl (struct ifnet *ifp, int command, caddr_t data)
{
    struct mcf5272_enet_struct *sc = ifp->if_softc;
    int error = 0;

    switch (command) {
    case SIOCGIFADDR:
    case SIOCSIFADDR:
        ether_ioctl (ifp, command, data);
        break;

    case SIOCSIFFLAGS:
        switch (ifp->if_flags & (IFF_UP | IFF_RUNNING)) {
        case IFF_RUNNING:
            mcf5272_enet_stop (sc);
            break;

        case IFF_UP:
            mcf5272_enet_init (sc);
            break;

        case IFF_UP | IFF_RUNNING:
            mcf5272_enet_stop (sc);
            mcf5272_enet_init (sc);
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


int
rtems_enet_driver_attach (struct rtems_bsdnet_ifconfig *config)
{
    struct mcf5272_enet_struct *sc;
    struct ifnet *ifp;
    int mtu;
    int unitNumber;
    char *unitName;

    /*
     * Parse driver name
     */
    unitNumber = rtems_bsdnet_parse_driver_name (config, &unitName);
    if (unitNumber < 0){
        return 0;
    }

    /*
     * Is driver free?
     */
    if ((unitNumber < 0) || (unitNumber > NIFACES)) {
        printf ("Bad unit number: %d.\n", unitNumber);
        return 0;
    }

    sc = &enet_driver[unitNumber];
    ifp = &sc->arpcom.ac_if;
    if (ifp->if_softc != NULL) {
        printf ("Driver already in use.\n");
        return 0;
    }

    /*
     * Process options
     */

    sc->arpcom.ac_enaddr[0] = (g_enet_regs->malr >> 24) & 0xff;
    sc->arpcom.ac_enaddr[1] = (g_enet_regs->malr >> 16) & 0xff;
    sc->arpcom.ac_enaddr[2] = (g_enet_regs->malr >> 8) & 0xff;
    sc->arpcom.ac_enaddr[3] = (g_enet_regs->malr >> 0) & 0xff;
    sc->arpcom.ac_enaddr[4] = (g_enet_regs->maur >> 24) & 0xff;
    sc->arpcom.ac_enaddr[5] = (g_enet_regs->maur >> 16) & 0xff;

    if (config->mtu) {
        mtu = config->mtu;
    } else {
        mtu = ETHERMTU;
    }

    if (config->rbuf_count) {
        sc->rxBdCount = config->rbuf_count;
    } else {
        sc->rxBdCount = RX_BUF_COUNT;
    }
    if (config->xbuf_count) {
        sc->txBdCount = config->xbuf_count;
    } else {
        sc->txBdCount = TX_BUF_COUNT * TX_BD_PER_BUF;
    }
    sc->acceptBroadcast = !config->ignore_broadcast;

    /*
     * Set up network interface values
     */
    ifp->if_softc = sc;
    ifp->if_unit = unitNumber;
    ifp->if_name = unitName;
    ifp->if_mtu = mtu;
    ifp->if_init = mcf5272_enet_init;
    ifp->if_ioctl = mcf5272_enet_ioctl;
    ifp->if_start = mcf5272_enet_start;
    ifp->if_output = ether_output;
    ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX;
    if (ifp->if_snd.ifq_maxlen == 0) {
        ifp->if_snd.ifq_maxlen = ifqmaxlen;
    }

    /*
     * Attach the interface
     */
    if_attach (ifp);
    cp;
    ether_ifattach (ifp);
    cp;
    return 1;
};



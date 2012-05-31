/*
 * RTEMS driver for MCF5282 Fast Ethernet Controller
 *
 *  Author: W. Eric Norum <norume@aps.anl.gov>
 *
 *  COPYRIGHT (c) 2005.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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

#define FEC_INTC0_TX_VECTOR (64+23)
#define FEC_INTC0_RX_VECTOR (64+27)
#define MII_VECTOR (64+7)  /* IRQ7* pin connected to external transceiver */
#define MII_EPPAR  MCF5282_EPORT_EPPAR_EPPA7_LEVEL
#define MII_EPDDR  MCF5282_EPORT_EPDDR_EPDD7
#define MII_EPIER  MCF5282_EPORT_EPIER_EPIE7
#define MII_EPPDR  MCF5282_EPORT_EPPDR_EPPD7

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
    #error "Driver must have MCLBYTES > RBUF_SIZE"
#endif

/*
 * Per-device data
 */
struct mcf5282_enet_struct {
    struct arpcom               arpcom;
    struct mbuf                 **rxMbuf;
    struct mbuf                 **txMbuf;
    int                         acceptBroadcast;
    int                         rxBdCount;
    int                         txBdCount;
    int                         txBdHead;
    int                         txBdTail;
    int                         txBdActiveCount;
    mcf5282BufferDescriptor_t  *rxBdBase;
    mcf5282BufferDescriptor_t  *txBdBase;
    rtems_id                    rxDaemonTid;
    rtems_id                    txDaemonTid;

    /*
     * Statistics
     */
    unsigned long   rxInterrupts;
    unsigned long   txInterrupts;
    unsigned long   miiInterrupts;
    unsigned long   txRawWait;
    unsigned long   txRealign;
    unsigned long   txRealignDrop;

    /*
     * Link parameters
     */
    enum            { link_auto, link_100Full, link_10Half } link;
    uint16_t        mii_cr;
    uint16_t        mii_sr2;
};
static struct mcf5282_enet_struct enet_driver[NIFACES];

/*
 * Read MII register
 * Busy-waits, but transfer time should be short!
 */
static int
getMII(int phyNumber, int regNumber)
{
    MCF5282_FEC_MMFR = (0x1 << 30)       |
                       (0x2 << 28)       |
                       (phyNumber << 23) |
                       (regNumber << 18) |
                       (0x2 << 16);
    while ((MCF5282_FEC_EIR & MCF5282_FEC_EIR_MII) == 0);
    MCF5282_FEC_EIR = MCF5282_FEC_EIR_MII;
    return MCF5282_FEC_MMFR & 0xFFFF;
}

/*
 * Write MII register
 * Busy-waits, but transfer time should be short!
 */
static void
setMII(int phyNumber, int regNumber, int value)
{
    MCF5282_FEC_MMFR = (0x1 << 30)       |
                       (0x1 << 28)       |
                       (phyNumber << 23) |
                       (regNumber << 18) |
                       (0x2 << 16)       |
                       (value & 0xFFFF);
    while ((MCF5282_FEC_EIR & MCF5282_FEC_EIR_MII) == 0);
    MCF5282_FEC_EIR = MCF5282_FEC_EIR_MII;
}

static rtems_isr
mcf5282_fec_rx_interrupt_handler( rtems_vector_number v )
{
    MCF5282_FEC_EIR = MCF5282_FEC_EIR_RXF;
    MCF5282_FEC_EIMR &= ~MCF5282_FEC_EIMR_RXF;
    enet_driver[0].rxInterrupts++;
    rtems_event_send(enet_driver[0].rxDaemonTid, RX_INTERRUPT_EVENT);
}

static rtems_isr
mcf5282_fec_tx_interrupt_handler( rtems_vector_number v )
{
    MCF5282_FEC_EIR = MCF5282_FEC_EIR_TXF;
    MCF5282_FEC_EIMR &= ~MCF5282_FEC_EIMR_TXF;
    enet_driver[0].txInterrupts++;
    rtems_event_send(enet_driver[0].txDaemonTid, TX_INTERRUPT_EVENT);
}

static rtems_isr
mcf5282_mii_interrupt_handler( rtems_vector_number v )
{
    uint16 sr2;

    enet_driver[0].miiInterrupts++;
    getMII(1, 19); /* Read and clear interrupt status bits */
    enet_driver[0].mii_sr2 = sr2 = getMII(1, 17);
    if (((sr2 & 0x200) != 0)
     && ((MCF5282_FEC_TCR & MCF5282_FEC_TCR_FDEN) == 0))
        MCF5282_FEC_TCR |= MCF5282_FEC_TCR_FDEN;
    else if (((sr2 & 0x200) == 0)
          && ((MCF5282_FEC_TCR & MCF5282_FEC_TCR_FDEN) != 0))
        MCF5282_FEC_TCR &= ~MCF5282_FEC_TCR_FDEN;
}

/*
 * Allocate buffer descriptors from (non-cached) on-chip static RAM
 * Ensure 128-bit (16-byte) alignment
 * Allow some space at the beginning for other diagnostic counters
 */
static mcf5282BufferDescriptor_t *
mcf5282_bd_allocate(unsigned int count)
{
    static mcf5282BufferDescriptor_t *bdp = __SRAMBASE.fec_descriptors;
    mcf5282BufferDescriptor_t *p = bdp;

    bdp += count;
    if ((int)bdp & 0xF)
        bdp = (mcf5282BufferDescriptor_t *)((char *)bdp + (16 - ((int)bdp & 0xF)));
    return p;
}

static void
mcf5282_fec_initialize_hardware(struct mcf5282_enet_struct *sc)
{
    int i;
    const unsigned char *hwaddr;
    rtems_status_code status;
    rtems_isr_entry old_handler;
	uint32_t clock_speed = bsp_get_CPU_clock_speed();

    /*
     * Issue reset to FEC
     */
    MCF5282_FEC_ECR = MCF5282_FEC_ECR_RESET;
    rtems_task_wake_after(2);
    MCF5282_FEC_ECR = 0;

    /*
     * Configuration of I/O ports is done outside of this function
     */
#if 0
    imm->gpio.pbcnt |= MCF5282_GPIO_PBCNT_SET_FEC;        /* Set up port b FEC pins */
#endif

    /*
     * Set our physical address
     */
    hwaddr = sc->arpcom.ac_enaddr;
    MCF5282_FEC_PALR = (hwaddr[0] << 24) | (hwaddr[1] << 16) |
                       (hwaddr[2] << 8)  | (hwaddr[3] << 0);
    MCF5282_FEC_PAUR = (hwaddr[4] << 24) | (hwaddr[5] << 16);


    /*
     * Clear the hash table
     */
    MCF5282_FEC_GAUR = 0;
    MCF5282_FEC_GALR = 0;

    /*
     * Set up receive buffer size
     */
    MCF5282_FEC_EMRBR = 1520; /* Standard Ethernet */

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
    sc->rxBdBase = mcf5282_bd_allocate(sc->rxBdCount);
    sc->txBdBase = mcf5282_bd_allocate(sc->txBdCount);
    MCF5282_FEC_ERDSR = (int)sc->rxBdBase;
    MCF5282_FEC_ETDSR = (int)sc->txBdBase;

    /*
     * Set up Receive Control Register:
     *   Not promiscuous
     *   MII mode
     *   Full duplex
     *   No loopback
     */
    MCF5282_FEC_RCR = MCF5282_FEC_RCR_MAX_FL(MAX_MTU_SIZE) |
                      MCF5282_FEC_RCR_MII_MODE;

    /*
     * Set up Transmit Control Register:
     *   Full or half duplex
     *   No heartbeat
     */
    if (sc->link == link_10Half)
        MCF5282_FEC_TCR = 0;
    else
    MCF5282_FEC_TCR = MCF5282_FEC_TCR_FDEN;

    /*
     * Initialize statistic counters
     */
    MCF5282_FEC_MIBC = MCF5282_FEC_MIBC_MIB_DISABLE;
    {
    vuint32 *vuip = &MCF5282_FEC_RMON_T_DROP;
    while (vuip <= &MCF5282_FEC_IEEE_R_OCTETS_OK)
        *vuip++ = 0;
    }
    MCF5282_FEC_MIBC = 0;

    /*
     * Set MII speed to <= 2.5 MHz
     */
    i = (clock_speed + 5000000 - 1) / 5000000;
    MCF5282_FEC_MSCR = MCF5282_FEC_MSCR_MII_SPEED(i);

    /*
     * Set PHYS
     *  LED1 receive status, LED2 link status, LEDs stretched
     *  Advertise 100 Mb/s, full-duplex, IEEE-802.3
     *  Turn off auto-negotiate
     *  Clear status
     */
    setMII(1, 20, 0x24F2);
    setMII(1,  4, 0x0181);
    setMII(1,  0, 0x0);
    rtems_task_wake_after(2);
    sc->mii_sr2 = getMII(1, 17);
    switch (sc->link) {
    case link_auto:
        /*
         * Enable speed-change, duplex-change and link-status-change interrupts
         * Enable auto-negotiate (start at 100/FULL)
         */
    setMII(1, 18, 0x0072);
        setMII(1, 0, 0x3100);
        break;

    case link_10Half:
        /*
         * Force 10/HALF
         */
        setMII(1, 0, 0x0);
        break;

    case link_100Full:
        /*
         * Force 100/FULL
         */
        setMII(1, 0, 0x2100);
        break;
    }
    sc->mii_cr = getMII(1, 0);

    /*
     * Set up receive buffer descriptors
     */
    for (i = 0 ; i < sc->rxBdCount ; i++)
        (sc->rxBdBase + i)->status = 0;

    /*
     * Set up transmit buffer descriptors
     */
    for (i = 0 ; i < sc->txBdCount ; i++) {
        sc->txBdBase[i].status = 0;
        sc->txMbuf[i] = NULL;
    }
    sc->txBdHead = sc->txBdTail = 0;
    sc->txBdActiveCount = 0;

    /*
     * Set up interrupts
     */
    status = rtems_interrupt_catch( mcf5282_fec_tx_interrupt_handler, FEC_INTC0_TX_VECTOR, &old_handler );
    if (status != RTEMS_SUCCESSFUL)
        rtems_panic ("Can't attach MCF5282 FEC TX interrupt handler: %s\n",
                                                 rtems_status_text(status));
    bsp_allocate_interrupt(FEC_IRQ_LEVEL, FEC_IRQ_TX_PRIORITY);
    MCF5282_INTC0_ICR23 = MCF5282_INTC_ICR_IL(FEC_IRQ_LEVEL) |
                          MCF5282_INTC_ICR_IP(FEC_IRQ_TX_PRIORITY);
    MCF5282_INTC0_IMRL &= ~(MCF5282_INTC_IMRL_INT23 | MCF5282_INTC_IMRL_MASKALL);

    status = rtems_interrupt_catch(mcf5282_fec_rx_interrupt_handler, FEC_INTC0_RX_VECTOR, &old_handler);
    if (status != RTEMS_SUCCESSFUL)
        rtems_panic ("Can't attach MCF5282 FEC RX interrupt handler: %s\n",
                                                 rtems_status_text(status));
    bsp_allocate_interrupt(FEC_IRQ_LEVEL, FEC_IRQ_RX_PRIORITY);
    MCF5282_INTC0_ICR27 = MCF5282_INTC_ICR_IL(FEC_IRQ_LEVEL) |
                          MCF5282_INTC_ICR_IP(FEC_IRQ_RX_PRIORITY);
    MCF5282_INTC0_IMRL &= ~(MCF5282_INTC_IMRL_INT27 | MCF5282_INTC_IMRL_MASKALL);

    status = rtems_interrupt_catch(mcf5282_mii_interrupt_handler, MII_VECTOR, &old_handler);
    if (status != RTEMS_SUCCESSFUL)
        rtems_panic ("Can't attach MCF5282 FEC MII interrupt handler: %s\n",
                                                 rtems_status_text(status));
    MCF5282_EPORT_EPPAR &= ~MII_EPPAR;
    MCF5282_EPORT_EPDDR &= ~MII_EPDDR;
    MCF5282_EPORT_EPIER |=  MII_EPIER;
    MCF5282_INTC0_IMRL &= ~(MCF5282_INTC_IMRL_INT7 | MCF5282_INTC_IMRL_MASKALL);
}

/*
 * Soak up buffer descriptors that have been sent.
 */
static void
fec_retire_tx_bd(volatile struct mcf5282_enet_struct *sc )
{
    struct mbuf *m, *n;
    uint16_t status;

    while ((sc->txBdActiveCount != 0)
        && (((status = sc->txBdBase[sc->txBdTail].status) & MCF5282_FEC_TxBD_R) == 0)) {
        if ((status & MCF5282_FEC_TxBD_TO1) == 0) {
            m = sc->txMbuf[sc->txBdTail];
            MFREE(m, n);
        }
        if (++sc->txBdTail == sc->txBdCount)
            sc->txBdTail = 0;
        sc->txBdActiveCount--;
    }
}

static void
fec_rxDaemon (void *arg)
{
    volatile struct mcf5282_enet_struct *sc = (volatile struct mcf5282_enet_struct *)arg;
    struct ifnet *ifp = (struct ifnet* )&sc->arpcom.ac_if;
    struct mbuf *m;
    uint16_t status;
    volatile mcf5282BufferDescriptor_t *rxBd;
    int rxBdIndex;

    /*
     * Allocate space for incoming packets and start reception
     */
    for (rxBdIndex = 0 ; ;) {
        rxBd = sc->rxBdBase + rxBdIndex;
        MGETHDR(m, M_WAIT, MT_DATA);
        MCLGET(m, M_WAIT);
        m->m_pkthdr.rcvif = ifp;
        sc->rxMbuf[rxBdIndex] = m;
        rxBd->buffer = mtod(m, void *);
        rxBd->status = MCF5282_FEC_RxBD_E;
        if (++rxBdIndex == sc->rxBdCount) {
            rxBd->status |= MCF5282_FEC_RxBD_W;
            break;
        }
    }

    /*
     * Input packet handling loop
     */
    MCF5282_FEC_RDAR = 0;

    rxBdIndex = 0;
    for (;;) {
        rxBd = sc->rxBdBase + rxBdIndex;

        /*
         * Wait for packet if there's not one ready
         */
        if ((status = rxBd->status) & MCF5282_FEC_RxBD_E) {
            /*
             * Clear old events.
             */
            MCF5282_FEC_EIR = MCF5282_FEC_EIR_RXF;

            /*
             * Wait for packet to arrive.
             * Check the buffer descriptor before waiting for the event.
             * This catches the case when a packet arrives between the
             * `if' above, and the clearing of the RXF bit in the EIR.
             */
            while ((status = rxBd->status) & MCF5282_FEC_RxBD_E) {
                rtems_event_set events;
                int level;

                rtems_interrupt_disable(level);
                MCF5282_FEC_EIMR |= MCF5282_FEC_EIMR_RXF;
                rtems_interrupt_enable(level);
                rtems_bsdnet_event_receive (RX_INTERRUPT_EVENT,
                                            RTEMS_WAIT|RTEMS_EVENT_ANY,
                                            RTEMS_NO_TIMEOUT,
                                            &events);
            }
        }

        /*
         * Check that packet is valid
         */
        if (status & MCF5282_FEC_RxBD_L) {
            /*
             * Pass the packet up the chain.
             * FIXME: Packet filtering hook could be done here.
             */
            struct ether_header *eh;
            int len = rxBd->length - sizeof(uint32_t);

            m = sc->rxMbuf[rxBdIndex];
#ifdef RTEMS_MCF5282_BSP_ENABLE_DATA_CACHE
            /*
             * Invalidate the cache.  The cache is so small that it's
             * reasonable to simply invalidate the whole thing.
             */
            rtems_cache_invalidate_entire_data();
#endif
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
        rxBd->status = (status & MCF5282_FEC_RxBD_W) | MCF5282_FEC_RxBD_E;
        MCF5282_FEC_RDAR = 0;

        /*
         * Move to next buffer descriptor
         */
        if (++rxBdIndex == sc->rxBdCount)
            rxBdIndex = 0;
    }
}

static void
fec_sendpacket(struct ifnet *ifp, struct mbuf *m)
{
    struct mcf5282_enet_struct *sc = ifp->if_softc;
    volatile mcf5282BufferDescriptor_t *firstTxBd, *txBd;
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

    while (m != NULL) {
        /*
         * Wait for buffer descriptor to become available
         */
        if ((sc->txBdActiveCount + nAdded)  == sc->txBdCount) {
            /*
             * Clear old events.
             */
            MCF5282_FEC_EIR = MCF5282_FEC_EIR_TXF;

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
                MCF5282_FEC_EIMR |= MCF5282_FEC_EIMR_TXF;
                rtems_interrupt_enable(level);
                sc->txRawWait++;
                rtems_bsdnet_event_receive(TX_INTERRUPT_EVENT,
                                           RTEMS_WAIT|RTEMS_EVENT_ANY,
                                           RTEMS_NO_TIMEOUT,
                                           &events);
                fec_retire_tx_bd(sc);
            }
        }

        /*
         * Don't set the READY flag on the first fragment
         * until the whole packet has been readied.
         */
        status = nAdded ? MCF5282_FEC_TxBD_R : 0;

        /*
         * The IP fragmentation routine in ip_output
         * can produce fragments with zero length.
         */
        txBd = sc->txBdBase + sc->txBdHead;
        if (m->m_len) {
            char *p = mtod(m, char *);
            int offset = (int)p & 0x3;
            if (offset == 0) {
                txBd->buffer = p;
                txBd->length = m->m_len;
                sc->txMbuf[sc->txBdHead] = m;
                m = m->m_next;
            }
            else {
                /*
                 * Stupid FEC can't handle misaligned data!
                 * Move offending bytes to a local buffer.
                 * Use buffer descriptor TO1 bit to indicate this.
                 */
                int nmove = 4 - offset;
                char *d = (char *)&sc->txMbuf[sc->txBdHead];
                status |= MCF5282_FEC_TxBD_TO1;
                sc->txRealign++;
                if (nmove > m->m_len)
                    nmove = m->m_len;
                m->m_data += nmove;
                m->m_len -= nmove;
                txBd->buffer = d;
                txBd->length = nmove;
                while (nmove--)
                    *d++ = *p++;
                if (m->m_len == 0) {
                    struct mbuf *n;
                    sc->txRealignDrop++;
                    MFREE(m, n);
                    m = n;
                }
            }
            nAdded++;
            if (++sc->txBdHead == sc->txBdCount) {
                status |= MCF5282_FEC_TxBD_W;
                sc->txBdHead = 0;
            }
            txBd->status = status;
        }
        else {
            /*
             * Toss empty mbufs.
             */
            struct mbuf *n;
            MFREE(m, n);
            m = n;
        }
    }
    if (nAdded) {
        txBd->status = status | MCF5282_FEC_TxBD_R
                              | MCF5282_FEC_TxBD_L
                              | MCF5282_FEC_TxBD_TC;
        if (nAdded > 1)
            firstTxBd->status |= MCF5282_FEC_TxBD_R;
        MCF5282_FEC_TDAR = 0;
        sc->txBdActiveCount += nAdded;
      }
}

void
fec_txDaemon(void *arg)
{
    struct mcf5282_enet_struct *sc = (struct mcf5282_enet_struct *)arg;
    struct ifnet *ifp = &sc->arpcom.ac_if;
    struct mbuf *m;
    rtems_event_set events;

    for (;;) {
        /*
         * Wait for packet
         */
        rtems_bsdnet_event_receive(START_TRANSMIT_EVENT,
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
            fec_sendpacket(ifp, m);
        }
        ifp->if_flags &= ~IFF_OACTIVE;
    }
}


/*
 * Send packet (caller provides header).
 */
static void
mcf5282_enet_start(struct ifnet *ifp)
{
    struct mcf5282_enet_struct *sc = ifp->if_softc;

    rtems_event_send(sc->txDaemonTid, START_TRANSMIT_EVENT);
    ifp->if_flags |= IFF_OACTIVE;
}

static void
fec_init(void *arg)
{
    struct mcf5282_enet_struct *sc = arg;
    struct ifnet *ifp = &sc->arpcom.ac_if;

    if (sc->txDaemonTid == 0) {
        /*
         * Set up hardware
         */
        mcf5282_fec_initialize_hardware(sc);

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
        MCF5282_FEC_RCR |= MCF5282_FEC_RCR_PROM;
    else
        MCF5282_FEC_RCR &= ~MCF5282_FEC_RCR_PROM;

    /*
     * Tell the world that we're running.
     */
    ifp->if_flags |= IFF_RUNNING;

    /*
     * Enable receiver and transmitter
     */
    MCF5282_FEC_ECR = MCF5282_FEC_ECR_ETHER_EN;
}


static void
fec_stop(struct mcf5282_enet_struct *sc)
{
    struct ifnet *ifp = &sc->arpcom.ac_if;

    ifp->if_flags &= ~IFF_RUNNING;

    /*
     * Shut down receiver and transmitter
     */
    MCF5282_FEC_ECR = 0x0;
}

/*
 * Show interface statistics
 */
static void
enet_stats(struct mcf5282_enet_struct *sc)
{
  printf("  Rx Interrupts:%-10lu",   sc->rxInterrupts);
  printf("Rx Packet Count:%-10lu",   (uint32_t) MCF5282_FEC_RMON_R_PACKETS);
  printf("   Rx Broadcast:%-10lu\n", (uint32_t) MCF5282_FEC_RMON_R_BC_PKT);
  printf("   Rx Multicast:%-10lu",   (uint32_t) MCF5282_FEC_RMON_R_MC_PKT);
  printf("CRC/Align error:%-10lu",   (uint32_t) MCF5282_FEC_RMON_R_CRC_ALIGN);
  printf("   Rx Undersize:%-10lu\n", (uint32_t) MCF5282_FEC_RMON_R_UNDERSIZE);
  printf("    Rx Oversize:%-10lu",   (uint32_t) MCF5282_FEC_RMON_R_OVERSIZE);
  printf("    Rx Fragment:%-10lu",   (uint32_t) MCF5282_FEC_RMON_R_FRAG);
  printf("      Rx Jabber:%-10lu\n", (uint32_t) MCF5282_FEC_RMON_R_JAB);
  printf("          Rx 64:%-10lu",   (uint32_t) MCF5282_FEC_RMON_R_P64);
  printf("      Rx 65-127:%-10lu",   (uint32_t) MCF5282_FEC_RMON_R_P65T0127);
  printf("     Rx 128-255:%-10lu\n", (uint32_t) MCF5282_FEC_RMON_R_P128TO255);
  printf("     Rx 256-511:%-10lu",   (uint32_t) MCF5282_FEC_RMON_R_P256TO511);
  printf("    Rx 511-1023:%-10lu",   (uint32_t) MCF5282_FEC_RMON_R_P512TO1023);
  printf("   Rx 1024-2047:%-10lu\n", (uint32_t) MCF5282_FEC_RMON_R_P1024TO2047);
  printf("      Rx >=2048:%-10lu",   (uint32_t) MCF5282_FEC_RMON_R_GTE2048);
  printf("      Rx Octets:%-10lu",   (uint32_t) MCF5282_FEC_RMON_R_OCTETS);
  printf("     Rx Dropped:%-10lu\n", (uint32_t) MCF5282_FEC_IEEE_R_DROP);
  printf("    Rx frame OK:%-10lu",   (uint32_t) MCF5282_FEC_IEEE_R_FRAME_OK);
  printf("   Rx CRC error:%-10lu",   (uint32_t) MCF5282_FEC_IEEE_R_CRC);
  printf(" Rx Align error:%-10lu\n", (uint32_t) MCF5282_FEC_IEEE_R_ALIGN);
  printf("  FIFO Overflow:%-10lu",   (uint32_t) MCF5282_FEC_IEEE_R_MACERR);
  printf("Rx Pause Frames:%-10lu",   (uint32_t) MCF5282_FEC_IEEE_R_FDXFC);
  printf("   Rx Octets OK:%-10lu\n", (uint32_t) MCF5282_FEC_IEEE_R_OCTETS_OK);
  printf("  Tx Interrupts:%-10lu",   sc->txInterrupts);
  printf("Tx Output Waits:%-10lu",   sc->txRawWait);
  printf("Tx mbuf realign:%-10lu\n", sc->txRealign);
  printf("Tx realign drop:%-10lu",   sc->txRealignDrop);
  printf(" Tx Unaccounted:%-10lu",   (uint32_t) MCF5282_FEC_RMON_T_DROP);
  printf("Tx Packet Count:%-10lu\n", (uint32_t) MCF5282_FEC_RMON_T_PACKETS);
  printf("   Tx Broadcast:%-10lu",   (uint32_t) MCF5282_FEC_RMON_T_BC_PKT);
  printf("   Tx Multicast:%-10lu",   (uint32_t) MCF5282_FEC_RMON_T_MC_PKT);
  printf("CRC/Align error:%-10lu\n", (uint32_t) MCF5282_FEC_RMON_T_CRC_ALIGN);
  printf("   Tx Undersize:%-10lu",   (uint32_t) MCF5282_FEC_RMON_T_UNDERSIZE);
  printf("    Tx Oversize:%-10lu",   (uint32_t) MCF5282_FEC_RMON_T_OVERSIZE);
  printf("    Tx Fragment:%-10lu\n", (uint32_t) MCF5282_FEC_RMON_T_FRAG);
  printf("      Tx Jabber:%-10lu",   (uint32_t) MCF5282_FEC_RMON_T_JAB);
  printf("  Tx Collisions:%-10lu",   (uint32_t) MCF5282_FEC_RMON_T_COL);
  printf("          Tx 64:%-10lu\n", (uint32_t) MCF5282_FEC_RMON_T_P64);
  printf("      Tx 65-127:%-10lu",   (uint32_t) MCF5282_FEC_RMON_T_P65TO127);
  printf("     Tx 128-255:%-10lu",   (uint32_t) MCF5282_FEC_RMON_T_P128TO255);
  printf("     Tx 256-511:%-10lu\n", (uint32_t) MCF5282_FEC_RMON_T_P256TO511);
  printf("    Tx 511-1023:%-10lu",   (uint32_t) MCF5282_FEC_RMON_T_P512TO1023);
  printf("   Tx 1024-2047:%-10lu",   (uint32_t) MCF5282_FEC_RMON_T_P1024TO2047);
  printf("      Tx >=2048:%-10lu\n", (uint32_t) MCF5282_FEC_RMON_T_P_GTE2048);
  printf("      Tx Octets:%-10lu",   (uint32_t) MCF5282_FEC_RMON_T_OCTETS);
  printf("     Tx Dropped:%-10lu",   (uint32_t) MCF5282_FEC_IEEE_T_DROP);
  printf("    Tx Frame OK:%-10lu\n", (uint32_t) MCF5282_FEC_IEEE_T_FRAME_OK);
  printf(" Tx 1 Collision:%-10lu",   (uint32_t) MCF5282_FEC_IEEE_T_1COL);
  printf("Tx >1 Collision:%-10lu",   (uint32_t) MCF5282_FEC_IEEE_T_MCOL);
  printf("    Tx Deferred:%-10lu\n", (uint32_t) MCF5282_FEC_IEEE_T_DEF);
  printf(" Late Collision:%-10lu",   (uint32_t) MCF5282_FEC_IEEE_T_LCOL);
  printf(" Excessive Coll:%-10lu",   (uint32_t) MCF5282_FEC_IEEE_T_EXCOL);
  printf("  FIFO Underrun:%-10lu\n", (uint32_t) MCF5282_FEC_IEEE_T_MACERR);
  printf("  Carrier Error:%-10lu",   (uint32_t) MCF5282_FEC_IEEE_T_CSERR);
  printf("   Tx SQE Error:%-10lu",   (uint32_t) MCF5282_FEC_IEEE_T_SQE);
  printf("Tx Pause Frames:%-10lu\n", (uint32_t) MCF5282_FEC_IEEE_T_FDXFC);
  printf("   Tx Octets OK:%-10lu",   (uint32_t) MCF5282_FEC_IEEE_T_OCTETS_OK);
  printf(" MII interrupts:%-10lu\n", sc->miiInterrupts);
  if ((sc->mii_sr2 & 0x400) == 0) {
      printf("LINK DOWN!\n");
  }
  else {
      int speed;
      int full;
      int fixed;
      if (sc->mii_cr & 0x1000) {
	  fixed = 0;
	  speed = sc->mii_sr2 & 0x4000 ? 100 : 10;
	  full = sc->mii_sr2 & 0x200 ? 1 : 0;
      }
      else {
	  fixed = 1;
	  speed = sc->mii_cr & 0x2000 ? 100 : 10;
	  full = sc->mii_cr & 0x100 ? 1 : 0;
      }
      printf("Link %s %d Mb/s, %s-duplex.\n",
					  fixed ? "fixed" : "auto-negotiate",
					  speed,
					  full ? "full" : "half");
  }
  printf(" EIR:%8.8lx  ",  (uint32_t) MCF5282_FEC_EIR);
  printf("EIMR:%8.8lx  ",  (uint32_t) MCF5282_FEC_EIMR);
  printf("RDAR:%8.8lx  ",  (uint32_t) MCF5282_FEC_RDAR);
  printf("TDAR:%8.8lx\n",  (uint32_t) MCF5282_FEC_TDAR);
  printf(" ECR:%8.8lx  ",  (uint32_t) MCF5282_FEC_ECR);
  printf(" RCR:%8.8lx  ",  (uint32_t) MCF5282_FEC_RCR);
  printf(" TCR:%8.8lx\n",  (uint32_t) MCF5282_FEC_TCR);
  printf("FRBR:%8.8lx  ",  (uint32_t) MCF5282_FEC_FRBR);
  printf("FRSR:%8.8lx\n",  (uint32_t) MCF5282_FEC_FRSR);
  if (sc->txBdActiveCount != 0) {
      int i, n;
      /*
       * Yes, there are races here with adding and retiring descriptors,
       * but this diagnostic is more for when things have backed up.
       */
      printf("Transmit Buffer Descriptors (Tail %d, Head %d, Unretired %d):\n",
						  sc->txBdTail,
						  sc->txBdHead,
						  sc->txBdActiveCount);
      i = sc->txBdTail;
      for (n = 0 ; n < sc->txBdCount ; n++) {
	  if ((sc->txBdBase[i].status & MCF5282_FEC_TxBD_R) != 0)
	      printf("  %3d: status:%4.4x  length:%-4d  buffer:%p\n",
						  i,
						  sc->txBdBase[i].status,
						  sc->txBdBase[i].length,
						  sc->txBdBase[i].buffer);
	  if (++i == sc->txBdCount)
	      i = 0;
      }
  }
}

static int
fec_ioctl(struct ifnet *ifp, ioctl_command_t command, caddr_t data)
{
    struct mcf5282_enet_struct *sc = ifp->if_softc;
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
rtems_fec_driver_attach(struct rtems_bsdnet_ifconfig *config, int attaching )
{
    struct mcf5282_enet_struct *sc;
    struct ifnet *ifp;
    int mtu;
    int unitNumber;
    char *unitName;
    const unsigned char *hwaddr;
    const char *env;

    /*
     * Parse driver name
     */
    if ((unitNumber = rtems_bsdnet_parse_driver_name (config, &unitName)) < 0)
        return 0;

    /*
     * Is driver free?
     */
    if ((unitNumber <= 0) || (unitNumber > NIFACES)) {
        printf("Bad FEC unit number.\n");
        return 0;
    }
    sc = &enet_driver[unitNumber - 1];
    ifp = &sc->arpcom.ac_if;
    if (ifp->if_softc != NULL) {
        printf("Driver already in use.\n");
        return 0;
    }

    /*
     * Process options
     */
    if (config->hardware_address) {
        hwaddr = config->hardware_address;
    } else if ((hwaddr = bsp_gethwaddr(unitNumber - 1)) == NULL) {
        /* Locally-administered address */
        static const unsigned char defaultAddress[ETHER_ADDR_LEN] = {
                                        0x06, 'R', 'T', 'E', 'M', 'S'};
        printf ("WARNING -- No %s%d Ethernet address specified "
                "-- Using default address.\n", unitName, unitNumber);
        hwaddr = defaultAddress;
    }
    printf("%s%d: Ethernet address: %02x:%02x:%02x:%02x:%02x:%02x\n",
                                            unitName, unitNumber,
                                            hwaddr[0], hwaddr[1], hwaddr[2],
                                            hwaddr[3], hwaddr[4], hwaddr[5]);
    memcpy(sc->arpcom.ac_enaddr, hwaddr, ETHER_ADDR_LEN);

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
    ifp->if_start = mcf5282_enet_start;
    ifp->if_output = ether_output;
    ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX;
    if (ifp->if_snd.ifq_maxlen == 0)
        ifp->if_snd.ifq_maxlen = ifqmaxlen;

    /*
     * Check for environment overrides
     */
    if (((env = bsp_getbenv("IPADDR0_100FULL")) != NULL)
     && ((*env == 'y') || (*env == 'Y')))
        sc->link = link_100Full;
    else if (((env = bsp_getbenv("IPADDR0_10HALF")) != NULL)
     && ((*env == 'y') || (*env == 'Y')))
        sc->link = link_10Half;
    else
        sc->link = link_auto;

    /*
     * Attach the interface
     */
    if_attach(ifp);
    ether_ifattach(ifp);
    return 1;
};

/**
 *  @file
 *  
 *  Au1x00 ethernet driver
 */

/*
 *  Copyright (c) 2005 by Cogent Computer Systems
 *  Written by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <bsp.h>
#include <rtems/bspIo.h>
#include <libcpu/au1x00.h>
#include <bsp/irq.h>

#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <rtems/error.h>

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>

#include <net/if.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>

#include <assert.h>

#define NUM_IFACES 1
#define NUM_TX_DMA_BUFS 4
#define NUM_RX_DMA_BUFS 4

/*  RTEMS event used to start tx daemon. */
#define START_TX_EVENT  RTEMS_EVENT_1
/*  RTEMS event used to start rx daemon. */
#define START_RX_EVENT  RTEMS_EVENT_2

rtems_isr au1x00_emac_isr(rtems_vector_number vector);

#define TX_BUF_SIZE 2048

char tx_buf_base[(4 * TX_BUF_SIZE) + 32];

volatile int wait_count;
/*
 * Hardware-specific storage
 */
typedef struct
{
    /*
     * Connection to networking code
     * This entry *must* be the first in the sonic_softc structure.
     */
    struct arpcom                    arpcom;

    /*
     * Interrupt vector
     */
    rtems_vector_number             vector;

    /*
     *  Indicates configuration
     */
    int                             acceptBroadcast;

    /*
     * Tasks waiting for interrupts
     */
    rtems_id                        rx_daemon_tid;
    rtems_id                        tx_daemon_tid;

    /*
     * Buffers
     */
    au1x00_macdma_rx_t             *rx_dma;
    au1x00_macdma_tx_t             *tx_dma;
    int                             rx_head;
    int                             rx_tail;
    int                             tx_head;
    int                             tx_tail;
    struct mbuf                    *rx_mbuf[NUM_RX_DMA_BUFS];

    unsigned char                  *tx_buf[4];

    /*
     * register addresses
     */
    uint32_t                      ctrl_regs;
    uint32_t                     *en_reg;
    uint32_t                      int_mask;
    uint32_t                      int_ctrlr;

    /*
     * device
     */
    int                             unitnumber;

    /*
     * Statistics
     */
    unsigned long                   interrupts;
    unsigned long                   rx_interrupts;
    unsigned long                   tx_interrupts;
    unsigned long                   rx_missed;
    unsigned long                   rx_bcast;
    unsigned long                   rx_mcast;
    unsigned long                   rx_unsupp;
    unsigned long                   rx_ctrl;
    unsigned long                   rx_len_err;
    unsigned long                   rx_crc_err;
    unsigned long                   rx_dribble;
    unsigned long                   rx_mii_err;
    unsigned long                   rx_collision;
    unsigned long                   rx_too_long;
    unsigned long                   rx_runt;
    unsigned long                   rx_watchdog;
    unsigned long                   rx_pkts;
    unsigned long                   rx_dropped;

    unsigned long                   tx_deferred;
    unsigned long                   tx_underrun;
    unsigned long                   tx_aborted;
    unsigned long                   tx_pkts;
} au1x00_emac_softc_t;

static au1x00_emac_softc_t softc[NUM_IFACES];


/* function prototypes */
int rtems_au1x00_emac_attach (struct rtems_bsdnet_ifconfig *config,
                              int attaching);
void au1x00_emac_init(void *arg);
void au1x00_emac_init_hw(au1x00_emac_softc_t *sc);
void au1x00_emac_start(struct ifnet *ifp);
void au1x00_emac_stop (au1x00_emac_softc_t *sc);
void au1x00_emac_tx_daemon (void *arg);
void au1x00_emac_rx_daemon (void *arg);
void au1x00_emac_sendpacket (struct ifnet *ifp, struct mbuf *m);
void au1x00_emac_stats (au1x00_emac_softc_t *sc);
static int au1x00_emac_ioctl (struct ifnet *ifp, ioctl_command_t command, caddr_t data);
static void mii_write(au1x00_emac_softc_t *sc, uint8_t reg, uint16_t val);
static void mii_read(au1x00_emac_softc_t *sc, uint8_t reg, uint16_t *val);
static void mii_init(au1x00_emac_softc_t *sc);

static void mii_write(au1x00_emac_softc_t *sc, uint8_t reg, uint16_t val)
{
    /* wait for the interface to get unbusy */
    while (AU1X00_MAC_MIICTRL(sc->ctrl_regs) & AU1X00_MAC_MIICTRL_MB) {
        continue;
    }

    /* write to address 0 - we only support address 0 */
    AU1X00_MAC_MIIDATA(sc->ctrl_regs) = val;
    AU1X00_MAC_MIICTRL(sc->ctrl_regs) = (((reg & 0x1f) << 6) |
                                         AU1X00_MAC_MIICTRL_MW);
    au_sync();

    /* wait for it to complete */
    while (AU1X00_MAC_MIICTRL(sc->ctrl_regs) & AU1X00_MAC_MIICTRL_MB) {
        continue;
    }
}

static void mii_read(au1x00_emac_softc_t *sc, uint8_t reg, uint16_t *val)
{
    /* wait for the interface to get unbusy */
    while (AU1X00_MAC_MIICTRL(sc->ctrl_regs) & AU1X00_MAC_MIICTRL_MB) {
        continue;
    }

    /* write to address 0 - we only support address 0 */
    AU1X00_MAC_MIICTRL(sc->ctrl_regs) = ((reg & 0x1f) << 6);
    au_sync();

    /* wait for it to complete */
    while (AU1X00_MAC_MIICTRL(sc->ctrl_regs) & AU1X00_MAC_MIICTRL_MB) {
        continue;
    }
    *val = AU1X00_MAC_MIIDATA(sc->ctrl_regs);
}

static void mii_init(au1x00_emac_softc_t *sc)
{
    uint16_t data;

    mii_write(sc, 0, 0x8000);     /* reset */
    do {
        mii_read(sc, 0, &data);
    } while (data & 0x8000);

    mii_write(sc, 0, 0x3200);     /* reset autonegotiation */
    mii_write(sc, 17, 0xffc0);    /* setup LEDs */

}



int rtems_au1x00_emac_attach (
    struct rtems_bsdnet_ifconfig *config,
    int attaching
    )
{
    struct ifnet *ifp;
    int mtu;
    int unitnumber;
    char *unitname;
    static au1x00_emac_softc_t *sc;

    /*
     * Parse driver name
     */
    if ((unitnumber = rtems_bsdnet_parse_driver_name (config, &unitname)) < 0)
        return 0;

    /*
     * Is driver free?
     */
    if (unitnumber > NUM_IFACES) {
        printf ("Bad AU1X00 EMAC unit number.\n");
        return 0;
    }

    sc = &softc[unitnumber];

    ifp = &sc->arpcom.ac_if;
    if (ifp->if_softc != NULL) {
        printf ("Driver already in use.\n");
        return 0;
    }

    /*
     *  zero out the control structure
     */

    memset((void *)sc, 0, sizeof(*sc));

    sc->unitnumber = unitnumber;
    sc->int_ctrlr = AU1X00_IC0_ADDR;

    if (unitnumber == 0) {
        sc->ctrl_regs = AU1100_MAC0_ADDR;
        sc->en_reg = (void *)(AU1100_MACEN_ADDR + 0);

        sc->tx_dma = (void *)(AU1X00_MACDMA0_ADDR + 0x000);
        sc->rx_dma = (void *)(AU1X00_MACDMA0_ADDR + 0x100);
        sc->int_mask = AU1X00_IC_IRQ_MAC0;
    } else {
        printk("Unknown network device: %d\n", unitnumber);
        return 0;
    }

    /* If the ethernet controller is already set up, read the MAC address */
    if ((*sc->en_reg & 0x33) == 0x33) {
        sc->arpcom.ac_enaddr[5] = ((AU1X00_MAC_ADDRHIGH(sc->ctrl_regs) >> 8) &
                                   0xff);
        sc->arpcom.ac_enaddr[4] = ((AU1X00_MAC_ADDRHIGH(sc->ctrl_regs) >> 0) &
                                   0xff);
        sc->arpcom.ac_enaddr[3] = ((AU1X00_MAC_ADDRLOW(sc->ctrl_regs) >> 24) &
                                   0xff);
        sc->arpcom.ac_enaddr[2] = ((AU1X00_MAC_ADDRLOW(sc->ctrl_regs) >> 16) &
                                   0xff);
        sc->arpcom.ac_enaddr[1] = ((AU1X00_MAC_ADDRLOW(sc->ctrl_regs) >> 8) &
                                   0xff);
        sc->arpcom.ac_enaddr[0] = ((AU1X00_MAC_ADDRLOW(sc->ctrl_regs) >> 0) &
                                   0xff);
    } else {
        /* It's not set up yet, so we set a MAC address */
        sc->arpcom.ac_enaddr[5] = 0x05;
        sc->arpcom.ac_enaddr[4] = 0xc0;
        sc->arpcom.ac_enaddr[3] = 0x50;
        sc->arpcom.ac_enaddr[2] = 0x31;
        sc->arpcom.ac_enaddr[1] = 0x23;
        sc->arpcom.ac_enaddr[0] = 0x00;
    }


    if (config->mtu) {
        mtu = config->mtu;
    } else {
        mtu = ETHERMTU;
    }

    sc->acceptBroadcast = !config->ignore_broadcast;

    /*
     * Set up network interface values
     */
    ifp->if_softc = sc;
    ifp->if_unit = unitnumber;
    ifp->if_name = unitname;
    ifp->if_mtu = mtu;
    ifp->if_init = au1x00_emac_init;
    ifp->if_ioctl = au1x00_emac_ioctl;
    ifp->if_start = au1x00_emac_start;
    ifp->if_output = ether_output;
    ifp->if_flags = IFF_BROADCAST;
    if (ifp->if_snd.ifq_maxlen == 0) {
        ifp->if_snd.ifq_maxlen = ifqmaxlen;
    }

    /*
     * Attach the interface
     */
    if_attach (ifp);
    ether_ifattach (ifp);
    return 1;
}

void au1x00_emac_init(void *arg)
{
    au1x00_emac_softc_t     *sc = arg;
    struct ifnet *ifp = &sc->arpcom.ac_if;

    /*
     *This is for stuff that only gets done once (au1x00_emac_init()
     * gets called multiple times
     */
    if (sc->tx_daemon_tid == 0)
    {
        /* Set up EMAC hardware */
        au1x00_emac_init_hw(sc);


        /* install the interrupt handler */
        if (sc->unitnumber == 0) {
            rtems_interrupt_handler_install(
              AU1X00_IRQ_MAC0,
              "NIC0",
              0,
              (rtems_interrupt_handler)au1x00_emac_isr,
              NULL
            );
        } else {
            rtems_interrupt_handler_install(
              AU1X00_IRQ_MAC1,
              "NIC1",
              0,
              (rtems_interrupt_handler)au1x00_emac_isr,
              NULL
            );
        }
        AU1X00_IC_MASKCLR(sc->int_ctrlr) = sc->int_mask;
        au_sync();

        /* set src bit */
        AU1X00_IC_SRCSET(sc->int_ctrlr) = sc->int_mask;

        /* high level */
        AU1X00_IC_CFG0SET(sc->int_ctrlr) = sc->int_mask;
        AU1X00_IC_CFG1CLR(sc->int_ctrlr) = sc->int_mask;
        AU1X00_IC_CFG2SET(sc->int_ctrlr) = sc->int_mask;

        /* assign to request 0 - negative logic */
        AU1X00_IC_ASSIGNSET(sc->int_ctrlr) = sc->int_mask;
        au_sync();

        /* Start driver tasks */
        sc->tx_daemon_tid = rtems_bsdnet_newproc("ENTx",
                                                 4096,
                                                 au1x00_emac_tx_daemon,
                                                 sc);

        sc->rx_daemon_tid = rtems_bsdnet_newproc("ENRx",
                                                 4096,
                                                 au1x00_emac_rx_daemon,
                                                 sc);


    }
    /* EMAC doesn't support promiscuous, so ignore requests */
    if (ifp->if_flags & IFF_PROMISC)
        printf ("Warning - AU1X00 EMAC doesn't support Promiscuous Mode!\n");

    /*
     * Tell the world that we're running.
     */
    ifp->if_flags |= IFF_RUNNING;

    /*
     * start tx, rx
     */
    AU1X00_MAC_CONTROL(sc->ctrl_regs) |= (AU1X00_MAC_CTRL_TE |
                                             AU1X00_MAC_CTRL_RE);
    au_sync();


} /* au1x00_emac_init() */

void  au1x00_emac_init_hw(au1x00_emac_softc_t *sc)
{
    int i;
    struct mbuf *m;
    struct ifnet *ifp = &sc->arpcom.ac_if;

    /* reset the MAC */
    *sc->en_reg = 0x40;
    au_sync();
    for (i = 0; i < 10000; i++) {
        continue;
    }

/*    *sc->en_reg = AU1X00_MAC_EN_CE; */
    *sc->en_reg = 41;
    au_sync();
    for (i = 0; i < 10000; i++) {
        continue;
    }

/*
    *sc->en_reg = (AU1X00_MAC_EN_CE |
                   AU1X00_MAC_EN_E2 |
                   AU1X00_MAC_EN_E1 |
                   AU1X00_MAC_EN_E0);
*/
    *sc->en_reg = 0x33;
    au_sync();
    mii_init(sc);

    /* set the mac address */
    AU1X00_MAC_ADDRHIGH(sc->ctrl_regs) = ((sc->arpcom.ac_enaddr[5] << 8) |
                                          (sc->arpcom.ac_enaddr[4] << 0));
    AU1X00_MAC_ADDRLOW(sc->ctrl_regs) = ((sc->arpcom.ac_enaddr[3] << 24) |
                                         (sc->arpcom.ac_enaddr[2] << 16) |
                                         (sc->arpcom.ac_enaddr[1] << 8) |
                                         (sc->arpcom.ac_enaddr[0] << 0));


    /* get the MAC address from the chip */
    sc->arpcom.ac_enaddr[5] = (AU1X00_MAC_ADDRHIGH(sc->ctrl_regs) >> 8) & 0xff;
    sc->arpcom.ac_enaddr[4] = (AU1X00_MAC_ADDRHIGH(sc->ctrl_regs) >> 0) & 0xff;
    sc->arpcom.ac_enaddr[3] = (AU1X00_MAC_ADDRLOW(sc->ctrl_regs) >> 24) & 0xff;
    sc->arpcom.ac_enaddr[2] = (AU1X00_MAC_ADDRLOW(sc->ctrl_regs) >> 16) & 0xff;
    sc->arpcom.ac_enaddr[1] = (AU1X00_MAC_ADDRLOW(sc->ctrl_regs) >> 8) & 0xff;
    sc->arpcom.ac_enaddr[0] = (AU1X00_MAC_ADDRLOW(sc->ctrl_regs) >> 0) & 0xff;

    printk("Setting mac_control to 0x%x\n",
           (AU1X00_MAC_CTRL_F |
            AU1X00_MAC_CTRL_PM |
            AU1X00_MAC_CTRL_RA |
            AU1X00_MAC_CTRL_DO |
            AU1X00_MAC_CTRL_EM));

    AU1X00_MAC_CONTROL(sc->ctrl_regs) = (AU1X00_MAC_CTRL_F |   /* full duplex */
                                         AU1X00_MAC_CTRL_PM |  /* pass mcast */
                                         AU1X00_MAC_CTRL_RA |  /* recv all */
                                         AU1X00_MAC_CTRL_DO |  /* disable own */
                                         AU1X00_MAC_CTRL_EM);  /* Big endian */
    au_sync();
    printk("mac_control was set to 0x%x\n", AU1X00_MAC_CONTROL(sc->ctrl_regs));
    printk("mac_control addr is 0x%x\n", &AU1X00_MAC_CONTROL(sc->ctrl_regs));

    /* initialize our receive buffer descriptors */
    for (i = 0; i < NUM_RX_DMA_BUFS; i++) {
        MGETHDR(m, M_WAIT, MT_DATA);
        MCLGET(m, M_WAIT);

        m->m_pkthdr.rcvif = ifp;
        m->m_nextpkt = 0;

        /*
         * The receive buffer must be aligned with a cache line
         * boundary.
         */
        if (mtod(m, uint32_t) & 0x1f) {
	  uint32_t *p = mtod(m, uint32_t *);
          *p = (mtod(m, uint32_t) + 0x1f) & 0x1f;
        }
        sc->rx_dma[i].addr = (mtod(m, uint32_t) & ~0xe0000000);
        sc->rx_mbuf[i] = m;
    }

    /* Initialize transmit buffer descriptors */
    for (i = 0; i < NUM_TX_DMA_BUFS; i++) {
        sc->tx_dma[i].addr = 0;
    }

    /* initialize the transmit buffers */
    sc->tx_buf[0] = (void *)((((int)&tx_buf_base[0]) + 0x1f) & ~0x1f);
    sc->tx_buf[1] = (void *)(((int)sc->tx_buf[0]) + TX_BUF_SIZE);
    sc->tx_buf[2] = (void *)(((int)sc->tx_buf[1]) + TX_BUF_SIZE);
    sc->tx_buf[3] = (void *)(((int)sc->tx_buf[2]) + TX_BUF_SIZE);

    sc->rx_head = (sc->rx_dma[0].addr >> 2) & 0x3;
    sc->rx_tail = (sc->rx_dma[0].addr >> 2) & 0x3;
    sc->tx_head = (sc->tx_dma[0].addr >> 2) & 0x3;
    sc->tx_tail = (sc->tx_dma[0].addr >> 2) & 0x3;

    for (i = 0; i < NUM_RX_DMA_BUFS; i++) {
        sc->rx_dma[i].addr |= AU1X00_MAC_DMA_RXADDR_EN;
    }

} /* au1x00_emac_init_hw() */

void au1x00_emac_start(struct ifnet *ifp)
{
    au1x00_emac_softc_t *sc = ifp->if_softc;

    rtems_event_send(sc->tx_daemon_tid, START_TX_EVENT);
    ifp->if_flags |= IFF_OACTIVE;
}

void au1x00_emac_stop (au1x00_emac_softc_t *sc)
{
    struct ifnet *ifp = &sc->arpcom.ac_if;

    ifp->if_flags &= ~IFF_RUNNING;

    /*
     * Stop the transmitter and receiver.
     */

    /* Disable TX/RX  */
    AU1X00_MAC_CONTROL(sc->ctrl_regs) &= ~(AU1X00_MAC_CTRL_TE |
                                      AU1X00_MAC_CTRL_RE);
    au_sync();
}

/*
 * Driver tx daemon
 */
void au1x00_emac_tx_daemon (void *arg)
{
    au1x00_emac_softc_t *sc = (au1x00_emac_softc_t *)arg;
    struct ifnet *ifp = &sc->arpcom.ac_if;
    struct mbuf *m;
    rtems_event_set events;
    uint32_t ic_base;     /* interrupt controller */

    ic_base = AU1X00_IC0_ADDR;

    /* turn on interrupt, then wait for one */
    if (sc->unitnumber == 0) {
        AU1X00_IC_MASKSET(ic_base) = AU1X00_IC_IRQ_MAC0;
    } else {
        AU1X00_IC_MASKSET(ic_base) = AU1X00_IC_IRQ_MAC1;
    }
    au_sync();

    for (;;)
    {
        rtems_bsdnet_event_receive(
            START_TX_EVENT,
            RTEMS_EVENT_ANY | RTEMS_WAIT,
            RTEMS_NO_TIMEOUT,
            &events);

        /* Send packets till queue is empty */
        for (;;)
        {
            /* Get the next mbuf chain to transmit. */
            IF_DEQUEUE(&ifp->if_snd, m);
            if (!m)
                break;

            sc->tx_pkts++;
            au1x00_emac_sendpacket (ifp, m);
        }
        ifp->if_flags &= ~IFF_OACTIVE;
    }
}

/*
 * Driver rx daemon
 */
void au1x00_emac_rx_daemon (void *arg)
{
    au1x00_emac_softc_t *sc = (au1x00_emac_softc_t *)arg;
    struct ifnet *ifp = &sc->arpcom.ac_if;
    struct mbuf *m;
    struct ether_header *eh;
    rtems_event_set events;
    uint32_t status;

    while (1) {
        rtems_bsdnet_event_receive(
            START_RX_EVENT,
            RTEMS_EVENT_ANY | RTEMS_WAIT,
            RTEMS_NO_TIMEOUT,
            &events);

        /* while there are packets to receive */

        while (!(sc->rx_dma[sc->rx_head].addr & (AU1X00_MAC_DMA_RXADDR_DN |
                                                AU1X00_MAC_DMA_RXADDR_EN))) {
            status = sc->rx_dma[sc->rx_head].stat;
            if (status & AU1X00_MAC_DMA_RXSTAT_MI) {
                sc->rx_missed++;
            }
            if (status & AU1X00_MAC_DMA_RXSTAT_BF) {
                sc->rx_bcast++;
            }
            if (status & AU1X00_MAC_DMA_RXSTAT_MF) {
                sc->rx_mcast++;
            }
            if (status & AU1X00_MAC_DMA_RXSTAT_UC) {
                sc->rx_unsupp++;
            }
            if (status & AU1X00_MAC_DMA_RXSTAT_CF) {
                sc->rx_ctrl++;
            }
            if (status & AU1X00_MAC_DMA_RXSTAT_LE) {
                sc->rx_len_err++;
            }
            if (status & AU1X00_MAC_DMA_RXSTAT_CR) {
                sc->rx_crc_err++;
            }
            if (status & AU1X00_MAC_DMA_RXSTAT_DB) {
                sc->rx_dribble++;
            }
            if (status & AU1X00_MAC_DMA_RXSTAT_ME) {
                sc->rx_mii_err++;
            }
            if (status & AU1X00_MAC_DMA_RXSTAT_CS) {
                sc->rx_collision++;
            }
            if (status & AU1X00_MAC_DMA_RXSTAT_FL) {
                sc->rx_too_long++;
            }
            if (status & AU1X00_MAC_DMA_RXSTAT_RF) {
                sc->rx_runt++;
            }
            if (status & AU1X00_MAC_DMA_RXSTAT_WT) {
                sc->rx_watchdog++;
            }

            /* If no errrors, accept packet */
            if ((status & (AU1X00_MAC_DMA_RXSTAT_CR |
                           AU1X00_MAC_DMA_RXSTAT_DB |
                           AU1X00_MAC_DMA_RXSTAT_RF)) == 0) {

                sc->rx_pkts++;

                /* find the start of the mbuf */
                m = sc->rx_mbuf[sc->rx_head];

                /* set the length of the mbuf */
                m->m_len = AU1X00_MAC_DMA_RXSTAT_LEN(sc->rx_dma[sc->rx_head].stat);
                m->m_len -= 4; /* remove ethernet CRC */

                m->m_pkthdr.len = m->m_len;

                /* strip off the ethernet header from the mbuf */
                /* but save the pointer to it */
                eh = mtod (m, struct ether_header *);
                m->m_data += sizeof(struct ether_header);

                /* give the received packet to the stack */
                ether_input(ifp, eh, m);
                /* get a new buf and make it ready for the MAC */
                MGETHDR(m, M_WAIT, MT_DATA);
                MCLGET(m, M_WAIT);

                m->m_pkthdr.rcvif = ifp;
                m->m_nextpkt = 0;

                /*
                 * The receive buffer must be aligned with a cache line
                 * boundary.
                 */
		{
                  uint32_t *p = mtod(m, uint32_t *);
                  *p = (mtod(m, uint32_t) + 0x1f) & ~0x1f;
		}

            } else {
                sc->rx_dropped++;

                /* find the mbuf so we can reuse it*/
                m = sc->rx_mbuf[sc->rx_head];
            }

            /* set up the receive dma to use the mbuf's cluster */
            sc->rx_dma[sc->rx_head].addr = (mtod(m, uint32_t) & ~0xe0000000);
            au_sync();
            sc->rx_mbuf[sc->rx_head] = m;

            sc->rx_dma[sc->rx_head].addr |= AU1X00_MAC_DMA_RXADDR_EN;
            au_sync();


            /* increment the buffer index */
            sc->rx_head++;
            if (sc->rx_head >= NUM_RX_DMA_BUFS) {
                sc->rx_head = 0;
            }
        }
    }
}

/* Send packet */
void au1x00_emac_sendpacket (struct ifnet *ifp, struct mbuf *m)
{
    struct mbuf *l = NULL;
    unsigned int pkt_offset = 0;
    au1x00_emac_softc_t *sc = (au1x00_emac_softc_t *)ifp->if_softc;
    uint32_t txbuf;

    /* Wait for EMAC Transmit Queue to become available. */
    while((sc->tx_dma[sc->tx_head].addr & (AU1X00_MAC_DMA_TXADDR_EN ||
                                           AU1X00_MAC_DMA_TXADDR_DN)) != 0) {
        continue;
    }

    /* copy the mbuf chain into the transmit buffer */
    l = m;

    txbuf = (uint32_t)sc->tx_buf[sc->tx_head];
    while (l != NULL)
    {

        memcpy(((char *)txbuf + pkt_offset), /* offset into pkt for mbuf */
               (char *)mtod(l, void *),      /* cast to void */
               l->m_len);                    /* length of this mbuf */

        pkt_offset += l->m_len;              /* update offset */
        l = l->m_next;                       /* get next mbuf, if any */
    }

    /* Pad if necessary */
    if (pkt_offset < 60) {
        memset((char *)(txbuf + pkt_offset), 0, (60 - pkt_offset));
        pkt_offset = 60;
    }

    /* send it off */
    sc->tx_dma[sc->tx_head].stat = 0;
    sc->tx_dma[sc->tx_head].len = pkt_offset;
    sc->tx_dma[sc->tx_head].addr = ((txbuf & ~0xe0000000) |
                                    AU1X00_MAC_DMA_TXADDR_EN);
    au_sync();


    /*
     *Without this delay, some outgoing packets never
     * make it out the device. Nothing in the documentation
     * explains this.
     */
    for (wait_count = 0; wait_count < 5000; wait_count++){
        continue;
    }

    /* free the mbuf chain we just copied */
    m_freem(m);

    sc->tx_head++;
    if (sc->tx_head >= NUM_TX_DMA_BUFS) {
        sc->tx_head = 0;
    }

} /* au1x00_emac_sendpacket () */



/* Show interface statistics */
void au1x00_emac_stats (au1x00_emac_softc_t *sc)
{
    printf("Interrupts:%-8lu", sc->interrupts);
    printf("    RX Interrupts:%-8lu", sc->rx_interrupts);
    printf(" TX Interrupts:%-8lu\n", sc->tx_interrupts);
    printf("RX Packets:%-8lu", sc->rx_pkts);
    printf("    RX Control:%-8lu", sc->rx_ctrl);
    printf("    RX broadcast:%-8lu\n", sc->rx_bcast);
    printf("RX Mcast:%-8lu", sc->rx_mcast);
    printf("      RX missed:%-8lu", sc->rx_missed);
    printf("     RX Unsupported ctrl:%-8lu\n", sc->rx_unsupp);
    printf("RX Len err:%-8lu", sc->rx_len_err);
    printf("    RX CRC err:%-8lu", sc->rx_crc_err);
    printf("    RX dribble:%-8lu\n", sc->rx_dribble);
    printf("RX MII err:%-8lu", sc->rx_mii_err);
    printf("    RX collision:%-8lu", sc->rx_collision);
    printf("  RX too long:%-8lu\n", sc->rx_too_long);
    printf("RX runt:%-8lu", sc->rx_runt);
    printf("       RX watchdog:%-8lu", sc->rx_watchdog);
    printf("   RX dropped:%-8lu\n", sc->rx_dropped);

    printf("TX Packets:%-8lu", sc->tx_pkts);
    printf("    TX Deferred:%-8lu", sc->tx_deferred);
    printf("   TX Underrun:%-8lu\n", sc->tx_underrun);
    printf("TX Aborted:%-8lu\n", sc->tx_aborted);

}


/*  Driver ioctl handler */
static int
au1x00_emac_ioctl (struct ifnet *ifp, ioctl_command_t command, caddr_t data)
{
    au1x00_emac_softc_t *sc = ifp->if_softc;
    int error = 0;

    switch (command) {
    case SIOCGIFADDR:
    case SIOCSIFADDR:
        ether_ioctl (ifp, command, data);
        break;

    case SIOCSIFFLAGS:
        switch (ifp->if_flags & (IFF_UP | IFF_RUNNING))
        {
        case IFF_RUNNING:
            au1x00_emac_stop (sc);
            break;

        case IFF_UP:
            au1x00_emac_init (sc);
            break;

        case IFF_UP | IFF_RUNNING:
            au1x00_emac_stop (sc);
            au1x00_emac_init (sc);
            break;

        default:
            break;
        } /* switch (if_flags) */
        break;

    case SIO_RTEMS_SHOW_STATS:
        au1x00_emac_stats (sc);
        break;

        /*
         * FIXME: All sorts of multicast commands need to be added here!
         */
    default:
        error = EINVAL;
        break;
    } /* switch (command) */
    return error;
}

/* interrupt handler */
rtems_isr au1x00_emac_isr (rtems_vector_number v)
{
    volatile au1x00_emac_softc_t *sc;
    int tx_flag = 0;
    int rx_flag = 0;

    sc = &softc[0];
    if (v != AU1X00_IRQ_MAC0) {
      assert(v == AU1X00_IRQ_MAC0);
    }

    sc->interrupts++;

    /*
     * Since there's no easy way to find out the source of the
     * interrupt, we have to look at the tx and rx dma buffers
     */
    /* receive interrupt */
    while(sc->rx_dma[sc->rx_tail].addr & AU1X00_MAC_DMA_RXADDR_DN) {
        rx_flag = 1;
        sc->rx_interrupts++;
        sc->rx_dma[sc->rx_tail].addr &= ~AU1X00_MAC_DMA_RXADDR_DN;
        au_sync();

        sc->rx_tail++;
        if (sc->rx_tail >= NUM_RX_DMA_BUFS) {
            sc->rx_tail = 0;
        }
    }
    if (rx_flag != 0) {
        rtems_event_send(sc->rx_daemon_tid, START_RX_EVENT);
    }

    /* transmit interrupt */
    while (sc->tx_dma[sc->tx_tail].addr & AU1X00_MAC_DMA_TXADDR_DN) {
        uint32_t status;
        tx_flag = 1;
        sc->tx_interrupts++;

        status = sc->tx_dma[sc->tx_tail].stat;
        if (status & AU1X00_MAC_DMA_TXSTAT_DF) {
            sc->tx_deferred++;
        }
        if (status & AU1X00_MAC_DMA_TXSTAT_UR) {
            sc->tx_underrun++;
        }
        if (status & AU1X00_MAC_DMA_TXSTAT_FA) {
            sc->tx_aborted++;
        }

        sc->tx_dma[sc->tx_tail].addr = 0;
        au_sync();

        sc->tx_tail++;
        if (sc->tx_tail >= NUM_TX_DMA_BUFS) {
            sc->tx_tail = 0;
        }
    }
    if (tx_flag != 0) {
        rtems_event_send(sc->tx_daemon_tid, START_TX_EVENT);
    }
}


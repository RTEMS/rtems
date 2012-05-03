/*
 *  MC9323MXL Ethernet driver
 *
 *  Copyright (c) 2004 by Cogent Computer Systems
 *  Written by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <mc9328mxl.h>
#include "lan91c11x.h"

#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <rtems/error.h>
#include <rtems/bspIo.h>

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>

#include <net/if.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>

#include <bsp/irq.h>

/*  RTEMS event used by interrupt handler to start receive daemon. */
#define START_RECEIVE_EVENT  RTEMS_EVENT_1

/* RTEMS event used to start transmit daemon. */
#define START_TRANSMIT_EVENT    RTEMS_EVENT_2

static void enet_isr(rtems_irq_hdl_param);
static void enet_isr_on(const rtems_irq_connect_data *unused);
static void enet_isr_off(const rtems_irq_connect_data *unused);
static int enet_isr_is_on(const rtems_irq_connect_data *irq);

/* Replace the first value with the clock's interrupt name. */
rtems_irq_connect_data mc9328mxl_enet_isr_data = {
    .name    = BSP_INT_GPIO_PORTA,
    .hdl     = enet_isr,
    .handle  = (void *)BSP_INT_GPIO_PORTA,
    .on      = enet_isr_on,
    .off     = enet_isr_off,
    .isOn    = enet_isr_is_on,
};
typedef struct {
  unsigned long rx_packets;        /* total packets received         */
  unsigned long tx_packets;        /* total packets transmitted      */
  unsigned long rx_bytes;          /* total bytes received           */
  unsigned long tx_bytes;          /* total bytes transmitted        */
  unsigned long interrupts;        /* total number of interrupts     */
  unsigned long rx_interrupts;     /* total number of rx interrupts  */
  unsigned long tx_interrupts;     /* total number of tx interrupts  */
  unsigned long txerr_interrupts;  /* total number of tx error interrupts  */

} eth_stats_t;

/*
 * Hardware-specific storage
 */
typedef struct
{
    /*
     * Connection to networking code
     * This entry *must* be the first in the sonic_softc structure.
     */
    struct arpcom arpcom;

    int accept_bcast;

    /* Tasks waiting for interrupts */
    rtems_id rx_task;
    rtems_id tx_task;

    eth_stats_t stats;

} mc9328mxl_enet_softc_t;

static mc9328mxl_enet_softc_t softc;


/* function prototypes */
int rtems_mc9328mxl_enet_attach(struct rtems_bsdnet_ifconfig *config,
                                void *chip);
void mc9328mxl_enet_init(void *arg);
void mc9328mxl_enet_init_hw(mc9328mxl_enet_softc_t *sc);
void mc9328mxl_enet_start(struct ifnet *ifp);
void mc9328mxl_enet_stop (mc9328mxl_enet_softc_t *sc);
void mc9328mxl_enet_tx_task (void *arg);
void mc9328mxl_enet_sendpacket (struct ifnet *ifp, struct mbuf *m);
void mc9328mxl_enet_rx_task(void *arg);
void mc9328mxl_enet_stats(mc9328mxl_enet_softc_t *sc);
static int mc9328mxl_enet_ioctl(struct ifnet *ifp,
                                ioctl_command_t command, caddr_t data);


int rtems_mc9328mxl_enet_attach (
    struct rtems_bsdnet_ifconfig *config,
    void *chip  /* only one ethernet, so no chip number */
    )
{
    struct ifnet *ifp;
    int mtu;
    int unitnumber;
    char *unitname;
    int tmp;

    /*
     * Parse driver name
     */
    unitnumber = rtems_bsdnet_parse_driver_name(config, &unitname);
    if (unitnumber < 0) {
        return 0;
    }

    /*
     * Is driver free?
     */
    if (unitnumber != 0) {
        printf ("Bad MC9328MXL unit number.\n");
        return 0;
    }

    ifp = &softc.arpcom.ac_if;
    if (ifp->if_softc != NULL) {
        printf ("Driver already in use.\n");
        return 0;
    }

    /* zero out the control structure  */
    memset( &softc, 0, sizeof(softc) );


    /* set the MAC address */
    tmp = lan91c11x_read_reg(LAN91C11X_IA0);
    softc.arpcom.ac_enaddr[0] = tmp & 0xff;
    softc.arpcom.ac_enaddr[1] = (tmp >> 8) & 0xff;

    tmp = lan91c11x_read_reg(LAN91C11X_IA2);
    softc.arpcom.ac_enaddr[2] = tmp & 0xff;
    softc.arpcom.ac_enaddr[3] = (tmp >> 8) & 0xff;

    tmp = lan91c11x_read_reg(LAN91C11X_IA4);
    softc.arpcom.ac_enaddr[4] = tmp & 0xff;
    softc.arpcom.ac_enaddr[5] = (tmp >> 8) & 0xff;

    if (config->mtu) {
        mtu = config->mtu;
    } else {
        mtu = ETHERMTU;
    }

    softc.accept_bcast = !config->ignore_broadcast;

    /*
     * Set up network interface values
     */
    ifp->if_softc = &softc;
    ifp->if_unit = unitnumber;
    ifp->if_name = unitname;
    ifp->if_mtu = mtu;
    ifp->if_init = mc9328mxl_enet_init;
    ifp->if_ioctl = mc9328mxl_enet_ioctl;
    ifp->if_start = mc9328mxl_enet_start;
    ifp->if_output = ether_output;
    ifp->if_flags = IFF_BROADCAST;
    if (ifp->if_snd.ifq_maxlen == 0) {
        ifp->if_snd.ifq_maxlen = ifqmaxlen;
    }

    /* Attach the interface */
    if_attach (ifp);
    ether_ifattach (ifp);
    return 1;
}

void mc9328mxl_enet_init(void *arg)
{
    mc9328mxl_enet_softc_t     *sc = arg;
    struct ifnet *ifp = &sc->arpcom.ac_if;

    /*
     *This is for stuff that only gets done once (mc9328mxl_enet_init()
     * gets called multiple times
     */
    if (sc->tx_task == 0)
    {
        /* Set up ENET hardware */
        mc9328mxl_enet_init_hw(sc);

        /* Start driver tasks */
        sc->rx_task = rtems_bsdnet_newproc("ENrx",
                                           4096,
                                           mc9328mxl_enet_rx_task,
                                           sc);
        sc->tx_task = rtems_bsdnet_newproc("ENtx",
                                           4096,
                                           mc9328mxl_enet_tx_task,
                                           sc);
    } /* if tx_task */


    /* Configure for promiscuous if needed */
    if (ifp->if_flags & IFF_PROMISC) {
        lan91c11x_write_reg(LAN91C11X_RCR,
                            (lan91c11x_read_reg(LAN91C11X_RCR) |
                             LAN91C11X_RCR_PRMS));
    }


    /*
     * Tell the world that we're running.
     */
    ifp->if_flags |= IFF_RUNNING;

    /* Enable TX/RX */
    lan91c11x_write_reg(LAN91C11X_TCR,
                        (lan91c11x_read_reg(LAN91C11X_TCR) |
                         LAN91C11X_TCR_TXENA));

    lan91c11x_write_reg(LAN91C11X_RCR,
                        (lan91c11x_read_reg(LAN91C11X_RCR) |
                         LAN91C11X_RCR_RXEN));


} /* mc9328mxl_enet_init() */

void  mc9328mxl_enet_init_hw(mc9328mxl_enet_softc_t *sc)
{
    uint16_t stat;
    uint16_t my = 0;

    lan91c11x_write_reg(LAN91C11X_RCR, LAN91C11X_RCR_RST);
    lan91c11x_write_reg(LAN91C11X_RCR, 0);
    rtems_task_wake_after(1);

    /* Reset the PHY */
    lan91c11x_write_phy_reg(PHY_CTRL, PHY_CTRL_RST);
    while(lan91c11x_read_phy_reg(PHY_CTRL) & PHY_CTRL_RST) {
        rtems_task_wake_after(1);
    }


    stat = lan91c11x_read_phy_reg(PHY_STAT);

    if(stat & PHY_STAT_CAPT4) {
        my |= PHY_ADV_T4;
    }
/* 100Mbs doesn't work, so we won't advertise it */

    if(stat & PHY_STAT_CAPTXF) {
        my |= PHY_ADV_TXFDX;
    }
    if(stat & PHY_STAT_CAPTXH) {
        my |= PHY_ADV_TXHDX;
    }

    if(stat & PHY_STAT_CAPTF) {
        my |= PHY_ADV_10FDX;
    }

    if(stat & PHY_STAT_CAPTH) {
        my |= PHY_ADV_10HDX;
    }

    my |= PHY_ADV_CSMA;

    lan91c11x_write_phy_reg(PHY_AD, my);


    /* Enable Autonegotiation */
#if 0
    lan91c11x_write_phy_reg(PHY_CTRL,
                            (PHY_CTRL_ANEGEN | PHY_CTRL_ANEGRST));
#endif

    /* Enable full duplex, let MAC take care
     * of padding and CRC.
     */
    lan91c11x_write_reg(LAN91C11X_TCR,
                        (LAN91C11X_TCR_PADEN |
                         LAN91C11X_TCR_SWFDUP));

    /* Disable promisc, don'tstrip CRC */
    lan91c11x_write_reg(LAN91C11X_RCR, 0);

    /* Enable auto-negotiation, LEDA is link, LEDB is traffic */
    lan91c11x_write_reg(LAN91C11X_RPCR,
                        (LAN91C11X_RPCR_ANEG |
                         LAN91C11X_RPCR_LS2B));

    /* Don't add wait states, enable PHY power */
    lan91c11x_write_reg(LAN91C11X_CONFIG,
                        (LAN91C11X_CONFIG_NOWAIT |
                         LAN91C11X_CONFIG_PWR));

    /* Disable error interrupts, enable auto release */
    lan91c11x_write_reg(LAN91C11X_CTRL, LAN91C11X_CTRL_AUTO);

    /* Reset MMU */
    lan91c11x_write_reg(LAN91C11X_MMUCMD,
                        LAN91C11X_MMUCMD_RESETMMU );


    rtems_task_wake_after(100);
    /* Enable Autonegotiation */
    lan91c11x_write_phy_reg(PHY_CTRL, 0x3000);
    rtems_task_wake_after(100);

    /* Enable Interrupts for RX */
    lan91c11x_write_reg(LAN91C11X_INT, LAN91C11X_INT_RXMASK);

    /* Enable interrupts on GPIO Port A3 */
    /*   Make pin 3 an input */
    MC9328MXL_GPIOA_DDIR &= ~bit(3);

    /*   Use GPIO function for pin 3 */
    MC9328MXL_GPIOA_GIUS |= bit(3);

    /*   Set for active high, level triggered interupt */
    MC9328MXL_GPIOA_ICR1 = ((MC9328MXL_GPIOA_ICR1 & ~(3 << 6)) |
                              (2 << 6));

    /*   Enable GPIO port A3 interrupt */
    MC9328MXL_GPIOA_IMR |= bit(3);

    /* Install the interrupt handler */
    BSP_install_rtems_irq_handler(&mc9328mxl_enet_isr_data);

} /* mc9328mxl_enet_init_hw() */

void mc9328mxl_enet_start(struct ifnet *ifp)
{
    mc9328mxl_enet_softc_t *sc = ifp->if_softc;

    rtems_event_send(sc->tx_task, START_TRANSMIT_EVENT);
    ifp->if_flags |= IFF_OACTIVE;
}

void mc9328mxl_enet_stop (mc9328mxl_enet_softc_t *sc)
{
    struct ifnet *ifp = &sc->arpcom.ac_if;

    ifp->if_flags &= ~IFF_RUNNING;


    /* Stop the transmitter and receiver. */
    lan91c11x_write_reg(LAN91C11X_TCR,
                        (lan91c11x_read_reg(LAN91C11X_TCR) &
                         ~LAN91C11X_TCR_TXENA));

    lan91c11x_write_reg(LAN91C11X_RCR,
                        (lan91c11x_read_reg(LAN91C11X_RCR) &
                        ~LAN91C11X_RCR_RXEN));

}

/*
 * Driver transmit daemon
 */
void mc9328mxl_enet_tx_task(void *arg)
{
    mc9328mxl_enet_softc_t *sc = (mc9328mxl_enet_softc_t *)arg;
    struct ifnet *ifp = &sc->arpcom.ac_if;
    struct mbuf *m;
    rtems_event_set events;

    for (;;)
    {
        rtems_bsdnet_event_receive(
            START_TRANSMIT_EVENT,
            RTEMS_EVENT_ANY | RTEMS_WAIT,
            RTEMS_NO_TIMEOUT,
            &events);

        /* Send packets till queue is empty */
        for (;;)
        {
            /* Get the next mbuf chain to transmit. */
            IF_DEQUEUE(&ifp->if_snd, m);
            if (!m) {
                break;
            }
            mc9328mxl_enet_sendpacket (ifp, m);
            softc.stats.tx_packets++;

        }
        ifp->if_flags &= ~IFF_OACTIVE;
    }
}

/* Send packet */
void mc9328mxl_enet_sendpacket (struct ifnet *ifp, struct mbuf *m)
{
    struct mbuf *l = NULL;
    int size = 0;
    int tmp;
    int i;
    int start;
    uint16_t d;

    /* How big is the packet ? */
    l = m;
    do {
        size += l->m_len;
        l = l->m_next;
    } while (l != NULL);

    /* Allocate a TX buffer */
    lan91c11x_write_reg(LAN91C11X_MMUCMD,
                        (LAN91C11X_MMUCMD_ALLOCTX |
                         (size >> 8)));

    /* Wait for the allocation */
    while ((lan91c11x_read_reg(LAN91C11X_INT) & LAN91C11X_INT_ALLOC) == 0) {
        continue;
    }

    tmp = lan91c11x_read_reg(LAN91C11X_PNR);
    lan91c11x_write_reg(LAN91C11X_PNR, ((tmp >> 8) & 0xff));

    /* Set the data pointer for auto increment */
    lan91c11x_write_reg(LAN91C11X_PTR, LAN91C11X_PTR_AUTOINC);

    /* A delay is needed between pointer and data access ?!? */
    for (i = 0; i < 10; i++) {
        continue;
    }

    /* Write status word */
    lan91c11x_write_reg(LAN91C11X_DATA, 0);

    /* Write byte count */
    if (size & 1) {
        size++;
    }
    lan91c11x_write_reg(LAN91C11X_DATA, size + 6);

    lan91c11x_lock();

    /* Copy the mbuf */
    l = m;
    start = 0;
    d = 0;
    while (l != NULL)
    {
        uint8_t *data;

        data = mtod(l, uint8_t *);

        for (i = start; i < l->m_len; i++) {
            if ((i & 1) == 0) {
                d = data[i] << 8;
            } else {
                d = d | data[i];
                lan91c11x_write_reg_fast(LAN91C11X_DATA, htons(d));
            }
        }

        /* If everything is 2 byte aligned, i will be even */
        start = (i & 1);

        l = l->m_next;
    }

    /* write control byte */
    if (i & 1) {
        lan91c11x_write_reg_fast(LAN91C11X_DATA,
                            htons(LAN91C11X_PKT_CTRL_ODD | d));
    } else {
        lan91c11x_write_reg_fast(LAN91C11X_DATA, 0);
    }

    lan91c11x_unlock();

    /* Enable TX interrupts */
    lan91c11x_write_reg(LAN91C11X_INT,
                        (lan91c11x_read_reg(LAN91C11X_INT) |
                         LAN91C11X_INT_TXMASK |
                         LAN91C11X_INT_TXEMASK));

    /* Enqueue it */
    lan91c11x_write_reg(LAN91C11X_MMUCMD,
                        LAN91C11X_MMUCMD_ENQUEUE);

    /* free the mbuf chain we just copied */
    m_freem(m);

} /* mc9328mxl_enet_sendpacket () */


/* reader task */
void mc9328mxl_enet_rx_task(void *arg)
{
    mc9328mxl_enet_softc_t *sc = (mc9328mxl_enet_softc_t *)arg;
    struct ifnet *ifp = &sc->arpcom.ac_if;
    struct mbuf *m;
    struct ether_header *eh;
    rtems_event_set events;
    int pktlen;
    uint16_t rsw;
    uint16_t bc;
    uint16_t cbyte;
    int i;
    uint16_t int_reg;

    /* Input packet handling loop */
    while (1) {
        rtems_bsdnet_event_receive(
            START_RECEIVE_EVENT,
            RTEMS_EVENT_ANY | RTEMS_WAIT,
            RTEMS_NO_TIMEOUT,
            &events);

        /* Configure for reads from RX data area */
        lan91c11x_write_reg(LAN91C11X_PTR,
                            (LAN91C11X_PTR_AUTOINC |
                             LAN91C11X_PTR_RCV |
                             LAN91C11X_PTR_READ));

        /* read the receive status word */
        rsw = lan91c11x_read_reg(LAN91C11X_DATA);
        /* TBD: Need to check rsw here */

        /* read the byte count */
        bc = lan91c11x_read_reg(LAN91C11X_DATA);
        pktlen = (bc & 0x7ff) - 6;

        /* get an mbuf for this packet */
        MGETHDR(m, M_WAIT, MT_DATA);

        /* now get a cluster pointed to by the mbuf */
        /* since an mbuf by itself is too small */
        MCLGET(m, M_WAIT);

        lan91c11x_lock();

        /* Copy the received packet into an mbuf */
        for (i = 0; i < (pktlen / 2); i++) {
            ((uint16_t*)m->m_ext.ext_buf)[i] =
                lan91c11x_read_reg_fast(LAN91C11X_DATA);
        }

        cbyte = lan91c11x_read_reg_fast(LAN91C11X_DATA);
        if (cbyte & LAN91C11X_PKT_CTRL_ODD) {
            ((uint16_t*)m->m_ext.ext_buf)[i] = cbyte;
            pktlen++;
        }
        lan91c11x_unlock();

        /* Release the packets memory */
        lan91c11x_write_reg(LAN91C11X_MMUCMD,
                            LAN91C11X_MMUCMD_REMTOP);

        /* set the receiving interface */
        m->m_pkthdr.rcvif = ifp;
        m->m_nextpkt = 0;

        /* set the length of the mbuf */
        m->m_len = pktlen - (sizeof(struct ether_header));
        m->m_pkthdr.len = m->m_len;

        /* strip off the ethernet header from the mbuf */
        /* but save the pointer to it */
        eh = mtod (m, struct ether_header *);
        m->m_data += sizeof(struct ether_header);


        softc.stats.rx_packets++;

        /* give all this stuff to the stack */
        ether_input(ifp, eh, m);

        /* renable RX interrupts */
        int_reg = lan91c11x_read_reg(LAN91C11X_INT);
        int_reg |= LAN91C11X_INT_RXMASK;
        lan91c11x_write_reg(LAN91C11X_INT, int_reg);

    }
} /* mc9328mxl_enet_rx_task */


/* Show interface statistics */
void mc9328mxl_enet_stats (mc9328mxl_enet_softc_t *sc)
{
    printf (" Total Interrupts:%-8lu",         sc->stats.interrupts);
    printf ("   Rx Interrupts:%-8lu",          sc->stats.rx_interrupts);
    printf ("   Tx Interrupts:%-8lu\n",        sc->stats.tx_interrupts);
    printf (" Tx Error Interrupts:%-8lu\n",    sc->stats.txerr_interrupts);
    printf (" Rx Packets:%-8lu",               sc->stats.rx_packets);
    printf ("   Tx Packets:%-8lu\n",           sc->stats.tx_packets);
}


/* Enables mc9328mxl_enet interrupts. */
static void enet_isr_on(const rtems_irq_connect_data *unused)
{
    /* Enable interrupts */
    MC9328MXL_AITC_INTENNUM = MC9328MXL_INT_GPIO_PORTA;

    return;
}

/* Disables enet interrupts */
static void enet_isr_off(const rtems_irq_connect_data *unused)
{
    /* disable all various TX/RX interrupts */
    MC9328MXL_AITC_INTDISNUM = MC9328MXL_INT_GPIO_PORTA;

    return;
}

/* Tests to see if mc9328mxl_enet interrupts are enabled, and returns non-0 if so.
 * If interrupt is not enabled, returns 0.
 */
static int enet_isr_is_on(const rtems_irq_connect_data *irq)
{
    return MC9328MXL_AITC_INTENABLEL & (1 << MC9328MXL_INT_GPIO_PORTA);
}

/*  Driver ioctl handler */
static int
mc9328mxl_enet_ioctl (struct ifnet *ifp, ioctl_command_t command, caddr_t data)
{
    mc9328mxl_enet_softc_t *sc = ifp->if_softc;
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
            mc9328mxl_enet_stop (sc);
            break;

        case IFF_UP:
            mc9328mxl_enet_init (sc);
            break;

        case IFF_UP | IFF_RUNNING:
            mc9328mxl_enet_stop (sc);
            mc9328mxl_enet_init (sc);
            break;

        default:
            break;
        } /* switch (if_flags) */
        break;

    case SIO_RTEMS_SHOW_STATS:
        mc9328mxl_enet_stats (sc);
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
static void enet_isr(rtems_irq_hdl_param unused)
{
    uint16_t int_reg;

    softc.stats.interrupts++;
    /* get the ISR status and determine RX or TX */
    int_reg = lan91c11x_read_reg(LAN91C11X_INT);

    /* Handle RX interrupts */
    if ((int_reg & LAN91C11X_INT_RX) && (int_reg & LAN91C11X_INT_RXMASK)) {
        softc.stats.rx_interrupts++;

        /* Disable the interrupt */
        int_reg &= ~LAN91C11X_INT_RXMASK;

        rtems_event_send (softc.rx_task, START_RECEIVE_EVENT);
    }

    /* Handle TX Empty interrupts */
    if ((int_reg & LAN91C11X_INT_TXE) && (int_reg & LAN91C11X_INT_TXEMASK)) {
        softc.stats.tx_interrupts++;

        /* Disable the interrupt */
        int_reg &= ~LAN91C11X_INT_TXEMASK;

        /* Acknowledge the interrupt */
        int_reg |= LAN91C11X_INT_TXE;

        rtems_event_send(softc.tx_task, START_TRANSMIT_EVENT);

    }

    /* Handle interrupts for transmit errors */
    if ((int_reg & LAN91C11X_INT_TX) && (int_reg & LAN91C11X_INT_TXMASK)) {
        softc.stats.txerr_interrupts++;
        printk("Caught TX interrupt - error on transmission\n");
    }

    /* Update the interrupt register on the 91c11x */
    lan91c11x_write_reg(LAN91C11X_INT, int_reg);

    /* clear GPIO Int. status */
    MC9328MXL_GPIOA_ISR |= bit(3);
}


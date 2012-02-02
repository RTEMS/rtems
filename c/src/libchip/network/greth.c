/*
 * Gaisler Research ethernet MAC driver
 * adapted from Opencores driver by Marko Isomaki
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 *
 * 2007-09-07, Ported GBIT support from 4.6.5
 */

#include <rtems.h>

#define GRETH_SUPPORTED
#include <bsp.h>

#include <inttypes.h>
#include <errno.h>
#include <rtems/bspIo.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>
#include "greth.h"

#include <sys/param.h>
#include <sys/mbuf.h>

#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>

#ifdef malloc
#undef malloc
#endif
#ifdef free
#undef free
#endif

#if defined(__m68k__)
extern m68k_isr_entry set_vector( rtems_isr_entry, rtems_vector_number, int );
#else
extern rtems_isr_entry set_vector( rtems_isr_entry, rtems_vector_number, int );
#endif


/* #define GRETH_DEBUG */

#ifdef CPU_U32_FIX
extern void ipalign(struct mbuf *m);
#endif

/*
 * Number of OCs supported by this driver
 */
#define NOCDRIVER	1

/*
 * Receive buffer size -- Allow for a full ethernet packet including CRC
 */
#define RBUF_SIZE 1518

#define	ET_MINLEN 64		/* minimum message length */

/*
 * RTEMS event used by interrupt handler to signal driver tasks.
 * This must not be any of the events used by the network task synchronization.
 */
#define INTERRUPT_EVENT	RTEMS_EVENT_1

/*
 * RTEMS event used to start transmit daemon.
 * This must not be the same as INTERRUPT_EVENT.
 */
#define START_TRANSMIT_EVENT	RTEMS_EVENT_2

 /* event to send when tx buffers become available */
#define GRETH_TX_WAIT_EVENT  RTEMS_EVENT_3

 /* suspend when all TX descriptors exhausted */
 /*
#define GRETH_SUSPEND_NOTXBUF
 */

#if (MCLBYTES < RBUF_SIZE)
# error "Driver must have MCLBYTES > RBUF_SIZE"
#endif

/* 4s Autonegotiation Timeout */
#ifndef GRETH_AUTONEGO_TIMEOUT_MS
#define GRETH_AUTONEGO_TIMEOUT_MS 4000
#endif

/* For optimizing the autonegotiation time */
#define GRETH_AUTONEGO_PRINT_TIME

/* Ethernet buffer descriptor */

typedef struct _greth_rxtxdesc {
   volatile uint32_t ctrl; /* Length and status */
   uint32_t *addr;         /* Buffer pointer */
} greth_rxtxdesc;


/*
 * Per-device data
 */
struct greth_softc
{

   struct arpcom arpcom;

   greth_regs *regs;

   int acceptBroadcast;
   rtems_id rxDaemonTid;
   rtems_id txDaemonTid;

   unsigned int tx_ptr;
   unsigned int tx_dptr;
   unsigned int tx_cnt;
   unsigned int rx_ptr;
   unsigned int txbufs;
   unsigned int rxbufs;
   greth_rxtxdesc *txdesc;
   greth_rxtxdesc *rxdesc;
   struct mbuf **rxmbuf;
   struct mbuf **txmbuf;
   rtems_vector_number vector;

   /*Status*/
   struct phy_device_info phydev;
   int fd;
   int sp;
   int gb;
   int gbit_mac;
   int auto_neg;
   unsigned int auto_neg_time;

   /*
    * Statistics
    */
   unsigned long rxInterrupts;

   unsigned long rxPackets;
   unsigned long rxLengthError;
   unsigned long rxNonOctet;
   unsigned long rxBadCRC;
   unsigned long rxOverrun;

   unsigned long txInterrupts;

   unsigned long txDeferred;
   unsigned long txHeartbeat;
   unsigned long txLateCollision;
   unsigned long txRetryLimit;
   unsigned long txUnderrun;

};

static struct greth_softc greth;

static char *almalloc(int sz)
{
        char *tmp;
        tmp = calloc(1,2*sz);
        tmp = (char *) (((uintptr_t)tmp+sz) & ~(sz -1));
        return(tmp);
}

/* GRETH interrupt handler */

static rtems_isr
greth_interrupt_handler (rtems_vector_number v)
{
        uint32_t status;
        /* read and clear interrupt cause */

        status = greth.regs->status;
        greth.regs->status = status;

        /* Frame received? */
        if (status & (GRETH_STATUS_RXERR | GRETH_STATUS_RXIRQ))
        {
                greth.rxInterrupts++;
                rtems_event_send (greth.rxDaemonTid, INTERRUPT_EVENT);
        }
#ifdef GRETH_SUSPEND_NOTXBUF
        if (status & (GRETH_STATUS_TXERR | GRETH_STATUS_TXIRQ))
        {
                greth.txInterrupts++;
                rtems_event_send (greth.txDaemonTid, GRETH_TX_WAIT_EVENT);
        }
#endif
        /*
          #ifdef __leon__
          LEON_Clear_interrupt(v-0x10);
          #endif
        */
}

static uint32_t read_mii(uint32_t phy_addr, uint32_t reg_addr)
{
    while (greth.regs->mdio_ctrl & GRETH_MDIO_BUSY) {}
    greth.regs->mdio_ctrl = (phy_addr << 11) | (reg_addr << 6) | GRETH_MDIO_READ;
    while (greth.regs->mdio_ctrl & GRETH_MDIO_BUSY) {}
    if (!(greth.regs->mdio_ctrl & GRETH_MDIO_LINKFAIL))
        return((greth.regs->mdio_ctrl >> 16) & 0xFFFF);
    else {
	printf("greth: failed to read mii\n");
	return (0);
    }
}

static void write_mii(uint32_t phy_addr, uint32_t reg_addr, uint32_t data)
{
    while (greth.regs->mdio_ctrl & GRETH_MDIO_BUSY) {}
    greth.regs->mdio_ctrl =
     ((data & 0xFFFF) << 16) | (phy_addr << 11) | (reg_addr << 6) | GRETH_MDIO_WRITE;
    while (greth.regs->mdio_ctrl & GRETH_MDIO_BUSY) {}
}

static void print_init_info(struct greth_softc *sc)
{
        printf("greth: driver attached\n");
        printf("**** PHY ****\n");
        printf("Vendor: %x   Device: %x   Revision: %d\n",sc->phydev.vendor, sc->phydev.device, sc->phydev.rev);
        printf("Current Operating Mode: ");
        if (sc->gb) {
                printf("1000 Mbit ");
        } else if (sc->sp) {
                printf("100 Mbit ");
        } else {
                printf("10 Mbit ");
        }
        if (sc->fd) {
                printf("Full Duplex\n");
        } else {
                printf("Half Duplex\n");
        }
#ifdef GRETH_AUTONEGO_PRINT_TIME
        if ( sc->auto_neg ){
          printf("Autonegotiation Time: %dms\n",sc->auto_neg_time);
        }
#endif
}


/*
 * Initialize the ethernet hardware
 */
static void
greth_initialize_hardware (struct greth_softc *sc)
{
    struct mbuf *m;
    int i;
    int phyaddr;
    int phyctrl;
    int phystatus;
    int tmp1;
    int tmp2;
    unsigned int msecs;
    struct timeval tstart, tnow;
    int anegtout;

    greth_regs *regs;

    regs = sc->regs;

    /* Reset the controller.  */
    greth.rxInterrupts = 0;
    greth.rxPackets = 0;

    regs->ctrl = 0;
    regs->ctrl = GRETH_CTRL_RST;	/* Reset ON */
    regs->ctrl = 0;			/* Reset OFF */

    /* Check if mac is gbit capable*/
    sc->gbit_mac = (regs->ctrl >> 27) & 1;

    /* Get the phy address which assumed to have been set
       correctly with the reset value in hardware*/
    phyaddr = (regs->mdio_ctrl >> 11) & 0x1F;

    /* get phy control register default values */
    while ((phyctrl = read_mii(phyaddr, 0)) & 0x8000) {}

    /* reset PHY and wait for completion */
    write_mii(phyaddr, 0, 0x8000 | phyctrl);

    while ((read_mii(phyaddr, 0)) & 0x8000) {}

    /* Check if PHY is autoneg capable and then determine operating mode,
       otherwise force it to 10 Mbit halfduplex */
    sc->gb = 0;
    sc->fd = 0;
    sc->sp = 0;
    sc->auto_neg = 0;
    sc->auto_neg_time = 0;
    /* the anegtout variable is needed because print cannot be done before mac has
       been reconfigured due to a possible deadlock situation if rtems 
       is run through the edcl with uart polling (-u)*/
    anegtout = 0;
    if ((phyctrl >> 12) & 1) {
            /*wait for auto negotiation to complete*/
            msecs = 0;
            sc->auto_neg = 1;
            if ( rtems_clock_get_tod_timeval(&tstart) == RTEMS_NOT_DEFINED){
                /* Not inited, set to epoch */
                rtems_time_of_day time;
                time.year   = 1988;
                time.month  = 1;
                time.day    = 1;
                time.hour   = 0;
                time.minute = 0;
                time.second = 0;
                time.ticks  = 0;
                rtems_clock_set(&time);

                tstart.tv_sec = 0;
                tstart.tv_usec = 0;
                rtems_clock_get_tod_timeval(&tstart);
            }
            while (!(((phystatus = read_mii(phyaddr, 1)) >> 5) & 1)) {
                    if ( rtems_clock_get_tod_timeval(&tnow) != RTEMS_SUCCESSFUL )
                      printk("rtems_clock_get_tod_timeval failed\n\r");
                    msecs = (tnow.tv_sec-tstart.tv_sec)*1000+(tnow.tv_usec-tstart.tv_usec)/1000;
                    if ( msecs > GRETH_AUTONEGO_TIMEOUT_MS ){
                            sc->auto_neg_time = msecs;
                            anegtout = 1
                            tmp1 = read_mii(phyaddr, 0);
                            sc->gb = ((phyctrl >> 6) & 1) && !((phyctrl >> 13) & 1);
                            sc->sp = !((phyctrl >> 6) & 1) && ((phyctrl >> 13) & 1);
                            sc->fd = (phyctrl >> 8) & 1;
                            goto auto_neg_done;
                    }
            }
            sc->auto_neg_time = msecs;
            sc->phydev.adv = read_mii(phyaddr, 4);
            sc->phydev.part = read_mii(phyaddr, 5);
            if ((phystatus >> 8) & 1) {
                    sc->phydev.extadv = read_mii(phyaddr, 9);
                    sc->phydev.extpart = read_mii(phyaddr, 10);
                       if ( (sc->phydev.extadv & GRETH_MII_EXTADV_1000FD) &&
                            (sc->phydev.extpart & GRETH_MII_EXTPRT_1000FD)) {
                               sc->gb = 1;
                               sc->fd = 1;
                       }
                       if ( (sc->phydev.extadv & GRETH_MII_EXTADV_1000HD) &&
                            (sc->phydev.extpart & GRETH_MII_EXTPRT_1000HD)) {
                               sc->gb = 1;
                               sc->fd = 0;
                       }
            }
            if ((sc->gb == 0) || ((sc->gb == 1) && (sc->gbit_mac == 0))) {
                    if ( (sc->phydev.adv & GRETH_MII_100TXFD) &&
                         (sc->phydev.part & GRETH_MII_100TXFD)) {
                            sc->sp = 1;
                            sc->fd = 1;
                    }
                    if ( (sc->phydev.adv & GRETH_MII_100TXHD) &&
                         (sc->phydev.part & GRETH_MII_100TXHD)) {
                            sc->sp = 1;
                            sc->fd = 0;
                    }
                    if ( (sc->phydev.adv & GRETH_MII_10FD) &&
                         (sc->phydev.part & GRETH_MII_10FD)) {
                            sc->fd = 1;
                    }
            }
    }
auto_neg_done:
    sc->phydev.vendor = 0;
    sc->phydev.device = 0;
    sc->phydev.rev = 0;
    phystatus = read_mii(phyaddr, 1);

    /*Read out PHY info if extended registers are available */
    if (phystatus & 1) {
            tmp1 = read_mii(phyaddr, 2);
            tmp2 = read_mii(phyaddr, 3);

            sc->phydev.vendor = (tmp1 << 6) | ((tmp2 >> 10) & 0x3F);
            sc->phydev.rev = tmp2 & 0xF;
            sc->phydev.device = (tmp2 >> 4) & 0x3F;
    }

    /* Force to 10 mbit half duplex if the 10/100 MAC is used with a 1000 PHY*/
    /*check if marvell 88EE1111 PHY. Needs special reset handling */
    if ((phystatus & 1) && (sc->phydev.vendor == 0x005043) && (sc->phydev.device == 0x0C)) {
            if (((sc->gb) && !(sc->gbit_mac)) || !((phyctrl >> 12) & 1)) {
                    write_mii(phyaddr, 0, sc->sp << 13);
                    write_mii(phyaddr, 0, 0x8000);
                    sc->gb = 0;
                    sc->sp = 0;
                    sc->fd = 0;
            }
    } else {
            if (((sc->gb) && !(sc->gbit_mac))  || !((phyctrl >> 12) & 1)) {
                    write_mii(phyaddr, 0, sc->sp << 13);
                    sc->gb = 0;
                    sc->sp = 0;
                    sc->fd = 0;
            }
    }
    while ((read_mii(phyaddr, 0)) & 0x8000) {}

    regs->ctrl = 0;
    regs->ctrl = GRETH_CTRL_RST;	/* Reset ON */
    regs->ctrl = 0;

    /* Initialize rx/tx descriptor pointers */
    sc->txdesc = (greth_rxtxdesc *) almalloc(1024);
    sc->rxdesc = (greth_rxtxdesc *) almalloc(1024);
    sc->tx_ptr = 0;
    sc->tx_dptr = 0;
    sc->tx_cnt = 0;
    sc->rx_ptr = 0;
    regs->txdesc = (uintptr_t) sc->txdesc;
    regs->rxdesc = (uintptr_t) sc->rxdesc;

    sc->rxmbuf = calloc(sc->rxbufs, sizeof(*sc->rxmbuf));
    sc->txmbuf = calloc(sc->txbufs, sizeof(*sc->txmbuf));

    for (i = 0; i < sc->txbufs; i++)
      {
              sc->txdesc[i].ctrl = 0;
              if (!(sc->gbit_mac)) {
                      sc->txdesc[i].addr = malloc(GRETH_MAXBUF_LEN);
              }
#ifdef GRETH_DEBUG
              /* printf("TXBUF: %08x\n", (int) sc->txdesc[i].addr); */
#endif
      }
    for (i = 0; i < sc->rxbufs; i++)
      {

          MGETHDR (m, M_WAIT, MT_DATA);
          MCLGET (m, M_WAIT);
          if (sc->gbit_mac)
                  m->m_data += 2;
	  m->m_pkthdr.rcvif = &sc->arpcom.ac_if;
          sc->rxmbuf[i] = m;
	  sc->rxdesc[i].addr = (uint32_t *) mtod(m, uint32_t *);
          sc->rxdesc[i].ctrl = GRETH_RXD_ENABLE | GRETH_RXD_IRQ;
#ifdef GRETH_DEBUG
/* 	  printf("RXBUF: %08x\n", (int) sc->rxdesc[i].addr); */
#endif
      }
    sc->rxdesc[sc->rxbufs - 1].ctrl |= GRETH_RXD_WRAP;

    /* set ethernet address.  */
    regs->mac_addr_msb =
      sc->arpcom.ac_enaddr[0] << 8 | sc->arpcom.ac_enaddr[1];

    uint32_t mac_addr_lsb;
    mac_addr_lsb = sc->arpcom.ac_enaddr[2];
    mac_addr_lsb <<= 8;
    mac_addr_lsb |= sc->arpcom.ac_enaddr[3];
    mac_addr_lsb <<= 8;
    mac_addr_lsb |= sc->arpcom.ac_enaddr[4];
    mac_addr_lsb <<= 8;
    mac_addr_lsb |= sc->arpcom.ac_enaddr[5];
    regs->mac_addr_lsb = mac_addr_lsb;

    /* install interrupt vector */
    set_vector(greth_interrupt_handler, sc->vector, 1);

    /* clear all pending interrupts */

    regs->status = 0xffffffff;

#ifdef GRETH_SUSPEND_NOTXBUF
    regs->ctrl |= GRETH_CTRL_TXIRQ;
#endif

    regs->ctrl |= GRETH_CTRL_RXEN | (sc->fd << 4) | GRETH_CTRL_RXIRQ | (sc->sp << 7) | (sc->gb << 8);
    
    if (anegtout) {
            printk("Auto negotiation timed out. Selecting default config\n\r");
    }
    
    print_init_info(sc);
}

static void
greth_rxDaemon (void *arg)
{
    struct ether_header *eh;
    struct greth_softc *dp = (struct greth_softc *) &greth;
    struct ifnet *ifp = &dp->arpcom.ac_if;
    struct mbuf *m;
    unsigned int len, len_status, bad;
    rtems_event_set events;

    for (;;)
      {
        rtems_bsdnet_event_receive (INTERRUPT_EVENT,
                                    RTEMS_WAIT | RTEMS_EVENT_ANY,
                                    RTEMS_NO_TIMEOUT, &events);

#ifdef GRETH_ETH_DEBUG
    printf ("r\n");
#endif
    while (!((len_status =
		    dp->rxdesc[dp->rx_ptr].ctrl) & GRETH_RXD_ENABLE))
	    {
                    bad = 0;
                    if (len_status & GRETH_RXD_TOOLONG)
                    {
                            dp->rxLengthError++;
                            bad = 1;
                    }
                    if (len_status & GRETH_RXD_DRIBBLE)
                    {
                            dp->rxNonOctet++;
                            bad = 1;
                    }
                    if (len_status & GRETH_RXD_CRCERR)
                    {
                            dp->rxBadCRC++;
                            bad = 1;
                    }
                    if (len_status & GRETH_RXD_OVERRUN)
                    {
                            dp->rxOverrun++;
                            bad = 1;
                    }
                    if (len_status & GRETH_RXD_LENERR)
                    {
                            dp->rxLengthError++;
                            bad = 1;
                    }
                    if (!bad)
                    {
                            /* pass on the packet in the receive buffer */
                            len = len_status & 0x7FF;
                            m = dp->rxmbuf[dp->rx_ptr];
#ifdef GRETH_DEBUG
                            int i;
                            printf("RX: 0x%08x, Len: %d : ", (int) m->m_data, len);
                            for (i=0; i<len; i++)
                                    printf("%x%x", (m->m_data[i] >> 4) & 0x0ff, m->m_data[i] & 0x0ff);
                            printf("\n");
#endif
                            m->m_len = m->m_pkthdr.len =
                                    len - sizeof (struct ether_header);

                            eh = mtod (m, struct ether_header *);
                            m->m_data += sizeof (struct ether_header);
#ifdef CPU_U32_FIX
                            if(!(dp->gbit_mac))
                                    ipalign(m);	/* Align packet on 32-bit boundary */
#endif

                            ether_input (ifp, eh, m);
                            MGETHDR (m, M_WAIT, MT_DATA);
                            MCLGET (m, M_WAIT);
                            if (dp->gbit_mac)
                                    m->m_data += 2;
                            dp->rxmbuf[dp->rx_ptr] = m;
                            m->m_pkthdr.rcvif = ifp;
                            dp->rxdesc[dp->rx_ptr].addr =
                                    (uint32_t *) mtod (m, uint32_t *);
                            dp->rxPackets++;
                    }
                    if (dp->rx_ptr == dp->rxbufs - 1) {
                            dp->rxdesc[dp->rx_ptr].ctrl = GRETH_RXD_ENABLE | GRETH_RXD_IRQ | GRETH_RXD_WRAP;
                    } else {
                            dp->rxdesc[dp->rx_ptr].ctrl = GRETH_RXD_ENABLE | GRETH_RXD_IRQ;
                    }
                    dp->regs->ctrl |= GRETH_CTRL_RXEN;
                    dp->rx_ptr = (dp->rx_ptr + 1) % dp->rxbufs;
            }
      }

}

static int inside = 0;
static void
sendpacket (struct ifnet *ifp, struct mbuf *m)
{
    struct greth_softc *dp = ifp->if_softc;
    unsigned char *temp;
    struct mbuf *n;
    unsigned int len;

    /*printf("Send packet entered\n");*/
    if (inside) printf ("error: sendpacket re-entered!!\n");
    inside = 1;
    /*
     * Waiting for Transmitter ready
     */
    n = m;

    while (dp->txdesc[dp->tx_ptr].ctrl & GRETH_TXD_ENABLE)
      {
#ifdef GRETH_SUSPEND_NOTXBUF
        dp->txdesc[dp->tx_ptr].ctrl |= GRETH_TXD_IRQ;
        rtems_event_set events;
        rtems_bsdnet_event_receive (GRETH_TX_WAIT_EVENT,
                                    RTEMS_WAIT | RTEMS_EVENT_ANY,
                                    TOD_MILLISECONDS_TO_TICKS(500), &events);
#endif
      }

    len = 0;
    temp = (unsigned char *) dp->txdesc[dp->tx_ptr].addr;
#ifdef GRETH_DEBUG
    printf("TXD: 0x%08x : BUF: 0x%08x\n", (int) m->m_data, (int) temp);
#endif
    for (;;)
    {
#ifdef GRETH_DEBUG
            int i;
            printf("MBUF: 0x%08x : ", (int) m->m_data);
            for (i=0;i<m->m_len;i++)
                    printf("%x%x", (m->m_data[i] >> 4) & 0x0ff, m->m_data[i] & 0x0ff);
            printf("\n");
#endif
            len += m->m_len;
            if (len <= RBUF_SIZE)
                    memcpy ((void *) temp, (char *) m->m_data, m->m_len);
            temp += m->m_len;
            if ((m = m->m_next) == NULL)
                    break;
    }

    m_freem (n);

    /* don't send long packets */

    if (len <= GRETH_MAXBUF_LEN) {
            if (dp->tx_ptr < dp->txbufs-1) {
                    dp->txdesc[dp->tx_ptr].ctrl = GRETH_TXD_ENABLE | len;
            } else {
                    dp->txdesc[dp->tx_ptr].ctrl =
                            GRETH_TXD_WRAP | GRETH_TXD_ENABLE | len;
            }
            dp->regs->ctrl = dp->regs->ctrl | GRETH_CTRL_TXEN;
            dp->tx_ptr = (dp->tx_ptr + 1) % dp->txbufs;
    }
    inside = 0;
}


static void
sendpacket_gbit (struct ifnet *ifp, struct mbuf *m)
{
        struct greth_softc *dp = ifp->if_softc;
        unsigned int len;

        /*printf("Send packet entered\n");*/
        if (inside) printf ("error: sendpacket re-entered!!\n");
        inside = 1;
        /*
         * Waiting for Transmitter ready
         */

        len = 0;
#ifdef GRETH_DEBUG
        printf("TXD: 0x%08x\n", (int) m->m_data);
#endif
        for (;;)
        {
                while (dp->txdesc[dp->tx_ptr].ctrl & GRETH_TXD_ENABLE)
                {
#ifdef GRETH_SUSPEND_NOTXBUF
                        dp->txdesc[dp->tx_ptr].ctrl |= GRETH_TXD_IRQ;
                        rtems_event_set events;
                        rtems_bsdnet_event_receive (GRETH_TX_WAIT_EVENT,
                                                    RTEMS_WAIT | RTEMS_EVENT_ANY,
                                                    TOD_MILLISECONDS_TO_TICKS(500), &events);
#endif
                }
#ifdef GRETH_DEBUG
                int i;
                printf("MBUF: 0x%08x, Len: %d : ", (int) m->m_data, m->m_len);
                for (i=0; i<m->m_len; i++)
                        printf("%x%x", (m->m_data[i] >> 4) & 0x0ff, m->m_data[i] & 0x0ff);
                printf("\n");
#endif
            len += m->m_len;
            dp->txdesc[dp->tx_ptr].addr = (uint32_t *)m->m_data;
            if (dp->tx_ptr < dp->txbufs-1) {
                    if ((m->m_next) == NULL) {
                            dp->txdesc[dp->tx_ptr].ctrl = GRETH_TXD_ENABLE | GRETH_TXD_CS | m->m_len;
                            break;
                    } else {
                            dp->txdesc[dp->tx_ptr].ctrl = GRETH_TXD_ENABLE | GRETH_TXD_MORE | GRETH_TXD_CS | m->m_len;
                    }
            } else {
                    if ((m->m_next) == NULL) {
                            dp->txdesc[dp->tx_ptr].ctrl =
                                    GRETH_TXD_WRAP | GRETH_TXD_ENABLE | GRETH_TXD_CS | m->m_len;
                            break;
                    } else {
                            dp->txdesc[dp->tx_ptr].ctrl =
                                    GRETH_TXD_WRAP | GRETH_TXD_ENABLE | GRETH_TXD_MORE | GRETH_TXD_CS | m->m_len;
                    }
            }
            dp->txmbuf[dp->tx_ptr] = m;
            dp->tx_ptr = (dp->tx_ptr + 1) % dp->txbufs;
            dp->tx_cnt++;
            m = m->m_next;

    }
        dp->txmbuf[dp->tx_ptr] = m;
        dp->tx_cnt++;
        dp->regs->ctrl = dp->regs->ctrl | GRETH_CTRL_TXEN;
        dp->tx_ptr = (dp->tx_ptr + 1) % dp->txbufs;
        inside = 0;
}

/*
 * Driver transmit daemon
 */
void
greth_txDaemon (void *arg)
{
    struct greth_softc *sc = (struct greth_softc *) arg;
    struct ifnet *ifp = &sc->arpcom.ac_if;
    struct mbuf *m;
    rtems_event_set events;

    for (;;)
    {
            /*
             * Wait for packet
             */

            rtems_bsdnet_event_receive (START_TRANSMIT_EVENT,
                                        RTEMS_EVENT_ANY | RTEMS_WAIT,
                                        RTEMS_NO_TIMEOUT, &events);
#ifdef GRETH_DEBUG
            printf ("t\n");
#endif

            /*
             * Send packets till queue is empty
             */


            for (;;)
	    {
                    /*
                     * Get the next mbuf chain to transmit.
                     */
                    IF_DEQUEUE (&ifp->if_snd, m);
                    if (!m)
                            break;
                    sendpacket(ifp, m);
            }
            ifp->if_flags &= ~IFF_OACTIVE;
    }
}

/*
 * Driver transmit daemon
 */
void
greth_txDaemon_gbit (void *arg)
{
    struct greth_softc *sc = (struct greth_softc *) arg;
    struct ifnet *ifp = &sc->arpcom.ac_if;
    struct mbuf *m;
    rtems_event_set events;

    for (;;)
    {
            /*
             * Wait for packet
             */

            rtems_bsdnet_event_receive (START_TRANSMIT_EVENT,
                                        RTEMS_EVENT_ANY | RTEMS_WAIT,
                                        RTEMS_NO_TIMEOUT, &events);
#ifdef GRETH_DEBUG
            printf ("t\n");
#endif

            /*
             * Send packets till queue is empty
             */
            for (;;)
	    {
                    while((sc->tx_cnt > 0) && !(sc->txdesc[sc->tx_dptr].ctrl & GRETH_TXD_ENABLE)) {
                            m_free(sc->txmbuf[sc->tx_dptr]);
                            sc->tx_dptr = (sc->tx_dptr + 1) % sc->txbufs;
                            sc->tx_cnt--;
                    }
                    /*
                     * Get the next mbuf chain to transmit.
                     */
                    IF_DEQUEUE (&ifp->if_snd, m);
                    if (!m)
                            break;
                    sendpacket_gbit(ifp, m);
            }
            ifp->if_flags &= ~IFF_OACTIVE;
    }
}


static void
greth_start (struct ifnet *ifp)
{
    struct greth_softc *sc = ifp->if_softc;

    ifp->if_flags |= IFF_OACTIVE;
    rtems_event_send (sc->txDaemonTid, START_TRANSMIT_EVENT);

}

/*
 * Initialize and start the device
 */
static void
greth_init (void *arg)
{
    struct greth_softc *sc = arg;
    struct ifnet *ifp = &sc->arpcom.ac_if;

    if (sc->txDaemonTid == 0)
      {

	  /*
	   * Set up GRETH hardware
	   */
          greth_initialize_hardware (sc);

	  /*
	   * Start driver tasks
	   */
	  sc->rxDaemonTid = rtems_bsdnet_newproc ("DCrx", 4096,
						  greth_rxDaemon, sc);
          if (sc->gbit_mac) {
                  sc->txDaemonTid = rtems_bsdnet_newproc ("DCtx", 4096,
                                                          greth_txDaemon_gbit, sc);
          } else {
                  sc->txDaemonTid = rtems_bsdnet_newproc ("DCtx", 4096,
                                                          greth_txDaemon, sc);
          }

      }

    /*
     * Tell the world that we're running.
     */
    ifp->if_flags |= IFF_RUNNING;

}

/*
 * Stop the device
 */
static void
greth_stop (struct greth_softc *sc)
{
    struct ifnet *ifp = &sc->arpcom.ac_if;

    ifp->if_flags &= ~IFF_RUNNING;

    sc->regs->ctrl = 0;		        /* RX/TX OFF */
    sc->regs->ctrl = GRETH_CTRL_RST;	/* Reset ON */
    sc->regs->ctrl = 0;	         	/* Reset OFF */
}


/*
 * Show interface statistics
 */
static void
greth_stats (struct greth_softc *sc)
{
  printf ("      Rx Interrupts:%-8lu", sc->rxInterrupts);
  printf ("      Rx Packets:%-8lu", sc->rxPackets);
  printf ("          Length:%-8lu", sc->rxLengthError);
  printf ("       Non-octet:%-8lu\n", sc->rxNonOctet);
  printf ("            Bad CRC:%-8lu", sc->rxBadCRC);
  printf ("         Overrun:%-8lu", sc->rxOverrun);
  printf ("      Tx Interrupts:%-8lu", sc->txInterrupts);
}

/*
 * Driver ioctl handler
 */
static int
greth_ioctl (struct ifnet *ifp, ioctl_command_t command, caddr_t data)
{
    struct greth_softc *sc = ifp->if_softc;
    int error = 0;

    switch (command)
      {
      case SIOCGIFADDR:
      case SIOCSIFADDR:
	  ether_ioctl (ifp, command, data);
	  break;

      case SIOCSIFFLAGS:
	  switch (ifp->if_flags & (IFF_UP | IFF_RUNNING))
	    {
	    case IFF_RUNNING:
		greth_stop (sc);
                break;

	    case IFF_UP:
		greth_init (sc);
		break;

	    case IFF_UP | IFF_RUNNING:
		greth_stop (sc);
		greth_init (sc);
		break;
       default:
		break;
	    }
	  break;

      case SIO_RTEMS_SHOW_STATS:
	  greth_stats (sc);
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
 * Attach an GRETH driver to the system
 */
int
rtems_greth_driver_attach (struct rtems_bsdnet_ifconfig *config,
			   greth_configuration_t *chip)
{
    struct greth_softc *sc;
    struct ifnet *ifp;
    int mtu;
    int unitNumber;
    char *unitName;

      /* parse driver name */
    if ((unitNumber = rtems_bsdnet_parse_driver_name (config, &unitName)) < 0)
	return 0;

    sc = &greth;
    ifp = &sc->arpcom.ac_if;
    memset (sc, 0, sizeof (*sc));

    if (config->hardware_address)
      {
	  memcpy (sc->arpcom.ac_enaddr, config->hardware_address,
		  ETHER_ADDR_LEN);
      }
    else
      {
	  memset (sc->arpcom.ac_enaddr, 0x08, ETHER_ADDR_LEN);
      }

    if (config->mtu)
	mtu = config->mtu;
    else
	mtu = ETHERMTU;

    sc->acceptBroadcast = !config->ignore_broadcast;
    sc->regs = (void *) chip->base_address;
    sc->vector = chip->vector;
    sc->txbufs = chip->txd_count;
    sc->rxbufs = chip->rxd_count;

    /*
     * Set up network interface values
     */
    ifp->if_softc = sc;
    ifp->if_unit = unitNumber;
    ifp->if_name = unitName;
    ifp->if_mtu = mtu;
    ifp->if_init = greth_init;
    ifp->if_ioctl = greth_ioctl;
    ifp->if_start = greth_start;
    ifp->if_output = ether_output;
    ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX;
    if (ifp->if_snd.ifq_maxlen == 0)
	ifp->if_snd.ifq_maxlen = ifqmaxlen;

    /*
     * Attach the interface
     */
    if_attach (ifp);
    ether_ifattach (ifp);

#ifdef GRETH_DEBUG
    printf ("GRETH : driver has been attached\n");
#endif
    return 1;
};


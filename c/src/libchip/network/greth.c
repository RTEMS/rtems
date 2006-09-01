/*
 *  RTEMS driver for Opencores Ethernet Controller
 *
 *  Weakly based on dec21140 rtems driver and open_eth linux driver
 *  Written by Jiri Gaisler, Gaisler Research
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems.h>

#define GRETH_SUPPORTED
#include <bsp.h>

#include <inttypes.h>
#include <errno.h>
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

extern rtems_isr_entry set_vector( rtems_isr_entry, rtems_vector_number, int );

/*
#define GRETH_DEBUG
*/

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
   unsigned int rx_ptr;
   unsigned int txbufs;
   unsigned int rxbufs;
   greth_rxtxdesc *txdesc;
   greth_rxtxdesc *rxdesc;
   struct mbuf **rxmbuf;
   rtems_vector_number vector;
    
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
        tmp = (char *) (((int)tmp+sz) & ~(sz -1));
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

static uint32_t read_mii(uint32_t addr)
{
    while (greth.regs->mdio_ctrl & GRETH_MDIO_BUSY) {}
    greth.regs->mdio_ctrl = addr << 6 | GRETH_MDIO_READ;
    while (greth.regs->mdio_ctrl & GRETH_MDIO_BUSY) {}
    if (!(greth.regs->mdio_ctrl & GRETH_MDIO_LINKFAIL))
        return((greth.regs->mdio_ctrl >> 16) & 0xFFFF);
    else {
	printf("greth: failed to read mii\n");
	return (0);
    }
}

static void write_mii(uint32_t addr, uint32_t data)
{
    while (greth.regs->mdio_ctrl & GRETH_MDIO_BUSY) {}
    greth.regs->mdio_ctrl = 
    ((data & 0xFFFF) << 16) | (addr << 8) | GRETH_MDIO_WRITE;
    while (greth.regs->mdio_ctrl & GRETH_MDIO_BUSY) {}
}
/*
 * Initialize the ethernet hardware
 */
static void
greth_initialize_hardware (struct greth_softc *sc)
{
    struct mbuf *m;
    int i;
    int fd;
    
    greth_regs *regs;

    regs = sc->regs;

    /* Reset the controller.  */
    greth.rxInterrupts = 0;
    greth.rxPackets = 0;

    regs->ctrl = 0;
    regs->ctrl = GRETH_CTRL_RST;	/* Reset ON */
    regs->ctrl = 0;			/* Reset OFF */
    
    /* reset PHY and wait for complettion */
    /*
    */
    write_mii(0, 0x8000);
    while (read_mii(0) & 0x8000) {}
    fd = regs->mdio_ctrl >> 24; /*duplex mode*/
    printf(
      "greth: driver attached, PHY config: 0x%04" PRIx32 "\n", read_mii(0));

    /* Initialize rx/tx descriptor pointers */
    sc->txdesc = (greth_rxtxdesc *) almalloc(1024);
    sc->rxdesc = (greth_rxtxdesc *) almalloc(1024);
    sc->tx_ptr = 0;
    sc->rx_ptr = 0;
    regs->txdesc = (int) sc->txdesc;
    regs->rxdesc = (int) sc->rxdesc;
    
    sc->rxmbuf = calloc(sc->rxbufs, sizeof(*sc->rxmbuf));

    for (i = 0; i < sc->txbufs; i++)
      {
          sc->txdesc[i].addr = (uint32_t *) calloc(1, GRETH_MAXBUF_LEN);
#ifdef GRETH_DEBUG
	  printf("TXBUF: %08x\n", (int) sc->txdesc[i].addr);
#endif
      }
    /*printf("RXbufs: %i\n", sc->rxbufs);*/
    for (i = 0; i < sc->rxbufs; i++)
      {

          MGETHDR (m, M_WAIT, MT_DATA);
          MCLGET (m, M_WAIT);
	  m->m_pkthdr.rcvif = &sc->arpcom.ac_if;
          sc->rxmbuf[i] = m;
	  sc->rxdesc[i].addr = (uint32_t *) mtod(m, uint32_t *);
          sc->rxdesc[i].ctrl = GRETH_RXD_ENABLE | GRETH_RXD_IRQ;
#ifdef GRETH_DEBUG
	  printf("RXBUF: %08x\n", (int) sc->rxdesc[i].addr);
#endif
      }
    sc->rxdesc[sc->rxbufs - 1].ctrl |= GRETH_RXD_WRAP;

    /* set ethernet address.  */
    regs->mac_addr_msb = 
      sc->arpcom.ac_enaddr[0] << 8 | sc->arpcom.ac_enaddr[1];
    regs->mac_addr_lsb = 
      sc->arpcom.ac_enaddr[2] << 24 | sc->arpcom.ac_enaddr[3] << 16 |
      sc->arpcom.ac_enaddr[4] << 8 | sc->arpcom.ac_enaddr[5];

    /* install interrupt vector */
    set_vector(greth_interrupt_handler, sc->vector, 1);

    /* clear all pending interrupts */

    regs->status = 0xffffffff;

#ifdef GRETH_SUSPEND_NOTXBUF
    regs->ctrl |= GRETH_CTRL_TXIRQ;
#endif

    regs->ctrl |= GRETH_CTRL_RXEN | (fd << 4) | GRETH_CTRL_RXIRQ;
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
    
    /*printf("Started RxDaemon\n");*/
    for (;;)
      {
        rtems_bsdnet_event_receive (INTERRUPT_EVENT,
                                    RTEMS_WAIT | RTEMS_EVENT_ANY,
                                    RTEMS_NO_TIMEOUT, &events);
        
#ifdef GRETH_ETH_DEBUG
    printf ("r\n");
#endif
    /*printf("Packet received\n");*/
            while (!((len_status =
		   dp->rxdesc[dp->rx_ptr].ctrl) & GRETH_RXD_ENABLE))
	    {
              /*printf("Status: %x\n", dp->rxdesc[dp->rx_ptr].ctrl);*/
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
                if (!bad)
		  {
                    /*printf("Received Ok packet\n");*/
		      /* pass on the packet in the receive buffer */
                    len = len_status & 0x7FF;
                    m = dp->rxmbuf[dp->rx_ptr];
                    m->m_len = m->m_pkthdr.len =
                      len - sizeof (struct ether_header);
                    /*printf("Packet of length: %i\n", len);*/
                    eh = mtod (m, struct ether_header *);
                    m->m_data += sizeof (struct ether_header);
                    /*printf("Mbuf handling done\n");*/
#ifdef CPU_U32_FIX
                    /*printf("Ip aligning\n");*/
                    ipalign(m);	/* Align packet on 32-bit boundary */
#endif
                    /*printf("Calling stack\n");*/
                    /*printf("Ifp: %x, Eh: %x, M: %x\n", (int)ifp, (int)eh, (int)m);*/
                    ether_input (ifp, eh, m);
                    /*printf("Returned from stack\n");*/
                    /* get a new mbuf */
                    /*printf("Getting new mbuf\n");*/
                    MGETHDR (m, M_WAIT, MT_DATA);
                    MCLGET (m, M_WAIT);
                    /*printf("Got new mbuf\n");*/
                    dp->rxmbuf[dp->rx_ptr] = m;
                    m->m_pkthdr.rcvif = ifp;
                    dp->rxdesc[dp->rx_ptr].addr =
                      (uint32_t *) mtod (m, uint32_t *);
                    dp->rxPackets++;
                  }
                /*printf("Reenabling desc\n");*/
                dp->rxdesc[dp->rx_ptr].ctrl = GRETH_RXD_ENABLE | GRETH_RXD_IRQ;
                if (dp->rx_ptr == dp->rxbufs - 1) {
                  dp->rxdesc[dp->rx_ptr].ctrl |= GRETH_RXD_WRAP;
                }
                dp->regs->ctrl |= GRETH_CTRL_RXEN;
                /*printf("rxptr: %i\n", dp->rx_ptr);*/
                dp->rx_ptr = (dp->rx_ptr + 1) % dp->rxbufs;
                /*printf("RxDesc reenabled\n");*/
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
    printf("TXD: 0x%08x\n", (int) m->m_data);
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
                
		sendpacket (ifp, m);
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
	  sc->txDaemonTid = rtems_bsdnet_newproc ("DCtx", 4096,
						  greth_txDaemon, sc);
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


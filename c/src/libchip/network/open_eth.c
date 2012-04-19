/*
 *  RTEMS driver for Opencores Ethernet Controller
 *
 *  Weakly based on dec21140 rtems driver and open_eth linux driver
 *  Written by Jiri Gaisler, Gaisler Research
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

/*
 *  This driver current only supports architectures with the old style
 *  exception processing.  The following checks try to keep this
 *  from being compiled on systems which can't support this driver.
 *
 *  NOTE: The i386, ARM, and PowerPC use a different interrupt API than
 *        that used by this driver.
 */

#if defined(__i386__) || defined(__arm__) || defined(__PPC__)
  #define OPENETH_NOT_SUPPORTED
#endif

#if !defined(OPENETH_NOT_SUPPORTED)
#include <bsp.h>
#include <rtems.h>

#include <bsp.h>

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>
#include <libchip/open_eth.h>

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
#define OPEN_ETH_DEBUG
 */

#ifdef CPU_U32_FIX
extern void ipalign(struct mbuf *m);
#endif

/* message descriptor entry */
struct MDTX
{
    char  *buf;
};

struct MDRX
{
    struct mbuf *m;
};

/*
 * Number of OCs supported by this driver
 */
#define NOCDRIVER	1

/*
 * Receive buffer size -- Allow for a full ethernet packet including CRC
 */
#define RBUF_SIZE	1536

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
#define OPEN_ETH_TX_WAIT_EVENT  RTEMS_EVENT_3

 /* suspend when all TX descriptors exhausted */
 /*
#define OETH_SUSPEND_NOTXBUF
 */

#if (MCLBYTES < RBUF_SIZE)
# error "Driver must have MCLBYTES > RBUF_SIZE"
#endif

/*
 * Per-device data
 */
struct open_eth_softc
{

    struct arpcom arpcom;

    oeth_regs *regs;

    int acceptBroadcast;
    rtems_id rxDaemonTid;
    rtems_id txDaemonTid;

    unsigned int tx_ptr;
    unsigned int rx_ptr;
    unsigned int txbufs;
    unsigned int rxbufs;
    struct MDTX *txdesc;
    struct MDRX *rxdesc;
    rtems_vector_number vector;
    unsigned int en100MHz;

    /*
     * Statistics
     */
    unsigned long rxInterrupts;
    unsigned long rxPackets;
    unsigned long rxLengthError;
    unsigned long rxNonOctet;
    unsigned long rxBadCRC;
    unsigned long rxOverrun;
    unsigned long rxMiss;
    unsigned long rxCollision;

    unsigned long txInterrupts;
    unsigned long txDeferred;
    unsigned long txHeartbeat;
    unsigned long txLateCollision;
    unsigned long txRetryLimit;
    unsigned long txUnderrun;
    unsigned long txLostCarrier;
    unsigned long txRawWait;
};

static struct open_eth_softc oc;

/* OPEN_ETH interrupt handler */

static rtems_isr
open_eth_interrupt_handler (rtems_vector_number v)
{
    uint32_t status;

    /* read and clear interrupt cause */

    status = oc.regs->int_src;
    oc.regs->int_src = status;

    /* Frame received? */

    if (status & (OETH_INT_RXF | OETH_INT_RXE))
      {
	  oc.rxInterrupts++;
	  rtems_event_send (oc.rxDaemonTid, INTERRUPT_EVENT);
      }
#ifdef OETH_SUSPEND_NOTXBUF
    if (status & (OETH_INT_MASK_TXB | OETH_INT_MASK_TXC | OETH_INT_MASK_TXE))
      {
	  oc.txInterrupts++;
	  rtems_event_send (oc.txDaemonTid, OPEN_ETH_TX_WAIT_EVENT);
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
    while (oc.regs->miistatus & OETH_MIISTATUS_BUSY) {}
    oc.regs->miiaddress = addr << 8;
    oc.regs->miicommand = OETH_MIICOMMAND_RSTAT;
    while (oc.regs->miistatus & OETH_MIISTATUS_BUSY) {}
    if (!(oc.regs->miistatus & OETH_MIISTATUS_NVALID))
        return(oc.regs->miirx_data);
    else {
	printf("open_eth: failed to read mii\n");
	return (0);
    }
}

static void write_mii(uint32_t addr, uint32_t data)
{
    while (oc.regs->miistatus & OETH_MIISTATUS_BUSY) {}
    oc.regs->miiaddress = addr << 8;
    oc.regs->miitx_data = data;
    oc.regs->miicommand = OETH_MIICOMMAND_WCTRLDATA;
    while (oc.regs->miistatus & OETH_MIISTATUS_BUSY) {}
}
/*
 * Initialize the ethernet hardware
 */
static void
open_eth_initialize_hardware (struct open_eth_softc *sc)
{
    struct mbuf *m;
    int i;
    int mii_cr = 0;

    oeth_regs *regs;

    regs = sc->regs;

    /* Reset the controller.  */

    regs->ctrlmoder = 0;
    regs->moder = OETH_MODER_RST;	/* Reset ON */
    regs->moder = 0;			/* Reset OFF */

    /* reset PHY and wait for complettion */
    mii_cr = 0x3300;
    if (!sc->en100MHz) mii_cr = 0;
    write_mii(0, mii_cr | 0x8000);
    while (read_mii(0) & 0x8000) {}
    if (!sc->en100MHz) write_mii(0, 0);
    mii_cr = read_mii(0);
    printf("open_eth: driver attached, PHY config : 0x%04" PRIx32 "\n", read_mii(0));

#ifdef OPEN_ETH_DEBUG
    printf("mii_cr: %04x\n", mii_cr);
    for (i=0;i<21;i++)
      printf("mii_reg %2d : 0x%04x\n", i, read_mii(i));
#endif

    /* Setting TXBD base to sc->txbufs  */

    regs->tx_bd_num = sc->txbufs;

    /* Initialize rx/tx pointers.  */

    sc->rx_ptr = 0;
    sc->tx_ptr = 0;

    /* Set min/max packet length */
    regs->packet_len = 0x00400600;

    /* Set IPGT register to recomended value */
    regs->ipgt = 0x00000015;

    /* Set IPGR1 register to recomended value */
    regs->ipgr1 = 0x0000000c;

    /* Set IPGR2 register to recomended value */
    regs->ipgr2 = 0x00000012;

    /* Set COLLCONF register to recomended value */
    regs->collconf = 0x000f003f;

    /* initialize TX descriptors */

    sc->txdesc = calloc(sc->txbufs, sizeof(*sc->txdesc));
    for (i = 0; i < sc->txbufs; i++)
      {
	  sc->regs->xd[i].len_status = OETH_TX_BD_PAD | OETH_TX_BD_CRC;
	  sc->txdesc[i].buf = calloc(1, OETH_MAXBUF_LEN);
#ifdef OPEN_ETH_DEBUG
	  printf("TXBUF: %08x\n", (int) sc->txdesc[i].buf);
#endif
      }
    sc->regs->xd[sc->txbufs - 1].len_status |= OETH_TX_BD_WRAP;

    /* allocate RX buffers */

    sc->rxdesc = calloc(sc->rxbufs, sizeof(*sc->rxdesc));
    for (i = 0; i < sc->rxbufs; i++)
      {

          MGETHDR (m, M_WAIT, MT_DATA);
          MCLGET (m, M_WAIT);
	  m->m_pkthdr.rcvif = &sc->arpcom.ac_if;
	  sc->rxdesc[i].m = m;
	  sc->regs->xd[i + sc->txbufs].addr = mtod (m, uint32_t*);
	  sc->regs->xd[i + sc->txbufs].len_status =
	      OETH_RX_BD_EMPTY | OETH_RX_BD_IRQ;
#ifdef OPEN_ETH_DEBUG
	  printf("RXBUF: %08x\n", (int) sc->rxdesc[i].m);
#endif
      }
    sc->regs->xd[sc->rxbufs + sc->txbufs - 1].len_status |= OETH_RX_BD_WRAP;


    /* set ethernet address.  */

    regs->mac_addr1 = sc->arpcom.ac_enaddr[0] << 8 | sc->arpcom.ac_enaddr[1];

    uint32_t mac_addr0;
    mac_addr0 = sc->arpcom.ac_enaddr[2];
    mac_addr0 <<= 8;
    mac_addr0 |= sc->arpcom.ac_enaddr[3];
    mac_addr0 <<= 8;
    mac_addr0 |= sc->arpcom.ac_enaddr[4];
    mac_addr0 <<= 8;
    mac_addr0 |= sc->arpcom.ac_enaddr[5];
    
    regs->mac_addr0 = mac_addr0;

    /* install interrupt vector */
    set_vector (open_eth_interrupt_handler, sc->vector, 1);

    /* clear all pending interrupts */

    regs->int_src = 0xffffffff;

    /* MAC mode register: PAD, IFG, CRCEN */

    regs->moder = OETH_MODER_PAD | OETH_MODER_CRCEN | ((mii_cr & 0x100) << 2);

    /* enable interrupts */

    regs->int_mask = OETH_INT_MASK_RXF | OETH_INT_MASK_RXE | OETH_INT_MASK_RXC;

#ifdef OETH_SUSPEND_NOTXBUF
    regs->int_mask |= OETH_INT_MASK_TXB | OETH_INT_MASK_TXC | OETH_INT_MASK_TXE | OETH_INT_BUSY;*/
    sc->regs->xd[(sc->txbufs - 1)/2].len_status |= OETH_TX_BD_IRQ;
    sc->regs->xd[sc->txbufs - 1].len_status |= OETH_TX_BD_IRQ;
#endif

    regs->moder |= OETH_MODER_RXEN | OETH_MODER_TXEN;
}

static void
open_eth_rxDaemon (void *arg)
{
    struct ether_header *eh;
    struct open_eth_softc *dp = (struct open_eth_softc *) &oc;
    struct ifnet *ifp = &dp->arpcom.ac_if;
    struct mbuf *m;
    unsigned int len;
    uint32_t len_status;
    unsigned int bad;
    rtems_event_set events;


    for (;;)
      {

	  rtems_bsdnet_event_receive (INTERRUPT_EVENT,
				      RTEMS_WAIT | RTEMS_EVENT_ANY,
				      RTEMS_NO_TIMEOUT, &events);
#ifdef OPEN_ETH_DEBUG
    printf ("r\n");
#endif

	  while (!
		 ((len_status =
		   dp->regs->xd[dp->rx_ptr+dp->txbufs].len_status) & OETH_RX_BD_EMPTY))
	    {
		bad = 0;
		if (len_status & (OETH_RX_BD_TOOLONG | OETH_RX_BD_SHORT))
		  {
		      dp->rxLengthError++;
		      bad = 1;
		  }
		if (len_status & OETH_RX_BD_DRIBBLE)
		  {
		      dp->rxNonOctet++;
		      bad = 1;
		  }
		if (len_status & OETH_RX_BD_CRCERR)
		  {
		      dp->rxBadCRC++;
		      bad = 1;
		  }
		if (len_status & OETH_RX_BD_OVERRUN)
		  {
		      dp->rxOverrun++;
		      bad = 1;
		  }
		if (len_status & OETH_RX_BD_MISS)
		  {
		      dp->rxMiss++;
		      bad = 1;
		  }
		if (len_status & OETH_RX_BD_LATECOL)
		  {
		      dp->rxCollision++;
		      bad = 1;
		  }

		if (!bad)
		  {
		      /* pass on the packet in the receive buffer */
		      len = len_status >> 16;
		      m = (struct mbuf *) (dp->rxdesc[dp->rx_ptr].m);
		      m->m_len = m->m_pkthdr.len =
			  len - sizeof (struct ether_header);
		      eh = mtod (m, struct ether_header *);
		      m->m_data += sizeof (struct ether_header);
#ifdef CPU_U32_FIX
	              ipalign(m);	/* Align packet on 32-bit boundary */
#endif

		      ether_input (ifp, eh, m);

		      /* get a new mbuf */
		      MGETHDR (m, M_WAIT, MT_DATA);
		      MCLGET (m, M_WAIT);
		      m->m_pkthdr.rcvif = ifp;
		      dp->rxdesc[dp->rx_ptr].m = m;
		      dp->regs->xd[dp->rx_ptr + dp->txbufs].addr =
			  (uint32_t*) mtod (m, void *);
		      dp->rxPackets++;
		  }

		dp->regs->xd[dp->rx_ptr+dp->txbufs].len_status =
		  (dp->regs->xd[dp->rx_ptr+dp->txbufs].len_status &
		    ~OETH_TX_BD_STATS) | OETH_TX_BD_READY;
		dp->rx_ptr = (dp->rx_ptr + 1) % dp->rxbufs;
	    }
      }
}

static int inside = 0;
static void
sendpacket (struct ifnet *ifp, struct mbuf *m)
{
    struct open_eth_softc *dp = ifp->if_softc;
    unsigned char *temp;
    struct mbuf *n;
    uint32_t len, len_status;

    if (inside) printf ("error: sendpacket re-entered!!\n");
    inside = 1;
    /*
     * Waiting for Transmitter ready
     */
    n = m;

    while (dp->regs->xd[dp->tx_ptr].len_status & OETH_TX_BD_READY)
      {
#ifdef OETH_SUSPEND_NOTXBUF
          rtems_event_set events;
	  rtems_bsdnet_event_receive (OPEN_ETH_TX_WAIT_EVENT,
				      RTEMS_WAIT | RTEMS_EVENT_ANY,
				      TOD_MILLISECONDS_TO_TICKS(500), &events);
#endif
      }

    len = 0;
    temp = (unsigned char *) dp->txdesc[dp->tx_ptr].buf;
    dp->regs->xd[dp->tx_ptr].addr = (uint32_t*) temp;

#ifdef OPEN_ETH_DEBUG
    printf("TXD: 0x%08x\n", (int) m->m_data);
#endif
    for (;;)
        {
#ifdef OPEN_ETH_DEBUG
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

    if (len <= RBUF_SIZE) {

     /* Clear all of the status flags.  */
     len_status = dp->regs->xd[dp->tx_ptr].len_status & ~OETH_TX_BD_STATS;

     /* If the frame is short, tell CPM to pad it.  */
     if (len < ET_MINLEN) {
	len_status |= OETH_TX_BD_PAD;
	len = ET_MINLEN;
     }
     else
	len_status &= ~OETH_TX_BD_PAD;

      /* write buffer descriptor length and status */
      len_status &= 0x0000ffff;
      len_status |= (len << 16) | (OETH_TX_BD_READY | OETH_TX_BD_CRC);
      dp->regs->xd[dp->tx_ptr].len_status = len_status;
      dp->tx_ptr = (dp->tx_ptr + 1) % dp->txbufs;

    }
    inside = 0;
}

/*
 * Driver transmit daemon
 */
void
open_eth_txDaemon (void *arg)
{
    struct open_eth_softc *sc = (struct open_eth_softc *) arg;
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
#ifdef OPEN_ETH_DEBUG
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
open_eth_start (struct ifnet *ifp)
{
    struct open_eth_softc *sc = ifp->if_softc;

    rtems_event_send (sc->txDaemonTid, START_TRANSMIT_EVENT);
    ifp->if_flags |= IFF_OACTIVE;
}

/*
 * Initialize and start the device
 */
static void
open_eth_init (void *arg)
{
    struct open_eth_softc *sc = arg;
    struct ifnet *ifp = &sc->arpcom.ac_if;

    if (sc->txDaemonTid == 0)
      {

	  /*
	   * Set up OPEN_ETH hardware
	   */
	  open_eth_initialize_hardware (sc);

	  /*
	   * Start driver tasks
	   */
	  sc->rxDaemonTid = rtems_bsdnet_newproc ("DCrx", 4096,
						  open_eth_rxDaemon, sc);
	  sc->txDaemonTid = rtems_bsdnet_newproc ("DCtx", 4096,
						  open_eth_txDaemon, sc);
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
open_eth_stop (struct open_eth_softc *sc)
{
    struct ifnet *ifp = &sc->arpcom.ac_if;

    ifp->if_flags &= ~IFF_RUNNING;

    sc->regs->moder = 0;		/* RX/TX OFF */
    sc->regs->moder = OETH_MODER_RST;	/* Reset ON */
    sc->regs->moder = 0;		/* Reset OFF */
}


/*
 * Show interface statistics
 */
static void
open_eth_stats (struct open_eth_softc *sc)
{
    printf ("         Rx Packets:%-8lu", sc->rxPackets);
    printf ("      Rx Interrupts:%-8lu", sc->rxInterrupts);
    printf ("          Length:%-8lu", sc->rxLengthError);
    printf ("       Non-octet:%-8lu\n", sc->rxNonOctet);
    printf ("            Bad CRC:%-8lu", sc->rxBadCRC);
    printf ("         Overrun:%-8lu", sc->rxOverrun);
    printf ("            Miss:%-8lu", sc->rxMiss);
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

/*
 * Driver ioctl handler
 */
static int
open_eth_ioctl (struct ifnet *ifp, ioctl_command_t command, caddr_t data)
{
    struct open_eth_softc *sc = ifp->if_softc;
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
		open_eth_stop (sc);
		break;

	    case IFF_UP:
		open_eth_init (sc);
		break;

	    case IFF_UP | IFF_RUNNING:
		open_eth_stop (sc);
		open_eth_init (sc);
		break;

	    default:
		break;
	    }
	  break;

      case SIO_RTEMS_SHOW_STATS:
	  open_eth_stats (sc);
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
 * Attach an OPEN_ETH driver to the system
 */
int
rtems_open_eth_driver_attach (struct rtems_bsdnet_ifconfig *config,
			      open_eth_configuration_t * chip)
{
    struct open_eth_softc *sc;
    struct ifnet *ifp;
    int mtu;
    int unitNumber;
    char *unitName;

      /* parse driver name */
    if ((unitNumber = rtems_bsdnet_parse_driver_name (config, &unitName)) < 0)
	return 0;

    sc = &oc;
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
    sc->regs = chip->base_address;
    sc->vector = chip->vector;
    sc->txbufs = chip->txd_count;
    sc->rxbufs = chip->rxd_count;
    sc->en100MHz = chip->en100MHz;


    /*
     * Set up network interface values
     */
    ifp->if_softc = sc;
    ifp->if_unit = unitNumber;
    ifp->if_name = unitName;
    ifp->if_mtu = mtu;
    ifp->if_init = open_eth_init;
    ifp->if_ioctl = open_eth_ioctl;
    ifp->if_start = open_eth_start;
    ifp->if_output = ether_output;
    ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX;
    if (ifp->if_snd.ifq_maxlen == 0)
	ifp->if_snd.ifq_maxlen = ifqmaxlen;

    /*
     * Attach the interface
     */
    if_attach (ifp);
    ether_ifattach (ifp);

#ifdef OPEN_ETH_DEBUG
    printf ("OPEN_ETH : driver has been attached\n");
#endif
    return 1;
};

#endif  /* OPENETH_NOT_SUPPORTED */

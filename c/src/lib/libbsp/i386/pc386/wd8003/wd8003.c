/*
 * RTEMS driver for WD800x
 *
 *  Based on the 68360 Network Driver by:
 *    W. Eric Norum
 *    Saskatchewan Accelerator Laboratory
 *    University of Saskatchewan
 *    Saskatoon, Saskatchewan, CANADA
 *    eric@skatter.usask.ca
 */

#include <bsp.h>
#include <wd80x3.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>	/* memcpy, memset */
#include <errno.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>

#include <net/if.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>

#include <bsp/irq.h>

#define	ET_MINLEN 60		/* minimum message length */

/*
 * Number of WDs supported by this driver
 */
#define NWDDRIVER	1

/*
 * Default number of buffer descriptors set aside for this driver.
 * The number of transmit buffer descriptors has to be quite large
 * since a single frame often uses four or more buffer descriptors.
 */
#define RX_BUF_COUNT     15
#define TX_BUF_COUNT     4
#define TX_BD_PER_BUF    4

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

/*
 * Receive buffer size -- Allow for a full ethernet packet including CRC
 */
#define RBUF_SIZE	1520

#if (MCLBYTES < RBUF_SIZE)
# error "Driver must have MCLBYTES > RBUF_SIZE"
#endif

/*
 * Per-device data
 */
struct wd_softc {
  struct arpcom			arpcom;
  rtems_irq_connect_data	irqInfo;
  struct mbuf			**rxMbuf;
  struct mbuf			**txMbuf;
  int				acceptBroadcast;
  int				rxBdCount;
  int				txBdCount;
  int				txBdHead;
  int				txBdTail;
  int				txBdActiveCount;
  rtems_id			rxDaemonTid;
  rtems_id			txDaemonTid;

  unsigned int 			port;
  unsigned char			*base;
  unsigned long			bpar;

  /*
   * Statistics
   */
  unsigned long	rxInterrupts;
  unsigned long	rxNotFirst;
  unsigned long	rxNotLast;
  unsigned long	rxGiant;
  unsigned long	rxNonOctet;
  unsigned long	rxRunt;
  unsigned long	rxBadCRC;
  unsigned long	rxOverrun;
  unsigned long	rxCollision;

  unsigned long	txInterrupts;
  unsigned long	txDeferred;
  unsigned long	txHeartbeat;
  unsigned long	txLateCollision;
  unsigned long	txRetryLimit;
  unsigned long	txUnderrun;
  unsigned long	txLostCarrier;
  unsigned long	txRawWait;
};

#define RO 0x10

#define SHATOT (8*1024)		/* size of shared memory */
#define SHAPAGE 256		/* shared memory information */
#define MAXSIZ 	1536		/*(MAXBUF - MESSH_SZ)*/
#define OUTPAGE ((SHATOT-(MAXSIZ+SHAPAGE-1))/SHAPAGE)

static volatile unsigned long overrun;
static volatile unsigned long resend;
static struct wd_softc wd_softc[NWDDRIVER];

/*
 * WD interrupt handler
 */
static void
wd8003Enet_interrupt_handler (void *unused)
{
  unsigned int tport;
  unsigned char status, status2;

  tport = wd_softc[0].port ;

  /*
   * Read status
   */
  inport_byte(tport+ISR, status);
  outport_byte(tport+IMR, 0x00);

  /*
   * Ring overwrite
   */

  if (status & MSK_OVW){
    outport_byte(tport+CMDR, MSK_STP + MSK_RD2);	/* stop 8390 */
    Wait_X_ms(2);
    outport_byte(tport+RBCR0, 0);			/* clear byte count */
    outport_byte(tport+RBCR1, 0);
    inport_byte(tport+ISR, status2);
    status |= (status2 & (MSK_PTX+MSK_TXE)) ;	/* TX status */
    outport_byte(tport+TCR, MSK_LOOP);		/* loopback mode */
    outport_byte(tport+CMDR, MSK_STA + MSK_RD2);	/* start */
    overrun = 1 ;
    if ((status & (MSK_PTX+MSK_TXE)) == 0)
	resend = 1;
  }

  /*
   * Frame received?
   */
  if (status & (MSK_PRX+MSK_RXE)) {
    outport_byte(tport+ISR, status & (MSK_PRX+MSK_RXE));
    wd_softc[0].rxInterrupts++;
    rtems_event_send (wd_softc[0].rxDaemonTid, INTERRUPT_EVENT);
  }

}

static void nopOn(const rtems_irq_connect_data* notUsed)
{
  /*
   * code should be moved from wd8003Enet_initialize_hardware
   * to this location
   */
}

static int wdIsOn(const rtems_irq_connect_data* irq)
{
  return BSP_irq_enabled_at_i8259s (irq->name);
}

/*
 * Initialize the ethernet hardware
 */
static void
wd8003Enet_initialize_hardware (struct wd_softc *sc)
{
  int  i1, ultra;
  char cc1, cc2;
  unsigned char  temp;
  rtems_status_code st;
  unsigned int tport;
  unsigned char *hwaddr;

  tport = sc->port;

  /* address from board ROM */
  inport_byte(tport+0x04, temp);
  outport_byte(tport+0x04, temp & 0x7f);

  hwaddr = sc->arpcom.ac_enaddr;
  for (i1=cc2=0; i1<8; i1++) {
    inport_byte(tport + ADDROM + i1, cc1);
    cc2 += cc1;
    if (i1 < 6)
      hwaddr[i1] = cc1;
  }

  inport_byte(tport+0x04, temp);
  outport_byte(tport+0x04, temp | 0x80);	/* alternate registers */
  outport_byte(tport+W83CREG, MSK_RESET);	/* reset board, set buffer */
  outport_byte(tport+W83CREG, 0);
  outport_byte(tport+W83CREG, MSK_ENASH + (int)((sc->bpar>>13)&0x3f));

  outport_byte(tport+CMDR, MSK_PG0 + MSK_RD2);
  cc1 = MSK_BMS + MSK_FT10; /* configure 8 or 16 bits */

  inport_byte(tport+0x07, temp) ;

  ultra = ((temp & 0xf0) == 0x20 || (temp & 0xf0) == 0x40);
  if (ultra)
    cc1 = MSK_WTS + MSK_BMS + MSK_FT10;
  outport_byte(tport+DCR, cc1);
  outport_byte(tport+RBCR0, 0);
  outport_byte(tport+RBCR1, 0);
  outport_byte(tport+RCR, MSK_MON);	       	/* disable the rxer */
  outport_byte(tport+TCR, 0);			/* normal operation */
  outport_byte(tport+PSTOP, OUTPAGE);		/* init PSTOP */
  outport_byte(tport+PSTART, 0);	       	/* init PSTART */
  outport_byte(tport+BNRY, -1);			/* init BNRY */
  outport_byte(tport+ISR, -1);			/* clear IR's */
  outport_byte(tport+IMR, 0x15);	       	/* enable interrupt */

  outport_byte(tport+CMDR, MSK_PG1 + MSK_RD2);

  for (i1=0; i1<6; i1++)			/* initial physical addr */
    outport_byte(tport+PAR+i1, hwaddr[i1]);

  for (i1=0; i1<MARsize; i1++)			/* clear multicast */
    outport_byte(tport+MAR+i1, 0);
  outport_byte(tport+CURR, 0);			/* init current packet */

  outport_byte(tport+CMDR, MSK_PG0 + MSK_RD2);
  outport_byte(tport+CMDR, MSK_STA + MSK_RD2);	/* put 8390 on line */
  outport_byte(tport+RCR, MSK_AB);		/* MSK_AB accept broadcast */

  if (ultra) {
    inport_byte(tport+0x0c, temp);
    outport_byte(tport+0x0c, temp | 0x80);
    outport_byte(tport+0x05, 0x80);
    outport_byte(tport+0x06, 0x01);
  }

  /*
   * Set up interrupts
   */
  sc->irqInfo.hdl = wd8003Enet_interrupt_handler;
  sc->irqInfo.on  = nopOn;
  sc->irqInfo.off = nopOn;
  sc->irqInfo.isOn = wdIsOn;

  st = BSP_install_rtems_irq_handler (&sc->irqInfo);
  if (!st)
    rtems_panic ("Can't attach WD interrupt handler for irq %d\n",
		  sc->irqInfo.name);
}

static void
wd_rxDaemon (void *arg)
{
  unsigned int tport;
  struct ether_header *eh;
  struct wd_softc *dp = (struct wd_softc *)&wd_softc[0];
  struct ifnet *ifp = &dp->arpcom.ac_if;
  struct mbuf *m;
  unsigned int i2;
  unsigned int len;
  volatile unsigned char start, next, current;
  unsigned char *shp, *temp;
  unsigned short *real_short_ptr;
  rtems_event_set events;

  tport = wd_softc[0].port ;

  for (;;){

    rtems_bsdnet_event_receive (INTERRUPT_EVENT,
				RTEMS_WAIT|RTEMS_EVENT_ANY,
				RTEMS_NO_TIMEOUT,
				&events);

    for (;;){
      inport_byte(tport+BNRY, start);

      outport_byte(tport+CMDR, MSK_PG1 + MSK_RD2);
      inport_byte(tport+CURR, current);
      outport_byte(tport+CMDR, MSK_PG0 + MSK_RD2);

      start += 1;
      if (start >= OUTPAGE){
	start = 0;
      }

      if (current == start)
	break;

      /* real_short_ptr avoids cast on lvalue which gcc no longer allows */
      shp = dp->base + 1 + (SHAPAGE * start);
      next = *shp++;
      real_short_ptr = (unsigned short *)shp;
      len = *(real_short_ptr)++ - 4;

      if (next >= OUTPAGE){
	next = 0;
      }

      MGETHDR (m, M_WAIT, MT_DATA);
      MCLGET (m, M_WAIT);
      m->m_pkthdr.rcvif = ifp;

      temp = (unsigned char *) m->m_data;
      m->m_len = m->m_pkthdr.len = len - sizeof(struct ether_header);

      if ((i2 = (OUTPAGE - start) * SHAPAGE - 4) < len){
	memcpy(temp, shp, i2);
	len -= i2;
	temp += i2;
	shp = dp->base;
      }
      memcpy(temp, shp, len);

      eh = mtod (m, struct ether_header *);
      m->m_data += sizeof(struct ether_header);
      ether_input (ifp, eh, m);

      outport_byte(tport+BNRY, next-1);
    }

  /*
   * Ring overwrite
   */
    if (overrun){
      outport_byte(tport+ISR, MSK_OVW);		/* reset IR */
      outport_byte(tport+TCR, 0);		/* out of loopback */
      if (resend  == 1)
	outport_byte(tport+CMDR, MSK_TXP + MSK_RD2);	/* resend */
      resend = 0;
      overrun = 0;
    }

    outport_byte(tport+IMR, 0x15);  /* re-enable IT rx */
  }
}

static void
sendpacket (struct ifnet *ifp, struct mbuf *m)
{
	struct wd_softc *dp = ifp->if_softc;
	struct mbuf *n;
	unsigned int len, tport;
	uint8_t *shp, txReady;

	tport = dp->port;

  /*
   * Waiting for Transmitter ready
   */
  inport_byte(tport+CMDR, txReady);
  while(txReady & MSK_TXP)
    inport_byte(tport+CMDR, txReady);

  len = 0;
  shp = dp->base + (SHAPAGE * OUTPAGE);

  n = m;

  for (;;){
    len += m->m_len;
    memcpy(shp, (char *)m->m_data, m->m_len);
    shp += m->m_len ;
    if ((m = m->m_next) == NULL)
      break;
  }

  m_freem(n);

  if (len < ET_MINLEN) len = ET_MINLEN;
  outport_byte(tport+TBCR0, len);
  outport_byte(tport+TBCR1, (len >> 8) );
  outport_byte(tport+TPSR, OUTPAGE);
  outport_byte(tport+CMDR, MSK_TXP + MSK_RD2);
}

/*
 * Driver transmit daemon
 */
void
wd_txDaemon (void *arg)
{
	struct wd_softc *sc = (struct wd_softc *)arg;
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
			sendpacket (ifp, m);
		}
		ifp->if_flags &= ~IFF_OACTIVE;
	}
}

/*
 * Send packet (caller provides header).
 */
static void
wd_start (struct ifnet *ifp)
{
	struct wd_softc *sc = ifp->if_softc;

	rtems_event_send (sc->txDaemonTid, START_TRANSMIT_EVENT);
	ifp->if_flags |= IFF_OACTIVE;
}

/*
 * Initialize and start the device
 */
static void
wd_init (void *arg)
{
  struct wd_softc *sc = arg;
  struct ifnet *ifp = &sc->arpcom.ac_if;

  if (sc->txDaemonTid == 0) {

    /*
     * Set up WD hardware
     */
    wd8003Enet_initialize_hardware (sc);

    /*
     * Start driver tasks
     */
    sc->txDaemonTid = rtems_bsdnet_newproc ("SCtx", 4096, wd_txDaemon, sc);
    sc->rxDaemonTid = rtems_bsdnet_newproc ("SCrx", 4096, wd_rxDaemon, sc);
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
wd_stop (struct wd_softc *sc)
{
  unsigned int tport;
  unsigned char temp;
  struct ifnet *ifp = &sc->arpcom.ac_if;

  ifp->if_flags &= ~IFF_RUNNING;

  /*
   * Stop the transmitter
   */
  tport=wd_softc[0].port ;
  inport_byte(tport+0x04,temp);
  outport_byte(tport+0x04, temp & 0x7f);
  outport_byte(tport + CMDR, MSK_STP + MSK_RD2);

}

/*
 * Show interface statistics
 */
static void
wd_stats (struct wd_softc *sc)
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
wd_ioctl (struct ifnet *ifp, ioctl_command_t command, caddr_t data)
{
	struct wd_softc *sc = ifp->if_softc;
	int error = 0;

	switch (command) {
	case SIOCGIFADDR:
	case SIOCSIFADDR:
		ether_ioctl (ifp, command, data);
		break;

	case SIOCSIFFLAGS:
		switch (ifp->if_flags & (IFF_UP | IFF_RUNNING)) {
		case IFF_RUNNING:
			wd_stop (sc);
			break;

		case IFF_UP:
			wd_init (sc);
			break;

		case IFF_UP | IFF_RUNNING:
			wd_stop (sc);
			wd_init (sc);
			break;

		default:
			break;
		}
		break;

	case SIO_RTEMS_SHOW_STATS:
		wd_stats (sc);
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
 * Attach an WD driver to the system
 */
int
rtems_wd_driver_attach (struct rtems_bsdnet_ifconfig *config, int attach)
{
	struct wd_softc *sc;
	struct ifnet *ifp;
	int mtu;
	int i;

	/*
	 * Find a free driver
	 */
	for (i = 0 ; i < NWDDRIVER ; i++) {
		sc = &wd_softc[i];
		ifp = &sc->arpcom.ac_if;
		if (ifp->if_softc == NULL)
			break;
	}
	if (i >= NWDDRIVER) {
		printf ("Too many WD drivers.\n");
		return 0;
	}

	/*
	 * Process options
	 */
	if (config->hardware_address) {
	  memcpy (sc->arpcom.ac_enaddr, config->hardware_address,
		  ETHER_ADDR_LEN);
	}
	else {
	  memset (sc->arpcom.ac_enaddr, 0x08,ETHER_ADDR_LEN);
	}
	if (config->mtu)
		mtu = config->mtu;
	else
		mtu = ETHERMTU;

	if (config->irno)
		sc->irqInfo.name = config->irno;
	else
		sc->irqInfo.name = 5;

	if (config->port)
		sc->port = config->port;
	else
		sc->port = 0x240;

	if (config->bpar) {
		sc->bpar = config->bpar;
		sc->base = (unsigned char*) config->bpar;
	}
	else {
		sc->bpar = 0xD0000;
		sc->base = (unsigned char*) 0xD0000;
	}

	sc->acceptBroadcast = !config->ignore_broadcast;

	/*
	 * Set up network interface values
	 */
	ifp->if_softc = sc;
	ifp->if_unit = i + 1;
	ifp->if_name = "wd";
	ifp->if_mtu = mtu;
	ifp->if_init = wd_init;
	ifp->if_ioctl = wd_ioctl;
	ifp->if_start = wd_start;
	ifp->if_output = ether_output;
	ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX;
	if (ifp->if_snd.ifq_maxlen == 0)
		ifp->if_snd.ifq_maxlen = ifqmaxlen;

	/*
	 * init some variables
	 */
	overrun = 0;
	resend = 0;

 	/*
	 * Attach the interface
	 */
	if_attach (ifp);
	ether_ifattach (ifp);
	return 1;
};

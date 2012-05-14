/*
 * RTEMS driver for M68360 SCC1 Ethernet
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 *
 * Modify for Motorola 68en320 Ethernet Controller
 * CXR S.A. France - 2000/09/14  - franckj@cxr.fr
 */
#include <bsp.h>
#include <stdio.h>
#include <rtems/error.h>
#include <m302_int.h>
#include <rtems/rtems_bsdnet.h>

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <errno.h>

#include <net/if.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <rtems/m68k/m68302.h>

/*
 * Number of SCCs supported by this driver
 */
#define NSCCDRIVER	1

/*
 * Default number of buffer descriptors set aside for this driver.
 * The number of transmit buffer descriptors has to be quite large
 * since a single frame often uses four or more buffer descriptors.
 */
#define RX_BUF_COUNT     64
#define TX_BUF_COUNT     64

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

static 	struct m68302_imp *		a_m68302_imp;

#define M302_ETHER_IVECTOR		0x40

/*
 * Per-device data
 */
struct scc_softc {
	struct arpcom		arpcom;
	struct mbuf		**rxMbuf;
	struct mbuf		**txMbuf;
	int			acceptBroadcast;
	int			rxBdCount;
	int			txBdCount;
	int			txBdHead;
	int			txBdTail;
	int			txBdActiveCount;

	struct m68302_scc_bd 	*rxBdBase;
	struct m68302_scc_bd 	*txBdBase;

	rtems_id		rxDaemonTid;
	rtems_id		txDaemonTid;

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
	unsigned long	txCoalesced;
	unsigned long	txCoalesceFailed;
	unsigned long	txRetry;
};
static struct scc_softc scc_softc[NSCCDRIVER];

/*
 *  interrupt handler
 */
static rtems_isr
m302Enet_interrupt_handler (rtems_vector_number v)
{

	/*
	 * Frame received?
	 */
	if (M68en302imp_intr_event & INTR_EVENT_BIT_RFINT) {
		M68en302imp_intr_event = INTR_EVENT_BIT_RFINT;
		M68en302imp_intr_mask &= ~INTR_MASK_BIT_RFIEN;
		scc_softc[0].rxInterrupts++;
		rtems_event_send (scc_softc[0].rxDaemonTid, INTERRUPT_EVENT);
	}

	/*
	 * Buffer transmitted    or transmitter error?
	 */
	if ((M68en302imp_intr_event & INTR_EVENT_BIT_TFINT) ||
		 ((M68en302imp_intr_event & INTR_EVENT_BIT_TXB))){
		M68en302imp_intr_event = INTR_EVENT_BIT_TFINT | INTR_EVENT_BIT_TXB;
		M68en302imp_intr_mask &= ~(INTR_MASK_BIT_TFIEN | INTR_MASK_BIT_TXIEN);
		scc_softc[0].txInterrupts++;
		rtems_event_send (scc_softc[0].txDaemonTid, INTERRUPT_EVENT);
	}
}

/*
 * Initialize the ethernet hardware
 */
static void
m302Enet_initialize_hardware (struct scc_softc *sc)
{
	int i;
	unsigned char *hwaddr;
	rtems_status_code status;
	rtems_isr_entry old_handler;
	struct m68302_scc_bd	*a_bd;			/* Buffer Descriptor pointer */
	ushort *cam;
#define LBK		0x0008
#define DSQE	0x0010
#define FDE		0x0020

	/*
	 * standard loopback
	 */
	M68302imp_port_data	(1) &= ~(LBK);
	M68302imp_port_data	(1) |= (FDE);

	M68en302imp_ecntrl=0x0001;
	/*
	 * Set dma configuration status register EDMA
	 */
	i = (sc->txBdCount == 16) ? EDMA_BDSIZE_16T_112R :
			(sc->txBdCount == 32) ? EDMA_BDSIZE_32T_96R :
			(sc->txBdCount == 64) ? EDMA_BDSIZE_64T_64R :
			EDMA_BDSIZE_8T_120R;

	M68en302imp_edma =  EDMA_BLIM_8ACCESS | EDMA_WMRK_16FIFO | EDMA_BIT_TSRLY | (ushort)i;

	/*
	 * Set maximum receive buffer length
	 */

	M68en302imp_emrblr = RBUF_SIZE;	/* 1520 */

	/*
	 * Set interrupt vector
	 */
	M68en302imp_intr_vect = M302_ETHER_IVECTOR;

	M68en302imp_intr_mask=0x0;

	/*
	 * Set ethernet Configuration
	 */
	M68en302imp_ecnfig=0x0000;

	/*
	 * Set ETHER_TEST
	 */
	M68en302imp_ether_test=0x0000;

	/*
	 *  Set AR control Register
	 *	Ignore/accept broadcast packets as specified
	 */
	M68en302imp_ar_cntrl = ((sc->acceptBroadcast) ? 0 : AR_CNTRL_BIT_NO_BROADCAST) ;

	/*
	 * Allocate mbuf pointers
	 */
	sc->rxMbuf = malloc (sc->rxBdCount * sizeof *sc->rxMbuf, M_MBUF, M_NOWAIT);
	sc->txMbuf = malloc (sc->txBdCount * sizeof *sc->txMbuf, M_MBUF, M_NOWAIT);
	if (!sc->rxMbuf || !sc->txMbuf)
		rtems_panic ("No memory for mbuf pointers");

	/*
	 * Set our physical address
	 */
	hwaddr = sc->arpcom.ac_enaddr;

	cam=(ushort *)(M68en302imp_cet);
	for (i=0;i<64;i++){
		cam[(4*i)]=0x00ff;
		cam[(4*i)+1]=0x00ff;
		cam[(4*i)+2]=0x00ff;
	}

	cam[4] = (hwaddr[0] << 8) | hwaddr[1];
	cam[5] = (hwaddr[2] << 8) | hwaddr[3];
	cam[6] = (hwaddr[4] << 8) | hwaddr[5];

	/*
	 * Set receiver and transmitter buffer descriptor bases
	 */
	a_bd = M68302imp_a_eth_bd (0);			/* point to first BD */

	for (i=0;i<128;i++){

		M68302_scc_bd_stat_ctrl	(a_bd + i) = 0;
		M68302_scc_bd_data_lgth	(a_bd + i) = 0;
		M68302_scc_bd_p_buffer	(a_bd + i) = NULL;
	}

	sc->txBdBase = M68302imp_a_eth_bd (  0 );			/* point to first BD */
	sc->rxBdBase = M68302imp_a_eth_bd ( sc->txBdCount);		/* point to first RX BD atfer all TX*/

	/*
	 * Set up transmit buffer descriptors
	 */
	for (i = 0 ; i < sc->txBdCount ; i++) {
		sc->txMbuf[i] = NULL;
	}
	sc->txBdHead = sc->txBdTail = 0;
	sc->txBdActiveCount = 0;

	/*
	 * Clear any outstanding events
	 */
	M68en302imp_intr_event = 0x07FF;
	/*
	 * Set up interrupts
	 */

	status = rtems_interrupt_catch (m302Enet_interrupt_handler,
						M302_ETHER_IVECTOR,
						&old_handler);
	if (status != RTEMS_SUCCESSFUL)
		rtems_panic ("Can't attach M302 ether interrupt handler: %s\r\n",
						rtems_status_text (status));
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
m302Enet_retire_tx_bd (struct scc_softc *sc)
{

	uint16_t         status;
	int i;
	int nRetired;
	struct mbuf *m, *n;
	int retries = 0;
	int saveStatus = 0;

	i = sc->txBdTail;
	nRetired = 0;
	while ((sc->txBdActiveCount != 0)
	   &&  (((status = (sc->txBdBase + i)->stat_ctrl) & BUF_STAT_READY) == 0)) {
		/*
		 * Check for errors which stop the transmitter.
		 */
		if (status & (BUF_STAT_LATE_COLLISION |
				BUF_STAT_RETRY_LIMIT |
				BUF_STAT_UNDERRUN)) {
			int j;

			if (status & BUF_STAT_LATE_COLLISION)
				sc->txLateCollision++;
			if (status & BUF_STAT_RETRY_LIMIT)
				sc->txRetryLimit++;
			if (status & BUF_STAT_UNDERRUN)
				sc->txUnderrun++;

			/*
			 * Reenable buffer descriptors
			 */
			j = sc->txBdTail;
			for (;;) {
				status = (sc->txBdBase + j)->stat_ctrl;
				if (status & BUF_STAT_READY)
					break;
				(sc->txBdBase + j)->stat_ctrl = BUF_STAT_READY |
					(status & ( BUF_STAT_WRAP |
						   BUF_STAT_INTERRUPT |
						   BUF_STAT_LAST |
						   BUF_STAT_TX_CRC));
				if (status & BUF_STAT_LAST)
					break;
				if (++j == sc->txBdCount)
					j = 0;
			}

			/*
			 * Move transmitter back to the first
			 * buffer descriptor in the frame.
			 */
/*			m360.scc1p._tbptr = m360.scc1p.tbase +
				sc->txBdTail * sizeof (m360BufferDescriptor_t);
*/
			/*
			 * Restart the transmitter
			 */
/*			M360ExecuteRISC (M360_CR_OP_RESTART_TX | M360_CR_CHAN_SCC1);*/
			continue;
		}
		saveStatus |= status;
		retries += (status >> 2) & 0xF;
		nRetired++;
		if (status & BUF_STAT_LAST) {
			/*
			 * A full frame has been transmitted.
			 * Free all the associated buffer descriptors.
			 */
			if (saveStatus & BUF_STAT_DEFER)
				sc->txDeferred++;
			if (saveStatus & BUF_STAT_HEARTBIT)
				sc->txHeartbeat++;
			if (saveStatus & BUF_STAT_CARRIER_LOST)
				sc->txLostCarrier++;
			saveStatus = 0;
			sc->txRetry += retries;
			retries = 0;
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
 * SCC reader task
 */
static void
scc_rxDaemon (void *arg)
{

	struct scc_softc *sc = (struct scc_softc *)arg;
	struct ifnet *ifp = &sc->arpcom.ac_if;
	struct mbuf *m;
	uint16_t         status;
	volatile struct m68302_scc_bd *rxBd;
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
		rxBd->p_buffer = mtod (m, void *);

		if (++rxBdIndex == sc->rxBdCount) {
			rxBd->stat_ctrl = BUF_STAT_EMPTY | BUF_STAT_INTERRUPT | BUF_STAT_WRAP;
			break;
		}
		rxBd->stat_ctrl = BUF_STAT_EMPTY | BUF_STAT_INTERRUPT;
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
		if ((status = rxBd->stat_ctrl) & BUF_STAT_EMPTY) {
			/*
			 * Clear old events
			 */
			M68en302imp_intr_event = INTR_EVENT_BIT_RFINT;

			/*
			 * Wait for packet
			 * Note that the buffer descriptor is checked
			 * *before* the event wait -- this catches the
			 * possibility that a packet arrived between the
			 * `if' above, and the clearing of the event register.
			 */
			while ((status = rxBd->stat_ctrl) & BUF_STAT_EMPTY) {
				rtems_interrupt_level level;
				rtems_event_set events;

				/*
				 * Unmask RXF (Full frame received) event
				 */
				rtems_interrupt_disable (level);
				M68en302imp_intr_mask |= INTR_MASK_BIT_RFIEN;

				rtems_interrupt_enable (level);
				rtems_bsdnet_event_receive (INTERRUPT_EVENT,
						RTEMS_WAIT|RTEMS_EVENT_ANY,
						RTEMS_NO_TIMEOUT,
						&events);
			}
		}

		/*
		 * Check that packet is valid
		 */
		if ((status & (BUF_STAT_LAST |
				BUF_STAT_FIRST_IN_FRAME |
				BUF_STAT_LONG |
				BUF_STAT_NONALIGNED |
				BUF_STAT_SHORT |
				BUF_STAT_CRC_ERROR |
				BUF_STAT_OVERRUN |
				BUF_STAT_COLLISION)) ==
						(BUF_STAT_LAST |
						BUF_STAT_FIRST_IN_FRAME)) {
			/*
			 * Pass the packet up the chain.
			 * FIXME: Packet filtering hook could be done here.
			 */
			struct ether_header *eh;

			m = sc->rxMbuf[rxBdIndex];
			m->m_len = m->m_pkthdr.len = rxBd->data_lgth -
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
			rxBd->p_buffer = mtod (m, void *);
		}
		else {
			/*
			 * Something went wrong with the reception
			 */
			if (!(status & BUF_STAT_LAST))
				sc->rxNotLast++;
			if (!(status & BUF_STAT_FIRST_IN_FRAME))
				sc->rxNotFirst++;
			if (status & BUF_STAT_LONG)
				sc->rxGiant++;
			if (status & BUF_STAT_NONALIGNED)
				sc->rxNonOctet++;
			if (status & BUF_STAT_SHORT)
				sc->rxRunt++;
			if (status & BUF_STAT_CRC_ERROR)
				sc->rxBadCRC++;
			if (status & BUF_STAT_OVERRUN)
				sc->rxOverrun++;
			if (status & BUF_STAT_COLLISION)
				sc->rxCollision++;
		}

		/*
		 * Reenable the buffer descriptor
		 */
		rxBd->stat_ctrl = (status & (BUF_STAT_WRAP | BUF_STAT_INTERRUPT)) | BUF_STAT_EMPTY;

		/*
		 * Move to next buffer descriptor
		 */
		if (++rxBdIndex == sc->rxBdCount)
			rxBdIndex = 0;
	}

}

static void
sendpacket (struct ifnet *ifp, struct mbuf *m)
{

	struct scc_softc *sc = ifp->if_softc;
	volatile struct m68302_scc_bd *firstTxBd, *txBd;
	struct mbuf *l = NULL;
	uint16_t         status;
	int nAdded;

	/*
	 * Free up buffer descriptors
	 */
	m302Enet_retire_tx_bd (sc);
	/*
	 * Set up the transmit buffer descriptors.
	 * No need to pad out short packets since the
	 * hardware takes care of that automatically.
	 * No need to copy the packet to a contiguous buffer
	 * since the hardware is capable of scatter/gather DMA.
	 */
	status = 0;
	nAdded = 0;
	txBd = firstTxBd = sc->txBdBase + sc->txBdHead;
	while (m) {
		/*
		 * There are more mbufs in the packet than there
		 * are transmit buffer descriptors.
		 * Coalesce into a single buffer.
		 */
		if (nAdded == sc->txBdCount) {
			struct mbuf *nm;
			int j;
			char *dest;

			/*
			 * Get the pointer to the first mbuf of the packet
			 */
			if (sc->txBdTail != sc->txBdHead)
				rtems_panic ("sendpacket coalesce");
			m = sc->txMbuf[sc->txBdTail];

			/*
			 * Rescind the buffer descriptor READY bits
			 */
			for (j = 0 ; j < sc->txBdCount ; j++)
				(sc->txBdBase + j)->stat_ctrl = 0;

			/*
			 * Allocate an mbuf cluster
			 * Toss the packet if allocation fails
			 */
			MGETHDR (nm, M_DONTWAIT, MT_DATA);
			if (nm == NULL) {
				sc->txCoalesceFailed++;
				m_freem (m);

				return;
			}
			MCLGET (nm, M_DONTWAIT);
			if (nm->m_ext.ext_buf == NULL) {
				sc->txCoalesceFailed++;
				m_freem (m);
				m_free (nm);

				return;
			}
			nm->m_pkthdr = m->m_pkthdr;
			nm->m_len = nm->m_pkthdr.len;

			/*
			 * Copy data from packet chain to mbuf cluster
			 */
			sc->txCoalesced++;
			dest = nm->m_ext.ext_buf;
			while (m) {
				struct mbuf *n;

				if (m->m_len) {
					memcpy (dest, mtod(m, caddr_t), m->m_len);
					dest += m->m_len;
				}
				MFREE (m, n);
				m = n;
			}

			/*
			 * Redo the send with the new mbuf cluster
			 */
			m = nm;
			nAdded = 0;
			status = 0;

			continue;
		}

		/*
		 * Wait for buffer descriptor to become available.
		 */
		if ((sc->txBdActiveCount + nAdded) == sc->txBdCount) {
			/*
			 * Clear old events
			 */
			M68en302imp_intr_event = INTR_EVENT_BIT_TFINT | INTR_EVENT_BIT_TXB;

			/*
			 * Wait for buffer descriptor to become available.
			 * Note that the buffer descriptors are checked
			 * *before* entering the wait loop -- this catches
			 * the possibility that a buffer descriptor became
			 * available between the `if' above, and the clearing
			 * of the event register.
			 * This is to catch the case where the transmitter
			 * stops in the middle of a frame -- and only the
			 * last buffer descriptor in a frame can generate
			 * an interrupt.
			 */
			m302Enet_retire_tx_bd (sc);
			while ((sc->txBdActiveCount + nAdded) == sc->txBdCount) {
				rtems_interrupt_level level;
				rtems_event_set events;

				/*
				 * Unmask TXB (buffer transmitted) and
				 * TXE (transmitter error) events.
				 */
				rtems_interrupt_disable (level);
				M68en302imp_intr_mask |= INTR_MASK_BIT_TFIEN | INTR_MASK_BIT_TXIEN;
  				rtems_interrupt_enable (level);

				rtems_bsdnet_event_receive (INTERRUPT_EVENT,
						RTEMS_WAIT|RTEMS_EVENT_ANY,
						RTEMS_NO_TIMEOUT,
						&events);
				m302Enet_retire_tx_bd (sc);
			}
		}

		/*
		 * The IP fragmentation routine in ip_output
		 * can produce packet fragments with zero length.
		 */
		if (m->m_len) {
			/*
			 * Fill in the buffer descriptor.
			 * Don't set the READY flag in the first buffer
			 * descriptor till the whole packet has been readied.
			 */
			txBd = sc->txBdBase + sc->txBdHead;
			txBd->p_buffer = mtod (m, void *);
			txBd->data_lgth = m->m_len;

			sc->txMbuf[sc->txBdHead] = m;
			status = nAdded ? BUF_STAT_READY : 0;
			if (++sc->txBdHead == sc->txBdCount) {
				status |= BUF_STAT_WRAP;
				sc->txBdHead = 0;
			}
			txBd->stat_ctrl = status;
			l = m;
			m = m->m_next;
			nAdded++;

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
	}
	if (nAdded) {
		/*
		 * Send the packet
		 */
		txBd->stat_ctrl = status | BUF_STAT_LAST | BUF_STAT_TX_CRC | BUF_STAT_INTERRUPT;
		firstTxBd->stat_ctrl |= BUF_STAT_READY;
		sc->txBdActiveCount += nAdded;

	}

}

/*
 * Driver transmit daemon
 */
void
scc_txDaemon (void *arg)
{

	struct scc_softc *sc = (struct scc_softc *)arg;
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
scc_start (struct ifnet *ifp)
{
	struct scc_softc *sc = ifp->if_softc;

	rtems_event_send (sc->txDaemonTid, START_TRANSMIT_EVENT);
	ifp->if_flags |= IFF_OACTIVE;
}

/*
 * Initialize and start the device
 */
static void
scc_init (void *arg)
{
	struct scc_softc *sc = arg;
	struct ifnet *ifp = &sc->arpcom.ac_if;

	if (sc->txDaemonTid == 0) {

		/*
		 * Set up SCC hardware
		 */
		m302Enet_initialize_hardware (sc);

		sc->txDaemonTid = rtems_bsdnet_newproc ("SCtx", 4096, scc_txDaemon, sc);
		sc->rxDaemonTid = rtems_bsdnet_newproc ("SCrx", 4096, scc_rxDaemon, sc);

	}

	/*
	 * Set flags appropriately
	 */
/*	if (ifp->if_flags & IFF_PROMISC)
		m360.scc1.psmr |= 0x200;
	else
		m360.scc1.psmr &= ~0x200;
*/
	/*
	 * Tell the world that we're running.
	 */
	ifp->if_flags |= IFF_RUNNING;

	/*
	 * Enable receiver and transmitter
	 */
	M68en302imp_ecntrl = ECNTRL_BIT_RESET	| ECNTRL_BIT_ETHER_EN;

}

/*
 * Stop the device
 */
static void
scc_stop (struct scc_softc *sc)
{

	struct ifnet *ifp = &sc->arpcom.ac_if;

	ifp->if_flags &= ~IFF_RUNNING;

	/*
	 * Shut down receiver and transmitter
	 */
	M68en302imp_ecntrl &= ~(ECNTRL_BIT_RESET	| ECNTRL_BIT_ETHER_EN);

}

/*
 * Show interface statistics
 */
static void
scc_stats (struct scc_softc *sc)
{
	printf ("      Rx Interrupts:%-8lu", sc->rxInterrupts);
	printf ("       Not First:%-8lu", sc->rxNotFirst);
	printf ("        Not Last:%-8lu\r\n", sc->rxNotLast);
	printf ("              Giant:%-8lu", sc->rxGiant);
	printf ("            Runt:%-8lu", sc->rxRunt);
	printf ("       Non-octet:%-8lu\r\n", sc->rxNonOctet);
	printf ("            Bad CRC:%-8lu", sc->rxBadCRC);
	printf ("         Overrun:%-8lu", sc->rxOverrun);
	printf ("       Collision:%-8lu\r\n", sc->rxCollision);
/*	printf ("          Discarded:%-8lu\r\n", (unsigned long)m360.scc1p.un.ethernet.disfc);
*/
	printf ("      Tx Interrupts:%-8lu", sc->txInterrupts);
	printf ("        Deferred:%-8lu", sc->txDeferred);
	printf (" Missed Hearbeat:%-8lu\r\n", sc->txHeartbeat);
	printf ("         No Carrier:%-8lu", sc->txLostCarrier);
	printf ("Retransmit Limit:%-8lu", sc->txRetryLimit);
	printf ("  Late Collision:%-8lu\r\n", sc->txLateCollision);
	printf ("           Underrun:%-8lu", sc->txUnderrun);
	printf (" Raw output wait:%-8lu", sc->txRawWait);
	printf ("       Coalesced:%-8lu\r\n", sc->txCoalesced);
	printf ("    Coalesce failed:%-8lu", sc->txCoalesceFailed);
	printf ("         Retries:%-8lu\r\n", sc->txRetry);
}

/*
 * Driver ioctl handler
 */
static int
scc_ioctl (struct ifnet *ifp, ioctl_command_t command, caddr_t data)
{

	struct scc_softc *sc = ifp->if_softc;
	int error = 0;

	switch (command) {
	case SIOCGIFADDR:
	case SIOCSIFADDR:
		ether_ioctl (ifp, command, data);
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
		scc_stats (sc);
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
rtems_ether1_driver_attach (struct rtems_bsdnet_ifconfig *config)
{
	struct scc_softc *sc;
	struct ifnet *ifp;
	int mtu;
	int unitNumber;
	char *unitName;

	a_m68302_imp = (struct m68302_imp *)0x700000L;
	/*
 	 * Parse driver name
	 */
	if ((unitNumber = rtems_bsdnet_parse_driver_name (config, &unitName)) < 0)
		return 0;

	/*
	 * Is driver free?
	 */
	if ((unitNumber <= 0) || (unitNumber > NSCCDRIVER)) {
		printf ("Bad SCC unit number.\r\n");
		return 0;
	}
	sc = &scc_softc[unitNumber - 1];
	ifp = &sc->arpcom.ac_if;
	if (ifp->if_softc != NULL) {
		printf ("Driver already in use.\r\n");
		return 0;
	}

	/*
	 * Process options
	 */
	if (config->hardware_address) {
		memcpy (sc->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);
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
		sc->txBdCount = TX_BUF_COUNT;
	sc->acceptBroadcast = !config->ignore_broadcast;

	/*
	 * Set up network interface values
	 */
	ifp->if_softc = sc;
	ifp->if_unit = unitNumber;
	ifp->if_name = unitName;
	ifp->if_mtu = mtu;
	ifp->if_init = scc_init;
	ifp->if_ioctl = scc_ioctl;
	ifp->if_start = scc_start;
	ifp->if_output = ether_output;
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

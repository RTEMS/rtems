/*
 * RTEMS driver for M68360 SCC1 Ethernet
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 */
#include <bsp.h>
#include <rtems/m68k/m68360.h>
#include <stdio.h>
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

/*
 * Number of SCCs supported by this driver
 */
#define NSCCDRIVER	1

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
	m360BufferDescriptor_t	*rxBdBase;
	m360BufferDescriptor_t	*txBdBase;
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

extern void *_RomBase;	/* From linkcmds */

/*
 * SCC1 interrupt handler
 */
static rtems_isr
m360Enet_interrupt_handler (rtems_vector_number v)
{
	/*
	 * Frame received?
	 */
	if ((m360.scc1.sccm & 0x8) && (m360.scc1.scce & 0x8)) {
		m360.scc1.scce = 0x8;
		m360.scc1.sccm &= ~0x8;
		scc_softc[0].rxInterrupts++;
		rtems_event_send (scc_softc[0].rxDaemonTid, INTERRUPT_EVENT);
	}

	/*
	 * Buffer transmitted or transmitter error?
	 */
	if ((m360.scc1.sccm & 0x12) && (m360.scc1.scce & 0x12)) {
		m360.scc1.scce = 0x12;
		m360.scc1.sccm &= ~0x12;
		scc_softc[0].txInterrupts++;
		rtems_event_send (scc_softc[0].txDaemonTid, INTERRUPT_EVENT);
	}
	m360.cisr = 1UL << 30;	/* Clear SCC1 interrupt-in-service bit */
}

/*
 * Initialize the ethernet hardware
 */
static void
m360Enet_initialize_hardware (struct scc_softc *sc)
{
	int i;
	unsigned char *hwaddr;
	rtems_status_code status;
	rtems_isr_entry old_handler;

	/*
	 * Configure port A CLK1, CLK2, TXD1 and RXD1 pins
	 */
	m360.papar |=  0x303;
	m360.padir &= ~0x303;
	m360.paodr &= ~0x303;

	/*
	 * Configure port C CTS1* and CD1* pins
	 */
	m360.pcpar &= ~0x30;
	m360.pcdir &= ~0x30;
	m360.pcso  |=  0x30;

	/*
	 * Connect CLK1 and CLK2 to SCC1
	 */
	m360.sicr &= ~0xFF;
	m360.sicr |= (5 << 3) | 4;

	/*
	 * Allocate mbuf pointers
	 */
	sc->rxMbuf = malloc (sc->rxBdCount * sizeof *sc->rxMbuf, M_MBUF, M_NOWAIT);
	sc->txMbuf = malloc (sc->txBdCount * sizeof *sc->txMbuf, M_MBUF, M_NOWAIT);
	if (!sc->rxMbuf || !sc->txMbuf)
		rtems_panic ("No memory for mbuf pointers");

	/*
	 * Set receiver and transmitter buffer descriptor bases
	 */
	sc->rxBdBase = M360AllocateBufferDescriptors(sc->rxBdCount);
	sc->txBdBase = M360AllocateBufferDescriptors(sc->txBdCount);
	m360.scc1p.rbase = (char *)sc->rxBdBase - (char *)&m360;
	m360.scc1p.tbase = (char *)sc->txBdBase - (char *)&m360;

	/*
	 * Send "Init parameters" command
	 */
	M360ExecuteRISC (M360_CR_OP_INIT_RX_TX | M360_CR_CHAN_SCC1);

	/*
	 * Set receive and transmit function codes
	 */
	m360.scc1p.rfcr = M360_RFCR_MOT | M360_RFCR_DMA_SPACE;
	m360.scc1p.tfcr = M360_TFCR_MOT | M360_TFCR_DMA_SPACE;

	/*
	 * Set maximum receive buffer length
	 */
	m360.scc1p.mrblr = RBUF_SIZE;

	/*
	 * Set CRC parameters
	 */
	m360.scc1p.un.ethernet.c_pres = 0xFFFFFFFF;
	m360.scc1p.un.ethernet.c_mask = 0xDEBB20E3;

	/*
	 * Clear diagnostic counters
	 */
	m360.scc1p.un.ethernet.crcec = 0;
	m360.scc1p.un.ethernet.alec = 0;
	m360.scc1p.un.ethernet.disfc = 0;

	/*
	 * Set pad value
	 */
	m360.scc1p.un.ethernet.pads = 0x8888;

	/*
	 * Set retry limit
	 */
	m360.scc1p.un.ethernet.ret_lim = 15;

	/*
	 * Set maximum and minimum frame length
	 */
	m360.scc1p.un.ethernet.mflr = 1518;
	m360.scc1p.un.ethernet.minflr = 64;
	m360.scc1p.un.ethernet.maxd1 = RBUF_SIZE;
	m360.scc1p.un.ethernet.maxd2 = RBUF_SIZE;

	/*
	 * Clear group address hash table
	 */
	m360.scc1p.un.ethernet.gaddr1 = 0;
	m360.scc1p.un.ethernet.gaddr2 = 0;
	m360.scc1p.un.ethernet.gaddr3 = 0;
	m360.scc1p.un.ethernet.gaddr4 = 0;

	/*
	 * Set our physical address
	 */
	hwaddr = sc->arpcom.ac_enaddr;
	m360.scc1p.un.ethernet.paddr_h = (hwaddr[5] << 8) | hwaddr[4];
	m360.scc1p.un.ethernet.paddr_m = (hwaddr[3] << 8) | hwaddr[2];
	m360.scc1p.un.ethernet.paddr_l = (hwaddr[1] << 8) | hwaddr[0];

	/*
	 * Aggressive retry
	 */
	m360.scc1p.un.ethernet.p_per = 0;

	/*
	 * Clear individual address hash table
	 */
	m360.scc1p.un.ethernet.iaddr1 = 0;
	m360.scc1p.un.ethernet.iaddr2 = 0;
	m360.scc1p.un.ethernet.iaddr3 = 0;
	m360.scc1p.un.ethernet.iaddr4 = 0;

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
	 * Clear any outstanding events
	 */
	m360.scc1.scce = 0xFFFF;

	/*
	 * Set up interrupts
	 */
	status = rtems_interrupt_catch (m360Enet_interrupt_handler,
						(m360.cicr & 0xE0) | 0x1E,
						&old_handler);
	if (status != RTEMS_SUCCESSFUL)
		rtems_panic ("Can't attach M360 SCC1 interrupt handler: %s\n",
						rtems_status_text (status));
	m360.scc1.sccm = 0;	/* No interrupts unmasked till necessary */
	m360.cimr |= (1UL << 30);	/* Enable SCC1 interrupt */

	/*
	 * Set up General SCC Mode Register
	 * Ethernet configuration
	 */
	m360.scc1.gsmr_h = 0x0;
	m360.scc1.gsmr_l = 0x1088000c;

	/*
	 * Set up data synchronization register
	 * Ethernet synchronization pattern
	 */
	m360.scc1.dsr = 0xd555;

	/*
	 * Set up protocol-specific mode register
	 *	Heartbeat check
	 *	No force collision
	 *	Discard short frames
	 *	Individual address mode
	 *	Ethernet CRC
	 *	Not promisuous
	 *	Ignore/accept broadcast packets as specified
	 *	Normal backoff timer
	 *	No loopback
	 *	No input sample at end of frame
	 *	64-byte limit for late collision
	 *	Wait 22 bits before looking for start of frame delimiter
	 *	Disable full-duplex operation
	 */
	m360.scc1.psmr = 0x880A | (sc->acceptBroadcast ? 0 : 0x100);

	/*
	 * Enable the TENA (RTS1*) pin
	 */
#if (defined (M68360_ATLAS_HSB))
	m360.pbpar |= 0x1000;
	m360.pbdir |= 0x1000;
#else
	m360.pcpar |=  0x1;
	m360.pcdir &= ~0x1;
#endif
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
m360Enet_retire_tx_bd (struct scc_softc *sc)
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
	   &&  (((status = (sc->txBdBase + i)->status) & M360_BD_READY) == 0)) {
		/*
		 * Check for errors which stop the transmitter.
		 */
		if (status & (M360_BD_LATE_COLLISION |
				M360_BD_RETRY_LIMIT |
				M360_BD_UNDERRUN)) {
			int j;

			if (status & M360_BD_LATE_COLLISION)
				sc->txLateCollision++;
			if (status & M360_BD_RETRY_LIMIT)
				sc->txRetryLimit++;
			if (status & M360_BD_UNDERRUN)
				sc->txUnderrun++;

			/*
			 * Reenable buffer descriptors
			 */
			j = sc->txBdTail;
			for (;;) {
				status = (sc->txBdBase + j)->status;
				if (status & M360_BD_READY)
					break;
				(sc->txBdBase + j)->status = M360_BD_READY |
					(status & (M360_BD_PAD |
						   M360_BD_WRAP |
						   M360_BD_INTERRUPT |
						   M360_BD_LAST |
						   M360_BD_TX_CRC));
				if (status & M360_BD_LAST)
					break;
				if (++j == sc->txBdCount)
					j = 0;
			}

			/*
			 * Move transmitter back to the first
			 * buffer descriptor in the frame.
			 */
			m360.scc1p._tbptr = m360.scc1p.tbase +
				sc->txBdTail * sizeof (m360BufferDescriptor_t);

			/*
			 * Restart the transmitter
			 */
			M360ExecuteRISC (M360_CR_OP_RESTART_TX | M360_CR_CHAN_SCC1);
			continue;
		}
		saveStatus |= status;
		retries += (status >> 2) & 0xF;
		nRetired++;
		if (status & M360_BD_LAST) {
			/*
			 * A full frame has been transmitted.
			 * Free all the associated buffer descriptors.
			 */
			if (saveStatus & M360_BD_DEFER)
				sc->txDeferred++;
			if (saveStatus & M360_BD_HEARTBEAT)
				sc->txHeartbeat++;
			if (saveStatus & M360_BD_CARRIER_LOST)
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
	volatile m360BufferDescriptor_t *rxBd;
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
		if (++rxBdIndex == sc->rxBdCount) {
			rxBd->status = M360_BD_EMPTY | M360_BD_INTERRUPT | M360_BD_WRAP;
			break;
		}
		rxBd->status = M360_BD_EMPTY | M360_BD_INTERRUPT;
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
		if ((status = rxBd->status) & M360_BD_EMPTY) {
			/*
			 * Clear old events
			 */
			m360.scc1.scce = 0x8;

			/*
			 * Wait for packet
			 * Note that the buffer descriptor is checked
			 * *before* the event wait -- this catches the
			 * possibility that a packet arrived between the
			 * `if' above, and the clearing of the event register.
			 */
			while ((status = rxBd->status) & M360_BD_EMPTY) {
				rtems_interrupt_level level;
				rtems_event_set events;

				/*
				 * Unmask RXF (Full frame received) event
				 */
				rtems_interrupt_disable (level);
				m360.scc1.sccm |= 0x8;
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
		if ((status & (M360_BD_LAST |
				M360_BD_FIRST_IN_FRAME |
				M360_BD_LONG |
				M360_BD_NONALIGNED |
				M360_BD_SHORT |
				M360_BD_CRC_ERROR |
				M360_BD_OVERRUN |
				M360_BD_COLLISION)) ==
						(M360_BD_LAST |
						M360_BD_FIRST_IN_FRAME)) {
			/*
			 * Pass the packet up the chain.
			 * FIXME: Packet filtering hook could be done here.
			 */
			struct ether_header *eh;

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
			if (!(status & M360_BD_LAST))
				sc->rxNotLast++;
			if (!(status & M360_BD_FIRST_IN_FRAME))
				sc->rxNotFirst++;
			if (status & M360_BD_LONG)
				sc->rxGiant++;
			if (status & M360_BD_NONALIGNED)
				sc->rxNonOctet++;
			if (status & M360_BD_SHORT)
				sc->rxRunt++;
			if (status & M360_BD_CRC_ERROR)
				sc->rxBadCRC++;
			if (status & M360_BD_OVERRUN)
				sc->rxOverrun++;
			if (status & M360_BD_COLLISION)
				sc->rxCollision++;
		}

		/*
		 * Reenable the buffer descriptor
		 */
		rxBd->status = (status & (M360_BD_WRAP | M360_BD_INTERRUPT)) | M360_BD_EMPTY;

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
	volatile m360BufferDescriptor_t *firstTxBd, *txBd;
	struct mbuf *l = NULL;
	uint16_t         status;
	int nAdded;

	/*
	 * Free up buffer descriptors
	 */
	m360Enet_retire_tx_bd (sc);

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
				(sc->txBdBase + j)->status = 0;

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
			m360.scc1.scce = 0x12;

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
			m360Enet_retire_tx_bd (sc);
			while ((sc->txBdActiveCount + nAdded) == sc->txBdCount) {
				rtems_interrupt_level level;
				rtems_event_set events;

				/*
				 * Unmask TXB (buffer transmitted) and
				 * TXE (transmitter error) events.
				 */
				rtems_interrupt_disable (level);
				m360.scc1.sccm |= 0x12;
				rtems_interrupt_enable (level);
				rtems_bsdnet_event_receive (INTERRUPT_EVENT,
						RTEMS_WAIT|RTEMS_EVENT_ANY,
						RTEMS_NO_TIMEOUT,
						&events);
				m360Enet_retire_tx_bd (sc);
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
			txBd->buffer = mtod (m, void *);
			txBd->length = m->m_len;
			sc->txMbuf[sc->txBdHead] = m;
			status = nAdded ? M360_BD_READY : 0;
			if (++sc->txBdHead == sc->txBdCount) {
				status |= M360_BD_WRAP;
				sc->txBdHead = 0;
			}
			txBd->status = status;
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
		txBd->status = status | M360_BD_PAD | M360_BD_LAST | M360_BD_TX_CRC | M360_BD_INTERRUPT;
		firstTxBd->status |= M360_BD_READY;
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
		m360Enet_initialize_hardware (sc);

		/*
		 * Start driver tasks
		 */
		sc->txDaemonTid = rtems_bsdnet_newproc ("SCtx", 4096, scc_txDaemon, sc);
		sc->rxDaemonTid = rtems_bsdnet_newproc ("SCrx", 4096, scc_rxDaemon, sc);

	}

	/*
	 * Set flags appropriately
	 */
	if (ifp->if_flags & IFF_PROMISC)
		m360.scc1.psmr |= 0x200;
	else
		m360.scc1.psmr &= ~0x200;

	/*
	 * Tell the world that we're running.
	 */
	ifp->if_flags |= IFF_RUNNING;

	/*
	 * Enable receiver and transmitter
	 */
	m360.scc1.gsmr_l |= 0x30;
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
	m360.scc1.gsmr_l &= ~0x30;
}

/*
 * Show interface statistics
 */
static void
scc_stats (struct scc_softc *sc)
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
	printf ("          Discarded:%-8lu\n", (unsigned long)m360.scc1p.un.ethernet.disfc);

	printf ("      Tx Interrupts:%-8lu", sc->txInterrupts);
	printf ("        Deferred:%-8lu", sc->txDeferred);
	printf (" Missed Hearbeat:%-8lu\n", sc->txHeartbeat);
	printf ("         No Carrier:%-8lu", sc->txLostCarrier);
	printf ("Retransmit Limit:%-8lu", sc->txRetryLimit);
	printf ("  Late Collision:%-8lu\n", sc->txLateCollision);
	printf ("           Underrun:%-8lu", sc->txUnderrun);
	printf (" Raw output wait:%-8lu", sc->txRawWait);
	printf ("       Coalesced:%-8lu\n", sc->txCoalesced);
	printf ("    Coalesce failed:%-8lu", sc->txCoalesceFailed);
	printf ("         Retries:%-8lu\n", sc->txRetry);
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
rtems_scc1_driver_attach (struct rtems_bsdnet_ifconfig *config, int attaching)
{
	struct scc_softc *sc;
	struct ifnet *ifp;
	int mtu;
	int unitNumber;
	char *unitName;

	/*
	 * Make sure we're really being attached
	 */
	if (!attaching) {
		printf ("SCC1 driver can not be detached.\n");
		return 0;
	}

	/*
 	 * Parse driver name
	 */
	if ((unitNumber = rtems_bsdnet_parse_driver_name (config, &unitName)) < 0)
		return 0;

	/*
	 * Is driver free?
	 */
	if ((unitNumber <= 0) || (unitNumber > NSCCDRIVER)) {
		printf ("Bad SCC unit number.\n");
		return 0;
	}
	sc = &scc_softc[unitNumber - 1];
	ifp = &sc->arpcom.ac_if;
	if (ifp->if_softc != NULL) {
		printf ("Driver already in use.\n");
		return 0;
	}

	/*
	 * Process options
	 */
	if (config->hardware_address) {
		memcpy (sc->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);
	}
	else {
		/*
		 * The first 4 bytes of the bootstrap prom
		 * contain the value loaded into the stack
		 * pointer as part of the CPU32's hardware
		 * reset exception handler.  The following
		 * 4 bytes contain the value loaded into the
		 * program counter.  The boards' Ethernet
		 * address is stored in the six bytes
		 * immediately preceding this initial
		 * program counter value.
		 *
		 * See start360/start360.s.
		 */
		const unsigned long *ExceptionVectors;
		const unsigned char *entryPoint;

		/*
		 * Sanity check -- assume entry point must be
		 * within 1 MByte of beginning of boot ROM.
		 */
		ExceptionVectors = (const unsigned long *)&_RomBase;
		entryPoint = (const unsigned char *)ExceptionVectors[1];
		if (((unsigned long)entryPoint - (unsigned long)ExceptionVectors)
					>= (1 * 1024 * 1024)) {
			printf ("Warning -- Ethernet address can not be found in bootstrap PROM.\n");
			sc->arpcom.ac_enaddr[0] = 0x08;
			sc->arpcom.ac_enaddr[1] = 0xF3;
			sc->arpcom.ac_enaddr[2] = 0x3E;
			sc->arpcom.ac_enaddr[3] = 0xC2;
			sc->arpcom.ac_enaddr[4] = 0x7E;
			sc->arpcom.ac_enaddr[5] = 0x38;
		}
		else {
			memcpy (sc->arpcom.ac_enaddr, entryPoint - ETHER_ADDR_LEN, ETHER_ADDR_LEN);
		}
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
}

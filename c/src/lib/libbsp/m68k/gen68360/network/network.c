/*
 * RTEMS/KA9Q driver for M68360 SCC1 Ethernet
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 *
 *  $Id$
 */
#include <bsp.h>
#include <m68360.h>
#include <rtems/error.h>
#include <ka9q/rtems_ka9q.h>
#include <ka9q/global.h>
#include <ka9q/enet.h>
#include <ka9q/iface.h>
#include <ka9q/netuser.h>
#include <ka9q/trace.h>
#include <ka9q/commands.h>

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
 * RTEMS event used by interrupt handler to signal daemons.
 * This must *not* be the same event used by the KA9Q task synchronization.
 */
#define INTERRUPT_EVENT	RTEMS_EVENT_1

/*
 * Receive buffer size -- Allow for a full ethernet packet plus a pointer
 */
#define RBUF_SIZE	(1520 + sizeof (struct iface *))

/*
 * Hardware-specific storage
 */
struct m360EnetDriver {
	struct mbuf		**rxMbuf;
	struct mbuf		**txMbuf;
	int			rxBdCount;
	int			txBdCount;
	int			txBdHead;
	int			txBdTail;
	int			txBdActiveCount;
	m360BufferDescriptor_t	*rxBdBase;
	m360BufferDescriptor_t	*txBdBase;
	struct iface		*iface;
	rtems_id		txWaitTid;

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
static struct m360EnetDriver m360EnetDriver[NSCCDRIVER];

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
		m360EnetDriver[0].rxInterrupts++;
		rtems_event_send (m360EnetDriver[0].iface->rxproc, INTERRUPT_EVENT);
	}

	/*
	 * Buffer transmitted or transmitter error?
	 */
	if ((m360.scc1.sccm & 0x12) && (m360.scc1.scce & 0x12)) {
		m360.scc1.scce = 0x12;
		m360.scc1.sccm &= ~0x12;
		m360EnetDriver[0].txInterrupts++;
		rtems_event_send (m360EnetDriver[0].txWaitTid, INTERRUPT_EVENT);
	}
	m360.cisr = 1UL << 30;	/* Clear SCC1 interrupt-in-service bit */
}

/*
 * Initialize the ethernet hardware
 */
static void
m360Enet_initialize_hardware (struct m360EnetDriver *dp, int broadcastFlag)
{
	int i;
	unsigned char *hwaddr;
	rtems_status_code sc;
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
	dp->rxMbuf = mallocw (dp->rxBdCount * sizeof *dp->rxMbuf);
	dp->txMbuf = mallocw (dp->txBdCount * sizeof *dp->txMbuf);

	/*
	 * Set receiver and transmitter buffer descriptor bases
	 */
	dp->rxBdBase = M360AllocateBufferDescriptors(dp->rxBdCount);
	dp->txBdBase = M360AllocateBufferDescriptors(dp->txBdCount);
	m360.scc1p.rbase = (char *)dp->rxBdBase - (char *)&m360;
	m360.scc1p.tbase = (char *)dp->txBdBase - (char *)&m360;

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
	m360.scc1p.mrblr = 1520;

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
	m360.scc1p.un.ethernet.maxd1 = 1520;
	m360.scc1p.un.ethernet.maxd2 = 1520;

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
	hwaddr = dp->iface->hwaddr;
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
	for (i = 0 ; i < dp->rxBdCount ; i++)
		(dp->rxBdBase + i)->status = 0;

	/*
	 * Set up transmit buffer descriptors
	 */
	for (i = 0 ; i < dp->txBdCount ; i++) {
		(dp->txBdBase + i)->status = 0;
		dp->txMbuf[i] = NULL;
	}
	dp->txBdHead = dp->txBdTail = 0;
	dp->txBdActiveCount = 0;

	/*
	 * Clear any outstanding events
	 */
	m360.scc1.scce = 0xFFFF;

	/*
	 * Set up interrupts
	 */
	sc = rtems_interrupt_catch (m360Enet_interrupt_handler,
						(m360.cicr & 0xE0) | 0x1E,
						&old_handler);
	if (sc != RTEMS_SUCCESSFUL)
		rtems_panic ("Can't attach M360 SCC1 interrupt handler: %s\n",
							rtems_status_text (sc));
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
	m360.scc1.psmr = 0x880A | (broadcastFlag ? 0 : 0x100);

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

	/*
	 * Enable receiver and transmitter
	 */
	m360.scc1.gsmr_l = 0x1088003c;
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
m360Enet_retire_tx_bd (struct m360EnetDriver *dp)
{
	rtems_unsigned16 status;
	int i;
	int nRetired;

	i = dp->txBdTail;
	nRetired = 0;
	while ((dp->txBdActiveCount != 0)
	   &&  (((status = (dp->txBdBase + i)->status) & M360_BD_READY) == 0)) {
		/*
		 * See if anything went wrong
		 */
		if (status & (M360_BD_DEFER |
				M360_BD_HEARTBEAT |
				M360_BD_LATE_COLLISION |
				M360_BD_RETRY_LIMIT |
				M360_BD_UNDERRUN |
				M360_BD_CARRIER_LOST)) {
			/*
			 * Check for errors which stop the transmitter.
			 */
			if (status & (M360_BD_LATE_COLLISION |
					M360_BD_RETRY_LIMIT |
					M360_BD_UNDERRUN)) {
				if (status & M360_BD_LATE_COLLISION)
					m360EnetDriver[0].txLateCollision++;
				if (status & M360_BD_RETRY_LIMIT)
					m360EnetDriver[0].txRetryLimit++;
				if (status & M360_BD_UNDERRUN)
					m360EnetDriver[0].txUnderrun++;

				/*
				 * Restart the transmitter
				 */
				M360ExecuteRISC (M360_CR_OP_RESTART_TX | M360_CR_CHAN_SCC1);
			}
			if (status & M360_BD_DEFER)
				m360EnetDriver[0].txDeferred++;
			if (status & M360_BD_HEARTBEAT)
				m360EnetDriver[0].txHeartbeat++;
			if (status & M360_BD_CARRIER_LOST)
				m360EnetDriver[0].txLostCarrier++;
		}
		nRetired++;
		if (status & M360_BD_LAST) {
			/*
			 * A full frame has been transmitted.
			 * Free all the associated buffer descriptors.
			 */
			dp->txBdActiveCount -= nRetired;
			while (nRetired) {
				nRetired--;
				free_mbuf (&dp->txMbuf[dp->txBdTail]);
				if (++dp->txBdTail == dp->txBdCount)
					dp->txBdTail = 0;
			}
		}
		if (++i == dp->txBdCount)
			i = 0;
	}
}

/*
 * Send raw packet (caller provides header).
 * This code runs in the context of the interface transmit
 * task or in the context of the network task.
 */
static int
m360Enet_raw (struct iface *iface, struct mbuf **bpp)
{
	struct m360EnetDriver *dp = &m360EnetDriver[iface->dev];
	struct mbuf *bp;
	volatile m360BufferDescriptor_t *firstTxBd, *txBd;
	rtems_unsigned16 status;
	int nAdded;

	/*
	 * Fill in some logging data
	 */
	iface->rawsndcnt++;
	iface->lastsent = secclock ();
	dump (iface, IF_TRACE_OUT, *bpp);

	/*
	 * It would not do to have two tasks active in the transmit
	 * loop at the same time.
	 * The blocking is simple-minded since the odds of two tasks
	 * simultaneously attempting to use this code are low.  The only
	 * way that two tasks can try to run here is:
	 *	1) Task A enters this code and ends up having to
	 *	   wait for a transmit buffer descriptor.
	 *	2) Task B  gains control and tries to transmit a packet.
	 * The RTEMS/KA9Q scheduling semaphore ensures that there
	 * are no race conditions associated with manipulating the
	 * txWaitTid variable.
	 */
	if (dp->txWaitTid) {
		dp->txRawWait++;
		while (dp->txWaitTid)
			rtems_ka9q_ppause (10);
	}

	/*
	 * Free up buffer descriptors
	 */
	m360Enet_retire_tx_bd (dp);

	/*
	 * Set up the transmit buffer descriptors.
	 * No need to pad out short packets since the
	 * hardware takes care of that automatically.
	 * No need to copy the packet to a contiguous buffer
	 * since the hardware is capable of scatter/gather DMA.
	 */
	bp = *bpp;
	nAdded = 0;
	txBd = firstTxBd = dp->txBdBase + dp->txBdHead;
	for (;;) {
		/*
		 * Wait for buffer descriptor to become available.
		 */
		if ((dp->txBdActiveCount + nAdded) == dp->txBdCount) {
			/*
			 * Find out who we are
			 */
			if (dp->txWaitTid == 0)
				rtems_task_ident (0, 0, &dp->txWaitTid);

			/*
			 * Clear old events
			 */
			m360.scc1.scce = 0x12;

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
			m360Enet_retire_tx_bd (dp);
			while ((dp->txBdActiveCount + nAdded) == dp->txBdCount) {
				/*
				 * Unmask TXB (buffer transmitted) and
				 * TXE (transmitter error) events.
				 */
				m360.scc1.sccm |= 0x12;

				rtems_ka9q_event_receive (INTERRUPT_EVENT,
						RTEMS_WAIT|RTEMS_EVENT_ANY,
						RTEMS_NO_TIMEOUT);
				m360Enet_retire_tx_bd (dp);
			}
		}

		/*
		 * Fill in the buffer descriptor
		 */
		txBd->buffer = bp->data;
		txBd->length = bp->cnt;
		dp->txMbuf[dp->txBdHead] = bp;

		/*
		 * Don't set the READY flag till the whole packet has been readied.
		 */
		status = nAdded ? M360_BD_READY : 0;
		nAdded++;
		if (++dp->txBdHead == dp->txBdCount) {
			status |= M360_BD_WRAP;
			dp->txBdHead = 0;
		}

		/*
		 * Set the transmit buffer status.
		 * Break out of the loop if this mbuf is the last in the frame.
		 */
		if ((bp = bp->next) == NULL) {
			status |= M360_BD_PAD | M360_BD_LAST | M360_BD_TX_CRC | M360_BD_INTERRUPT;
			txBd->status = status;
			firstTxBd->status |= M360_BD_READY;
			dp->txBdActiveCount += nAdded;
			break;
		}
		txBd->status = status;
		txBd = dp->txBdBase + dp->txBdHead;
	}

	/*
	 * Show that we've finished with the packet
	 */
	dp->txWaitTid = 0;
	*bpp = NULL;
	return 0;
}

/*
 * SCC reader task
 */
static void
m360Enet_rx (int dev, void *p1, void *p2)
{
	struct iface *iface = (struct iface *)p1;
	struct m360EnetDriver *dp = (struct m360EnetDriver *)p2;
	struct mbuf *bp;
	rtems_unsigned16 status;
	m360BufferDescriptor_t *rxBd;
	int rxBdIndex;
	int continuousCount;

	/*
	 * Allocate space for incoming packets and start reception
	 */
	for (rxBdIndex = 0 ; ;) {
		rxBd = dp->rxBdBase + rxBdIndex;
		dp->rxMbuf[rxBdIndex] = bp = ambufw (RBUF_SIZE);
		bp->data += sizeof (struct iface *);
		rxBd->buffer = bp->data;
		rxBd->status = M360_BD_EMPTY | M360_BD_INTERRUPT;
		if (++rxBdIndex == dp->rxBdCount) {
			rxBd->status |= M360_BD_WRAP;
			break;
		}
	}

	/*
	 * Input packet handling loop
	 */
	continuousCount = 0;
	rxBdIndex = 0;
	for (;;) {
		rxBd = dp->rxBdBase + rxBdIndex;

		/*
		 * Wait for packet if there's not one ready
		 */
		if ((status = rxBd->status) & M360_BD_EMPTY) {
			/*
			 * Reset `continuous-packet' count
			 */
			continuousCount = 0;

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
				/*
				 * Unmask RXF (Full frame received) event
				 */
				m360.scc1.sccm |= 0x8;

				rtems_ka9q_event_receive (INTERRUPT_EVENT,
						RTEMS_WAIT|RTEMS_EVENT_ANY,
						RTEMS_NO_TIMEOUT);
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
			 * Pass the packet up the chain
			 * The mbuf count is reduced to remove
			 * the frame check sequence at the end
			 * of the packet.
			 */
			bp = dp->rxMbuf[rxBdIndex];
			bp->cnt = rxBd->length - sizeof (uint32);
			net_route (iface, &bp);

			/*
			 * Give the network code a chance to digest the
			 * packet.  This guards against a flurry of 
			 * incoming packets (usually an ARP storm) from
			 * using up all the available memory.
			 */
			if (++continuousCount >= dp->rxBdCount)
				kwait_null ();

			/*
			 * Allocate a new mbuf
			 * FIXME: It seems to me that it would be better
			 * if there were some way to limit number of mbufs
			 * in use by this interface, but I don't see any
			 * way of determining when the mbuf we pass up
			 * is freed.
			 */
			dp->rxMbuf[rxBdIndex] = bp = ambufw (RBUF_SIZE);
			bp->data += sizeof (struct iface *);
			rxBd->buffer = bp->data;
		}
		else {
			/*
			 * Something went wrong with the reception
			 */
			if (!(status & M360_BD_LAST))
				dp->rxNotLast++;
			if (!(status & M360_BD_FIRST_IN_FRAME))
				dp->rxNotFirst++;
			if (status & M360_BD_LONG)
				dp->rxGiant++;
			if (status & M360_BD_NONALIGNED)
				dp->rxNonOctet++;
			if (status & M360_BD_SHORT)
				dp->rxRunt++;
			if (status & M360_BD_CRC_ERROR)
				dp->rxBadCRC++;
			if (status & M360_BD_OVERRUN)
				dp->rxOverrun++;
			if (status & M360_BD_COLLISION)
				dp->rxCollision++;
		}

		/*
		 * Reenable the buffer descriptor
		 */
		rxBd->status = (status & (M360_BD_WRAP | M360_BD_INTERRUPT)) | M360_BD_EMPTY;

		/*
		 * Move to next buffer descriptor
		 */
		if (++rxBdIndex == dp->rxBdCount)
			rxBdIndex = 0;
	}
}

/*
 * Shut down the interface
 * FIXME: This is a pretty simple-minded routine.  It doesn't worry
 * about cleaning up mbufs, shutting down daemons, etc.
 */
static int
m360Enet_stop (struct iface *iface)
{
	/*
	 * Stop the transmitter
	 */
	M360ExecuteRISC (M360_CR_OP_GR_STOP_TX | M360_CR_CHAN_SCC1);

	/*
	 * Wait for graceful stop
	 * FIXME: Maybe there should be a watchdog loop around this....
	 */
	while ((m360.scc1.scce & 0x80) == 0)
		continue;

	/*
	 * Shut down receiver and transmitter
	 */
	m360.scc1.gsmr_l &= ~0x30;
	return 0;
}

/*
 * Show interface statistics
 */
static void
m360Enet_show (struct iface *iface)
{
	printf ("      Rx Interrupts:%-8lu", m360EnetDriver[0].rxInterrupts);
	printf ("       Not First:%-8lu", m360EnetDriver[0].rxNotFirst);
	printf ("        Not Last:%-8lu\n", m360EnetDriver[0].rxNotLast);
	printf ("              Giant:%-8lu", m360EnetDriver[0].rxGiant);
	printf ("            Runt:%-8lu", m360EnetDriver[0].rxRunt);
	printf ("       Non-octet:%-8lu\n", m360EnetDriver[0].rxNonOctet);
	printf ("            Bad CRC:%-8lu", m360EnetDriver[0].rxBadCRC);
	printf ("         Overrun:%-8lu", m360EnetDriver[0].rxOverrun);
	printf ("       Collision:%-8lu\n", m360EnetDriver[0].rxCollision);
	printf ("          Discarded:%-8lu\n", (unsigned long)m360.scc1p.un.ethernet.disfc);

	printf ("      Tx Interrupts:%-8lu", m360EnetDriver[0].txInterrupts);
	printf ("        Deferred:%-8lu", m360EnetDriver[0].txDeferred);
	printf (" Missed Hearbeat:%-8lu\n", m360EnetDriver[0].txHeartbeat);
	printf ("         No Carrier:%-8lu", m360EnetDriver[0].txLostCarrier);
	printf ("Retransmit Limit:%-8lu", m360EnetDriver[0].txRetryLimit);
	printf ("  Late Collision:%-8lu\n", m360EnetDriver[0].txLateCollision);
	printf ("           Underrun:%-8lu", m360EnetDriver[0].txUnderrun);
	printf (" Raw output wait:%-8lu\n", m360EnetDriver[0].txRawWait);
}

/*
 * Attach an SCC driver to the system
 * This is the only `extern' function in the driver.
 *
 * argv[0]: interface label, e.g., "rtems"
 * The remainder of the arguemnts are key/value pairs:
 * mtu ##                  --  maximum transmission unit, default 1500
 * broadcast y/n           -- accept or ignore broadcast packets, default yes
 * rbuf ##                 -- Set number of receive buffer descriptors
 * rbuf ##                 -- Set number of transmit buffer descriptors
 * ip ###.###.###.###      -- IP address
 * ether ##:##:##:##:##:## -- Ethernet address
 * ether prom              -- Get Ethernet address from bootstrap PROM
 */
int
rtems_ka9q_driver_attach (int argc, char *argv[], void *p)
{
	struct iface *iface;
	struct m360EnetDriver *dp;
	char *cp;
	int i;
	int argIndex;
	int broadcastFlag;
	char cbuf[30];

	/*
	 * Find a free driver
	 */
	for (i = 0 ; i < NSCCDRIVER ; i++) {
		if (m360EnetDriver[i].iface == NULL)
			break;
	}
	if (i >= NSCCDRIVER) {
		printf ("Too many SCC drivers.\n");
		return -1;
	}
	if (if_lookup (argv[0]) != NULL) {
		printf ("Interface %s already exists\n", argv[0]);
		return -1;
	}
	dp = &m360EnetDriver[i];

	/*
	 * Create an inteface descriptor
	 */
	iface = callocw (1, sizeof *iface);
	iface->name = strdup (argv[0]);

	/*
	 * Set default values
	 */
	broadcastFlag = 1;
	dp->txWaitTid = 0;
	dp->rxBdCount = RX_BUF_COUNT;
	dp->txBdCount = TX_BUF_COUNT * TX_BD_PER_BUF;
	iface->mtu = 1500;
	iface->addr = Ip_addr;
	iface->hwaddr = mallocw (EADDR_LEN);
	memset (iface->hwaddr, 0x08, EADDR_LEN);

	/*
	 * Parse arguments
	 */
	for (argIndex = 1 ; argIndex < (argc - 1) ; argIndex++) {
		if (strcmp ("mtu", argv[argIndex]) == 0) {
			iface->mtu = atoi (argv[++argIndex]);
		}
		else if (strcmp ("broadcast", argv[argIndex]) == 0) {
			if (*argv[++argIndex] == 'n')
				broadcastFlag = 0;
		}
		else if (strcmp ("rbuf", argv[argIndex]) == 0) {
			dp->rxBdCount = atoi (argv[++argIndex]);
		}
		else if (strcmp ("tbuf", argv[argIndex]) == 0) {
			dp->txBdCount = atoi (argv[++argIndex]) * TX_BD_PER_BUF;
		}
		else if (strcmp ("ip", argv[argIndex]) == 0) {
			iface->addr = resolve (argv[++argIndex]);
		}
		else if (strcmp ("ether", argv[argIndex]) == 0) {
			argIndex++;
			if (strcmp (argv[argIndex], "prom") == 0) {
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
				extern void *_RomBase;	/* From linkcmds */
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
					iface->hwaddr[0] = 0x08;
					iface->hwaddr[1] = 0xF3;
					iface->hwaddr[2] = 0x3E;
					iface->hwaddr[3] = 0xC2;
					iface->hwaddr[4] = 0xE7;
					iface->hwaddr[5] = 0x08;
				}
				else {
					memcpy (iface->hwaddr, entryPoint - 6, 6);
				}
			}
			else {
				gether (iface->hwaddr, argv[argIndex]);
			}
		}
		else {
			printf ("Argument %d (%s) is invalid.\n", argIndex, argv[argIndex]);
			return -1;
		}
	}
	printf ("Ethernet address: %s\n", pether (cbuf, iface->hwaddr));

	/*
	 * Fill in remainder of interface configuration
	 */
	iface->dev = i;
	iface->raw = m360Enet_raw;
	iface->stop = m360Enet_stop;
	iface->show = m360Enet_show;
	dp->iface = iface;
	setencap (iface, "Ethernet");

	/*
	 * Set up SCC hardware
	 */
	m360Enet_initialize_hardware (dp, broadcastFlag);

	/*
	 * Chain onto list of interfaces
	 */
	iface->next = Ifaces;
	Ifaces = iface;

	/*
	 * Start I/O daemons
	 */
	cp = if_name (iface, " tx");
	iface->txproc = newproc (cp, 1024, if_tx, iface->dev, iface, NULL, 0);
	free (cp);
	cp = if_name (iface, " rx");
	iface->rxproc = newproc (cp, 1024, m360Enet_rx, iface->dev, iface, dp, 0);
	free (cp);
	return 0;
}

/*
 * FIXME: There should be an ioctl routine to allow things like
 * enabling/disabling reception of broadcast packets.
 */

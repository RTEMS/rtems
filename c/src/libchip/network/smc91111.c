#include <rtems.h>
#include <errno.h>

#include <bsp.h>

/*
 *  This driver currently only supports SPARC with the old style
 *  exception processing and the Phytec Phycore MPC5554.
 *  This test keeps it from being compiled on systems which haven't been
 *  tested.
 *
 */

#if defined(HAS_SMC91111)
  #define SMC91111_SUPPORTED
#endif

#if defined(HAS_SMC91111)

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/irq-extension.h>

#include <sys/param.h>
#include <sys/mbuf.h>

#include <sys/socket.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>

#define SMC91111_INTERRUPT_EVENT      RTEMS_EVENT_1	/* RTEMS event used by interrupt handler to signal driver tasks. This must not be any of the events used by the network task synchronization. */
#define SMC91111_START_TRANSMIT_EVENT RTEMS_EVENT_2	/* RTEMS event used to start transmit/receive daemon. This must not be the same as INTERRUPT_EVENT. */
#define SMC91111_TX_WAIT_EVENT        RTEMS_EVENT_3	/* event to send when tx buffers become available */

/* Set to perms of:
   0 disables all debug output
   1 for process debug output
   2 for added data IO output: get_reg, put_reg
   4 for packet allocation/free output
   8 for only startup status, so we can tell we're installed OK
   16 dump phy read/write
   32 precise register dump
   64 dump packets
*/
/*#define DEBUG (-1)*/
/*#define DEBUG (-1 & ~(16))*/
#define DEBUG (0)
/*#define DEBUG (1)*/

#include "smc91111config.h"
#include <libchip/smc91111.h>

#ifdef BSP_FEATURE_IRQ_EXTENSION
  #include <rtems/irq-extension.h>
#endif

struct lan91cxx_priv_data smc91111;

int lan91cxx_hardware_init(struct lan91cxx_priv_data *cpd);
static uint16_t lan91cxx_read_phy(struct lan91cxx_priv_data *cpd,
				    uint8_t phyaddr, uint8_t phyreg);
static void lan91cxx_write_phy(struct lan91cxx_priv_data *cpd,
			       uint8_t phyaddr, uint8_t phyreg,
			       uint16_t value);
static void lan91cxx_start(struct ifnet *ifp);
static void smc91111_start(struct ifnet *ifp);
static int smc_probe(struct lan91cxx_priv_data *cpd);
static void smc91111_stop(struct lan91cxx_priv_data *cpd);
static void smc91111_init(void *arg);
static void lan91cxx_finish_sent(struct lan91cxx_priv_data *cpd);
#if 0
static int lan91cxx_phy_fixed(struct lan91cxx_priv_data *cpd);
static void lan91cxx_phy_configure(struct lan91cxx_priv_data *cpd);
#endif

#define min(l,r) ((l) < (r) ? (l) : (r))
#define max(l,r) ((l) > (r) ? (l) : (r))

/* \ ------------- Interrupt ------------- \ */
void lan91cxx_interrupt_handler(void *arg)
{
	struct lan91cxx_priv_data *cpd = arg;
	unsigned short irq, event;
	unsigned short oldbase;
	unsigned short oldpointer;
	INCR_STAT(cpd, interrupts);
	DEBUG_FUNCTION();

	HAL_READ_UINT16(cpd->base + (LAN91CXX_BS), oldbase);
	oldpointer = get_reg(cpd, LAN91CXX_POINTER);

	/* Get the (unmasked) requests */
	irq = get_reg(cpd, LAN91CXX_INTERRUPT);
	event = irq & (irq >> 8) & 0xff;
	if (0 == event)
		return;

	/*put_reg(cpd, LAN91CXX_INTERRUPT, irq ); */ /* ack interrupts */

	if (event & LAN91CXX_INTERRUPT_ERCV_INT) {
		db_printf("Early receive interrupt");
	} else if (event & LAN91CXX_INTERRUPT_EPH_INT) {
		db_printf("ethernet protocol handler failures");
	} else if (event & LAN91CXX_INTERRUPT_RX_OVRN_INT) {
		db_printf("receive overrun");
	} else if (event & LAN91CXX_INTERRUPT_ALLOC_INT) {
		db_printf("allocation interrupt");
	} else {

		if (event & LAN91CXX_INTERRUPT_TX_SET) {
			db_printf("#*tx irq\n");
			lan91cxx_finish_sent(cpd);
			put_reg(cpd, LAN91CXX_INTERRUPT,
				(irq & 0xff00) | LAN91CXX_INTERRUPT_TX_INT);

			/*rtems_event_send (cpd->txDaemonTid, SMC91111_INTERRUPT_EVENT); */
			/*put_reg(cpd, LAN91CXX_INTERRUPT, (irq & 0xff00) | LAN91CXX_INTERRUPT_TX_INT); */
			/*rtems_event_send (cpd->txDaemonTid, SMC91111_TX_WAIT_EVENT); */
		}
		if (event & LAN91CXX_INTERRUPT_RCV_INT) {
			db_printf("#*rx irq\n");
			rtems_event_send(cpd->rxDaemonTid,
					 SMC91111_INTERRUPT_EVENT);
		}
		if (event &
		    ~(LAN91CXX_INTERRUPT_TX_SET | LAN91CXX_INTERRUPT_RCV_INT))
			db_printf("Unknown interrupt\n");
	}
	db_printf("out %s\n", __FUNCTION__);

	put_reg(cpd, LAN91CXX_POINTER, oldpointer);
	HAL_WRITE_UINT16(cpd->base + (LAN91CXX_BS), oldbase);
}

/* \ ------------- Rx receive ------------- \ */

 /**/
/* This function is called as a result of the "readpacket()" call.*/
/* Its job is to actually fetch data for a packet from the hardware once*/
/* memory buffers have been allocated for the packet.  Note that the buffers*/
/* may come in pieces, using a mbuf list.  */
static void lan91cxx_recv(struct lan91cxx_priv_data *cpd, struct mbuf *m)
{
	struct ifnet *ifp = &cpd->arpcom.ac_if;
	struct ether_header *eh;
	short mlen = 0, plen;
	char *start;
	rxd_t *data = NULL, val;
#if DEBUG & 64
	rxd_t lp = 0;
#endif
	struct mbuf *n;
	dbg_prefix = "<";

	DEBUG_FUNCTION();
	INCR_STAT(cpd, rx_deliver);

	/* ############ read packet ############ */

	put_reg(cpd, LAN91CXX_POINTER,
		(LAN91CXX_POINTER_RCV | LAN91CXX_POINTER_READ |
		 LAN91CXX_POINTER_AUTO_INCR));
	val = get_data(cpd);

	/* packet length (minus header/footer) */
#ifdef LAN91CXX_32BIT_RX
	val = CYG_LE32_TO_CPU(val);
	plen = (val >> 16) - 6;
#else
	val = CYG_LE16_TO_CPU(val);
	plen = get_data(cpd);
	plen = CYG_LE16_TO_CPU(plen) - 6;
#endif

	if ( cpd->c111_reva || LAN91CXX_RX_STATUS_IS_ODD(cpd, val) ) /* RevA Odd-bit BUG */
		plen++;

	for (n = m; n; n = n->m_next) {
#ifdef LAN91CXX_32BIT_RX
		if (mlen == 2) {
#if DEBUG & 64
			db_printf("Appending to last apacket\n");
#endif

			val = get_data(cpd);
			*((unsigned short *)data) = (val >> 16) & 0xffff;
			plen -= 2;
			data = (rxd_t *) n->m_data;
			start = (char *)data;
			mlen = n->m_len;
			if ((data) && (mlen > 1)) {
				*(unsigned short *)data = (val & 0xffff);
				data = (rxd_t *)((unsigned short *)data + 1);
				plen -= 2;
				mlen -= 2;
			}
		} else {
			data = (rxd_t *) n->m_data;
			start = (char *)data;
			mlen = n->m_len;
		}
#else
		data = (rxd_t *) n->m_data;
		start = (char *)data;
		mlen = n->m_len;
#endif

		db1_printf("<[packet : mlen 0x%x, plen 0x%x]\n", mlen, plen);

		if (data) {
			while (mlen >= sizeof(*data)) {
#ifdef LAN91CXX_32BIT_RX
				val = get_data(cpd);
				*(unsigned short *)data = (val >> 16) & 0xffff;
				data = (rxd_t *)((unsigned short *)data + 1);
				*(unsigned short *)data = (val & 0xffff);
				data = (rxd_t *)((unsigned short *)data + 1);
#else
				*data++ = get_data(cpd);
#endif
				mlen -= sizeof(*data);
				plen -= sizeof(*data);
			}
		} else {	/* must actively discard ie. read it from the chip anyway. */
			while (mlen >= sizeof(*data)) {
				(void)get_data(cpd);
				mlen -= sizeof(*data);
				plen -= sizeof(*data);
			}
		}

#if DEBUG & 64
		lp = 0;
		while (((int)start) < ((int)data)) {
			unsigned char a = *(start++);
			unsigned char b = *(start++);
			db64_printf("%02x %02x ", a, b);
			lp += 2;
			if (lp >= 16) {
				db64_printf("\n");
				lp = 0;
			}
		}
		db64_printf(" \n");

#endif
	}
	val = get_data(cpd);	/* Read control word (and potential data) unconditionally */
#ifdef LAN91CXX_32BIT_RX
	if (plen & 2) {
		if (data && (mlen>1) ) {
			*(unsigned short *)data = (val >> 16) & 0xffff;
			data = (rxd_t *)((unsigned short *)data + 1);
			val <<= 16;
			mlen-=2;
		}
	}
	if ( (plen & 1) && data && (mlen>0) )
		*(unsigned char *)data = val >> 24;
#else
	val = CYG_LE16_TO_CPU(val);
	cp = (unsigned char *)data;

	CYG_ASSERT(val & LAN91CXX_CONTROLBYTE_RX, "Controlbyte is not for Rx");
	CYG_ASSERT((1 == mlen) == (0 != LAN91CXX_CONTROLBYTE_IS_ODD(cpd, val)),
		   "Controlbyte does not match");
	if (data && (1 == mlen) && LAN91CXX_CONTROLBYTE_IS_ODD(cpd, val)) {
		cval = val & 0x00ff;	/* last byte contains data */
		*cp = cval;
	}
#endif

	val = get_reg(cpd, LAN91CXX_FIFO_PORTS);
	if (0x8000 & val) {	/* Then the Rx FIFO is empty */
		db4_printf
		    ("<+Rx packet NOT freed, stat is %x (expected %x)\n",
		     val, cpd->rxpacket);
	} else {
		db4_printf("<+Rx packet freed %x (expected %x)\n",
			   0xff & (val >> 8), cpd->rxpacket);
	}

	CYG_ASSERT((0xff & (val >> 8)) == cpd->rxpacket,
		   "Unexpected rx packet");

	/* ############ free packet ############ */
	/* Free packet */
	put_reg(cpd, LAN91CXX_MMU_COMMAND, LAN91CXX_MMU_remrel_rx_frame);

	dbg_prefix = "";

	/* Remove the mac header. This is different from the NetBSD stack. */
	eh = mtod(m, struct ether_header *);
	m->m_data += sizeof(struct ether_header);
	m->m_len -= sizeof(struct ether_header);
	m->m_pkthdr.len -= sizeof(struct ether_header);

	ether_input(ifp, eh, m);

}

/* allocate mbuf chain */
static struct mbuf *smc91111_allocmbufchain(int totlen, struct ifnet *ifp)
{

	struct mbuf *m, *m0, *newm;
	int len;

	MGETHDR(m0, M_DONTWAIT, MT_DATA);
	if (m0 == 0)
		return (0);
	m0->m_pkthdr.rcvif = ifp;
	m0->m_pkthdr.len = totlen;
	len = MHLEN;
	m = m0;

	/* This loop goes through and allocates mbufs for all the data we will be copying in.  */
	while (totlen > 0) {
		if (totlen >= MINCLSIZE) {
			MCLGET(m, M_DONTWAIT);
			if ((m->m_flags & M_EXT) == 0)
				goto bad;
			len = MCLBYTES;
		}

		if (m == m0) {
			caddr_t newdata = (caddr_t)
			    ALIGN(m->m_data +
				  sizeof(struct ether_header)) -
			    sizeof(struct ether_header);
			len -= newdata - m->m_data;
			m->m_data = newdata;
		}

		m->m_len = len = min(totlen, len);

		totlen -= len;
		if (totlen > 0) {
			MGET(newm, M_DONTWAIT, MT_DATA);
			if (newm == 0)
				goto bad;
			len = MLEN;
			m = m->m_next = newm;
		}
	}
	return (m0);

      bad:
	m_freem(m0);
	return (0);
}

static int readpacket(struct lan91cxx_priv_data *cpd)
{
	struct mbuf *m;
	unsigned short stat, complen;
	struct ifnet *ifp = &cpd->arpcom.ac_if;
#ifdef LAN91CXX_32BIT_RX
	uint32_t val;
#endif

	DEBUG_FUNCTION();

	/* ############ read packet nr ############ */
	stat = get_reg(cpd, LAN91CXX_FIFO_PORTS);
	db1_printf("+LAN91CXX_FIFO_PORTS: 0x%04x\n", stat);

	if (0x8000 & stat) {
		/* Then the Rx FIFO is empty */
		db4_printf("!RxEvent with empty fifo\n");
		return 0;
	}

	INCR_STAT(cpd, rx_count);

	db4_printf("+Rx packet allocated %x (previous %x)\n",
		   0xff & (stat >> 8), cpd->rxpacket);

	/* There is an Rx Packet ready */
	cpd->rxpacket = 0xff & (stat >> 8);

	/* ############ read packet header ############ */
	/* Read status and (word) length */
	put_reg(cpd, LAN91CXX_POINTER,
		(LAN91CXX_POINTER_RCV | LAN91CXX_POINTER_READ |
		 LAN91CXX_POINTER_AUTO_INCR | 0x0000));
#ifdef LAN91CXX_32BIT_RX
	val = get_data(cpd);
	val = CYG_LE32_TO_CPU(val);
	stat = val & 0xffff;
	complen = ((val >> 16) & 0xffff) - 6;	/* minus header/footer words */
#else
	stat = get_data(cpd);
	stat = CYG_LE16_TO_CPU(stat);
	complen = get_data(cpd);
	complen = CYG_LE16_TO_CPU(len) - 6;	/* minus header/footer words */
#endif

#ifdef KEEP_STATISTICS
	if (stat & LAN91CXX_RX_STATUS_ALIGNERR)
		INCR_STAT(cpd, rx_align_errors);
	/*if ( stat & LAN91CXX_RX_STATUS_BCAST    ) INCR_STAT(  ); */
	if (stat & LAN91CXX_RX_STATUS_BADCRC)
		INCR_STAT(cpd, rx_crc_errors);
	if (stat & LAN91CXX_RX_STATUS_TOOLONG)
		INCR_STAT(cpd, rx_too_long_frames);
	if (stat & LAN91CXX_RX_STATUS_TOOSHORT)
		INCR_STAT(cpd, rx_short_frames);
	/*if ( stat & LAN91CXX_RX_STATUS_MCAST    ) INCR_STAT(  ); */
#endif				/* KEEP_STATISTICS */

	if ((stat & LAN91CXX_RX_STATUS_BAD) == 0) {
		INCR_STAT(cpd, rx_good);
		/* Then it's OK */

		if (cpd->c111_reva || LAN91CXX_RX_STATUS_IS_ODD(cpd, stat)) /* RevA Odd-bit BUG */
			complen++;

#if DEBUG & 1
		db_printf("good rx - stat: 0x%04x, len: 0x%04x\n", stat,
			  complen);
#endif
		/* Check for bogusly short packets; can happen in promisc mode: */
		/* Asserted against and checked by upper layer driver. */
		if (complen > sizeof(struct ether_header)) {
			/* then it is acceptable; offer the data to the network stack */

			complen = ((complen + 3) & ~3);

			m = smc91111_allocmbufchain(complen, ifp);
			{
				struct mbuf *n = m;
				db_printf("mbuf-chain:");
				while (n) {
					db_printf("[%" PRIxPTR ":%x]",
						  n->m_data,
						  (unsigned int)(n->m_len));
					n = n->m_next;
				}
				db_printf("\n");
			}

			if (m) {
				/* fetch packet data into mbuf chain */
				lan91cxx_recv(cpd, m);
				return 1;
			}
		}
		/*(sc->funs->eth_drv->recv)(sc, len); */
	}

	/* Not OK for one reason or another... */
	db1_printf("!bad rx: stat: 0x%04x, len: 0x%04x\n", stat, complen);

	/* ############ free packet ############ */
	/* Free packet */
	put_reg(cpd, LAN91CXX_MMU_COMMAND, LAN91CXX_MMU_remrel_rx_frame);
	return 1;

}

static void smc91111_rxDaemon(void *arg)
{
	struct lan91cxx_priv_data *cpd = arg;
	rtems_event_set events;
	DEBUG_FUNCTION();

	for (;;) {
		rtems_bsdnet_event_receive(INTERRUPT_EVENT,
					   RTEMS_WAIT | RTEMS_EVENT_ANY,
					   RTEMS_NO_TIMEOUT, &events);

		/* read until read fifo is empty */
		while (!(get_reg(cpd, LAN91CXX_FIFO_PORTS) & 0x8000)) {
			readpacket(cpd);
		}
	}
}

/* \ ------------- Tx send ------------- \ */

static void sendpacket(struct ifnet *ifp, struct mbuf *m)
{
	struct lan91cxx_priv_data *cpd = ifp->if_softc;
	int i, len, plen, tcr, odd;
	struct mbuf *n = m;
	unsigned short *sdata = NULL;
	unsigned short ints, control;
	uint16_t packet, status;
	dbg_prefix = ">";
	DEBUG_FUNCTION();

	cpd->txbusy = 1;

	/* Worry about the TX engine stopping. */
	tcr = get_reg(cpd, LAN91CXX_TCR);
	if (0 == (LAN91CXX_TCR_TXENA & tcr)) {
		db1_printf("> ENGINE RESTART: tcr %x\n", tcr);
		tcr |= LAN91CXX_TCR_TXENA;
		put_reg(cpd, LAN91CXX_TCR, tcr);
	}

	/* ############ packet allocation ############ */

	/* Find packet length */
	plen = 0;
	while (n) {
		plen += n->m_len;
		n = n->m_next;
	}

	/* Alloc new TX packet */
	do {
		put_reg(cpd, LAN91CXX_MMU_COMMAND,
			LAN91CXX_MMU_alloc_for_tx | ((plen >> 8) & 0x07));

		i = 1024 * 1024;
		do {
			status = get_reg(cpd, LAN91CXX_INTERRUPT);
		} while (0 == (status & LAN91CXX_INTERRUPT_ALLOC_INT)
			 && (--i > 0));
		if (i)
			packet = get_reg(cpd, LAN91CXX_PNR);
		else
			packet = 0xffff;
		db1_printf(">+allocated packet %04x\n", packet);

		packet = packet >> 8;
		if (packet & 0x80) {
			/* Hm.. Isn't this a dead end? */
			db1_printf("Allocation failed! Retrying...\n");
			continue;
		}
	} while (0);

	db4_printf(">+Tx packet allocated %x (previous %x)\n",
		   packet, cpd->txpacket);

	cpd->txpacket = packet;

	/* ############ assemble packet data ############ */

	/* prepare send */
	put_reg(cpd, LAN91CXX_PNR, packet);
	/* Note: Check FIFO state here before continuing? */
	put_reg(cpd, LAN91CXX_POINTER, LAN91CXX_POINTER_AUTO_INCR | 0x0000);
	/* Pointer is now set, and the proper bank is selected for */
	/* data writes. */

	/* Prepare header: */
	put_data(cpd, CYG_CPU_TO_LE16(0));	/* reserve space for status word */
	/* packet length (includes status, byte-count and control shorts) */
	put_data(cpd, CYG_CPU_TO_LE16(0x7FE & (plen + 6)));	/* Always even, always < 15xx(dec) */

	/* Put data into buffer */
	odd = 0;
	n = m;
	while (n) {
		sdata = (unsigned short *)n->m_data;
		len = n->m_len;

		CYG_ASSERT(sdata, "!No sg data pointer here");

		/* start on an odd offset?
		 * If last byte also (1byte mbuf with different pointer should not occur)
		 * let following code handle it
		 */
		if ( ((unsigned int)sdata & 1) && (len>1) ){
			put_data8(cpd,*(unsigned char *)sdata);
			sdata = (unsigned short *)((unsigned int)sdata + 1);
			odd = ~odd;
			len--;
		}

		/* speed up copying a bit, never copy last word */
		while(len >= 17){
			put_data(cpd, *(sdata));
			put_data(cpd, *(sdata+1));
			put_data(cpd, *(sdata+2));
			put_data(cpd, *(sdata+3));
			put_data(cpd, *(sdata+4));
			put_data(cpd, *(sdata+5));
			put_data(cpd, *(sdata+6));
			put_data(cpd, *(sdata+7));
			sdata += 8;
			len -= 16;
		}

		/* copy word wise, skip last word */
		while (len >= 3) {
			put_data(cpd, *sdata++);
			len -= sizeof(*sdata);
		}

		/* one or two bytes left to put into fifo */
		if ( len > 1 ){
			/* the last 2bytes */
			if ( !odd || n->m_next ){
				put_data(cpd, *sdata++);
				len -= sizeof(*sdata);
			}else{
				/* write next byte, mark that we are not at an odd offset any more,
				 * remaining byte will be written outside while together with control byte.
				 */
				put_data8(cpd,*(unsigned char *)sdata);
				sdata = (unsigned short *)((unsigned int)sdata + 1);
				odd = 0;
				len--;
				/*break;*/
			}
		}else if ( (len>0) && (n->m_next) ){
			/* one byte left to write, and more bytes is comming in next mbuf */
			put_data8(cpd,*(unsigned char *)sdata);
			odd = ~odd;
		}

		n = n->m_next;
	}

	/* Lay down the control short unconditionally at the end. */
	/* (or it might use random memory contents) */
	control = 0;
	if ( len > 0 ){
		if ( !odd ) {
			/* Need to set ODD flag and insert the data */
			unsigned char onebyte = *(unsigned char *)sdata;
			control = onebyte;
			control |= LAN91CXX_CONTROLBYTE_ODD;
		}else{
			put_data8(cpd,*(unsigned char *)sdata);
		}
	}
	control |= LAN91CXX_CONTROLBYTE_CRC;	/* Just in case... */
	put_data(cpd, CYG_CPU_TO_LE16(control));

	m_freem(m);
	CYG_ASSERT(sdata, "!No sg data pointer outside");

	/* ############ start transmit ############ */

	/* Ack TX empty int and unmask it. */
	ints = get_reg(cpd, LAN91CXX_INTERRUPT) & 0xff00;
	put_reg(cpd, LAN91CXX_INTERRUPT,
		ints | LAN91CXX_INTERRUPT_TX_EMPTY_INT);
	put_reg(cpd, LAN91CXX_INTERRUPT, ints | LAN91CXX_INTERRUPT_TX_INT_M);	/* notify on error only (Autorelease) */

	/* Enqueue the packet */
	put_reg(cpd, LAN91CXX_MMU_COMMAND, LAN91CXX_MMU_enq_packet);

	ints = get_reg(cpd, LAN91CXX_INTERRUPT);
	db1_printf(">END: ints at TX: %04x\n", ints);
	dbg_prefix = "";
}

void smc91111_txDaemon(void *arg)
{
	struct lan91cxx_priv_data *cpd = arg;
	struct ifnet *ifp = &cpd->arpcom.ac_if;
	struct mbuf *m;
	rtems_event_set events;
	DEBUG_FUNCTION();

	for (;;) {
		/*
		 * Wait for packet
		 */

		rtems_bsdnet_event_receive
		    (SMC91111_START_TRANSMIT_EVENT,
		     RTEMS_EVENT_ANY | RTEMS_WAIT, RTEMS_NO_TIMEOUT, &events);

		/*IF_DEQUEUE (&ifp->if_snd, m);
		   if (m) {
		   sendpacket (ifp, m);
		   } */

		for (;;) {
			IF_DEQUEUE(&ifp->if_snd, m);
			if (!m)
				break;
			sendpacket(ifp, m);
		}
		ifp->if_flags &= ~IFF_OACTIVE;

	}

}

/* start transmit */
static void smc91111_start(struct ifnet *ifp)
{
	struct lan91cxx_priv_data *cpd = ifp->if_softc;

	if ((ifp->if_flags & (IFF_RUNNING | IFF_OACTIVE)) != IFF_RUNNING)
		return;

	rtems_event_send(cpd->txDaemonTid, START_TRANSMIT_EVENT);
	ifp->if_flags |= IFF_OACTIVE;

}

/* called after a tx error interrupt, freet the packet */
static void lan91cxx_finish_sent(struct lan91cxx_priv_data *cpd)
{
	unsigned short packet, tcr;
	int saved_packet;

	DEBUG_FUNCTION();

	INCR_STAT(cpd, tx_complete);

	saved_packet = get_reg(cpd, LAN91CXX_PNR);

	/* Ack and mask TX interrupt set */
	/*ints = get_reg(cpd, LAN91CXX_INTERRUPT) & 0xff00;
	   ints |= LAN91CXX_INTERRUPT_TX_SET_ACK;
	   ints &= ~LAN91CXX_INTERRUPT_TX_SET_M;
	   put_reg(cpd, LAN91CXX_INTERRUPT, ints); */

	/* Get number of completed packet and read the status word */
	packet = get_reg(cpd, LAN91CXX_FIFO_PORTS);
	db1_printf("%s:START: fifo %04x \n", __FUNCTION__, packet);

	{
		unsigned short reg;

		reg = get_reg(cpd, LAN91CXX_EPH_STATUS);

		/* Covering each bit in turn... */
		if (reg & LAN91CXX_STATUS_TX_UNRN)
			INCR_STAT(cpd, tx_underrun);
		if (reg & LAN91CXX_STATUS_LOST_CARR)
			INCR_STAT(cpd, tx_carrier_loss);
		if (reg & LAN91CXX_STATUS_LATCOL)
			INCR_STAT(cpd, tx_late_collisions);
		if (reg & LAN91CXX_STATUS_TX_DEFR)
			INCR_STAT(cpd, tx_deferred);
		if (reg & LAN91CXX_STATUS_SQET)
			INCR_STAT(cpd, tx_sqetesterrors);
		if (reg & LAN91CXX_STATUS_16COL)
			INCR_STAT(cpd, tx_max_collisions);
		if (reg & LAN91CXX_STATUS_MUL_COL)
			INCR_STAT(cpd, tx_mult_collisions);
		if (reg & LAN91CXX_STATUS_SNGL_COL)
			INCR_STAT(cpd, tx_single_collisions);
		if (reg & LAN91CXX_STATUS_TX_SUC)
			INCR_STAT(cpd, tx_good);

		cpd->stats.tx_total_collisions =
		    cpd->stats.tx_late_collisions +
		    cpd->stats.tx_max_collisions +
		    cpd->stats.tx_mult_collisions +
		    cpd->stats.tx_single_collisions;

		/* We do not need to look in the Counter Register (LAN91CXX_COUNTER)
		   because it just mimics the info we already have above. */
	}

	/* We do not really care about Tx failure.  Ethernet is not a reliable
	   medium.  But we do care about the TX engine stopping. */
	tcr = get_reg(cpd, LAN91CXX_TCR);
	if (0 == (LAN91CXX_TCR_TXENA & tcr)) {
		db1_printf("%s: ENGINE RESTART: tcr %x \n", __FUNCTION__, tcr);
		tcr |= LAN91CXX_TCR_TXENA;
		put_reg(cpd, LAN91CXX_TCR, tcr);
	}

	packet &= 0xff;

	/* and then free the packet */
	put_reg(cpd, LAN91CXX_PNR, cpd->txpacket);
	put_reg(cpd, LAN91CXX_MMU_COMMAND, LAN91CXX_MMU_rel_packet);

	while (get_reg(cpd, LAN91CXX_MMU_COMMAND) & LAN91CXX_MMU_COMMAND_BUSY) ;
	/* Don't change Packet Number Reg until busy bit is cleared */
	/* Per LAN91C111 Spec, Page 50 */
	put_reg(cpd, LAN91CXX_PNR, saved_packet);

}

/* \ ------------- Helpers ------------- \ */

/*
 * Show interface statistics
 */
static void smc91111_stats(struct lan91cxx_priv_data *priv)
{
	printf("tx_good             :%-8d\n", priv->stats.tx_good);
	printf("tx_max_collisions   :%-8d\n", priv->stats.tx_max_collisions);
	printf("tx_late_collisions  :%-8d\n", priv->stats.tx_late_collisions);
	printf("tx_underrun         :%-8d\n", priv->stats.tx_underrun);
	printf("tx_carrier_loss     :%-8d\n", priv->stats.tx_carrier_loss);
	printf("tx_deferred         :%-8d\n", priv->stats.tx_deferred);
	printf("tx_sqetesterrors    :%-8d\n", priv->stats.tx_sqetesterrors);
	printf("tx_single_collisions:%-8d\n", priv->stats.tx_single_collisions);
	printf("tx_mult_collisions  :%-8d\n", priv->stats.tx_mult_collisions);
	printf("tx_total_collisions :%-8d\n", priv->stats.tx_total_collisions);
	printf("rx_good             :%-8d\n", priv->stats.rx_good);
	printf("rx_crc_errors       :%-8d\n", priv->stats.rx_crc_errors);
	printf("rx_align_errors     :%-8d\n", priv->stats.rx_align_errors);
	printf("rx_resource_errors  :%-8d\n", priv->stats.rx_resource_errors);
	printf("rx_overrun_errors   :%-8d\n", priv->stats.rx_overrun_errors);
	printf("rx_collisions       :%-8d\n", priv->stats.rx_collisions);
	printf("rx_short_frames     :%-8d\n", priv->stats.rx_short_frames);
	printf("rx_too_long_frames  :%-8d\n", priv->stats.rx_too_long_frames);
	printf("rx_symbol_errors    :%-8d\n", priv->stats.rx_symbol_errors);
	printf("interrupts          :%-8d\n", priv->stats.interrupts);
	printf("rx_count            :%-8d\n", priv->stats.rx_count);
	printf("rx_deliver          :%-8d\n", priv->stats.rx_deliver);
	printf("rx_resource         :%-8d\n", priv->stats.rx_resource);
	printf("rx_restart          :%-8d\n", priv->stats.rx_restart);
	printf("tx_count            :%-8d\n", priv->stats.tx_count);
	printf("tx_complete         :%-8d\n", priv->stats.tx_complete);
	printf("tx_dropped          :%-8d\n", priv->stats.tx_dropped);
}

/*
 * Driver ioctl handler
 */
static int smc91111_ioctl(struct ifnet *ifp, ioctl_command_t command, caddr_t data)
{
	struct lan91cxx_priv_data *cpd = ifp->if_softc;
	int error = 0;
	DEBUG_FUNCTION();

	switch (command) {
	case SIOCGIFADDR:
	case SIOCSIFADDR:
		db_printf("SIOCSIFADDR\n");
		ether_ioctl(ifp, command, data);
		break;

	case SIOCSIFFLAGS:
		db_printf("SIOCSIFFLAGS\n");
		switch (ifp->if_flags & (IFF_UP | IFF_RUNNING)) {
		case IFF_RUNNING:
			smc91111_stop(cpd);
			break;

		case IFF_UP:
			smc91111_init(cpd);
			break;

		case IFF_UP | IFF_RUNNING:
			smc91111_stop(cpd);
			smc91111_init(cpd);
			break;

		default:
			break;
		}
		break;

	case SIO_RTEMS_SHOW_STATS:
		db_printf("SIO_RTEMS_SHOW_STATS\n");
		smc91111_stats(cpd);
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
 * Attach an SMC91111 driver to the system
 */
int _rtems_smc91111_driver_attach (struct rtems_bsdnet_ifconfig *config,
                                  struct scmv91111_configuration * chip)
{
	struct ifnet *ifp;
	struct lan91cxx_priv_data *cpd;
	int unitNumber;
	char *unitName;
	int mtu;
	DEBUG_FUNCTION();

	/* parse driver name */
	if ((unitNumber = rtems_bsdnet_parse_driver_name(config, &unitName)) < 0) {
		db_printf("Unitnumber < 0: %d\n", unitNumber);
		return 0;
	}

	db_printf("Unitnumber: %d, baseaddr: 0x%p\n", unitNumber, chip->baseaddr);

	cpd = &smc91111;
	ifp = &cpd->arpcom.ac_if;
	memset(cpd, 0, sizeof(*cpd));

	cpd->config = *chip;
	cpd->base = chip->baseaddr;

	if (smc_probe(cpd)) {
		return 0;
	}

	if (config->hardware_address) {
		memcpy(cpd->arpcom.ac_enaddr, config->hardware_address, ETHER_ADDR_LEN);
	} else {
#ifdef SMC91111_ENADDR_IS_SETUP
        /* The address was put in the chip at reset time.  Retrieve it. */
        int i;
        for (i = 0; i < sizeof(cpd->enaddr); i += 2) {
            unsigned short r = get_reg(cpd, LAN91CXX_IA01 + i / 2);
            cpd->arpcom.ac_enaddr[i] = r;
            cpd->arpcom.ac_enaddr[i+1] = r >> 8;
        }
#else
		/* dummy default address */
		cpd->arpcom.ac_enaddr[0] = 0x12;
		cpd->arpcom.ac_enaddr[1] = 0x13;
		cpd->arpcom.ac_enaddr[2] = 0x14;
		cpd->arpcom.ac_enaddr[3] = 0x15;
		cpd->arpcom.ac_enaddr[4] = 0x16;
		cpd->arpcom.ac_enaddr[5] = 0x17;
#endif
	}

	cpd->enaddr[0] = cpd->arpcom.ac_enaddr[0];
	cpd->enaddr[1] = cpd->arpcom.ac_enaddr[1];
	cpd->enaddr[2] = cpd->arpcom.ac_enaddr[2];
	cpd->enaddr[3] = cpd->arpcom.ac_enaddr[3];
	cpd->enaddr[4] = cpd->arpcom.ac_enaddr[4];
	cpd->enaddr[5] = cpd->arpcom.ac_enaddr[5];
	cpd->rpc_cur_mode =
	    LAN91CXX_RPCR_LEDA_RX | LAN91CXX_RPCR_LEDB_LINK |
	    LAN91CXX_RPCR_ANEG;

	if (config->mtu)
		mtu = config->mtu;
	else
		mtu = ETHERMTU;

	/*
	 * Set up network interface values
	 */
	ifp->if_softc = cpd;
	ifp->if_unit = unitNumber;
	ifp->if_name = unitName;
	ifp->if_mtu = mtu;
	ifp->if_init = smc91111_init;
	ifp->if_ioctl = smc91111_ioctl;
	ifp->if_start = smc91111_start;
	ifp->if_output = ether_output;
	ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX;
	if (ifp->if_snd.ifq_maxlen == 0)
		ifp->if_snd.ifq_maxlen = ifqmaxlen;

	/*
	 * Attach the interface
	 */
	if_attach(ifp);
	ether_ifattach(ifp);

#ifdef DEBUG
	printf("SMC91111 : driver has been attached\n");
#endif

	return 1;
};

/* \ ------------- Initialization ------------- \ */

/*
 * Initialize and start the device
 */
static void smc91111_init(void *arg)
{
	struct lan91cxx_priv_data *cpd = arg;
	struct ifnet *ifp = &cpd->arpcom.ac_if;
	DEBUG_FUNCTION();

	if (cpd->txDaemonTid == 0) {

		lan91cxx_hardware_init(cpd);
		lan91cxx_start(ifp);

		cpd->rxDaemonTid = rtems_bsdnet_newproc("DCrx", 4096,
							smc91111_rxDaemon, cpd);
		cpd->txDaemonTid =
		    rtems_bsdnet_newproc("DCtx", 4096, smc91111_txDaemon, cpd);
	} else {
		lan91cxx_start(ifp);
	}

	/*
	 * Tell the world that we're running.
	 */
	ifp->if_flags |= IFF_RUNNING;
}

/*
 * Stop the device
 */
static void smc91111_stop(struct lan91cxx_priv_data *cpd)
{
	struct ifnet *ifp = &cpd->arpcom.ac_if;
	DEBUG_FUNCTION();

	ifp->if_flags &= ~IFF_RUNNING;

	/* Reset chip */
	put_reg(cpd, LAN91CXX_RCR, LAN91CXX_RCR_SOFT_RST);
	put_reg(cpd, LAN91CXX_RCR, 0);
	cpd->txbusy = cpd->within_send = 0;

}

int lan91cxx_hardware_init(struct lan91cxx_priv_data *cpd)
{
	unsigned short val;
	int i, rc;

	DEBUG_FUNCTION();

	cpd->txbusy = cpd->within_send = 0;

	/* install interrupt vector */
#ifdef BSP_FEATURE_IRQ_EXTENSION
	{
		rtems_status_code sc = RTEMS_SUCCESSFUL;

		sc = rtems_interrupt_handler_install(
			cpd->config.vector,
			cpd->config.info,
			cpd->config.options,
			cpd->config.interrupt_wrapper,
			cpd->config.arg
		);
		if (sc != RTEMS_SUCCESSFUL) {
			printf("rtems_interrupt_handler_install returned %d.\n", sc);
			return 0;
		}
	}
#else
	db_printf("Install lan91cxx isr at vec/irq %d\n", cpd->config.vector);
	rc = rtems_interrupt_handler_install(cpd->config.vector, "smc91cxx",
		RTEMS_INTERRUPT_SHARED, lan91cxx_interrupt_handler, cpd);
	if (rc != RTEMS_SUCCESSFUL)
		return 0;
#endif

	/* Reset chip */
	put_reg(cpd, LAN91CXX_RCR, LAN91CXX_RCR_SOFT_RST);
	put_reg(cpd, LAN91CXX_RCR, 0);
	HAL_DELAY_US(100000);
	put_reg(cpd, LAN91CXX_CONFIG, 0x9000);
	put_reg(cpd, LAN91CXX_RCR, 0);
	put_reg(cpd, LAN91CXX_TCR, 0);
	put_reg(cpd, LAN91CXX_MMU_COMMAND, LAN91CXX_MMU_reset_mmu);

	val = get_reg(cpd, LAN91CXX_EPH_STATUS);
	/* probe chip by reading the signature in BS register */
	val = get_banksel(cpd);
	db9_printf("LAN91CXX - supposed BankReg @ %x = %04x\n",
		   (unsigned int)(cpd->base + LAN91CXX_BS), val);

	if ((0xff00 & val) != 0x3300) {
		printf("No 91Cxx signature");
		printf("smsc_lan91cxx_init: No 91Cxx signature found\n");
		return 0;
	}

	val = get_reg(cpd, LAN91CXX_REVISION);

	db9_printf("LAN91CXX - type: %01x, rev: %01x\n",
		   (val >> 4) & 0xf, val & 0xf);

	/* Set RevA flag for LAN91C111 so we can cope with the odd-bit bug. */
	cpd->c111_reva = (val == 0x3390); /* 90=A, 91=B, 92=C */

	/* The controller may provide a function used to set up the ESA */
	if (cpd->config_enaddr)
		(*cpd->config_enaddr) (cpd);

	db9_printf("LAN91CXX - status: %04x\n", val);
	/* Use statically configured ESA from the private data */
	db9_printf
	    ("LAN91CXX - static ESA: %02x:%02x:%02x:%02x:%02x:%02x\n",
	     cpd->enaddr[0], cpd->enaddr[1], cpd->enaddr[2],
	     cpd->enaddr[3], cpd->enaddr[4], cpd->enaddr[5]);
	/* Set up hardware address */
	for (i = 0; i < sizeof(cpd->enaddr); i += 2)
		put_reg(cpd, LAN91CXX_IA01 + i / 2,
			cpd->enaddr[i] | (cpd->enaddr[i + 1] << 8));

	return 1;
}

/*
  This function is called to "start up" the interface.  It may be called
  multiple times, even when the hardware is already running.  It will be
  called whenever something "hardware oriented" changes and should leave
  the hardware ready to send/receive packets.
*/
static void lan91cxx_start(struct ifnet *ifp)
{
	struct lan91cxx_priv_data *cpd = ifp->if_softc;

	uint16_t intr;
	uint16_t phy_ctl;
	int delay;
	DEBUG_FUNCTION();

	HAL_DELAY_US(100000);

	/* 91C111 Errata. Internal PHY comes up disabled. Must enable here. */
	phy_ctl = lan91cxx_read_phy(cpd, 0, LAN91CXX_PHY_CTRL);
	phy_ctl &= ~LAN91CXX_PHY_CTRL_MII_DIS;
	lan91cxx_write_phy(cpd, 0, LAN91CXX_PHY_CTRL, phy_ctl);

	/* Start auto-negotiation */
	put_reg(cpd, LAN91CXX_RPCR,
		LAN91CXX_RPCR_LEDA_RX | LAN91CXX_RPCR_LEDB_LINK |
		LAN91CXX_RPCR_ANEG);
	cpd->rpc_cur_mode =
	    LAN91CXX_RPCR_LEDA_RX | LAN91CXX_RPCR_LEDB_LINK |
	    LAN91CXX_RPCR_ANEG;

	/* wait for auto-negotiation to finish. */
	/* give it ~5 seconds before giving up (no cable?) */
	delay = 50;
	while (!(lan91cxx_read_phy(cpd, 0, LAN91CXX_PHY_STAT) & 0x20)) {
		if (--delay <= 0) {
			printf("Timeout autonegotiation\n");
			break;
		}
		HAL_DELAY_US(100000);
	}

	put_reg(cpd, LAN91CXX_MMU_COMMAND, LAN91CXX_MMU_reset_mmu);

	put_reg(cpd, LAN91CXX_INTERRUPT, 0);	/* disable interrupts */
	intr = get_reg(cpd, LAN91CXX_INTERRUPT);
	put_reg(cpd, LAN91CXX_INTERRUPT, intr &	/* ack old interrupts */
		(LAN91CXX_INTERRUPT_TX_INT |
		 LAN91CXX_INTERRUPT_TX_EMPTY_INT |
		 LAN91CXX_INTERRUPT_RX_OVRN_INT | LAN91CXX_INTERRUPT_ERCV_INT));
	put_reg(cpd, LAN91CXX_RCR,
		LAN91CXX_RCR_STRIP_CRC | LAN91CXX_RCR_RXEN |
		LAN91CXX_RCR_ALMUL);
	put_reg(cpd, LAN91CXX_TCR, LAN91CXX_TCR_TXENA | LAN91CXX_TCR_PAD_EN);
	put_reg(cpd, LAN91CXX_CONTROL, LAN91CXX_CONTROL_AUTO_RELEASE);	/*  */
	put_reg(cpd, LAN91CXX_INTERRUPT,	/* enable interrupts */
		LAN91CXX_INTERRUPT_RCV_INT_M);

	if ((0
#ifdef ETH_DRV_FLAGS_PROMISC_MODE
	     != (flags & ETH_DRV_FLAGS_PROMISC_MODE)
#endif
	    ) || (ifp->if_flags & IFF_PROMISC)
	    ) {
		/* Then we select promiscuous mode. */
		unsigned short rcr;
		rcr = get_reg(cpd, LAN91CXX_RCR);
		rcr |= LAN91CXX_RCR_PRMS;
		put_reg(cpd, LAN91CXX_RCR, rcr);
	}
}

/* \ ------------- Probe ------------- \ */

static const char *chip_ids[15] = {
	NULL, NULL, NULL,
	/* 3 */ "SMC91C90/91C92",
	/* 4 */ "SMC91C94",
	/* 5 */ "SMC91C95",
	/* 6 */ "SMC91C96",
	/* 7 */ "SMC91C100",
	/* 8 */ "SMC91C100FD",
	/* 9 */ "SMC91C11xFD",
	NULL, NULL,
	NULL, NULL, NULL
};

static int smc_probe(struct lan91cxx_priv_data *cpd)
{
	unsigned short bank;
	unsigned short revision_register;

	DEBUG_FUNCTION();

	/* First, see if the high byte is 0x33 */
	HAL_READ_UINT16(cpd->base + (LAN91CXX_BS), bank);
	bank = CYG_LE16_TO_CPU(bank);
	if ((bank & 0xFF00) != 0x3300) {
		db_printf("<1>Smc probe bank check 1 failed.\n");
		return -ENODEV;
	}
	/* The above MIGHT indicate a device, but I need to write to further
	   test this.  */
	HAL_WRITE_UINT16(cpd->base + (LAN91CXX_BS), CYG_CPU_TO_LE16(0 >> 3));
	HAL_READ_UINT16(cpd->base + (LAN91CXX_BS), bank);
	bank = CYG_LE16_TO_CPU(bank);
	if ((bank & 0xFF00) != 0x3300) {
		db_printf("<1>Smc probe bank check 2 failed.\n");
		return -ENODEV;
	}
#if SMC_DEBUG > 3
	{
		unsigned short bank16, bank16_0, bank16_1;
		HAL_READ_UINT16(cpd->base + (LAN91CXX_BS), bank16);
		bank = CYG_LE16_TO_CPU(bank);
		HAL_READ_UINT8(cpd->base + (LAN91CXX_BS), bank16_0);
		HAL_READ_UINT8(cpd->base + (LAN91CXX_BS + 1), bank16_1);

		db_printf
		    ("smc_probe:Bank read as a 16 bit value:0x%04x\n", bank16);
		db_printf
		    ("smc_probe:Bank read as an 8 bit value:0x%02x\n",
		     bank16_0);
		db_printf
		    ("smc_probe:Bank + 1 read as an 8 bit value:0x%02x\n",
		     bank16_1);
	}
#endif

	/*  check if the revision register is something that I recognize.
	   These might need to be added to later, as future revisions
	   could be added.  */
	revision_register = get_reg(cpd, LAN91CXX_REVISION);
	if (!chip_ids[(revision_register >> 4) & 0xF]) {
		/* I don't recognize this chip, so... */
		db_printf
		    ("smc_probe: IO %" PRIxPTR ": Unrecognized revision register:"
		     " %x, Contact author. \n", cpd->base,
		     revision_register);

		return -ENODEV;
	}
	db_printf("LAN91CXX(0x%x) - type: %s, rev: %01x\n",
		  revision_register,
		  chip_ids[(revision_register >> 4) & 0xF],
		  revision_register & 0xf);

	/* Set RevA flag for LAN91C111 so we can cope with the odd-bit bug. */
	if (revision_register == 0x3390) {
		db_printf("!Revision A\n");
	}

	return 0;
}

#if 0
/* \ ------------- PHY read/write ------------- \ */
/*Sets the PHY to a configuration as determined by the user*/
static int lan91cxx_phy_fixed(struct lan91cxx_priv_data *cpd)
{
	int my_fixed_caps;
	int cfg1;

	DEBUG_FUNCTION();
	db4_printf("lan91cxx_phy_fixed: full duplex: %d, speed: %d\n",
		   cpd->config.ctl_rfduplx, cpd->config.ctl_rspeed);

	/* Enter Link Disable state */
	cfg1 = lan91cxx_read_phy(cpd, 0, LAN91CXX_PHY_CONFIG1);
	cfg1 |= PHY_CFG1_LNKDIS;
	lan91cxx_write_phy(cpd, 0, LAN91CXX_PHY_CONFIG1, cfg1);

	/* Set our fixed capabilities, Disable auto-negotiation */
	my_fixed_caps = 0;

	if (cpd->config.ctl_rfduplx)
		my_fixed_caps |= LAN91CXX_PHY_CTRL_DPLX;

	if (cpd->config.ctl_rspeed == 100)
		my_fixed_caps |= LAN91CXX_PHY_CTRL_SPEED;

	/* Write capabilities to the phy control register */
	lan91cxx_write_phy(cpd, 0, LAN91CXX_PHY_CTRL, my_fixed_caps);

	/* Re-Configure the Receive/Phy Control register */
	put_reg(cpd, LAN91CXX_RPCR, cpd->rpc_cur_mode);

	return (1);
}
#endif

#if 0
/*Configures the specified PHY using Autonegotiation. */
static void lan91cxx_phy_configure(struct lan91cxx_priv_data *cpd)
{

	unsigned int phyaddr;
	unsigned int my_phy_caps;	/* My PHY capabilities */
	unsigned int my_ad_caps;	/* My Advertised capabilities */
	unsigned int status = 0;
	int failed = 0, delay;

	DEBUG_FUNCTION();

	/* Set the blocking flag */
	cpd->autoneg_active = 1;

	/* Get the detected phy address */
	phyaddr = cpd->phyaddr;

	/* Reset the PHY, setting all other bits to zero */
	lan91cxx_write_phy(cpd, 0, PHY_CNTL_REG, PHY_CNTL_RST);

	/* Wait for the reset to complete, or time out */
	delay = 50;
	while (delay--) {
		if (!(lan91cxx_read_phy(cpd, 0, PHY_CNTL_REG)
		      & PHY_CNTL_RST)) {
			break;
		}
		HAL_DELAY_US(100000);
	}

	if (delay < 1) {
		db_printf("smc91111:!PHY reset timed out\n");
		goto smc_phy_configure_exit;
	}

	/* Read PHY Register 18, Status Output */
	cpd->lastPhy18 = lan91cxx_read_phy(cpd, 0, PHY_INT_REG);

	/* Enable PHY Interrupts (for register 18) */
	/* Interrupts listed here are disabled */
	lan91cxx_write_phy(cpd, 0, PHY_MASK_REG,
			   PHY_INT_LOSSSYNC | PHY_INT_CWRD | PHY_INT_SSD
			   | PHY_INT_ESD | PHY_INT_RPOL | PHY_INT_JAB |
			   PHY_INT_SPDDET | PHY_INT_DPLXDET);

	/* Configure the Receive/Phy Control register */
	put_reg(cpd, LAN91CXX_RPCR, cpd->rpc_cur_mode);

	/* Copy our capabilities from PHY_STAT_REG to PHY_AD_REG */
	my_phy_caps = lan91cxx_read_phy(cpd, phyaddr, PHY_STAT_REG);
	my_ad_caps = PHY_AD_CSMA;	/* I am CSMA capable */

	if (my_phy_caps & PHY_STAT_CAP_T4)
		my_ad_caps |= PHY_AD_T4;

	if (my_phy_caps & PHY_STAT_CAP_TXF)
		my_ad_caps |= PHY_AD_TX_FDX;

	if (my_phy_caps & PHY_STAT_CAP_TXH)
		my_ad_caps |= PHY_AD_TX_HDX;

	if (my_phy_caps & PHY_STAT_CAP_TF)
		my_ad_caps |= PHY_AD_10_FDX;

	if (my_phy_caps & PHY_STAT_CAP_TH)
		my_ad_caps |= PHY_AD_10_HDX;

	/* Disable capabilities not selected by our user */
	if (cpd->config.ctl_rspeed != 100) {
		my_ad_caps &= ~(PHY_AD_T4 | PHY_AD_TX_FDX | PHY_AD_TX_HDX);
	}

	if (!cpd->config.ctl_rfduplx) {
		my_ad_caps &= ~(PHY_AD_TX_FDX | PHY_AD_10_FDX);
	}

	/* Update our Auto-Neg Advertisement Register */
	lan91cxx_write_phy(cpd, 0, PHY_AD_REG, my_ad_caps);

	db4_printf("smc91111:phy caps=%x\n", my_phy_caps);
	db4_printf("smc91111:phy advertised caps=%x\n", my_ad_caps);

	/* If the user requested no auto neg, then go set his request */
	if (!(cpd->config.ctl_autoneg)) {
		lan91cxx_phy_fixed(cpd);

		goto smc_phy_configure_exit;
	}

	/* Restart auto-negotiation process in order to advertise my caps */
	lan91cxx_write_phy(cpd, 0, PHY_CNTL_REG,
			   PHY_CNTL_ANEG_EN | PHY_CNTL_ANEG_RST);

	/* wait for auto-negotiation to finish. */
	/* give it ~5 seconds before giving up (no cable?) */
	delay = 50;
	while (!
	       ((status =
		 lan91cxx_read_phy(cpd, 0, LAN91CXX_PHY_STAT)) & 0x20)) {
		if (--delay <= 0) {
			printf("Timeout autonegotiation\n");
			failed = 1;
			break;
		}

		/* Restart auto-negotiation if remote fault */
		if (status & PHY_STAT_REM_FLT) {
			db_printf("smc91111:PHY remote fault detected\n");

			/* Restart auto-negotiation */
			db_printf("smc91111:PHY restarting auto-negotiation\n");
			lan91cxx_write_phy(cpd, 0, PHY_CNTL_REG,
					   PHY_CNTL_ANEG_EN |
					   PHY_CNTL_ANEG_RST |
					   PHY_CNTL_SPEED | PHY_CNTL_DPLX);
		}
		HAL_DELAY_US(100000);
	}

	/* Fail if we detected an auto-negotiate remote fault */
	if (status & PHY_STAT_REM_FLT) {
		db_printf("smc91111:PHY remote fault detected\n");
		failed = 1;
	}

	/* The smc_phy_interrupt() routine will be called to update lastPhy18 */

	/* Set our sysctl parameters to match auto-negotiation results */
	if (cpd->lastPhy18 & PHY_INT_SPDDET) {
		db_printf("smc91111:PHY 100BaseT\n");
		cpd->rpc_cur_mode |= LAN91CXX_RPCR_SPEED;
	} else {
		db_printf("smc91111:PHY 10BaseT\n");
		cpd->rpc_cur_mode &= ~LAN91CXX_RPCR_SPEED;
	}

	if (cpd->lastPhy18 & PHY_INT_DPLXDET) {
		db_printf("smc91111:PHY Full Duplex\n");
		cpd->rpc_cur_mode |= LAN91CXX_RPCR_DPLX;
	} else {
		db_printf("smc91111:PHY Half Duplex\n");
		cpd->rpc_cur_mode &= ~LAN91CXX_RPCR_DPLX;
	}

	/* Re-Configure the Receive/Phy Control register */
	put_reg(cpd, LAN91CXX_RPCR, cpd->rpc_cur_mode);

      smc_phy_configure_exit:

	/* Exit auto-negotiation */
	cpd->autoneg_active = 0;
}
#endif

static uint16_t
lan91cxx_read_phy(struct lan91cxx_priv_data *cpd, uint8_t phyaddr,
		  uint8_t phyreg)
{
	int i, mask, input_idx, clk_idx = 0;
	uint16_t mii_reg, value;
	uint8_t bits[64];

	/* 32 consecutive ones on MDO to establish sync */
	for (i = 0; i < 32; ++i)
		bits[clk_idx++] = LAN91CXX_MGMT_MDOE | LAN91CXX_MGMT_MDO;

	/* Start code <01> */
	bits[clk_idx++] = LAN91CXX_MGMT_MDOE;
	bits[clk_idx++] = LAN91CXX_MGMT_MDOE | LAN91CXX_MGMT_MDO;

	/* Read command <10> */
	bits[clk_idx++] = LAN91CXX_MGMT_MDOE | LAN91CXX_MGMT_MDO;
	bits[clk_idx++] = LAN91CXX_MGMT_MDOE;

	/* Output the PHY address, msb first */
	for (mask = 0x10; mask; mask >>= 1) {
		if (phyaddr & mask)
			bits[clk_idx++] =
			    LAN91CXX_MGMT_MDOE | LAN91CXX_MGMT_MDO;
		else
			bits[clk_idx++] = LAN91CXX_MGMT_MDOE;
	}

	/* Output the phy register number, msb first */
	for (mask = 0x10; mask; mask >>= 1) {
		if (phyreg & mask)
			bits[clk_idx++] =
			    LAN91CXX_MGMT_MDOE | LAN91CXX_MGMT_MDO;
		else
			bits[clk_idx++] = LAN91CXX_MGMT_MDOE;
	}

	/* Tristate and turnaround (1 bit times) */
	bits[clk_idx++] = 0;

	/* Input starts at this bit time */
	input_idx = clk_idx;

	/* Will input 16 bits */
	for (i = 0; i < 16; ++i)
		bits[clk_idx++] = 0;

	/* Final clock bit */
	bits[clk_idx++] = 0;

	/* Get the current MII register value */
	mii_reg = get_reg(cpd, LAN91CXX_MGMT);

	/* Turn off all MII Interface bits */
	mii_reg &= ~(LAN91CXX_MGMT_MDOE | LAN91CXX_MGMT_MCLK |
		     LAN91CXX_MGMT_MDI | LAN91CXX_MGMT_MDO);
	HAL_DELAY_US(50);

	/* Clock all 64 cycles */
	for (i = 0; i < sizeof(bits); ++i) {
		/* Clock Low - output data */
		put_reg(cpd, LAN91CXX_MGMT, mii_reg | bits[i]);
		HAL_DELAY_US(50);

		/* Clock Hi - input data */
		put_reg(cpd, LAN91CXX_MGMT,
			mii_reg | bits[i] | LAN91CXX_MGMT_MCLK);
		HAL_DELAY_US(50);

		bits[i] |= get_reg(cpd, LAN91CXX_MGMT) & LAN91CXX_MGMT_MDI;
	}

	/* Return to idle state */
	put_reg(cpd, LAN91CXX_MGMT, mii_reg);
	HAL_DELAY_US(50);

	/* Recover input data */
	for (value = 0, i = 0; i < 16; ++i) {
		value <<= 1;
		if (bits[input_idx++] & LAN91CXX_MGMT_MDI)
			value |= 1;
	}

	db16_printf("phy_read : %d : %04x\n", phyreg, value);
	return value;
}

static void
lan91cxx_write_phy(struct lan91cxx_priv_data *cpd, uint8_t phyaddr,
		   uint8_t phyreg, uint16_t value)
{
	int i, mask, clk_idx = 0;
	uint16_t mii_reg;
	uint8_t bits[65];

	/* 32 consecutive ones on MDO to establish sync */
	for (i = 0; i < 32; ++i)
		bits[clk_idx++] = LAN91CXX_MGMT_MDOE | LAN91CXX_MGMT_MDO;

	/* Start code <01> */
	bits[clk_idx++] = LAN91CXX_MGMT_MDOE;
	bits[clk_idx++] = LAN91CXX_MGMT_MDOE | LAN91CXX_MGMT_MDO;

	/* Write command <01> */
	bits[clk_idx++] = LAN91CXX_MGMT_MDOE;
	bits[clk_idx++] = LAN91CXX_MGMT_MDOE | LAN91CXX_MGMT_MDO;

	/* Output the PHY address, msb first */
	for (mask = 0x10; mask; mask >>= 1) {
		if (phyaddr & mask)
			bits[clk_idx++] =
			    LAN91CXX_MGMT_MDOE | LAN91CXX_MGMT_MDO;
		else
			bits[clk_idx++] = LAN91CXX_MGMT_MDOE;
	}

	/* Output the phy register number, msb first */
	for (mask = 0x10; mask; mask >>= 1) {
		if (phyreg & mask)
			bits[clk_idx++] =
			    LAN91CXX_MGMT_MDOE | LAN91CXX_MGMT_MDO;
		else
			bits[clk_idx++] = LAN91CXX_MGMT_MDOE;
	}

	/* Tristate and turnaround (2 bit times) */
	bits[clk_idx++] = 0;
	bits[clk_idx++] = 0;

	/* Write out 16 bits of data, msb first */
	for (mask = 0x8000; mask; mask >>= 1) {
		if (value & mask)
			bits[clk_idx++] =
			    LAN91CXX_MGMT_MDOE | LAN91CXX_MGMT_MDO;
		else
			bits[clk_idx++] = LAN91CXX_MGMT_MDOE;
	}

	/* Final clock bit (tristate) */
	bits[clk_idx++] = 0;

	/* Get the current MII register value */
	mii_reg = get_reg(cpd, LAN91CXX_MGMT);

	/* Turn off all MII Interface bits */
	mii_reg &= ~(LAN91CXX_MGMT_MDOE | LAN91CXX_MGMT_MCLK |
		     LAN91CXX_MGMT_MDI | LAN91CXX_MGMT_MDO);
	HAL_DELAY_US(50);

	/* Clock all cycles */
	for (i = 0; i < sizeof(bits); ++i) {
		/* Clock Low - output data */
		put_reg(cpd, LAN91CXX_MGMT, mii_reg | bits[i]);
		HAL_DELAY_US(50);

		/* Clock Hi - input data */
		put_reg(cpd, LAN91CXX_MGMT,
			mii_reg | bits[i] | LAN91CXX_MGMT_MCLK);
		HAL_DELAY_US(50);

/*	bits[i] |= get_reg(cpd, LAN91CXX_MGMT) & LAN91CXX_MGMT_MDI;*/
	}

	/* Return to idle state */
	put_reg(cpd, LAN91CXX_MGMT, mii_reg);
	HAL_DELAY_US(50);

	db16_printf("phy_write: %d : %04x\n", phyreg, value);
}

#if 0
void lan91cxx_print_bank(int bank){
	struct lan91cxx_priv_data *cpd = &smc91111;
	int regno;
	unsigned short regval[8];
	int i;

	if ( bank >= 4 )
		return;
	for(i=0; i<8; i++){
		regno=i+bank<<3;
		regval[i] = get_reg(cpd, regno);
	}
	printk("---- BANK %d ----\n\r",bank);
	for(i=0; i<8; i++){
		printk("0x%x: 0x%x\n\r",i,regval[i]);
	}

}
#endif

#endif

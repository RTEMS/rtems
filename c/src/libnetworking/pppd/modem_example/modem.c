/*
 *  Modem device driver for RTEMS
 *  Author: Tomasz Domin (dot@comarch.pl)
 *  Copyright (C) 1998 by ComArch SA
 *  Driver will use termios for character output and ppp specific out procecedures for network protocols
 */

#include <bsp.h>
#include <rtems.h>
#include <rtems/libio.h>
#include <sys/ttycom.h>
#include <unistd.h>
#include <sys/errno.h>
#include <rtems/rtems/event.h>
#include <rtems/rtems/tasks.h>
#include <mpc823.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/file.h>

#include <sys/kernel.h>
#include <sys/conf.h>
#include <net/if.h>
#include <net/ppp_defs.h>
#include <sys/fcntl.h>
#include "ppp.h"
#include <net/if_pppvar.h>
#include <net/if_ppp.h>
/* RTEMS specific */
rtems_id modem_task_id;
#include "16550.h"
#include <mpc823.h>
#define ESCAPE_P(c)	(sc->sc_asyncmap[(c) >> 5] & (1 << ((c) & 0x1F)))
#define M_IS_CLUSTER(m)	((m)->m_flags & M_EXT)

#define M_DATASTART(m)	\
	(M_IS_CLUSTER(m) ? (m)->m_ext.ext_buf : \
	    (m)->m_flags & M_PKTHDR ? (m)->m_pktdat : (m)->m_dat)

#define M_DATASIZE(m)	\
	(M_IS_CLUSTER(m) ? (m)->m_ext.ext_size : \
	    (m)->m_flags & M_PKTHDR ? MHLEN: MLEN)


struct rtems_termios_tty *modem_tty;
/*
 * RTEMS event used by interrupt handler to signal daemons.
 * This must *not* be the same event used by the KA9Q task synchronization.
 */
#define INTERRUPT_EVENT	      RTEMS_EVENT_1
#define START_TRANSMIT_EVENT	RTEMS_EVENT_2
#define PPP_LOWAT	100	/* Process more output when < LOWAT on queue */
#define	PPP_HIWAT	400	/* Don't start a new packet if HIWAT on que */

/*
 * RTEMS event used to start transmit daemon.
 * This must not be the same as INTERRUPT_EVENT.
 */

/*static struct modem_tty *tp=&modem_tp;*/
static rtems_id modem_rx_task,modem_tx_task;



/* internal FIFO buffers for input and output characters */
#define MODEM_BUFFER_LENGTH 4096
#define RTS_STOP_SIZE MODEM_BUFFER_LENGTH-128
#define RTS_START_SIZE 16

int xmt_start,xmt_len;
int rcv_start,rcv_len;

static unsigned char xmt_buf[MODEM_BUFFER_LENGTH];
static unsigned char rcv_buf[MODEM_BUFFER_LENGTH];

static volatile char _tx_stop = 0 ,_modem_cd=0;
struct ModemData
{
	int t_line;
	struct ppp_softc *t_sc;
	rtems_id pppsem;
	int offhook;
} ModemData;

int pppstart(struct tty *ignore);
static u_short fcstab[256] = {
	0x0000,	0x1189,	0x2312,	0x329b,	0x4624,	0x57ad,	0x6536,	0x74bf,
	0x8c48,	0x9dc1,	0xaf5a,	0xbed3,	0xca6c,	0xdbe5,	0xe97e,	0xf8f7,
	0x1081,	0x0108,	0x3393,	0x221a,	0x56a5,	0x472c,	0x75b7,	0x643e,
	0x9cc9,	0x8d40,	0xbfdb,	0xae52,	0xdaed,	0xcb64,	0xf9ff,	0xe876,
	0x2102,	0x308b,	0x0210,	0x1399,	0x6726,	0x76af,	0x4434,	0x55bd,
	0xad4a,	0xbcc3,	0x8e58,	0x9fd1,	0xeb6e,	0xfae7,	0xc87c,	0xd9f5,
	0x3183,	0x200a,	0x1291,	0x0318,	0x77a7,	0x662e,	0x54b5,	0x453c,
	0xbdcb,	0xac42,	0x9ed9,	0x8f50,	0xfbef,	0xea66,	0xd8fd,	0xc974,
	0x4204,	0x538d,	0x6116,	0x709f,	0x0420,	0x15a9,	0x2732,	0x36bb,
	0xce4c,	0xdfc5,	0xed5e,	0xfcd7,	0x8868,	0x99e1,	0xab7a,	0xbaf3,
	0x5285,	0x430c,	0x7197,	0x601e,	0x14a1,	0x0528,	0x37b3,	0x263a,
	0xdecd,	0xcf44,	0xfddf,	0xec56,	0x98e9,	0x8960,	0xbbfb,	0xaa72,
	0x6306,	0x728f,	0x4014,	0x519d,	0x2522,	0x34ab,	0x0630,	0x17b9,
	0xef4e,	0xfec7,	0xcc5c,	0xddd5,	0xa96a,	0xb8e3,	0x8a78,	0x9bf1,
	0x7387,	0x620e,	0x5095,	0x411c,	0x35a3,	0x242a,	0x16b1,	0x0738,
	0xffcf,	0xee46,	0xdcdd,	0xcd54,	0xb9eb,	0xa862,	0x9af9,	0x8b70,
	0x8408,	0x9581,	0xa71a,	0xb693,	0xc22c,	0xd3a5,	0xe13e,	0xf0b7,
	0x0840,	0x19c9,	0x2b52,	0x3adb,	0x4e64,	0x5fed,	0x6d76,	0x7cff,
	0x9489,	0x8500,	0xb79b,	0xa612,	0xd2ad,	0xc324,	0xf1bf,	0xe036,
	0x18c1,	0x0948,	0x3bd3,	0x2a5a,	0x5ee5,	0x4f6c,	0x7df7,	0x6c7e,
	0xa50a,	0xb483,	0x8618,	0x9791,	0xe32e,	0xf2a7,	0xc03c,	0xd1b5,
	0x2942,	0x38cb,	0x0a50,	0x1bd9,	0x6f66,	0x7eef,	0x4c74,	0x5dfd,
	0xb58b,	0xa402,	0x9699,	0x8710,	0xf3af,	0xe226,	0xd0bd,	0xc134,
	0x39c3,	0x284a,	0x1ad1,	0x0b58,	0x7fe7,	0x6e6e,	0x5cf5,	0x4d7c,
	0xc60c,	0xd785,	0xe51e,	0xf497,	0x8028,	0x91a1,	0xa33a,	0xb2b3,
	0x4a44,	0x5bcd,	0x6956,	0x78df,	0x0c60,	0x1de9,	0x2f72,	0x3efb,
	0xd68d,	0xc704,	0xf59f,	0xe416,	0x90a9,	0x8120,	0xb3bb,	0xa232,
	0x5ac5,	0x4b4c,	0x79d7,	0x685e,	0x1ce1,	0x0d68,	0x3ff3,	0x2e7a,
	0xe70e,	0xf687,	0xc41c,	0xd595,	0xa12a,	0xb0a3,	0x8238,	0x93b1,
	0x6b46,	0x7acf,	0x4854,	0x59dd,	0x2d62,	0x3ceb,	0x0e70,	0x1ff9,
	0xf78f,	0xe606,	0xd49d,	0xc514,	0xb1ab,	0xa022,	0x92b9,	0x8330,
	0x7bc7,	0x6a4e,	0x58d5,	0x495c,	0x3de3,	0x2c6a,	0x1ef1,	0x0f78
};


static u_short
pppfcs(fcs, cp, len)
    register u_short fcs;
    register u_char *cp;
    register int len;
{
    while (len--)
	fcs = PPP_FCS(fcs, *cp++);
    return (fcs);
}

static void
ppp_timeout(x)
    void *x;
{
    struct ppp_softc *sc = ModemData.t_sc;
    struct tty *tp = (struct tty *) sc->sc_devp;
    int s;

    sc->sc_flags &= ~SC_TIMEOUT;
    pppstart(tp);
}

static void
pppasyncrelinq(sc)
    struct ppp_softc *sc;
{
    int s;

    if (sc->sc_outm) {
	m_freem(sc->sc_outm);
	sc->sc_outm = NULL;
    }
    if (sc->sc_m) {
	m_freem(sc->sc_m);
	sc->sc_m = NULL;
    }
    if (sc->sc_flags & SC_TIMEOUT) {
	untimeout(ppp_timeout, (void *) sc);
	sc->sc_flags &= ~SC_TIMEOUT;
    }
}

/* Put data in input queue */
int
pppstart(struct tty *ignore)
{
    register struct ppp_softc *sc = ModemData.t_sc;

    /*
     * If there is stuff in the output queue, send it now.
     * We are being called in lieu of ttstart and must do what it would.
     */
	rtems_event_send(modem_tx_task,START_TRANSMIT_EVENT);
	sc->sc_if.if_flags  |= IFF_OACTIVE;
	
    /*
     * If the transmit queue has drained and the tty has not hung up
     * or been disconnected from the ppp unit, then tell if_ppp.c that
     * we need more output.
     */
/*    if (CCOUNT(&tp->t_outq) < PPP_LOWAT
	&& !((tp->t_state & TS_CARR_ON) == 0 && (tp->t_cflag & CLOCAL) == 0)
	&& sc != NULL && tp == (struct tty *) sc->sc_devp) {
	ppp_restart(sc);
    }
*/
    return 0;
}
int putc(unsigned char c)
{	
	int level;
	
	if (xmt_len<MODEM_BUFFER_LENGTH-1)
	{
		_CPU_ISR_Disable(level);
		xmt_buf[(xmt_start+xmt_len)%MODEM_BUFFER_LENGTH]=c;
		xmt_len++;
		_CPU_ISR_Enable(level);
		return 0;
	}
	return 1;
}

void  unputc(void)
{	
	int level;
	
	if (xmt_len==0)
		return;
	_CPU_ISR_Disable(level);
		xmt_len--;
	_CPU_ISR_Enable(level);
}

static void
pppasyncstart(sc)
    register struct ppp_softc *sc;
{
    /* Call pppstart to start output again if necessary. */
    pppstart(sc->sc_devp);

    /*
     * This timeout is needed for operation on a pseudo-tty,
     * because the pty code doesn't call pppstart after it has
     * drained the t_outq.
     */
/*    if (!idle && (sc->sc_flags & SC_TIMEOUT) == 0) {
	timeout(ppp_timeout, (void *) sc, 1);
	sc->sc_flags |= SC_TIMEOUT;
    }
*/

}


void modem_sendpacket()
{
  struct mbuf *l = NULL;
  rtems_unsigned16 status;
  int curr;
  register struct mbuf *m;
  register int len;
  register u_char *start, *stop, *cp;
  int n, ndone, done, idle;
  struct mbuf *m2;
  int s;
   register struct ppp_softc *sc=ModemData.t_sc;
  

   while (xmt_len < PPP_HIWAT) {
	/*
	 * See if we have an existing packet partly sent.
	 * If not, get a new packet and start sending it.
	 */
	m = sc->sc_outm;
	if (m == NULL) {
	    /*
	     * Get another packet to be sent.
	     
	     */
	    m = ppp_dequeue(sc);
	    if (m == NULL) 
	    {
			break;
		 }

	    /*
	     * The extra PPP_FLAG will start up a new packet, and thus
	     * will flush any accumulated garbage.  We do this whenever
	     * the line may have been idle for some time.
	     */
	    if (xmt_len == 0) {
	   	++sc->sc_stats.ppp_obytes;
			 putc(PPP_FLAG);
	    }

	    /* Calculate the FCS for the first mbuf's worth. */
	    sc->sc_outfcs = pppfcs(PPP_INITFCS, mtod(m, u_char *), m->m_len);
	    microtime(&sc->sc_if.if_lastchange);
	}

	for (;;) {
	    start = mtod(m, u_char *);
	    len = m->m_len;
	    stop = start + len;
	    while (len > 0) {
		/*
		 * Find out how many bytes in the string we can
		 * handle without doing something special.
		 */
		for (cp = start; cp < stop; cp++)
		    if (ESCAPE_P(*cp))
				break;
		n = cp - start;
		if (n) {
			register int i;
		    /* NetBSD (0.9 or later), 4.3-Reno or similar. */
			 for(i=0;i<n;i++)
			 	if (putc(start[i]))
			 		break;
		    ndone = i;
		    len -= ndone;
		    start += ndone;
		    sc->sc_stats.ppp_obytes += ndone;

		    if (ndone < n)
			break;	/* packet doesn't fit */
		}
		/*
		 * If there are characters left in the mbuf,
		 * the first one must be special.
		 * Put it out in a different form.
		 */
		if (len) {
		    if (putc(PPP_ESCAPE))
			break;
		    if (putc(*start ^ PPP_TRANS)) {
			 unputc();
			break;
		    }
		    sc->sc_stats.ppp_obytes += 2;
		    start++;
		    len--;
		}
	    }

	    /*
	     * If we didn't empty this mbuf, remember where we're up to.
	     * If we emptied the last mbuf, try to add the FCS and closing
	     * flag, and if we can't, leave sc_outm pointing to m, but with
	     * m->m_len == 0, to remind us to output the FCS and flag later.
	     */
	    done = len == 0;
	    if (done && m->m_next == NULL) {
		u_char *p, *q;
		int c;
		u_char endseq[8];

		/*
		 * We may have to escape the bytes in the FCS.
		 */
		p = endseq;
		c = ~sc->sc_outfcs & 0xFF;
		if (ESCAPE_P(c)) {
		    *p++ = PPP_ESCAPE;
		    *p++ = c ^ PPP_TRANS;
		} else
		    *p++ = c;
		c = (~sc->sc_outfcs >> 8) & 0xFF;
		if (ESCAPE_P(c)) {
		    *p++ = PPP_ESCAPE;
		    *p++ = c ^ PPP_TRANS;
		} else
		    *p++ = c;
		*p++ = PPP_FLAG;

		/*
		 * Try to output the FCS and flag.  If the bytes
		 * don't all fit, back out.
		 */
		for (q = endseq; q < p; ++q)
		    if (putc(*q)) {
			done = 0;
			for (; q > endseq; --q)
			    unputc();
			break;
		    }
		if (done)
		    sc->sc_stats.ppp_obytes += q - endseq;
	    }

	    if (!done) {
		/* remember where we got to */
		m->m_data = start;
		m->m_len = len;
		break;
	    }

	    /* Finished with this mbuf; free it and move on. */
	    MFREE(m, m2);
	    m = m2;
	    if (m == NULL) {
		/* Finished a packet */
		break;
	    }
	    sc->sc_outfcs = pppfcs(sc->sc_outfcs, mtod(m, u_char *), m->m_len);
	}

	/*
	 * If m == NULL, we have finished a packet.
	 * If m != NULL, we've either done as much work this time
	 * as we need to, or else we've filled up the output queue.
	 */
	sc->sc_outm = m;
	if (m)
	    break;
    }

}

static void
pppasyncctlp(sc)
    struct ppp_softc *sc;
{
	rtems_semaphore_release(ModemData.pppsem);
}


static unsigned paritytab[8] = {
    0x96696996, 0x69969669, 0x69969669, 0x96696996,
    0x69969669, 0x96696996, 0x96696996, 0x69969669
};
#define MAX_DUMP_BYTES	128

static void
pppdumpb(b, l)
    u_char *b;
    int l;
{
    char buf[3*MAX_DUMP_BYTES+4];
    char *bp = buf;
    static char digits[] = "0123456789abcdef";

    while (l--) {
	if (bp >= buf + sizeof(buf) - 3) {
	    *bp++ = '>';
	    break;
	}
	*bp++ = digits[*b >> 4]; /* convert byte to ascii hex */
	*bp++ = digits[*b++ & 0xf];
	*bp++ = ' ';
    }

    *bp = 0;

}

static void
ppplogchar(sc, c)
    struct ppp_softc *sc;
    int c;
{
    if (c >= 0)
	sc->sc_rawin[sc->sc_rawin_count++] = c;
    if (sc->sc_rawin_count >= sizeof(sc->sc_rawin)
	|| (c < 0 && sc->sc_rawin_count > 0)) {
/*	printf("ppp%d input: ", sc->sc_if.if_unit);
*/	pppdumpb(sc->sc_rawin, sc->sc_rawin_count);
	sc->sc_rawin_count = 0;
    }
}
static void
pppgetm(sc)
    register struct ppp_softc *sc;
{
    struct mbuf *m, **mp;
    int len;
    mp = &sc->sc_m;
    for (len = sc->sc_mru + PPP_HDRLEN + PPP_FCSLEN; len > 0; ){
	if ((m = *mp) == NULL) {
	    MGETHDR(m, M_DONTWAIT, MT_DATA);
	    if (m == NULL)
		break;
	    *mp = m;
	    MCLGET(m, M_DONTWAIT);
	}
	len -= M_DATASIZE(m);
	mp = &m->m_next;
    }
}


int
pppinput(c)
    int c;
  
{
    register struct ppp_softc *sc=ModemData.t_sc;
    struct mbuf *m;
    int ilen, s;

    if (sc == NULL )
	return 0;

    ++sc->sc_stats.ppp_ibytes;
/*
    if (c & TTY_FE) {*/
	/* framing error or overrun on this char - abort packet */
/*	if (sc->sc_flags & SC_DEBUG)
	    printf("ppp%d: bad char %x\n", sc->sc_if.if_unit, c);
	goto flush;
    }*/

    c &= 0xff;

    /*
     * Handle software flow control of output.
     */
/*    if (tp->t_iflag & IXON) {
	if (c == tp->t_cc[VSTOP] ) {
	    if ((tp->t_state & TS_TTSTOP) == 0) {
		tp->t_state |= TS_TTSTOP;
		sccppp_stop_transmission(tp);
	    }
	    return 0;
	}
	if (c == tp->t_cc[VSTART]  ) {
	    tp->t_state &= ~TS_TTSTOP;
		sccppp_start_transmission(tp);
	    return 0;
	}
    }
*/

    if (c & 0x80)
	sc->sc_flags |= SC_RCV_B7_1;
    else
	sc->sc_flags |= SC_RCV_B7_0;
    if (paritytab[c >> 5] & (1 << (c & 0x1F)))
	sc->sc_flags |= SC_RCV_ODDP;
    else
	sc->sc_flags |= SC_RCV_EVNP;


/*    if (sc->sc_flags & SC_LOG_RAWIN)*/
//	ppplogchar(sc, c);

    if (c == PPP_FLAG) {
	ilen = sc->sc_ilen;
	sc->sc_ilen = 0;

	if (sc->sc_rawin_count > 0) 
	    ppplogchar(sc, -1);

	/*
	 * If SC_ESCAPED is set, then we've seen the packet
	 * abort sequence 
	 */
	if (sc->sc_flags & (SC_FLUSH | SC_ESCAPED)
	    || (ilen > 0 && sc->sc_fcs != PPP_GOODFCS)) {
	    sc->sc_flags |= SC_PKTLOST;	/* note the dropped packet */
	    if ((sc->sc_flags & (SC_FLUSH | SC_ESCAPED)) == 0){
		if (sc->sc_flags & SC_DEBUG)
		    printf("ppp%d: bad fcs %x, pkt len %d\n",
			   sc->sc_if.if_unit, sc->sc_fcs, ilen);
		sc->sc_if.if_ierrors++;
		sc->sc_stats.ppp_ierrors++;
	    } else
		sc->sc_flags &= ~(SC_FLUSH | SC_ESCAPED);
	    return 0;
	}

	if (ilen < PPP_HDRLEN + PPP_FCSLEN) {
	    if (ilen) {
		if (sc->sc_flags & SC_DEBUG)
/*		    printf("ppp%d: too short (%d)\n", sc->sc_if.if_unit, ilen);
*/		sc->sc_if.if_ierrors++;
		sc->sc_stats.ppp_ierrors++;
		sc->sc_flags |= SC_PKTLOST;
		splx(s);
	    }
	    return 0;
	}

	/*
	 * Remove FCS trailer.  Somewhat painful...
	 */
	ilen -= 2;
	if (--sc->sc_mc->m_len == 0) {
	    for (m = sc->sc_m; m->m_next != sc->sc_mc; m = m->m_next)
		;
	    sc->sc_mc = m;
	}
	sc->sc_mc->m_len--;

	/* excise this mbuf chain */
	m = sc->sc_m;
	sc->sc_m = sc->sc_mc->m_next;
	sc->sc_mc->m_next = NULL;
	ppppktin(sc, m, sc->sc_flags & SC_PKTLOST);
	if (sc->sc_flags & SC_PKTLOST) {
	    sc->sc_flags &= ~SC_PKTLOST;
	}

	pppgetm(sc);
	return 0;
    }

    if (sc->sc_flags & SC_FLUSH) {
	if (sc->sc_flags & SC_LOG_FLUSH)
	    ppplogchar(sc, c);
	return 0;
    }

    if (c < 0x20 && (sc->sc_rasyncmap & (1 << c)))
	return 0;

    if (sc->sc_flags & SC_ESCAPED) {
	sc->sc_flags &= ~SC_ESCAPED;
	c ^= PPP_TRANS;
    } else if (c == PPP_ESCAPE) {
	sc->sc_flags |= SC_ESCAPED;
	return 0;
    }

    /*
     * Initialize buffer on first octet received.
     * First octet could be address or protocol (when compressing
     * address/control).
     * Second octet is control.
     * Third octet is first or second (when compressing protocol)
     * octet of protocol.
     * Fourth octet is second octet of protocol.
     */
    if (sc->sc_ilen == 0) {
	/* reset the first input mbuf */
	if (sc->sc_m == NULL) {
	    pppgetm(sc);
	    if (sc->sc_m == NULL) {
		if (sc->sc_flags & SC_DEBUG)
/*		    printf("ppp%d: no input mbufs!\n", sc->sc_if.if_unit);
*/		goto flush;
	    }
	}
	m = sc->sc_m;
	m->m_len = 0;
	m->m_data = M_DATASTART(sc->sc_m);
	sc->sc_mc = m;
	sc->sc_mp = mtod(m, char *);
	sc->sc_fcs = PPP_INITFCS;
	if (c != PPP_ALLSTATIONS) {
	    if (sc->sc_flags & SC_REJ_COMP_AC) {
		if (sc->sc_flags & SC_DEBUG)
/*		    printf("ppp%d: garbage received: 0x%x (need 0xFF)\n",
			   sc->sc_if.if_unit, c);*/
		goto flush;
	    }
	    *sc->sc_mp++ = PPP_ALLSTATIONS;
	    *sc->sc_mp++ = PPP_UI;
	    sc->sc_ilen += 2;
	    m->m_len += 2;
	}
    }
    if (sc->sc_ilen == 1 && c != PPP_UI) {
	if (sc->sc_flags & SC_DEBUG)
/*	    printf("ppp%d: missing UI (0x3), got 0x%x\n",
		   sc->sc_if.if_unit, c);
*/	goto flush;
    }
    if (sc->sc_ilen == 2 && (c & 1) == 1) {
	/* a compressed protocol */
	*sc->sc_mp++ = 0;
	sc->sc_ilen++;
	sc->sc_mc->m_len++;
    }
    if (sc->sc_ilen == 3 && (c & 1) == 0) {
	if (sc->sc_flags & SC_DEBUG)
/*	    printf("ppp%d: bad protocol %x\n", sc->sc_if.if_unit,
		   (sc->sc_mp[-1] << 8) + c);
*/	goto flush;
    }

    /* packet beyond configured mru? */
    if (++sc->sc_ilen > sc->sc_mru + PPP_HDRLEN + PPP_FCSLEN) {
	if (sc->sc_flags & SC_DEBUG)
/*	    printf("ppp%d: packet too big\n", sc->sc_if.if_unit);
*/	goto flush;
    }

    /* is this mbuf full? */
    m = sc->sc_mc;
    if (M_TRAILINGSPACE(m) <= 0) {
	if (m->m_next == NULL) {
	    pppgetm(sc);
	    if (m->m_next == NULL) {
		if (sc->sc_flags & SC_DEBUG)
/*		    printf("ppp%d: too few input mbufs!\n", sc->sc_if.if_unit);
*/		goto flush;
	    }
	}
	sc->sc_mc = m = m->m_next;
	m->m_len = 0;
	m->m_data = M_DATASTART(m);
	sc->sc_mp = mtod(m, char *);
    }

    ++m->m_len;
    *sc->sc_mp++ = c;
    sc->sc_fcs = PPP_FCS(sc->sc_fcs, c);
    return 0;

 flush:
    if (!(sc->sc_flags & SC_FLUSH)) {
	sc->sc_if.if_ierrors++;
	sc->sc_stats.ppp_ierrors++;
	sc->sc_flags |= SC_FLUSH;
	if (sc->sc_flags & SC_LOG_FLUSH)
	    ppplogchar(sc, c);
    }
    return 0;
}


void
modem_txDaemon (void *arg)
{
	rtems_event_set events;
	register int level,i,maxonce;
	while (1)
	{
		if (xmt_len==0) //if there is nothing then wait for an event
			rtems_event_receive(START_TRANSMIT_EVENT|INTERRUPT_EVENT,RTEMS_EVENT_ANY | RTEMS_WAIT, RTEMS_NO_TIMEOUT,&events);
		/* wait for transmit buffer to become empty */

		while(_tx_stop) //here we are wainting for transmission start
			rtems_event_receive(INTERRUPT_EVENT|START_TRANSMIT_EVENT,RTEMS_EVENT_ANY | RTEMS_WAIT, RTEMS_NO_TIMEOUT,&events);

		_CPU_ISR_Disable(level);
		if (*LSR & THRE) //if there is no transmission then send first bytes, if there is one they will go eitherway
		{
			maxonce=(xmt_len>14)?14:xmt_len;
			if (maxonce>0)
			{
				for (i=0;i<maxonce;i++)
				{
					*THR=xmt_buf[xmt_start];
					xmt_start=(xmt_start+1)%MODEM_BUFFER_LENGTH;
				}
				xmt_len-=maxonce;
			}
		}
		_CPU_ISR_Enable(level);
		if ((ModemData.t_line==PPPDISC))
		{
			rtems_bsdnet_semaphore_obtain();
			modem_sendpacket();
			rtems_bsdnet_semaphore_release();
		}

	}
}

static void modem_rxDaemon (void *arg)
{
	rtems_event_set events;
	unsigned char bufor_posr[MODEM_BUFFER_LENGTH];
	register unsigned char ch;
	int level,i,j;
	while(1)
	{
		/* wait for interrupt */		
		i=0;
		_CPU_ISR_Disable(level);
		while(rcv_len>0)
		{
				bufor_posr[i++]=rcv_buf[rcv_start];
				rcv_start=(rcv_start+1)%MODEM_BUFFER_LENGTH;
				rcv_len--;
		}
		_CPU_ISR_Enable(level);
		if (ModemData.t_line==PPPDISC)
		{
			rtems_bsdnet_semaphore_obtain();
			for(j=0;j<i;j++)
				pppinput(bufor_posr[j]);			
			rtems_bsdnet_semaphore_release();
		}
		else
			if (i!=0)rtems_termios_enqueue_raw_characters(modem_tty,bufor_posr,i);
		rtems_event_receive(INTERRUPT_EVENT,RTEMS_EVENT_ANY | RTEMS_WAIT, RTEMS_NO_TIMEOUT,&events);
	}
}
#define CLK_FREQ 1843200
void set_modem_speed(int speed)
{
  *LCR = (char)(DLAB);
  *DLL = (char)((int)(CLK_FREQ/speed/16.0+0.5) & 0xFF);
  *DLM = (char)(((int)(CLK_FREQ/speed/16.0+0.5) & 0xFF00) >> 8);
  *LCR = (char)(WL_8 ); /* 8 bitowe slowo */
}


static void
modem_init (int speed)  /* port is the SMC number (i.e. 1 or 2) */
{
	set_modem_speed(speed);
  /* Line control setup */

  *LCR = (char)(WL_8 ); /* 8 bit word */
  /* was NSB - was 2 is 1*/

  /* Interrupt setup */
  *IER = (char) 0x0f;		/* enable transmit, receive, modem stat int */
  
  /* FIFO setup */
  *FCR = (char)(FIFO_E | 0xc0);

  /* Modem control setup */
  *MCR = (char) RTS|DTR|OUT2;

  /* init tx_stop with CTS */
  _tx_stop = ( (*MDSR & CTS) ? 0 : 1);
}

void set_modem_dtr(int how)
{
	unsigned char ch;
	ch=*MCR;
	*MCR=(how)?(ch|DTR):(ch&(~DTR));	
}	
void modem_status()
{
	unsigned char status;
	status=*MDSR;

/*	printf("Modem Status %x ",status);
*/	if (status&CTS)
	{
		_tx_stop=0;
		rtems_event_send (modem_tx_task, START_TRANSMIT_EVENT|INTERRUPT_EVENT);
	}
	else
	{
		_tx_stop=1;
	}
	if (status&DCD)
		_modem_cd=1;
	else
		_modem_cd=0;	
}

static rtems_isr
modemInterruptHandler (rtems_vector_number v)
{
  register char a,i,ch;
  register int maxonce;
	static errcount = 0;
  for(;;)
  {
	  a=*IIR & (NIP | IID_MASK); /* read interrupt id register */
	  switch (a) {
		  case 0x04: case 0x0c:
		  /*
		   * Buffer received?
		   */
		while( (*LSR & DR) != 0)
		{
			ch=*RBR;
			rcv_buf[(rcv_start+rcv_len)%MODEM_BUFFER_LENGTH]=ch;
			rcv_len++;
		}
	    rtems_event_send (modem_rx_task, INTERRUPT_EVENT);
	    break;
 	  case 0x02:
		  /*
		   * Buffer transmitted ?
		   */
		if (*LSR & THRE) //if there is no transmission (and it shouldn't be) then send some bytes
		{
			maxonce=(xmt_len>14)?14:xmt_len;
			if (maxonce>0)
			{
				for (i=0;i<maxonce;i++)
				{
					*THR=xmt_buf[xmt_start];
					xmt_start=(xmt_start+1)%MODEM_BUFFER_LENGTH;
				}
				xmt_len-=maxonce;
			}
		}
		rtems_event_send (modem_tx_task, INTERRUPT_EVENT);
 	   break;
 	  case 0x00:
 	  		modem_status();
 	  		break;
 	  case 0x01:
		  	return;
	  case 0x06:
		ch=*RBR;
		errcount++;
		break;
			  		
  		default:
		    break;
	  }
  }
}


void modem_flush(int mode)
{
	if (mode&FWRITE)
	{
	}
	if (mode&FREAD)
	{
   }

}



int modemWriteTermios(int minor, const char *buf, int len)
{
	int level;
	int i,maxonce;
	if (len<=0)
		return 0;
		
	if (ModemData.t_line!=PPPDISC)
	{
	_CPU_ISR_Disable(level);
	for(i=0;i<len;i++)
	{
		if (xmt_len>=MODEM_BUFFER_LENGTH)
			break;
		xmt_buf[(xmt_start+xmt_len)%MODEM_BUFFER_LENGTH]=buf[i];
		xmt_len++;
	}
		_CPU_ISR_Enable(level);
	}
	rtems_event_send(modem_tx_task,START_TRANSMIT_EVENT);
	return i;
}


/*
 * Initialize and register the device
 */
rtems_device_driver modem_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
	rtems_status_code status;
	rtems_isr_entry old_handler;
	rtems_status_code sc;
   rtems_termios_initialize ();
	sc = rtems_semaphore_create (
			rtems_build_name ('M', 'D', 'M', 'P'),
			0,
			RTEMS_COUNTING_SEMAPHORE,
			RTEMS_NO_PRIORITY_CEILING,
			&ModemData.pppsem);  
	if (sc != RTEMS_SUCCESSFUL)
		rtems_fatal_error_occurred (sc);
	modem_init(9600); 

	sc = rtems_interrupt_catch (modemInterruptHandler,
						PPC_IRQ_IRQ4, 
						&old_handler);

	/*
	 * Register the devices
	 */
  	modem_tx_task=0;
	modem_rx_task=0;
	status = rtems_io_register_name ("/dev/modem", major, 0);
	if (status != RTEMS_SUCCESSFUL)
		rtems_fatal_error_occurred (status);
	return RTEMS_SUCCESSFUL;
}

rtems_device_driver modem_first_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
	modem_tty=(struct rtems_termios_tty *)(((rtems_libio_open_close_args_t *)arg)->iop->data1);
	return RTEMS_SUCCESSFUL;
}
rtems_device_driver modem_last_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  m823.siu.simask &=(~M823_SIMASK_IRM4); /* block the interrupts */
	return RTEMS_SUCCESSFUL;
}

static int
modemSetAttr(int minor, const struct termios *t)
{
  int baud;

  switch (t->c_cflag & CBAUD) 
    {
    case B50:	
      baud = 50;
      break;
    case B75:	
      baud = 75;	
      break;
    case B110:	
      baud = 110;	
      break;
    case B134:	
      baud = 134;	
      break;
    case B150:	
      baud = 150;	
      break;
    case B200:
      baud = 200;	
      break;
    case B300:	
      baud = 300;
      break;
    case B600:	
      baud = 600;	
      break;
    case B1200:	
      baud = 1200;
      break;
    case B1800:	
      baud = 1800;	
      break;
    case B2400:	
      baud = 2400;
      break;
    case B4800:	
      baud = 4800;
      break;
    case B9600:	
      baud = 9600;
      break;
    case B19200:
      baud = 19200;
      break;
    case B38400:
      baud = 38400;
      break;
    case B57600:	
      baud = 57600;
      break;
    case B115200:
      baud = 115200;
      break;
    default:
      baud = 0;
      rtems_fatal_error_occurred (RTEMS_INTERNAL_ERROR);
      return 0;
    }
	set_modem_speed(baud);
	return RTEMS_SUCCESSFUL;
}


/*
 * Open the device
 */

	

rtems_device_driver modem_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
    int error, s;
  rtems_status_code              status;
   register struct ppp_softc *sc;
  static rtems_termios_callbacks cb = 
  {
    modem_first_open,	              /* firstOpen */
    modem_last_close,       /* lastClose */
    NULL,                     /* poll read  */
    modemWriteTermios, /* write */
    modemSetAttr,	              /* setAttributes */
    NULL,	              /* stopRemoteTx */
    NULL,	              /* startRemoteTx */
    0		              /* outputUsesInterrupts */
  };	 

	if (ModemData.t_line == PPPDISC) 
	{
		sc = ModemData.t_sc;
		if (sc != NULL && sc->sc_devp == (void *) &ModemData) 
		{
			return (0);
		}
	}

	if ((ModemData.t_sc= sc = pppalloc(1)) == NULL)
		return 2;

    if (sc->sc_relinq)
	(*sc->sc_relinq)(sc);	/* get previous owner to relinquish the unit */

    sc->sc_ilen = 0;
    sc->sc_m = NULL;
    bzero(sc->sc_asyncmap, sizeof(sc->sc_asyncmap));
    sc->sc_asyncmap[0] = 0xffffffff;
    sc->sc_asyncmap[3] = 0x60000000;
    sc->sc_rasyncmap = 0;
    sc->sc_devp = &ModemData;
    sc->sc_start = pppasyncstart;
    sc->sc_ctlp = pppasyncctlp;
    sc->sc_relinq = pppasyncrelinq;
    sc->sc_outm = NULL;
    pppgetm(sc);
    sc->sc_if.if_flags |= IFF_RUNNING;
    sc->sc_if.if_baudrate = 38400; 

	status = rtems_termios_open (major, minor, arg, &cb);
  if(status != RTEMS_SUCCESSFUL)
    {
/*      printf("Error openning console device\n");
*/      return status;
    }


/* init rx and tx task for device */
	xmt_start=xmt_len=0;
	rcv_start=rcv_len=0;

	if (modem_rx_task==0 && modem_tx_task==0)
	{
	rtems_status_code sc;
	sc = rtems_task_create (rtems_build_name ('M', 'D', 't', 'x'),
			101,
			16*1024,
			RTEMS_PREEMPT|RTEMS_TIMESLICE|RTEMS_NO_ASR|RTEMS_INTERRUPT_LEVEL(0),
			RTEMS_NO_FLOATING_POINT|RTEMS_LOCAL,
			&modem_tx_task); 
	if (sc != RTEMS_SUCCESSFUL) {
			 
	}

	sc = rtems_task_start (modem_tx_task, (void *)modem_txDaemon, 0);
	if (sc != RTEMS_SUCCESSFUL) {
		 
	}

	sc = rtems_task_create (rtems_build_name ('M', 'D', 'r', 'x'),
			101,
			16*1024,
			RTEMS_PREEMPT|RTEMS_TIMESLICE|RTEMS_NO_ASR|RTEMS_INTERRUPT_LEVEL(0),
			RTEMS_NO_FLOATING_POINT|RTEMS_LOCAL,
			&modem_rx_task); 
	if (sc != RTEMS_SUCCESSFUL) {
			 
	}

	sc = rtems_task_start (modem_rx_task, (void *)modem_rxDaemon, 0);
	if (sc != RTEMS_SUCCESSFUL) {
			 
	}

	}
  /* Enable modem interrupts */
  m823.siu.simask |= M823_SIMASK_IRM4;

  return RTEMS_SUCCESSFUL;
}
 
/*
 * Close the device
 */
rtems_device_driver modem_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
    register struct ppp_softc *sc;
    modem_flush( FREAD|FWRITE);

    ModemData.t_line = 0;
    sc = ModemData.t_sc;
    if (sc != NULL) {
		ModemData.t_sc = NULL;
		if (&ModemData == (struct tty *) sc->sc_devp) {
		rtems_bsdnet_semaphore_obtain();
	    pppasyncrelinq(sc);
	    pppdealloc(sc);
		rtems_bsdnet_semaphore_release();
	}
    }
    return RTEMS_SUCCESSFUL;
}

/* for now works only as serial device */


rtems_device_driver modem_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_libio_rw_args_t *rw_args=  (rtems_libio_rw_args_t *)arg;
  char *buffer;
  int count=0,maximum;
  rtems_status_code              sc;

  buffer = rw_args->buffer;
  maximum = rw_args->count;
  if (ModemData.t_line==PPPDISC)
  {

    struct mbuf *m, *m0;
    register int s;
    int error = 0;
    rtems_status_code status;
    rtems_interval    ticks;
	register struct ppp_softc *sc = (struct ppp_softc *)ModemData.t_sc;
	 ticks=1000000/rtems_bsdnet_microseconds_per_tick;

    if (sc == NULL)
   	return 0;
    /*
     * Loop waiting for input, checking that nothing disasterous
     * happens in the meantime.
     */
    for (;;) {
		if (sc->sc_inq.ifq_head != NULL)
		{
/*	      printf("Read : Data are in the buffer\n");
*/			break;
		}
/*      printf("Read : Waiting for data\n");
*/
		status=rtems_semaphore_obtain(ModemData.pppsem,RTEMS_WAIT,ticks);
		if (_modem_cd==0)
		{
			rw_args->bytes_moved =0;
			return RTEMS_SUCCESSFUL;
		}
		
		if (status==RTEMS_TIMEOUT)
			return status;
	}
	rtems_bsdnet_semaphore_obtain();
    IF_DEQUEUE(&sc->sc_inq, m0);
	rtems_bsdnet_semaphore_release();

  for (m = m0; m && (count+m->m_next->m_len<maximum); m = m->m_next) /* check if packet will fit in buffer */
  {
  		memcpy(buffer,mtod(m, u_char *),m->m_len);
		count+=m->m_len;
		buffer+=m->m_len;
  }
   m_freem(m0);
  rw_args->bytes_moved = count;

  }
	else
		  sc = rtems_termios_read (arg);

  count=rw_args->bytes_moved;
  return (count >= 0) ? RTEMS_SUCCESSFUL : RTEMS_UNSATISFIED;
}

/*
 * Write to the device
 */
rtems_device_driver modem_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  int count,len;
  rtems_libio_rw_args_t *rw_args;
  char *out_buffer;
  int n,maximum;
  rw_args = (rtems_libio_rw_args_t *) arg;

  out_buffer = rw_args->buffer;
  maximum = rw_args->count;

	if (ModemData.t_line==PPPDISC)
	{
		register struct ppp_softc *sc = (struct ppp_softc *)ModemData.t_sc;
	   struct sockaddr dst;
		struct mbuf *m, *m0, **mp;	
		rtems_bsdnet_semaphore_obtain();
		for (mp = &m0; maximum; mp = &m->m_next) 
		{
			MGET(m, M_WAIT, MT_DATA);
			if ((*mp = m) == NULL) 
			{
				rtems_bsdnet_semaphore_release();
			    m_freem(m0);
			    return (ENOBUFS);
			}
			m->m_len = 0;
			if (maximum>= MCLBYTES / 2)
		    MCLGET(m, M_DONTWAIT);
			len = M_TRAILINGSPACE(m);
			if (len > maximum)
			{
				memcpy(mtod(m, u_char *),out_buffer,maximum);
				m->m_len=maximum;
				maximum=0;
			}
			else
			{
				memcpy(mtod(m, u_char *),out_buffer,len);
				maximum-=len;
				m->m_len=len;
				out_buffer+=len;
			}
		}
		
	    dst.sa_family = AF_UNSPEC;
	    bcopy(mtod(m0, u_char *), dst.sa_data, PPP_HDRLEN);
	    m0->m_data += PPP_HDRLEN;
	    m0->m_len -= PPP_HDRLEN;

/*		printf("Sending %d bytes \n",m0->m_len);
*/		n=pppoutput(&sc->sc_if, m0, &dst, (struct rtentry *)0);
		rtems_bsdnet_semaphore_release();
		return n;
	}
	else
	   return rtems_termios_write (arg);
}



/*
 * Handle ioctl request.
 * Should set hardware line speed, bits/char, etc.
 */
rtems_device_driver modem_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{ 
 	rtems_libio_ioctl_args_t *args=(rtems_libio_ioctl_args_t *)arg;
	struct ppp_softc *sc=ModemData.t_sc;
    int cmd;
    caddr_t data;
    int error=RTEMS_SUCCESSFUL;

	data=args->buffer;
	cmd=args->command;

    switch (cmd) {
	case RTEMS_IO_GET_ATTRIBUTES:
	case RTEMS_IO_SET_ATTRIBUTES:
	case RTEMS_IO_TCDRAIN:
		return rtems_termios_ioctl (arg);
	break;	
   case PPPIOCSASYNCMAP:
		sc->sc_asyncmap[0] = *(u_int *)data;
		break;

   case PPPIOCGASYNCMAP:
		*(u_int *)data = sc->sc_asyncmap[0];
	break;
	
	case PPPIOCSRASYNCMAP:
		sc->sc_rasyncmap = *(u_int *)data;
	break;

	case TIOCSETD:
		ModemData.t_line=*(int*)data;
	break;
	case TIOCGETD:	
		*(int*)data=ModemData.t_line;
	break;		
	case TIOCMBIS:
		if ((*(int*)data)&TIOCM_DTR)
			set_modem_dtr(1);
		break;
	case TIOCMBIC:
		if ((*(int*)data)&TIOCM_DTR)
			set_modem_dtr(0);
		break;	
   case PPPIOCGRASYNCMAP:
		*(u_int *)data = sc->sc_rasyncmap;
	break;

	case PPPIOCSXASYNCMAP:
		bcopy(data, sc->sc_asyncmap, sizeof(sc->sc_asyncmap));
		sc->sc_asyncmap[1] = 0;		    /* mustn't escape 0x20 - 0x3f */
		sc->sc_asyncmap[2] &= ~0x40000000;  /* mustn't escape 0x5e */
		sc->sc_asyncmap[3] |= 0x60000000;   /* must escape 0x7d, 0x7e */
		break;

   case PPPIOCGXASYNCMAP:
		bcopy(sc->sc_asyncmap, data, sizeof(sc->sc_asyncmap));
		break;

	default:
	rtems_bsdnet_semaphore_obtain();
	error = pppioctl(sc, cmd, data, 0, NULL);
		if (error == 0 && cmd == PPPIOCSMRU)
			pppgetm(sc);
	rtems_bsdnet_semaphore_release();
	
	}
	return error;

}



void
wait_input(timo)
    struct timeval *timo;
{
    int n;
    rtems_event_set events;
    rtems_interval    ticks;
  rtems_status_code              err;
	int num_wait=1;
    register struct ppp_softc *sc = (struct ppp_softc *)ModemData.t_sc;
    ticks = 1+(timo->tv_sec*1000000+timo->tv_usec)/rtems_bsdnet_microseconds_per_tick;
	while (num_wait)
	{
			
		if (sc->sc_inq.ifq_head != NULL)
			break;
/*      printf("Wait : Waiting for data for %d ticks\n",ticks);
*/		err=rtems_semaphore_obtain(ModemData.pppsem,RTEMS_WAIT,ticks);
		if (err==RTEMS_TIMEOUT)
		{
/*			printf("TIMEOUT : No data\n");
*/			break;
		}
	}

}

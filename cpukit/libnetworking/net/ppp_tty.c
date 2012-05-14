/*
 * ppp_tty.c - Point-to-Point Protocol (PPP) driver for asynchronous
 * 	       tty devices.
 *
 * Copyright (c) 1989 Carnegie Mellon University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by Carnegie Mellon University.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Drew D. Perkins
 * Carnegie Mellon University
 * 4910 Forbes Ave.
 * Pittsburgh, PA 15213
 * (412) 268-8576
 * ddp@andrew.cmu.edu
 *
 * Based on:
 *	@(#)if_sl.c	7.6.1.2 (Berkeley) 2/15/89
 *
 * Copyright (c) 1987 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Serial Line interface
 *
 * Rick Adams
 * Center for Seismic Studies
 * 1300 N 17th Street, Suite 1450
 * Arlington, Virginia 22209
 * (703)276-7900
 * rick@seismo.ARPA
 * seismo!rick
 *
 * Pounded on heavily by Chris Torek (chris@mimsy.umd.edu, umcp-cs!chris).
 * Converted to 4.3BSD Beta by Chris Torek.
 * Other changes made at Berkeley, based in part on code by Kirk Smith.
 *
 * Converted to 4.3BSD+ 386BSD by Brad Parker (brad@cayman.com)
 * Added VJ tcp header compression; more unified ioctls
 *
 * Extensively modified by Paul Mackerras (paulus@cs.anu.edu.au).
 * Cleaned up a lot of the mbuf-related code to fix bugs that
 * caused system crashes and packet corruption.  Changed pppstart
 * so that it doesn't just give up with a "collision" if the whole
 * packet doesn't fit in the output ring buffer.
 *
 * Added priority queueing for interactive IP packets, following
 * the model of if_sl.c, plus hooks for bpf.
 * Paul Mackerras (paulus@cs.anu.edu.au).
 */

/* $FreeBSD: src/sys/net/ppp_tty.c,v 1.69 2005/10/16 20:44:18 phk Exp $ */
/* from if_sl.c,v 1.11 84/10/04 12:54:47 rick Exp */
/* from NetBSD: if_ppp.c,v 1.15.2.2 1994/07/28 05:17:58 cgd Exp */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "opt_ppp.h"		/* XXX for ppp_defs.h */

#if NPPP > 0

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/kernel.h>

#include <net/if.h>
#include <net/if_types.h>

#ifdef VJC
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <net/slcompress.h>
#endif

#include <rtems.h>
#include <rtems/libio.h>
#include <sys/ttycom.h>
#include <termios.h>
#include <rtems/termiostypes.h>

#ifdef PPP_FILTER
#include <net/bpf.h>
#endif
#include <net/ppp_defs.h>
#include <net/if_ppp.h>
#include <net/if_pppvar.h>


void	pppasyncattach(void);
int     pppopen(struct rtems_termios_tty *tty);
int     pppclose(struct rtems_termios_tty *tty);
int     pppread(struct rtems_termios_tty *tty, rtems_libio_rw_args_t *rw_args);
int     pppwrite(struct rtems_termios_tty *tty, rtems_libio_rw_args_t *rw_args);
int     ppptioctl(struct rtems_termios_tty *tty, rtems_libio_ioctl_args_t *args);
int     pppinput(int c, struct rtems_termios_tty *tty);
int     pppstart(struct rtems_termios_tty *tp);
u_short pppfcs(u_short fcs, u_char *cp, int len);
void    pppallocmbuf(struct ppp_softc *sc, struct mbuf **mp);

static void pppasyncstart(struct ppp_softc *);
static void pppasyncctlp(struct ppp_softc *);
static void pppasyncrelinq(struct ppp_softc *);
/*static void ppp_timeout __P((void *)); */
/*static void pppdumpb __P((u_char *b, int l)); */
/*static void ppplogchar __P((struct ppp_softc *, int)); */

/*
 * Some useful mbuf macros not in mbuf.h.
 */
#define M_IS_CLUSTER(m)	((m)->m_flags & M_EXT)

#define M_DATASTART(m)	\
	(M_IS_CLUSTER(m) ? (m)->m_ext.ext_buf : \
	    (m)->m_flags & M_PKTHDR ? (m)->m_pktdat : (m)->m_dat)

#define M_DATASIZE(m)	\
	(M_IS_CLUSTER(m) ? (m)->m_ext.ext_size : \
	    (m)->m_flags & M_PKTHDR ? MHLEN: MLEN)

/*
 * We steal two bits in the mbuf m_flags, to mark high-priority packets
 * for output, and received packets following lost/corrupted packets.
 */
#define M_HIGHPRI	0x2000	/* output packet for sc_fastq */
#define M_ERRMARK	0x4000	/* steal a bit in mbuf m_flags */

/*
 * Does c need to be escaped?
 */
#define ESCAPE_P(c)	(sc->sc_asyncmap[(c) >> 5] & (1 << ((c) & 0x1F)))

/*
 * Procedures for using an async tty interface for PPP.
 */

/* This is a FreeBSD-2.0 kernel. */
#define CCOUNT(rb)      (((rb).Size+(rb).Head-(rb).Tail) % (rb).Size)
#define FCOUNT(rb)      ((rb).Size-CCOUNT(rb)-1)
#define PPP_LOWAT	100	/* Process more output when < LOWAT on queue */
#define	PPP_HIWAT	400	/* Don't start a new packet if HIWAT on que */

/*
 * Define the PPP line discipline.
 */

static struct rtems_termios_linesw pppdisc = {
	pppopen, pppclose, pppread, pppwrite,
	pppinput, pppstart, ppptioctl, NULL
};

void
pppasyncattach(void)
{
    rtems_termios_linesw[PPPDISC] = pppdisc;
}

TEXT_SET(pseudo_set, pppasyncattach);

/*
 * Line specific open routine for async tty devices.
 * Attach the given tty to the first available ppp unit.
 * Called from device open routine or ttioctl.
 */
/* ARGSUSED */
int
pppopen(struct rtems_termios_tty *tty)
{
    int                        i;
    register struct ppp_softc *sc;
    struct mbuf *m = (struct mbuf *)0;

    if (tty->t_line == PPPDISC) {
	sc = (struct ppp_softc *)tty->t_sc;
	if (sc != NULL && sc->sc_devp == (void *)tty) {
	    return (0);
	}
    }

    if ((sc = pppalloc(1)) == NULL) {
	return ENXIO;
    }

    if (sc->sc_relinq)
	(*sc->sc_relinq)(sc);	/* get previous owner to relinquish the unit */

    sc->sc_ilen = 0;
    sc->sc_m = NULL;
    bzero(sc->sc_asyncmap, sizeof(sc->sc_asyncmap));
    sc->sc_asyncmap[0] = 0xffffffff;
    sc->sc_asyncmap[3] = 0x60000000;
    sc->sc_rasyncmap = 0;
    sc->sc_devp = tty;
    sc->sc_start = pppasyncstart;
    sc->sc_ctlp = pppasyncctlp;
    sc->sc_relinq = pppasyncrelinq;
    sc->sc_outm = NULL;
    sc->sc_outmc = NULL;
    
    /* preallocate mbufs for free queue */
    rtems_bsdnet_semaphore_obtain();
    for (i=0; i<NUM_MBUFQ; i++) {
      pppallocmbuf(sc, &m);
      if ( i == 0 ) {
        /* use first mbuf for rx iterrupt handling */
        sc->sc_m = m;
      }
      else {
        /* enqueue mbuf for later use */
        IF_ENQUEUE(&sc->sc_freeq, m);
      }
      m = (struct mbuf *)0;
    }
    rtems_bsdnet_semaphore_release();

    /* initialize values */
    sc->sc_if.if_flags |= IFF_RUNNING;
    sc->sc_if.if_baudrate =
	rtems_termios_baud_to_number(tty->termios.c_cflag & CBAUD);

    tty->t_sc = (void *)sc;

    return ( RTEMS_SUCCESSFUL );
}

/*
 * Line specific close routine, called from device close routine
 * and from ttioctl.
 * Detach the tty from the ppp unit.
 * Mimics part of ttyclose().
 */
int
pppclose(struct rtems_termios_tty *tty)
{
    register struct ppp_softc *sc;

    tty->t_line = 0;
    sc = (struct ppp_softc *)tty->t_sc;
    if (sc != NULL) {
	tty->t_sc = NULL;
	if (tty == (struct rtems_termios_tty *)sc->sc_devp) {
            rtems_bsdnet_semaphore_obtain();
	    pppasyncrelinq(sc);
	    pppdealloc(sc);
            rtems_bsdnet_semaphore_release();
	}
    }
    return ( RTEMS_SUCCESSFUL );
}

/*
 * Relinquish the interface unit to another device.
 */
static void
pppasyncrelinq(struct ppp_softc *sc)
{
#ifdef XXX_XXX
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
#endif
}

/*
 * Line specific (tty) read routine.
 */
int
pppread(struct rtems_termios_tty *tty, rtems_libio_rw_args_t *rw_args)
{
    rtems_status_code            status  = RTEMS_UNSATISFIED;
    int                          count   = 0;
    int                          maximum = rw_args->count;
    char                        *buffer  = rw_args->buffer;
    register struct ppp_softc   *sc      = (struct ppp_softc *)tty->t_sc;
    struct mbuf                 *m;
    struct mbuf                 *m0;
    u_char                      *p;

    if (sc == NULL)
        return 0;

    /*
     * Loop waiting for input, checking that nothing disasterous
     * happens in the meantime.
     */
    if (tty != (struct rtems_termios_tty *)sc->sc_devp || tty->t_line != PPPDISC) {
        return ( status );
    }
    if (sc->sc_inq.ifq_head == NULL) {
      return ( status );
    }

    /* Get the packet from the input queue */
    rtems_bsdnet_semaphore_obtain();
    IF_DEQUEUE(&sc->sc_inq, m0);

    /* loop over mbuf chain */
    m = m0;
    while (( m != NULL ) && ( m->m_len > 0 ) && ( count+m->m_len < maximum )) {
      /* copy data into buffer */
      p = mtod(m, u_char *);
      memcpy(buffer, p, m->m_len);
      memset(p, 0, m->m_len);
      count  += m->m_len;
      buffer += m->m_len;

      /* increment loop index */
      m = m->m_next;
    }

    /* free mbuf chain */
    m_freem(m0);
    rtems_bsdnet_semaphore_release();

    /* update return values */
    rw_args->bytes_moved = count;
    if ( count >= 0 ) {
      status = RTEMS_SUCCESSFUL;
    }

    /* check to see if queue is empty */
    if (sc->sc_inq.ifq_head != NULL) {
      /* queue is not empty - post another event to ourself */
      rtems_event_send(sc->sc_pppdtask, PPPD_EVENT);
    }

    return ( status );
}

/*
 * Line specific (tty) write routine.
 */
int
pppwrite(struct rtems_termios_tty *tty, rtems_libio_rw_args_t *rw_args)
{
    struct sockaddr               dst;
    int                           n;
    int                           len;
    int                           maximum    = rw_args->count;
    char                         *out_buffer = rw_args->buffer; 
    register struct ppp_softc    *sc         = (struct ppp_softc *)tty->t_sc;
    struct mbuf                  *m;
    struct mbuf                  *m0;
    struct mbuf                 **mp;

    rtems_bsdnet_semaphore_obtain();
    for (mp = &m0; maximum; mp = &m->m_next) {
	MGET(m, M_WAIT, MT_DATA);
	if ((*mp = m) == NULL) {
	    m_freem(m0);
	    return (ENOBUFS);
	}
	m->m_len = 0;
	if (maximum >= MCLBYTES / 2) {
	    MCLGET(m, M_DONTWAIT);
        }
	len = M_TRAILINGSPACE(m);
	if (len > maximum) {
          memcpy(mtod(m, u_char *),out_buffer,maximum);
          m->m_len = maximum;
          maximum  = 0;
        }
        else {
          memcpy(mtod(m, u_char *),out_buffer,len);
          m->m_len    = len;
          maximum    -= len;
          out_buffer += len;
        }
    }

    dst.sa_family = AF_UNSPEC;
    bcopy(mtod(m0, u_char *), dst.sa_data, PPP_HDRLEN);
    m0->m_data += PPP_HDRLEN;
    m0->m_len  -= PPP_HDRLEN;

    n = pppoutput(&sc->sc_if, m0, &dst, (struct rtentry *)0);
    rtems_bsdnet_semaphore_release();

    return ( n );
}

/*
 * Line specific (tty) ioctl routine.
 * This discipline requires that tty device drivers call
 * the line specific l_ioctl routine from their ioctl routines.
 */
/* ARGSUSED */
int
ppptioctl(struct rtems_termios_tty *tty, rtems_libio_ioctl_args_t *args)
{
/*    int                 i;	*/
    int                 error = RTEMS_SUCCESSFUL;
    ioctl_command_t     cmd   = args->command;
    caddr_t             data  = args->buffer;
    struct ppp_softc   *sc    = tty->t_sc;

    switch (cmd) {
    case RTEMS_IO_GET_ATTRIBUTES:
    case RTEMS_IO_SET_ATTRIBUTES:
    case RTEMS_IO_TCDRAIN:
    case RTEMS_IO_SNDWAKEUP:
    case RTEMS_IO_RCVWAKEUP:
    case TIOCGETD:
    case TIOCSETD:
        error = rtems_termios_ioctl(args);
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
        rtems_bsdnet_semaphore_release();
    }
    return error;
}

/*
 * FCS lookup table as calculated by genfcstab.
 */
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

/*
 * Calculate a new FCS given the current FCS and the new data.
 */
u_short
pppfcs(u_short fcs, u_char *cp, int len)
{
    while (len--)
	fcs = PPP_FCS(fcs, *cp++);
    return (fcs);
}

/*
 * This gets called at splsoftnet from if_ppp.c at various times
 * when there is data ready to be sent.
 */
void pppasyncstart(struct ppp_softc *sc)
{
  /* check to see if output is not active */
  if ( sc->sc_outflag == 0 ) {
    /* mark active and post tx event to daemon */
    sc->sc_outflag |= SC_TX_PENDING;
    rtems_event_send(sc->sc_txtask, TX_PACKET);
  }
}

/*
 * This gets called when a received packet is placed on
 * the inq, at splsoftnet.
 */
static void
pppasyncctlp(
    struct ppp_softc *sc)
{
  /* check to see if task id was set */
  if ( sc->sc_pppdtask != 0 ) {
    /* post event to daemon */
    rtems_event_send(sc->sc_pppdtask, PPPD_EVENT);
  }
}

/*
 * Start output on async tty interface.  If the transmit queue
 * has drained sufficiently, arrange for pppasyncstart to be
 * called later at splsoftnet.
 * Called at spltty or higher.
 */
int
pppstart(struct rtems_termios_tty *tp)
{
  u_char             *sendBegin;
  u_long              ioffset = (u_long       )0;
  struct mbuf        *m       = (struct mbuf *)0;
  struct ppp_softc   *sc      = tp->t_sc;

  /* ensure input is valid and we are busy */
  if (( sc != NULL ) && ( sc->sc_outflag & SC_TX_BUSY )) {
    /* check to see if we need to get the next buffer */

    /* Ready with PPP_FLAG Character ? */
    if(sc->sc_outflag & SC_TX_LASTCHAR){
        sc->sc_outflag &= ~(SC_TX_BUSY | SC_TX_FCS | SC_TX_LASTCHAR);
        rtems_event_send(sc->sc_txtask, TX_TRANSMIT);	/* Ready for the next Packet */
        return(0);
    }

    if ( sc->sc_outoff >= sc->sc_outlen ) {
      /* loop to get next non-zero length buffer */
      if ( sc->sc_outmc != NULL ) {
        m = sc->sc_outmc->m_next;
      }

      /* check for next mbuf in chain */
      if ( m != NULL ) {
        /* update values to use this mbuf */
        sc->sc_outmc  = m;
        sc->sc_outbuf = mtod(m, u_char *);
        sc->sc_outlen = m->m_len;
        sc->sc_outoff = (short)0;
      }
      else if ( (sc->sc_outflag & SC_TX_FCS) == 0 ) {
        /* setup to use FCS buffer */
        sc->sc_outflag |= SC_TX_FCS;
        sc->sc_outbuf   = sc->sc_outfcsbuf;
        sc->sc_outlen   = sc->sc_outfcslen;
        sc->sc_outoff   = (short)0;
      }
      else {
        /* done with this packet */
        sc->sc_outflag |= SC_TX_LASTCHAR;
        sc->sc_outflag &=~(SC_TX_FCS);
		sc->sc_outchar = (u_char)PPP_FLAG;
        (*tp->device.write)(tp->minor, (char *)&sc->sc_outchar, 1);
        return(0);
      }
    }

    /* check to see if there is some data to write out */
    if ( sc->sc_outoff < sc->sc_outlen ) {
      /* check to see if character needs to be escaped */
      sc->sc_outchar = sc->sc_outbuf[sc->sc_outoff];
      if ( ESCAPE_P(sc->sc_outchar) ) {
        if ( sc->sc_outflag & SC_TX_ESCAPE ) {
          /* last sent character was the escape character */
          sc->sc_outchar = sc->sc_outchar ^ PPP_TRANS;

          /* clear the escape flag and increment the offset */
          sc->sc_outflag &= ~SC_TX_ESCAPE;
          ioffset++;
        }
        else {
          /* need to send the escape character */
          sc->sc_outchar = PPP_ESCAPE;

          /* set the escape flag */
          sc->sc_outflag |= SC_TX_ESCAPE;
        }
	sendBegin = &sc->sc_outchar;
      }
      else {
        /* escape not needed - increment the offset as much as possible */
	while ((!ESCAPE_P(sc->sc_outchar)) && ((sc->sc_outoff + ioffset) < sc->sc_outlen)) {
	    ioffset++;
	    sc->sc_outchar = sc->sc_outbuf[sc->sc_outoff + ioffset];
	}
	sendBegin = &sc->sc_outbuf[sc->sc_outoff];
      }

      /* write out the character(s) and update the stats */
      (*tp->device.write)(tp->minor, (char *)sendBegin, (ioffset > 0) ? ioffset : 1);
      sc->sc_stats.ppp_obytes += (ioffset > 0) ? ioffset : 1;
      sc->sc_outoff += ioffset;
    }
  }

  return ( 0 );
}

#ifdef XXX_XXX
/*
 * Timeout routine - try to start some more output.
 */
static void
ppp_timeout(void *x)
{
    struct rtems_termios_tty *tty = (struct rtems_termios_tty *)x;
    struct ppp_softc         *sc  = tty->t_sc;
/*    struct rtems_termios_tty *tp  = (struct rtems_termios_tty *)sc->sc_devp; */

    sc->sc_flags &= ~SC_TIMEOUT;
/*    pppstart(tp); */
}
#endif

/*
 * Allocate enough mbuf to handle current MRU.
 */
#ifdef XXX_XXX
static void
pppgetm(struct ppp_softc *sc)
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
#endif

void
pppallocmbuf(struct ppp_softc *sc, struct mbuf **mp)
{
  int            ilen;
  struct mbuf   *m;

  /* loop over length value */
  ilen = sc->sc_mru + PPP_HDRLEN + PPP_FCSLEN;
  while ( ilen > 0 ) {
    /* see if this is end of the chain */
    m = *mp;
    if ( m == NULL ) {
      /* get mbuf header */
      MGETHDR(m, M_DONTWAIT, MT_DATA);
      if ( m == NULL ) {
        /* error - set condition to break out */
        printf("pppallocmbuf: MGETHDR failed\n");
        break;
      }
      MCLGET(m, M_DONTWAIT);
      m->m_next = NULL;
      *mp = m;
    }

    /* update loop variables */
    mp    = &m->m_next;
    ilen -= M_DATASIZE(m);
  }
}

/*
 * tty interface receiver interrupt.
 */
static uint32_t paritytab[8] = {
    0x96696996L, 0x69969669L, 0x69969669L, 0x96696996L,
    0x69969669L, 0x96696996L, 0x96696996L, 0x69969669L
};

int
pppinput(int c, struct rtems_termios_tty *tp)
{
    register struct ppp_softc *sc = tp->t_sc;
    struct mbuf *m;
    int ilen;

    if (sc == NULL || tp != (struct rtems_termios_tty *)sc->sc_devp)
	return 0;
    if (sc->sc_m == NULL) {
        rtems_event_send(sc->sc_rxtask, RX_EMPTY);
        IF_DEQUEUE(&sc->sc_freeq, sc->sc_m);
        if ( sc->sc_m == NULL ) {
          return 0;
        }
    }

    ++sc->sc_stats.ppp_ibytes;

    c &= 0xff;
    if (c & 0x80)
	sc->sc_flags |= SC_RCV_B7_1;
    else
	sc->sc_flags |= SC_RCV_B7_0;
    if (paritytab[c >> 5] & (1 << (c & 0x1F)))
	sc->sc_flags |= SC_RCV_ODDP;
    else
	sc->sc_flags |= SC_RCV_EVNP;

    if (c == PPP_FLAG) {
	ilen = sc->sc_ilen;
	sc->sc_ilen = 0;

	/*
	 * If SC_ESCAPED is set, then we've seen the packet
	 * abort sequence "}~".
	 */
	if (sc->sc_flags & (SC_FLUSH | SC_ESCAPED)
	    || (ilen > 0 && sc->sc_fcs != PPP_GOODFCS)) {
	    sc->sc_flags |= SC_PKTLOST;	/* note the dropped packet */
	    if ((sc->sc_flags & (SC_FLUSH | SC_ESCAPED)) == 0){
                /* bad fcs error */
		sc->sc_if.if_ierrors++;
		sc->sc_stats.ppp_ierrors++;
	    } else
		sc->sc_flags &= ~(SC_FLUSH | SC_ESCAPED);
	    return 0;
	}

	if (ilen < PPP_HDRLEN + PPP_FCSLEN) {
	    if (ilen) {
                /* too short error */
		sc->sc_if.if_ierrors++;
		sc->sc_stats.ppp_ierrors++;
		sc->sc_flags |= SC_PKTLOST;
	    }
	    return 0;
	}

	/* Remove FCS trailer.  Somewhat painful... */
	ilen -= 2;
	if (--sc->sc_mc->m_len == 0) {
	    for (m = sc->sc_m; m->m_next != sc->sc_mc; m = m->m_next);
	    sc->sc_mc = m;
	}
	sc->sc_mc->m_len--;

	/* excise this mbuf chain - place on raw queue */
	m = sc->sc_m;
        if ( sc->sc_flags & SC_PKTLOST ) {
          m->m_flags   |= M_ERRMARK;
          sc->sc_flags &= ~SC_PKTLOST;
        }
        IF_ENQUEUE(&sc->sc_rawq, m);

        /* setup next mbuf chain */
        IF_DEQUEUE(&sc->sc_freeq, sc->sc_m);

        /* send rx packet event */
        rtems_event_send(sc->sc_rxtask, RX_PACKET);
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
	m = sc->sc_m;
	m->m_len = 0;
	m->m_data = M_DATASTART(sc->sc_m);
	sc->sc_mc = m;
	sc->sc_mp = mtod(m, char *);
	sc->sc_fcs = PPP_INITFCS;
	if (c != PPP_ALLSTATIONS) {
	    if (sc->sc_flags & SC_REJ_COMP_AC) {
                /* garbage received error */
		goto flush;
	    }
	    *sc->sc_mp++ = PPP_ALLSTATIONS;
	    *sc->sc_mp++ = PPP_UI;
	    sc->sc_ilen += 2;
	    m->m_len += 2;
	}
    }
    if (sc->sc_ilen == 1 && c != PPP_UI) {
        /* missing UI error */
	goto flush;
    }
    if (sc->sc_ilen == 2 && (c & 1) == 1) {
	/* a compressed protocol */
	*sc->sc_mp++ = 0;
	sc->sc_ilen++;
	sc->sc_mc->m_len++;
    }
    if (sc->sc_ilen == 3 && (c & 1) == 0) {
        /* bad protocol error */
	goto flush;
    }

    /* packet beyond configured mru? */
    if (++sc->sc_ilen > sc->sc_mru + PPP_HDRLEN + PPP_FCSLEN) {
        /* packet too big error */
	goto flush;
    }

    /* is this mbuf full? */
    m = sc->sc_mc;
    if (M_TRAILINGSPACE(m) <= 0) {
	if (m->m_next == NULL) {
          /* get next available mbuf for the chain */
          IF_DEQUEUE(&sc->sc_freeq, m->m_next);
          if (m->m_next == NULL) {
            /* too few mbufs */
            goto flush;
          }
          else {
            /* send rx mbuf event */
            rtems_event_send(sc->sc_rxtask, RX_MBUF);
          }
	}
	sc->sc_mc = m = m->m_next;
	m->m_len  = 0;
	m->m_next = 0;
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
    }
    return 0;
}

#ifdef XXX_XXX
#define MAX_DUMP_BYTES	128

static void
ppplogchar(struct ppp_softc *sc, int c)
{
    if (c >= 0)
	sc->sc_rawin[sc->sc_rawin_count++] = c;
    if (sc->sc_rawin_count >= sizeof(sc->sc_rawin)
	|| (c < 0 && sc->sc_rawin_count > 0)) {
	printf("ppp%d input: ", sc->sc_if.if_unit);
	pppdumpb(sc->sc_rawin, sc->sc_rawin_count);
	sc->sc_rawin_count = 0;
    }
}

static void
pppdumpb(u_char *b, int l)
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
    printf("%s\n", buf);
}
#endif

#endif	/* NPPP > 0 */

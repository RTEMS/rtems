/*
 * if_ppp.c - Point-to-Point Protocol (PPP) Asynchronous driver.
 */

/*-
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
 * so that it doesn't just give up with a collision if the whole
 * packet doesn't fit in the output ring buffer.
 *
 * Added priority queueing for interactive IP packets, following
 * the model of if_sl.c, plus hooks for bpf.
 * Paul Mackerras (paulus@cs.anu.edu.au).
 */

/* $FreeBSD: src/sys/net/if_ppp.c,v 1.109 2005/10/12 19:52:16 thompsa Exp $ */
/* from if_sl.c,v 1.11 84/10/04 12:54:47 rick Exp */
/* from NetBSD: if_ppp.c,v 1.15.2.2 1994/07/28 05:17:58 cgd Exp */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "opt_inet.h"
#include "opt_ipx.h"
#include "opt_mac.h"
#include "opt_ppp.h"

#if NPPP > 0

#include <termios.h>
#include <rtems/termiostypes.h>
#include <rtems/rtems_bsdnet.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/kernel.h>
#include <sys/time.h>
#include <sys/malloc.h>

#include <net/if.h>
#include <net/if_types.h>
#include <net/netisr.h>
#include <net/route.h>
#ifdef PPP_FILTER
#include <net/bpf.h>
#endif

#if INET
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/in_var.h>
#include <netinet/ip.h>
#endif

#if NBPFILTER > 0
#include <net/bpf.h>
#endif

#ifdef VJC
#include <net/slcompress.h>
#endif

#include <net/ppp_defs.h>
#include <net/if_ppp.h>
#include <net/if_pppvar.h>
#include <machine/cpu.h>

#define splsoftnet	splnet

#ifdef PPP_COMPRESS
#define PACKETPTR	struct mbuf *
#include <net/ppp-comp.h>
#endif

static struct	ppp_softc ppp_softc[NPPP];

static int	pppsioctl(struct ifnet *ifp, ioctl_command_t cmd, caddr_t data);
static void	ppp_requeue(struct ppp_softc *);
#ifdef PPP_COMPRESS
static void	ppp_ccp(struct ppp_softc *, struct mbuf *m, int rcvd);
static void	ppp_ccp_closed(struct ppp_softc *);
#endif
static struct mbuf *ppp_inproc(struct ppp_softc *, struct mbuf *);
static void	pppdumpm(struct mbuf *m0);

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


#ifdef PPP_COMPRESS
/*
 * List of compressors we know about.
 * We leave some space so maybe we can modload compressors.
 */

extern struct compressor ppp_bsd_compress;
extern struct compressor ppp_deflate, ppp_deflate_draft;

struct compressor *ppp_compressors[8] = {
#if DO_BSD_COMPRESS
    &ppp_bsd_compress,
#endif
#if DO_DEFLATE
    &ppp_deflate,
    &ppp_deflate_draft,
#endif
    NULL
};
#endif /* PPP_COMPRESS */

extern struct ifqueue    ipintrq;
static struct timeval    ppp_time;

#ifndef __rtems__
TEXT_SET(pseudo_set, ppp_rxdaemon);
#endif

static rtems_task ppp_rxdaemon(rtems_task_argument arg)
{
  rtems_event_set             events;
  rtems_interrupt_level       level;
  struct ppp_softc           *sc = (struct ppp_softc *)arg;
  struct mbuf                *mp = (struct mbuf      *)0;
  struct mbuf                *m;

  /* enter processing loop */
  while ( 1 ) {
    /* wait for event */
    rtems_event_receive(RX_PACKET|RX_MBUF|RX_EMPTY,RTEMS_WAIT|RTEMS_EVENT_ANY,RTEMS_NO_TIMEOUT,&events);
    if ( events & RX_EMPTY ) {
      printf("RX: QUEUE is EMPTY\n");
      events &= ~RX_EMPTY;
    }

    if ( events ) {
      /* get the network semaphore */
      rtems_bsdnet_semaphore_obtain();

      /* check to see if new packet was received */
      if ( events & RX_PACKET ) {
        /* get received packet mbuf chain */
        rtems_interrupt_disable(level);
        IF_DEQUEUE(&sc->sc_rawq, m);
        rtems_interrupt_enable(level);

        /* ensure packet was retrieved */
        if ( m != (struct mbuf *)0 ) {
          /* process the received packet */
          mp = ppp_inproc(sc, m);
        }
      }

      /* allocate a new mbuf to replace one */
      if ( mp == NULL ) {
        pppallocmbuf(sc, &mp);
      }

      /* place mbuf on freeq */
      rtems_interrupt_disable(level);
      IF_ENQUEUE(&sc->sc_freeq, mp);
      rtems_interrupt_enable(level);
      mp = (struct mbuf *)0;

      /* release the network semaphore */
      rtems_bsdnet_semaphore_release();

      /* check to see if queue is empty */
      if ( sc->sc_rawq.ifq_head ) {
        /* queue is not empty - post another event */
        rtems_event_send(sc->sc_rxtask, RX_PACKET);
      }
    }
  }
}

static rtems_task ppp_txdaemon(rtems_task_argument arg)
{
  rtems_event_set             events;
  int                         iprocess = (int               )0;
  struct ppp_softc           *sc       = (struct ppp_softc *)arg;
  struct mbuf                *mp;
  struct mbuf                *mf;
  struct mbuf                *m;
  struct rtems_termios_tty   *tp;
  
  int frag;

  /* enter processing loop */
  while ( 1 ) {
    /* wait for event */
    rtems_event_receive(TX_PACKET|TX_TRANSMIT,RTEMS_WAIT|RTEMS_EVENT_ANY,RTEMS_NO_TIMEOUT,&events);
    if ( events & TX_TRANSMIT ) {
      /* received event from interrupt handler - free current mbuf */
      rtems_bsdnet_semaphore_obtain();

      m_freem(sc->sc_outm);
      
      rtems_bsdnet_semaphore_release();

      /* chain is done - clear the values */
      sc->sc_outm  = (struct mbuf *)0;
      sc->sc_outmc = (struct mbuf *)0;

      /* now set flag to fake receive of TX_PACKET event */
      /* this will check to see if we have any pending packets */
      events |= TX_PACKET;
    }

    /* received event from pppasyncstart */
    if ( events & TX_PACKET ) {
      /* ensure we are not busy */
      if ( sc->sc_outm == (struct mbuf *)0 ) {
        /* try dequeuing a packet */
        sc->sc_outm = ppp_dequeue(sc);
        if ( sc->sc_outm == NULL ) {
          /* clear output flags */
          sc->sc_outflag      = 0;
          sc->sc_if.if_flags &= ~IFF_OACTIVE;
        }
        else {
          /* set flag to start process */
          iprocess            = 1;
          sc->sc_outflag      = SC_TX_BUSY;
          sc->sc_if.if_flags |= IFF_OACTIVE;
        }
      }
    }

    /* check to see if there is any processing required */
    if ( iprocess ) {
      /* clear process flag */
      iprocess = (int)0;
      frag=0;

      /* initialize output values */
      sc->sc_outfcs    = PPP_INITFCS;
      sc->sc_outbuf    = (u_char *)0;
      sc->sc_outlen    = (short   )0;
      sc->sc_outoff    = (short   )0;
      sc->sc_outfcslen = (short   )0;

/*	  printf("Start Transmit Packet..\n"); */

      /* loop over all mbufs in chain */
      mf     = NULL;
      mp     = NULL;
      m      = sc->sc_outm;

      sc->sc_outmc  = m;
      sc->sc_outlen = m->m_len;
      sc->sc_outbuf = mtod(m, u_char *);

      while (( m != (struct mbuf *)0 ) && ( m->m_len > 0 )) {
      	frag++;

        /* update the FCS value and then check next packet length */
        if(m->m_len){
       	 sc->sc_outfcs = pppfcs(sc->sc_outfcs, mtod(m, u_char *), m->m_len);
       	}

        if(( m->m_next != NULL ) && ( m->m_next->m_len == 0 )) {
			if(mf){
				printf(" if_ppp.c : MBUF Error !!!!\n");
			}
			else{
            	mf = m->m_next;
	        	m->m_next = NULL;
			}
		}

#ifdef LALL_X
        /* check next packet to see if it is empty */
        while (( m->m_next != NULL ) && ( m->m_next->m_len == 0 )) {
          /* next mbuf is zero length */
          /* add empty mbuf to free chain */
          if ( mp == NULL ) {
            /* item is head of free list */
            mf = m->m_next;
            mp = mf;
          }
          else {
            /* add item to end of the free list */
            mp->m_next = m->m_next;
            mp         = m->m_next;
          }

          /* remove empty item from process chain */
          m->m_next  = m->m_next->m_next;
          mp->m_next = NULL;
        }
#endif
        /* move to next packet */
        m = m->m_next;
      }

      /* ensure there is data to be sent out */
      tp = (struct rtems_termios_tty *)sc->sc_devp;
      if (( tp != NULL ) && ( sc->sc_outmc != (struct mbuf *)0 )) {
        /* place FCS value into buffer */
        sc->sc_outfcsbuf[sc->sc_outfcslen++] = ~sc->sc_outfcs & 0xff;
        sc->sc_outfcsbuf[sc->sc_outfcslen++] = (~sc->sc_outfcs >> 8) & 0xff;
        microtime(&sc->sc_if.if_lastchange);
  
        /* write out frame byte to start the transmission */
		sc->sc_outchar = (u_char)PPP_FLAG;
        (*tp->device.write)(tp->minor, (char *)&sc->sc_outchar, 1);
      }

      /* check to see if we need to free some empty mbufs */
      if ( mf != (struct mbuf *)0 ) {
        /* free empty mbufs */
        rtems_bsdnet_semaphore_obtain();
        m_freem(mf);
        rtems_bsdnet_semaphore_release();
      }
    }
  }
}

static void ppp_init(struct ppp_softc *sc)
{
  rtems_status_code   status;
  uint32_t      priority = 100;

  /* determine priority value */
  if ( rtems_bsdnet_config.network_task_priority ) {
    priority = rtems_bsdnet_config.network_task_priority;
  }

  /* check to see if we need to start up daemons */
  if ( sc->sc_rxtask == 0 ) {
    /* start rx daemon task */
    status = rtems_task_create(rtems_build_name('R','x','P','0'+sc->sc_if.if_unit), priority, 2048,
                               RTEMS_PREEMPT|RTEMS_NO_TIMESLICE|RTEMS_NO_ASR|RTEMS_INTERRUPT_LEVEL(0),
                               RTEMS_NO_FLOATING_POINT|RTEMS_LOCAL,
                               &sc->sc_rxtask);
    if (status != RTEMS_SUCCESSFUL) {
      rtems_fatal_error_occurred(status);
    }
    else {
      status = rtems_task_start(sc->sc_rxtask, ppp_rxdaemon, (rtems_task_argument)sc);
      if (status != RTEMS_SUCCESSFUL) {
        rtems_fatal_error_occurred(status);
      }
    }

    /* start tx daemon task */
    status = rtems_task_create(rtems_build_name('T','x','P','0'+sc->sc_if.if_unit), priority, 2048,
                               RTEMS_PREEMPT|RTEMS_NO_TIMESLICE|RTEMS_NO_ASR|RTEMS_INTERRUPT_LEVEL(0),
                               RTEMS_NO_FLOATING_POINT|RTEMS_LOCAL,
                               &sc->sc_txtask);
    if (status != RTEMS_SUCCESSFUL) {
      rtems_fatal_error_occurred(status);
    }
    else {
      status = rtems_task_start(sc->sc_txtask, ppp_txdaemon, (rtems_task_argument)sc);
      if (status != RTEMS_SUCCESSFUL) {
        rtems_fatal_error_occurred(status);
      }
    }
  }

  /* mark driver running and output inactive */
  /* ilya: IFF_RUNNING flag will be marked after the IPCP goes up */
/*  sc->sc_if.if_flags |= IFF_RUNNING;	*/
}

/*
 * Called from boot code to establish ppp interfaces.
 */
int rtems_ppp_driver_attach(struct rtems_bsdnet_ifconfig *config, int attaching)
{
/*    int                 i = (int)0;	*/
    struct ppp_softc   *sc;
    char               *name;
    int                 number;
    
    
    number = rtems_bsdnet_parse_driver_name (config, &name);
    
    if (!attaching || (number >= NPPP))
        return 0;
	
    sc = &ppp_softc[number];
    
    if (sc->sc_if.if_name != NULL)
	return 0;	/* interface is already attached */
    
/*    for (sc = ppp_softc; i < NPPP; sc++) {	*/
	sc->sc_if.if_name = name /*"ppp"*/;
	sc->sc_if.if_unit = number /*i++*/;
	sc->sc_if.if_mtu = PPP_MTU;
	sc->sc_if.if_flags = IFF_POINTOPOINT | IFF_MULTICAST;
	sc->sc_if.if_type = IFT_PPP;
	sc->sc_if.if_hdrlen = PPP_HDRLEN;
	sc->sc_if.if_ioctl = pppsioctl;
	sc->sc_if.if_output = pppoutput;
	sc->sc_if.if_snd.ifq_maxlen = IFQ_MAXLEN;
	sc->sc_inq.ifq_maxlen = IFQ_MAXLEN;
	sc->sc_fastq.ifq_maxlen = IFQ_MAXLEN;
	sc->sc_rawq.ifq_maxlen = IFQ_MAXLEN;
	sc->sc_freeq.ifq_maxlen = NUM_MBUFQ;

        /* initialize and attach */
	ppp_init(sc);
	if_attach(&sc->sc_if);
#if NBPFILTER > 0
	bpfattach(&sc->sc_bpf, &sc->sc_if, DLT_PPP, PPP_HDRLEN);
#endif
/*    }	*/

    return ( 1 );
}

/*
 * Allocate a ppp interface unit and initialize it.
 */
struct ppp_softc *
pppalloc(pid_t pid)
{
    int nppp, i;
    struct ppp_softc *sc;

    for (nppp = 0, sc = ppp_softc; nppp < NPPP; nppp++, sc++)
	if (sc->sc_xfer == pid) {
	    sc->sc_xfer = 0;
	    return sc;
	}
    for (nppp = 0, sc = ppp_softc; nppp < NPPP; nppp++, sc++)
	if (sc->sc_devp == NULL)
	    break;
    if (nppp >= NPPP)
	return NULL;

    sc->sc_flags = 0;
    sc->sc_mru = PPP_MRU;
    sc->sc_relinq = NULL;
    bzero((char *)&sc->sc_stats, sizeof(sc->sc_stats));
#ifdef VJC
    MALLOC(sc->sc_comp, struct vjcompress *, sizeof(struct vjcompress),
	   M_DEVBUF, M_NOWAIT);
    if (sc->sc_comp)
	sl_compress_init(sc->sc_comp, -1);
#endif
#ifdef PPP_COMPRESS
    sc->sc_xc_state = NULL;
    sc->sc_rc_state = NULL;
#endif /* PPP_COMPRESS */
    for (i = 0; i < NUM_NP; ++i)
	sc->sc_npmode[i] = NPMODE_ERROR;
    sc->sc_npqueue = NULL;
    sc->sc_npqtail = &sc->sc_npqueue;
    microtime(&ppp_time);
    sc->sc_last_sent = sc->sc_last_recv = ppp_time.tv_sec;

    return sc;
}

/*
 * Deallocate a ppp unit.  Must be called at splsoftnet or higher.
 */
void
pppdealloc(struct ppp_softc *sc)
{
    struct mbuf *m;
    rtems_interrupt_level       level;

    if_down(&sc->sc_if);
    sc->sc_if.if_flags &= ~(IFF_UP|IFF_RUNNING);
    sc->sc_devp = NULL;
    sc->sc_xfer = 0;

    rtems_interrupt_disable(level);
    if ( sc->sc_m != NULL ) {
	m_freem(sc->sc_m);
        sc->sc_m = (struct mbuf *)0;
    }
    if ( sc->sc_outm != NULL ) {
	m_freem(sc->sc_outm);
        sc->sc_outm    = (struct mbuf *)0;
        sc->sc_outmc   = (struct mbuf *)0;
        sc->sc_outflag = 0;
    }
    do {
      IF_DEQUEUE(&sc->sc_freeq, m);
      if (m != NULL) {
        m_freem(m);
      }
    } while ( m != NULL );
    do {
      IF_DEQUEUE(&sc->sc_rawq, m);
      if (m != NULL) {
        m_freem(m);
      }
    } while ( m != NULL );
    rtems_interrupt_enable(level);

    for (;;) {
	IF_DEQUEUE(&sc->sc_inq, m);
	if (m == NULL)
	    break;
	m_freem(m);
    }
    for (;;) {
	IF_DEQUEUE(&sc->sc_fastq, m);
	if (m == NULL)
	    break;
	m_freem(m);
    }
    while ((m = sc->sc_npqueue) != NULL) {
	sc->sc_npqueue = m->m_nextpkt;
	m_freem(m);
    }
#ifdef PPP_COMPRESS
    ppp_ccp_closed(sc);
    sc->sc_xc_state = NULL;
    sc->sc_rc_state = NULL;
#endif /* PPP_COMPRESS */
#ifdef PPP_FILTER
    if (sc->sc_pass_filt.bf_insns != 0) {
	FREE(sc->sc_pass_filt.bf_insns, M_DEVBUF);
	sc->sc_pass_filt.bf_insns = 0;
	sc->sc_pass_filt.bf_len = 0;
    }
    if (sc->sc_active_filt.bf_insns != 0) {
	FREE(sc->sc_active_filt.bf_insns, M_DEVBUF);
	sc->sc_active_filt.bf_insns = 0;
	sc->sc_active_filt.bf_len = 0;
    }
#endif /* PPP_FILTER */
#ifdef VJC
    if (sc->sc_comp != 0) {
	FREE(sc->sc_comp, M_DEVBUF);
	sc->sc_comp = 0;
    }
#endif
}

/*
 * Ioctl routine for generic ppp devices.
 */
int
pppioctl(struct ppp_softc *sc, ioctl_command_t cmd, caddr_t data,
    int flag, struct proc *p)
{
    int s, flags, mru, npx, taskid;
    struct npioctl *npi;
    time_t t;
#ifdef PPP_FILTER
    int error;
    struct bpf_program *bp, *nbp;
    struct bpf_insn *newcode, *oldcode;
    int newcodelen;
#endif /* PPP_FILTER */
#ifdef	PPP_COMPRESS
    int nb;
    struct ppp_option_data *odp;
    struct compressor **cp;
    u_char ccp_option[CCP_MAX_OPTION_LENGTH];
#endif

    switch (cmd) {
    case FIONREAD:
	*(int *)data = sc->sc_inq.ifq_len;
	break;

    case PPPIOCSTASK:
	taskid = *(int *)data;
	sc->sc_pppdtask = taskid;
	break;

    case PPPIOCGUNIT:
	*(int *)data = sc->sc_if.if_unit;
	break;

    case PPPIOCGFLAGS:
	*(u_int *)data = sc->sc_flags;
	break;

    case PPPIOCSFLAGS:
	flags = *(int *)data & SC_MASK;
	s = splsoftnet();
#ifdef PPP_COMPRESS
	if (sc->sc_flags & SC_CCP_OPEN && !(flags & SC_CCP_OPEN))
	    ppp_ccp_closed(sc);
#endif
	s = splimp();
	sc->sc_flags = (sc->sc_flags & ~SC_MASK) | flags;
	splx(s);
	break;

    case PPPIOCSMRU:
	mru = *(int *)data;
        if ( mru >= MCLBYTES ) {
          /* error - currently only handle 1 culster sized MRU */
          /* if we want to handle up to PPP_MAXMRU then we */
          /*   need to reallocate all mbufs on the freeq */
          /*   this can only be done with iterrupts disabled */
          return ( -1 );
        }
        else if ( mru >= PPP_MRU ) {
          /* update the size */
          sc->sc_mru = mru;
        }
	break;

    case PPPIOCGMRU:
	*(int *)data = sc->sc_mru;
	break;

#ifdef VJC
    case PPPIOCSMAXCID:
	if (sc->sc_comp) {
	    s = splsoftnet();
	    sl_compress_init(sc->sc_comp, *(int *)data);
	    splx(s);
	}
	break;
#endif

    case PPPIOCXFERUNIT:
	sc->sc_xfer = 0; /* Always root p->p_pid;*/
	break;

#ifdef PPP_COMPRESS
    case PPPIOCSCOMPRESS:
	odp = (struct ppp_option_data *) data;
	nb = odp->length;
	if (nb > sizeof(ccp_option))
	    nb = sizeof(ccp_option);
	if ((error = copyin(odp->ptr, ccp_option, nb)) != 0)
	    return (error);
	if (ccp_option[1] < 2)	/* preliminary check on the length byte */
	    return (EINVAL);
	for (cp = ppp_compressors; *cp != NULL; ++cp)
	    if ((*cp)->compress_proto == ccp_option[0]) {
		/*
		 * Found a handler for the protocol - try to allocate
		 * a compressor or decompressor.
		 */
		error = 0;
		if (odp->transmit) {
		    s = splsoftnet();
		    if (sc->sc_xc_state != NULL)
			(*sc->sc_xcomp->comp_free)(sc->sc_xc_state);
		    sc->sc_xcomp = *cp;
		    sc->sc_xc_state = (*cp)->comp_alloc(ccp_option, nb);
		    if (sc->sc_xc_state == NULL) {
			if (sc->sc_flags & SC_DEBUG)
			    printf("ppp%d: comp_alloc failed\n",
			       sc->sc_if.if_unit);
			error = ENOBUFS;
		    }
		    splimp();
		    sc->sc_flags &= ~SC_COMP_RUN;
		    splx(s);
		} else {
		    s = splsoftnet();
		    if (sc->sc_rc_state != NULL)
			(*sc->sc_rcomp->decomp_free)(sc->sc_rc_state);
		    sc->sc_rcomp = *cp;
		    sc->sc_rc_state = (*cp)->decomp_alloc(ccp_option, nb);
		    if (sc->sc_rc_state == NULL) {
			if (sc->sc_flags & SC_DEBUG)
			    printf("ppp%d: decomp_alloc failed\n",
			       sc->sc_if.if_unit);
			error = ENOBUFS;
		    }
		    splimp();
		    sc->sc_flags &= ~SC_DECOMP_RUN;
		    splx(s);
		}
		return (error);
	    }
	if (sc->sc_flags & SC_DEBUG)
	    printf("ppp%d: no compressor for [%x %x %x], %x\n",
		   sc->sc_if.if_unit, ccp_option[0], ccp_option[1],
		   ccp_option[2], nb);
	return (EINVAL);	/* no handler found */
#endif /* PPP_COMPRESS */

    case PPPIOCGNPMODE:
    case PPPIOCSNPMODE:
	npi = (struct npioctl *) data;
	switch (npi->protocol) {
	case PPP_IP:
	    npx = NP_IP;
	    break;
	default:
	    return EINVAL;
	}
	if (cmd == PPPIOCGNPMODE) {
	    npi->mode = sc->sc_npmode[npx];
	} else {
	    if (npi->mode != sc->sc_npmode[npx]) {
		s = splsoftnet();
		sc->sc_npmode[npx] = npi->mode;
		if (npi->mode != NPMODE_QUEUE) {
		    ppp_requeue(sc);
		    (*sc->sc_start)(sc);
		}
		splx(s);
	    }
	}
	break;

    case PPPIOCGIDLE:
	s = splsoftnet();
        microtime(&ppp_time);
	t = ppp_time.tv_sec;
	((struct ppp_idle *)data)->xmit_idle = t - sc->sc_last_sent;
	((struct ppp_idle *)data)->recv_idle = t - sc->sc_last_recv;
	splx(s);
	break;

#ifdef PPP_FILTER
    case PPPIOCSPASS:
    case PPPIOCSACTIVE:
	nbp = (struct bpf_program *) data;
	if ((unsigned) nbp->bf_len > BPF_MAXINSNS)
	    return EINVAL;
	newcodelen = nbp->bf_len * sizeof(struct bpf_insn);
	if (newcodelen != 0) {
	    MALLOC(newcode, struct bpf_insn *, newcodelen, M_DEVBUF, M_WAITOK);
	    if (newcode == 0) {
		return EINVAL;		/* or sumpin */
	    }
	    if ((error = copyin((caddr_t)nbp->bf_insns, (caddr_t)newcode,
			       newcodelen)) != 0) {
		FREE(newcode, M_DEVBUF);
		return error;
	    }
	    if (!bpf_validate(newcode, nbp->bf_len)) {
		FREE(newcode, M_DEVBUF);
		return EINVAL;
	    }
	} else
	    newcode = 0;
	bp = (cmd == PPPIOCSPASS)? &sc->sc_pass_filt: &sc->sc_active_filt;
	oldcode = bp->bf_insns;
	s = splimp();
	bp->bf_len = nbp->bf_len;
	bp->bf_insns = newcode;
	splx(s);
	if (oldcode != 0)
	    FREE(oldcode, M_DEVBUF);
	break;
#endif

    default:
	return (-1);
    }
    return (0);
}

/*
 * Process an ioctl request to the ppp network interface.
 */
static int
pppsioctl(struct ifnet *ifp, ioctl_command_t cmd, caddr_t data)
{
    /*struct proc *p = curproc;*/	/* XXX */
    register struct ppp_softc *sc = &ppp_softc[ifp->if_unit];
    register struct ifaddr *ifa = (struct ifaddr *)data;
    register struct ifreq *ifr = (struct ifreq *)data;
    struct ppp_stats *psp;
#ifdef	PPP_COMPRESS
    struct ppp_comp_stats *pcp;
#endif
    int s = splimp(), error = 0;

    switch (cmd) {
    case SIOCSIFFLAGS:
	if ((ifp->if_flags & IFF_RUNNING) == 0)
	    ifp->if_flags &= ~IFF_UP;
	break;

    case SIOCSIFADDR:
	if (ifa->ifa_addr->sa_family != AF_INET)
	    error = EAFNOSUPPORT;
	break;

    case SIOCSIFDSTADDR:
	if (ifa->ifa_addr->sa_family != AF_INET)
	    error = EAFNOSUPPORT;
	break;

    case SIOCSIFMTU:
	sc->sc_if.if_mtu = ifr->ifr_mtu;
	break;

    case SIOCGIFMTU:
	ifr->ifr_mtu = sc->sc_if.if_mtu;
	break;

    case SIOCADDMULTI:
    case SIOCDELMULTI:
	if (ifr == 0) {
	    error = EAFNOSUPPORT;
	    break;
	}
	switch(ifr->ifr_addr.sa_family) {
#ifdef INET
	case AF_INET:
	    break;
#endif
	default:
	    error = EAFNOSUPPORT;
	    break;
	}
	break;

    case SIO_RTEMS_SHOW_STATS:
        printf("              MRU:%-8u",   sc->sc_mru);
        printf("   Bytes received:%-8u",   sc->sc_stats.ppp_ibytes);
        printf(" Packets received:%-8u",   sc->sc_stats.ppp_ipackets);
        printf("   Receive errors:%-8u\n", sc->sc_stats.ppp_ierrors);
        printf("       Bytes sent:%-8u",   sc->sc_stats.ppp_obytes);
        printf("     Packets sent:%-8u",   sc->sc_stats.ppp_opackets);
        printf("  Transmit errors:%-8u\n", sc->sc_stats.ppp_oerrors);
	break;

    case SIOCGPPPSTATS:
	psp = &((struct ifpppstatsreq *) data)->stats;
	bzero(psp, sizeof(*psp));
	psp->p = sc->sc_stats;
#if defined(VJC) && !defined(SL_NO_STATS)
	if (sc->sc_comp) {
	    psp->vj.vjs_packets = sc->sc_comp->sls_packets;
	    psp->vj.vjs_compressed = sc->sc_comp->sls_compressed;
	    psp->vj.vjs_searches = sc->sc_comp->sls_searches;
	    psp->vj.vjs_misses = sc->sc_comp->sls_misses;
	    psp->vj.vjs_uncompressedin = sc->sc_comp->sls_uncompressedin;
	    psp->vj.vjs_compressedin = sc->sc_comp->sls_compressedin;
	    psp->vj.vjs_errorin = sc->sc_comp->sls_errorin;
	    psp->vj.vjs_tossed = sc->sc_comp->sls_tossed;
	}
#endif /* VJC */
	break;

#ifdef PPP_COMPRESS
    case SIOCGPPPCSTATS:
	pcp = &((struct ifpppcstatsreq *) data)->stats;
	bzero(pcp, sizeof(*pcp));
	if (sc->sc_xc_state != NULL)
	    (*sc->sc_xcomp->comp_stat)(sc->sc_xc_state, &pcp->c);
	if (sc->sc_rc_state != NULL)
	    (*sc->sc_rcomp->decomp_stat)(sc->sc_rc_state, &pcp->d);
	break;
#endif /* PPP_COMPRESS */

    default:
	error = EINVAL;
    }
    splx(s);
    return (error);
}

/*
 * Queue a packet.  Start transmission if not active.
 * Packet is placed in Information field of PPP frame.
 */
int
pppoutput(struct ifnet *ifp, struct mbuf *m0, struct sockaddr *dst,
    struct rtentry *rtp)
{
    register struct ppp_softc *sc = &ppp_softc[ifp->if_unit];
    int protocol, address, control;
    u_char *cp;
    int s, error;
    struct ip *ip;
    struct ifqueue *ifq;
    enum NPmode mode;
    int len;
    struct mbuf *m;

    if (sc->sc_devp == NULL || (ifp->if_flags & IFF_RUNNING) == 0
	|| ((ifp->if_flags & IFF_UP) == 0 && dst->sa_family != AF_UNSPEC)) {
	error = ENETDOWN;	/* sort of */
	goto bad;
    }

    /*
     * Compute PPP header.
     */
    m0->m_flags &= ~M_HIGHPRI;
    switch (dst->sa_family) {
#ifdef INET
    case AF_INET:
	address = PPP_ALLSTATIONS;
	control = PPP_UI;
	protocol = PPP_IP;
	mode = sc->sc_npmode[NP_IP];

	/*
	 * If this packet has the "low delay" bit set in the IP header,
	 * put it on the fastq instead.
	 */
	ip = mtod(m0, struct ip *);
	if (ip->ip_tos & IPTOS_LOWDELAY)
	    m0->m_flags |= M_HIGHPRI;
	break;
#endif
    case AF_UNSPEC:
	address = PPP_ADDRESS(dst->sa_data);
	control = PPP_CONTROL(dst->sa_data);
	protocol = PPP_PROTOCOL(dst->sa_data);
	mode = NPMODE_PASS;
	break;
    default:
	printf("ppp%d: af%d not supported\n", ifp->if_unit, dst->sa_family);
	error = EAFNOSUPPORT;
	goto bad;
    }

    /*
     * Drop this packet, or return an error, if necessary.
     */
    if (mode == NPMODE_ERROR) {
	error = ENETDOWN;
	goto bad;
    }
    if (mode == NPMODE_DROP) {
	error = 0;
	goto bad;
    }

    /*
     * Add PPP header.  If no space in first mbuf, allocate another.
     * (This assumes M_LEADINGSPACE is always 0 for a cluster mbuf.)
     */
    if (M_LEADINGSPACE(m0) < PPP_HDRLEN) {
	m0 = m_prepend(m0, PPP_HDRLEN, M_DONTWAIT);
	if (m0 == 0) {
	    error = ENOBUFS;
	    goto bad;
	}
	m0->m_len = 0;
    } else
	m0->m_data -= PPP_HDRLEN;

    cp = mtod(m0, u_char *);
    *cp++ = address;
    *cp++ = control;
    *cp++ = protocol >> 8;
    *cp++ = protocol & 0xff;
    m0->m_len += PPP_HDRLEN;

    len = 0;
    for (m = m0; m != 0; m = m->m_next)
	len += m->m_len;

    if (sc->sc_flags & SC_LOG_OUTPKT) {
	printf("ppp%d output: ", ifp->if_unit);
	pppdumpm(m0);
    }

    if ((protocol & 0x8000) == 0) {
#ifdef PPP_FILTER
	/*
	 * Apply the pass and active filters to the packet,
	 * but only if it is a data packet.
	 */
	*mtod(m0, u_char *) = 1;	/* indicates outbound */
	if (sc->sc_pass_filt.bf_insns != 0
	    && bpf_filter(sc->sc_pass_filt.bf_insns, (u_char *) m0,
			  len, 0) == 0) {
	    error = 0;		/* drop this packet */
	    goto bad;
	}

	/*
	 * Update the time we sent the most recent packet.
	 */
	if (sc->sc_active_filt.bf_insns == 0
	    || bpf_filter(sc->sc_active_filt.bf_insns, (u_char *) m0, len, 0))
	    sc->sc_last_sent = time.tv_sec;

	*mtod(m0, u_char *) = address;
#else
	/*
	 * Update the time we sent the most recent data packet.
	 */
	microtime(&ppp_time);
	sc->sc_last_sent = ppp_time.tv_sec;
#endif /* PPP_FILTER */
    }

#if NBPFILTER > 0
    /*
     * See if bpf wants to look at the packet.
     */
    if (sc->sc_bpf)
	bpf_mtap(sc->sc_bpf, m0);
#endif

    /*
     * Put the packet on the appropriate queue.
     */
    s = splsoftnet();
    if (mode == NPMODE_QUEUE) {
	/* XXX we should limit the number of packets on this queue */
	*sc->sc_npqtail = m0;
	m0->m_nextpkt = NULL;
	sc->sc_npqtail = &m0->m_nextpkt;
    } else {
	ifq = (m0->m_flags & M_HIGHPRI)? &sc->sc_fastq: &ifp->if_snd;
	if (IF_QFULL(ifq) && dst->sa_family != AF_UNSPEC) {
	    IF_DROP(ifq);
	    splx(s);
	    sc->sc_if.if_oerrors++;
	    sc->sc_stats.ppp_oerrors++;
	    error = ENOBUFS;
	    goto bad;
	}
	IF_ENQUEUE(ifq, m0);
	(*sc->sc_start)(sc);
    }
    ifp->if_lastchange = ppp_time;
    ifp->if_opackets++;
    ifp->if_obytes += len;

    splx(s);
    return (0);

bad:
    m_freem(m0);
    return (error);
}

/*
 * After a change in the NPmode for some NP, move packets from the
 * npqueue to the send queue or the fast queue as appropriate.
 * Should be called at spl[soft]net.
 */
static void
ppp_requeue(struct ppp_softc *sc)
{
    struct mbuf *m, **mpp;
    struct ifqueue *ifq;
    enum NPmode mode;

    for (mpp = &sc->sc_npqueue; (m = *mpp) != NULL; ) {
	switch (PPP_PROTOCOL(mtod(m, u_char *))) {
	case PPP_IP:
	    mode = sc->sc_npmode[NP_IP];
	    break;
	default:
	    mode = NPMODE_PASS;
	}

	switch (mode) {
	case NPMODE_PASS:
	    /*
	     * This packet can now go on one of the queues to be sent.
	     */
	    *mpp = m->m_nextpkt;
	    m->m_nextpkt = NULL;
	    ifq = (m->m_flags & M_HIGHPRI)? &sc->sc_fastq: &sc->sc_if.if_snd;
	    if (IF_QFULL(ifq)) {
		IF_DROP(ifq);
		sc->sc_if.if_oerrors++;
		sc->sc_stats.ppp_oerrors++;
	    } else
		IF_ENQUEUE(ifq, m);
	    break;

	case NPMODE_DROP:
	case NPMODE_ERROR:
	    *mpp = m->m_nextpkt;
	    m_freem(m);
	    break;

	case NPMODE_QUEUE:
	    mpp = &m->m_nextpkt;
	    break;
	}
    }
    sc->sc_npqtail = mpp;
}

/*
 * Get a packet to send.  This procedure is intended to be called at
 * splsoftnet, since it may involve time-consuming operations such as
 * applying VJ compression, packet compression, address/control and/or
 * protocol field compression to the packet.
 */
struct mbuf *
ppp_dequeue(struct ppp_softc *sc)
{
#ifdef PPP_COMPRESS
    struct mbuf *mp;
#endif
    struct mbuf *m;
    u_char *cp;
    int address, control, protocol;

    /*
     * Grab a packet to send: first try the fast queue, then the
     * normal queue.
     */
    rtems_bsdnet_semaphore_obtain();
    IF_DEQUEUE(&sc->sc_fastq, m);
    if (m == NULL)
	IF_DEQUEUE(&sc->sc_if.if_snd, m);
    rtems_bsdnet_semaphore_release();

    if (m == NULL)
	return NULL;

    ++sc->sc_stats.ppp_opackets;

    /*
     * Extract the ppp header of the new packet.
     * The ppp header will be in one mbuf.
     */
    cp = mtod(m, u_char *);
    address = PPP_ADDRESS(cp);
    control = PPP_CONTROL(cp);
    protocol = PPP_PROTOCOL(cp);

    switch (protocol) {
    case PPP_IP:
#ifdef VJC
	/*
	 * If the packet is a TCP/IP packet, see if we can compress it.
	 */
	if ((sc->sc_flags & SC_COMP_TCP) && sc->sc_comp != NULL) {
	    struct ip *ip;
	    int type;

	    mp = m;
	    ip = (struct ip *) (cp + PPP_HDRLEN);
	    if (mp->m_len <= PPP_HDRLEN) {
		mp = mp->m_next;
		if (mp == NULL)
		    break;
		ip = mtod(mp, struct ip *);
	    }
	    /* this code assumes the IP/TCP header is in one non-shared mbuf */
	    if (ip->ip_p == IPPROTO_TCP) {
		type = sl_compress_tcp(mp, ip, sc->sc_comp,
				       !(sc->sc_flags & SC_NO_TCP_CCID));
		switch (type) {
		case TYPE_UNCOMPRESSED_TCP:
		    protocol = PPP_VJC_UNCOMP;
		    break;
		case TYPE_COMPRESSED_TCP:
		    protocol = PPP_VJC_COMP;
		    cp = mtod(m, u_char *);
		    cp[0] = address;	/* header has moved */
		    cp[1] = control;
		    cp[2] = 0;
		    break;
		}
		cp[3] = protocol;	/* update protocol in PPP header */
	    }
	}
#endif	/* VJC */
	break;

#ifdef PPP_COMPRESS
    case PPP_CCP:
	ppp_ccp(sc, m, 0);
	break;
#endif	/* PPP_COMPRESS */
    }

#ifdef PPP_COMPRESS
    if (protocol != PPP_LCP && protocol != PPP_CCP
	&& sc->sc_xc_state && (sc->sc_flags & SC_COMP_RUN)) {
	struct mbuf *mcomp = NULL;
	int slen, clen;

	slen = 0;
	for (mp = m; mp != NULL; mp = mp->m_next)
	    slen += mp->m_len;
	clen = (*sc->sc_xcomp->compress)
	    (sc->sc_xc_state, &mcomp, m, slen, sc->sc_if.if_mtu + PPP_HDRLEN);
	if (mcomp != NULL) {
	    if (sc->sc_flags & SC_CCP_UP) {
		/* Send the compressed packet instead of the original. */
		m_freem(m);
		m = mcomp;
		cp = mtod(m, u_char *);
		protocol = cp[3];
	    } else {
		/* Can't transmit compressed packets until CCP is up. */
		m_freem(mcomp);
	    }
	}
    }
#endif	/* PPP_COMPRESS */

    /*
     * Compress the address/control and protocol, if possible.
     */
    if (sc->sc_flags & SC_COMP_AC && address == PPP_ALLSTATIONS &&
	control == PPP_UI && protocol != PPP_ALLSTATIONS &&
	protocol != PPP_LCP) {
	/* can compress address/control */
	m->m_data += 2;
	m->m_len -= 2;
    }
    if (sc->sc_flags & SC_COMP_PROT && protocol < 0xFF) {
	/* can compress protocol */
	if (mtod(m, u_char *) == cp) {
	    cp[2] = cp[1];	/* move address/control up */
	    cp[1] = cp[0];
	}
	++m->m_data;
	--m->m_len;
    }

    return m;
}

#ifdef PPP_COMPRESS
/*
 * Handle a CCP packet.  `rcvd' is 1 if the packet was received,
 * 0 if it is about to be transmitted.
 */
static void
ppp_ccp(struct ppp_softc *sc, struct mbuf *m, int rcvd)
{
    u_char *dp, *ep;
    struct mbuf *mp;
    int slen, s;

    /*
     * Get a pointer to the data after the PPP header.
     */
    if (m->m_len <= PPP_HDRLEN) {
	mp = m->m_next;
	if (mp == NULL)
	    return;
	dp = (mp != NULL)? mtod(mp, u_char *): NULL;
    } else {
	mp = m;
	dp = mtod(mp, u_char *) + PPP_HDRLEN;
    }

    ep = mtod(mp, u_char *) + mp->m_len;
    if (dp + CCP_HDRLEN > ep)
	return;
    slen = CCP_LENGTH(dp);
    if (dp + slen > ep) {
	if (sc->sc_flags & SC_DEBUG)
	    printf("if_ppp/ccp: not enough data in mbuf (%p+%x > %p+%x)\n",
		   dp, slen, mtod(mp, u_char *), mp->m_len);
	return;
    }

    switch (CCP_CODE(dp)) {
    case CCP_CONFREQ:
    case CCP_TERMREQ:
    case CCP_TERMACK:
	/* CCP must be going down - disable compression */
	if (sc->sc_flags & SC_CCP_UP) {
	    s = splimp();
	    sc->sc_flags &= ~(SC_CCP_UP | SC_COMP_RUN | SC_DECOMP_RUN);
	    splx(s);
	}
	break;

    case CCP_CONFACK:
	if (sc->sc_flags & SC_CCP_OPEN && !(sc->sc_flags & SC_CCP_UP)
	    && slen >= CCP_HDRLEN + CCP_OPT_MINLEN
	    && slen >= CCP_OPT_LENGTH(dp + CCP_HDRLEN) + CCP_HDRLEN) {
	    if (!rcvd) {
		/* we're agreeing to send compressed packets. */
		if (sc->sc_xc_state != NULL
		    && (*sc->sc_xcomp->comp_init)
			(sc->sc_xc_state, dp + CCP_HDRLEN, slen - CCP_HDRLEN,
			 sc->sc_if.if_unit, 0, sc->sc_flags & SC_DEBUG)) {
		    s = splimp();
		    sc->sc_flags |= SC_COMP_RUN;
		    splx(s);
		}
	    } else {
		/* peer is agreeing to send compressed packets. */
		if (sc->sc_rc_state != NULL
		    && (*sc->sc_rcomp->decomp_init)
			(sc->sc_rc_state, dp + CCP_HDRLEN, slen - CCP_HDRLEN,
			 sc->sc_if.if_unit, 0, sc->sc_mru,
			 sc->sc_flags & SC_DEBUG)) {
		    s = splimp();
		    sc->sc_flags |= SC_DECOMP_RUN;
		    sc->sc_flags &= ~(SC_DC_ERROR | SC_DC_FERROR);
		    splx(s);
		}
	    }
	}
	break;

    case CCP_RESETACK:
	if (sc->sc_flags & SC_CCP_UP) {
	    if (!rcvd) {
		if (sc->sc_xc_state && (sc->sc_flags & SC_COMP_RUN))
		    (*sc->sc_xcomp->comp_reset)(sc->sc_xc_state);
	    } else {
		if (sc->sc_rc_state && (sc->sc_flags & SC_DECOMP_RUN)) {
		    (*sc->sc_rcomp->decomp_reset)(sc->sc_rc_state);
		    s = splimp();
		    sc->sc_flags &= ~SC_DC_ERROR;
		    splx(s);
		}
	    }
	}
	break;
    }
}

/*
 * CCP is down; free (de)compressor state if necessary.
 */
static void
ppp_ccp_closed(struct ppp_softc *sc)
{
    if (sc->sc_xc_state) {
	(*sc->sc_xcomp->comp_free)(sc->sc_xc_state);
	sc->sc_xc_state = NULL;
    }
    if (sc->sc_rc_state) {
	(*sc->sc_rcomp->decomp_free)(sc->sc_rc_state);
	sc->sc_rc_state = NULL;
    }
}
#endif /* PPP_COMPRESS */

/*
 * Process a received PPP packet, doing decompression as necessary.
 * Should be called at splsoftnet.
 */
#define COMPTYPE(proto)	((proto) == PPP_VJC_COMP? TYPE_COMPRESSED_TCP: \
			 TYPE_UNCOMPRESSED_TCP)

static struct mbuf *
ppp_inproc(struct ppp_softc *sc, struct mbuf *m)
{
    struct mbuf  *mf = (struct mbuf *)0;
    struct ifnet *ifp = &sc->sc_if;
    struct ifqueue *inq;
    int s, ilen, proto, rv; 
    u_char *cp, adrs, ctrl;
    struct mbuf *mp;
#ifdef PPP_COMPRESS
    struct mbuf *dmp = NULL;
#endif
#ifdef VJC
    u_char *iphdr;
    u_int hlen; 
    int xlen;
#endif

    sc->sc_stats.ppp_ipackets++;

    if (sc->sc_flags & SC_LOG_INPKT) {
	ilen = 0;
	for (mp = m; mp != NULL; mp = mp->m_next)
	    ilen += mp->m_len;
	printf("ppp%d: got %d bytes\n", ifp->if_unit, ilen);
	pppdumpm(m);
    }

    cp = mtod(m, u_char *);
    adrs = PPP_ADDRESS(cp);
    ctrl = PPP_CONTROL(cp);
    proto = PPP_PROTOCOL(cp);

    if (m->m_flags & M_ERRMARK) {
	m->m_flags &= ~M_ERRMARK;
	s = splimp();
	sc->sc_flags |= SC_VJ_RESET;
	splx(s);
    }

#ifdef PPP_COMPRESS
    /*
     * Decompress this packet if necessary, update the receiver's
     * dictionary, or take appropriate action on a CCP packet.
     */
    if (proto == PPP_COMP && sc->sc_rc_state && (sc->sc_flags & SC_DECOMP_RUN)
	&& !(sc->sc_flags & SC_DC_ERROR) && !(sc->sc_flags & SC_DC_FERROR)) {
	/* decompress this packet */
	rv = (*sc->sc_rcomp->decompress)(sc->sc_rc_state, m, &dmp);
	if (rv == DECOMP_OK) {
	    m_freem(m);
	    if (dmp == NULL) {
		/* no error, but no decompressed packet produced */
		return mf;
	    }
	    m = dmp;
	    cp = mtod(m, u_char *);
	    proto = PPP_PROTOCOL(cp);

	} else {
	    /*
	     * An error has occurred in decompression.
	     * Pass the compressed packet up to pppd, which may take
	     * CCP down or issue a Reset-Req.
	     */
	    if (sc->sc_flags & SC_DEBUG)
		printf("ppp%d: decompress failed %d\n", ifp->if_unit, rv);
	    s = splimp();
	    sc->sc_flags |= SC_VJ_RESET;
	    if (rv == DECOMP_ERROR)
		sc->sc_flags |= SC_DC_ERROR;
	    else
		sc->sc_flags |= SC_DC_FERROR;
	    splx(s);
	}

    } else {
	if (sc->sc_rc_state && (sc->sc_flags & SC_DECOMP_RUN)) {
	    (*sc->sc_rcomp->incomp)(sc->sc_rc_state, m);
	}
	if (proto == PPP_CCP) {
	    ppp_ccp(sc, m, 1);
	}
    }
#endif

    ilen = 0;
    for (mp = m; mp != NULL; mp = mp->m_next)
	ilen += mp->m_len;

#ifdef VJC
    if (sc->sc_flags & SC_VJ_RESET) {
	/*
	 * If we've missed a packet, we must toss subsequent compressed
	 * packets which don't have an explicit connection ID.
	 */
	if (sc->sc_comp)
	    sl_uncompress_tcp(NULL, 0, TYPE_ERROR, sc->sc_comp);
	s = splimp();
	sc->sc_flags &= ~SC_VJ_RESET;
	splx(s);
    }

    /*
     * See if we have a VJ-compressed packet to uncompress.
     */
    if (proto == PPP_VJC_COMP) {
	if ((sc->sc_flags & SC_REJ_COMP_TCP) || sc->sc_comp == 0)
	    goto bad;

	xlen = sl_uncompress_tcp_core(cp + PPP_HDRLEN, m->m_len - PPP_HDRLEN,
				      ilen - PPP_HDRLEN, TYPE_COMPRESSED_TCP,
				      sc->sc_comp, &iphdr, &hlen);

	if (xlen <= 0) {
	    if (sc->sc_flags & SC_DEBUG)
		printf("ppp%d: VJ uncompress failed on type comp\n",
			ifp->if_unit);
	    goto bad;
	}

	/* Copy the PPP and IP headers into a new mbuf. */
	MGETHDR(mp, M_DONTWAIT, MT_DATA);
	if (mp == NULL)
	    goto bad;
	mp->m_len = 0;
	mp->m_next = NULL;
	if (hlen + PPP_HDRLEN > MHLEN) {
	    MCLGET(mp, M_DONTWAIT);
	    if (M_TRAILINGSPACE(mp) < hlen + PPP_HDRLEN) {
		m_freem(mp);
		goto bad;	/* lose if big headers and no clusters */
	    }
	}
#ifdef MAC
	mac_create_mbuf_from_mbuf(m, mp);
#endif
	cp = mtod(mp, u_char *);
	cp[0] = adrs;
	cp[1] = ctrl;
	cp[2] = 0;
	cp[3] = PPP_IP;
	proto = PPP_IP;
	bcopy(iphdr, cp + PPP_HDRLEN, hlen);
	mp->m_len = hlen + PPP_HDRLEN;

	/*
	 * Trim the PPP and VJ headers off the old mbuf
	 * and stick the new and old mbufs together.
	 */
	m->m_data += PPP_HDRLEN + xlen;
	m->m_len -= PPP_HDRLEN + xlen;
	if (m->m_len <= M_TRAILINGSPACE(mp)) {
	    bcopy(mtod(m, u_char *), mtod(mp, u_char *) + mp->m_len, m->m_len);
	    mp->m_len += m->m_len;
	    MFREE(m, mp->m_next);
	} else
	    mp->m_next = m;
	m = mp;
	ilen += hlen - xlen;

    } else if (proto == PPP_VJC_UNCOMP) {
	if ((sc->sc_flags & SC_REJ_COMP_TCP) || sc->sc_comp == 0)
	    goto bad;

	xlen = sl_uncompress_tcp_core(cp + PPP_HDRLEN, m->m_len - PPP_HDRLEN,
				      ilen - PPP_HDRLEN, TYPE_UNCOMPRESSED_TCP,
				      sc->sc_comp, &iphdr, &hlen);

	if (xlen < 0) {
	    if (sc->sc_flags & SC_DEBUG)
		printf("ppp%d: VJ uncompress failed on type uncomp\n",
			ifp->if_unit);
	    goto bad;
	}

	proto = PPP_IP;
	cp[3] = PPP_IP;
    }
#endif /* VJC */

    /*
     * If the packet will fit in a header mbuf, don't waste a
     * whole cluster on it.
     */
    if (ilen <= MHLEN && M_IS_CLUSTER(m)) {
	MGETHDR(mp, M_DONTWAIT, MT_DATA);
	if (mp != NULL) {
	    m_copydata(m, 0, ilen, mtod(mp, caddr_t));
            /* instead of freeing - return cluster mbuf so it can be reused */
            /* m_freem(m); */
            mf = m;
	    m = mp;
	    m->m_len = ilen;
	}
    }
    m->m_pkthdr.len = ilen;
    m->m_pkthdr.rcvif = ifp;

    if ((proto & 0x8000) == 0) {
#ifdef PPP_FILTER
	/*
	 * See whether we want to pass this packet, and
	 * if it counts as link activity.
	 */
	adrs = *mtod(m, u_char *);	/* save address field */
	*mtod(m, u_char *) = 0;		/* indicate inbound */
	if (sc->sc_pass_filt.bf_insns != 0
	    && bpf_filter(sc->sc_pass_filt.bf_insns, (u_char *) m,
			  ilen, 0) == 0) {
	    /* drop this packet */
	    m_freem(m);
	    return mf;
	}
	if (sc->sc_active_filt.bf_insns == 0
	    || bpf_filter(sc->sc_active_filt.bf_insns, (u_char *) m, ilen, 0))
	    sc->sc_last_recv = time.tv_sec;

	*mtod(m, u_char *) = adrs;
#else
	/*
	 * Record the time that we received this packet.
	 */
	microtime(&ppp_time);
	sc->sc_last_recv = ppp_time.tv_sec;
#endif /* PPP_FILTER */
    }

#if NBPFILTER > 0
    /* See if bpf wants to look at the packet. */
    if (sc->sc_bpf)
	bpf_mtap(sc->sc_bpf, m);
#endif

    rv = 0;
    switch (proto) {
#ifdef INET
    case PPP_IP:
	/*
	 * IP packet - take off the ppp header and pass it up to IP.
	 */
	if ((ifp->if_flags & IFF_UP) == 0
	    || sc->sc_npmode[NP_IP] != NPMODE_PASS) {
	    /* interface is down - drop the packet. */
	    m_freem(m);
	    return mf;
	}
	m->m_pkthdr.len -= PPP_HDRLEN;
	m->m_data += PPP_HDRLEN;
	m->m_len -= PPP_HDRLEN;
	schednetisr(NETISR_IP);
	inq = &ipintrq;
	break;
#endif

    default:
	/*
	 * Some other protocol - place on input queue for read().
	 */
	inq = &sc->sc_inq;
	rv = 1;
	break;
    }

    /*
     * Put the packet on the appropriate input queue.
     */
    s = splimp();
    if (IF_QFULL(inq)) {
	IF_DROP(inq);
	splx(s);
	if (sc->sc_flags & SC_DEBUG)
	    printf("ppp%d: input queue full\n", ifp->if_unit);
	ifp->if_iqdrops++;
	goto bad;
    }
    IF_ENQUEUE(inq, m);
    splx(s);

    ifp->if_ipackets++;
    ifp->if_ibytes += ilen;
    microtime(&ppp_time);
    ifp->if_lastchange = ppp_time;

    if (rv) {
      (*sc->sc_ctlp)(sc);
    }

    return mf;

 bad:
    m_freem(m);
    sc->sc_if.if_ierrors++;
    sc->sc_stats.ppp_ierrors++;
    return mf;
}

#define MAX_DUMP_BYTES	128

static void
pppdumpm(struct mbuf *m0)
{
    char buf[3*MAX_DUMP_BYTES+4];
    char *bp = buf;
    struct mbuf *m;
    static char digits[] = "0123456789abcdef";

    for (m = m0; m; m = m->m_next) {
	int l = m->m_len;
	u_char *rptr = (u_char *)m->m_data;

	while (l--) {
	    if (bp > buf + sizeof(buf) - 4)
		goto done;
	    *bp++ = digits[*rptr >> 4]; /* convert byte to ascii hex */
	    *bp++ = digits[*rptr++ & 0xf];
	}

	if (m->m_next) {
	    if (bp > buf + sizeof(buf) - 3)
		goto done;
	    *bp++ = '|';
	} else
	    *bp++ = ' ';
    }
done:
    if (m)
	*bp++ = '>';
    *bp = 0;
    printf("%s\n", buf);
}

#endif	/* NPPP > 0 */

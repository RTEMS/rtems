/*
 * Created from if_ethersubr.c by Andy Dachs <a.dachs@sstl.co.uk>
 * Surrey Satellite Technology Limited (SSTL), 2001
 * Modified (hacked) to support IP frames transmitted over HDLC. This
 * all needs tidying up in future but it does actually work for point
 * to point communications. I have an SDL WANic400 synchronous
 * communications card that comes with Windows NT drivers.  The drivers
 * support a mode that they call "Ethernet Emulation".  That simply
 * puts the IP frame in an HDLC frame without any ethernet header. i.e.
 * <HDLC Flag><IpFrame><CRC><HDLC Flag>.  There is no addressing beyond
 * the IP header information so is only suitable for point to point links
 * with a single protocol. "At some point" I will add a Frame Relay header
 * but at the moment I have difficulties getting the WANic card driver's
 * Frame Relay driver to work.
 *
 * Copyright (c) 1982, 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)if_ethersubr.c	8.1 (Berkeley) 6/10/93
 */

#include <rtems/rtems_bsdnet.h>

#include <sys/param.h>
/*#include <sys/systm.h>
#include <sys/kernel.h> */
#define KERNEL
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/syslog.h>
#include <sys/sysctl.h>

#include <net/if.h>
#include <net/netisr.h>
#include <net/route.h>
#include <net/if_llc.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/ethernet.h>

#include <netinet/in.h>
#include <netinet/in_var.h>
#include <netinet/if_ether.h>

#include <stdio.h>

#include "if_hdlcsubr.h"

/*
u_char	etherbroadcastaddr[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
*/
#define senderr(e) { error = (e); goto bad;}

/*
 * HDLC output routine.
 * Just transmit the packet (hardware adds flags and CRC)
 */
int
hdlc_output(ifp, m0, dst, rt0)
	register struct ifnet *ifp;
	struct mbuf *m0;
	struct sockaddr *dst;
	struct rtentry *rt0;
{
	short type;
	int s, error = 0;
#if 0
	u_char  edst[6];
#endif
	register struct mbuf *m = m0;
	register struct rtentry *rt;
	struct mbuf *mcopy = (struct mbuf *)0;
/*	register struct ether_header *eh; */
	int off, len = m->m_pkthdr.len;

/*	printk( "hdlc output" ); */
/*	struct arpcom *ac = (struct arpcom *)ifp; */

	if ((ifp->if_flags & (IFF_UP|IFF_RUNNING)) != (IFF_UP|IFF_RUNNING))
		senderr(ENETDOWN);
	rt = rt0;
	if (rt) {
		if ((rt->rt_flags & RTF_UP) == 0) {
			rt0 = rt = rtalloc1(dst, 1, 0UL);
			if (rt0)
				rt->rt_refcnt--;
			else
				senderr(EHOSTUNREACH);
		}
		if (rt->rt_flags & RTF_GATEWAY) {
			if (rt->rt_gwroute == 0)
				goto lookup;
			if (((rt = rt->rt_gwroute)->rt_flags & RTF_UP) == 0) {
				rtfree(rt); rt = rt0;
			lookup: rt->rt_gwroute = rtalloc1(rt->rt_gateway, 1,
							  0UL);
				if ((rt = rt->rt_gwroute) == 0)
					senderr(EHOSTUNREACH);
			}
		}
		if (rt->rt_flags & RTF_REJECT)
			if (rt->rt_rmx.rmx_expire == 0 ||
			    rtems_bsdnet_seconds_since_boot() < rt->rt_rmx.rmx_expire)
				senderr(rt == rt0 ? EHOSTDOWN : EHOSTUNREACH);
	}
	switch (dst->sa_family) {

	case AF_INET:
#if 0
		if (!arpresolve(ac, rt, m, dst, edst, rt0))
			return (0);	/* if not yet resolved */
#endif

		/* If broadcasting on a simplex interface, loopback a copy */
		if ((m->m_flags & M_BCAST) && (ifp->if_flags & IFF_SIMPLEX))
			mcopy = m_copy(m, 0, (int)M_COPYALL);
		off = m->m_pkthdr.len - m->m_len;
		type = htons(ETHERTYPE_IP);
		break;
#if 0
	case AF_UNSPEC:
		eh = (struct ether_header *)dst->sa_data;
 		(void)memcpy(edst, eh->ether_dhost, sizeof (edst));
		type = eh->ether_type;
		break;
#endif
	default:
		printf("%s%d: can't handle af%d\n", ifp->if_name, ifp->if_unit,
			dst->sa_family);
		senderr(EAFNOSUPPORT);
	}

	if (mcopy)
		(void) looutput(ifp, mcopy, dst, rt);
#if 0
	/*
	 * Add local net header.  If no space in first mbuf,
	 * allocate another.
	 */
	M_PREPEND(m, sizeof (struct ether_header), M_DONTWAIT);
#endif

	if (m == 0)
		senderr(ENOBUFS);

#if 0
	eh = mtod(m, struct ether_header *);
	(void)memcpy(&eh->ether_type, &type,
		sizeof(eh->ether_type));
 	(void)memcpy(eh->ether_dhost, edst, sizeof (edst));
 	(void)memcpy(eh->ether_shost, ac->ac_enaddr,
	    sizeof(eh->ether_shost));
#endif

	s = splimp();
	/*
	 * Queue message on interface, and start output if interface
	 * not yet active.
	 */
	if (IF_QFULL(&ifp->if_snd)) {
		IF_DROP(&ifp->if_snd);
		splx(s);
		senderr(ENOBUFS);
	}
	IF_ENQUEUE(&ifp->if_snd, m);
	if ((ifp->if_flags & IFF_OACTIVE) == 0)
		(*ifp->if_start)(ifp);
	splx(s);

	ifp->if_obytes += len /*+ sizeof (struct ether_header)*/;
	if (m->m_flags & M_MCAST)
		ifp->if_omcasts++;
	return (error);

bad:
	if (m)
		m_freem(m);
	return (error);
}

/*
 * Process a received Ethernet packet;
 * the packet is in the mbuf chain m without
 * the ether header, which is provided separately.
 */
void
hdlc_input(ifp, m)
	struct ifnet *ifp;
	struct mbuf *m;
{
	register struct ifqueue *inq;
	int s;

	struct ether_header eh;

	if ((ifp->if_flags & IFF_UP) == 0) {
		m_freem(m);
		return;
	}
	ifp->if_ibytes += m->m_pkthdr.len;
/*
	if (memcmp((caddr_t)etherbroadcastaddr, (caddr_t)eh->ether_dhost,
	    sizeof(etherbroadcastaddr)) == 0)
		m->m_flags |= M_BCAST;
	else if (eh->ether_dhost[0] & 1)
		m->m_flags |= M_MCAST;
*/
	if (m->m_flags & (M_BCAST|M_MCAST))
		ifp->if_imcasts++;

	/*
	 * RTEMS addition -- allow application to `tap into'
	 * the incoming packet stream.
	 */
	if (ifp->if_tap && (*ifp->if_tap)(ifp, &eh, m)) {
		m_freem(m);
		return;
	}

	schednetisr(NETISR_IP);
	inq = &ipintrq;

	s = splimp();
	if (IF_QFULL(inq)) {
	  IF_DROP(inq);
	  m_freem(m);
	} else
  	  IF_ENQUEUE(inq, m);
	splx(s);
}

/*
 * Perform common duties while attaching to interface list
 */
void
hdlc_ifattach(ifp)
	register struct ifnet *ifp;
{
	register struct ifaddr *ifa;
	register struct sockaddr_dl *sdl;

	ifp->if_type = IFT_ETHER;
	ifp->if_addrlen = 0;
	ifp->if_hdrlen  = 0;
	ifp->if_mtu  = 2048;
	if (ifp->if_baudrate == 0)
	    ifp->if_baudrate = 8000000;
	for (ifa = ifp->if_addrlist; ifa; ifa = ifa->ifa_next)
		if ((sdl = (struct sockaddr_dl *)ifa->ifa_addr) &&
		    sdl->sdl_family == AF_LINK) {
			sdl->sdl_type = IFT_ETHER;
			sdl->sdl_alen = ifp->if_addrlen;
/*
			memcpy(LLADDR(sdl),
			      (caddr_t)((struct arpcom *)ifp)->ac_enaddr,
			      ifp->if_addrlen);
*/
			break;
		}
}

/* SYSCTL_NODE(_net_link, IFT_ETHER, ether, CTLFLAG_RW, 0, "Ethernet"); */

int
hdlc_ioctl(struct ifnet *ifp, int command, caddr_t data)
{
	struct ifaddr *ifa = (struct ifaddr *) data;
	struct ifreq *ifr = (struct ifreq *) data;
	int error = 0;

	switch (command) {
	case SIOCSIFADDR:
		ifp->if_flags |= IFF_UP;

		switch (ifa->ifa_addr->sa_family) {
#if 0
#ifdef INET
		case AF_INET:
			ifp->if_init(ifp->if_softc);	/* before arpwhohas */

			arp_ifinit((struct arpcom *)ifp, ifa);
			break;
#endif
#endif
		default:
			ifp->if_init(ifp->if_softc);
			break;
		}
		break;

	case SIOCGIFADDR:
		{
			struct sockaddr *sa;

			sa = (struct sockaddr *) & ifr->ifr_data;
/*
			memcpy((caddr_t) sa->sa_data,
			      ((struct arpcom *)ifp->if_softc)->ac_enaddr,
			      ETHER_ADDR_LEN);
*/
		}
		break;

	case SIOCSIFMTU:
		/*
		 * Set the interface MTU.
		 */
		if (ifr->ifr_mtu > ETHERMTU) {
			error = EINVAL;
		} else {
			ifp->if_mtu = ifr->ifr_mtu;
		}
		break;
	}
	return (error);
}

/*
 * Copyright (c) 1982, 1986, 1988, 1990, 1993, 1995
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
 *	@(#)udp_usrreq.c	8.6 (Berkeley) 5/23/95
 *	$Id$
 */


#include <sys/param.h>
#include <sys/queue.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/kernel.h>
#include <sys/sysctl.h>
#include <sys/syslog.h>

#include <net/if.h>
#include <net/pf.h>
#include <net/route.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/in_pcb.h>
#include <netinet/in_var.h>
#include <netinet/ip_var.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <netinet/udp_var.h>
#include <time.h>

#undef malloc
#undef free

/*
 * UDP protocol implementation.
 * Per RFC 768, August, 1980.
 */
#ifndef	COMPAT_42
static int	udpcksum = 1;
#else
static int	udpcksum = 0;		/* XXX */
#endif
SYSCTL_INT(_net_inet_udp, UDPCTL_CHECKSUM, checksum, CTLFLAG_RW,
		&udpcksum, 0, "Calculate UDP checksum");
		

#ifdef FORWARD_PROTOCOL

static int
udp_store(struct mbuf *m);

static int
udp_doutput(struct in_addr dst);


/*
 * To implement udp broadcast forwarding we should check ``broadcast storm''
 * condition that can be caused if there is alternate ways between two subnets
 * and efficiently cut such loops. This can be done by caching sensetive packet
 * information and performing tests on it:
 * 1. If we got packet that is not in our cache we pass this packet and
 *    add it into cache. If there is no room in cache - LRU packet is discarded
 * 2. If we got packet that already in our cache we make it MRU and:          
 *    We check ttl of ip packet - if it same or greater as in cache we just 
 *    pass it. If it less - just drop this packet.
 */

#define MAXUDPCACHE  10  /* Seems to be reasonable size */
#define UDPMAXEXPIRE 30  /* 30 sec expire cache         */

struct packet_cache {
    unsigned8      pc_ttl;   /* IP packet TTL */
    unsigned16     pc_sum;   /* UDP packet checksum */
    struct in_addr pc_src;   /* IP packet source address*/
    struct udphdr  pc_uh;    /* UDP packet header */
    time_t  pc_time;         /* Expiry value */
    struct packet_cache *next , *prev;
    
};

static struct packet_cache *udp_cache = NULL;

#endif

	
static u_long	udp_sendspace = 9216;		/* really max datagram size */
					/* 40 1K datagrams */
SYSCTL_INT(_net_inet_udp, UDPCTL_MAXDGRAM, maxdgram, CTLFLAG_RW,
	&udp_sendspace, 0, "Maximum UDP datagram size");

static u_long	udp_recvspace = 40 * (1024 + sizeof(struct sockaddr_in));
SYSCTL_INT(_net_inet_udp, UDPCTL_RECVSPACE, recvspace, CTLFLAG_RW,
	&udp_recvspace, 0, "Maximum UDP receive buffer size");
	

static struct	inpcbhead udb;		/* from udp_var.h */
static struct	inpcbinfo udbinfo;

#ifndef UDBHASHSIZE
#define UDBHASHSIZE 64
#endif

/*
 * UDP statistics
 */
struct udpstat udpstat;

SYSCTL_STRUCT(_net_inet_udp, UDPCTL_STATS, stats, CTLFLAG_RW,
	&udpstat, udpstat, "UDP statistic");

static struct	sockaddr_in udp_in = { sizeof(udp_in), AF_INET };

static	void udp_detach __P((struct inpcb *));
static	int udp_output __P((struct inpcb *, struct mbuf *, struct mbuf *,
			    struct mbuf *));
static	void udp_notify __P((struct inpcb *, int));


/* Added by Vasilkov. This system call is used by snmp agent code.
 *
 */
void udp_get_struct_udb (struct inpcbhead * strudb)
{
	memcpy ((char*)strudb, (char*)&udb, sizeof(struct inpcbhead));
}


/*
 * Register sysctl's
 */
void 
sysctl_register_udp_usrreq() {

	sysctl_register(_net_inet_udp,checksum);
	sysctl_register(_net_inet_udp,maxdgram);
	sysctl_register(_net_inet_udp,stats);
	sysctl_register(_net_inet_udp,recvspace);
}

void
udp_init()
{
	LIST_INIT(&udb);
	udbinfo.listhead = &udb;
	udbinfo.hashbase = hashinit(UDBHASHSIZE, M_PCB, &udbinfo.hashmask);
	
}

#ifdef FORWARD_PROTOCOL
static unsigned char udp_ports[65536/8];

/*
 * Enable/Disable udp port forwarding
 */
int udp_forward_port(int port,int forward) {

    int byte = port/8;
    int offset = port%8;
    
    if (forward)
	udp_ports[byte] |= (0x80 >> offset);
    else
	udp_ports[byte] &= ~(0x80 >> offset);
	
    return 0;
}

/*
 * Check if port should be forwarded
 */
static int udp_if_forward(int port) {

    int byte = port/8;
    int offset = port%8;
    
    return (udp_ports[byte] & (0x80 >> offset));

}

/*
 * Get packet_cache from mbuf
 */
static int udp_pc_from_m(struct packet_cache *pc, struct mbuf *m) {

    struct ip *iph = mtod(m,struct ip *);
    struct udphdr *uh = (struct udphdr *)((char *)iph + sizeof(struct ip));
    
    pc->pc_ttl  = iph->ip_ttl;
    pc->pc_sum  = uh->uh_sum;
    pc->pc_src  = iph->ip_src;
    pc->pc_uh   = *uh;
    pc->pc_time = time(NULL) + UDPMAXEXPIRE;
    
    return 0;
}

/*
 * Make cache entry MRU
 */
static void udp_make_mru(struct packet_cache *pc) {

    if (pc == udp_cache)
	return ;
    
    /* MRU it */
    if (pc->prev) pc->prev->next = pc->next;
    if (pc->next) pc->next->prev = pc->prev;
    
    pc->prev = NULL;
    pc->next = udp_cache;
    udp_cache->prev = pc;
    udp_cache = pc;
    
    pc->pc_time = time(NULL) + UDPMAXEXPIRE;
    
    /*
     * HUGE FIXME: pc_sum should be strong checksum of udp data. md5 seems 
     * to be ok.
     */
    
}


/*
 * 
 */
#define UDP_PASS 0
#define UDP_DROP 1

static int udp_analyze(struct mbuf *m) {

    time_t now;
    struct packet_cache *pc,my,*empty = NULL;
    
    
    /*
     * If still no cache allocated - allocate it
     */
    if (!udp_cache) {
	int i;
	
	for (i=0;i<MAXUDPCACHE;i++) {
	    struct packet_cache *pc = malloc(sizeof(struct packet_cache));
	    if (pc) {
		memset(pc,0,sizeof(struct packet_cache));
		pc->next = udp_cache;
		if (udp_cache) udp_cache->prev = pc;
		udp_cache = pc;
	    }
	}
    }
    
    /*
     * If no memory - just drop packet
     */
    if (!udp_cache)
	return UDP_DROP;
    
    pc = udp_cache;
    now = time(NULL);
    
    udp_pc_from_m(&my,m);
    
    if (my.pc_ttl <= IPTTLDEC) 
	return UDP_DROP;
    
    while( pc ) {
    
	if (pc->pc_ttl)	{ /*Non-empty entry*/
	    if (pc->pc_time < now) {
		pc->pc_ttl = 0;
		empty = pc;
#ifdef FORWARD_DEBUG		
/*		printf("Entry expired :%s, sum %lu\n",inet_ntoa(pc->pc_src),pc->pc_sum);*/
#endif		
	    }
	    else {
		if ((pc->pc_sum == my.pc_sum) &&
		    (pc->pc_src.s_addr == my.pc_src.s_addr) &&
		    (pc->pc_uh.uh_dport == my.pc_uh.uh_dport) &&
		    (pc->pc_uh.uh_ulen == my.pc_uh.uh_ulen)) {
		    
#ifdef FORWARD_DEBUG		    
/*			printf("Cache HIT\n");*/
#endif			
		    
			udp_make_mru(pc);
			if (pc->pc_ttl <= my.pc_ttl) 
			    return UDP_PASS;
			
#ifdef FORWARD_DEBUG			
/*			printf("Loop detected!\n");*/
#endif			
			return UDP_DROP;
		    }
		    
	    }
	}
	else
	    empty = pc;
    
	pc = pc->next;
    }
    
    /*
     * If no free entry in cache - remove LRU entry
     */
    if (!empty) {
    
#ifdef FORWARD_DEBUG    
/*	printf("Cache full, removing LRU\n");*/
#endif	
	empty = udp_cache;
	while(empty->next) 
	    empty = empty->next;
	
    }
    
    /* Cache it and make MRU */
#ifdef FORWARD_DEBUG    
/*    printf("Caching packet\n");*/
#endif    
    udp_make_mru(empty);
    
    empty->pc_ttl  = my.pc_ttl;
    empty->pc_sum  = my.pc_sum;
    empty->pc_src  = my.pc_src;
    empty->pc_uh   = my.pc_uh;
    empty->pc_time = my.pc_time;

    return UDP_PASS;
}



#endif /* FORWARD_PROTOCOL */

void
udp_input(m, iphlen)
	register struct mbuf *m;
	int iphlen;
{
	register struct ip *ip;
	register struct udphdr *uh;
	register struct inpcb *inp;
	struct mbuf *opts = 0;
	int len;
	struct ip save_ip;
	struct ifnet *ifp = m->m_pkthdr.rcvif;
	int log_in_vain = 0;
	int blackhole = 0;


	/*
	 * Fetch logging flag from interface
	 */	
	if (ifp->if_ip.ifi_udp & IFNET_UDP_LOG_IN_VAIN)
	    log_in_vain = 1;
	    
	/*
	 * Check if we should silently discard refused connects
	 */
	if (ifp->if_ip.ifi_udp & IFNET_UDP_BLACKHOLE)
	    blackhole = 1;

	udpstat.udps_ipackets++;

	/*
	 * Strip IP options, if any; should skip this,
	 * make available to user, and use on returned packets,
	 * but we don't yet have a way to check the checksum
	 * with options still present.
	 */
	if (iphlen > sizeof (struct ip)) {
		ip_stripoptions(m, (struct mbuf *)0);
		iphlen = sizeof(struct ip);
	}

	/*
	 * Get IP and UDP header together in first mbuf.
	 */
	ip = mtod(m, struct ip *);
	if (m->m_len < iphlen + sizeof(struct udphdr)) {
		if ((m = m_pullup(m, iphlen + sizeof(struct udphdr))) == 0) {
			udpstat.udps_hdrops++;
			return;
		}
		ip = mtod(m, struct ip *);
	}
        uh = (struct udphdr *)((caddr_t)ip + iphlen);

#ifdef FORWARD_PROTOCOL    
        if (udp_if_forward(ntohs(uh->uh_dport))) {
            if (in_broadcast(ip->ip_dst, m->m_pkthdr.rcvif))
                udp_store(m);
        }
#endif	


	/*
	 * Make mbuf data length reflect UDP length.
	 * If not enough data to reflect UDP length, drop.
	 */
	len = ntohs((u_short)uh->uh_ulen);
	if (ip->ip_len != len) {
		if (len > ip->ip_len || len < sizeof(struct udphdr)) {
			udpstat.udps_badlen++;
			goto bad;
		}
		m_adj(m, len - ip->ip_len);
		/* ip->ip_len = len; */
	}
	/*
	 * Save a copy of the IP header in case we want restore it
	 * for sending an ICMP error message in response.
	 */
	save_ip = *ip;

	/*
	 * Checksum extended UDP header and data.
	 */

	if (uh->uh_sum) {
		
		((struct ipovly *)ip)->ih_next = 0;
		((struct ipovly *)ip)->ih_prev = 0;
		((struct ipovly *)ip)->ih_x1 = 0;
		((struct ipovly *)ip)->ih_len = uh->uh_ulen;
		uh->uh_sum = in_cksum(m, len + sizeof (struct ip));
		if (uh->uh_sum) {
			udpstat.udps_badsum++;
			m_freem(m);
			return;
		}
	}

	if (IN_MULTICAST(ntohl(ip->ip_dst.s_addr)) ||
	    in_broadcast(ip->ip_dst, m->m_pkthdr.rcvif)) {
		struct inpcb *last;
		
#ifdef FORWARD_PROTOCOL		
		/*
		 * If our router configured to route broadcasts (this may
		 * be required to enable NetBIOS thru router) 
                 */

                if (udp_if_forward(ntohs(uh->uh_dport))) {
                    /*
                     * For each interface that allow directed broadcast
                     * we should reflect this packet with destanation address
                     * equal to interface subnet broadcast address
                     */
                    struct ifnet *ifp = ifnet;
		    
		    /*
		     * Checksum udp data + header without address information
		     */
		    m->m_len  -= sizeof(struct ip);
		    m->m_data += sizeof(struct ip);
		    uh->uh_sum = in_cksum(m, len/* + sizeof (struct ip)*/);
		    m->m_len  += sizeof(struct ip);
		    m->m_data -= sizeof(struct ip);
		    
		    *ip = save_ip;
		    
		    if (udp_analyze(m) == UDP_DROP) {
#ifdef FORWARD_DEBUG
			printf("UDP DROP <%s:%d>, ttl=%d, sum=%lu\n",inet_ntoa(ip->ip_src),uh->uh_sport,ip->ip_ttl,uh->uh_sum);
#endif		    			
			goto bad;

		    }
		    
#ifdef FORWARD_DEBUG		    
		    printf("UDP PASS <%s:%d>, ttl=%d, sum=%lu\n",inet_ntoa(ip->ip_src),uh->uh_sport,ip->ip_ttl,uh->uh_sum);
#endif		    

		    
                    while (ifp) {
		    
                        if ((ifp != m->m_pkthdr.rcvif) &&
			    !(ifp->if_flags & IFF_LOOPBACK) /*&& 
			    (ifp->if_ip.ifi_udp & IFNET_UDP_FORWARD_PROTOCOL)*/) {
			    
                            struct ifaddr *ifa = ifp->if_addrlist;
			    
#ifdef FORWARD_DEBUG			    
			    /*printf("\tForwarding through %s%d\n",ifp->if_name,ifp->if_unit);*/
#endif			    
			    
                            while(ifa) {

				
				if  (ifa->ifa_addr->sa_family == AF_INET) {
		    

				
				    if (ifp->if_flags | IFF_BROADCAST) {
				    	    if (ifa->ifa_dstaddr)
					    ip->ip_dst.s_addr = ((struct sockaddr_in *)(ifa->ifa_dstaddr))->sin_addr.s_addr;
					else {
					    ip->ip_dst.s_addr = ((struct sockaddr_in *)(ifa->ifa_addr))->sin_addr.s_addr;
					    ip->ip_dst.s_addr &= ((struct sockaddr_in *)(ifa->ifa_netmask))->sin_addr.s_addr;
					    ip->ip_dst.s_addr |= ~(((struct sockaddr_in *)(ifa->ifa_netmask))->sin_addr.s_addr);
					}
				    }
				    else
					goto bad;
					
				    /*Calculate correct UDP checksum*/
				    
				    
#ifdef FORWARD_DEBUG
				    printf("\t\tForwarding to %s\n",inet_ntoa(ip->ip_dst));
#endif
                                    udp_doutput(ip->ip_dst);

				}

                            	ifa = ifa->ifa_next;
                    	    }
                    	}
                        ifp = ifp->if_next;
            	    }
		    
		    if (opts)
			m_freem(opts);
		    if (m)
			m_freem(m);
			
		    /*
		     * FIXME: should I also pass udp packet to socket?
		     */

                    return ;
                }
#endif /* FORWARD_PROTOCOL */		
		
		/*
		 * Deliver a multicast or broadcast datagram to *all* sockets
		 * for which the local and remote addresses and ports match
		 * those of the incoming datagram.  This allows more than
		 * one process to receive multi/broadcasts on the same port.
		 * (This really ought to be done for unicast datagrams as
		 * well, but that would cause problems with existing
		 * applications that open both address-specific sockets and
		 * a wildcard socket listening to the same port -- they would
		 * end up receiving duplicates of every unicast datagram.
		 * Those applications open the multiple sockets to overcome an
		 * inadequacy of the UDP socket interface, but for backwards
		 * compatibility we avoid the problem here rather than
		 * fixing the interface.  Maybe 4.5BSD will remedy this?)
		 */

		/*
		 * Construct sockaddr format source address.
		 */
		udp_in.sin_port = uh->uh_sport;
		udp_in.sin_addr = ip->ip_src;
		m->m_len -= sizeof (struct udpiphdr);
		m->m_data += sizeof (struct udpiphdr);
		/*
		 * Locate pcb(s) for datagram.
		 * (Algorithm copied from raw_intr().)
		 */
		last = NULL;
		for (inp = udb.lh_first; inp != NULL; inp = inp->inp_list.le_next) {
			if (inp->inp_lport != uh->uh_dport)
				continue;
			if (inp->inp_laddr.s_addr != INADDR_ANY) {
				if (inp->inp_laddr.s_addr !=
				    ip->ip_dst.s_addr)
					continue;
			}
			if (inp->inp_faddr.s_addr != INADDR_ANY) {
				if (inp->inp_faddr.s_addr !=
				    ip->ip_src.s_addr ||
				    inp->inp_fport != uh->uh_sport)
					continue;
			}

			if (last != NULL) {
				struct mbuf *n;

				if ((n = m_copy(m, 0, M_COPYALL)) != NULL) {
					if (last->inp_flags & INP_CONTROLOPTS
					    || last->inp_socket->so_options & SO_TIMESTAMP)
						ip_savecontrol(last, &opts, ip, n);
					if (sbappendaddr(&last->inp_socket->so_rcv,
						(struct sockaddr *)&udp_in,
						n, opts) == 0) {
						m_freem(n);
						if (opts)
						    m_freem(opts);
						udpstat.udps_fullsock++;
					} else
						sorwakeup(last->inp_socket);
					opts = 0;
				}
			}
			last = inp;
			/*
			 * Don't look for additional matches if this one does
			 * not have either the SO_REUSEPORT or SO_REUSEADDR
			 * socket options set.  This heuristic avoids searching
			 * through all pcbs in the common case of a non-shared
			 * port.  It * assumes that an application will never
			 * clear these options after setting them.
			 */
			if (((last->inp_socket->so_options&(SO_REUSEPORT|SO_REUSEADDR)) == 0))
				break;
		}

		if (last == NULL) {
			/*
			 * No matching pcb found; discard datagram.
			 * (No need to send an ICMP Port Unreachable
			 * for a broadcast or multicast datgram.)
			 */
			udpstat.udps_noport++;
			udpstat.udps_noportbcast++;
			goto bad;
		}
		if (last->inp_flags & INP_CONTROLOPTS
		    || last->inp_socket->so_options & SO_TIMESTAMP)
			ip_savecontrol(last, &opts, ip, m);
		if (sbappendaddr(&last->inp_socket->so_rcv,
		     (struct sockaddr *)&udp_in,
		     m, opts) == 0) {
			udpstat.udps_fullsock++;
			goto bad;
		}
		sorwakeup(last->inp_socket);
		return;
	}
	/*
	 * Locate pcb for datagram.
	 */
	inp = in_pcblookuphash(&udbinfo, ip->ip_src, uh->uh_sport,
	    ip->ip_dst, uh->uh_dport, 1);
	if (inp == NULL) {
		if (log_in_vain) {
			char bufdst[20];
			char bufsrc[20];
			
			inet_ntop(AF_INET,&(ip->ip_dst),bufdst,sizeof(bufdst));
			inet_ntop(AF_INET,&(ip->ip_src),bufdst,sizeof(bufsrc));

			log(LOG_INFO, "Connection attempt to UDP %s:%d"
			    " from %s:%d\n",
				bufdst, ntohs(uh->uh_dport),
				bufsrc, ntohs(uh->uh_sport));
		}
		udpstat.udps_noport++;
		if (m->m_flags & (M_BCAST | M_MCAST)) {
			udpstat.udps_noportbcast++;
			goto bad;
		}
		*ip = save_ip;
		
		/*
		 * If we forced to be silent as much as possible..
		 */
		if (blackhole)
		    goto bad;
		    
		icmp_error(m, ICMP_UNREACH, ICMP_UNREACH_PORT, 0, 0);
		return;
	}

	/*
	 * Construct sockaddr format source address.
	 * Stuff source address and datagram in user buffer.
	 */
	udp_in.sin_port = uh->uh_sport;
	udp_in.sin_addr = ip->ip_src;
	if (inp->inp_flags & INP_CONTROLOPTS
	    || inp->inp_socket->so_options & SO_TIMESTAMP)
		ip_savecontrol(inp, &opts, ip, m);
	iphlen += sizeof(struct udphdr);
	m->m_len -= iphlen;
	m->m_pkthdr.len -= iphlen;
	m->m_data += iphlen;
	if (sbappendaddr(&inp->inp_socket->so_rcv, (struct sockaddr *)&udp_in,
	    m, opts) == 0) {
		udpstat.udps_fullsock++;
		goto bad;
	}
	sorwakeup(inp->inp_socket);
	return;
bad:
	m_freem(m);
	if (opts)
	    m_freem(opts);
}

/*
 * Notify a udp user of an asynchronous error;
 * just wake up so that he can collect error status.
 */
static void
udp_notify(inp, errnum)
	register struct inpcb *inp;
	int errnum;
{
	inp->inp_socket->so_error = errnum;
	sorwakeup(inp->inp_socket);
	sowwakeup(inp->inp_socket);
}

void
udp_ctlinput(cmd, sa, vip)
	int cmd;
	struct sockaddr *sa;
	void *vip;
{
	register struct ip *ip = vip;
	register struct udphdr *uh;

	if (!PRC_IS_REDIRECT(cmd) &&
	    ((unsigned)cmd >= PRC_NCMDS || inetctlerrmap[cmd] == 0))
		return;
	if (ip) {
		uh = (struct udphdr *)((caddr_t)ip + (ip->ip_hl << 2));
		in_pcbnotify(&udb, sa, uh->uh_dport, ip->ip_src, uh->uh_sport,
			cmd, udp_notify);
	} else
		in_pcbnotify(&udb, sa, 0, zeroin_addr, 0, cmd, udp_notify);
}

static int
udp_output(inp, m, addr, control)
	register struct inpcb *inp;
	register struct mbuf *m;
	struct mbuf *addr, *control;
{
	register struct udpiphdr *ui;
	register int len = m->m_pkthdr.len;
	struct in_addr laddr;
	int s = 0, error = 0;

	if (control)
		m_freem(control);		/* XXX */

	if (len + sizeof(struct udpiphdr) > IP_MAXPACKET) {
		error = EMSGSIZE;
		goto release;
	}

	if (addr) {
		laddr = inp->inp_laddr;
		if (inp->inp_faddr.s_addr != INADDR_ANY) {
			error = EISCONN;
			goto release;
		}
		/*
		 * Must block input while temporarily connected.
		 */
		s = splnet();
		error = in_pcbconnect(inp, addr);
		if (error) {
			splx(s);
			goto release;
		}
	} else {
		if (inp->inp_faddr.s_addr == INADDR_ANY) {
			error = ENOTCONN;
			goto release;
		}
	}
	/*
	 * Calculate data length and get a mbuf
	 * for UDP and IP headers.
	 */
	M_PREPEND(m, sizeof(struct udpiphdr), M_DONTWAIT);
	if (m == 0) {
		error = ENOBUFS;
		if (addr)
			splx(s);
		goto release;
	}

	/*
	 * Fill in mbuf with extended UDP header
	 * and addresses and length put into network format.
	 */
	ui = mtod(m, struct udpiphdr *);
	ui->ui_next = ui->ui_prev = 0;
	ui->ui_x1 = 0;
	ui->ui_pr = IPPROTO_UDP;
	ui->ui_len = htons((u_short)len + sizeof (struct udphdr));
	ui->ui_src = inp->inp_laddr;
	ui->ui_dst = inp->inp_faddr;
	ui->ui_sport = inp->inp_lport;
	ui->ui_dport = inp->inp_fport;
	ui->ui_ulen = ui->ui_len;

	/*
	 * Stuff checksum and output datagram.
	 */
	ui->ui_sum = 0;
	if (udpcksum) { /*FIXME: should be taken from ouput interface */
	    if ((ui->ui_sum = in_cksum(m, sizeof (struct udpiphdr) + len)) == 0)
		ui->ui_sum = 0xffff;
	}
	((struct ip *)ui)->ip_len = sizeof (struct udpiphdr) + len;
	((struct ip *)ui)->ip_ttl = inp->inp_ip_ttl;	/* XXX */
	((struct ip *)ui)->ip_tos = inp->inp_ip_tos;	/* XXX */
	udpstat.udps_opackets++;
	error = ip_output(m, inp->inp_options, &inp->inp_route,
	    inp->inp_socket->so_options & (SO_DONTROUTE | SO_BROADCAST),
	    inp->inp_moptions,0);

	if (addr) {
		in_pcbdisconnect(inp);
		inp->inp_laddr = laddr;
		splx(s);
	}
	return (error);

release:
	m_freem(m);
	return (error);
}


/*ARGSUSED*/
int
udp_usrreq(so, req, m, addr, control)
	struct socket *so;
	int req;
	struct mbuf *m, *addr, *control;
{
	struct inpcb *inp = sotoinpcb(so);
	int error = 0;
	int s;

	if (req == PRU_CONTROL)
		return (in_control(so, (u_long)m, (caddr_t)addr,
			(struct ifnet *)control));
	if (inp == NULL && req != PRU_ATTACH) {
		error = EINVAL;
		goto release;
	}
	/*
	 * Note: need to block udp_input while changing
	 * the udp pcb queue and/or pcb addresses.
	 */
	switch (req) {

	case PRU_ATTACH:
		if (inp != NULL) {
			error = EINVAL;
			break;
		}
		s = splnet();
		error = in_pcballoc(so, &udbinfo);
		splx(s);
		if (error)
			break;
		error = soreserve(so, udp_sendspace, udp_recvspace);
		if (error)
			break;
		((struct inpcb *) so->so_pcb)->inp_ip_ttl = ip_defttl; /*FIXME: fetch ttl from interface first*/
		break;

	case PRU_DETACH:
		udp_detach(inp);
		break;

	case PRU_BIND:
		s = splnet();
		error = in_pcbbind(inp, addr);
		splx(s);
		break;

	case PRU_LISTEN:
		error = EOPNOTSUPP;
		break;

	case PRU_CONNECT:
		if (inp->inp_faddr.s_addr != INADDR_ANY) {
			error = EISCONN;
			break;
		}
		s = splnet();
		error = in_pcbconnect(inp, addr);
		splx(s);
		if (error == 0)
			soisconnected(so);
		break;

	case PRU_CONNECT2:
		error = EOPNOTSUPP;
		break;

	case PRU_ACCEPT:
		error = EOPNOTSUPP;
		break;

	case PRU_DISCONNECT:
		if (inp->inp_faddr.s_addr == INADDR_ANY) {
			error = ENOTCONN;
			break;
		}
		s = splnet();
		in_pcbdisconnect(inp);
		inp->inp_laddr.s_addr = INADDR_ANY;
		splx(s);
		so->so_state &= ~SS_ISCONNECTED;		/* XXX */
		break;

	case PRU_SHUTDOWN:
		socantsendmore(so);
		break;

	case PRU_SEND:
		return (udp_output(inp, m, addr, control));

	case PRU_ABORT:
		soisdisconnected(so);
		udp_detach(inp);
		break;

	case PRU_SOCKADDR:
		in_setsockaddr(inp, addr);
		break;

	case PRU_PEERADDR:
		in_setpeeraddr(inp, addr);
		break;

	case PRU_SENSE:
		/*
		 * stat: don't bother with a blocksize.
		 */
		return (0);

	case PRU_SENDOOB:
	case PRU_FASTTIMO:
	case PRU_SLOWTIMO:
	case PRU_PROTORCV:
	case PRU_PROTOSEND:
		error =  EOPNOTSUPP;
		break;

	case PRU_RCVD:
	case PRU_RCVOOB:
		return (EOPNOTSUPP);	/* do not free mbuf's */

	default:
		panic("udp_usrreq");
	}

release:
	if (control) {
		printf("udp control data unexpectedly retained\n");
		m_freem(control);
	}
	if (m)
		m_freem(m);
	return (error);
}

static void
udp_detach(inp)
	struct inpcb *inp;
{
	int s = splnet();

	in_pcbdetach(inp);
	splx(s);
}

#ifdef FORWARD_PROTOCOL

static int ttl;
static struct in_addr src;
static int sport,dport;
static char buf[8096];
static int len;

/*
 * Store packet
 */
static int
udp_store(struct mbuf *m) {

    struct ip *iph = mtod(m,struct ip *);
    struct udphdr *uh = iph + 1;

#ifdef FORWARD_DEBUG
    printf("Storing %d bytes at offset %d of total packet len %d\n",uh->uh_ulen - sizeof(struct udphdr),sizeof(struct ip) + sizeof(struct udphdr),m->m_pkthdr.len);
#endif    
    
    if (m_copydata(m,sizeof(struct ip) + sizeof(struct udphdr),
                 uh->uh_ulen - sizeof(struct udphdr),
		 buf)<0) {
	ttl = 0;
	return -1;
    }
		 

    ttl = iph->ip_ttl;
    src = iph->ip_src;
    sport = uh->uh_sport;
    dport = uh->uh_dport;
    len = uh->uh_ulen - sizeof(struct udphdr);

    return 0;
}

/*
 * Pull packet to network 
 */
static int
udp_doutput(struct in_addr dst) {


    struct udpiphdr *ui;
    struct mbuf *m;
    int error;
    struct route ro;
    
    if (ttl <= 1)
	return -1;
    
    m = m_gethdr(M_DONTWAIT,MT_DATA);
        
    if (!m) {
#ifdef FORWARD_DEBUG    
	printf("udp_doutput() : No buffers available\n");
#endif	
	return -1;
    }

    m->m_pkthdr.len = 0;
    m->m_pkthdr.rcvif = NULL;    
    m->m_len = MHLEN;
    
    if (m_copyback(m,0,len,buf)<0) {
	m_freem(m);
	return -1;
    }
    
    M_PREPEND(m, sizeof(struct udpiphdr), M_DONTWAIT);
    
    if (!m) {
#ifdef FORWARD_DEBUG    
	printf("udp_douptut() : No buffers available\n");
#endif	
	return -1;
    }

    /*
     * Fill in mbuf with extended UDP header
     * and addresses and length put into network format.
     */
    ui = mtod(m, struct udpiphdr *);
    ui->ui_next = ui->ui_prev = 0;
    ui->ui_x1 = 0;
    ui->ui_pr = IPPROTO_UDP;
    ui->ui_len = htons((u_short)len + sizeof (struct udphdr));
    ui->ui_src = src;
    ui->ui_dst = dst;
    ui->ui_sport = sport;
    ui->ui_dport = dport;
    ui->ui_ulen = ui->ui_len;

    /*
     * Stuff checksum and output datagram.
     */
    ui->ui_sum = 0;
    if ((ui->ui_sum = in_cksum(m, sizeof (struct udpiphdr) + len)) == 0)
	ui->ui_sum = 0xffff;
	
	
    ((struct ip *)ui)->ip_len = sizeof (struct udpiphdr) + len;
    ((struct ip *)ui)->ip_ttl = ttl - 1;
    ((struct ip *)ui)->ip_tos = 0;
    
    bzero(&ro, sizeof ro);
    
    udpstat.udps_opackets++;

#ifdef FORWARD_DEBUG   
    {
	struct mbuf *n = m;
	printf("Sending buffer chain: ");
	while (n) {
	    printf("[%d] ",n->m_len);
	    n = n->m_next;
	}
	printf("\n");
	
    }
#endif    
    
    error = ip_output(m, NULL, &ro,IP_ALLOWBROADCAST,NULL,0);

    if (ro.ro_rt)
    	RTFREE(ro.ro_rt);
	
    return error;
}

#endif /* FORWARD_PROTOCOL */

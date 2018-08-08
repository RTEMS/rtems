/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	$KAME: in6.h,v 1.89 2001/05/27 13:28:35 itojun Exp $
 */

#if !defined(_NETINET6_IN6_H_) || !defined(_KERNEL)
#error "must be included via <netinet6/in6.h> in kernel space"
#endif

/* XXX nonstandard */
#define s6_addr8  __u6_addr.__u6_addr8
#define s6_addr16 __u6_addr.__u6_addr16
#define s6_addr32 __u6_addr.__u6_addr32

/*
 * Local definition for masks
 */
/* XXX nonstandard */
#define IN6MASK0	{{{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }}}
#define IN6MASK32	{{{ 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, \
			    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }}}
#define IN6MASK64	{{{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
			    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }}}
#define IN6MASK96	{{{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
			    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 }}}
#define IN6MASK128	{{{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
			    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }}}

extern const struct sockaddr_in6 sa6_any;

extern const struct in6_addr in6mask0;
extern const struct in6_addr in6mask32;
extern const struct in6_addr in6mask64;
extern const struct in6_addr in6mask96;
extern const struct in6_addr in6mask128;

/*
 * Macros started with IPV6_ADDR is KAME local
 */
/* XXX nonstandard */
#if _BYTE_ORDER == _BIG_ENDIAN
#define IPV6_ADDR_INT32_ONE	1
#define IPV6_ADDR_INT32_TWO	2
#define IPV6_ADDR_INT32_MNL	0xff010000
#define IPV6_ADDR_INT32_MLL	0xff020000
#define IPV6_ADDR_INT32_SMP	0x0000ffff
#define IPV6_ADDR_INT16_ULL	0xfe80
#define IPV6_ADDR_INT16_USL	0xfec0
#define IPV6_ADDR_INT16_MLL	0xff02
#elif _BYTE_ORDER == _LITTLE_ENDIAN
#define IPV6_ADDR_INT32_ONE	0x01000000
#define IPV6_ADDR_INT32_TWO	0x02000000
#define IPV6_ADDR_INT32_MNL	0x000001ff
#define IPV6_ADDR_INT32_MLL	0x000002ff
#define IPV6_ADDR_INT32_SMP	0xffff0000
#define IPV6_ADDR_INT16_ULL	0x80fe
#define IPV6_ADDR_INT16_USL	0xc0fe
#define IPV6_ADDR_INT16_MLL	0x02ff
#endif

/* XXX nonstandard */
#define IPV6_ADDR_SCOPE_NODELOCAL	0x01
#define IPV6_ADDR_SCOPE_INTFACELOCAL	0x01
#define IPV6_ADDR_SCOPE_LINKLOCAL	0x02
#define IPV6_ADDR_SCOPE_SITELOCAL	0x05
#define IPV6_ADDR_SCOPE_ORGLOCAL	0x08	/* just used in this file */
#define IPV6_ADDR_SCOPE_GLOBAL		0x0e

#define IPV6_ADDR_MC_SCOPE(a)		((a)->s6_addr[1] & 0x0f)

/* refers nonstandard items */
#define IN6_IS_ADDR_MC_NODELOCAL(a)	\
	(IN6_IS_ADDR_MULTICAST(a) &&	\
	 (IPV6_ADDR_MC_SCOPE(a) == IPV6_ADDR_SCOPE_NODELOCAL))
#define IN6_IS_ADDR_MC_INTFACELOCAL(a)	\
	(IN6_IS_ADDR_MULTICAST(a) &&	\
	 (IPV6_ADDR_MC_SCOPE(a) == IPV6_ADDR_SCOPE_INTFACELOCAL))
#define IN6_IS_ADDR_MC_LINKLOCAL(a)	\
	(IN6_IS_ADDR_MULTICAST(a) &&	\
	 (IPV6_ADDR_MC_SCOPE(a) == IPV6_ADDR_SCOPE_LINKLOCAL))
#define IN6_IS_ADDR_MC_SITELOCAL(a)	\
	(IN6_IS_ADDR_MULTICAST(a) &&	\
	 (IPV6_ADDR_MC_SCOPE(a) == IPV6_ADDR_SCOPE_SITELOCAL))
#define IN6_IS_ADDR_MC_ORGLOCAL(a)	\
	(IN6_IS_ADDR_MULTICAST(a) &&	\
	 (IPV6_ADDR_MC_SCOPE(a) == IPV6_ADDR_SCOPE_ORGLOCAL))
#define IN6_IS_ADDR_MC_GLOBAL(a)	\
	(IN6_IS_ADDR_MULTICAST(a) &&	\
	 (IPV6_ADDR_MC_SCOPE(a) == IPV6_ADDR_SCOPE_GLOBAL))

/* nonstandard */
/*
 * KAME Scope
 */
#define IN6_IS_SCOPE_LINKLOCAL(a)	\
	((IN6_IS_ADDR_LINKLOCAL(a)) ||	\
	 (IN6_IS_ADDR_MC_LINKLOCAL(a)))
#define	IN6_IS_SCOPE_EMBED(a)			\
	((IN6_IS_ADDR_LINKLOCAL(a)) ||		\
	 (IN6_IS_ADDR_MC_LINKLOCAL(a)) ||	\
	 (IN6_IS_ADDR_MC_INTFACELOCAL(a)))

#define IFA6_IS_DEPRECATED(a) \
	((a)->ia6_lifetime.ia6t_pltime != ND6_INFINITE_LIFETIME && \
	 (u_int32_t)((time_uptime - (a)->ia6_updatetime)) > \
	 (a)->ia6_lifetime.ia6t_pltime)
#define IFA6_IS_INVALID(a) \
	((a)->ia6_lifetime.ia6t_vltime != ND6_INFINITE_LIFETIME && \
	 (u_int32_t)((time_uptime - (a)->ia6_updatetime)) > \
	 (a)->ia6_lifetime.ia6t_vltime)

#define MTAG_ABI_IPV6		1444287380	/* IPv6 ABI */
#define IPV6_TAG_DIRECT		0		/* direct-dispatch IPv6 */

/* RFC2292 options */
#define IPV6_2292PKTINFO	19 /* bool; send/recv if, src/dst addr */
#define IPV6_2292HOPLIMIT	20 /* bool; hop limit */
#define IPV6_2292NEXTHOP	21 /* bool; next hop addr */
#define IPV6_2292HOPOPTS	22 /* bool; hop-by-hop option */
#define IPV6_2292DSTOPTS	23 /* bool; destinaion option */
#define IPV6_2292RTHDR		24 /* bool; routing header */
#define IPV6_2292PKTOPTIONS	25 /* buf/cmsghdr; set/get IPv6 options */

#define IPV6_RECVRTHDRDSTOPTS	41 /* bool; recv dst option before rthdr */

struct cmsghdr;
struct ip6_hdr;

int	in6_cksum_pseudo(struct ip6_hdr *, uint32_t, uint8_t, uint16_t);
int	in6_cksum(struct mbuf *, u_int8_t, u_int32_t, u_int32_t);
int	in6_cksum_partial(struct mbuf *, u_int8_t, u_int32_t, u_int32_t,
			  u_int32_t);
int	in6_localaddr(struct in6_addr *);
int	in6_localip(struct in6_addr *);
int	in6_ifhasaddr(struct ifnet *, struct in6_addr *);
int	in6_addrscope(const struct in6_addr *);
char	*ip6_sprintf(char *, const struct in6_addr *);
struct	in6_ifaddr *in6_ifawithifp(struct ifnet *, struct in6_addr *);
extern void in6_if_up(struct ifnet *);
struct sockaddr;
extern	u_char	ip6_protox[];

void	in6_sin6_2_sin(struct sockaddr_in *sin,
			    struct sockaddr_in6 *sin6);
void	in6_sin_2_v4mapsin6(struct sockaddr_in *sin,
				 struct sockaddr_in6 *sin6);
void	in6_sin6_2_sin_in_sock(struct sockaddr *nam);
void	in6_sin_2_v4mapsin6_in_sock(struct sockaddr **nam);
extern void addrsel_policy_init(void);

#define	satosin6(sa)	((struct sockaddr_in6 *)(sa))
#define	sin6tosa(sin6)	((struct sockaddr *)(sin6))
#define	ifatoia6(ifa)	((struct in6_ifaddr *)(ifa))

/*-
 * Copyright (c) 1982, 1986, 1990, 1993
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
 */

#ifndef _RTEMS_RTEMS_NETINET_IN_H
#define _RTEMS_RTEMS_NETINET_IN_H

#include <rtems.h>
#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Ports > IPPORT_USERRESERVED are reserved
 * for servers, not necessarily privileged.  (IP_PORTRANGE_DEFAULT)
 */
#define	IPPORT_USERRESERVED	5000

/* Deprecated in current FreeBSD */
#define IPCTL_RTEXPIRE		5	/* cloned route expiration time */
#define IPCTL_RTMINEXPIRE	6	/* min value for expiration time */
#define IPCTL_RTMAXCACHE	7	/* trigger level for dynamic expire */

int	 in_cksum(struct mbuf *, int);

/* Firewall hooks */
struct ip;
typedef	int ip_fw_chk_t(struct ip**, int, struct ifnet*, int, struct mbuf**);
typedef	int ip_fw_ctl_t(int, struct mbuf**);
extern	ip_fw_chk_t *ip_fw_chk_ptr;
extern	ip_fw_ctl_t *ip_fw_ctl_ptr;

/* IP NAT hooks */
typedef	int ip_nat_t(struct ip**, struct mbuf**, struct ifnet*, int);
typedef	int ip_nat_ctl_t(int, struct mbuf**);
extern	ip_nat_t *ip_nat_ptr;
extern	ip_nat_ctl_t *ip_nat_ctl_ptr;
#define	IP_NAT_IN	0x00000001
#define	IP_NAT_OUT	0x00000002

/*
 * Options for use with [gs]etsockopt at the IP level.
 *
 * The value is stored in an integer. Use negative numbers to avoid conflicts
 * with BSD.
 */
#define IP_NAT			(-55)   /* set/get NAT opts */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_RTEMS_NETINET_IN_H */

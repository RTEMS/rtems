/*
 * RTEMS/TCPIP driver for MPC8260 SCC HDLC networking
 *
 * Submitted by Andy Dachs <a.dachs@sstl.co.uk>
 * (c) Surrey Satellite Technology Limited, 2001
 *
 *   On the ADS board the ethernet interface is connected to FCC2
 *   but in my application I want TCP over HDLC (see README)
 *   so will use SCC3 as the network interface. I have other plans
 *   for the FCCs so am unlikely to add true ethernet support to
 *   this BSP.  Contributions welcome!
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#ifndef __IF_HDLCSUBR_H
#define __IF_HDLCSUBR_H

struct ifnet;
struct mbuf;
struct sockaddr;
struct rtentry;

void	hdlc_ifattach (struct ifnet *);
void	hdlc_input (struct ifnet *, struct mbuf *);
int	hdlc_output (struct ifnet *,
	   struct mbuf *, struct sockaddr *, struct rtentry *);
int	hdlc_ioctl (struct ifnet *, int , caddr_t );

#endif

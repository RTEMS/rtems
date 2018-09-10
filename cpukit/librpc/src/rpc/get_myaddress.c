#include "rtems-rpc-config.h"

/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user.
 *
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun RPC is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */

#if defined(LIBC_SCCS) && !defined(lint)
/*static char *sccsid = "from: @(#)get_myaddress.c 1.4 87/08/11 Copyr 1984 Sun Micro";*/
/*static char *sccsid = "from: @(#)get_myaddress.c	2.1 88/07/29 4.0 RPCSRC";*/
static char *rcsid = "$FreeBSD: src/lib/libc/rpc/get_myaddress.c,v 1.17 2000/01/27 23:06:37 jasone Exp $";
#endif

/*
 * get_myaddress.c
 *
 * Get client's IP address via ioctl.  This avoids using the yellowpages.
 * Copyright (C) 1984, Sun Microsystems, Inc.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rpc/types.h>
#include <rpc/xdr.h>
#include <rpc/pmap_prot.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>


/*
 * Determine the size of an ifreq structure when addresses larger 
 * than the pifreq structure size may be returned from the kernel.
 */
static size_t ifreqSize ( struct ifreq *pifreq )
{
  size_t size = pifreq->ifr_addr.sa_len + sizeof(pifreq->ifr_name);
  if ( size < sizeof ( *pifreq ) ) {
    size = sizeof ( *pifreq );
  }
  return size;
}

/*
 * don't use gethostbyname, which would invoke yellow pages
 *
 * Avoid loopback interfaces.  We return information from a loopback
 * interface only if there are no other possible interfaces.
 */
int
get_myaddress(
	struct sockaddr_in *addr)
{
	int s;
	struct ifconf ifc;
	struct ifreq ifreq, *ifr;
	int loopback = 0, gotit = 0;

	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		return(-1);
	}
again:
	ifc.ifc_len = sizeof ( struct ifreq ) * 8u;
	ifc.ifc_buf = malloc ( ifc.ifc_len );
  if ( ! ifc.ifc_buf ) {
		_RPC_close(s);
    return -1;
  }
	if (ioctl(s, SIOCGIFCONF, (char *)&ifc) < 0) {
		_RPC_close(s);
    free ( ifc.ifc_buf );
		return(-1);
	}
	ifr = ifc.ifc_req;

	while ( ifc.ifc_len >= ifreqSize ( ifr ) ) {
    ifreq = *ifr;
		if (ioctl(s, SIOCGIFFLAGS, (char *) &ifreq ) < 0) {
			_RPC_close(s);
      free ( ifc.ifc_buf );
			return(-1);
		}
		if (((ifreq.ifr_flags & IFF_UP) &&
		    ifr->ifr_addr.sa_family == AF_INET &&
			!(ifreq.ifr_flags & IFF_LOOPBACK)) ||
		    (loopback == 1 && (ifreq.ifr_flags & IFF_LOOPBACK)
			&& (ifr->ifr_addr.sa_family == AF_INET)
			&& (ifreq.ifr_flags &  IFF_UP))) {
			*addr = *((struct sockaddr_in *)&ifr->ifr_addr);
			addr->sin_port = htons(PMAPPORT);
			gotit = 1;
			break;
		}

    const size_t len = ifreqSize ( ifr );
    ifc.ifc_len -= len;
    /* 
     * RTEMS seems to require copy up to properly aligned 
     * boundary at the beginning of the buffer?
     */
    memmove ( ifr, len + (char *) ifr, ifc.ifc_len );
	}
	if (gotit == 0 && loopback == 0) {
    free ( ifc.ifc_buf );
		loopback = 1;
		goto again;
	}
	(void)_RPC_close(s);
  free ( ifc.ifc_buf );
	return (gotit ? 0 : -1);
}

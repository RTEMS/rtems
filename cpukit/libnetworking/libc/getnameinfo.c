#include <machine/rtems-bsd-user-space.h>

/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

int
getnameinfo(const struct sockaddr *sa, socklen_t salen, char *node,
    size_t nodelen, char *service, size_t servicelen, int flags)
{
	int af;
	const struct sockaddr_in *sa_in = (const struct sockaddr_in *)sa;

	(void) salen;

	af = sa->sa_family;
	if (af != AF_INET) {
		return EAI_FAMILY;
	}

	if ((flags & NI_NAMEREQD) != 0) {
		return EAI_NONAME;
	}

	/* FIXME: This return just the address value. Try resolving instead. */
	if (node != NULL && nodelen > 0) {
		if (inet_ntop(af, &sa_in->sin_addr, node, nodelen) == NULL) {
			return EAI_FAIL;
		}
	}

	if (service != NULL && servicelen > 0) {
		in_port_t port = ntohs(sa_in->sin_port);
		int rv;

		rv = snprintf(service, servicelen, "%u", port);
		if (rv <= 0) {
			return EAI_FAIL;
		} else if ((unsigned)rv >= servicelen) {
			return EAI_OVERFLOW;
		}
	}

	return 0;
}

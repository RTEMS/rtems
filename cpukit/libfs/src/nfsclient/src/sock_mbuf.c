/*
 *  NOTE:
 *    This is derived from libnetworking/rtems/rtems_syscall.c
 *
 *    RTEMS/libnetworking LICENSING restrictions may apply
 *
 *    Author (modifications only):
 *    Copyright: 2002, Stanford University and
 *    		 Till Straumann, <strauman@slac.stanford.edu>
 *    Licensing: 'LICENSE.NET' file in the RTEMS top source directory
 *               for more information.
 */

/*
The RTEMS TCP/IP stack is a port of the FreeBSD TCP/IP stack.  The following
copyright and licensing information applies to this code.

This code is found under the c/src/libnetworking directory but does not
constitute the entire contents of that subdirectory.

=============================================================================

Copyright (c) 1980, 1983, 1988, 1993
	The Regents of the University of California.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgment:
	This product includes software developed by the University of
	California, Berkeley and its contributors.
4. Neither the name of the University nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

-
Portions Copyright (c) 1993 by Digital Equipment Corporation.

Permission to use, copy, modify, and distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies, and that
the name of Digital Equipment Corporation not be used in advertising or
publicity pertaining to distribution of the document or software without
specific, written prior permission.

THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL DIGITAL EQUIPMENT
CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

=============================================================================
*/

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/error.h>

#define _KERNEL
#define __BSD_VISIBLE	1
#include <rtems/rtems_bsdnet.h>

#include <sys/errno.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/protosw.h>
#include <sys/proc.h>
#include <sys/fcntl.h>
#include <sys/filio.h>

#include <net/if.h>
#include <net/route.h>

struct socket *rtems_bsdnet_fdToSocket(int fd);

/*
 * Package system call argument into mbuf.
 *
 * (unfortunately, the original is not public)
 */
static int
sockaddrtombuf (struct mbuf **mp, const struct sockaddr *buf, int buflen)
{
struct mbuf *m;
struct sockaddr *sa;

	if ((u_int)buflen > MLEN)
		return (EINVAL);

	rtems_bsdnet_semaphore_obtain();
	m = m_get(M_WAIT, MT_SONAME);
	rtems_bsdnet_semaphore_release();

	if (m == NULL)
		return (ENOBUFS);
	m->m_len = buflen;
	memcpy (mtod(m, caddr_t), buf, buflen);
	*mp = m;
	sa = mtod(m, struct sockaddr *);
	sa->sa_len = buflen;

	return 0;
}

static void
dummyproc(caddr_t ext_buf, u_int ext_size)
{
}

/*
 * send data by simply allocating an MBUF packet
 * header and pointing it to our data region.
 *
 * Optionally, the caller may supply 'reference'
 * and 'free' procs. (The latter may call the
 * user back once the networking stack has
 * released the buffer).
 *
 * The callbacks are provided with the 'closure'
 * pointer and the 'buflen' argument.
 */
ssize_t
sendto_nocpy (
		int s,
		const void *buf, size_t buflen,
		int flags,
		const struct sockaddr *toaddr, int tolen,
		void *closure,
		void (*freeproc)(caddr_t, u_int),
		void (*refproc)(caddr_t, u_int)
)
{
	int           error;
	struct socket *so;
	struct mbuf   *to, *m;
	int           ret = -1;

	rtems_bsdnet_semaphore_obtain ();
	if ((so = rtems_bsdnet_fdToSocket (s)) == NULL) {
		rtems_bsdnet_semaphore_release ();
		return -1;
	}

	error = sockaddrtombuf (&to, toaddr, tolen);
	if (error) {
		errno = error;
		rtems_bsdnet_semaphore_release ();
		return -1;
	}

	MGETHDR(m, M_WAIT, MT_DATA);
	m->m_pkthdr.len   = 0;
	m->m_pkthdr.rcvif =  (struct ifnet *) 0;

	m->m_flags       |= M_EXT;
	m->m_ext.ext_buf  = closure ? closure : (void*)buf;
	m->m_ext.ext_size = buflen;
	/* we _must_ supply non-null procs; otherwise,
	 * the kernel code assumes it's a mbuf cluster
	 */
	m->m_ext.ext_free = freeproc ? freeproc : dummyproc;
	m->m_ext.ext_ref  = refproc  ? refproc  : dummyproc;
	m->m_pkthdr.len  += buflen;
	m->m_len          = buflen;
	m->m_data		  = (void*)buf;

	error = sosend (so, to, NULL, m, NULL, flags);
	if (error) {
		if (/*auio.uio_resid != len &&*/ (error == EINTR || error == EWOULDBLOCK))
			error = 0;
	}
	if (error)
		errno = error;
	else
		ret = buflen;
	if (to)
		m_freem(to);
	rtems_bsdnet_semaphore_release ();
	return (ret);
}


/*
 * receive data in an 'mbuf chain'.
 * The chain must be released once the
 * data has been extracted:
 *
 *   rtems_bsdnet_semaphore_obtain();
 *   	m_freem(chain);
 *   rtems_bsdnet_semaphore_release();
 */
ssize_t
recv_mbuf_from(int s, struct mbuf **ppm, long len, struct sockaddr *fromaddr, int *fromlen)
{
	int ret = -1;
	int error;
	struct uio auio;
	struct socket *so;
	struct mbuf *from = NULL;

	memset(&auio, 0, sizeof(auio));
	*ppm = 0;

	rtems_bsdnet_semaphore_obtain ();
	if ((so = rtems_bsdnet_fdToSocket (s)) == NULL) {
		rtems_bsdnet_semaphore_release ();
		return -1;
	}
/*	auio.uio_iov = mp->msg_iov;
	auio.uio_iovcnt = mp->msg_iovlen;
	auio.uio_segflg = UIO_USERSPACE;
	auio.uio_rw = UIO_READ;
	auio.uio_offset = 0;
*/
	auio.uio_resid = len;
	error = soreceive (so, &from, &auio, (struct mbuf **) ppm,
			(struct mbuf **)NULL,
			NULL);
	if (error) {
		if (auio.uio_resid != len && (error == EINTR || error == EWOULDBLOCK))
			error = 0;
	}
	if (error) {
		errno = error;
	}
	else {
		ret = len - auio.uio_resid;
		if (fromaddr) {
			len = *fromlen;
			if ((len <= 0) || (from == NULL)) {
				len = 0;
			}
			else {
				if (len > from->m_len)
					len = from->m_len;
				memcpy (fromaddr, mtod(from, caddr_t), len);
			}
			*fromlen = len;
		}
	}
	if (from)
		m_freem (from);
	if (error && *ppm) {
		m_freem(*ppm);
		*ppm = 0;
	}
	rtems_bsdnet_semaphore_release ();
	return (ret);
}

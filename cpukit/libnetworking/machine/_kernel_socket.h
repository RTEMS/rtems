/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 1982, 1985, 1986, 1988, 1993, 1994
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
 * 3. Neither the name of the University nor the names of its contributors
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
 *	@(#)socket.h	8.4 (Berkeley) 2/21/94
 * $FreeBSD: head/sys/sys/socket.h 334719 2018-06-06 15:45:57Z sbruno $
 */

#if !defined(_SYS_SOCKET_H_) || !defined(_KERNEL)
#error "must be included via <sys/socket.h> in kernel space"
#endif

/*
 * Flags for accept1(), kern_accept4() and solisten_dequeue, in addition
 * to SOCK_CLOEXEC and SOCK_NONBLOCK.
 */
#define ACCEPT4_INHERIT 0x1
#define ACCEPT4_COMPAT  0x2

#define	MSG_SOCALLBCK    0x00010000	/* for use by socket callbacks - soreceive (TCP) */

#define	MSG_MORETOCOME	 0x00100000	/* additional data pending */

#define	CMSG_ALIGN(n)	_ALIGN(n)

#define	SF_READAHEAD(flags)	((flags) >> 16)

struct socket;

struct tcpcb *so_sototcpcb(struct socket *so);
struct inpcb *so_sotoinpcb(struct socket *so);
struct sockbuf *so_sockbuf_snd(struct socket *);
struct sockbuf *so_sockbuf_rcv(struct socket *);

int so_state_get(const struct socket *);
void so_state_set(struct socket *, int);

int so_options_get(const struct socket *);
void so_options_set(struct socket *, int);

int so_error_get(const struct socket *);
void so_error_set(struct socket *, int);

int so_linger_get(const struct socket *);
void so_linger_set(struct socket *, int);

struct protosw *so_protosw_get(const struct socket *);
void so_protosw_set(struct socket *, struct protosw *);

void so_sorwakeup_locked(struct socket *so);
void so_sowwakeup_locked(struct socket *so);

void so_sorwakeup(struct socket *so);
void so_sowwakeup(struct socket *so);

void so_lock(struct socket *so);
void so_unlock(struct socket *so);

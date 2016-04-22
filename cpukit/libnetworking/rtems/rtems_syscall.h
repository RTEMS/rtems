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

#ifndef _LIBNETWORKING_RTEMS_SYSCALL_H_
#define _LIBNETWORKING_RTEMS_SYSCALL_H_

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>

__BEGIN_DECLS

int	select(int, fd_set *, fd_set *, fd_set *, struct timeval *);

int	accept(int, struct sockaddr * __restrict, socklen_t * __restrict);

int	bind(int, const struct sockaddr *, socklen_t);

int	connect(int, const struct sockaddr *, socklen_t);

int	getpeername(int, struct sockaddr * __restrict, socklen_t * __restrict);

int	getsockname(int, struct sockaddr * __restrict, socklen_t * __restrict);

int	getsockopt(int, int, int, void * __restrict, socklen_t * __restrict);

int	listen(int, int);

ssize_t	recv(int, void *, size_t, int);

ssize_t	recvfrom(int, void *, size_t, int, struct sockaddr * __restrict, socklen_t * __restrict);

ssize_t	recvmsg(int, struct msghdr *, int);

ssize_t	send(int, const void *, size_t, int);

ssize_t	sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t);

ssize_t	sendmsg(int, const struct msghdr *, int);

int	setsockopt(int, int, int, const void *, socklen_t);

int	shutdown(int, int);

int	socket(int, int, int);

int	socketpair(int, int, int, int *);

int	sysctl(const int *, u_int, void *, size_t *, const void *, size_t);

int	sysctlbyname(const char *, void *, size_t *, const void *, size_t);

int	sysctlnametomib(const char *, int *, size_t *);

__END_DECLS

#endif /* _LIBNETWORKING_RTEMS_SYSCALL_H_ */

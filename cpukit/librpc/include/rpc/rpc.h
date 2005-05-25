/*	$NetBSD: rpc.h,v 1.13 2000/06/02 22:57:56 fvdl Exp $	*/

/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user.
 *
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
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
 *
 *	from: @(#)rpc.h 1.9 88/02/08 SMI
 *	from: @(#)rpc.h	2.4 89/07/11 4.0 RPCSRC
 * $FreeBSD: src/include/rpc/rpc.h,v 1.17 2002/03/23 17:24:55 imp Exp $
 */

/*
 * rpc.h, Just includes the billions of rpc header files necessary to
 * do remote procedure calling.
 *
 * Copyright (C) 1984, Sun Microsystems, Inc.
 */
#ifndef _RPC_RPC_H
#define _RPC_RPC_H

#include <rpc/types.h>		/* some typedefs */
#include <netinet/in.h>

/* external data representation interfaces */
#include <rpc/xdr.h>		/* generic (de)serializer */

/* Client side only authentication */
#include <rpc/auth.h>		/* generic authenticator (client side) */

/* Client side (mostly) remote procedure call */
#include <rpc/clnt.h>		/* generic rpc stuff */

/* semi-private protocol headers */
#include <rpc/rpc_msg.h>	/* protocol for rpc messages */
#include <rpc/auth_unix.h>	/* protocol for unix style cred */

/* Server side only remote procedure callee */
#include <rpc/svc.h>		/* service manager and multiplexer */
#include <rpc/svc_auth.h>	/* service side authenticator */

#include <rpc/rpcent.h>

__BEGIN_DECLS
extern int get_myaddress(struct sockaddr_in *);
extern int bindresvport(int, struct sockaddr_in *);
extern int bindresvport_sa(int, struct sockaddr *);
__END_DECLS

int rtems_rpc_task_init (void);
int rtems_rpc_start_portmapper (int priority);

#ifdef _RTEMS_RPC_INTERNAL_
/*
 * Multi-threaded support
 * Group all global and static variables into a single spot.
 * This area will be allocated on a per-task basis
 */
struct _rtems_rpc_task_variables {
	int		svc_svc_maxfd;
	fd_set		svc_svc_fdset;
	SVCXPRT **	svc_xports;
	int		svc_xportssize;
	int		svc__svc_fdsetsize;
	fd_set		*svc__svc_fdset;
	struct svc_callout	*svc_svc_head;

	char		*clnt_perror_buf;

	struct clnt_raw_private *clnt_raw_private;

	void		*call_rpc_private;

	struct call_rpc_private *svc_raw_private;

	struct prog_lst *svc_simple_proglst;
	struct prog_lst *svc_simple_pl;
	SVCXPRT		*svc_simple_transp;

	char		*rpcdname_default_domain;

	struct authsvc *svc_auths_Auths;
};
extern struct _rtems_rpc_task_variables *rtems_rpc_task_variables;

#define svc_maxfd (rtems_rpc_task_variables->svc_svc_maxfd)
#define svc_fdset (rtems_rpc_task_variables->svc_svc_fdset)
#define __svc_fdsetsize (rtems_rpc_task_variables->svc__svc_fdsetsize)
#define __svc_fdset (rtems_rpc_task_variables->svc__svc_fdset)

#endif /* _RTEMS_RPC_INTERNAL_ */

#endif /* !_RPC_RPC_H */

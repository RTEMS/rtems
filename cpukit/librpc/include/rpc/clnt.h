/*	$NetBSD: clnt.h,v 1.14 2000/06/02 22:57:55 fvdl Exp $	*/

/*
 * The contents of this file are subject to the Sun Standards
 * License Version 1.0 the (the "License";) You may not use
 * this file except in compliance with the License.  You may
 * obtain a copy of the License at lib/libc/rpc/LICENSE
 *
 * Software distributed under the License is distributed on
 * an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See the License for the specific
 * language governing rights and limitations under the License.
 *
 * The Original Code is Copyright 1998 by Sun Microsystems, Inc
 *
 * The Initial Developer of the Original Code is:  Sun
 * Microsystems, Inc.
 *
 * All Rights Reserved.
 *
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
 *	from: @(#)clnt.h 1.31 94/04/29 SMI
 *	from: @(#)clnt.h	2.1 88/07/29 4.0 RPCSRC
 * $FreeBSD: src/include/rpc/clnt.h,v 1.21 2003/01/24 01:47:55 fjoe Exp $
 */

/*
 * clnt.h - Client side remote procedure call interface.
 *
 * Copyright (c) 1986-1991,1994-1999 by Sun Microsystems, Inc.
 * All rights reserved.
 */


#ifndef _RPC_CLNT_H_
#define _RPC_CLNT_H_
#include <rpc/clnt_stat.h>
#include <sys/cdefs.h>
#include <sys/un.h>
#include <rpc/auth.h> /* auth_stat */

/*
 * Error info.
 */
struct rpc_err {
	enum clnt_stat re_status;
	union {
		int RE_errno;		/* related system error */
		enum auth_stat RE_why;	/* why the auth error occurred */
		struct {
			rpcvers_t low;	/* lowest version supported */
			rpcvers_t high;	/* highest version supported */
		} RE_vers;
		struct {		/* maybe meaningful if RPC_FAILED */
			int32_t s1;
			int32_t s2;
		} RE_lb;		/* life boot & debugging only */
	} ru;
#define	re_errno	ru.RE_errno
#define	re_why		ru.RE_why
#define	re_vers		ru.RE_vers
#define	re_lb		ru.RE_lb
};


/*
 * Client rpc handle.
 * Created by individual implementations
 * Client is responsible for initializing auth, see e.g. auth_none.c.
 */
typedef struct __rpc_client {
	AUTH	*cl_auth;			/* authenticator */
	struct clnt_ops {
		/* call remote procedure */
		enum clnt_stat	(*cl_call)(struct __rpc_client *,
				    rpcproc_t, xdrproc_t, void *, xdrproc_t,
				        void *, struct timeval);
		/* abort a call */
		void		(*cl_abort)(void);
		/* get specific error code */
		void		(*cl_geterr)(struct __rpc_client *,
					struct rpc_err *);
		/* frees results */
		bool_t		(*cl_freeres)(struct __rpc_client *,
					xdrproc_t, void *);
		/* destroy this structure */
		void		(*cl_destroy)(struct __rpc_client *);
		/* the ioctl() of rpc */
		bool_t          (*cl_control)(struct __rpc_client *, int,
					char *);
	} *cl_ops;
	void			*cl_private;	/* private stuff */
} CLIENT;

#define RPCSMALLMSGSIZE	400	/* a more reasonable packet size */

/*
 * client side rpc interface ops
 *
 * Parameter types are:
 *
 */

/*
 * enum clnt_stat
 * CLNT_CALL(rh, proc, xargs, argsp, xres, resp, timeout)
 * 	CLIENT *rh;
 *	rpcproc_t proc;
 *	xdrproc_t xargs;
 *	void *argsp;
 *	xdrproc_t xres;
 *	void *resp;
 *	struct timeval timeout;
 */
#define	CLNT_CALL(rh, proc, xargs, argsp, xres, resp, secs)	\
	((*(rh)->cl_ops->cl_call)(rh, proc, xargs, \
		argsp, xres, resp, secs))
#define	clnt_call(rh, proc, xargs, argsp, xres, resp, secs)	\
	((*(rh)->cl_ops->cl_call)(rh, proc, xargs, \
		argsp, xres, resp, secs))

/*
 * void
 * CLNT_ABORT(rh);
 * 	CLIENT *rh;
 */
#define	CLNT_ABORT(rh)	((*(rh)->cl_ops->cl_abort)(rh))
#define	clnt_abort(rh)	((*(rh)->cl_ops->cl_abort)(rh))

/*
 * struct rpc_err
 * CLNT_GETERR(rh);
 * 	CLIENT *rh;
 */
#define	CLNT_GETERR(rh,errp)	((*(rh)->cl_ops->cl_geterr)(rh, errp))
#define	clnt_geterr(rh,errp)	((*(rh)->cl_ops->cl_geterr)(rh, errp))


/*
 * bool_t
 * CLNT_FREERES(rh, xres, resp);
 * 	CLIENT *rh;
 *	xdrproc_t xres;
 *	void *resp;
 */
#define	CLNT_FREERES(rh,xres,resp) ((*(rh)->cl_ops->cl_freeres)(rh,xres,resp))
#define	clnt_freeres(rh,xres,resp) ((*(rh)->cl_ops->cl_freeres)(rh,xres,resp))

/*
 * bool_t
 * CLNT_CONTROL(cl, request, info)
 *      CLIENT *cl;
 *      u_int request;
 *      char *info;
 */
#define	CLNT_CONTROL(cl,rq,in) ((*(cl)->cl_ops->cl_control)(cl,rq,in))
#define	clnt_control(cl,rq,in) ((*(cl)->cl_ops->cl_control)(cl,rq,in))

/*
 * control operations that apply to udp, tcp and unix transports
 *
 * Note: options marked XXX are no-ops in this implementation of RPC.
 * The are present in TI-RPC but can't be implemented here since they
 * depend on the presence of STREAMS/TLI, which we don't have.
 *
 */
#define CLSET_TIMEOUT       1   /* set timeout (timeval) */
#define CLGET_TIMEOUT       2   /* get timeout (timeval) */
#define CLGET_SERVER_ADDR   3   /* get server's address (sockaddr) */
#define CLGET_FD            6	/* get connections file descriptor */
#define CLGET_SVC_ADDR      7   /* get server's address (netbuf) */
#define CLSET_FD_CLOSE      8   /* close fd while clnt_destroy */
#define CLSET_FD_NCLOSE     9   /* Do not close fd while clnt_destroy */
#define CLGET_XID           10	/* Get xid */
#define CLSET_XID           11	/* Set xid */
#define CLGET_VERS          12	/* Get version number */
#define CLSET_VERS          13	/* Set version number */
#define CLGET_PROG	    14	/* Get program number */
#define CLSET_PROG          15	/* Set program number */
#define CLSET_SVC_ADDR      16	/* get server's address (netbuf)         XXX */
#define CLSET_PUSH_TIMOD    17	/* push timod if not already present     XXX */
#define CLSET_POP_TIMOD     18	/* pop timod                             XXX */

/*
 * Connectionless only control operations
 */
#define CLSET_RETRY_TIMEOUT 4   /* set retry timeout (timeval) */
#define CLGET_RETRY_TIMEOUT 5   /* get retry timeout (timeval) */

/*
 * Operations which GSSAPI needs. (Bletch.)
 */
#define CLGET_LOCAL_ADDR    19	/* get local addr (sockaddr) */


/*
 * void
 * CLNT_DESTROY(rh);
 * 	CLIENT *rh;
 */
#define	CLNT_DESTROY(rh)	((*(rh)->cl_ops->cl_destroy)(rh))
#define	clnt_destroy(rh)	((*(rh)->cl_ops->cl_destroy)(rh))


/*
 * RPCTEST is a test program which is accessible on every rpc
 * transport/port.  It is used for testing, performance evaluation,
 * and network administration.
 */

#define RPCTEST_PROGRAM		((rpcprog_t)1)
#define RPCTEST_VERSION		((rpcvers_t)1)
#define RPCTEST_NULL_PROC	((rpcproc_t)2)
#define RPCTEST_NULL_BATCH_PROC	((rpcproc_t)3)

/*
 * By convention, procedure 0 takes null arguments and returns them
 */

#define NULLPROC ((rpcproc_t)0)

/*
 * Below are the client handle creation routines for the various
 * implementations of client side rpc.  They can return NULL if a
 * creation failure occurs.
 */

/*
 * Generic client creation routine. Supported protocols are "udp", "tcp"
 * and "unix".
 */
__BEGIN_DECLS
extern CLIENT *clnt_create(const char *, const rpcprog_t, const rpcvers_t,
	const char *);
__END_DECLS

/*
 * Added for compatibility to old rpc 4.0. Obsoleted by clnt_vc_create().
 */
__BEGIN_DECLS
extern CLIENT *clntunix_create(struct sockaddr_un *,
			       u_long, u_long, int *, u_int, u_int);
__END_DECLS


/*
 * Print why creation failed
 */
__BEGIN_DECLS
extern void clnt_pcreateerror(const char *);			/* stderr */
extern char *clnt_spcreateerror(const char *);		/* string */
__END_DECLS

/*
 * Like clnt_perror(), but is more verbose in its output
 */
__BEGIN_DECLS
extern void clnt_perrno(enum clnt_stat);		/* stderr */
extern char *clnt_sperrno(enum clnt_stat);		/* string */
__END_DECLS

/*
 * Print an English error message, given the client error code
 */
__BEGIN_DECLS
extern void clnt_perror(CLIENT *, const char *); 		/* stderr */
extern char *clnt_sperror(CLIENT *, const char *);		/* string */
__END_DECLS


/*
 * If a creation fails, the following allows the user to figure out why.
 */
struct rpc_createerr {
	enum clnt_stat cf_stat;
	struct rpc_err cf_error; /* useful when cf_stat == RPC_PMAPFAILURE */
};

extern struct rpc_createerr rpc_createerr;

/* For backward compatibility */
#include <rpc/clnt_soc.h>

#endif /* !_RPC_CLNT_H_ */

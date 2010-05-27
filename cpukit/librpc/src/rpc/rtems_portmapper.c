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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rpc/rpc.h>
#include <rpc/pmap_prot.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/signal.h>

static void reg_service(struct svc_req *rqstp, SVCXPRT *xprt);
static void callit(struct svc_req *rqstp, SVCXPRT *xprt);
static struct pmaplist *pmaplist;
static int debugging = 0;

#include <rtems.h>
#define fork()	(-1)


static rtems_task rtems_portmapper (rtems_task_argument unused)
{
	SVCXPRT *xprt;
	int sock;
	struct sockaddr_in addr;
	int len = sizeof(struct sockaddr_in);
	register struct pmaplist *pml;

	rtems_rpc_task_init ();
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		perror("portmap cannot create socket");
		rtems_task_delete (RTEMS_SELF);
	}

	addr.sin_addr.s_addr = 0;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PMAPPORT);
	if (bind(sock, (struct sockaddr *)&addr, len) != 0) {
		perror("portmap cannot bind");
		close (sock);
		rtems_task_delete (RTEMS_SELF);
	}

	if ((xprt = svcudp_create(sock)) == (SVCXPRT *)NULL) {
		fprintf(stderr, "couldn't do udp_create\n");
		close (sock);
		rtems_task_delete (RTEMS_SELF);
	}
	/* make an entry for ourself */
	pml = (struct pmaplist *)malloc(sizeof(struct pmaplist));
	pml->pml_next = 0;
	pml->pml_map.pm_prog = PMAPPROG;
	pml->pml_map.pm_vers = PMAPVERS;
	pml->pml_map.pm_prot = IPPROTO_UDP;
	pml->pml_map.pm_port = PMAPPORT;
	pmaplist = pml;

	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("portmap cannot create socket");
		close (sock);
		rtems_task_delete (RTEMS_SELF);
	}
	if (bind(sock, (struct sockaddr *)&addr, len) != 0) {
		perror("portmap cannot bind");
		close (sock);
		rtems_task_delete (RTEMS_SELF);
	}
	if ((xprt = svctcp_create(sock, RPCSMALLMSGSIZE, RPCSMALLMSGSIZE))
	    == (SVCXPRT *)NULL) {
		fprintf(stderr, "couldn't do tcp_create\n");
		close (sock);
		rtems_task_delete (RTEMS_SELF);
	}
	/* make an entry for ourself */
	pml = (struct pmaplist *)malloc(sizeof(struct pmaplist));
	pml->pml_map.pm_prog = PMAPPROG;
	pml->pml_map.pm_vers = PMAPVERS;
	pml->pml_map.pm_prot = IPPROTO_TCP;
	pml->pml_map.pm_port = PMAPPORT;
	pml->pml_next = pmaplist;
	pmaplist = pml;

	(void)svc_register(xprt, PMAPPROG, PMAPVERS, reg_service, FALSE);

	svc_run();
	fprintf(stderr, "run_svc returned unexpectedly\n");
	close (sock);
	rtems_task_delete (RTEMS_SELF);
}

static struct pmaplist *
find_service(
  u_long prog,
  u_long vers,
  int prot )
{
  register struct pmaplist *hit = NULL;
  register struct pmaplist *pml;

  for (pml = pmaplist; pml != NULL; pml = pml->pml_next) {
	if ((pml->pml_map.pm_prog != prog) ||
		(pml->pml_map.pm_prot != prot))
		continue;
	hit = pml;
	if (pml->pml_map.pm_vers == vers)
	    break;
  }
  return (hit);
}

/* 
 * 1 OK, 0 not
 */
static void reg_service(
	struct svc_req *rqstp,
	SVCXPRT *xprt )
{
	struct pmap reg;
	struct pmaplist *pml, *prevpml, *fnd;
	int ans, port;
	caddr_t t;
	
#ifdef DEBUG
	fprintf(stderr, "server: about do a switch\n");
#endif
	switch (rqstp->rq_proc) {

	case PMAPPROC_NULL:
		/*
		 * Null proc call
		 */
		if ((!svc_sendreply(xprt, (xdrproc_t) xdr_void, NULL)) &&
		     debugging) {
			abort();
		}
		break;

	case PMAPPROC_SET:
		/*
		 * Set a program,version to port mapping
		 */
		if (!svc_getargs(xprt, (xdrproc_t) xdr_pmap, (caddr_t)&reg))
			svcerr_decode(xprt);
		else {
			/*
			 * check to see if already used
			 * find_service returns a hit even if
			 * the versions don't match, so check for it
			 */
			fnd = find_service(reg.pm_prog, reg.pm_vers, reg.pm_prot);
			if (fnd && fnd->pml_map.pm_vers == reg.pm_vers) {
				if (fnd->pml_map.pm_port == reg.pm_port) {
					ans = 1;
					goto done;
				}
				else {
					ans = 0;
					goto done;
				}
			} else {
				/* 
				 * add to END of list
				 */
				pml = (struct pmaplist *)
				    malloc(sizeof(struct pmaplist));
				pml->pml_map = reg;
				pml->pml_next = 0;
				if (pmaplist == 0) {
					pmaplist = pml;
				} else {
					for (fnd= pmaplist; fnd->pml_next != 0;
					    fnd = fnd->pml_next);
					fnd->pml_next = pml;
				}
				ans = 1;
			}
		done:
			if ((!svc_sendreply(xprt, (xdrproc_t) xdr_long, (caddr_t)&ans)) &&
			    debugging) {
				fprintf(stderr, "svc_sendreply\n");
				abort();
			}
		}
		break;

	case PMAPPROC_UNSET:
		/*
		 * Remove a program,version to port mapping.
		 */
		if (!svc_getargs(xprt, (xdrproc_t) xdr_pmap, (caddr_t)&reg))
			svcerr_decode(xprt);
		else {
			ans = 0;
			for (prevpml = NULL, pml = pmaplist; pml != NULL; ) {
				if ((pml->pml_map.pm_prog != reg.pm_prog) ||
					(pml->pml_map.pm_vers != reg.pm_vers)) {
					/* both pml & prevpml move forwards */
					prevpml = pml;
					pml = pml->pml_next;
					continue;
				}
				/* found it; pml moves forward, prevpml stays */
				ans = 1;
				t = (caddr_t)pml;
				pml = pml->pml_next;
				if (prevpml == NULL)
					pmaplist = pml;
				else
					prevpml->pml_next = pml;
				free(t);
			}
			if ((!svc_sendreply(xprt, (xdrproc_t) xdr_long, (caddr_t)&ans)) &&
			    debugging) {
				fprintf(stderr, "svc_sendreply\n");
				abort();
			}
		}
		break;

	case PMAPPROC_GETPORT:
		/*
		 * Lookup the mapping for a program,version and return its port
		 */
		if (!svc_getargs(xprt, (xdrproc_t) xdr_pmap, (caddr_t)&reg))
			svcerr_decode(xprt);
		else {
			fnd = find_service(reg.pm_prog, reg.pm_vers, reg.pm_prot);
			if (fnd)
				port = fnd->pml_map.pm_port;
			else
				port = 0;
			if ((!svc_sendreply(xprt, (xdrproc_t) xdr_long, (caddr_t)&port)) &&
			    debugging) {
				fprintf(stderr, "svc_sendreply\n");
				abort();
			}
		}
		break;

	case PMAPPROC_DUMP:
		/*
		 * Return the current set of mapped program,version
		 */
		if (!svc_getargs(xprt, (xdrproc_t) xdr_void, NULL))
			svcerr_decode(xprt);
		else {
			if ((!svc_sendreply(xprt, (xdrproc_t) xdr_pmaplist,
			    (caddr_t)&pmaplist)) && debugging) {
				fprintf(stderr, "svc_sendreply\n");
				abort();
			}
		}
		break;

	case PMAPPROC_CALLIT:
		/*
		 * Calls a procedure on the local machine.  If the requested
		 * procedure is not registered this procedure does not return
		 * error information!!
		 * This procedure is only supported on rpc/udp and calls via 
		 * rpc/udp.  It passes null authentication parameters.
		 */
		callit(rqstp, xprt);
		break;

	default:
		svcerr_noproc(xprt);
		break;
	}
}


/*
 * Stuff for the rmtcall service
 */
#define ARGSIZE 9000

struct encap_parms {
	u_long arglen;
	char *args;
};

static bool_t
xdr_encap_parms(
	XDR *xdrs,
	struct encap_parms *epp )
{

	u_int temp_epp_arglen = epp->arglen;
	return (xdr_bytes(xdrs, &(epp->args), &temp_epp_arglen, ARGSIZE));
}

struct rmtcallargs {
	u_long	rmt_prog;
	u_long	rmt_vers;
	u_long	rmt_port;
	u_long	rmt_proc;
	struct encap_parms rmt_args;
};

static bool_t
xdr_rmtcall_args(
	register XDR *xdrs,
	register struct rmtcallargs *cap )
{

	/* does not get a port number */
	if (xdr_u_long(xdrs, &(cap->rmt_prog)) &&
	    xdr_u_long(xdrs, &(cap->rmt_vers)) &&
	    xdr_u_long(xdrs, &(cap->rmt_proc))) {
		return (xdr_encap_parms(xdrs, &(cap->rmt_args)));
	}
	return (FALSE);
}

static bool_t
xdr_rmtcall_result(
	register XDR *xdrs,
	register struct rmtcallargs *cap )
{
	if (xdr_u_long(xdrs, &(cap->rmt_port)))
		return (xdr_encap_parms(xdrs, &(cap->rmt_args)));
	return (FALSE);
}

/*
 * only worries about the struct encap_parms part of struct rmtcallargs.
 * The arglen must already be set!!
 */
static bool_t
xdr_opaque_parms(
	XDR *xdrs,
	void *args,
	... )
{
        struct rmtcallargs *cap = (struct rmtcallargs *) args;

	return (xdr_opaque(xdrs, cap->rmt_args.args, cap->rmt_args.arglen));
}

/*
 * This routine finds and sets the length of incoming opaque paraters
 * and then calls xdr_opaque_parms.
 */
static bool_t
xdr_len_opaque_parms(
	XDR *xdrs,
	void *args, 
	... )
{
        struct rmtcallargs *cap = (struct rmtcallargs *) args;
	register u_int beginpos, lowpos, highpos, currpos, pos;

	beginpos = lowpos = pos = xdr_getpos(xdrs);
	highpos = lowpos + ARGSIZE;
	while ((int)(highpos - lowpos) >= 0) {
		currpos = (lowpos + highpos) / 2;
		if (xdr_setpos(xdrs, currpos)) {
			pos = currpos;
			lowpos = currpos + 1;
		} else {
			highpos = currpos - 1;
		}
	}
	xdr_setpos(xdrs, beginpos);
	cap->rmt_args.arglen = pos - beginpos;
	return (xdr_opaque_parms(xdrs, cap));
}

/*
 * Call a remote procedure service
 * This procedure is very quiet when things go wrong.
 * The proc is written to support broadcast rpc.  In the broadcast case,
 * a machine should shut-up instead of complain, less the requestor be
 * overrun with complaints at the expense of not hearing a valid reply ...
 *
 * This now forks so that the program & process that it calls can call 
 * back to the portmapper.
 */
static void
callit(
	struct svc_req *rqstp,
	SVCXPRT *xprt )
{
	struct rmtcallargs a;
	struct pmaplist *pml;
	u_short port;
	struct sockaddr_in me;
	int pid, socket = -1;
	CLIENT *client;
	struct authunix_parms *au = (struct authunix_parms *)rqstp->rq_clntcred;
	struct timeval timeout;
	char buf[ARGSIZE];

	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	a.rmt_args.args = buf;
	if (!svc_getargs(xprt, (xdrproc_t) xdr_rmtcall_args, (caddr_t)&a))
	    return;
	if ((pml = find_service(a.rmt_prog, a.rmt_vers, IPPROTO_UDP)) == NULL)
	    return;
	/*
	 * fork a child to do the work.  Parent immediately returns.
	 * Child exits upon completion.
	 */
	if ((pid = fork()) != 0) {
		if (debugging && (pid < 0)) {
			fprintf(stderr, "portmap CALLIT: cannot fork.\n");
		}
		return;
	}
	port = pml->pml_map.pm_port;
	get_myaddress(&me);
	me.sin_port = htons(port);
	client = clntudp_create(&me, a.rmt_prog, a.rmt_vers, timeout, &socket);
	if (client != (CLIENT *)NULL) {
		if (rqstp->rq_cred.oa_flavor == AUTH_UNIX) {
			client->cl_auth = authunix_create(au->aup_machname,
			   au->aup_uid, au->aup_gid, au->aup_len, au->aup_gids);
		}
		a.rmt_port = (u_long)port;
		if (clnt_call(client, a.rmt_proc, xdr_opaque_parms, &a,
		    xdr_len_opaque_parms, &a, timeout) == RPC_SUCCESS) {
			svc_sendreply(xprt, (xdrproc_t) xdr_rmtcall_result, (caddr_t)&a);
		}
		AUTH_DESTROY(client->cl_auth);
		clnt_destroy(client);
	}
	(void)close(socket);
	exit(0);
}

/*
 * Start the RPC portmapper
 */
int rtems_rpc_start_portmapper (int priority)
{
	rtems_mode mode;
	rtems_status_code sc;
	rtems_id tid;
	static int started;

	rtems_task_mode (RTEMS_NO_PREEMPT, RTEMS_PREEMPT_MASK, &mode);
	if (started) {
		rtems_task_mode (mode, RTEMS_PREEMPT_MASK, &mode);
		return RTEMS_SUCCESSFUL;
	}
	sc = rtems_task_create (rtems_build_name('P', 'M', 'A', 'P'),
		priority,
		ARGSIZE + 8000,
		RTEMS_PREEMPT|RTEMS_NO_TIMESLICE|RTEMS_NO_ASR|RTEMS_INTERRUPT_LEVEL(0),
		RTEMS_NO_FLOATING_POINT|RTEMS_LOCAL,
		&tid);
	if (sc != RTEMS_SUCCESSFUL) {
		rtems_task_mode (mode, RTEMS_PREEMPT_MASK, &mode);
		return sc;
	}
	sc = rtems_task_start (tid, rtems_portmapper, 0);
	if (sc != RTEMS_SUCCESSFUL) {
		rtems_task_mode (mode, RTEMS_PREEMPT_MASK, &mode);
		return sc;
	}
	started = 1;
	rtems_task_mode (mode, RTEMS_PREEMPT_MASK, &mode);
	return RTEMS_SUCCESSFUL;
}

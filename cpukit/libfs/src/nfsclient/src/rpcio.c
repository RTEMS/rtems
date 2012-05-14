/* RPC multiplexor for a multitasking environment */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002 */

/* This code funnels arbitrary task's UDP/RPC requests
 * through one socket to arbitrary servers.
 * The replies are gathered and dispatched to the
 * requestors.
 * One task handles all the sending and receiving
 * work including retries.
 * It is up to the requestor, however, to do
 * the XDR encoding of the arguments / decoding
 * of the results (except for the RPC header which
 * is handled by the daemon).
 */

/*
 * Authorship
 * ----------
 * This software (NFS-2 client implementation for RTEMS) was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2002-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * The NFS-2 client implementation for RTEMS was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <inttypes.h>

#include <rtems.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>
#include <stdlib.h>
#include <time.h>
#include <rpc/rpc.h>
#include <rpc/pmap_prot.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "rpcio.h"

/****************************************************************/
/* CONFIGURABLE PARAMETERS                                      */
/****************************************************************/

#define MBUF_RX			/* If defined: use mbuf XDR stream for
						 *  decoding directly out of mbufs
						 *  Otherwise, the regular 'recvfrom()'
						 *  interface will be used involving an
						 *  extra buffer allocation + copy step.
						 */

#define MBUF_TX			/* If defined: avoid copying data when
						 *  sending. Instead, use a wrapper to
						 *  'sosend()' which will point an MBUF
						 *  directly to our buffer space.
						 *  Note that the BSD stack does not copy
						 *  data when fragmenting packets - it
						 *  merely uses an mbuf chain pointing
						 *  into different areas of the data.
						 *
						 * If undefined, the regular 'sendto()'
						 *  interface is used.
						 */

#undef REJECT_SERVERIP_MISMATCH
						/* If defined, RPC replies must come from the server
						 * that was queried. Eric Norum has reported problems
						 * with clustered NFS servers. So we disable this
						 * reducing paranoia...
						 */

/* daemon task parameters */
#define RPCIOD_STACK		10000
#define RPCIOD_PRIO			100	/* *fallback* priority */

/* depth of the message queue for sending
 * RPC requests to the daemon
 */
#define RPCIOD_QDEPTH		20

/* Maximum retry limit for retransmission */
#define RPCIOD_RETX_CAP_S	3 /* seconds */

/* Default timeout for RPC calls */
#define RPCIOD_DEFAULT_TIMEOUT	(&_rpc_default_timeout)
static struct timeval _rpc_default_timeout = { 10 /* secs */, 0 /* usecs */ };

/* how many times should we try to resend a failed
 * transaction with refreshed AUTHs
 */
#define RPCIOD_REFRESH		2

/* Events we are using; the RPC_EVENT
 * MUST NOT be used by any application
 * thread doing RPC IO (e.g. NFS)
 */
#define RTEMS_RPC_EVENT		RTEMS_EVENT_30	/* THE event used by RPCIO. Every task doing
											 * RPC IO will receive this - hence it is
											 * RESERVED
											 */
#define RPCIOD_RX_EVENT		RTEMS_EVENT_1	/* Events the RPCIOD is using/waiting for */
#define RPCIOD_TX_EVENT		RTEMS_EVENT_2
#define RPCIOD_KILL_EVENT	RTEMS_EVENT_3	/* send to the daemon to kill it          */

#define LD_XACT_HASH		8				/* ld of the size of the transaction hash table  */


/* Debugging Flags                                              */

/* NOTE: defining DEBUG 0 leaves some 'assert()' paranoia checks
 *       but produces no output
 */

#define	DEBUG_TRACE_XACT	(1<<0)
#define DEBUG_EVENTS		(1<<1)
#define DEBUG_MALLOC		(1<<2)
#define DEBUG_TIMEOUT		(1<<3)
#define DEBUG_PACKLOSS		(1<<4)	/* This introduces random, artificial packet losses to test retransmission */

#define DEBUG_PACKLOSS_FRACT (0xffffffff/10)

/* USE PARENTHESIS WHEN 'or'ing MULTIPLE FLAGS: (DEBUG_XX | DEBUG_YY) */
#define DEBUG				(0)

/****************************************************************/
/* END OF CONFIGURABLE SECTION                                  */
/****************************************************************/

/* prevent rollover of our timers by readjusting the epoch on the fly */
#if	(DEBUG) & DEBUG_TIMEOUT
#define RPCIOD_EPOCH_SECS	10
#else
#define RPCIOD_EPOCH_SECS	10000
#endif

#ifdef	DEBUG
#define ASSERT(arg)			assert(arg)
#else
#define ASSERT(arg)			if (arg)
#endif

/****************************************************************/
/* MACROS                                                       */
/****************************************************************/


#define XACT_HASHS		(1<<(LD_XACT_HASH))	/* the hash table size derived from the ld       */
#define XACT_HASH_MSK	((XACT_HASHS)-1)	/* mask to extract the hash index from a RPC-XID */


#define MU_LOCK(mutex)		do { 							\
							assert(							\
								RTEMS_SUCCESSFUL ==			\
								rtems_semaphore_obtain(		\
										(mutex),			\
										RTEMS_WAIT,			\
										RTEMS_NO_TIMEOUT	\
										) );				\
							} while(0)

#define MU_UNLOCK(mutex)	do {							\
							assert(							\
								RTEMS_SUCCESSFUL ==			\
								rtems_semaphore_release(	\
										(mutex)				\
										) );				\
							} while(0)

#define MU_CREAT(pmutex)	do {							\
							assert(							\
								RTEMS_SUCCESSFUL ==			\
								rtems_semaphore_create(		\
										rtems_build_name(	\
											'R','P','C','l'	\
											),				\
										1,					\
										MUTEX_ATTRIBUTES,	\
										0,					\
										(pmutex)) );		\
							} while (0)


#define MU_DESTROY(mutex)	do {							\
							assert(							\
								RTEMS_SUCCESSFUL ==			\
								rtems_semaphore_delete(		\
										mutex				\
										) );				\
							} while (0)

#define MUTEX_ATTRIBUTES	(RTEMS_LOCAL           | 		\
			   				RTEMS_PRIORITY         | 		\
			   				RTEMS_INHERIT_PRIORITY | 		\
						   	RTEMS_BINARY_SEMAPHORE)

#define FIRST_ATTEMPT		0x88888888 /* some time that is never reached */

/****************************************************************/
/* TYPE DEFINITIONS                                             */
/****************************************************************/

typedef	rtems_interval		TimeoutT;

/* 100000th implementation of a doubly linked list;
 * since only one thread is looking at these,
 * we need no locking
 */
typedef struct ListNodeRec_ {
	struct ListNodeRec_ *next, *prev;
} ListNodeRec, *ListNode;


/* Structure representing an RPC server */
typedef struct RpcUdpServerRec_ {
		RpcUdpServer		next;			/* linked list of all servers; protected by hlock */
		union {
		struct sockaddr_in	sin;
		struct sockaddr     sa;
		}					addr;
		AUTH				*auth;
		rtems_id			authlock;		/* must MUTEX the auth object - it's not clear
											 *  what is better:
											 *   1 having one (MUTEXed) auth per server
											 *	   who is shared among all transactions
											 *	   using that server
											 *	 2 maintaining an AUTH per transaction
											 *	   (there are then other options: manage
											 *	   XACT pools on a per-server basis instead
											 *	   of associating a server with a XACT when
											 *   sending)
											 * experience will show if the current (1)
											 * approach has to be changed.
											 */
		TimeoutT			retry_period;	/* dynamically adjusted retry period
											 * (based on packet roundtrip time)
											 */
		/* STATISTICS */
		unsigned long		retrans;		/* how many retries were issued by this server         */
		unsigned long		requests;		/* how many requests have been sent                    */
		unsigned long       timeouts;		/* how many requests have timed out                    */
		unsigned long       errors;         /* how many errors have occurred (other than timeouts) */
		char				name[20];		/* server's address in IP 'dot' notation               */
} RpcUdpServerRec;

typedef union  RpcBufU_ {
		uint32_t			xid;
		char				buf[1];
} RpcBufU, *RpcBuf;

/* RX Buffer implementation; this is either
 * an MBUF chain (MBUF_RX configuration)
 * or a buffer allocated from the heap
 * where recvfrom copies the (encoded) reply
 * to. The XDR routines the copy/decode
 * it into the user's data structures.
 */
#ifdef MBUF_RX
typedef	struct mbuf *		RxBuf;	/* an MBUF chain */
static  void   				bufFree(struct mbuf **m);
#define XID(ibuf) 			(*(mtod((ibuf), u_long *)))
extern void 				xdrmbuf_create(XDR *, struct mbuf *, enum xdr_op);
#else
typedef RpcBuf				RxBuf;
#define	bufFree(b)			do { MY_FREE(*(b)); *(b)=0; } while(0)
#define XID(ibuf) 			((ibuf)->xid)
#endif

/* A RPC 'transaction' consisting
 * of server and requestor information,
 * buffer space and an XDR object
 * (for encoding arguments).
 */
typedef struct RpcUdpXactRec_ {
		ListNodeRec			node;		/* so we can put XACTs on a list                */
		RpcUdpServer		server;		/* server this XACT goes to                     */
		long				lifetime;	/* during the lifetime, retry attempts are made */
		long				tolive;		/* lifetime timer                               */
		struct rpc_err		status;		/* RPC reply error status                       */
		long				age;		/* age info; needed to manage retransmission    */
		long				trip;		/* record round trip time in ticks              */
		rtems_id			requestor;	/* the task waiting for this XACT to complete   */
		RpcUdpXactPool		pool;		/* if this XACT belong to a pool, this is it    */
		XDR					xdrs;		/* argument encoder stream                      */
		int					xdrpos;     /* stream position after the (permanent) header */
		xdrproc_t			xres;		/* reply decoder proc - TODO needn't be here    */
		caddr_t				pres;		/* reply decoded obj  - TODO needn't be here    */
#ifndef MBUF_RX
		int					ibufsize;	/* size of the ibuf (bytes)                     */
#endif
#ifdef  MBUF_TX
		int					refcnt;		/* mbuf external storage reference count        */
#endif
		int					obufsize;	/* size of the obuf (bytes)                     */
		RxBuf				ibuf;		/* pointer to input buffer assigned by daemon   */
		RpcBufU				obuf;       /* output buffer (encoded args) APPENDED HERE   */
} RpcUdpXactRec;

typedef struct RpcUdpXactPoolRec_ {
	rtems_id	box;
	int			prog;
	int			version;
	int			xactSize;
} RpcUdpXactPoolRec;

/* a global hash table where all 'living' transaction
 * objects are registered.
 * A number of bits in a transaction's XID maps 1:1 to
 * an index in this table. Hence, the XACT matching
 * an RPC/UDP reply packet can quickly be found
 * The size of this table imposes a hard limit on the
 * number of all created transactions in the system.
 */
static RpcUdpXact xactHashTbl[XACT_HASHS]={0};
static u_long     xidUpper   [XACT_HASHS]={0};
static unsigned   xidHashSeed            = 0 ;

/* forward declarations */
static RpcUdpXact
sockRcv(void);

static void
rpcio_daemon(rtems_task_argument);

#ifdef MBUF_TX
ssize_t
sendto_nocpy (
		int s,
		const void *buf, size_t buflen,
		int flags,
		const struct sockaddr *toaddr, int tolen,
		void *closure,
		void (*freeproc)(caddr_t, u_int),
		void (*refproc)(caddr_t, u_int)
);
static void paranoia_free(caddr_t closure, u_int size);
static void paranoia_ref (caddr_t closure, u_int size);
#define SENDTO	sendto_nocpy
#else
#define SENDTO	sendto
#endif

static RpcUdpServer		rpcUdpServers = 0;	/* linked list of all servers; protected by llock */

static int				ourSock = -1;		/* the socket we are using for communication */
static rtems_id			rpciod  = 0;		/* task id of the RPC daemon                 */
static rtems_id			msgQ    = 0;		/* message queue where the daemon picks up
											 * requests
											 */
#ifndef NDEBUG
static rtems_id			llock	= 0;		/* MUTEX protecting the server list */
static rtems_id			hlock	= 0;		/* MUTEX protecting the hash table and the list of servers */
#endif
static rtems_id			fini	= 0;		/* a synchronization semaphore we use during
											 * module cleanup / driver unloading
											 */
static rtems_interval	ticksPerSec;		/* cached system clock rate (WHO IS ASSUMED NOT
											 * TO CHANGE)
											 */

rtems_task_priority		rpciodPriority = 0;

#if (DEBUG) & DEBUG_MALLOC
/* malloc wrappers for debugging */
static int nibufs = 0;

static inline void *MY_MALLOC(int s)
{
	if (s) {
		void *rval;
		MU_LOCK(hlock);
		assert(nibufs++ < 2000);
		MU_UNLOCK(hlock);
		assert((rval = malloc(s)) != 0);
		return rval;
	}
	return 0;
}

static inline void *MY_CALLOC(int n, int s)
{
	if (s) {
		void *rval;
		MU_LOCK(hlock);
		assert(nibufs++ < 2000);
		MU_UNLOCK(hlock);
		assert((rval = calloc(n,s)) != 0);
		return rval;
	}
	return 0;
}


static inline void MY_FREE(void *p)
{
	if (p) {
		MU_LOCK(hlock);
		nibufs--;
		MU_UNLOCK(hlock);
		free(p);
	}
}
#else
#define MY_MALLOC	malloc
#define MY_CALLOC	calloc
#define MY_FREE		free
#endif

static inline bool_t
locked_marshal(RpcUdpServer s, XDR *xdrs)
{
bool_t rval;
	MU_LOCK(s->authlock);
	rval = AUTH_MARSHALL(s->auth, xdrs);
	MU_UNLOCK(s->authlock);
	return rval;
}

/* Locked operations on a server's auth object */
static inline bool_t
locked_validate(RpcUdpServer s, struct opaque_auth *v)
{
bool_t rval;
	MU_LOCK(s->authlock);
	rval = AUTH_VALIDATE(s->auth, v);
	MU_UNLOCK(s->authlock);
	return rval;
}

static inline bool_t
locked_refresh(RpcUdpServer s)
{
bool_t rval;
	MU_LOCK(s->authlock);
	rval = AUTH_REFRESH(s->auth);
	MU_UNLOCK(s->authlock);
	return rval;
}

/* Create a server object
 *
 */
enum clnt_stat
rpcUdpServerCreate(
	struct sockaddr_in	*paddr,
	rpcprog_t		prog,
	rpcvers_t		vers,
	u_long			uid,
	u_long			gid,
	RpcUdpServer		*psrv
	)
{
RpcUdpServer	rval;
u_short			port;
char			hname[MAX_MACHINE_NAME + 1];
int				theuid, thegid;
int				thegids[NGRPS];
gid_t			gids[NGROUPS];
int				len,i;
AUTH			*auth;
enum clnt_stat	pmap_err;
struct pmap		pmaparg;

	if ( gethostname(hname, MAX_MACHINE_NAME) ) {
		fprintf(stderr,
				"RPCIO - error: I have no hostname ?? (%s)\n",
				strerror(errno));
		return RPC_UNKNOWNHOST;
	}

	if ( (len = getgroups(NGROUPS, gids) < 0 ) ) {
		fprintf(stderr,
				"RPCIO - error: I unable to get group ids (%s)\n",
				strerror(errno));
		return RPC_FAILED;
	}

	if ( len > NGRPS )
		len = NGRPS;

	for (i=0; i<len; i++)
		thegids[i] = (int)gids[i];

	theuid = (int) ((RPCIOD_DEFAULT_ID == uid) ? geteuid() : uid);
	thegid = (int) ((RPCIOD_DEFAULT_ID == gid) ? getegid() : gid);

	if ( !(auth = authunix_create(hname, theuid, thegid, len, thegids)) ) {
		fprintf(stderr,
				"RPCIO - error: unable to create RPC AUTH\n");
		return RPC_FAILED;
	}

	/* if they specified no port try to ask the portmapper */
	if (!paddr->sin_port) {

		paddr->sin_port = htons(PMAPPORT);

        pmaparg.pm_prog = prog;
        pmaparg.pm_vers = vers;
        pmaparg.pm_prot = IPPROTO_UDP;
        pmaparg.pm_port = 0;  /* not needed or used */


		/* dont use non-reentrant pmap_getport ! */

		pmap_err = rpcUdpCallRp(
						paddr,
						PMAPPROG,
						PMAPVERS,
						PMAPPROC_GETPORT,
						xdr_pmap,
						&pmaparg,
						xdr_u_short,
						&port,
						uid,
						gid,
						0);

		if ( RPC_SUCCESS != pmap_err ) {
			paddr->sin_port = 0;
			return pmap_err;
		}

		paddr->sin_port = htons(port);
	}

	if (0==paddr->sin_port) {
			return RPC_PROGNOTREGISTERED;
	}

	rval       			= (RpcUdpServer)MY_MALLOC(sizeof(*rval));
	memset(rval, 0, sizeof(*rval));

	if (!inet_ntop(AF_INET, &paddr->sin_addr, rval->name, sizeof(rval->name)))
		sprintf(rval->name,"?.?.?.?");
	rval->addr.sin		= *paddr;

	/* start with a long retransmission interval - it
	 * will be adapted dynamically
	 */
	rval->retry_period  = RPCIOD_RETX_CAP_S * ticksPerSec;

	rval->auth 			= auth;

	MU_CREAT( &rval->authlock );

	/* link into list */
	MU_LOCK( llock );
	rval->next = rpcUdpServers;
	rpcUdpServers = rval;
	MU_UNLOCK( llock );

	*psrv				= rval;
	return RPC_SUCCESS;
}

void
rpcUdpServerDestroy(RpcUdpServer s)
{
RpcUdpServer prev;
	if (!s)
		return;
	/* we should probably verify (but how?) that nobody
	 * (at least: no outstanding XACTs) is using this
	 * server;
	 */

	/* remove from server list */
	MU_LOCK(llock);
	prev = rpcUdpServers;
	if ( s == prev ) {
		rpcUdpServers = s->next;
	} else {
		for ( ; prev ; prev = prev->next) {
			if (prev->next == s) {
				prev->next = s->next;
				break;
			}
		}
	}
	MU_UNLOCK(llock);

	/* MUST have found it */
	assert(prev);

	auth_destroy(s->auth);

	MU_DESTROY(s->authlock);
	MY_FREE(s);
}

int
rpcUdpStats(FILE *f)
{
RpcUdpServer s;

	if (!f) f = stdout;

	fprintf(f,"RPCIOD statistics:\n");

	MU_LOCK(llock);
	for (s = rpcUdpServers; s; s=s->next) {
		fprintf(f,"\nServer -- %s:\n", s->name);
		fprintf(f,"  requests    sent: %10ld, retransmitted: %10ld\n",
						s->requests, s->retrans);
		fprintf(f,"         timed out: %10ld,   send errors: %10ld\n",
						s->timeouts, s->errors);
		fprintf(f,"  current retransmission interval: %dms\n",
						(unsigned)(s->retry_period * 1000 / ticksPerSec) );
	}
	MU_UNLOCK(llock);

	return 0;
}

RpcUdpXact
rpcUdpXactCreate(
	u_long	program,
	u_long	version,
	u_long	size
	)
{
RpcUdpXact		rval=0;
struct rpc_msg	header;
register int	i,j;

	if (!size)
		size = UDPMSGSIZE;
	/* word align */
	size = (size + 3) & ~3;

	rval = (RpcUdpXact)MY_CALLOC(1,sizeof(*rval) - sizeof(rval->obuf) + size);

	if (rval) {

		header.rm_xid             = 0;
		header.rm_direction       = CALL;
		header.rm_call.cb_rpcvers = RPC_MSG_VERSION;
		header.rm_call.cb_prog    = program;
		header.rm_call.cb_vers    = version;
		xdrmem_create(&(rval->xdrs), rval->obuf.buf, size, XDR_ENCODE);

		if (!xdr_callhdr(&(rval->xdrs), &header)) {
			MY_FREE(rval);
			return 0;
		}
		/* pick a free table slot and initialize the XID */
		rval->obuf.xid = time(0) ^ (uintptr_t)rval;
		MU_LOCK(hlock);
		rval->obuf.xid = (xidHashSeed++ ^ ((uintptr_t)rval>>10)) & XACT_HASH_MSK;
		i=j=(rval->obuf.xid & XACT_HASH_MSK);
		if (msgQ) {
			/* if there's no message queue, refuse to
			 * give them transactions; we might be in the process to
			 * go away...
			 */
			do {
				i=(i+1) & XACT_HASH_MSK; /* cheap modulo */
				if (!xactHashTbl[i]) {
#if (DEBUG) & DEBUG_TRACE_XACT
					fprintf(stderr,"RPCIO: entering index %i, val %x\n",i,rval);
#endif
					xactHashTbl[i]=rval;
					j=-1;
					break;
				}
			} while (i!=j);
		}
		MU_UNLOCK(hlock);
		if (i==j) {
			XDR_DESTROY(&rval->xdrs);
			MY_FREE(rval);
			return 0;
		}
		rval->obuf.xid  = xidUpper[i] | i;
		rval->xdrpos    = XDR_GETPOS(&(rval->xdrs));
		rval->obufsize  = size;
	}
	return rval;
}

void
rpcUdpXactDestroy(RpcUdpXact xact)
{
int i = xact->obuf.xid & XACT_HASH_MSK;

#if (DEBUG) & DEBUG_TRACE_XACT
		fprintf(stderr,"RPCIO: removing index %i, val %x\n",i,xact);
#endif

		ASSERT( xactHashTbl[i]==xact );

		MU_LOCK(hlock);
		xactHashTbl[i]=0;
		/* remember XID we used last time so we can avoid
		 * reusing the same one (incremented by rpcUdpSend routine)
		 */
		xidUpper[i]   = xact->obuf.xid & ~XACT_HASH_MSK;
		MU_UNLOCK(hlock);

		bufFree(&xact->ibuf);

		XDR_DESTROY(&xact->xdrs);
		MY_FREE(xact);
}



/* Send a transaction, i.e. enqueue it to the
 * RPC daemon who will actually send it.
 */
enum clnt_stat
rpcUdpSend(
	RpcUdpXact		xact,
	RpcUdpServer	srvr,
	struct timeval	*timeout,
	u_long			proc,
	xdrproc_t		xres, caddr_t pres,
	xdrproc_t		xargs, caddr_t pargs,
	...
   )
{
register XDR	*xdrs;
unsigned long	ms;
va_list			ap;

	va_start(ap,pargs);

	if (!timeout)
		timeout = RPCIOD_DEFAULT_TIMEOUT;

	ms = 1000 * timeout->tv_sec + timeout->tv_usec/1000;

	/* round lifetime to closest # of ticks */
	xact->lifetime  = (ms * ticksPerSec + 500) / 1000;
	if ( 0 == xact->lifetime )
		xact->lifetime = 1;

#if (DEBUG) & DEBUG_TIMEOUT
	{
	static int once=0;
	if (!once++) {
		fprintf(stderr,
				"Initial lifetime: %i (ticks)\n",
				xact->lifetime);
	}
	}
#endif

	xact->tolive    = xact->lifetime;

	xact->xres      = xres;
	xact->pres      = pres;
	xact->server    = srvr;

	xdrs            = &xact->xdrs;
	xdrs->x_op      = XDR_ENCODE;
	/* increment transaction ID */
	xact->obuf.xid += XACT_HASHS;
	XDR_SETPOS(xdrs, xact->xdrpos);
	if ( !XDR_PUTLONG(xdrs,(long*)&proc) || !locked_marshal(srvr, xdrs) ||
		 !xargs(xdrs, pargs) ) {
		va_end(ap);
		return(xact->status.re_status=RPC_CANTENCODEARGS);
	}
	while ((xargs=va_arg(ap,xdrproc_t))) {
		if (!xargs(xdrs, va_arg(ap,caddr_t)))
		va_end(ap);
		return(xact->status.re_status=RPC_CANTENCODEARGS);
	}

	va_end(ap);

	rtems_task_ident(RTEMS_SELF, RTEMS_WHO_AM_I, &xact->requestor);
	if ( rtems_message_queue_send( msgQ, &xact, sizeof(xact)) ) {
		return RPC_CANTSEND;
	}
	/* wakeup the rpciod */
	ASSERT( RTEMS_SUCCESSFUL==rtems_event_send(rpciod, RPCIOD_TX_EVENT) );

	return RPC_SUCCESS;
}

/* Block for the RPC reply to an outstanding
 * transaction.
 * The caller is woken by the RPC daemon either
 * upon reception of the reply or on timeout.
 */
enum clnt_stat
rpcUdpRcv(RpcUdpXact xact)
{
int					refresh;
XDR			reply_xdrs;
struct rpc_msg		reply_msg;
rtems_status_code	status;
rtems_event_set		gotEvents;

	refresh = 0;

	do {

	/* block for the reply */
	status = rtems_event_receive(
		RTEMS_RPC_EVENT,
		RTEMS_WAIT | RTEMS_EVENT_ANY,
		RTEMS_NO_TIMEOUT,
		&gotEvents);
	ASSERT( status == RTEMS_SUCCESSFUL );

	if (xact->status.re_status) {
#ifdef MBUF_RX
		/* add paranoia */
		ASSERT( !xact->ibuf );
#endif
		return xact->status.re_status;
	}

#ifdef MBUF_RX
	xdrmbuf_create(&reply_xdrs, xact->ibuf, XDR_DECODE);
#else
	xdrmem_create(&reply_xdrs, xact->ibuf->buf, xact->ibufsize, XDR_DECODE);
#endif

	reply_msg.acpted_rply.ar_verf          = _null_auth;
	reply_msg.acpted_rply.ar_results.where = xact->pres;
	reply_msg.acpted_rply.ar_results.proc  = xact->xres;

	if (xdr_replymsg(&reply_xdrs, &reply_msg)) {
		/* OK */
		_seterr_reply(&reply_msg, &xact->status);
		if (RPC_SUCCESS == xact->status.re_status) {
			if ( !locked_validate(xact->server,
								&reply_msg.acpted_rply.ar_verf) ) {
				xact->status.re_status = RPC_AUTHERROR;
				xact->status.re_why    = AUTH_INVALIDRESP;
			}
			if (reply_msg.acpted_rply.ar_verf.oa_base) {
				reply_xdrs.x_op = XDR_FREE;
				xdr_opaque_auth(&reply_xdrs, &reply_msg.acpted_rply.ar_verf);
			}
			refresh = 0;
		} else {
			/* should we try to refresh our credentials ? */
			if ( !refresh ) {
				/* had never tried before */
				refresh = RPCIOD_REFRESH;
			}
		}
	} else {
		reply_xdrs.x_op        = XDR_FREE;
		xdr_replymsg(&reply_xdrs, &reply_msg);
		xact->status.re_status = RPC_CANTDECODERES;
	}
	XDR_DESTROY(&reply_xdrs);

	bufFree(&xact->ibuf);

#ifndef MBUF_RX
	xact->ibufsize = 0;
#endif

	if (refresh && locked_refresh(xact->server)) {
		rtems_task_ident(RTEMS_SELF, RTEMS_WHO_AM_I, &xact->requestor);
		if ( rtems_message_queue_send(msgQ, &xact, sizeof(xact)) ) {
			return RPC_CANTSEND;
		}
		/* wakeup the rpciod */
		fprintf(stderr,"RPCIO INFO: refreshing my AUTH\n");
		ASSERT( RTEMS_SUCCESSFUL==rtems_event_send(rpciod, RPCIOD_TX_EVENT) );
	}

	} while ( 0 &&  refresh-- > 0 );

	return xact->status.re_status;
}


/* On RTEMS, I'm told to avoid select(); this seems to
 * be more efficient
 */
static void
rxWakeupCB(struct socket *sock, void *arg)
{
  rtems_id *rpciod = (rtems_id*) arg;
  rtems_event_send(*rpciod, RPCIOD_RX_EVENT);
}

int
rpcUdpInit(void)
{
int			s;
rtems_status_code	status;
int			noblock = 1;
struct sockwakeup	wkup;

	if (ourSock < 0) {
    fprintf(stderr,"RTEMS-RPCIOD $Release$, " \
            "Till Straumann, Stanford/SLAC/SSRL 2002, " \
            "See LICENSE file for licensing info.\n");

		ourSock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (ourSock>=0) {
			bindresvport(ourSock,(struct sockaddr_in*)0);
			s = ioctl(ourSock, FIONBIO, (char*)&noblock);
			assert( s == 0 );
			/* assume nobody tampers with the clock !! */
			ticksPerSec = rtems_clock_get_ticks_per_second();
			MU_CREAT( &hlock );
			MU_CREAT( &llock );

			if ( !rpciodPriority ) {
				/* use configured networking priority */
				if ( ! (rpciodPriority = rtems_bsdnet_config.network_task_priority) )
					rpciodPriority = RPCIOD_PRIO;	/* fallback value */
			}

			status = rtems_task_create(
											rtems_build_name('R','P','C','d'),
											rpciodPriority,
											RPCIOD_STACK,
											RTEMS_DEFAULT_MODES,
											/* fprintf saves/restores FP registers on PPC :-( */
											RTEMS_DEFAULT_ATTRIBUTES | RTEMS_FLOATING_POINT,
											&rpciod);
			assert( status == RTEMS_SUCCESSFUL );

			wkup.sw_pfn = rxWakeupCB;
			wkup.sw_arg = &rpciod;
			assert( 0==setsockopt(ourSock, SOL_SOCKET, SO_RCVWAKEUP, &wkup, sizeof(wkup)) );
			status = rtems_message_queue_create(
											rtems_build_name('R','P','C','q'),
											RPCIOD_QDEPTH,
											sizeof(RpcUdpXact),
											RTEMS_DEFAULT_ATTRIBUTES,
											&msgQ);
			assert( status == RTEMS_SUCCESSFUL );
			status = rtems_task_start( rpciod, rpcio_daemon, 0 );
			assert( status == RTEMS_SUCCESSFUL );

		} else {
			return -1;
		}
	}
	return 0;
}

int
rpcUdpCleanup(void)
{
	rtems_semaphore_create(
			rtems_build_name('R','P','C','f'),
			0,
			RTEMS_DEFAULT_ATTRIBUTES,
			0,
			&fini);
	rtems_event_send(rpciod, RPCIOD_KILL_EVENT);
	/* synchronize with daemon */
	rtems_semaphore_obtain(fini, RTEMS_WAIT, 5*ticksPerSec);
	/* if the message queue is still there, something went wrong */
	if (!msgQ) {
		rtems_task_delete(rpciod);
	}
	rtems_semaphore_delete(fini);
	return (msgQ !=0);
}

/* Another API - simpler but less efficient.
 * For each RPCall, a server and a Xact
 * are created and destroyed on the fly.
 *
 * This should be used for infrequent calls
 * (e.g. a NFS mount request).
 *
 * This is roughly compatible with the original
 * clnt_call() etc. API - but it uses our
 * daemon and is fully reentrant.
 */
enum clnt_stat
rpcUdpClntCreate(
		struct sockaddr_in	*psaddr,
		rpcprog_t		prog,
		rpcvers_t		vers,
		u_long			uid,
		u_long			gid,
		RpcUdpClnt		*pclnt
)
{
RpcUdpXact		x;
RpcUdpServer	s;
enum clnt_stat	err;

	if ( RPC_SUCCESS != (err=rpcUdpServerCreate(psaddr, prog, vers, uid, gid, &s)) )
		return err;

	if ( !(x=rpcUdpXactCreate(prog, vers, UDPMSGSIZE)) ) {
		rpcUdpServerDestroy(s);
		return RPC_FAILED;
	}
	/* TODO: could maintain a server cache */

	x->server = s;

	*pclnt = x;

	return RPC_SUCCESS;
}

void
rpcUdpClntDestroy(RpcUdpClnt xact)
{
	rpcUdpServerDestroy(xact->server);
	rpcUdpXactDestroy(xact);
}

enum clnt_stat
rpcUdpClntCall(
	RpcUdpClnt		xact,
	u_long			proc,
	XdrProcT		xargs,
	CaddrT			pargs,
	XdrProcT		xres,
	CaddrT			pres,
	struct timeval	*timeout
	)
{
enum clnt_stat	stat;

		if ( (stat = rpcUdpSend(xact, xact->server, timeout, proc,
								xres, pres,
								xargs, pargs,
								0)) ) {
			fprintf(stderr,"RPCIO Send failed: %i\n",stat);
			return stat;
		}
		return rpcUdpRcv(xact);
}

/* a yet simpler interface */
enum clnt_stat
rpcUdpCallRp(
	struct sockaddr_in	*psrvr,
	u_long				prog,
	u_long				vers,
	u_long				proc,
	XdrProcT			xargs,
	CaddrT				pargs,
	XdrProcT			xres,
	CaddrT				pres,
	u_long				uid,		/* RPCIO_DEFAULT_ID picks default */
	u_long				gid,		/* RPCIO_DEFAULT_ID picks default */
	struct timeval		*timeout	/* NULL picks default		*/
)
{
RpcUdpClnt			clp;
enum clnt_stat		stat;

	stat = rpcUdpClntCreate(
				psrvr,
				prog,
				vers,
				uid,
				gid,
				&clp);

	if ( RPC_SUCCESS != stat )
		return stat;

	stat = rpcUdpClntCall(
				clp,
				proc,
				xargs, pargs,
				xres,  pres,
				timeout);

	rpcUdpClntDestroy(clp);

	return stat;
}

/* linked list primitives */
static void
nodeXtract(ListNode n)
{
	if (n->prev)
		n->prev->next = n->next;
	if (n->next)
		n->next->prev = n->prev;
	n->next = n->prev = 0;
}

static void
nodeAppend(ListNode l, ListNode n)
{
	if ( (n->next = l->next) )
		n->next->prev = n;
	l->next = n;
	n->prev = l;

}

/* this code does the work */
static void
rpcio_daemon(rtems_task_argument arg)
{
rtems_status_code stat;
RpcUdpXact        xact;
RpcUdpServer      srv;
rtems_interval    next_retrans, then, unow;
long			  			now;	/* need to do signed comparison with age! */
rtems_event_set   events;
ListNode          newList;
size_t            size;
rtems_id          q          =  0;
ListNodeRec       listHead   = {0, 0};
unsigned long     epoch      = RPCIOD_EPOCH_SECS * ticksPerSec;
unsigned long			max_period = RPCIOD_RETX_CAP_S * ticksPerSec;
rtems_status_code	status;


        then = rtems_clock_get_ticks_since_boot();

	for (next_retrans = epoch;;) {

		if ( RTEMS_SUCCESSFUL !=
			 (stat = rtems_event_receive(
						RPCIOD_RX_EVENT | RPCIOD_TX_EVENT | RPCIOD_KILL_EVENT,
						RTEMS_WAIT | RTEMS_EVENT_ANY,
						next_retrans,
						&events)) ) {
			ASSERT( RTEMS_TIMEOUT == stat );
			events = 0;
		}

		if (events & RPCIOD_KILL_EVENT) {
			int i;

#if (DEBUG) & DEBUG_EVENTS
			fprintf(stderr,"RPCIO: got KILL event\n");
#endif

			MU_LOCK(hlock);
			for (i=XACT_HASHS-1; i>=0; i--) {
				if (xactHashTbl[i]) {
					break;
				}
			}
			if (i<0) {
				/* prevent them from creating and enqueueing more messages */
				q=msgQ;
				/* messages queued after we executed this assignment will fail */
				msgQ=0;
			}
			MU_UNLOCK(hlock);
			if (i>=0) {
				fprintf(stderr,"RPCIO There are still transactions circulating; I refuse to go away\n");
				fprintf(stderr,"(1st in slot %i)\n",i);
				rtems_semaphore_release(fini);
			} else {
				break;
			}
		}

        	unow = rtems_clock_get_ticks_since_boot();

		/* measure everything relative to then to protect against
		 * rollover
		 */
		now = unow - then;

		/* NOTE: we don't lock the hash table while we are operating
		 * on transactions; the paradigm is that we 'own' a particular
		 * transaction (and hence it's hash table slot) from the
		 * time the xact was put into the message queue until we
		 * wake up the requestor.
		 */

		if (RPCIOD_RX_EVENT & events) {

#if (DEBUG) & DEBUG_EVENTS
			fprintf(stderr,"RPCIO: got RX event\n");
#endif

			while ((xact=sockRcv())) {

				/* extract from the retransmission list */
				nodeXtract(&xact->node);

				/* change the ID - there might already be
				 * a retransmission on the way. When it's
				 * reply arrives we must not find it's ID
				 * in the hashtable
				 */
				xact->obuf.xid        += XACT_HASHS;

				xact->status.re_status = RPC_SUCCESS;

				/* calculate roundtrip ticks */
				xact->trip             = now - xact->trip;

				srv                    = xact->server;

				/* adjust the server's retry period */
				{
					register TimeoutT rtry = srv->retry_period;
					register TimeoutT trip = xact->trip;

					ASSERT( trip >= 0 );

					if ( 0==trip )
						trip = 1;

					/* retry_new = 0.75*retry_old + 0.25 * 8 * roundrip */
					rtry   = (3*rtry + (trip << 3)) >> 2;

					if ( rtry > max_period )
						rtry = max_period;

					srv->retry_period = rtry;
				}

				/* wakeup requestor */
				rtems_event_send(xact->requestor, RTEMS_RPC_EVENT);
			}
		}

		if (RPCIOD_TX_EVENT & events) {

#if (DEBUG) & DEBUG_EVENTS
			fprintf(stderr,"RPCIO: got TX event\n");
#endif

			while (RTEMS_SUCCESSFUL == rtems_message_queue_receive(
											msgQ,
											&xact,
											&size,
											RTEMS_NO_WAIT,
											RTEMS_NO_TIMEOUT)) {
				/* put to the head of timeout q */
				nodeAppend(&listHead, &xact->node);

				xact->age  = now;
				xact->trip = FIRST_ATTEMPT;
			}
		}


		/* work the timeout q */
		newList = 0;
		for ( xact=(RpcUdpXact)listHead.next;
			  xact && xact->age <= now;
			  xact=(RpcUdpXact)listHead.next ) {

				/* extract from the list */
				nodeXtract(&xact->node);

				srv = xact->server;

				if (xact->tolive < 0) {
					/* this one timed out */
					xact->status.re_errno  = ETIMEDOUT;
					xact->status.re_status = RPC_TIMEDOUT;

					srv->timeouts++;

					/* Change the ID - there might still be
					 * a reply on the way. When it arrives we
					 * must not find it's ID in the hash table
					 *
					 * Thanks to Steven Johnson for hunting this
					 * one down.
					 */
					xact->obuf.xid        += XACT_HASHS;

#if (DEBUG) & DEBUG_TIMEOUT
					fprintf(stderr,"RPCIO XACT timed out; waking up requestor\n");
#endif
					if ( rtems_event_send(xact->requestor, RTEMS_RPC_EVENT) ) {
						rtems_panic("RPCIO PANIC file %s line: %i, requestor id was 0x%08x",
									__FILE__,
									__LINE__,
									xact->requestor);
					}

				} else {
					int len;

					len = (int)XDR_GETPOS(&xact->xdrs);

#ifdef MBUF_TX
					xact->refcnt = 1;	/* sendto itself */
#endif
					if ( len != SENDTO( ourSock,
										xact->obuf.buf,
										len,
										0,
										&srv->addr.sa,
										sizeof(srv->addr.sin)
#ifdef MBUF_TX
										, xact,
										paranoia_free,
										paranoia_ref
#endif
										) ) {

						xact->status.re_errno  = errno;
						xact->status.re_status = RPC_CANTSEND;
						srv->errors++;

						/* wakeup requestor */
						fprintf(stderr,"RPCIO: SEND failure\n");
						status = rtems_event_send(xact->requestor, RTEMS_RPC_EVENT);
						assert( status == RTEMS_SUCCESSFUL );

					} else {
						/* send successful; calculate retransmission time
						 * and enqueue to temporary list
						 */
						if (FIRST_ATTEMPT != xact->trip) {
#if (DEBUG) & DEBUG_TIMEOUT
							fprintf(stderr,
								"timed out; tolive is %i (ticks), retry period is %i (ticks)\n",
									xact->tolive,
									srv->retry_period);
#endif
							/* this is a real retry; we backup
							 * the server's retry interval
							 */
							if ( srv->retry_period < max_period ) {

								/* If multiple transactions for this server
								 * fail (e.g. because it died) this will
								 * back-off very agressively (doubling
								 * the retransmission period for every
								 * timed out transaction up to the CAP limit)
								 * which is desirable - single packet failure
								 * is treated more gracefully by this algorithm.
								 */

								srv->retry_period<<=1;
#if (DEBUG) & DEBUG_TIMEOUT
								fprintf(stderr,
										"adjusted to; retry period %i\n",
										srv->retry_period);
#endif
							} else {
								/* never wait longer than RPCIOD_RETX_CAP_S seconds */
								fprintf(stderr,
										"RPCIO: server '%s' not responding - still trying\n",
										srv->name);
							}
							if ( 0 == ++srv->retrans % 1000) {
								fprintf(stderr,
										"RPCIO - statistics: already %li retries to server %s\n",
										srv->retrans,
										srv->name);
							}
						} else {
							srv->requests++;
						}
						xact->trip      = now;
						{
						long capped_period = srv->retry_period;
							if ( xact->lifetime < capped_period )
								capped_period = xact->lifetime;
						xact->age       = now + capped_period;
						xact->tolive   -= capped_period;
						}
						/* enqueue to the list of newly sent transactions */
						xact->node.next = newList;
						newList         = &xact->node;
#if (DEBUG) & DEBUG_TIMEOUT
						fprintf(stderr,
								"XACT (0x%08x) age is 0x%x, now: 0x%x\n",
								xact,
								xact->age,
								now);
#endif
					}
				}
	    }

		/* insert the newly sent transactions into the
		 * sorted retransmission list
		 */
		for (; (xact = (RpcUdpXact)newList); ) {
			register ListNode p,n;
			newList = newList->next;
			for ( p=&listHead; (n=p->next) && xact->age > ((RpcUdpXact)n)->age; p=n )
				/* nothing else to do */;
			nodeAppend(p, &xact->node);
		}

		if (now > epoch) {
			/* every now and then, readjust the epoch */
			register ListNode n;
			then += now;
			for (n=listHead.next; n; n=n->next) {
				/* readjust outstanding time intervals subject to the
				 * condition that the 'absolute' time must remain
				 * the same. 'age' and 'trip' are measured with
				 * respect to 'then' - hence:
				 *
				 * abs_age == old_age + old_then == new_age + new_then
				 *
				 * ==> new_age = old_age + old_then - new_then == old_age - 'now'
				 */
				((RpcUdpXact)n)->age  -= now;
				((RpcUdpXact)n)->trip -= now;
#if (DEBUG) & DEBUG_TIMEOUT
				fprintf(stderr,
						"readjusted XACT (0x%08x); age is 0x%x, trip: 0x%x now: 0x%x\n",
						(RpcUdpXact)n,
						((RpcUdpXact)n)->trip,
						((RpcUdpXact)n)->age,
						now);
#endif
			}
			now = 0;
		}

		next_retrans = listHead.next ?
							((RpcUdpXact)listHead.next)->age - now :
							epoch;	/* make sure we don't miss updating the epoch */
#if (DEBUG) & DEBUG_TIMEOUT
		fprintf(stderr,"RPCIO: next timeout is %x\n",next_retrans);
#endif
	}
	/* close our socket; shut down the receiver */
	close(ourSock);

#if 0 /* if we get here, no transactions exist, hence there can be none
	   * in the queue whatsoever
	   */
	/* flush the message queue */
	while (RTEMS_SUCCESSFUL == rtems_message_queue_receive(
										q,
										&xact,
										&size,
										RTEMS_NO_WAIT,
										RTEMS_NO_TIMEOUT)) {
			/* TODO enque xact */
	}

	/* flush all outstanding transactions */

	for (xact=((RpcUdpXact)listHead.next); xact; xact=((RpcUdpXact)xact->node.next)) {
			xact->status.re_status = RPC_TIMEDOUT;
			rtems_event_send(xact->requestor, RTEMS_RPC_EVENT);
	}
#endif

	rtems_message_queue_delete(q);

	MU_DESTROY(hlock);

	fprintf(stderr,"RPC daemon exited...\n");

	rtems_semaphore_release(fini);
	rtems_task_suspend(RTEMS_SELF);
}


/* support for transaction 'pools'. A number of XACT objects
 * is always kept around. The initial number is 0 but it
 * is allowed to grow up to a maximum.
 * If the need grows beyond the maximum, behavior depends:
 * Users can either block until a transaction becomes available,
 * they can create a new XACT on the fly or get an error
 * if no free XACT is available from the pool.
 */

RpcUdpXactPool
rpcUdpXactPoolCreate(
	rpcprog_t prog, 		rpcvers_t version,
	int xactsize,	int poolsize)
{
RpcUdpXactPool	rval = MY_MALLOC(sizeof(*rval));
rtems_status_code	status;

	ASSERT( rval );
	status = rtems_message_queue_create(
					rtems_build_name('R','P','C','p'),
					poolsize,
					sizeof(RpcUdpXact),
					RTEMS_DEFAULT_ATTRIBUTES,
					&rval->box);
	assert( status == RTEMS_SUCCESSFUL );

	rval->prog     = prog;
	rval->version  = version;
	rval->xactSize = xactsize;
	return rval;
}

void
rpcUdpXactPoolDestroy(RpcUdpXactPool pool)
{
RpcUdpXact xact;

	while ((xact = rpcUdpXactPoolGet(pool, XactGetFail))) {
		rpcUdpXactDestroy(xact);
	}
	rtems_message_queue_delete(pool->box);
	MY_FREE(pool);
}

RpcUdpXact
rpcUdpXactPoolGet(RpcUdpXactPool pool, XactPoolGetMode mode)
{
RpcUdpXact	 xact = 0;
size_t           size;

	if (RTEMS_SUCCESSFUL != rtems_message_queue_receive(
								pool->box,
								&xact,
								&size,
								XactGetWait == mode ?
									RTEMS_WAIT : RTEMS_NO_WAIT,
								RTEMS_NO_TIMEOUT)) {

		/* nothing found in box; should we create a new one ? */

		xact = (XactGetCreate == mode) ?
					rpcUdpXactCreate(
							pool->prog,
							pool->version,
							pool->xactSize) : 0 ;
		if (xact)
				xact->pool = pool;

	}
	return xact;
}

void
rpcUdpXactPoolPut(RpcUdpXact xact)
{
RpcUdpXactPool pool;

	pool = xact->pool;
	ASSERT( pool );

	if (RTEMS_SUCCESSFUL != rtems_message_queue_send(
								pool->box,
								&xact,
								sizeof(xact)))
		rpcUdpXactDestroy(xact);
}

#ifdef MBUF_RX

/* WORKAROUND: include sys/mbuf.h (or other bsdnet headers) only
 *             _after_ using malloc()/free() & friends because
 *             the RTEMS/BSDNET headers redefine those :-(
 */

#define _KERNEL
#include <sys/mbuf.h>

ssize_t
recv_mbuf_from(int s, struct mbuf **ppm, long len, struct sockaddr *fromaddr, int *fromlen);

static void
bufFree(struct mbuf **m)
{
	if (*m) {
		rtems_bsdnet_semaphore_obtain();
		m_freem(*m);
		rtems_bsdnet_semaphore_release();
		*m = 0;
	}
}
#endif

#ifdef MBUF_TX
static void
paranoia_free(caddr_t closure, u_int size)
{
#if (DEBUG)
RpcUdpXact xact = (RpcUdpXact)closure;
int        len  = (int)XDR_GETPOS(&xact->xdrs);

	ASSERT( --xact->refcnt >= 0 && size == len );
#endif
}

static void
paranoia_ref (caddr_t closure, u_int size)
{
#if (DEBUG)
RpcUdpXact xact = (RpcUdpXact)closure;
int        len  = (int)XDR_GETPOS(&xact->xdrs);
	ASSERT( size == len );
	xact->refcnt++;
#endif
}
#endif

/* receive from a socket and find
 * the transaction corresponding to the
 * transaction ID received in the server
 * reply.
 *
 * The semantics of the 'pibuf' pointer are
 * as follows:
 *
 * MBUF_RX:
 *
 */

#define RPCIOD_RXBUFSZ	UDPMSGSIZE

static RpcUdpXact
sockRcv(void)
{
int					len,i;
uint32_t				xid;
union {
	struct sockaddr_in	sin;
	struct sockaddr     sa;
}					fromAddr;
int					fromLen  = sizeof(fromAddr.sin);
RxBuf				ibuf     = 0;
RpcUdpXact			xact     = 0;

	do {

	/* rcv_mbuf() and recvfrom() differ in that the
	 * former allocates buffers and passes them back
	 * to us whereas the latter requires us to provide
	 * buffer space.
	 * Hence, in the first case whe have to make sure
	 * no old buffer is leaked - in the second case,
	 * we might well re-use an old buffer but must
	 * make sure we have one allocated
	 */
#ifdef MBUF_RX
	if (ibuf)
		bufFree(&ibuf);

	len  = recv_mbuf_from(
					ourSock,
					&ibuf,
					RPCIOD_RXBUFSZ,
				    &fromAddr.sa,
				    &fromLen);
#else
	if ( !ibuf )
		ibuf = (RpcBuf)MY_MALLOC(RPCIOD_RXBUFSZ);
	if ( !ibuf )
		goto cleanup; /* no memory - drop this message */

	len  = recvfrom(ourSock,
				    ibuf->buf,
				    RPCIOD_RXBUFSZ,
				    0,
				    &fromAddr.sa,
					&fromLen);
#endif

	if (len <= 0) {
		if (EAGAIN != errno)
			fprintf(stderr,"RECV failed: %s\n",strerror(errno));
		goto cleanup;
	}

#if (DEBUG) & DEBUG_PACKLOSS
	if ( (unsigned)rand() < DEBUG_PACKLOSS_FRACT ) {
		/* lose packets once in a while */
		static int xxx = 0;
		if ( ++xxx % 16 == 0 )
			fprintf(stderr,"DEBUG: dropped %i packets, so far...\n",xxx);
		if ( ibuf )
			bufFree( &ibuf );
		continue;
	}
#endif

	i = (xid=XID(ibuf)) & XACT_HASH_MSK;

	if ( !(xact=xactHashTbl[i])                                             ||
		   xact->obuf.xid                     != xid                        ||
#ifdef REJECT_SERVERIP_MISMATCH
		   xact->server->addr.sin.sin_addr.s_addr != fromAddr.sin.sin_addr.s_addr	||
#endif
		   xact->server->addr.sin.sin_port        != fromAddr.sin.sin_port ) {

		if (xact) {
			if (
#ifdef REJECT_SERVERIP_MISMATCH
			    xact->server->addr.sin.sin_addr.s_addr == fromAddr.sin.sin_addr.s_addr &&
#endif
		        xact->server->addr.sin.sin_port        == fromAddr.sin.sin_port        &&
			    ( xact->obuf.xid                   == xid + XACT_HASHS   ||
				  xact->obuf.xid                   == xid + 2*XACT_HASHS    )
				) {
#ifndef DEBUG /* don't complain if it's just a late arrival of a retry */
			fprintf(stderr,"RPCIO - FYI sockRcv(): dropping late/redundant retry answer\n");
#endif
			} else {
			fprintf(stderr,"RPCIO WARNING sockRcv(): transaction mismatch\n");
			fprintf(stderr,"xact: xid  0x%08" PRIx32 "  -- got 0x%08" PRIx32 "\n",
							xact->obuf.xid, xid);
			fprintf(stderr,"xact: addr 0x%08" PRIx32 "  -- got 0x%08" PRIx32 "\n",
							xact->server->addr.sin.sin_addr.s_addr,
							fromAddr.sin.sin_addr.s_addr);
			fprintf(stderr,"xact: port 0x%08x  -- got 0x%08x\n",
							xact->server->addr.sin.sin_port,
							fromAddr.sin.sin_port);
			}
		} else {
			fprintf(stderr,
					"RPCIO WARNING sockRcv(): got xid 0x%08" PRIx32 " but its slot is empty\n",
					xid);
		}
		/* forget about this one and try again */
		xact = 0;
	}

	} while ( !xact );

	xact->ibuf     = ibuf;
#ifndef MBUF_RX
	xact->ibufsize = RPCIOD_RXBUFSZ;
#endif

	return xact;

cleanup:

	bufFree(&ibuf);

	return 0;
}


#include <rtems/rtems_bsdnet_internal.h>
/* double check the event configuration; should probably globally
 * manage system events!!
 * We do this at the end of the file for the same reason we had
 * included mbuf.h only a couple of lines above - see comment up
 * there...
 */
#if RTEMS_RPC_EVENT & SOSLEEP_EVENT & SBWAIT_EVENT & NETISR_EVENTS
#error ILLEGAL EVENT CONFIGURATION
#endif

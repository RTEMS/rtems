#include <rpc/types.h>
#include <rpc/rpc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bsp.h>
#include <rdbg/servrpc.h>
#include <rdbg/remdeb.h>
#define printf(c)
/*HEADER_START*/
#define RTEMS_PORT 2071
#define RTEMS_BACK_PORT 2073
/*
 * Sun request values for the remote ptrace system call
 */

 /*
   * Memory data for read/write text or data. The size is in data. The target
   * addr is in the addr field.
   * Be careful before modifying because this value goes into internal
   * pipes and is allocated on stack too. Pipes and/or the stack could
   * become too small if this value gets incremented.
   */
 /* now open_connex() routine which establishes a connection to server */
#define DEBUGGER_IS_GDB 0x2 /* */
 /* now close_connex() routine which detaches from server */
 /* now send_signal() routine which sends signals to processes like kill(2) */
 /* now wait_info() routine which returns results of polling the wait status
	of a process/actor. It may return 0 if running, else pid or -1 */
 /* now ptrace() routine. This matches the Sun UNIX ptrace as well as
	some additions */
 /* now define the actual calls we support */
const char* names [] = {
 "NULLPROC", "OPEN_CONNEX", "SEND_SIGNAL", "name3",
 "name4", "name5", "name6", "name7",
 "name8", "name9", "CLOSE_CONNEX", "PTRACE",
 "name12", "WAIT_INFO", "name14", "name15",
 "name16", "GET_SIGNAL_NAMES", "name18"
};

void
remotedeb_2(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		open_in open_connex_2_arg;
		signal_in send_signal_2_arg;
		close_in close_connex_2_arg;
		ptrace_in ptrace_2_arg;
		wait_in wait_info_2_arg;
	} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	DPRINTF (("remotedeb_2: %s (%d)\n", 
		(unsigned) rqstp->rq_proc < 
		(unsigned) (sizeof names / sizeof names[0]) ?
		names [rqstp->rq_proc] : "???", 
		(int) rqstp->rq_proc));

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		return;

	case OPEN_CONNEX:
		_xdr_argument = (xdrproc_t) xdr_open_in;
		_xdr_result = (xdrproc_t) xdr_open_out;
		local = (char *(*)(char *, struct svc_req *)) open_connex_2_svc;
		break;

	case SEND_SIGNAL:
		_xdr_argument = (xdrproc_t) xdr_signal_in;
		_xdr_result = (xdrproc_t) xdr_signal_out;
		local = (char *(*)(char *, struct svc_req *)) send_signal_2_svc;
		break;

	case CLOSE_CONNEX:
		_xdr_argument = (xdrproc_t) xdr_close_in;
		_xdr_result = (xdrproc_t) xdr_void;
		local = (char *(*)(char *, struct svc_req *)) close_connex_2_svc;
		break;

	case PTRACE:
		_xdr_argument = (xdrproc_t) xdr_ptrace_in;
		_xdr_result = (xdrproc_t) xdr_ptrace_out;
		local = (char *(*)(char *, struct svc_req *)) ptrace_2_svc;
		break;

	case WAIT_INFO:
		_xdr_argument = (xdrproc_t) xdr_wait_in;
		_xdr_result = (xdrproc_t) xdr_wait_out;
		local = (char *(*)(char *, struct svc_req *)) wait_info_2_svc;
		break;

	case GET_SIGNAL_NAMES:
		_xdr_argument = (xdrproc_t) xdr_void;
		_xdr_result = (xdrproc_t) xdr_get_signal_names_out;
		local = (char *(*)(char *, struct svc_req *)) get_signal_names_2_svc;
		break;

	default:
		svcerr_noproc (transp);
		return;
	}
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		return;
	}
	result = (*local)((char *)&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, _xdr_result, result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, _xdr_argument, (caddr_t) &argument)) {
		printf( "unable to free arguments");
		exit (1);
	}
	return;
}

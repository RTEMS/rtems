/*
 **************************************************************************
 *
 * Component =   RDBG
 * 
 * Synopsis  =   rdbg.h
 *
 **************************************************************************
 */

#ifndef RDBG_H
#define RDBG_H

#include <rpc/rpc.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <rdbg/rdbg_f.h>
#include <stdlib.h>		/* For malloc() and free() prototypes */
#include <bsp.h>		

#define	Malloc(size)		malloc (size)
#define	Free(block)		free (block)
#define	Realloc(block,size)	realloc (block, size)
#define	StrDup(str)		strdup(str)

#define LIST_PID	16	/* dynamic list of processes/tasks */
#define LIST_PID_DEB	17	/* list of processes under debug now */
#define LIST_PID_THREAD	18	/* list of threads for specific process */
#define LIST_CONN	19	/* dynamic list of connections */

    /* RTEMS internals */
extern void	remotedeb_2	(struct svc_req* rqstp, SVCXPRT* transp);
extern void 	setErrno 	(int error);
extern int 	getErrno	();
extern int	ptrace 		(int request, int pid, char* addr,
				 int data, char* addr2);

extern int 			TSP_RETRIES;
extern volatile int		ExitForSingleStep;
extern volatile int		justSaveContext;
extern volatile Objects_Id	currentTargetThread;
extern volatile int		CannotRestart;
extern volatile int		TotalReboot;

    /* Missing RPC prototypes */
SVCXPRT*  svcudp_create		(int fd);
void	  svc_processrequest 	(SVCXPRT* xprt,
				 u_long prog, u_long vers,
				 void (*dispatch)());
int 	  svcudp_enablecache   	(SVCXPRT *transp, u_long size);

#endif /* !RDBG_H */


/*
 **************************************************************************
 *
 * Component =   RDBG
 * 
 * Synopsis  =   rdbg.h
 *
 * $Id$
 *
 **************************************************************************
 */

#ifndef RDBG_H
#define RDBG_H

#include <rpc/rpc.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>		/* For malloc() and free() prototypes */
#include <bsp.h>		
#include <rtems.h>

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

typedef struct Exception_context_struct {
  struct Exception_context_struct *next;
  struct Exception_context_struct *previous;
  Objects_Id id;
  Objects_Id semaphoreId;
  CPU_Exception_frame *ctx;
} Exception_context;

struct		xdr_regs;

extern int 	PushExceptCtx 		(Objects_Id Id,
					 Objects_Id semId,
					 CPU_Exception_frame *ctx);
extern int	PopExceptCtx  		(Objects_Id Id);
extern Exception_context *GetExceptCtx  (Objects_Id Id);
extern int  	Single_Step		(CPU_Exception_frame* ctx);
extern int 	CheckForSingleStep 	(CPU_Exception_frame* ctx);
extern void	BreakPointExcHdl   	(CPU_Exception_frame *ctx);
extern void	CtxToRegs	   	(const CPU_Exception_frame*,struct xdr_regs*);
extern void	RegsToCtx	   	(const struct xdr_regs*,CPU_Exception_frame*);

extern void	enterRdbg		();
extern void 	get_ctx_thread		(Thread_Control *thread,
					 CPU_Exception_frame* ctx);
extern void 	set_ctx_thread		(Thread_Control *thread,
					 CPU_Exception_frame* ctx);
extern int      PushSavedExceptCtx      ( Objects_Id Id,
					  CPU_Exception_frame *ctx );
extern int	ExcepToSig		(Exception_context *ctx);

extern rtems_id serializeSemId;
extern rtems_id wakeupEventSemId;
extern volatile unsigned int NbSerializedCtx;

void copyback_data_cache_and_invalidate_instr_cache(unsigned char* addr, int size);

#include <rdbg/rdbg_f.h>

#endif /* !RDBG_H */


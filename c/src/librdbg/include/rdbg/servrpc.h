/*
 * $Id$
 */

#ifndef SERVRPC_H
#define SERVRPC_H


#include <rtems/system.h>
#include <rtems/score/cpu.h>

#include <signal.h>		
#include <stdio.h>		
#include <stdlib.h>		
#include <string.h>		

#include <rpc/types.h>		
#include <rdbg/remdeb.h>	
#include <rpc/rpc.h>		
#include <rpc/svc.h>		

extern int CONN_LIST_INC;
extern int PID_LIST_INC;
extern int TSP_RETRIES;
extern int BackPort;
extern char taskName[];
extern int getId();


#ifdef DDEBUG
int   rdb_debug;		/* True if env var RDB_DEBUG defined */
extern const char* PtraceNames[]; /* list of ptrace requests for debug out */
extern const char* BmsgNames[]; /* list of BMSG_xxx names */
extern const char* PtraceName(int req);

#ifdef i386			/* low-high machine such as 386 */
#define HL_W(w)		(((UINT16)(w)>>8)+((((w)&0xFF)<<8)))
#define HL_D(d)		(((UINT32)(d)>>24)+(((d)&0x00FF0000)>>8) \
		       +(((d)&0xFF00)<<8)+(((d)&0xFF)<<24))
#else
#define HL_W(w)		w
#define HL_D(d)		d
#endif

# define DPRINTF(a)	(rdb_debug ? printk ("%d >>> ", getId()), printk a : 0)
#else
# define DPRINTF(a)		/* suppress */
#endif

  /* Macros for analyzing/creating process status values. Presently do
	not need to be separated per target. Could even use WIF... */

#define STS_SIGNALLED(status)	(((status) & 0xFF) == 0x7F)
#define STS_TERMONSIG(status)	(((status) & 0xFF) && !STS_SIGNALLED(status))
#define STS_TERMGETSIG(status)	((status) & 0x7F)
#define STS_MAKESIG(sig)	(((sig) << 8) | 0x7f)
#define STS_GETSIG(status)	((status) >> 8)
#define STS_GETCODE(status)	((status) >> 8)


/* now define base types */
#ifndef UCHAR_DEFINED
#define UCHAR_DEFINED		/* to handle duplicate typedes */
typedef unsigned char	UCHAR;
typedef unsigned char	UINT8;
typedef char		INT8;
typedef unsigned short	UINT16;
typedef short		INT16;
typedef unsigned long	UINT32;
typedef long		INT32;
#endif				/* UCHAR_DEFINED */

typedef long		PID;	/* generalized process id */

#ifndef True
# define True	1
# define False	0
typedef char Boolean;		
#endif

#define MAX_FILENAME	1024	/* largest filename with path */
#define MAX_SEND	5	/* up to 5 pended outbound messages */

#define SERVER_VERS     1

typedef enum
{				/* message types */
  BMSG_WARM=1,			/* warm test for network connection */
  BMSG_WAIT,			/* wait change (stopped/started) */
  BMSG_BREAK,			/* breakpoint changed */
  BMSG_EXEC_FAIL,		/* exec failed from spawn */
  BMSG_DETACH,			/* process detached from server */
  BMSG_KILLED,			/* killed by server */
  BMSG_NOT_PRIM,		/* no longer the primary owner */
  BMSG_NEW_PID			/* the process was restart with new pid (in
				   context). Same ownership rules.   */
} BACK_MSG;

typedef struct 
{				/* this is the break_list[0] entry of pid */
  UCHAR		clr_step;	/* true if step off break in last_break */
  UCHAR		pad1;		/* Set if STEPEMUL breakpoints exist */
  UINT16	last_break;	/* last breakpoint we stopped on (if break) */
  UINT32	range_start;	/* start address of range */
  UINT32	range_end;	/* end address inclusive */
} BASE_BREAK;

enum 
{				/* last start values */
  LAST_NONE,			/* stopped already */
  LAST_STEP,			/* did a step last - do not send to prim */
  LAST_CONT,			/* did a continue last */
  LAST_RANGE,			/* in the middle of step-in-range */
  LAST_STEPOFF,			/* stepped off break, now need to cont */
  LAST_KILLED,			/* was killed by ptrace */
  LAST_DETACHED			/* was detached by ptrace */
};
#define LAST_START 0x80		/* first execed. This is to handle MiX
				   bug where we need to start again */

typedef struct 
{				/* one per open process */
  PID		pid;		/* process id (or 0 if free) */
  int		state;		/* status from last wait if stopped */
  UCHAR		running;	/* True if running, else stopped/term */
  /* now connection control over process */
  UCHAR		owners;		/* count of owners for notify and term release */
  UCHAR		primary_conn;	/* primary owner connection or 255=none */

  UCHAR		filler;		/* Preserve alignment */

  /* now break control and support */
  UINT8		last_start;	/* LAST_xx start info for wait() */
  UINT8		flags;		/* PIDFLG_xxx flags */
  UINT16	break_alloc;	/* number of entries in break_list */
  xdr_break	*break_list;	/* list of breakpoints ([0] is BASE_BREAK) */
  /* now registers and other save information */
  xdr_regs	regs;		/* saved registers when stopped */
  int		is_step;	/* Was break or step (regs ambiguous often) */
  int		stop_wanted;	/* Don't ignore next stop */
  UINT32	thread;		/* current stopped thread or -1 if none */
  char		*name;		/* full pathname or NULL if not known */
  INT32		child;		/* child pid that manages the pid */
  UINT32	textStart;	/* for relocating breakpoints at restart */
} PID_LIST;
PID_LIST	*pid_list;	/* array of processes being managed */
int		pid_list_cnt;	/* number of entries allocated */
UINT16		last_break;	/* unique handle generator for breaks */
#define NO_PRIMARY ((UCHAR)-1)

typedef union
{				/* an opaque net address */
  unsigned long	l[4];
  unsigned char c[16];		/* corresponds to IP, enough for ChIPC */
} NET_OPAQUE;

typedef struct
{				/* one per connection */
  UCHAR		in_use;		/* True if in use */
  UCHAR		debug_type;	/* type of connection */
  UINT16	flags;		/* flags for connection (CFLG_xxx) */
  NET_OPAQUE 	sender;		/* opaque address for transport compare */
  NET_OPAQUE 	back_port;	/* opaque address for transport event msgs */
  NET_OPAQUE	route;		/* optional route address */
  UINT32	pid_map[10];	/* map of pids owned relative to pid list */
				/* this allows up to 320 pids to be managed */
  UCHAR 	last_msg_num;	/* msg number used last to handle multi-send */
  /* next field associated with UDP send messages */
  UCHAR		retry;		/* count of retries. If 0, ok. If not 0, we
				   are in active wait for reply to an event */
  UCHAR		send_idx;	/* current number of send's pended */
  struct SEND_LIST
  {				/* holds pending msgs */
    UCHAR 	send_type;	/* BMSG_xxx type of message */
    UCHAR	retry;		/* number of times to retry */
    UINT16	spec;		/* spec field */
    PID		pid;		/* pid if applies */
    UINT32	context;	/* additional context if needed */
  }		send_list[MAX_SEND]; /* pended list of messages being sent */
  char		user_name[NAMEMAX]; /* name of user connecting in */
  /* next fields are managed at runtime to handle lists, command upload, and
     command download.							*/
  enum {LST_NONE, LST_SPAWN, LST_INFO, LST_CMD_DOWN} list_type;
  char		*list;		/* curr list we are sending/getting (malloced) */
  UINT16	list_sz;	/* size of current list (string len) */
  UINT16	list_num;	/* number of current list or position */
  UINT16	list_alloc;	/* amount allocated so far */
  UINT16	list_save;	/* used internally */
} CONN_LIST;
CONN_LIST 	*conn_list;	/* an array of connections */
int		conn_list_cnt;	/* number allocated */

    /* Operations over the PID map. Each indexes into long and then bit */
    /* 5 is log2 of 32, the number of bits in an int */
#define PIDMAP_TEST(conn,idx) \
    (conn_list [conn].pid_map [(idx) >> 5] & (1 << ((idx) & 31)))

#define PIDMAP_SET(conn,idx) \
    (conn_list [conn].pid_map [(idx) >> 5] |= 1 << ((idx) & 31))

#define PIDMAP_CLEAR(conn,idx) \
    (conn_list [conn].pid_map [(idx) >> 5] &= ~(1 << ((idx) &31)))

#define PROC_TERMINATED(plst) \
    (!(plst)->running && !STS_SIGNALLED ((plst)->state))


/* first define the Connection routines exported from servcon.c */

int ConnCreate	(struct svc_req *rqstp, open_in *in);
void ConnDelete	(int conn_idx, struct svc_req *rqstp, close_control control);

void 	TspInit	(int rpc_io_channel);
Boolean TspTranslateRpcAddr	(struct svc_req *rqstp, NET_OPAQUE *opaque);
Boolean TspValidateAddr	(NET_OPAQUE *opaque, NET_OPAQUE *sender);
int 	TspConnGetIndex	(struct svc_req *rqstp);

void 	TspSendWaitChange  (int conn_idx, BACK_MSG msg, UINT16 spec, PID pid,
			    UINT32 context, Boolean force);
void 	TspSendMessage	   (int conn_idx, Boolean resend);
void 	TspMessageReceive  (int conn_idx, PID pid);
char* 	TspGetHostName     (int conn_idx);
void 	TgtCreateNew	   (PID pid, int conn_idx, INT32 child,
			    char *name, Boolean spawn);
Boolean TgtAttach	   (int conn_idx, PID pid);
void 	TgtNotifyWaitChange(PID pid, int status, Boolean exclude);
void 	TgtNotifyAll	   (int pid_idx, BACK_MSG msg, UINT16 spec,
			    UINT32 context, int exclude_conn, Boolean force);
void 	TgtDelete	   (PID_LIST*, int conn_idx, BACK_MSG notify);
int 	TgtKillAndDelete   (PID_LIST *plst, struct svc_req *rqstp, Boolean term);
void 	TgtDetachCon	   (int conn_idx, int pid_idx, Boolean delete);
int	TgtThreadList 	   (PID_LIST*, unsigned* buf, unsigned int size);
int	TgtGetThreadName   (PID_LIST*, unsigned thLi, char* name);
int 	TgtPtrace	   (int req, PID pid, char *addr, int data, void *addr2);
int 	TgtRealPtrace      (int req, PID pid, char *addr, int data, void *addr2);
Boolean TgtHandleChildChange(PID pid, int* status, int* unexp,
			     CPU_Exception_frame *ctx);
#ifdef DDEBUG
  /* TgtDbgPtrace is a wrapper for RealPtrace() doing traces */
int 	TgtDbgPtrace       (int req, PID pid, char *addr, int data, void *addr2);
#endif


/* Information stored in "handle" */
#define BKPT_INACTIVE	1	/* bkpt inactive for this execution */
#define BKPT_ACTIVE	0	/* bkpt active for this execution */

int 	BreakOverwrite 	(const PID_LIST* plst,const char* addr,
			 unsigned int size);
int 	BreakSet 	(PID_LIST*, int conn_idx, xdr_break*);
int 	BreakSetAt 	(PID_LIST*, int conn_idx, unsigned long addr,break_type);
int 	BreakClear 	(PID_LIST*, int conn_idx, int handle);
int 	BreakGetIndex	(PID_LIST*, void* addr);
int 	BreakGet 	(const PID_LIST*, int data, xdr_break*);
void 	BreakHide 	(const PID_LIST*, void*, int, void*);
int 	BreakStepOff 	(const PID_LIST*, void** paddr2);
void 	BreakSteppedOff (PID_LIST*);
int 	BreakRespawn 	(PID_LIST*);
int 	BreakIdentify 	(PID_LIST*, int adjust, int thread);
void 	BreakPcChanged 	(PID_LIST*);
int 	BreakStepRange 	(PID_LIST*, void* addr, int len);
void 	BreaksDisable 	(int pid);
void 	BreaksEnable 	(int pid);

int 	TgtBreakRestoreOrig (int pid, void* addr, void* addr2);
void 	TgtBreakCancelStep  (PID_LIST* plst);

Boolean ListAlloc	(char *buff, CONN_LIST *clst);
int 	FindPidEntry 	(int pid);

open_out*    RPCGENSRVNAME(open_connex_2_svc)    (open_in *in,
                                                 struct svc_req *rqstp);
signal_out*  RPCGENSRVNAME(send_signal_2_svc)    (signal_in *in,
                                                 struct svc_req *rqstp);
ptrace_out*  RPCGENSRVNAME(ptrace_2_svc)         (ptrace_in *in,
                                                 struct svc_req *rqstp);
wait_out*    RPCGENSRVNAME(wait_info_2_svc)      (wait_in *in,
                                                 struct svc_req *rqstp);
#endif /* !SERVRPC_H */


/*
 **********************************************************************
 *
 *  Component:	RDBG servers
 *  Module:	remdeb.x
 *
 *  Synopsis:	XDR definitions for remote debug server RPC calls.
 *		XDR definitions for RPCGEN to build remote debug server.
 *
 * $Id$
 *
 **********************************************************************
 */

#ifdef RPC_SVC
%/*HEADER_START*/
#endif

%#define RTEMS_PORT 2071
%#define RTEMS_BACK_PORT 2073

#ifdef   RPC_HDR
%#ifndef REMDEB_H
%#define RPCGENSRVNAME(a)     a
#endif

enum rpc_type {
	SUNRPC 		= 0,
	BADRPCTYPE	= 25
};


const NET_SAFE = 1400;	/* this is safe for UDP messages */

struct UDP_MSG
{				/* format of UDP messages (should be in .h) */
  unsigned char  type;		/* type of message (BMSG_xx) */
  unsigned char	 msg_num;	/* ringed number for resend detect */
  unsigned short spec;		/* specific information for type */
  long		 pid;		/* process this affects */
  unsigned long  context;	/* specific information to request */
};

  /* First we support the overhead structures and types needed for RPC 
	requests. Then, we have all RPC routines input/output args.	*/

%/*
% * Sun request values for the remote ptrace system call
% */
%
enum ptracereq
{				/* these match PTRACE_xxx numbers */
  RPT_TRACEME = 0,		/* 0, by tracee to begin tracing */
  RPT_CHILDDONE = 0,		/* 0, tracee is done with his half */
  RPT_PEEKTEXT,			/* 1, read word from text segment */
  RPT_PEEKDATA,			/* 2, read word from data segment */
  RPT_PEEKUSER,			/* 3, read word from user struct */
  RPT_POKETEXT,			/* 4, write word into text segment */
  RPT_POKEDATA,			/* 5, write word into data segment */
  RPT_POKEUSER,			/* 6, write word into user struct */
  RPT_CONT,			/* 7, continue process */
  RPT_KILL,			/* 8, terminate process */
  RPT_SINGLESTEP,		/* 9, single step process */
  RPT_ATTACH,			/* 10, attach to an existing process (returns 2 if not primary)*/
  RPT_DETACH,			/* 11, detach from a process */
  RPT_GETREGS,			/* 12, get all registers */
  RPT_SETREGS,			/* 13, set all registers */
  RPT_GETFPREGS,		/* 14, get all floating point regs */
  RPT_SETFPREGS,		/* 15, set all floating point regs */
  RPT_READDATA,			/* 16, read data segment */
  RPT_WRITEDATA,		/* 17, write data segment */
  RPT_READTEXT,			/* 18, read text segment */
  RPT_WRITETEXT,		/* 19, write text segment */
  RPT_GETFPAREGS,		/* 20, get all fpa regs */
  RPT_SETFPAREGS,		/* 21, set all fpa regs */
  RPT_22,			/* 22, filler */
  RPT_23,			/* 23, filler */
  RPT_SYSCALL,			/* 24, trap next sys call */
  RPT_DUMPCORE,			/* 25, dump process core */
  RPT_26,			/* 26, filler */
  RPT_27,			/* 27, filler */
  RPT_28,			/* 28, filler */
  RPT_GETUCODE,			/* 29, get u.u_code */
  /* Begin  specific ptrace options */
  RPT_GETTARGETTHREAD = 50,	/* get PM target thread identifier */
  RPT_SETTARGETTHREAD = 51,	/* set PM target thread identifier */
  RPT_THREADSUSPEND   = 52,	/* suspend a thread */
  RPT_THREADRESUME    = 53,	/* resume a thread */
  RPT_THREADLIST      = 54,	/* get list of process's threads */
  RPT_GETTHREADNAME   = 55,	/* get the name of the thread */
  RPT_SETTHREADNAME   = 56,	/* set the name of the thread */
  RPT_SETTHREADREGS   = 57,	/* set all registers for a specific thread*/
  RPT_GETTHREADREGS   = 58,	/* get all registers for a specific thread*/
  /* Begin extended ptrace options for remote debug server */
  RPT_STEPRANGE	      = 75,	/* step while in range (addr=start, data=len) */
  RPT_CONTTO	      = 76,	/* cont from PC to temp break in addr */
  RPT_SETBREAK	      = 77,	/* set a breakpoint (addr=break) */
  RPT_CLRBREAK        = 78,	/* clear a breakpoint (data=handle or 0 for all) */
  RPT_GETBREAK        = 79,	/* get breakpoint (data=handle, addr=buffer to
				   fill). Returns next break. If data=0,
				   returns number of breaks. */			
  RPT_GETNAME	      = 80,	/* get name of process (data 0=name, 1=path
				   as started, 2=fullpath). Return in addr
				   as mem) */
  RPT_STOP            = 81,	/* (C-actors) Stop the C-actor */
  RPT_PGETREGS	      = 82,	/* portable version */
  RPT_PSETREGS	      = 83,	/* portable version */
  RPT_PSETTHREADREGS  = 84,	/* portable version */
  RPT_PGETTHREADREGS  = 85	/* portable version */
};

#include FRONTEND

const MAXDEBUGGEE= 150;
const NAMEMAX = 17;   

%  /*
%   * Memory data for read/write text or data. The size is in data. The target
%   * addr is in the addr field.
%   * Be careful before modifying because this value goes into internal
%   * pipes and is allocated on stack too. Pipes and/or the stack could
%   * become too small if this value gets incremented.
%   */

const MEM_DATA_MAX	= 256;

#ifndef RPC_XDR

struct xdr_mem {
	u_long		addr;
	u_int		dataNb;
	unsigned char	data[MEM_DATA_MAX];
};

#else
/* manually define best XDR function for this */
%bool_t xdr_xdr_mem(xdrs, objp)
%	XDR *xdrs;
%	struct xdr_mem *objp;
%{
%	if (!xdr_u_long(xdrs, &objp->addr)) {
%		return (FALSE);
%	}
%	if (!xdr_u_int(xdrs, &objp->dataNb)) {
%		return(FALSE);
%	}
%	return (xdr_opaque(xdrs, objp->data, objp->dataNb));
%}

#endif

/* Breakpoint structure maps to same structure on host. Do not change one
   without changing the other. */

enum break_type
{				/* types of breakpoints */
  BRKT_NONE,			/* unused entry */
  BRKT_INSTR,			/* general instruction break */
  BRKT_READ,			/* read break */
  BRKT_WRITE,			/* write breakpoint */
  BRKT_ACCESS,			/* read-or-write break */
  BRKT_EXEC,			/* execution HW breakpoint */
  BRKT_OS_CALL,			/* break on OS call, addr is call number */
  BRKT_OS_SWITCH,		/* dispatch breakpoint */
  BRKT_STEPEMUL			/* emulate hardware single-step */
};
const MAX_THRD_BRK = 4;		/* enough for 128 threads per process */
struct xdr_break
{				/* one per process local breakpoint */
  u_char	type; 		/* BRKT_xxx type of break */
  u_char	thread_spec;	/* 0=all, else count of threads it affects */
  u_short	handle;		/* handle of breakpoint returned */
  u_long	ee_loc;		/* address of start */
  u_long	ee_type;	/* type/method of address */
  u_short	length;		/* length of break if range, else 0 */
  u_char	pass_count;	/* pass count to initialize to (0=none) */
  u_char	curr_pass;	/* pass count current value */
  u_long	thread_list[MAX_THRD_BRK]; /* bit map for thread list */
};				/* 20 bytes+4 per thread_list (4x4=16) = 36 */

const UTHREAD_MAX     = 64;

const THREADNAMEMAX =	16;
typedef string thread_name <THREADNAMEMAX>;

struct KernThread {
	unsigned int threadLi;
};

#ifndef RPC_XDR

#ifdef RPC_HDR
%typedef KernThread *ptThreadList;
#endif

struct thread_list {
	unsigned int nbThread;
	ptThreadList threads;	
};

#else /* RPC_XDR */

/* must write this function by hand */

%bool_t xdr_thread_list(xdrs, objp)
%	XDR *xdrs;
%	struct thread_list *objp;
%{
%	return (xdr_array(xdrs, (char**)&objp->threads, &objp->nbThread,
%			UTHREAD_MAX, sizeof(KernThread), xdr_KernThread));
%}

#endif /* not RPC_XDR */


union ptrace_addr_data_in switch (ptracereq req) {
    /*
     * due to rpcgen poor features, we cannot put RPC_SETREGS 
     * AND RPC_SETTHREADREGS in the case list. So we use a hack (FIX rpcgen).
     */
#ifndef RPC_HDR
	case RPT_SETTHREADREGS : 
				xdr_regs regs;
#endif
	case RPT_SETREGS:

			  	xdr_regs regs;

#ifndef RPC_HDR
	case RPT_PSETTHREADREGS:
				u_int pregs<>;
#endif
	case RPT_PSETREGS:
				u_int pregs<>;

#ifdef LATER
	case RPT_SETFPREGS:
			  	xdr_fp_status fpregs;
#endif
	case RPT_SETTHREADNAME:
				thread_name name;
#ifndef RPC_HDR
	case RPT_WRITETEXT:
				xdr_mem	mem;
#endif
        case RPT_WRITEDATA:
				xdr_mem	mem;
	case RPT_SETBREAK:
				xdr_break breakp;
	default:
				u_int address;
};

union ptrace_addr_data_out switch (ptracereq req) {
	case RPT_GETREGS:
			  	xdr_regs regs;
#ifndef RPC_HDR
	case RPT_GETTHREADREGS:
			  	xdr_regs regs;
#endif

	case RPT_PGETREGS:
				u_int pregs<>;

#ifndef RPC_HDR
	case RPT_PGETTHREADREGS:
				u_int pregs<>;
#endif

#ifdef LATER
	case RPT_GETFPREGS:
			  	xdr_fp_status fpregs;
#endif
	case RPT_THREADLIST:
				thread_list threads;
	case RPT_GETTHREADNAME:
				thread_name name;				
#ifndef RPC_HDR
	case RPT_READTEXT:
				xdr_mem	mem;
	case RPT_GETNAME:
				xdr_mem mem;
#endif
        case RPT_READDATA:
				xdr_mem	mem;
	case RPT_GETBREAK:
				xdr_break breakp;
	default:
				u_int addr;
};

typedef opaque CHAR_DATA <NET_SAFE>;	/* variable sized data */

typedef string	one_arg <NET_SAFE>;	

%  /* now open_connex() routine which establishes a connection to server */

enum debug_type
{				/* type of connection requested */
  DEBTYP_PROCESS = 0,		/* process connection */
  DEBTYP_C_ACTOR = 1,		/* C-Actor connection */
  DEBTYP_KERNEL = 2,		/* kernel debug connection */
  DEBTYP_OTHER = 3		/* other subsystem */
};

%#define DEBUGGER_IS_GDB 0x2    /* */

struct open_in
{				/* input args to open a connection */
  u_char	back_port[16];	/* opaque NET address format */
  u_short	debug_type;	/* type of process DEBTYP_xxx */
  u_short	flags;		/* connection information OPNFLG_xxx */
  u_char	destination[16];/* opaque address if to router */
  one_arg	user_name;	/* name of user on host */
};

struct open_out
{				/* return from open_connex */
  u_long	port;		/* connection number to server or -1 if error */
  u_int		pad[4];		/* Planned to be KnIpcDest. Never used */
  u_int		fp;		/* True if floating point processor. If error, 
				   set to errno for open error.		*/
  u_char	cmd_table_num;	/* command table used */
  u_char	cmd_table_vers;	/* version of command table */
  u_short	server_vers;	/* version number of server itself */
};

%  /* now close_connex() routine which detaches from server */

enum close_control
{				/* choice of how to handle owned processes */
  CLOSE_IGNORE = 0,		/* ignore all controlled pids on close */
  CLOSE_KILL = 1,		/* kill all controlled pids on close */
  CLOSE_DETACH = 2		/* detach free running all controlled pids */
};

struct close_in
{				/* arg to close connection */
  close_control	control;	/* shutdown of owned processes control */
};

%  /* now send_signal() routine which sends signals to processes like kill(2) */

struct signal_in 
{				/* input to send_signal */
  int		pid;		/* process/actor to send signal to */
  int		sig;		/* signal to send (from /usr/include/signal.h) */
};

struct signal_out
{				/* return from send_signal */
  int		kill_return;	/* return code from kill(2) call */
  int		errNo;		/* error code if failed */
};


%  /* now wait_info() routine which returns results of polling the wait status
%	of a process/actor. It may return 0 if running, else pid or -1 */

enum stop_code 
{				/* stop code information */
  STOP_ERROR = 0,		/* error, errno set */
  STOP_NONE = 1,		/* not stopped */
  STOP_UNKNOWN = 2,		/* unknown stop reason */
  STOP_BREAK = 3,		/* stopped on breakpoint */
  STOP_STEP = 4,		/* stopped on step */
  STOP_SIGNAL = 5,		/* stopped on signal receieve */
  STOP_TERM_EXIT = 6,		/* terminated normally */
  STOP_TERM_SIG = 7,		/* terminated by signal */
  STOP_DETACHED = 8,		/* detached from server */
  STOP_KILLED = 9,		/* killed by ptrace KILL */
  STOP_SPAWN_FAILED = 10	/* spawn failed in exec part, handle=errno */
};

struct wait_in
{				/* input arg to wait is process */
  int		pid;		/* process/actor id */
};

struct wait_out
{				/* result of wait_info call */
  int		wait_return;	/* -1=error,0=running,pid=stopped */
  int		errNo;		/* error code if error */
  int		status;		/* wait(2) status if stopped */
  stop_code	reason;		/* reason in more abstracted terms */
  int		handle;		/* handle of break if stopped on break,
				   or signal number or exit code  */
  u_long	PC;		/* program counter if stopped */
  u_long	SP;		/* stack pointer if stopped */
  u_long	FP;		/* frame pointer if stopped */
  u_long	thread;		/* thread that stopped if applies (else -1) */
};

%  /* now ptrace() routine. This matches the Sun UNIX ptrace as well as
%	some additions */ 

const PTRFLG_FORCE = 1;		/* when set and process running, forces process
				   to stop, make the request, then start again.
				   This is used for breakpoints and the like */
const PTRFLG_NON_OWNER = 2;	/* do request even if not primary owner (will
				   notify all owners including caller if owns) */
const PTRFLG_FREE = 4;		/* free pid_list after KILL/DETACH */

const PTRDET_UNOWN = 0x100;	/* data value in RPT_DETACH just disconnects
				   caller as an owner of process.	*/

struct ptrace_in
{				/* input args matches ptrace but for XDR */
  int		pid;		/* process to act on */
  ptrace_addr_data_in addr;	/* mappings for addr and addr2 */
  u_int 	data;		/* simple data arg of ptrace */
  u_int		flags;		/* mask of PTRFLG_xxx flags. */
};

struct ptrace_out
{				/* return information from ptrace */
  ptrace_addr_data_out  addr;	/* return through addr/addr2 */
  int 			result;	/* result of ptrace call (return value) */
  int 			errNo;	/* error code if error */
};

    /* Data for GET_GLOBAL_SYMBOLS */
struct one_symbol {		/* Must match common/src/lib/ctx/ctx.h */
    string symbolName<>;
    long symbolValue;
};

typedef one_symbol all_symbols<>;

struct get_global_symbols_out {
    all_symbols	symbols;
};

    /* Data for GET_TEXT_DATA */
struct get_text_data_in {
  int		pid;		/* process/actor id if non-zero */
  string	actorName<16>;	/* actor name for system mode */
};

struct get_text_data_out {
    int		result;
    int		errNo;
    u_long	textStart;
    u_long	textSize;
    u_long	dataStart;
    u_long	dataSize;
};

    /* Data for GET_SIGNAL_NAMES */
struct one_signal {
    u_int	number;
    string	name<>;
};

typedef one_signal all_signals<>;

struct get_signal_names_out {
    all_signals signals;
};

%  /* now define the actual calls we support */

program REMOTEDEB {
	version REMOTEVERS {

	        /* open a connection to server or router */
		open_out
		OPEN_CONNEX(open_in)		= 1;

		/* send a signal to a process */
		signal_out
		SEND_SIGNAL(signal_in) 		= 2;

	/* all routines below require a connection first */

		/* close the connection to the server */
		void
		CLOSE_CONNEX(close_in)		= 10;

		/* process ptrace request */
		ptrace_out
		PTRACE(ptrace_in) 		= 11;
		
		/* poll for status of process */
		wait_out
		WAIT_INFO(wait_in)		= 13;

		get_signal_names_out
		GET_SIGNAL_NAMES(void)		= 17;

	} = 2;			/* now version 2 */
} = 0x20000fff;

#ifdef RPC_HDR
%#define REMDEB_H
%#endif
#endif

#ifdef RPC_SVC

%const char* names [] = {
%    "NULLPROC", "OPEN_CONNEX", "SEND_SIGNAL", "name3",
%    "name4", "name5", "name6", "name7",
%    "name8", "name9", "CLOSE_CONNEX", "PTRACE",
%    "name12", "WAIT_INFO", "name14", "name15",
%    "name16", "GET_SIGNAL_NAMES", "name18"
%};
%

%/*HEADER_END*/
#endif

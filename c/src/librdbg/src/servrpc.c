/*
 **********************************************************************
 *
 *  Component:	RDBG
 *  Module:	servrpc.c
 *
 *  Synopsis:	support routines for RPC dispatch for remote debug server.
 *		Main server dispatch routines from RPC to support remote debug.
 *
 * $Id$
 *
 **********************************************************************
 */

#include <string.h>
#include <sys/errno.h>
#include <rdbg/rdbg.h>
#include <rdbg/remdeb.h>
#include <rdbg/servrpc.h>

/************************************************************************/

/* -----------------------------------------------------------------------
   open_connex_2_svc - setup a new connection from a client.

   Notes:
      - this function creates a new connection to a client. It allocates
	an entry in the connection structure and fills in the information
	sent and implied by the message.
      - a client connection entry is needed for all further messages to work
	properly.
   ----------------------------------------------------------------------- */

  open_out *
RPCGENSRVNAME (open_connex_2_svc) (open_in * in,
                                   struct svc_req * rqstp)
{
  static open_out out;          /* output response. This could be heap local */
  int idx;
  static int one_time = 0;      /* we do one-time setup on back port */

  /*
   * need to support in->debug_type, in->flags, and in->destination!!! 
   */

  if (!one_time) {              /* only setup one backport socket */
    /*
     * now setup signals and the like for handling process changes 
     */
    setErrno (0);
    TspInit (rqstp->rq_xprt->xp_sock);  /* init transport system */
    if (getErrno ()) {          /* failed in setup */
      out.port = (u_long) - 1;
      out.fp = getErrno ();     /* error causing to fail */
      return (&out);            /* fail */
    }
    one_time = True;            /* disable doing this again */
  }

  DPRINTF (("open_connex_2_svc: Opening connection from '%s'\n",
            in->user_name));

  /*
   * now setup a validation of all other connections 
   */
  for (idx = 0; idx < conn_list_cnt; idx++)
    if (conn_list[idx].in_use) {    /* setup retry timer */
      DPRINTF (("open_connex_2_svc: Still have connection %d with port %d\n",
                idx, HL_W (*((UINT16 *) & conn_list[idx].back_port.c[2]))));
    }

  idx = ConnCreate (rqstp, in); /* setup the connection */
  out.port = idx;               /* connection number */
  if (idx == -1)
    out.fp = getErrno ();       /* error causing to fail */
  else
    out.fp = TARGET_PROC_TYPE;

  out.server_vers = SERVER_VERS;
  return (&out);
}

/* -----------------------------------------------------------------------
   send_signal_2_svc - send a kill/signal to the specified process.

   Notes:
      - this function sends a signal to the process specified. This process
	does not have to be under debug nor attached by this server. The kill
	may be refused on other grounds though.
      - kill(pid, 0) can be used to validate the process still exists if
	needed.
   ----------------------------------------------------------------------- */

  signal_out *
RPCGENSRVNAME (send_signal_2_svc) (signal_in * in,
                                   struct svc_req * rqstp)
{
  static signal_out out;        /* return code from kill */

  /*
   * we do not care if connected 
   */
  setErrno (0);
  out.kill_return = 0;
  out.errNo = 0;
  TotalReboot = 1;
  return (&out);
}

/* -----------------------------------------------------------------------
   close_connex_2_svc - close a connection from a client.
   ----------------------------------------------------------------------- */

  void *
RPCGENSRVNAME (close_connex_2_svc) (close_in * in,
                                    struct svc_req * rqstp)
{
  int conn_idx = TspConnGetIndex (rqstp);

  if (conn_idx != -1)           /* found it, clear out */
    ConnDelete (conn_idx, rqstp, in->control);

  return (void *) "";           /* need to return something */
}

/* -----------------------------------------------------------------------
   ptrace_2_svc - control process under debug. 
   ----------------------------------------------------------------------- */

#define REG_COUNT \
    (sizeof (xdr_regs) / sizeof (int))

  ptrace_out *
RPCGENSRVNAME (ptrace_2_svc) (ptrace_in * in,
                              struct svc_req * rqstp)
{
  int conn_idx = rqstp ? TspConnGetIndex (rqstp) : -1;
  static ptrace_out out;        /* outut response (error or data) */
  void *addr, *addr2;           /* used for actual ptrace call */
  unsigned int data;
  int req, pid, ret, pid_idx, idx;
  static union {                /* local buffer for returned data */
    Objects_Id t_list[UTHREAD_MAX]; /* thread_list return */
    char t_name[THREADNAMEMAX]; /* thread name return */
  } local_buff;                 /* for return handling of strings and the like */
  PID_LIST *plst = NULL;        /* current pid_list entry */

  DPRINTF (("ptrace_2_svc: entered (%s (%d), %d, XXXX, %d, XXXX)\n",
            PtraceName (in->addr.req), in->addr.req, in->pid, in->data));

  out.addr.ptrace_addr_data_out_u.addr = 0;

  /*
   * validate the connection 
   */
  if (conn_idx == -1 && rqstp != NULL) {    /* no connection, error */
    DPRINTF (("ptrace_2_svc: msg from unknown debugger!\n"));
    out.result = -1;
    out.errNo = ECHILD;         /* closest error */
    out.addr.req = 0;           /* to avoid copies that should not occur */
    return (&out);
  }
  /*
   * Consider that the last back-message is acknowledged 
   */
  if (conn_idx >= 0 && conn_list[conn_idx].retry) {
    TspMessageReceive (conn_idx, in->pid);
  }

  req = in->addr.req;
  out.addr.req = req;           /* needed for RPC */
  pid = in->pid;
  addr = addr2 = NULL;
  data = in->data;
  setErrno (0);                 /* assume works */
  out.result = 0;               /* assume worked ok */
  out.errNo = 0;

  /*
   * lookup process to make sure we have under control 
   */
  pid_idx = FindPidEntry (in->pid);
  if (pid_idx >= 0) {           /* found it */
    plst = &pid_list[pid_idx];
    if (conn_idx < 0)
      conn_idx = plst->primary_conn;
  }

  /*
   * now we handle the special case of ATTACH to a pid we already control 
   */
  if (req == RPT_ATTACH) {      /* look it up first */
    if (plst) {                 /* we have controlled , so return ok+show conn */
      ret = 2;                  /* normally secondary connection */
      if (!PIDMAP_TEST (conn_idx, pid_idx)) {   /* mark as an owner if not already */
        plst->owners++;
        PIDMAP_SET (conn_idx, pid_idx); /* mask in */
      } else if (plst->primary_conn != NO_PRIMARY) {    /* regrab makes primary */
        /*
         * Only if not primary already 
         */
        if (plst->primary_conn != conn_idx) {
          TspSendWaitChange (plst->primary_conn, BMSG_NOT_PRIM, conn_idx, plst->pid, 0, False); /* tell old owner */
        }
        plst->primary_conn = NO_PRIMARY;
      }

      if (plst->primary_conn == NO_PRIMARY) {   /* none now, so take over */
        plst->primary_conn = conn_idx;  /* new primary */
        ret = 1;                /* primary */
      }
      out.result = ret;         /* primary or secondary owner */
      return (&out);
    }
    /*
     * else attach process using target code 
     */
    setErrno (ESRCH);           /* assume the worst */
    if (!TgtAttach (conn_idx, pid)) {   /* failed */
      out.errNo = getErrno ();
      out.result = 0;
    }
    return (&out);
  } else if (req == RPT_DETACH) {   /* see which kind of detach */
    if (data == PTRDET_UNOWN) { /* only want to disconnect from */
      TgtDetachCon (conn_idx, pid_idx, True);   /* remove from control */
      return (&out);            /* done */
    }
  } else if (plst && (req == RPT_GETNAME || req == RPT_GETBREAK)) {
    /*
     * do nothing 
     */
  }

  else if (plst && req == RPT_CLRBREAK) {
    /*
     * To be able to remove breakpoints from a "running" system 
     */
    DPRINTF (("ptrace_2_svc: allowing RPT_CLRBREAK %d\n", data));
    /*
     * do nothing 
     */
  }

  else if (plst && plst->running) { /* error, process is running and not detach */
    out.result = -1;
    out.errNo = ETXTBSY;        /* closest error */
    DPRINTF (("ptrace_2_svc: failed, still running.\n"));
    return (&out);
  }
  if (plst == NULL) {
    out.result = -1;
    out.errNo = ESRCH;
    DPRINTF (("ptrace_2_svc: No such process.\n"));
    return (&out);
  }

  /*
   * now make sure secondary owner is not trying to modify 
   */
  if (!(in->flags & PTRFLG_NON_OWNER))  /* if not overriden */
    if (conn_idx != plst->primary_conn
        && ((req >= RPT_POKETEXT && req <= RPT_SINGLESTEP)
            || (req >= RPT_SETREGS && req <= RPT_SETFPAREGS && (req & 1))
            || (req >= RPT_SYSCALL && req <= RPT_DUMPCORE)
            || (req >= RPT_SETTARGETTHREAD && req <= RPT_THREADRESUME)
            || (req >= RPT_SETTHREADNAME && req <= RPT_SETTHREADREGS)
            || (req >= RPT_STEPRANGE && req <= RPT_CLRBREAK)
            || (req == RPT_STOP)
            || (req >= RPT_PSETREGS && req <= RPT_PSETTHREADREGS))) {   /* not owner */
      out.result = -1;
      out.errNo = EPERM;        /* cannot alter as not primary */
      DPRINTF (("ptrace_2_svc: refused, not owner, flags %d conn_idx %d primary_conn %d\n", in->flags, conn_idx, plst->primary_conn));
      return (&out);
    }

  addr = (void *) in->addr.ptrace_addr_data_in_u.address;   /* default */
  /*
   * now setup normal ptrace request by unpacking. May execute here. 
   */
  switch (req) {                /* handle unpacking or setup for real call */
    /*
     * first the ones where addr points to input data 
     */
  case RPT_SETREGS:
  case RPT_SETTHREADREGS:
    addr = (void *) &in->addr.ptrace_addr_data_in_u.regs;   /* reg list */
    break;

  case RPT_PSETREGS:
  case RPT_PSETTHREADREGS:
    if (in->addr.ptrace_addr_data_in_u.pregs.pregs_len != REG_COUNT) {
      DPRINTF (("ptrace_2_svc: pid %d got %d expected %d\n", pid,
                in->addr.ptrace_addr_data_in_u.pregs.pregs_len, REG_COUNT));
      setErrno (EINVAL);
      break;
    }
    req = req == RPT_PSETREGS ? RPT_SETREGS : RPT_SETTHREADREGS;
    addr = (void *) in->addr.ptrace_addr_data_in_u.pregs.pregs_val;
    break;

  case RPT_SETTHREADNAME:
    addr = (void *) in->addr.ptrace_addr_data_in_u.name;
    break;
  case RPT_WRITETEXT:
  case RPT_WRITEDATA:
    if ((int) data < 0) {
      setErrno (EINVAL);
      break;
    }
    addr = (void *) in->addr.ptrace_addr_data_in_u.mem.addr;    /* targ addr */
    addr2 = (void *) in->addr.ptrace_addr_data_in_u.mem.data;   /* buff */

    /*
     * Forbid writing over breakpoints 
     */
    if (BreakOverwrite (plst, addr, data)) {
      setErrno (EBUSY);
    }
    break;

  case RPT_POKETEXT:
  case RPT_POKEDATA:
    /*
     * Forbid writing over breakpoints 
     */
    if (BreakOverwrite (plst, addr, sizeof (int))) {
      setErrno (EBUSY);
    }
    break;

    /*
     * now ones where we handle locally 
     */
  case RPT_GETTARGETTHREAD:
    out.result = plst->thread;
    req = 0;                    /* force exit */
    break;

  case RPT_PGETREGS:           /* return from our buffer */
    out.addr.ptrace_addr_data_out_u.pregs.pregs_len = REG_COUNT;
    out.addr.ptrace_addr_data_out_u.pregs.pregs_val = (u_int *) & plst->regs;
    req = 0;                    /* force exit */
    break;

  case RPT_GETREGS:
    /*
     * return directly from our buffer 
     */
    /*
     * this buffer is refreshed when changing target thread 
     */
    out.addr.ptrace_addr_data_out_u.regs = plst->regs;
    req = 0;                    /* force exit */
    break;

  case RPT_SETBREAK:
    idx = BreakSet (plst, conn_idx, &in->addr.ptrace_addr_data_in_u.breakp);
    if (idx < 0)
      break;
    req = 0;                    /* force exit */
    out.result = idx;           /* return break index (>0) */
    break;

  case RPT_CLRBREAK:
    if (conn_list[conn_idx].flags & DEBUGGER_IS_GDB) {
      data = BreakGetIndex (plst, addr);
    }
    out.result = BreakClear (plst, conn_idx, data);
    /*
     * if errored, errno will still be set 
     */
    req = 0;
    break;

  case RPT_GETBREAK:
    /*
     * data=handle, addr=in_buffer, returns next break. Data=0, returns cnt 
     */
    out.result = BreakGet (plst, data, &out.addr.
                           ptrace_addr_data_out_u.breakp);
    req = 0;                    /* handle locally */
    break;

  case RPT_GETNAME:            /* get the name of the process */
    if (!plst->name)
      out.addr.ptrace_addr_data_out_u.mem.dataNb = 0;
    else {
      int maxLen = sizeof(out.addr.ptrace_addr_data_out_u.mem.data) - 1;
      data = strlen (plst->name);
      if (data > maxLen)
        data = maxLen;
      out.addr.ptrace_addr_data_out_u.mem.dataNb = data + 1;
      memcpy (out.addr.ptrace_addr_data_out_u.mem.data, plst->name, data + 1);
      out.addr.ptrace_addr_data_out_u.mem.data[maxLen] = '\0';
    }
    req = 0;
    break;

  case RPT_CONTTO:
    if (BreakSetAt (plst, conn_idx, (u_long) addr, BRKT_STEPEMUL) < 0) {
      DPRINTF (("ptrace_2_svc: BreakSet failed at %x", addr));
      break;
    }
    req = RPT_CONT;
    /*
     * data can contain a signal number, addr2 is unused 
     */
    goto case_RPT_CONT;

  case RPT_STEPRANGE:
    /*
     * convert to step 
     */
    if (!data)
      data = 1;                 /* should we give an error?? */
    BreakStepRange (plst, addr, data);
    if (getErrno ())
      break;

    req = RPT_SINGLESTEP;       /* do by stepping */
    addr = (void *) 1;          /* start from current PC */
    data = -2;                  /* want non-atomic stepping */
    /*
     * fall through to other exec cases 
     */

  case RPT_CONT:
  case_RPT_CONT:
  case RPT_SINGLESTEP:

    if (BreakStepOff (plst, &addr2)) {  /* need clear then step off break */
      /*
       * clear break, step, then do exec 
       */
      if (addr == (void *) 1)
        addr = (void *) plst->regs.REG_PC;  /* need for patch */

      /*
       * data is always 0, so atomic single-step 
       */
    } else if (req == RPT_SINGLESTEP) {
      data = -2;                /* want non-atomic stepping */
    }
    break;

    /*
     * now ones where addr points to an output area 
     */
  case RPT_PGETTHREADREGS:
    addr = (void *) out.addr.ptrace_addr_data_out_u.mem.data;
    if (sizeof(out.addr.ptrace_addr_data_out_u.mem.data) < REG_COUNT * sizeof (int)) {
      setErrno (EINVAL);
      break;
    }
    if (data == plst->thread) {
      out.addr.ptrace_addr_data_out_u.pregs.pregs_len = REG_COUNT;
      out.addr.ptrace_addr_data_out_u.pregs.pregs_val =
        (u_int *) & plst->regs;
      req = 0;                  /* force exit */
      break;
    }
    req = RPT_GETTHREADREGS;
    break;

  case RPT_GETTHREADREGS:
    addr = (void *) &out.addr.ptrace_addr_data_out_u.regs;
    break;
  case RPT_GETTHREADNAME:
    out.addr.ptrace_addr_data_out_u.name = local_buff.t_name;
    addr = (void *) out.addr.ptrace_addr_data_out_u.name;
    break;
  case RPT_THREADLIST:
    out.addr.ptrace_addr_data_out_u.threads.threads =
      (ptThreadList) local_buff.t_list;
    addr = (void *) out.addr.ptrace_addr_data_out_u.threads.threads;
    break;
  case RPT_READTEXT:
  case RPT_READDATA:
    if ((int) data < 0) {
      setErrno (EINVAL);
      break;
    }
    addr = (void *) in->addr.ptrace_addr_data_in_u.address;
    addr2 = (void *) out.addr.ptrace_addr_data_out_u.mem.data;
    out.addr.ptrace_addr_data_out_u.mem.dataNb = data;
    break;
  case RPT_DETACH:
    /*
     * Do not allow detaching if breakpoints still there 
     */
    if (BreakGet (plst, 0, NULL)) { /* some bkpts still set */
      setErrno (EINVAL);        /* cannot detach safely */
      break;
    }
    /*
     * fall through 
     */
  case RPT_KILL:
    /*
     * in the event they are trying to detach or kill a terminated process,
     * we just delete the entry.                  
     */
    if (PROC_TERMINATED (plst)) {
      TgtDelete (plst, -1, BMSG_KILLED);    /* just blow off */
      req = 0;                  /* now exit */
    }
    break;
  }

  if (getErrno ()) {            /* failed in code above */
    out.result = -1;
    out.errNo = getErrno ();
    DPRINTF (("ptrace_2_svc: result %d errNo %d\n", out.result, out.errNo));
    return (&out);
  } else if (!req) {            /* bail out now */
    DPRINTF (("ptrace_2_svc: result %d errNo %d\n", out.result, out.errNo));
    return (&out);
  }

  /*
   * OK, make the call 
   */
  out.result = TgtPtrace (req, pid, addr, data, addr2);
  out.errNo = getErrno ();

  /*
   * if no error, cleanup afterwards 
   */
  if (getErrno ()) {
    /*
     * Remove step-emul breakpoints if any 
     */
    if (req == RPT_SINGLESTEP || req == RPT_CONT) {
      BreakClear (plst, -1, -1);
    }
    DPRINTF (("ptrace_2_svc: result %d errNo %d\n", out.result, out.errNo));
    return (&out);              /* return error */
  }

  switch (in->addr.req) {       /* handle some special calls that affect state */
  case RPT_CONT:
  case RPT_STEPRANGE:
    /*
     * change to running 
     */
    if (in->addr.req == RPT_STEPRANGE)
      plst->last_start = LAST_RANGE;    /* so range steps */
    else if (addr2)
      plst->last_start = LAST_STEPOFF;  /* now continue after wait */
    else
      plst->last_start = LAST_CONT;
    plst->running = 1;          /* mark as running */
    if (!rqstp)                 /* Called internally to restart bkpt, no msg to anybody */
      break;
    TgtNotifyAll (pid_idx, BMSG_WAIT, 0, 0, (in->flags & PTRFLG_NON_OWNER)
                  ? -1 : conn_idx, True);
    break;
  case RPT_SINGLESTEP:
    /*
     * mark as step 
     */
    plst->last_start = LAST_STEP;   /* so we know how started */
    plst->running = 1;          /* mark as running (wait should catch fast) */
    break;
  case RPT_DETACH:             /* mark as disconnected */
  case RPT_KILL:               /* mark as killed */
    if (in->flags & PTRFLG_FREE)    /* notify and delete entry */
      TgtDelete (plst, -1,
                 (in->addr.req == RPT_KILL) ? BMSG_KILLED : BMSG_DETACH);
    else {                      /* notify and mark */
      plst->last_start = (in->addr.req == RPT_KILL) ?
        LAST_KILLED : LAST_DETACHED;
      plst->state = -1;
      plst->running = False;
      TgtNotifyAll (pid_idx, (in->addr.req == RPT_KILL) ?
                    BMSG_KILLED : BMSG_DETACH, 0, 0, -1, True);
    }
    break;
  case RPT_SETTHREADREGS:
  case RPT_PSETTHREADREGS:
    if (data != plst->thread)
      break;
    DPRINTF (("ptrace_2_svc: pid %d target thread regs changed!\n", pid));

  case RPT_SETREGS:
  case RPT_PSETREGS:
    /*
     * change our buffer as well 
     */
    if (plst->regs.REG_PC != ((xdr_regs *) addr)->REG_PC)
      BreakPcChanged (plst);
    plst->regs = *(xdr_regs *) addr;    /* copy in */
    break;

    /*
     * case RPT_PGETREGS has been handled locally above 
     */
  case RPT_PGETTHREADREGS:
    /*
     * We need to update pointer so that XDR works on return 
     */
    out.addr.ptrace_addr_data_out_u.pregs.pregs_len = REG_COUNT;
    out.addr.ptrace_addr_data_out_u.pregs.pregs_val =
      (void *) out.addr.ptrace_addr_data_out_u.mem.data;
    break;

  case RPT_PEEKTEXT:
  case RPT_PEEKDATA:
  case RPT_READDATA:
  case RPT_READTEXT:
    if (req < RPT_READDATA) {   /* peek */
      /*
       * addr is start 
       */
      data = sizeof (int);
      addr2 = &out.result;      /* data buffer */
      /*
       * Like read: addr is start, data is length, addr2 is buffer 
       */
    }
    BreakHide (plst, addr, data, addr2);
    break;

  case RPT_SETTARGETTHREAD:
    DPRINTF (("ptrace_2_svc: pid %d new target thread %d\n", pid, data));
    TgtPtrace (RPT_GETREGS, pid, (char *) &plst->regs, 0, NULL);
    plst->thread = data;
    if (plst->break_list) {     /* Forget we had to step off breakpoint */
      BASE_BREAK *base = (BASE_BREAK *) plst->break_list;
      DPRINTF (("ptrace_2_svc: clr_step %d last_break %d\n", base->clr_step,
                base->last_break));
      base->clr_step = 0;       /* Not stopped on break */
      base->last_break = 0;
    }
    break;

  case RPT_THREADLIST:
    out.addr.ptrace_addr_data_out_u.threads.nbThread = out.result;
    break;

  default:
    break;
  }                             /* end switch */
  DPRINTF (("ptrace_2_svc 2: result %d errNo %d\n", out.result, out.errNo));
  return (&out);
}

/* -----------------------------------------------------------------------
   wait_info_2_svc - non-blocking wait request to check status.
   ----------------------------------------------------------------------- */

  wait_out *
RPCGENSRVNAME (wait_info_2_svc) (wait_in * in, struct svc_req *rqstp)
{
  int conn_idx = TspConnGetIndex (rqstp);
  static wait_out out;          /* output of pid and status */
  int idx;
  PID_LIST *plst;

  memset (&out, 0, sizeof (out));   /* zero for safety */
  out.reason = STOP_ERROR;      /* assume the worst */

  if (conn_idx == -1) {         /* no connection, error */
    DPRINTF (("wait_info_2_svc: msg from unknown debugger!\n"));
    out.wait_return = -1;
    out.errNo = ECHILD;         /* closest error */
    return (&out);
  } else {                      /* see if confirming message received */
    if (conn_list[conn_idx].retry)
      TspMessageReceive (conn_idx, in->pid);
  }

  if (!in->pid) {               /* warm test verify only */
    /*
     * this call (pid==0) is made to confirm that that connection is still
     * active.      
     */
    /*
     * we let it fall through as an error since any use other than connection
     * reset would be an error (there is no pid0).          
     */
  } else {                      /* normal request */
    idx = FindPidEntry (in->pid);
    if (idx >= 0) {             /* found process they requested on */
      plst = &pid_list[idx];
      out.wait_return = plst->running ? 0 : in->pid;
      /*
       * return: 0 is running, pid is stopped/term 
       */
      out.errNo = 0;
      out.status = plst->state; /* last stopped reason if stopped */
      out.thread = plst->thread;    /* current thread (or -1 if none) from stop */
      if (!out.wait_return)
        out.reason = STOP_NONE; /* running, no action */
      else if (STS_SIGNALLED (out.status)) {    /* stopped on signal */
        out.handle = STS_GETSIG (out.status);   /* signal number */
        if (out.handle == SIGTRAP)
          if (plst->is_step) {  /* single step with hitting a break */
            out.reason = STOP_STEP;
            out.handle = 0;     /* no information */
          } else {              /* stopped on break */
            out.reason = STOP_BREAK;
            if (plst->break_list)
              out.handle = ((BASE_BREAK *) plst->break_list)->last_break;
            else
              out.handle = 0;   /* no break */
        } else
          out.reason = STOP_SIGNAL;
        out.PC = plst->regs.REG_PC; /* copy standard regs */
        out.SP = plst->regs.REG_SP;
        out.FP = plst->regs.REG_FP;
      } else {                  /* terminated, so lower use count */
        if (plst->last_start == LAST_KILLED)
          out.reason = STOP_KILLED;
        else if (plst->last_start == LAST_DETACHED)
          out.reason = STOP_DETACHED;
        else if (plst->last_start == LAST_START) {  /* failed in exec */
          out.reason = STOP_SPAWN_FAILED;
          out.handle = STS_GETCODE (out.status);    /* errno reason */
        } else if (STS_TERMONSIG (out.status)) {    /* terminated on signal */
          out.reason = STOP_TERM_SIG;
          /*
           * mask off the core-dumped bit 7 
           */
          out.handle = (int) (unsigned) (u_char) STS_TERMGETSIG (out.status);
        } else {                /* exit(2)ed */
          out.reason = STOP_TERM_EXIT;
          out.handle = STS_GETCODE (out.status);    /* code */
        }
      }
      DPRINTF (("wait_info_2_svc: pid %d return %d status %x errNo %d"
                " reason %d handle %d pc %x sp %x fp %x thread %d\n",
                in->pid, out.wait_return, out.status, out.errNo, out.reason,
                out.handle, out.PC, out.SP, out.FP, out.thread));
      return (&out);
    }
  }
  /*
   * if not found in list, we return error: no such process 
   */
  out.wait_return = -1;
  out.errNo = ESRCH;            /* no process */
  out.status = 0;
  return (&out);
}

/* -----------------------------------------------------------------------
   get_signal_names_2_svc - return names for signals
   ----------------------------------------------------------------------- */

static one_signal SignalNames[] = {
  {SIGILL, "SIGILL/EVT_ILL"},
  {SIGTRAP, "SIGTRAP/EVT_BKPT"},
  {SIGFPE, "SIGFPE/EVT_FPE"},
  {SIGKILL, "SIGKILL/EVT_AKILL"},
  {SIGSEGV, "SIGSEGV/EVT_SEGV"},
  {17, "SIGSTOP"},
  {23, "SIGSTOP"}
};

  get_signal_names_out *
RPCGENSRVNAME (get_signal_names_2_svc) (void * in, struct svc_req * rqstp)
{
  static get_signal_names_out out;

  out.signals.all_signals_len = sizeof (SignalNames) / sizeof (SignalNames[0]);
  out.signals.all_signals_val = SignalNames;

  return (&out);
}

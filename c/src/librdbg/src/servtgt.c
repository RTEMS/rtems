/*
 **************************************************************************
 *
 *  Component:	RDB servers
 *  Module:	servtgt.c
 *
 * $Id$
 *
 **************************************************************************
 */

#include <string.h>
#include <sys/errno.h>
#include <rdbg/rdbg.h>
#include <rdbg/servrpc.h>
#include <sys/socket.h>
#include <assert.h>

#ifdef DDEBUG
#define	Ptrace	TgtDbgPtrace
#else
#define Ptrace	TgtRealPtrace
#endif

/*
 * TgtBreakRestoreOrig - Restore original instruction at "addr"
 * just before single-stepping it.
 */

  int
TgtBreakRestoreOrig (int pid, void *addr, void *addr2)
            /*
             * Process identifier 
             */
            /*
             * Breakpoint address 
             */
            /*
             * Original instruction or bkpt number 
             */
{
  int ret;
  int l;

  l = (long) Ptrace (RPT_PEEKTEXT, pid, addr, 0, NULL); /* assume ok */
  ret = ORG_BREAK (l, (UINT32) addr2);  /* reconstruct old instr */
  ret = Ptrace (RPT_POKETEXT, pid, addr, ret, NULL);    /* poke back old */
  return ret;
}

/*
 *
 *  TgtBreakCancelStep - Restore the breakpoint at "addr" if the single-step
 *  has failed at the ptrace level.
 */

#define BKPT0(plst)     ((BASE_BREAK*)(plst)->break_list)

  void
TgtBreakCancelStep (PID_LIST * plst)
{
  assert (plst->break_list);
  assert (BKPT0 (plst)->clr_step);

  if (plst->break_list && BKPT0 (plst)->clr_step) {
    int idx = BKPT0 (plst)->last_break;
    int data;

    data = Ptrace (RPT_PEEKTEXT, plst->pid,
                   (char *) plst->break_list[idx].ee_loc, 0, NULL);
    assert (!IS_BREAK (data));
    Ptrace (RPT_POKETEXT, plst->pid,
            (char *) plst->break_list[idx].ee_loc,
            (int) SET_BREAK (data), NULL);
  }
}

/*
 * TgtCreateNew - add a new process into the process management lists.
 */

  void
TgtCreateNew (PID pid, int conn, INT32 child, char *name, Boolean spawn)
{
  int idx;

  for (idx = 0; idx < pid_list_cnt; idx++)
    if (!pid_list[idx].pid)
      break;                    /* find empty */

  if (idx >= pid_list_cnt) {    /* no empties, add more */
    PID_LIST *tmp_pid_list = pid_list;

    pid_list_cnt += PID_LIST_INC;
    pid_list = (PID_LIST *) Realloc (pid_list,  /* get new or extend */
                                     pid_list_cnt * sizeof (PID_LIST));
    if (!pid_list) {            /* out of memory */
      pid_list_cnt -= PID_LIST_INC;
      if (pid_list_cnt) {       /* realloc failed - malloc again */
        pid_list = tmp_pid_list;
        /*
         * above relies on old pointer being valid after failed realloc 
         */
      }
      return;                   /* failed */
    }
    /*
     * now clear newly added space 
     */
    memset (pid_list + pid_list_cnt - PID_LIST_INC, 0,
            PID_LIST_INC * sizeof (PID_LIST));
    idx = pid_list_cnt - PID_LIST_INC;
  } else                        /* clear entry we found */
    memset (&pid_list[idx], 0, sizeof (PID_LIST));

  /*
   * now fill in empty entry 
   */
  pid_list[idx].pid = pid;
  pid_list[idx].running = 1;    /* we have not called wait yet */
  pid_list[idx].primary_conn = (UCHAR) conn;    /* primary owner */
  if (conn != -1) {             /* found caller */
    pid_list[idx].owners = 1;
    PIDMAP_SET (conn, idx);     /* mask in */
  }
  pid_list[idx].thread = (UINT32) - 1;  /* no thread for now */
  pid_list[idx].last_start = LAST_START;    /* handle MiX bug */

  pid_list[idx].name = name ? (char *) StrDup (name) : (char *) NULL;

}

/*
 * TgtNotifyWaitChange - send event to clients indicating child changed state.
 */

  void
TgtNotifyWaitChange (PID pid, int status, Boolean exclude)
{
  int conn, idx;

  idx = FindPidEntry (pid);     /* locate the pid that changed */
  if (idx < 0) {
    DPRINTF (("TgtNotifyWaitChange: pid %d not in our list\n", (int) pid));
    return;                     /* not in our list */
  }
  pid_list[idx].running = 0;    /* not running */
  pid_list[idx].state = status; /* save status of stop/term */
  if (!pid_list[idx].owners && !STS_SIGNALLED (status))
    TgtDelete (&pid_list[idx], -1, 0);  /* terminated and no owners */
  else {                        /* normal cases */
    for (conn = 0; conn < conn_list_cnt; conn++) {  /* now find all interested clients */
      if (!conn_list[conn].in_use   /* free entry */
          || !PIDMAP_TEST (conn, idx))
        continue;               /* not using this pid */
      if (conn == exclude)
        continue;               /* do not do this one */
      TspSendWaitChange (conn, BMSG_WAIT, 1, pid, 0, False);    /* notify of change */
    }
  }
}

/*
 *  TgtNotifyAll - send a message to all clients interested in process.
 */

  void
TgtNotifyAll (int pid_idx, BACK_MSG msg, UINT16 spec,
              UINT32 context, int exclude, Boolean force)
{
  int conn;

  DPRINTF (("TgtNotifyAll: msg %d (%s) for pid_idx=%d (%d,%d)\n",
            msg, BmsgNames[msg], pid_idx, exclude, force));
  for (conn = 0; conn < conn_list_cnt; conn++)
    if (conn_list[conn].in_use  /* not free */
        && PIDMAP_TEST (conn, pid_idx)) {
      if (conn != exclude)
        TspSendWaitChange (conn, msg, spec, pid_list[pid_idx].pid, context,
                           force);
    }
}

/*
 * TgtDelete - mark process as now uncontrolled.
 *
 *  Notes:
 *     - this function removes a process from the process list.
 *     - the notify argument indicates a message to send if needed.
 */

  void
TgtDelete (PID_LIST * plst, int conn_idx, BACK_MSG notify)
{
  int idx = plst - pid_list, cnt, conn;

  /*
   * found 
   */
  cnt = pid_list[idx].owners;
  if (cnt) {                    /* some connections to break */
    for (conn = 0; cnt && conn < conn_list_cnt; conn++)
      if (conn_list[conn].in_use    /* not free */
          && PIDMAP_TEST (conn, idx)) { /* found one that uses it */
        PIDMAP_CLEAR (conn, idx);
        if (notify && conn != conn_idx)
          TspSendWaitChange (conn, notify, 0, plst->pid, 0, True);
        if (!--cnt)
          break;
      }
  }
  if (pid_list[idx].name)
    Free (pid_list[idx].name);  /* free string name back */
  /*
   * Free breakpoint list 
   */
  if (pid_list[idx].break_list != NULL) {
    Free (pid_list[idx].break_list);
  }
  pid_list[idx].pid = 0;        /* gone */
}

/*
 * TgtKillAndDelete - kill or detach process and remove entry.
 */

  int
TgtKillAndDelete (PID_LIST * plst, struct svc_req *rqstp, Boolean term)
{
  ptrace_in pin;                /* used for ptrace call */
  ptrace_out *pout;

  /*
   * Remove breakpoints 
   */
  if (plst->break_alloc > 0) {
    pin.pid = plst->pid;
    pin.addr.req = RPT_CLRBREAK;
    pin.data = 0;               /* clear all */
    pin.flags = PTRFLG_NON_OWNER;
    pout = RPCGENSRVNAME (ptrace_2_svc) (&pin, rqstp);
    if (pout->result < 0) {
      DPRINTF (("TgtKillAndDelete: RPT_CLRBREAK failed %d\n", getErrno ()));
      return -1;
    }
  }

  if (term) {                   /* kill */
    pin.addr.ptrace_addr_data_in_u.address = 0;
    pin.data = -1;              /* Don't want notification from slave */
    pin.addr.req = RPT_KILL;
  } else {                      /* detach */
    pin.addr.ptrace_addr_data_in_u.address = 1;
    pin.data = 0;
    pin.addr.req = RPT_DETACH;
  }
  pin.pid = plst->pid;
  pin.flags = PTRFLG_FREE | PTRFLG_NON_OWNER;

  DPRINTF (("TgtKillAndDelete: ptrace_2_svc (%s (%d), %d)\n",
            PtraceName (pin.addr.req), pin.addr.req, pin.pid));

  pout = RPCGENSRVNAME (ptrace_2_svc) (&pin, rqstp);    /* start it */
  if (pout->errNo == ESRCH && plst->pid)
    TgtDelete (plst, -1, BMSG_KILLED);  /* only entry remains */
  return 0;
}

/*
 * TgtDetachCon - detach a connection's ownership of a process.
 */

  void
TgtDetachCon (int conn_idx, int pid_idx, Boolean delete)
{
  if ((unsigned) pid_idx >= pid_list_cnt || !pid_list[pid_idx].pid)
    return;                     /* not valid */
  if (PIDMAP_TEST (conn_idx, pid_idx)) {    /* if an owner, release control */
    PIDMAP_CLEAR (conn_idx, pid_idx);

    if (pid_list[pid_idx].owners)
      pid_list[pid_idx].owners--;
    if (pid_list[pid_idx].primary_conn == conn_idx)
      pid_list[pid_idx].primary_conn = NO_PRIMARY;
    if (delete
        && !pid_list[pid_idx].owners && PROC_TERMINATED (pid_list + pid_idx))
      TgtDelete (&pid_list[pid_idx], -1, 0);    /* remove entry */
  }
}

/* -----------------------------------------------------------------------
   TgtHandleChildChange - decide what action to take after wait() returns.
		Used in the master only.
   ----------------------------------------------------------------------- */

#ifdef DDEBUG
static char *LastStartNames[] = {
  "NONE", "STEP", "CONT", "RANGE",
  "STEPOFF", "KILLED", "DETACHED"
};

  char *
GetLastStartName (int last_start)
{
  static char buf[32];

  strcpy (buf, LastStartNames[last_start & ~LAST_START]);
  if (last_start & LAST_START) {
    strcat (buf, "+START");
  }
  return buf;
}
#endif

  Boolean
TgtHandleChildChange (PID pid, int *status, int *unexp,
                      CPU_Exception_frame * ctx)
{                               /* return False if continue, else stop */
  int idx, sig;
  int bidx = 0;
  PID_LIST *plst;
  unsigned long PC;
  BASE_BREAK *base = NULL;      /* break_list[0] is really BASE_BREAK */
  int hadStepEmul;
  int origHadStepEmul;
  int stopWanted;

  DPRINTF (("TgtHandleChildChange: pid %d status %x cap\n",
            (int) pid, *status));
  if (unexp)
    *unexp = 0;                 /* initialize to ok */

  /*
   * first, find pid in question 
   */
  idx = FindPidEntry (pid);
  if (idx < 0) {                /* cannot locate this process */
    DPRINTF (("TgtHandleChildChange: unknown process (%s pid)\n",
              FindPidEntry (pid) >= 0 ? "stale" : "unknown"));
    if (unexp)
      *unexp = 1;               /* Unexpected change */
    return (False);             /* unknown: ignore (used to stop and notify) */
  }

  /*
   * found 
   */
  plst = &pid_list[idx];        /* pointer to entry */
  /*
   * first we see if just stopped 
   */

  /*
   * copy ctxt 
   */
  CtxToRegs (ctx, &(plst->regs));

  stopWanted = plst->stop_wanted;
  plst->stop_wanted = 0;        /* For the next time */

  hadStepEmul = BreakClear (plst, -1, -1) > 0;
  origHadStepEmul = hadStepEmul;    /* hadStepEmul is cleared if real bkpt met */

  if (STS_SIGNALLED (*status)) {    /* stopped, not terminated */
    sig = STS_GETSIG (*status); /* signal that stopped us */

    /*
     * now, we read the registers and see what to do next 
     */
    if (TgtPtrace (RPT_GETREGS, pid, (void *) &plst->regs, 0, NULL) < 0) {
      memset (&plst->regs, 0, sizeof (plst->regs));
    }

    /*
     * Get current thread 
     */
    plst->thread = TgtPtrace (RPT_GETTARGETTHREAD, pid, NULL, 0, NULL);

    if (sig == SIGTRAP) {       /* stopped from break/step */
      PC = plst->regs.REG_PC;
      /*
       * Must check PC to see whether in situations where we had
       * step emulation we are on a breakpoint or just
       * have returned from an emulated single-step 
       */
      if (BreakIdentify (plst, 0 /*no adjust */ , -1 /*no thread */ ) > 0) {
        hadStepEmul = 0;
      }
      plst->is_step = hadStepEmul || IS_STEP (plst->regs)
        || plst->last_start == LAST_START;
      DPRINTF (("TgtHandleChildChange: %s last_start %s\n", plst->is_step
                ? "step" : "break", GetLastStartName (plst->last_start)));

      if ((plst->is_step || origHadStepEmul || stopWanted)
          && (plst->last_start == LAST_STEP
              || plst->last_start == LAST_STEPOFF
              || plst->last_start == LAST_RANGE)) {
        DPRINTF (("TgtHandleChildChange: restoring stepped-off bkpt\n"));
        BreakSteppedOff (plst);
      }

      if (plst->last_start == LAST_STEPOFF && (plst->is_step || origHadStepEmul)) { /* stepped off break and now need cont */
        DPRINTF (("TgtHandleChildChange: auto-resuming after step-off\n"));
        plst->last_start = LAST_CONT;   /* convert to normal cont */
        if (!stopWanted) {
          if (TgtPtrace (RPT_CONT, pid, (char *) 1, 0, NULL))
            return True;        /* tell people */
          return (False);       /* wait for change */
        }
        DPRINTF (("TgtHandleChildChange: stop_wanted %d in step-off\n",
                  stopWanted));
        *status = STS_MAKESIG (stopWanted);
        return True;            /* Stop and notify */
      }

      base = plst->break_list ? ((BASE_BREAK *) plst->break_list) :
        ((BASE_BREAK *) NULL);
      /*
       * now see if step in range 
       */

      if (plst->last_start == LAST_RANGE    /* step in range */
          && (plst->is_step || origHadStepEmul) /* not a breakpoint */
          &&PC >= base->range_start && PC <= base->range_end) { /* still in range, keep going */
        if (stopWanted) {
          DPRINTF (("TgtHandleChildChange: stop_wanted %d in step-range\n",
                    stopWanted));
        } else {
          DPRINTF (("TgtHandleChildChange: Reservation at %x\n",
                    plst->regs.REG_PC));
        }
      }
      if (!plst->is_step) {     /* was break */
        bidx = BreakIdentify (plst, 1 /*adjust */ , plst->thread);
        if (bidx == 0) {
          DPRINTF (("TgtHandleChildChange: forwarding bkpt to kernel\n"));
          if (unexp) {
            *unexp = 1;
          }
          return False;
        }
        if (bidx < 0) {         /* Unwanted breakpoint, must step it off */
          ptrace_in pin;
          ptrace_out *out;
          if (origHadStepEmul) {
            DPRINTF (("TgtHandleChildChange: bkpt %x becomes step\n",
                      plst->regs.REG_PC));
            bidx = -bidx;
            plst->is_step = 1;
            base->clr_step = plst->break_list[bidx].type == BRKT_INSTR;
            base->last_break = bidx;
            return True;
          }
          if (stopWanted) {
            DPRINTF (("TgtHandleChildChange: stop_wanted %d at bkpt %x\n",
                      stopWanted, plst->regs.REG_PC));
            /*
             * The PC has already been adjusted by BreakIdentify 
             */
            *status = STS_MAKESIG (stopWanted);
            return True;
          }
          /*
           * All the handling is done in ptrace_2_svc() so call it 
           */
          bidx = -bidx;
          DPRINTF (("TgtHandleChildChange: last %d (%s) restarting bkpt %d\n",
                    plst->last_start, GetLastStartName (plst->last_start),
                    bidx));
          base->clr_step = 1;
          base->last_break = bidx;  /* remember which one */
          plst->running = 0;    /* So that ptrace is accepted */
          pin.pid = plst->pid;

          if (plst->last_start == LAST_STEP) {
            pin.addr.req = RPT_SINGLESTEP;
          } else {
            pin.addr.req = RPT_CONT;
          }
          pin.addr.ptrace_addr_data_in_u.address = 1;
          pin.data = 0;
          pin.flags = PTRFLG_NON_OWNER;
          out = RPCGENSRVNAME (ptrace_2_svc) (&pin, NULL);
          if (out->result == 0)
            return False;       /* Continue waiting */
          DPRINTF (("TgtHandleChildChange: failed to restart bkpt!\n"));
          /*
           * If something went wrong, just stop on breakpoint 
           */
        }
      }
    }

    /*
     * else sig != SIGTRAP 
     */
    /*
     * finally, fill in stop info in break point array base 
     */
    if (bidx > 0) {             /* store break info */
      /*
       * will need to get off the break for SW breakpoints only 
       */
      base->clr_step = plst->break_list[bidx].type == BRKT_INSTR;
      base->last_break = bidx;  /* remember which one */
    } else if (base) {          /* clear break info */
      base->clr_step = False;   /* not stopped on break */
      base->last_break = 0;
    }
    /*
     * decision to notify owner based on last_start 
     */
  } /* stopped */
  else {                        /* terminated */

    if (plst->last_start == LAST_START) {   /* spawn failed */
      TgtNotifyAll (idx, BMSG_EXEC_FAIL, 0, 0, -1, True);
      plst->running = False;    /* not running - dead */
      plst->state = *status;    /* contains errno in high word */
      return (False);
    }

    else if ((UCHAR) (plst->last_start & ~LAST_START) < (UCHAR) LAST_KILLED)
      plst->last_start = LAST_NONE; /* doesn't matter anymore */
    else
      return (False);           /* killed and detach already notified */
  }
  return (True);                /* stop and notify */
}

#ifdef DDEBUG

/*
 * TgtDbgPtrace - debug version of ptrace.
 */

  int
TgtDbgPtrace (int request, PID pid, char *addr, int data, void *addr2)
{
  int diag;

  DPRINTF (("TgtDbgPtrace: entered (%s (%d), %d, %x, %d, %x)\n",
            PtraceName (request), request, pid, (int) addr, data,
            (int) addr2));

  if (request == RPT_WRITETEXT || request == RPT_WRITEDATA) {
    int i;

    DPRINTF (("TgtDbgPtrace:"));
    if (rdb_debug) {
      for (i = 0; i < data && i < 16; ++i) {
        printf (" %02x", ((char *) addr2)[i] & 0xFF);
      }
      printf ("\n");
    }
  }

  diag = TgtRealPtrace (request, pid, addr, data, addr2);

  DPRINTF (("TgtDbgPtrace: returned %d (%x) errno %d\n",
            diag, diag, getErrno ()));

  if (request == RPT_GETREGS || request == RPT_GETTHREADREGS
      || request == RPT_SETREGS || request == RPT_SETTHREADREGS) {
    /*
     * Use DPRINTF() so as to have the id prefix 
     */
    DPRINTF (("TgtDbgPtrace: (%s) PC = %x, SP = %x, FP = %x\n",
              PtraceName (request),
              ((xdr_regs *) addr)->REG_PC,
              ((xdr_regs *) addr)->REG_SP, ((xdr_regs *) addr)->REG_FP));
  }

  return (diag);
}
#endif /* DDEBUG */

/*
 **********************************************************************
 *
 *  Component:	RDB servers
 *  Module:	servbkpt.c
 *
 *  Synopsis:	Management of breakpoints
 *
 * $Id$
 *
 **********************************************************************
 */

#include <sys/errno.h>
#include <assert.h>
#include <rdbg/rdbg.h>
#include <rdbg/servrpc.h>

/*----- Macros -----*/

#define BKPT0(plst)	((BASE_BREAK*)(plst)->break_list)
#define BKPT_INCR	5           /* how many bkpt slots alloc at a time */

#define BKPT_OVER(plst,idx,addr,size) \
    ((plst)->break_list[idx].ee_loc + BREAK_SIZE > (UINT32) (addr) \
    &&  (plst)->break_list[idx].ee_loc < (UINT32) (addr) + (size))

#define BKPT_SLOTS \
	(sizeof ((xdr_break*) 0)->thread_list / \
	 sizeof ((xdr_break*) 0)->thread_list [0])

    /*
     *  BreakAlloc - alloc a breakpoint entry.
     *
     *  This is a generic routine to insert an entry in the
     *  breakpoint list. It returns the number of entry just
     *  created. It returns -1 if failed.
     */

  static int
BreakAlloc (PID_LIST * plst, Boolean normal)
{
  int idx, len;
  xdr_break *blst;

  if (!normal) {                /* want 0 entry */
    if (plst->break_list) {
      return (0);               /* already there */
    }
    idx = 1;                    /* force alloc below */
  } else {
    for (idx = 1; idx < (int) plst->break_alloc; idx++) {
      if (plst->break_list[idx].type == BRKT_NONE) {
        /*
         * got a free one 
         */
        memset (&plst->break_list[idx], 0, sizeof (xdr_break));
        return (idx);           /* found one */
      }
    }
  }
  /*
   * idx is the requested entry 
   */

  if (idx >= (int) plst->break_alloc) { /* need more space */
    len = plst->break_alloc + BKPT_INCR;
    blst = (xdr_break *) Realloc (plst->break_list, len * sizeof (xdr_break));
    if (!blst) {
      return (-1);              /* failed, no space */
    }
    plst->break_alloc = len;    /* got more */
    plst->break_list = blst;

    /*
     * Clear new space 
     */
    memset (blst + len - BKPT_INCR, 0, BKPT_INCR * sizeof (xdr_break));
    idx = len - BKPT_INCR;      /* next available */
    if (!idx) {
      idx = 1;                  /* for normal cases */
    }
  }
  return (normal ? idx : 0);    /* return it */
}

    /*
     *  BreakSet - set a breakpoint in process
     *
     *  Returns the number or -1/errno.
     */

#ifdef DDEBUG
static const char *BreakTypes[] = {
  "NONE", "INSTR", "READ", "WRITE",
  "ACCESS", "EXEC", "OS_CALL", "OS_SWITCH",
  "STEPEMUL"
};

#define BN_MAX		(sizeof BreakTypes / sizeof BreakTypes[0])
#define BREAK_NAME(t)	((unsigned) (t) < BN_MAX ? BreakTypes[t] : "???")
#endif

  int
BreakSet (PID_LIST * plst, int conn_idx, xdr_break * bkpt)
{
  int pid = plst->pid;
  int type = bkpt->type;
  void *addr = (void *) bkpt->ee_loc;
  int idx;
  int data;

  DPRINTF (("BreakSet: type %d (%s) at 0x%x th %d ee_type %d len %d "
            "pass %d curr %d list %d %d %d %d\n", type, BREAK_NAME (type),
            (int) addr,
            bkpt->thread_spec, bkpt->ee_type, bkpt->length, bkpt->pass_count,
            bkpt->curr_pass, bkpt->thread_list[0], bkpt->thread_list[1],
            bkpt->thread_list[2], bkpt->thread_list[3]));

  idx = BreakAlloc (plst, True);    /* get entry */
  if (idx < 0) {                /* no memory */
    setErrno (ENOMEM);          /* set for safety */
    return -1;                  /* return the error */
  }

  data = TgtPtrace (RPT_PEEKTEXT, pid, addr, 0, NULL);  /* current */
  if (getErrno ()) {
    return -1;                  /* failed, return the error */
  }
  if (IS_BREAK (data)) {        /* There is already a break here */
    DPRINTF (("BreakSet: already have soft bkpt at %x\n", addr));
    if (type == BRKT_STEPEMUL) {
      ++BKPT0 (plst)->pad1;
      return 1;                 /* Any non-error value is OK */
    }
    setErrno (EBUSY);
    return -1;
  }

  TgtPtrace (RPT_POKETEXT, pid, addr, SET_BREAK (data), NULL);

  if (getErrno ()) {
    return -1;
  }

  plst->break_list[idx] = *bkpt;
  plst->break_list[idx].ee_type = data; /* saved data */

  /*
   * Inform other owners 
   */
  if (type != BRKT_STEPEMUL) {
    TgtNotifyAll (plst - pid_list, BMSG_BREAK, 1 /*added */ , idx,
                  conn_idx, False);
  } else {
    ++BKPT0 (plst)->pad1;
  }
  /*
   * Return the number 
   */
  setErrno (0);                 /* Just in case */
  return idx;
}

  int
BreakSetAt (PID_LIST * plst, int conn_idx, unsigned long addr,
            break_type type)
{
  xdr_break xb;

  memset (&xb, 0, sizeof xb);
  xb.type = type;
  xb.ee_loc = addr;
  return BreakSet (plst, conn_idx, &xb);
}

/*----- Find a breakpoint by address -----*/

  int
BreakGetIndex (PID_LIST * plst, void *addr)
{
  int idx;
  int data = -1;

  if (!plst->break_alloc) {
    setErrno (EFAULT);
    return -1;
  }
  for (idx = 1; idx < (int) plst->break_alloc; idx++) {
    if ((u_long) addr == plst->break_list[idx].ee_loc) {
      data = idx;
      break;
    }
  }
  return data;
}

/*----- Getting information about breakpoint -----*/

    /*
     *  If data > 0, fill "bkpt" with information about breakpoint
     *  and return the number of the next one.
     *  If data == 0, return the count of breakpoints.
     */

  int
BreakGet (const PID_LIST * plst, int data, xdr_break * bkpt)
{
  int idx;

  if (!data) {                  /* just count them */
    for (idx = 1; idx < (int) plst->break_alloc; idx++) {
      if (plst->break_list[idx].type != BRKT_NONE) {
        data++;
      }
    }
    return data;                /* count */
  }
  if ((unsigned) data >= plst->break_alloc) {
    /*
     * out of range 
     */
    setErrno (EFAULT);          /* closest match */
    return -1;
  }
  /*
   * get it and say which is next 
   */
  *bkpt = plst->break_list[data];
  for (idx = (int) data + 1; idx < (int) plst->break_alloc; idx++) {
    if (plst->break_list[idx].type != BRKT_NONE) {
      return idx;
    }
  }
  return 0;                     /* otherwise returns 0 for no more */
}

/*----- Clearing bkpts -----*/

    /*
     *  BreakClear - clear one (if data != 0) or all breakpoints
     *  (if data == 0). Return the number of bkpts cleared.
     *  If (data == -1), remove step-emulation breakpoints.
     */

  int
BreakClear (PID_LIST * plst, int conn_idx, int data)
{
  int pid_idx = plst - pid_list;
  int idx;
  int cleared = 0;
  int clearStepEmul = 0;
  int terminated = PROC_TERMINATED (plst);
  int stepEmulCount = 0;

  /*
   * break handle in data 
   */
  if (!plst->break_alloc) {     /* there are no breaks */
    DPRINTF (("BreakClear: no bkpts defined.\n"));
    setErrno (EFAULT);          /* closest match */
    return -1;                  /* return error */
  }
  if (!data) {                  /* clear all */
    idx = 1;
    data = plst->break_alloc - 1;

    /*
     * Inform other owners 
     */
    DPRINTF (("BreakClear: clearing all bkpts.\n"));
    TgtNotifyAll (pid_idx, BMSG_BREAK, 0 /*clr */ , 0, conn_idx, False);

  } else if (data == -1) {      /* clear all step-emul bkpts */
    DPRINTF (("BreakClear: removing %d step-emul bkpts\n",
              BKPT0 (plst)->pad1));

    stepEmulCount = BKPT0 (plst)->pad1;
    BKPT0 (plst)->pad1 = 0;

    clearStepEmul = 1;
    idx = 1;
    data = plst->break_alloc - 1;
  } else if ((unsigned) data >= plst->break_alloc
             || plst->break_list[data].type == BRKT_NONE) {

    /*
     * out of range 
     */
    DPRINTF (("BreakClear: invalid bkpt %d\n", data));
    setErrno (EFAULT);          /* closest match */
    return -1;                  /* return error */
  } else {
    idx = data;
    /*
     * Inform other owners 
     */
    TgtNotifyAll (pid_idx, BMSG_BREAK, 0 /*clr */ , idx, conn_idx, False);
    DPRINTF (("BreakClear: clearing bkpt %d\n", data));
  }

  for (; idx <= data; idx++) {  /* clear each one */
    int type = plst->break_list[idx].type;

    if (clearStepEmul && type != BRKT_STEPEMUL)
      continue;

    if (type == BRKT_INSTR || (clearStepEmul && type == BRKT_STEPEMUL)) {
      /*
       * just patch back 
       */
      char *addr = (char *) plst->break_list[idx].ee_loc;
      int val;

      if (BKPT0 (plst)->clr_step && BKPT0 (plst)->last_break == idx) {
        BKPT0 (plst)->clr_step = 0; /* not needed */
      }
      /*
       * Neighboring bytes can have breakpoints too... 
       */
      if (!terminated) {
        setErrno (0);
        val = TgtPtrace (RPT_PEEKTEXT, plst->pid, addr, 0, NULL);
        if (getErrno ()) {
          DPRINTF (("BreakClear: addr %x not readable!\n", addr));
          setErrno (0);         /* Forget bkpt */
        } else {
          assert (IS_BREAK (val));
          val = ORG_BREAK (val, (int) plst->break_list[idx].ee_type);
          TgtPtrace (RPT_POKETEXT, plst->pid, addr, val, NULL);
          if (getErrno ()) {
            DPRINTF (("BreakClear: addr %x not writable!\n", addr));
            setErrno (0);
          }
        }
      }
      ++cleared;                /* indicate cleared */
    }
    memset (&plst->break_list[idx], 0, sizeof (xdr_break));
  }
  assert (!clearStepEmul || cleared <= stepEmulCount);
  if (stepEmulCount && cleared == 0) {
    DPRINTF (("BreakClear: all STEPEMUL bkpts were shared\n"));
    return 1;
  }
  return cleared;
}

/*----- Hiding of breakpoints -----*/

    /*
     *  PatchBreak - patch original data from break into data buffer.
     *
     * Notes:
     *  - this routine patches the original data under a break into the data
     *    buffer from a ptrace read/peek. 
     */

  static void
PatchBreak (char *buff, UINT32 bstart, int bsize, UINT32 dstart, char *dvalue)
{
  int size = BREAK_SIZE;        /* default size */

  /*
   * Must deal with all sorts of unalignments
   * * (3 full overlaps, 3 unaligns)
   */
  if (bsize < BREAK_SIZE) {
    /*
     * case where buffer is smaller than data 
     */
    memcpy (buff, dvalue + (bstart - dstart), bsize);   /* copy over */
    return;
  }
  /*
   * buffer larger than data.
   * * we need to see where break fits in buffer and whether
   * * we have part of it off the end. We set bstart to be the
   * * buffer offset, dtart to be the break data offset, and
   * * size to be the amount to copy
   */
  if (dstart < bstart) {
    /*
     * break before actual buffer 
     */
    dstart = bstart - dstart;   /* offset in data */
    size -= dstart;             /* amount to copy */
    bstart = 0;                 /* offset in buffer */

  } else if (dstart + size > bstart + bsize) {
    /*
     * off end 
     */
    bstart += bsize;            /* end of buffer */
    size -= (dstart + size) - bstart;
    bstart = bsize - size;      /* come back into buffer enough */
    dstart = 0;                 /* start of data */

  } else {                      /* normal case */
    bstart = dstart - bstart;   /* offset in buffer */
    dstart = 0;
  }
  memcpy (buff + bstart, dvalue + dstart, size);
}

  void
BreakHide (const PID_LIST * plst, void *addr, int data, void *addr2)
{
  int idx;

  if (!plst->break_list)        /* no breaks exist, so skip this */
    return;

  /*
   * if breakpoints, replace 
   */

  for (idx = 1; idx < (int) plst->break_alloc; idx++) {
    int type = plst->break_list[idx].type;

    if (type != BRKT_INSTR && type != BRKT_STEPEMUL) {
      continue;
    }
    /*
     * break, see if overlaps 
     */
    if (BKPT_OVER (plst, idx, addr, data)) {

      /*
       * overlaps, patch in old value 
       */
      PatchBreak ((char *) addr2, (UINT32) addr, data,
                  plst->break_list[idx].ee_loc,
                  (char *) &plst->break_list[idx].ee_type);
    }
  }
}

/*----- Checking of breakpoint overwrites -----*/

    /*
     *  BreakOverwrite - check if memory write does not involve addresses
     *  having software breakpoints.
     */

  int
BreakOverwrite (const PID_LIST * plst, const char *addr, unsigned int size)
{
  int idx;

  if (!plst->break_list) {      /* No breaks exist */
    return 0;
  }

  for (idx = 1; idx < (int) plst->break_alloc; idx++) {
    int type = plst->break_list[idx].type;

    /*
     * Consider only breakpoints involving modified memory 
     */
    if (type != BRKT_INSTR && type != BRKT_STEPEMUL) {
      continue;
    }
    if (BKPT_OVER (plst, idx, addr, size)) {
      return -1;                /* overlaps */
    }
  }
  return 0;
}

/*----- Execution support -----*/

    /*
     *  BreakStepRange - Start stepping in a range.
     *
     *  Range is saved in breakpoint 0.
     */

  int
BreakStepRange (PID_LIST * plst, void *addr, int len)
{
  if (!plst->break_list) {
    /*
     * get list 
     */
    if (BreakAlloc (plst, False) == -1) {   /* must not be any memory */
      setErrno (ENOMEM);        /* to be safe */
      return -1;                /* fails */
    }
  }
  BKPT0 (plst)->range_start = (UINT32) addr;
  BKPT0 (plst)->range_end = (UINT32) addr + (len - 1);
  return 0;
}

    /*
     *  If the Program Counter is changed, consider that the
     *  current breakpoint has not been reached yet.
     */

  void
BreakPcChanged (PID_LIST * plst)
{
  if (plst->break_list) {
    /*
     * clear break stuff 
     */
    BKPT0 (plst)->clr_step = False;
  }
}

    /*
     *  BreakStepOff - prepare stepping off a breakpoint.
     */

  int
BreakStepOff (const PID_LIST * plst, void **paddr2)
{
  if (plst->break_list && BKPT0 (plst)->clr_step) {

    /*
     * need clear then step off break 
     */
    int last = BKPT0 (plst)->last_break;

    /*
     * clear break, step, then do exec 
     */

    *paddr2 = (void *) plst->break_list[last].ee_type;

    /*
     * Need to clr_step after TgtPtrace() when wait() returns 
     */
    return 1;
  }
  return 0;
}

    /*
     *  BreakSteppedOff - check if just stepped off a breakpoint
     *  and re-insert it into the code.
     */

  void
BreakSteppedOff (PID_LIST * plst)
{
  if (plst->break_list && BKPT0 (plst)->clr_step) {
    int idx = BKPT0 (plst)->last_break;
    int data;

    BKPT0 (plst)->clr_step = 0;

    /*
     *  Re-insert the breakpoint.
     */
    data = TgtPtrace (RPT_PEEKTEXT, plst->pid,
                      (char *) plst->break_list[idx].ee_loc, 0, NULL);
    assert (!IS_BREAK (data));
    TgtPtrace (RPT_POKETEXT, plst->pid,
               (char *) plst->break_list[idx].ee_loc,
               (int) SET_BREAK (data), NULL);
  }
}

    /*
     *  Returns whether a thread matches a breakpoint.
     */

  static int
BreakThreadMatch (xdr_break * xb, int thread)
{
  int slot;

  if (thread < 0)
    return 1;                   /* Break existence check only */

  if (xb->thread_list[0] == 0)
    return 1;                   /* Universal break */

  for (slot = 0; slot < BKPT_SLOTS; ++slot) {
    if (xb->thread_list[slot] == 0)
      return 0;                 /* End of list */
    if (xb->thread_list[slot] == thread)
      return 1;                 /* Match */
  }
  return 0;                     /* No matches found */
}

  int
BreakAdjustPC (PID_LIST * plst)
{
  /*
   *  BREAK_ADJ is the value by which the Program Counter
   *  has to be decremented after a software breakpoint
   *  is hit. It must be defined and can be zero.
   */
#if BREAK_ADJ
  /*
   * subtract back if necessary 
   */
  plst->regs.REG_PC -= BREAK_ADJ;   /* now write back */
  TgtPtrace (RPT_SETREGS, plst->pid, (char *) &plst->regs, 0, NULL);
#else
  (void) plst;
#endif
  return 0;
}

/*
 *  Identify the current breakpoint. The process just stopped.
 */

  int
BreakIdentify (PID_LIST * plst, int adjust, int thread)
{
  int foreignBkpt = 0;
  int bidx;

  for (bidx = 1; bidx < (int) plst->break_alloc; bidx++) {
    int type = plst->break_list[bidx].type;

    if ((type == BRKT_INSTR || type == BRKT_STEPEMUL)
        && plst->regs.REG_PC - BREAK_ADJ == plst->break_list[bidx].ee_loc) {    /* found matching */
      if (!BreakThreadMatch (&plst->break_list[bidx], thread)) {
        if (foreignBkpt == 0) {
          foreignBkpt = bidx;
        }
        continue;
      }
      if (adjust) {
        BreakAdjustPC (plst);
      }
      return bidx;
    }
  }
  if (foreignBkpt) {
    if (adjust) {
      BreakAdjustPC (plst);
    }
    return -foreignBkpt;
  }
  return 0;
}

/*
 **********************************************************************
 *
 *  Component:	RDB
 *  Module:	servutil.c
 *
 *  Synopsis:	Various utility routines
 *
 * $Id$
 *
 **********************************************************************
 */

#include <string.h>
#include <rdbg/rdbg.h>
#include <rdbg/servrpc.h>

/*----- Management of per-process list ----*/

    /*
     *  ListAlloc - build up list entry.
     *
     *  Notes:
     *  - this is a generic routine to build up entries in the per-connection
     *    list. The fields list, list_sz and list_alloc are affected.
     */

  Boolean
ListAlloc (buff, clst)
     char *buff;
     CONN_LIST *clst;           /* place to copy it */
{
  int tmp;
  char *name;
  int new_len;
  int len;

  tmp = strlen (buff);
  new_len = (int) clst->list_sz + 1 + tmp;
  if (clst->list_alloc < (unsigned) new_len) {
    /*
     * need more space 
     */
    name = (char *) Realloc (clst->list, len = new_len + MAX_FILENAME);
    if (name == NULL) {
      return (False);           /* failed, no space */
    }
    clst->list_alloc = len;
    clst->list = name;
  }
  strcpy (clst->list + clst->list_sz, buff);
  clst->list_sz += tmp;
  return (True);
}

/*----- Management of processes -----*/

    /*
     *  FindPidEntry - locate pid_list entry from pid 
     */

  int
FindPidEntry (int pid) /* pid = process identifier */
{
  int idx;

  /*
   * pid 0 is invalid, and signals a free slot 
   */
  if (pid_list == NULL || pid == 0) {
    return -1;
  }
  for (idx = 0; idx < pid_list_cnt; idx++) {
    if (pid_list[idx].pid == pid)
      return idx;
  }
  return -1;
}

/*----- Debug suport -----*/

#ifdef DDEBUG

    /*
     *  Names of debug primitives
     */

const char *PtraceNames[] = {

  "RPT_TRACEME", "RPT_PEEKTEXT", "RPT_PEEKDATA", "RPT_PEEKUSER",
  "RPT_POKETEXT", "RPT_POKEDATA", "RPT_POKEUSER", "RPT_CONT",
  "RPT_KILL", "RPT_SINGLESTEP", "RPT_ATTACH", "RPT_DETACH",
  "RPT_GETREGS", "RPT_SETREGS", "RPT_GETFPREGS", "RPT_SETFPREGS",
  "RPT_READDATA", "RPT_WRITEDATA", "RPT_READTEXT", "RPT_WRITETEXT",
  "RPT_GETFPAREGS", "RPT_SETFPAREGS", "RPT_22", "RPT_23",
  "RPT_SYSCALL", "RPT_DUMPCORE", "RPT_26", "RPT_27",
  "RPT_28", "RPT_GETUCODE", "RPT_30", "RPT_31",
  "RPT_32", "RPT_33", "RPT_34", "RPT_35",
  "RPT_36", "RPT_37", "RPT_38", "RPT_39",
  "RPT_40", "RPT_41", "RPT_42", "RPT_43",
  "RPT_44", "RPT_45", "RPT_46", "RPT_47",
  "RPT_48", "RPT_49", "RPT_GETTARGETTHREAD", "RPT_SETTARGETTHREAD",
  "RPT_THREADSUSPEND", "RPT_THREADRESUME", "RPT_THREADLIST",
    "RPT_GETTHREADNAME",
  "RPT_SETTHREADNAME", "RPT_SETTHREADREGS", "RPT_GETTHREADREGS",
  "RPT_59",
  "RPT_60", "RPT_61", "RPT_62", "RPT_63",
  "RPT_64", "RPT_65", "RPT_66", "RPT_67",
  "RPT_68", "RPT_69", "RPT_70", "RPT_71",
  "RPT_72", "RPT_73", "RPT_74", "RPT_STEPRANGE",
  "RPT_CONTTO", "RPT_SETBREAK", "RPT_CLRBREAK", "RPT_GETBREAK",
  "RPT_GETNAME", "RPT_STOP",
  "RPT_PGETREGS", "RPT_PSETREGS",
  "RPT_PSETTHREADREGS", "RPT_PGETTHREADREGS"
};

  const char *
PtraceName (int req)
{
  static char bufret[40];

  if ((req < 0) || (req >= sizeof (PtraceNames) / sizeof (char *))) {
    sprintf (bufret, "BAD_REQ_%d", req);
    return bufret;
  }
  return PtraceNames[req];
}

const char *BmsgNames[] = {
  "?", "WARM", "WAIT", "BREAK",
  "EXEC_FAIL", "DETACH", "KILLED", "NOT_PRIM",
  "NEW_PID"
};

#endif /* DDEBUG */

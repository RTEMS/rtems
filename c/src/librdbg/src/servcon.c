/*
 **************************************************************************
 *
 *  Component:	RDBG
 *  Module:	servcon.c
 *
 *  Synopsis:	Management of RPC client connections.
 *
 * $Id$
 *
 **************************************************************************
 */

#include <sys/errno.h>
#include <rdbg/rdbg.h>
#include <rdbg/servrpc.h>

    /*
     *  ConnCreate - create a new connection entry for a client.
     *
     *  This function finds an empty entry in the connection array
     *  or makes space. It fills in the fields that are passed to it.
     *  It does not do any validation on net addresses nor does it
     *  start a validation cycle on other clients. This is done by
     *  the caller.
     */

  int
ConnCreate (struct svc_req *rqstp, open_in * in)
{
  NET_OPAQUE sender;
  int idx;
  CONN_LIST *clst;

  setErrno (0);

  /*
   * Convert to valid Net address 
   */
  if (!TspTranslateRpcAddr (rqstp, &sender)) {
    DPRINTF (("ConnCreate: TspTranslateRpcAddr failed\n"));
    return -1;
  }
  if (!TspValidateAddr ((NET_OPAQUE *) in->back_port, &sender)) {
    DPRINTF (("ConnCreate: TspValidateAddr failed\n"));
    return -1;                  /* errno now setup with error */
  }

  /*
   * look for an empty connection entry 
   */
  for (idx = 0; idx < conn_list_cnt; idx++) {
    if (!conn_list[idx].in_use)
      break;                    /* an empty one found */
  }

  if (idx >= conn_list_cnt) {   /* no empties, create space */
    CONN_LIST *tmp_conn_list = conn_list;

    conn_list_cnt += CONN_LIST_INC;
    if (conn_list) {
      conn_list = (CONN_LIST *) Realloc (conn_list, /* extend */
                                         conn_list_cnt * sizeof (CONN_LIST));
    } else {
      conn_list = (CONN_LIST *) Malloc (conn_list_cnt * sizeof (CONN_LIST));
    }

    if (!conn_list) {           /* unable to get space */
      if ((conn_list_cnt -= CONN_LIST_INC)) {
        /*
         * was realloc, restore space 
         */
        conn_list = tmp_conn_list;
      }
      return -1;                /* errno set by failed alloc */
    }
    /*
     * clear newly created memory 
     */
    memset (conn_list + idx, 0, CONN_LIST_INC * sizeof (CONN_LIST));
  } else {                      /* clear new entry */
    memset (conn_list + idx, 0, sizeof (CONN_LIST));
  }
  clst = conn_list + idx;

  clst->in_use = True;          /* now in use */
  clst->sender = sender;
  memcpy (&clst->back_port, &in->back_port, sizeof (NET_OPAQUE));
  memcpy (&clst->route, &in->destination, sizeof (NET_OPAQUE));
  clst->debug_type = (UCHAR) in->debug_type;
  clst->flags = in->flags;
  strncpy (clst->user_name, in->user_name, NAMEMAX - 1);
  clst->user_name[NAMEMAX - 1] = 0;

  return idx;
}

    /*
     *  ConnDelete - remove connection entry when shutdown.
     *
     */

  void
ConnDelete (int conn, struct svc_req *rqstp, close_control control)
{
  CONN_LIST *clst = conn_list + conn;
  int idx;
  Boolean prim;

  if (!clst->in_use)
    return;                     /* not active */

  for (idx = 0; idx < pid_list_cnt; idx++) {
    PID_LIST *plst = pid_list + idx;

    if (!PIDMAP_TEST (conn, idx))
      continue;

    /*
     * found a controlled pid 
     */
    prim = (plst->primary_conn == conn) ? True : False;
    TgtDetachCon (conn, idx, True);

    /*
     * if still running or alive, we use close control on it 
     */
    if (!plst->pid)
      continue;                 /* entry gone */

    if (prim && control == CLOSE_KILL) {
      /*
       * kill off process 
       */
      TgtKillAndDelete (plst, rqstp, True);
    } else if (!plst->owners) {
      /*
       * no owners left 
       */
      if (control == CLOSE_DETACH) {
        TgtKillAndDelete (plst, rqstp, False);
      }
      if (control == CLOSE_DETACH || PROC_TERMINATED (plst)) {
        TgtDelete (plst, conn, (control == CLOSE_DETACH) ? BMSG_DETACH : 0);
      }
    }
  }
  if (clst->list) {
    Free (clst->list);          /* free allocated memory */
  }
  DPRINTF (("ConnDelete: Connection closed for port %u\n",
            HL_W (*((UINT16 *) & clst->back_port.c[2]))));

  clst->in_use = False;         /* free it back */
}

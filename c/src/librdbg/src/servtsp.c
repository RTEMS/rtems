/*
 **************************************************************************
 *
 *  Component:	RDBG
 *  Module:	servtsp.c
 *
 *  Synopsis:	Transport management for remote debug server.
 *
 * $Id$
 *
 **************************************************************************
 */

#include <sys/errno.h>
#include <rdbg/rdbg.h>
#include <rdbg/servrpc.h>
#include <signal.h>
#include <rpc/rpc.h>
#include <rpc/svc.h>
#include <netinet/in.h>
#include <sys/socket.h>

static int out_sock;
static int warm_test;

static void TimeTestHandler ();

    /*
     *  TspInit - Initialize the transport system.
     *
     */

  void
TspInit (int id)
{
  struct sigaction sa;
  struct sockaddr_in addr;

  /*
   * setup a socket to send event messages back through 
   */
  out_sock = socket (PF_INET, SOCK_DGRAM, 0);
  if (out_sock < 0) {
    DPRINTF (("TspInit: socket() failed %d errno %d\n",
              out_sock, getErrno ()));
    return;                     /* failed to open socket, let caller deal with */
  }

  bzero ((void *) (&addr), sizeof (addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons (BackPort);
  if (bind (out_sock, (struct sockaddr *) &addr, sizeof (addr)) < 0) {
    DPRINTF (("TspInit: bind() failed\n"));
  }
  /*
   * setup alarm timer for warm testing 
   */
  memset (&sa, 0, sizeof (sa));
  sa.sa_handler = TimeTestHandler;
  sigaction (SIGALRM, &sa, 0);
}

    /*
     *  TspTranslateRpcAddr - translate from an RPC handle to an
     *                        opaque address.
     *
     *  Converts the sender's address into the opaque data structure
     *  used for network addresses. This is used to look up the sender
     *  on each call.
     */

  Boolean
TspTranslateRpcAddr (struct svc_req *rqstp, NET_OPAQUE * opaque)
{
  struct sockaddr_in *addr;     /* used as template to extract net info */
  unsigned char *up;

  memset (opaque, 0, sizeof (NET_OPAQUE));
  /*
   *  We interpret the remote address as a standard netbuf name.
   *  The format is 2 bytes of address family (normally AF_INET)
   *  and then a length (5) and then the IP address.
   */
  if (rqstp->rq_xprt->xp_addrlen != 16) {
    DPRINTF (("TspTranslateRpcAddr: Unknown remote address!!!\n"));
    setErrno (EPROTONOSUPPORT);
    return False;               /* invalid, so fails */
  }
  /*
   * addr = &rqstp->rq_xprt->xp_raddr; 
   */
  addr = svc_getcaller (rqstp->rq_xprt);
  /*
   * verify it is AF_INET 
   */
  if (addr->sin_family != AF_INET) {    /* no, error */
    DPRINTF (("TspTranslateRpcAddr: Not an internet address!!\n"));
    setErrno (EAFNOSUPPORT);    /* invalid addr family */
    return False;
  }
  /*
   * good address type 
   */
  up = (unsigned char *) &addr->sin_addr.s_addr;
  DPRINTF (("TspTranslateRpcAddr: Sent by %u.%u.%u.%u port #%u\n",
            up[0], up[1], up[2], up[3], htons (addr->sin_port)));
  memcpy (opaque, addr, sizeof (struct sockaddr_in));
  return True;
}

    /*
     *  TspValidateAddr - validate a passed in opaque address.
     *
     *  Checks that the passed in address is in the format
     *  expected.
     */

  Boolean
TspValidateAddr (NET_OPAQUE * opaque, NET_OPAQUE * sender)
{
  struct sockaddr_in *addr;     /* used as template to extract net info */

  addr = (struct sockaddr_in *) opaque;
  /*
   * Verify it is AF_INET. Should check against sender IP address too 
   */
  if (addr->sin_family != AF_INET) {
    DPRINTF (("TspValidateAddr: Back port invalid: %d\n",
              htons (addr->sin_port)));
    return False;               /* not valid */
  }
  /*
   * otherwise, we copy in the IP address, since client may not know it 
   */
  addr->sin_addr.s_addr = ((struct sockaddr_in *) sender)->sin_addr.s_addr;
  DPRINTF (("TspValidateAddr: Back port is %d\n", htons (addr->sin_port)));
  return True;
}

    /*
     *  TspConnGetIndex - lookup an rpc caller's address as a connection entry.
     *
     *  Looks up an ip address of a caller to locate the
     *  connection index in our connection array.
     */

  int
TspConnGetIndex (struct svc_req *rqstp)
{
  int conn;
  /*
   * &rqstp->rq_xprt->xp_raddr; 
   */
  struct sockaddr_in *addr = svc_getcaller (rqstp->rq_xprt);

  for (conn = 0; conn < conn_list_cnt; conn++) {
    if (!conn_list[conn].in_use)
      continue;                 /* not used */

    if (addr->sin_addr.s_addr == ((struct sockaddr_in *)
                                  &conn_list[conn].sender)->sin_addr.s_addr
        && addr->sin_port == ((struct sockaddr_in *)
                              &conn_list[conn].sender)->sin_port) {
      return conn;
    }
  }
  return -1;
}

    /*
     *  TspSendWaitChange - send wait-change message to clients to
     *                      notify change.
     */

  void
TspSendWaitChange (int conn,        /* connection to send to */
                   BACK_MSG msg,    /* BMSG type */
                   UINT16 spec,     /* special information */
                   PID pid,         /* pid it refers to */
                   UINT32 context,  /* additional context for message */
                   Boolean force)  /* force into being only message */
{                               
  int idx;
  struct SEND_LIST *snd_ptr;

  if (force) {
    /*
     * force to top, which means others gone 
     */
    idx = 0;
    conn_list[conn].send_idx = 1;
    conn_list[conn].retry = 0;
  } else {
    for (idx = 0; idx < (int) conn_list[conn].send_idx; idx++) {
      if (conn_list[conn].send_list[idx].send_type == msg
          && conn_list[conn].send_list[idx].pid == pid)
        return;                 /* already pended for this pid */
    }
    idx = conn_list[conn].send_idx;
    if (idx + 1 > MAX_SEND)
      return;                   /* we lose it, what should we do??? */
    conn_list[conn].send_idx++;
  }
  snd_ptr = &conn_list[conn].send_list[idx];
  snd_ptr->send_type = msg;     /* message to send */
  snd_ptr->retry = TSP_RETRIES; /* about 1 minute of retries */
  snd_ptr->spec = htons ((u_short) spec);
  snd_ptr->pid = htonl (pid);
  snd_ptr->context = htonl (context);
  TspSendMessage (conn, False); /* now do the send */
}

    /*
     *  TspSendMessage - send message at top of send list for connection.
     */

  void
TspSendMessage (int conn, Boolean resend)
{
  struct sockaddr_in addr;
  struct UDP_MSG msg;
  int cnt;

  if (!resend && conn_list[conn].retry)
    return;                     /* already waiting for reply */

  /*
   *  Note on above: if no back port we can't remove unless
   *  someone blows off.
   */
  if (!resend) {
    /*
     * first time, setup. Set retry count: 
     */
    conn_list[conn].retry = conn_list[conn].send_list[0].retry;
    conn_list[conn].last_msg_num++; /* new sequence number */
    if (!warm_test++) {         /* starting, so enable timer */
      alarm (2);                /* resend every 2 seconds as needed */
    }
  }

  msg.type = conn_list[conn].send_list[0].send_type;
  msg.msg_num = conn_list[conn].last_msg_num;
  msg.spec = conn_list[conn].send_list[0].spec;
  msg.pid = conn_list[conn].send_list[0].pid;
  msg.context = conn_list[conn].send_list[0].context;

  memset (&addr, 0, sizeof (addr));
  addr.sin_family = AF_INET;
  addr.sin_port =
    ((struct sockaddr_in *) &conn_list[conn].back_port)->sin_port;
  addr.sin_addr.s_addr =
    ((struct sockaddr_in *) &conn_list[conn].back_port)->sin_addr.s_addr;

  DPRINTF (("TspSendMessage: Sending msg %d (%s) to port %d\n",
            msg.type, BmsgNames[msg.type], HL_W (addr.sin_port)));

  cnt = sendto (out_sock, &msg, sizeof (msg), 0, (struct sockaddr *) &addr,
                sizeof (addr));
  if (cnt != sizeof (msg)) {      /* failed on send */
    printf ("%s: Failed to send msg %d to conn %d (%d vs. %d)\n",
            taskName, msg.type, conn, cnt, sizeof (msg));
  }
}

    /*
     *  TspMessageReceive - confirmation received, now send next if any.
     *
     *  - since UDP is connectionless, we batch up the sends and use
     *    one at a time until we get a message indicating ready for
     *    next (from ack).
     */

  void
TspMessageReceive (int conn, PID pid)
{
  /*
   * We remove the send list entry and use next if any 
   */
  conn_list[conn].retry = 0;    /* reset */
  if (!warm_test || !--warm_test) {
    alarm (0);                  /* reset timer if not used */
  }
#ifdef DDEBUG
  if (conn_list[conn].send_list[0].send_type == BMSG_WARM) {
    DPRINTF (("TspMessageReceive: Connection reset for conn %d\n", conn));
  }
#endif
  /*
   * Move up by one if needed 
   */
  if (!--conn_list[conn].send_idx)
    return;                     /* no more to do */

  memcpy (conn_list[conn].send_list, conn_list[conn].send_list + 1, conn_list[conn].send_idx * sizeof (struct SEND_LIST));  /* copy down */
  TspSendMessage (conn, 0);
}

    /*
     *  TspGetHostName - return client's host name.
     *
     *  - this routine returns the name of the client's host or the net
     *    number of unknown.
     */

  char *
TspGetHostName (conn_idx)
     int conn_idx;              /* client connection number */
{
  static char buff[30];         /* largest net num */
  unsigned char *cp;

  cp = conn_list[conn_idx].sender.c + 4;
  sprintf (buff, "%u.%u.%u.%u", cp[0], cp[1], cp[2], cp[3]);
  return buff;
}

    /*
     *  TimeTestHandler - alarm timer handler to resend warm/wait test.
     */

  static void
TimeTestHandler ()
{
  int conn;

  if (!warm_test)
    return;                     /* no longer enabled */

  for (conn = 0; conn < conn_list_cnt; conn++) {
    /*
     * locate all that are using this 
     */
    if (!conn_list[conn].in_use)
      continue;                 /* not used */

    if (!conn_list[conn].retry)
      continue;
    /*
     * found one that we are testing 
     */
    if (!--conn_list[conn].retry) {
      /*
       *  Counted down the retries: blow off.
       *  Need to have connection flag to indicate not blowing
       *  off for cases where client is stopped due to being
       *  debugged.
       */
      ConnDelete (conn, NULL, CLOSE_IGNORE);
      continue;
    }
    TspSendMessage (conn, True);    /* send another message */
  }
  alarm (2);                    /* setup for 2 seconds from now */
}

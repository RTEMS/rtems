/*
 **************************************************************************
 *
 * Component =   
 * 
 * Synopsis  =   rdbg.c
 *
 * $Id$
 *
 **************************************************************************
 */

#include <assert.h>
#include <errno.h>
#include <rtems.h>
#include <rtems/error.h>
#include <rdbg/rdbg.h>
#include <rdbg/servrpc.h>
#include <rtems/rtems_bsdnet.h>
#include <rpc/pmap_clnt.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <rtems/bspIo.h>

u_short rtemsPort = RTEMS_PORT;
int BackPort = RTEMS_BACK_PORT;
int rtemsActive = 0;
SVCXPRT *rtemsXprt;
int rtemsSock;
char taskName[] = "RTEMS rdbg daemon";
volatile int ExitForSingleStep = 0;
volatile int Continue;
volatile int justSaveContext;
volatile Objects_Id currentTargetThread;
volatile int CannotRestart = 0;
volatile int TotalReboot = 0;
int CONN_LIST_INC = 3;
int PID_LIST_INC = 1;
int TSP_RETRIES = 10;

  int
getId ()
{
  rtems_id id;

  rtems_task_ident (RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &id);
  return (int) (id);
}

  static int
rdbgInit (void)
{
  int sock;
  struct sockaddr_in addr;

  sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock == -1) {
    printf ("%s: rdbgInit: cannot allocate socket\n", taskName);
    return -1;
  }

  bzero ((void *) &addr, sizeof (struct sockaddr_in));
  addr.sin_port = htons (rtemsPort);
  if ((bind (sock, (struct sockaddr *) &addr, sizeof (addr))) == -1) {
    printf ("%s: rdbgInit: cannot bind socket\n", taskName);
    return -2;
  }
  rtemsXprt = svcudp_create (sock);
  if (svcudp_enablecache (rtemsXprt, 1) == 0) {
    printf ("%s: rdbgInit: cannot enable rpc cache\n", taskName);
    return -3;
  }
  rtemsSock = sock;
  if (!svc_register (rtemsXprt, REMOTEDEB, REMOTEVERS, remotedeb_2, 0)) {
    fprintf (stderr, "unable to register (REMOTEDEB, REMOTEVERS, udp).");
    return -4;
  }
  connect_rdbg_exception ();

  return 0;
}

  rtems_task
rdbgDaemon (rtems_task_argument argument)
{
  svc_run ();
}

  void
rtems_rdbg_initialize (void)
{
  rtems_name task_name;
  rtems_id tid;
  rtems_status_code status;

#ifdef DDEBUG
  rdb_debug = 1;                /* DPRINTF now will display */
#endif

  DPRINTF (("%s init starting\n", taskName));

  /*
   * Print version string 
   */
#ifdef DDEBUG
  printk ("RDBG v.%d built on [%s %s]\n", SERVER_VERS, __DATE__, __TIME__);
#else
  printk ("RDBG v.%d\n", SERVER_VERS);
#endif

  /*
   * Create socket and init UDP RPC server 
   */
  if (rdbgInit () != 0)
    goto error;

  Continue = 1;
  justSaveContext = 0;
  currentTargetThread = 0;

  task_name = rtems_build_name ('R', 'D', 'B', 'G');
  if ((status = rtems_task_create (task_name, 5, 24576,
                                   RTEMS_INTERRUPT_LEVEL (0),
                                   RTEMS_DEFAULT_ATTRIBUTES |
                                   RTEMS_SYSTEM_TASK, &tid))
      != RTEMS_SUCCESSFUL) {
    printf ("status = %d\n", status);
    rtems_panic ("Can't create task.\n");
  }

  status = rtems_task_start (tid, rdbgDaemon, 0);

  return;

error:
  printf ("initialization failed.\n");
}

  void
setErrno (int error)
{
  errno = error;
}

  int
getErrno ()
{
  return errno;
}

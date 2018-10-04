#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include <bsp.h>

#include <stdio.h>
#include <rtems/rtemspppd.h>
#include "system.h"


/* define global variables */
static unsigned int      pppdapp_linkcount = 0;
static rtems_id          pppdapp_taskid;


static void pppdapp_linkup_hook(void)
{
  pppdapp_linkcount++;
  printf("PPP LINK UP   [%d]\n", pppdapp_linkcount);
}

static void pppdapp_linkdown_hook(void)
{
  printf("PPP LINK DOWN [%d]\n", pppdapp_linkcount);
}

static void pppdapp_ipup_hook(void)
{
  /* send ipup signal to pppdapp task */
  rtems_event_send(pppdapp_taskid, RTEMS_EVENT_10);
}

static void pppdapp_ipdown_hook(void)
{
  /* send ip down signal to pppdapp task */
  rtems_event_send(pppdapp_taskid, RTEMS_EVENT_11);
}

static void pppdapp_setup(void)
{
  const char   *pUser     = "oscar";
  const char   *pPassword = "goldman";

#undef  USE_MODEM
#ifdef  USE_MODEM
  const char   *pTelephone        = "5551234";
  const char   *pInitScript       = "TIMEOUT@5@@AT@@OK@";
  const char   *pConnectScript    = "TIMEOUT@90@@ATDT%s@CONNECT@@name:@%s@word:@%s@";
  const char   *pDisconnectScript = "TIMEOUT@5@@ATH0@@OK@";
  char          pConnect[128];

  /* set the connect string */
  sprintf(pConnect, pConnectScript, pTelephone, pUser, pPassword);

  /* set pppd options for modem */
  rtems_pppd_set_option("/dev/ttyS2", NULL);
  rtems_pppd_set_option("57600", NULL);
  rtems_pppd_set_option("crtscts", NULL);
  rtems_pppd_set_option("modem", NULL);
  rtems_pppd_set_option("noauth", NULL);
  rtems_pppd_set_option("debug", NULL);
  rtems_pppd_set_option("init", pInitScript);
  rtems_pppd_set_option("connect", pConnect);
  rtems_pppd_set_option("disconnect", pDisconnectScript);
#else
  /* set pppd options for null modem direct link serial cable */
  rtems_pppd_set_option("/dev/ttyS1", NULL);
  rtems_pppd_set_option("57600", NULL);
  rtems_pppd_set_option("crtscts", NULL);
  rtems_pppd_set_option("local", NULL);
  rtems_pppd_set_option("noauth", NULL);
  rtems_pppd_set_option("debug", NULL);
  rtems_pppd_set_option("user", pUser);
  rtems_pppd_set_option("password", pPassword);
#endif

  /* set up pppd hooks */
  rtems_pppd_set_hook(RTEMS_PPPD_LINKUP_HOOK, pppdapp_linkup_hook);
  rtems_pppd_set_hook(RTEMS_PPPD_LINKDOWN_HOOK, pppdapp_linkdown_hook);
  rtems_pppd_set_hook(RTEMS_PPPD_IPUP_HOOK, pppdapp_ipup_hook);
  rtems_pppd_set_hook(RTEMS_PPPD_IPDOWN_HOOK, pppdapp_ipdown_hook);
}

static rtems_task pppdapp(rtems_task_argument arg)
{
  rtems_status_code   sc             = RTEMS_SUCCESSFUL;
  rtems_interval      tickspersecond = 0;
  rtems_option        options;
  rtems_event_set     in;
  rtems_event_set     out;

  /* initialize ticks per second */
  tickspersecond = rtems_clock_get_ticks_per_second();
  if ( tickspersecond == 0 ) {
    /* ensure value is greater than zero */
    tickspersecond = 100;
  }

  /* initiate connection */
  pppdapp_setup();
  rtems_pppd_connect();

  /* enter processing loop */
  in      = (RTEMS_EVENT_10 | RTEMS_EVENT_11);
  options = (RTEMS_EVENT_ANY | RTEMS_WAIT);
  while ( sc == RTEMS_SUCCESSFUL ) {
    /* wait for the next event */
    sc = rtems_event_receive(in, options, RTEMS_NO_TIMEOUT, &out);
    if ( sc == RTEMS_SUCCESSFUL ) {
      /* determine which event was sent */
      if ( out & RTEMS_EVENT_10 ) {
        /* ip up recived */
        /* call disconnect function */
        rtems_pppd_disconnect();
      }
      if ( out & RTEMS_EVENT_11 ) {
        /* ip down recived */
        /* sleep 10 seconds and call connect function */
        rtems_task_wake_after(10*tickspersecond);
        rtems_pppd_connect();
      }
    }
  }

  /* terminate myself */
  rtems_task_exit();
}

int pppdapp_initialize(void)
{
  int                 iReturn = (int)-1;
  rtems_status_code   status;
  rtems_name          taskName;

  taskName = rtems_build_name( 'p', 'a', 'p', 'p' );
  status   = rtems_task_create(taskName,
                               CONFIGURE_INIT_TASK_PRIORITY,
                               CONFIGURE_INIT_TASK_STACK_SIZE,
                               CONFIGURE_INIT_TASK_INITIAL_MODES,
                               RTEMS_DEFAULT_ATTRIBUTES,
                               &pppdapp_taskid);
  if ( status == RTEMS_SUCCESSFUL ) {
    status = rtems_task_start(pppdapp_taskid, pppdapp, 0);
    if ( status == RTEMS_SUCCESSFUL ) {
      iReturn = (int)0;
    }
  }

  return ( iReturn );
}

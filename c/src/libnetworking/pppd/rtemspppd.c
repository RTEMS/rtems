
#include <rtems.h>
#include "pppd.h"
#include "rtemspppd.h"


/* define pppd function prototypes */
extern void pppasyncattach(void);
extern int pppdmain(int, char **);

/* define global variables */
rtems_id rtems_pppd_taskid;


static rtems_task pppTask(rtems_task_argument arg)
{
  rtems_status_code   sc = RTEMS_SUCCESSFUL;
  rtems_option        options;
  rtems_event_set     in;
  rtems_event_set     out;

  /* call function to setup ppp line discipline */
  pppasyncattach();

  /* enter processing loop */
  in      = (RTEMS_EVENT_29 | RTEMS_EVENT_30);
  options = (RTEMS_EVENT_ANY | RTEMS_WAIT);
  while ( sc == RTEMS_SUCCESSFUL ) {
    /* wait for the next event */
    sc = rtems_event_receive(in, options, RTEMS_NO_TIMEOUT, &out);
    if ( sc == RTEMS_SUCCESSFUL ) {
      /* determine which event was sent */
      if ( out & RTEMS_EVENT_29 ) {
        /* terminate event received */
        /* set value to break out of event loop */
        sc = RTEMS_UNSATISFIED;
      }
      else if ( out & RTEMS_EVENT_30 ) {
        /* connect request */
        /* execute the pppd main code */
        pppdmain(0, NULL);
      }
    }
  }

  /* terminate myself */
  rtems_task_delete(RTEMS_SELF);
}

int rtems_pppd_initialize(void)
{
  int                 iReturn = (int)-1;
  rtems_status_code   status;
  rtems_name          taskName;

  taskName = rtems_build_name( 'p', 'p', 'p', 'd' );
  status   = rtems_task_create(taskName,
                               RTEMS_PPPD_TASK_PRIORITY,
                               RTEMS_PPPD_TASK_STACK_SIZE,
                               RTEMS_PPPD_TASK_INITIAL_MODES,
                               RTEMS_DEFAULT_ATTRIBUTES,
                               &rtems_pppd_taskid);
  if ( status == RTEMS_SUCCESSFUL ) {
    status = rtems_task_start(rtems_pppd_taskid, pppTask, 0);
    if ( status == RTEMS_SUCCESSFUL ) {
      iReturn = rtems_pppd_reset_options();
    }
  }

  return ( iReturn );
}

int rtems_pppd_terminate(void)
{
  /* send terminate signal to pppd task */
  rtems_event_send(rtems_pppd_taskid, RTEMS_EVENT_29);

  /* call the disconnect function */
  rtems_pppd_disconnect();

  return ( 0 );
}

int rtems_pppd_reset_options(void)
{
    int i;
    struct protent *protp;

    /*
     * Initialize to the standard option set, then parse, in order,
     * the system options file, the user's options file,
     * the tty's options file, and the command line arguments.
     */
    for (i = 0; (protp = protocols[i]) != NULL; ++i)
        (*protp->init)(0);

  return ( 0 );
}

int rtems_pppd_set_hook(int id, rtems_pppd_hookfunction hookfp)
{
  int     iReturn = (int)0;

  switch ( id ) {
  case RTEMS_PPPD_LINKUP_HOOK:
    auth_linkup_hook = hookfp;
    break;
  case RTEMS_PPPD_LINKDOWN_HOOK:
    auth_linkdown_hook = hookfp;
    break;
  case RTEMS_PPPD_IPUP_HOOK:
    ip_up_hook = hookfp;
    break;
  case RTEMS_PPPD_IPDOWN_HOOK:
    ip_down_hook = hookfp;
    break;
  default:
    iReturn = (int)-1;
    break;
  }

  return ( iReturn );
}

int rtems_pppd_set_option(const char *pOption, const char *pValue)
{
  int                iReturn = (int)0;
  int                prevPhase;
  struct wordlist    option;
  struct wordlist    value;

  if ( pOption != (const char *)0 ) {
    /* initialize the values */
    option.word = (char *)pOption;
    option.next = (struct wordlist *)0;
    if ( pValue != (const char *)0 ) {
      option.next = &value;
      value.word  = (char *)pValue;
      value.next  = (struct wordlist *)0;
    }

    /* save current phase value */
    prevPhase = phase;
    phase     = PHASE_INITIALIZE;

    /* process option and reset phase value */
    iReturn = options_from_list(&option, 1);
    phase   = prevPhase;
  }

  return ( iReturn );
}

int rtems_pppd_connect(void)
{
  /* send connect signal to pppd task */
  rtems_event_send(rtems_pppd_taskid, RTEMS_EVENT_30);

  return ( 0 );
}

int rtems_pppd_disconnect(void)
{
  /* set pppd global variables to disconnect */
  persist   = 0;
  kill_link = 1;

  /* send event to wake up the pppd code */
  /* pretend its a serial interrput */
  rtems_event_send(rtems_pppd_taskid, RTEMS_EVENT_31);

  return ( 0 );
}

/*
 * COPYRIGHT (c) 2001, Michael Siers <mikes@poliac.com>.
 *                     Poliac Research, Burnsville, Minnesota USA.
 * COPYRIGHT (c) 2001, On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include "pppd.h"
#include "rtemspppd.h"


/* define pppd function prototypes */
extern void pppasyncattach(void);
extern int  pppdmain(int, char **);

/* define global variables */
rtems_id                   rtems_pppd_taskid;
rtems_pppd_hookfunction    rtems_pppd_errorfp;
rtems_pppd_hookfunction    rtems_pppd_exitfp;


static rtems_task pppTask(rtems_task_argument arg)
{
  rtems_status_code   sc = RTEMS_SUCCESSFUL;
  rtems_option        options;
  rtems_event_set     in;
  rtems_event_set     out;
  int                 iStatus;

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
        iStatus = pppdmain(0, NULL);
        if ( iStatus == EXIT_OK ) {
          /* check exit callback */
          if ( rtems_pppd_exitfp ) {
            (*rtems_pppd_exitfp)();
          }
        }
        else {
          /* check error callback */
          if ( rtems_pppd_errorfp ) {
            (*rtems_pppd_errorfp)();
          }
        }
      }
    }
  }

  /* terminate myself */
  rtems_pppd_taskid = 0;
  rtems_task_delete(RTEMS_SELF);
}

int rtems_pppd_initialize(void)
{
  int                 iReturn  = (int)-1;
  rtems_task_priority priority = 100;
  rtems_status_code   status;
  rtems_name          taskName;

  /* determine priority value */
  if ( rtems_bsdnet_config.network_task_priority ) {
    priority = rtems_bsdnet_config.network_task_priority;
  }

  /* initialize the exit hook */
  rtems_pppd_exitfp = (rtems_pppd_hookfunction)0;

  /* create the rtems task */
  taskName = rtems_build_name( 'p', 'p', 'p', 'd' );
  status   = rtems_task_create(taskName, priority, 8192,
                               (RTEMS_PREEMPT|RTEMS_NO_TIMESLICE|RTEMS_NO_ASR|RTEMS_INTERRUPT_LEVEL(0)),
                               RTEMS_NO_FLOATING_POINT|RTEMS_LOCAL,
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
  case RTEMS_PPPD_ERROR_HOOK:
    rtems_pppd_errorfp = hookfp;
    break;
  case RTEMS_PPPD_EXIT_HOOK:
    rtems_pppd_exitfp = hookfp;
    break;
  default:
    iReturn = (int)-1;
    break;
  }

  return ( iReturn );
}

int rtems_pppd_set_dialer(rtems_pppd_dialerfunction dialerfp)
{
  pppd_dialer = dialerfp;
  return ( (int)0 );
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
    prevPhase = pppd_phase;
    pppd_phase     = PHASE_INITIALIZE;

    /* process option and reset phase value */
    iReturn = options_from_list(&option, 1);
    pppd_phase   = prevPhase;
  }

  return ( iReturn );
}

int rtems_pppd_connect(void)
{
  /* send connect signal to pppd task */
  rtems_event_send(rtems_pppd_taskid, RTEMS_EVENT_30);

  return ( 0 );
}

static void timeout_terminate(void *arg)
{
  /* set pppd global variables to disconnect */
  persist   = 0;
  pppd_kill_link = 1;
}

int rtems_pppd_disconnect(void)
{
  /* need to wait a little time before we can bring the link down */
  /* set up time out in 1 seconds */
  TIMEOUT(timeout_terminate, NULL, 1);

  /* send event to wake up the pppd code */
  /* pretend its a serial interrput */
  rtems_event_send(rtems_pppd_taskid, RTEMS_EVENT_31);

  return ( 0 );
}

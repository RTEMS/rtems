/* threadsup.h
 *
 *  $Id$
 */

#ifndef __RTEMS_POSIX_THREAD_SUPPORT_h
#define __RTEMS_POSIX_THREAD_SUPPORT_h

#include <rtems/score/coresem.h>
#include <rtems/score/tqdata.h>

typedef struct {
  pthread_attr_t          Attributes;
  int                     detachstate;
  Thread_queue_Control    Join_List;
  int                     schedpolicy;
  struct sched_param      schedparam;
  Watchdog_Control        Sporadic_timer;

  sigset_t                signals_blocked;
  sigset_t                signals_pending;
  sigset_t                signals_global_pending;

#if 0
  /*
   *  POSIX Interrupts 
   */
  unsigned32              interrupts_installed;
  CORE_semaphore_Control  Interrupt_Semaphore;
#endif

#if 0
  /*
   *  POSIX Cancelability 
   */
  int                     cancelability_state;
  int                     cancelability_type;
  int                     cancelation_requested;
  Chain_Control           Cancellation_Handlers;
#endif

} POSIX_API_Control;

#endif
/* end of include file */


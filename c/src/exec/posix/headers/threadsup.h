/* threadsup.h
 *
 *  $Id$
 */

#ifndef __RTEMS_POSIX_THREAD_SUPPORT_h
#define __RTEMS_POSIX_THREAD_SUPPORT_h

#include <rtems/score/coresem.h>

typedef struct {
  pthread_attr_t          Attributes;
  int                     detachstate;
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


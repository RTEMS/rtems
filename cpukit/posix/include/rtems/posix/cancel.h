/* rtems/posix/cancel.h
 *
 *  $Id$
 */

#ifndef __RTEMS_POSIX_CANCEL_h
#define __RTEMS_POSIX_CANCEL_h

typedef struct {
  Chain_Node  Node;
  void      (*routine)( void * );
  void       *arg;
}  POSIX_Cancel_Handler_control;

/*
 *  POSIX_Thread_cancel_run
 *
 *  DESCRIPTION:
 *
 *  XXX
 */

void POSIX_Thread_cancel_run(
  Thread_Control *the_thread
);

#endif
/* end of include file */

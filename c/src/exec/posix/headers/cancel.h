/* rtems/posix/cancel.h
 *
 */

#ifndef __RTEMS_POSIX_CANCEL_h
#define __RTEMS_POSIX_CANCEL_h

typedef struct {
  Chain_Node  Node;
  void      (*routine)( void * );
  void       *arg;
}  POSIX_Cancel_Handler_control;

#endif
/* end of include file */

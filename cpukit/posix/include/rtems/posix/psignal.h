/*
 *  $Id$
 */

#ifndef __RTEMS_POSIX_SIGNALS_h
#define __RTEMS_POSIX_SIGNALS_h

typedef struct {
  Chain_Node  Node;
  siginfo_t   Info;
}  POSIX_signals_Siginfo_node;

void _POSIX_signals_Manager_Initialization(
  int  maximum_queued_signals
);

void _POSIX_signals_Post_switch_extension(
  Thread_Control  *the_thread
);

#endif
/* end of file */

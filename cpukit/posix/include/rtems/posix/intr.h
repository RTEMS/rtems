/*  rtems/posix/intr.h
 *
 *  This include file contains all the private support information for
 *  POSIX Interrupt Manager.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
 
#ifndef __RTEMS_POSIX_KEY_h
#define __RTEMS_POSIX_KEY_h
 
#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/isr.h>
#include <rtems/score/object.h>

/*
 *  Data Structure used to manage each POSIX Interrupt Vector
 */

typedef struct {
  int            number_installed;
  int            lock_count;
  int            deferred_count;
  Chain_Control  Handlers;
} POSIX_Interrupt_Control;
 
/*
 *  Data Structure used to manage a POSIX Interrupt Handler
 */
 
typedef struct {
  Objects_Control         Object;
  int                     is_active; 
  intr_t                  vector; 
  Thread_Control         *server;
  int                   (*handler)( void *area );
  volatile void           *user_data_area;
}  POSIX_Interrupt_Handler_control;

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */
 
POSIX_EXTERN Objects_Information _POSIX_Interrupt_Handlers_Information;

/*
 *  The following is an array which is used to manage the set of
 *  interrupt handlers installed on each vector.
 */

POSIX_EXTERN POSIX_Interrupt_Control 
               _POSIX_Interrupt_Information[ ISR_NUMBER_OF_VECTORS ];
 
/*
 *  _POSIX_Interrupt_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */
 
void _POSIX_Interrupt_Manager_initialization( 
  unsigned32  maximum_interrupt_handlers
);
 
/*
 *  _POSIX_Interrupt_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a interrupt handler control block from
 *  the inactive chain of free interrupt handler control blocks.
 */
 
RTEMS_INLINE_ROUTINE POSIX_Interrupt_Handler_control *
  _POSIX_Interrupt_Allocate( void );
 
/*
 *  _POSIX_Interrupt_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a interrupt handler control block to the
 *  inactive chain of free interrupt handler control blocks.
 */
 
RTEMS_INLINE_ROUTINE void _POSIX_Interrupt_Free (
  POSIX_Interrupt_Handler_control *the_intr
);
 
/*
 *  _POSIX_Interrupt_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps interrupt handler IDs to interrupt handler control 
 *  blocks.  If ID corresponds to a local interrupt handler, then it returns
 *  the_intr control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the interrupt handler ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_intr is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_intr is undefined.
 */
 
RTEMS_INLINE_ROUTINE POSIX_Interrupt_Control *_POSIX_Interrupt_Get (
  Objects_Id         id,
  Objects_Locations *location
);
 
/*
 *  _POSIX_Interrupt_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_intr is NULL and FALSE otherwise.
 */
 
RTEMS_INLINE_ROUTINE boolean _POSIX_Interrupt_Is_null (
  POSIX_Interrupt_Handler_control *the_intr
);
 
/*
 *  _POSIX_Interrupt_Handler
 *
 *  DESCRIPTION:
 *
 *  This function XXX.
 */
 
void _POSIX_Interrupt_Handler(
  ISR_Vector_number   vector
);

#include <rtems/posix/intr.inl>

#ifdef __cplusplus
}
#endif
 
#endif
/*  end of include file */


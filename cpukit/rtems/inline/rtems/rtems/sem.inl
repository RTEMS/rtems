/*  sem.inl
 *
 *  This file contains the static inlin implementation of the inlined
 *  routines from the Semaphore Manager.
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

#ifndef __SEMAPHORE_inl
#define __SEMAPHORE_inl

/*PAGE
 *
 *  _Semaphore_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a semaphore control block from
 *  the inactive chain of free semaphore control blocks.
 */

RTEMS_INLINE_ROUTINE Semaphore_Control *_Semaphore_Allocate( void )
{
  return (Semaphore_Control *) _Objects_Allocate( &_Semaphore_Information );
}

/*PAGE
 *
 *  _Semaphore_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a semaphore control block to the
 *  inactive chain of free semaphore control blocks.
 */

RTEMS_INLINE_ROUTINE void _Semaphore_Free (
  Semaphore_Control *the_semaphore
)
{
  _Objects_Free( &_Semaphore_Information, &the_semaphore->Object );
}

/*PAGE
 *
 *  _Semaphore_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps semaphore IDs to semaphore control blocks.
 *  If ID corresponds to a local semaphore, then it returns
 *  the_semaphore control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the semaphore ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_semaphore is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_semaphore is undefined.
 */

RTEMS_INLINE_ROUTINE Semaphore_Control *_Semaphore_Get (
  Objects_Id         id,
  Objects_Locations *location
)
{
  return (Semaphore_Control *)
    _Objects_Get( &_Semaphore_Information, id, location );
}

RTEMS_INLINE_ROUTINE Semaphore_Control *_Semaphore_Get_interrupt_disable (
  Objects_Id         id,
  Objects_Locations *location,
  ISR_Level         *level
)
{
  return (Semaphore_Control *)
    _Objects_Get_isr_disable( &_Semaphore_Information, id, location, level );
}

/*PAGE
 *
 *  _Semaphore_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_semaphore is NULL and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Semaphore_Is_null (
  Semaphore_Control *the_semaphore
)
{
  return ( the_semaphore == NULL );
}

#endif
/*  end of include file */

/*  sem.inl
 *
 *  This file contains the static inlin implementation of the inlined
 *  routines from the Semaphore Manager.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
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

/**
 *  @file rtems/posix/key.inl
 *
 *  This include file contains the static inline implementation of the private 
 *  inlined routines for POSIX key's.
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
 
#ifndef _RTEMS_POSIX_KEY_H
# error "Never use <rtems/posix/key.inl> directly; include <rtems/posix/key.h> instead."
#endif

#ifndef _RTEMS_POSIX_KEY_INL
#define _RTEMS_POSIX_KEY_INL
 
/**
 *  @brief _POSIX_Keys_Allocate
 *
 *  This function allocates a keys control block from
 *  the inactive chain of free keys control blocks.
 */
 
RTEMS_INLINE_ROUTINE POSIX_Keys_Control *_POSIX_Keys_Allocate( void )
{
  return (POSIX_Keys_Control *) _Objects_Allocate( &_POSIX_Keys_Information );
}
 
/**
 *  @brief _POSIX_Keys_Free
 *
 *  This routine frees a keys control block to the
 *  inactive chain of free keys control blocks.
 */
RTEMS_INLINE_ROUTINE void _POSIX_Keys_Free (
  POSIX_Keys_Control *the_key
)
{
  _Objects_Free( &_POSIX_Keys_Information, &the_key->Object );
}
 
/**
 *  @brief _POSIX_Keys_Get
 *
 *  This function maps key IDs to key control blocks.
 *  If ID corresponds to a local keys, then it returns
 *  the_key control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the keys ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_key is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_key is undefined.
 */
 
RTEMS_INLINE_ROUTINE POSIX_Keys_Control *_POSIX_Keys_Get (
  pthread_key_t      id,
  Objects_Locations *location
)
{
  return (POSIX_Keys_Control *)
    _Objects_Get( &_POSIX_Keys_Information, (Objects_Id) id, location );
}
 
/**
 *  @brief _POSIX_Keys_Is_null
 *
 *  This function returns TRUE if the_key is NULL and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _POSIX_Keys_Is_null (
  POSIX_Keys_Control *the_key
)
{
  return !the_key;
}

#endif
/*  end of include file */


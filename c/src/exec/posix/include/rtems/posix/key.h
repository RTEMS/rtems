/*  rtems/posix/key.h
 *
 *  This include file contains all the private support information for
 *  POSIX key.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */
 
#ifndef __RTEMS_POSIX_KEY_h
#define __RTEMS_POSIX_KEY_h
 
#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Data Structure used to manage a POSIX key
 *
 *  NOTE:  The Values is a table indexed by the index portion of the
 *         ID of the currently executing thread.
 */
 
typedef struct {
   Objects_Control     Object;
   boolean             is_active;
   void              (*destructor)( void * );
   void              **Values[ OBJECTS_APIS_LAST + 1 ];
}  POSIX_Keys_Control;

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */
 
POSIX_EXTERN Objects_Information  _POSIX_Keys_Information;
 
/*
 *  _POSIX_Keys_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */
 
void _POSIX_Key_Manager_initialization(
  unsigned32 maximum_keys
);
 
/*
 *  _POSIX_Keys_Run_destructors
 *
 *  DESCRIPTION:
 *
 *  This function executes all the destructors associated with the thread's
 *  keys.  This function will execute until all values have been set to NULL.
 *
 *  NOTE:  This is the routine executed when a thread exits to
 *         run through all the keys and do the destructor action.
 */
 
void _POSIX_Keys_Run_destructors(
  Thread_Control *thread
);

/*
 *  _POSIX_Keys_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a keys control block from
 *  the inactive chain of free keys control blocks.
 */
 
RTEMS_INLINE_ROUTINE POSIX_Keys_Control *_POSIX_Keys_Allocate( void );
 
/*
 *  _POSIX_Keys_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a keys control block to the
 *  inactive chain of free keys control blocks.
 */
 
RTEMS_INLINE_ROUTINE void _POSIX_Keys_Free (
  POSIX_Keys_Control *the_key
);
 
/*
 *  _POSIX_Keys_Get
 *
 *  DESCRIPTION:
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
  Objects_Id         id,
  Objects_Locations *location
);
 
/*
 *  _POSIX_Keys_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_key is NULL and FALSE otherwise.
 */
 
RTEMS_INLINE_ROUTINE boolean _POSIX_Keys_Is_null (
  POSIX_Keys_Control *the_key
);

#include <rtems/posix/key.inl>

#ifdef __cplusplus
}
#endif
 
#endif
/*  end of include file */


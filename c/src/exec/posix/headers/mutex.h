/*  rtems/posix/mutex.h
 *
 *  This include file contains all the private support information for
 *  POSIX mutex's.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */
 
#ifndef __RTEMS_POSIX_MUTEX_h
#define __RTEMS_POSIX_MUTEX_h
 
#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/coremutex.h>
#include <pthread.h>

/*
 *  Data Structure used to manage a POSIX mutex
 */
 
typedef struct {
   Objects_Control     Object;
   int                 process_shared;
   CORE_mutex_Control  Mutex;
}  POSIX_Mutex_Control;

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */
 
POSIX_EXTERN Objects_Information  _POSIX_Mutex_Information;
 
/*
 *  _POSIX_Mutex_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */
 
void _POSIX_Mutex_Manager_initialization(
  unsigned32 maximum_mutexes
);
 
/*
 *  _POSIX_Mutex_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a mutexes control block from
 *  the inactive chain of free mutexes control blocks.
 */
 
RTEMS_INLINE_ROUTINE POSIX_Mutex_Control *_POSIX_Mutex_Allocate( void );
 
/*
 *  _POSIX_Mutex_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a mutexes control block to the
 *  inactive chain of free mutexes control blocks.
 */
 
RTEMS_INLINE_ROUTINE void _POSIX_Mutex_Free (
  POSIX_Mutex_Control *the_mutex
);
 
/*
 *  _POSIX_Mutex_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps mutexes IDs to mutexes control blocks.
 *  If ID corresponds to a local mutexes, then it returns
 *  the_mutex control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the mutexes ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_mutex is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_mutex is undefined.
 */
 
RTEMS_INLINE_ROUTINE POSIX_Mutex_Control *_POSIX_Mutex_Get (
  Objects_Id        *id,
  Objects_Locations *location
);
 
/*
 *  _POSIX_Mutex_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_mutex is NULL and FALSE otherwise.
 */
 
RTEMS_INLINE_ROUTINE boolean _POSIX_Mutex_Is_null (
  POSIX_Mutex_Control *the_mutex
);

#include <rtems/posix/mutex.inl>
#include <rtems/posix/mutexmp.h>

#ifdef __cplusplus
}
#endif
 
#endif
/*  end of include file */


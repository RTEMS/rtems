/*  apimutex.h
 *
 *  This include file contains all the constants and structures associated
 *  with the API Mutex Handler.  This handler is used by API level 
 *  routines to manage mutual exclusion.
 *
 *  COPYRIGHT (c) 1989-2002.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __API_MUTEX_h
#define __API_MUTEX_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/coremutex.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>

/*
 *  The following defines the control block used to manage each mutex.
 */

typedef struct {
  Objects_Control       Object;
  CORE_mutex_Control    Mutex;
}   API_Mutex_Control;

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */

SCORE_EXTERN Objects_Information  _API_Mutex_Information;

/*
 *  _API_Mutex_Initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this handler.
 */

#if defined(RTEMS_MULTIPROCESSING)
#define _API_Mutex_Initialization( _maximum_mutexes ) \
  _Objects_Initialize_information( \
    &_API_Mutex_Information, \
    OBJECTS_INTERNAL_API, \
    OBJECTS_INTERNAL_MUTEXES, \
    _maximum_mutexes, \
    sizeof( API_Mutex_Control ), \
    FALSE, \
    0, \
    FALSE, \
    NULL \
  );
#else
#define _API_Mutex_Initialization( _maximum_mutexes ) \
  _Objects_Initialize_information( \
    &_API_Mutex_Information, \
    OBJECTS_INTERNAL_API, \
    OBJECTS_INTERNAL_MUTEXES, \
    _maximum_mutexes, \
    sizeof( API_Mutex_Control ), \
    FALSE, \
    0  \
  );
#endif

/*
 *  _API_Mutex_Allocate
 *
 *  DESCRIPTION:
 *
 *  This routine allocates an api mutex from the inactive set.
 */

#define _API_Mutex_Allocate( _the_mutex ) \
  do { \
    CORE_mutex_Attributes attr =  \
      { CORE_MUTEX_NESTING_IS_ERROR, FALSE, \
        CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT, 0 }; \
    (_the_mutex) = (API_Mutex_Control *) \
      _Objects_Allocate( &_API_Mutex_Information ); \
    _CORE_mutex_Initialize( \
       &(_the_mutex)->Mutex, &attr, CORE_MUTEX_UNLOCKED ); \
  } while (0)

/*
 *  _API_Mutex_Lock
 *
 *  DESCRIPTION:
 *
 *  This routine acquires the specified api mutex.
 */

#define _API_Mutex_Lock( _the_mutex ) \
  do { \
    ISR_Level _level;  \
    _ISR_Disable( _level ); \
    _CORE_mutex_Seize( \
      &(_the_mutex)->Mutex, (_the_mutex)->Object.id, TRUE, 0, (_level) ); \
  } while (0)
  
/*
 *  _API_Mutex_Unlock
 *
 *  DESCRIPTION:
 *
 *  This routine releases the specified api mutex.
 */

#define _API_Mutex_Unlock( _the_mutex ) \
  do { \
    _Thread_Disable_dispatch(); \
      _CORE_mutex_Surrender( \
        &(_the_mutex)->Mutex, (_the_mutex)->Object.id, NULL ); \
    _Thread_Enable_dispatch(); \
  } while (0);

/*XXX when the APIs all use this for allocation and deallocation
 *XXX protection, then they should be renamed and probably moved
 */

SCORE_EXTERN API_Mutex_Control  *_RTEMS_Allocator_Mutex;

#define _RTEMS_Lock_allocator() \
  _API_Mutex_Lock( _RTEMS_Allocator_Mutex )

#define _RTEMS_Unlock_allocator() \
  _API_Mutex_Unlock( _RTEMS_Allocator_Mutex )

/*
 *  There are no inlines for this handler.
 */

#ifndef __RTEMS_APPLICATION__
/* #include <rtems/score/apimutex.inl> */
#endif

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */

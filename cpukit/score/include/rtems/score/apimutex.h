/** 
 *  @file apimutex.h
 *
 *  This include file contains all the constants and structures associated
 *  with the API Mutex Handler.  This handler is used by API level
 *  routines to manage mutual exclusion.
 */

/*
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __API_MUTEX_h
#define __API_MUTEX_h

/**
 *  @defgroup ScoreAPIMutex API Mutex Handler
 *
 *  This group contains functionality which provides mutexes to be used
 *  in the implementation of API functionality. 
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/coremutex.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>

/**
 *  The following defines the control block used to manage each API mutex.
 *  An API Mutex is an aggregration of an Object and a SuperCore Mutex.
 */
typedef struct {
  /** This field allows each API Mutex to be a full-fledged RTEMS object.
  Objects_Control       Object;
  /** This field contains the SuperCore mutex information. */
  CORE_mutex_Control    Mutex;
}   API_Mutex_Control;

/**
 *  The following variable is the information control block used to manage
 *  this class of objects.
 */
SCORE_EXTERN Objects_Information  _API_Mutex_Information;

/**
 *  This routine performs the initialization necessary for this handler.
 *
 *  @param _maximum_mutexes (in) is the maximum number of API mutexes
 *         that may exist at any time
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

/**
 *  This routine allocates an API mutex from the inactive set.
 *
 *  @param _the_mutex (out) will contain the allocated mutex.
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

/**
 *  This routine acquires the specified API mutex.
 *
 *  @param _the_mutex (in) is the mutex to acquire.
 */
#define _API_Mutex_Lock( _the_mutex ) \
  do { \
    ISR_Level _level;  \
    _ISR_Disable( _level ); \
    _CORE_mutex_Seize( \
      &(_the_mutex)->Mutex, (_the_mutex)->Object.id, TRUE, 0, (_level) ); \
  } while (0)

/**
 *  This routine releases the specified API mutex.
 *
 *  @param _the_mutex (in) is the mutex to release.
 */

#define _API_Mutex_Unlock( _the_mutex ) \
  do { \
    _Thread_Disable_dispatch(); \
      _CORE_mutex_Surrender( \
        &(_the_mutex)->Mutex, (_the_mutex)->Object.id, NULL ); \
    _Thread_Enable_dispatch(); \
  } while (0);

/**
 *  This variable points to the API Mutex instance that is used
 *  to protect all memory allocation and deallocation in RTEMS.
 *
 *  @note When the APIs all use this for allocation and deallocation
 *  protection, then this possibly should be renamed and moved to a
 *  higher level in the hierarchy.
 */
SCORE_EXTERN API_Mutex_Control  *_RTEMS_Allocator_Mutex;

/**
 *  This macro locks the RTEMS Allocation Mutex.
 *
 *  @see _RTEMS_Allocator_Mutex
 */
#define _RTEMS_Lock_allocator() \
  _API_Mutex_Lock( _RTEMS_Allocator_Mutex )

/**
 *  This macro unlocks the RTEMS Allocation Mutex.
 *
 *  @see _RTEMS_Allocator_Mutex
 */
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

/*!@}*/

#endif
/*  end of include file */

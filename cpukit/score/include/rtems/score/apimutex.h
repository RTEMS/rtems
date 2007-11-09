/** 
 *  @file  rtems/score/apimutex.h
 *
 *  This include file contains all the constants and structures associated
 *  with the API Mutex Handler.  This handler is used by API level
 *  routines to manage mutual exclusion.
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_APIMUTEX_H
#define _RTEMS_SCORE_APIMUTEX_H

/**
 *  @defgroup ScoreAPIMutex API Mutex Handler
 *
 *  This handler encapsulates functionality which provides mutexes to be used
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
  /** This field allows each API Mutex to be a full-fledged RTEMS object. */
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
 *  @param[in] maximum_mutexes is the maximum number of API mutexes
 *         that may exist at any time
 */
void _API_Mutex_Initialization(
  uint32_t maximum_mutexes
);

/**
 *  This routine allocates an API mutex from the inactive set.
 *
 *  @param[out] the_mutex will contain the allocated mutex.
 */
void _API_Mutex_Allocate(
  API_Mutex_Control **the_mutex
);

/**
 *  This routine acquires the specified API mutex.
 *
 *  @param[in] the_mutex is the mutex to acquire.
 */
void _API_Mutex_Lock(
  API_Mutex_Control *the_mutex
);

/**
 *  This routine releases the specified API mutex.
 *
 *  @param[in] _the_mutex is the mutex to release.
 */
void _API_Mutex_Unlock(
  API_Mutex_Control *the_mutex
);

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

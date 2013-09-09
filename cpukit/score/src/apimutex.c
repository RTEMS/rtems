/**
 * @file
 *
 * @brief Initialization and Allocation for API Mutex Handler
 *
 * @ingroup ScoreAPIMutex
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/apimutex.h>
#include <rtems/score/coremuteximpl.h>
#include <rtems/score/objectimpl.h>

static Objects_Information _API_Mutex_Information;

void _API_Mutex_Initialization(
  uint32_t maximum_mutexes
)
{
  _Objects_Initialize_information(
    &_API_Mutex_Information,     /* object information table */
    OBJECTS_INTERNAL_API,        /* object API */
    OBJECTS_INTERNAL_MUTEXES,    /* object class */
    maximum_mutexes,             /* maximum objects of this class */
    sizeof( API_Mutex_Control ), /* size of this object's control block */
    false,                       /* true if the name is a string */
    0                            /* maximum length of an object name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    true,                        /* true if this is a global object class */
    NULL                         /* Proxy extraction support callout */
#endif
  );
}

void _API_Mutex_Allocate(
  API_Mutex_Control **the_mutex
)
{
  API_Mutex_Control *mutex;

  CORE_mutex_Attributes attr =  {
    CORE_MUTEX_NESTING_ACQUIRES,
    false,
    CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT,
    0
  };

  mutex = (API_Mutex_Control *) _Objects_Allocate( &_API_Mutex_Information );

  _CORE_mutex_Initialize( &mutex->Mutex, NULL, &attr, CORE_MUTEX_UNLOCKED );

  _Objects_Open_u32( &_API_Mutex_Information, &mutex->Object, 1 );

  *the_mutex = mutex;
}

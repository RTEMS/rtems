/**
 * @file
 * 
 * @brief Private Inlined Routines for POSIX Semaphores
 *
 * This include file contains the static inline implementation of the private 
 * inlined routines for POSIX Semaphores.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_SEMAPHOREIMPL_H
#define _RTEMS_POSIX_SEMAPHOREIMPL_H

#include <rtems/posix/semaphore.h>
#include <rtems/posix/posixapi.h>
#include <rtems/score/coresemimpl.h>

#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */

POSIX_EXTERN Objects_Information  _POSIX_Semaphore_Information;

extern const int
  _POSIX_Semaphore_Return_codes[CORE_SEMAPHORE_STATUS_LAST + 1];

/*
 *  _POSIX_Semaphore_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */

void _POSIX_Semaphore_Manager_initialization(void);

/*
 *  _POSIX_Semaphore_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a semaphore control block from
 *  the inactive chain of free semaphore control blocks.
 */

RTEMS_INLINE_ROUTINE POSIX_Semaphore_Control *_POSIX_Semaphore_Allocate( void );

/*
 *  _POSIX_Semaphore_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a semaphore control block to the
 *  inactive chain of free semaphore control blocks.
 */

RTEMS_INLINE_ROUTINE void _POSIX_Semaphore_Free (
  POSIX_Semaphore_Control *the_semaphore
);

/*
 *  _POSIX_Semaphore_Get
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

RTEMS_INLINE_ROUTINE POSIX_Semaphore_Control *_POSIX_Semaphore_Get (
  sem_t        *id,
  Objects_Locations *location
);

/*
 *  _POSIX_Semaphore_Create_support
 *
 *  DESCRIPTION:
 *
 *  This routine supports the sem_init and sem_open routines.
 */

int _POSIX_Semaphore_Create_support(
  const char                *name,
  size_t                     name_len,
  int                        pshared,
  unsigned int               value,
  POSIX_Semaphore_Control  **the_sem
);

/**
 * @brief POSIX delete a semaphore. 
 *
 * DESCRIPTION:
 *
 * This routine supports the sem_close and sem_unlink routines.
 */
void _POSIX_Semaphore_Delete(
  POSIX_Semaphore_Control *the_semaphore
);

/**
 * @brief POSIX semaphore wait support.
 *
 * DESCRIPTION:
 *
 * This routine supports the sem_wait, sem_trywait, and sem_timedwait
 * services.
 */
int _POSIX_Semaphore_Wait_support(
  sem_t               *sem,
  bool                 blocking,
  Watchdog_Interval    timeout
);

/*
 *  _POSIX_Semaphore_Translate_core_semaphore_return_code
 *
 *  DESCRIPTION:
 *
 *  A support routine which converts core semaphore status codes into the
 *  appropriate POSIX status values.
 */

RTEMS_INLINE_ROUTINE int
_POSIX_Semaphore_Translate_core_semaphore_return_code(
  CORE_semaphore_Status  the_semaphore_status
)
{
  /*
   *  Internal consistency check for bad status from SuperCore
   */
  #if defined(RTEMS_DEBUG)
    if ( the_semaphore_status > CORE_SEMAPHORE_STATUS_LAST )
      return EINVAL;
  #endif
  return _POSIX_Semaphore_Return_codes[the_semaphore_status];
}
 
/*
 *  _POSIX_Semaphore_Allocate
 */
 
RTEMS_INLINE_ROUTINE POSIX_Semaphore_Control *_POSIX_Semaphore_Allocate( void )
{
  return (POSIX_Semaphore_Control *)
    _Objects_Allocate( &_POSIX_Semaphore_Information );
}
 
/*
 *  _POSIX_Semaphore_Free
 */
 
RTEMS_INLINE_ROUTINE void _POSIX_Semaphore_Free (
  POSIX_Semaphore_Control *the_semaphore
)
{
  _Objects_Free( &_POSIX_Semaphore_Information, &the_semaphore->Object );
}
 
/*
 *  _POSIX_Semaphore_Namespace_remove
 */
 
RTEMS_INLINE_ROUTINE void _POSIX_Semaphore_Namespace_remove (
  POSIX_Semaphore_Control *the_semaphore
)
{
  _Objects_Namespace_remove( 
    &_POSIX_Semaphore_Information, &the_semaphore->Object );
}
 


/*
 *  _POSIX_Semaphore_Get
 */
RTEMS_INLINE_ROUTINE POSIX_Semaphore_Control *_POSIX_Semaphore_Get (
  sem_t             *id,
  Objects_Locations *location
)
{
  return (POSIX_Semaphore_Control *)
    _Objects_Get( &_POSIX_Semaphore_Information, (Objects_Id)*id, location );
}

/**
 * @see _POSIX_Name_to_id().
 */
RTEMS_INLINE_ROUTINE int _POSIX_Semaphore_Name_to_id(
  const char *name,
  Objects_Id *id,
  size_t     *len
)
{
  return _POSIX_Name_to_id( &_POSIX_Semaphore_Information, name, id, len );
}

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */

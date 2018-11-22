/**
 * @file
 * 
 * @brief Private Inlined Routines for POSIX Semaphores
 *
 * This include file contains the static inline implementation of the private 
 * inlined routines for POSIX Semaphores.
 */

/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_SEMAPHOREIMPL_H
#define _RTEMS_POSIX_SEMAPHOREIMPL_H

#include <rtems/posix/semaphore.h>
#include <rtems/posix/posixapi.h>
#include <rtems/score/semaphoreimpl.h>
#include <rtems/seterr.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This is a random number used to check if a semaphore object is
 * properly initialized.
 */
#define POSIX_SEMAPHORE_MAGIC 0x5d367fe7UL

RTEMS_INLINE_ROUTINE POSIX_Semaphore_Control *
  _POSIX_Semaphore_Allocate_unprotected( void )
{
  return (POSIX_Semaphore_Control *)
    _Objects_Allocate_unprotected( &_POSIX_Semaphore_Information );
}

/**
 *  @brief POSIX Semaphore Free
 *
 *  This routine frees a semaphore control block to the
 *  inactive chain of free semaphore control blocks.
 */
RTEMS_INLINE_ROUTINE void _POSIX_Semaphore_Free (
  POSIX_Semaphore_Control *the_semaphore
)
{
  _Objects_Free( &_POSIX_Semaphore_Information, &the_semaphore->Object );
}

RTEMS_INLINE_ROUTINE POSIX_Semaphore_Control *_POSIX_Semaphore_Get(
  sem_t *sem
)
{
  return RTEMS_CONTAINER_OF( sem, POSIX_Semaphore_Control, Semaphore );
}

RTEMS_INLINE_ROUTINE bool _POSIX_Semaphore_Is_named( const sem_t *sem )
{
  return sem->_Semaphore._Queue._name != NULL;
}

RTEMS_INLINE_ROUTINE bool _POSIX_Semaphore_Is_busy( const sem_t *sem )
{
  return sem->_Semaphore._Queue._heads != NULL;
}

RTEMS_INLINE_ROUTINE void _POSIX_Semaphore_Initialize(
  sem_t        *sem,
  const char   *name,
  unsigned int  value
)
{
  sem->_flags = (uintptr_t) sem ^ POSIX_SEMAPHORE_MAGIC;
  _Semaphore_Initialize_named( &sem->_Semaphore, name, value );
}

RTEMS_INLINE_ROUTINE void _POSIX_Semaphore_Destroy( sem_t *sem )
{
  sem->_flags = 0;
  _Semaphore_Destroy( &sem->_Semaphore );
}

/**
 *  @brief POSIX Semaphore Delete
 *
 * This routine supports the sem_close and sem_unlink routines.
 */
void _POSIX_Semaphore_Delete( POSIX_Semaphore_Control *the_semaphore );

/**
 *  @brief POSIX Semaphore Namespace Remove
 */
RTEMS_INLINE_ROUTINE void _POSIX_Semaphore_Namespace_remove (
  POSIX_Semaphore_Control *the_semaphore
)
{
  _Objects_Namespace_remove_string(
    &_POSIX_Semaphore_Information,
    &the_semaphore->Object
  );
}

RTEMS_INLINE_ROUTINE POSIX_Semaphore_Control *_POSIX_Semaphore_Get_by_name(
  const char                *name,
  size_t                    *name_length_p,
  Objects_Get_by_name_error *error
)
{
  return (POSIX_Semaphore_Control *) _Objects_Get_by_name(
    &_POSIX_Semaphore_Information,
    name,
    name_length_p,
    error
  );
}

#define POSIX_SEMAPHORE_VALIDATE_OBJECT( sem ) \
  do { \
    if ( \
      ( sem ) == NULL \
        || ( (uintptr_t) ( sem ) ^ POSIX_SEMAPHORE_MAGIC ) != ( sem )->_flags \
    ) { \
      rtems_set_errno_and_return_minus_one( EINVAL ); \
    } \
  } while ( 0 )

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */

/* SPDX-License-Identifier: BSD-2-Clause */

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
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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

static inline POSIX_Semaphore_Control *
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
static inline void _POSIX_Semaphore_Free (
  POSIX_Semaphore_Control *the_semaphore
)
{
  _Objects_Free( &_POSIX_Semaphore_Information, &the_semaphore->Object );
}

static inline POSIX_Semaphore_Control *_POSIX_Semaphore_Get(
  sem_t *sem
)
{
  return RTEMS_CONTAINER_OF( sem, POSIX_Semaphore_Control, Semaphore );
}

static inline bool _POSIX_Semaphore_Is_named( const sem_t *sem )
{
  return sem->_Semaphore._Queue._name != NULL;
}

static inline bool _POSIX_Semaphore_Is_busy( const sem_t *sem )
{
  return sem->_Semaphore._Queue._heads != NULL;
}

static inline void _POSIX_Semaphore_Initialize(
  sem_t        *sem,
  const char   *name,
  unsigned int  value
)
{
  sem->_flags = (uintptr_t) sem ^ POSIX_SEMAPHORE_MAGIC;
  _Semaphore_Initialize_named( &sem->_Semaphore, name, value );
}

static inline void _POSIX_Semaphore_Destroy( sem_t *sem )
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
static inline void _POSIX_Semaphore_Namespace_remove (
  POSIX_Semaphore_Control *the_semaphore
)
{
  _Objects_Namespace_remove_string(
    &_POSIX_Semaphore_Information,
    &the_semaphore->Object
  );
}

static inline POSIX_Semaphore_Control *_POSIX_Semaphore_Get_by_name(
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

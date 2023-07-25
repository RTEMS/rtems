/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSAPISelfContainedObjects
 *
 * @brief This header file provides the API of
 *   @ref RTEMSAPISelfContainedObjects.
 */

/*
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_THREAD_H
#define _RTEMS_THREAD_H

#include <sys/lock.h>
#include <errno.h>
#include <stdint.h>

__BEGIN_DECLS

/* Temporarily defined, will be shipped with a Newlib update */
int _Semaphore_Wait_timed_ticks(struct _Semaphore_Control *, __uint32_t);

/* Temporarily defined, will be shipped with a Newlib update */
int _Semaphore_Try_wait(struct _Semaphore_Control *);

/* Temporarily defined, will be shipped with a Newlib update */
void _Semaphore_Post_binary(struct _Semaphore_Control *);

typedef struct _Mutex_Control rtems_mutex;

/**
 * @defgroup RTEMSAPISelfContainedObjects Self-Contained Objects
 *
 * @ingroup RTEMSAPI
 *
 * @brief This group contains the self-contained objects API.
 *
 * @{
 */

#define RTEMS_MUTEX_INITIALIZER( name ) _MUTEX_NAMED_INITIALIZER( name )

static __inline void rtems_mutex_init( rtems_mutex *mutex, const char *name )
{
  _Mutex_Initialize_named( mutex, name );
}

static __inline const char *rtems_mutex_get_name( const rtems_mutex *mutex )
{
  return mutex->_Queue._name;
}

static __inline void rtems_mutex_set_name( rtems_mutex *mutex, const char *name )
{
  mutex->_Queue._name = name;
}

static __inline void rtems_mutex_lock( rtems_mutex *mutex )
{
  _Mutex_Acquire( mutex );
}

static __inline int rtems_mutex_try_lock( rtems_mutex *mutex )
{
  return _Mutex_Try_acquire( mutex );
}

static __inline void rtems_mutex_unlock( rtems_mutex *mutex )
{
  _Mutex_Release( mutex );
}

static __inline void rtems_mutex_destroy( rtems_mutex *mutex )
{
  _Mutex_Destroy( mutex );
}

typedef struct _Mutex_recursive_Control rtems_recursive_mutex;

#define RTEMS_RECURSIVE_MUTEX_INITIALIZER( name ) \
  _MUTEX_RECURSIVE_NAMED_INITIALIZER( name )

static __inline void rtems_recursive_mutex_init(
  rtems_recursive_mutex *mutex, const char *name
)
{
  _Mutex_recursive_Initialize_named( mutex, name );
}

static __inline const char *rtems_recursive_mutex_get_name(
  const rtems_recursive_mutex *mutex
)
{
  return mutex->_Mutex._Queue._name;
}

static __inline void rtems_recursive_mutex_set_name(
  rtems_recursive_mutex *mutex, const char *name
)
{
  mutex->_Mutex._Queue._name = name;
}

static __inline void rtems_recursive_mutex_lock(
  rtems_recursive_mutex *mutex
)
{
  _Mutex_recursive_Acquire( mutex );
}

static __inline int rtems_recursive_mutex_try_lock(
  rtems_recursive_mutex *mutex
)
{
  return _Mutex_recursive_Try_acquire( mutex );
}

static __inline void rtems_recursive_mutex_unlock(
  rtems_recursive_mutex *mutex
)
{
  _Mutex_recursive_Release( mutex );
}

static __inline void rtems_recursive_mutex_destroy(
  rtems_recursive_mutex *mutex
)
{
  _Mutex_recursive_Destroy( mutex );
}

typedef struct _Condition_Control rtems_condition_variable;

#define RTEMS_CONDITION_VARIABLE_INITIALIZER( name ) \
  _CONDITION_NAMED_INITIALIZER( name )

static __inline void rtems_condition_variable_init(
  rtems_condition_variable *condition_variable,
  const char               *name
)
{
  _Condition_Initialize_named( condition_variable, name );
}

static __inline const char *rtems_condition_variable_get_name(
  const rtems_condition_variable *condition_variable
)
{
  return condition_variable->_Queue._name;
}

static __inline void rtems_condition_variable_set_name(
  rtems_condition_variable *condition_variable,
  const char               *name
)
{
  condition_variable->_Queue._name = name;
}

static __inline void rtems_condition_variable_wait(
  rtems_condition_variable *condition_variable,
  rtems_mutex *mutex
)
{
  _Condition_Wait( condition_variable, mutex );
}

static __inline void rtems_condition_variable_signal(
  rtems_condition_variable *condition_variable
)
{
  _Condition_Signal( condition_variable );
}

static __inline void rtems_condition_variable_broadcast(
  rtems_condition_variable *condition_variable
)
{
  _Condition_Broadcast( condition_variable );
}

static __inline void rtems_condition_variable_destroy(
  rtems_condition_variable *condition_variable
)
{
  _Condition_Destroy( condition_variable );
}

typedef struct _Semaphore_Control rtems_counting_semaphore;

#define RTEMS_COUNTING_SEMAPHORE_INITIALIZER( name, value ) \
  _SEMAPHORE_NAMED_INITIALIZER( name, value )

static __inline void rtems_counting_semaphore_init(
  rtems_counting_semaphore *counting_semaphore,
  const char               *name,
  unsigned int              value
)
{
  _Semaphore_Initialize_named( counting_semaphore, name, value );
}

static __inline const char *rtems_counting_semaphore_get_name(
  const rtems_counting_semaphore *counting_semaphore
)
{
  return counting_semaphore->_Queue._name;
}

static __inline void rtems_counting_semaphore_set_name(
  rtems_counting_semaphore *counting_semaphore,
  const char               *name
)
{
  counting_semaphore->_Queue._name = name;
}

static __inline void rtems_counting_semaphore_wait(
  rtems_counting_semaphore *counting_semaphore
)
{
  _Semaphore_Wait( counting_semaphore );
}

static __inline void rtems_counting_semaphore_post(
  rtems_counting_semaphore *counting_semaphore
)
{
  _Semaphore_Post( counting_semaphore );
}

static __inline void rtems_counting_semaphore_destroy(
  rtems_counting_semaphore *counting_semaphore
)
{
  _Semaphore_Destroy( counting_semaphore );
}

typedef struct {
  struct _Semaphore_Control Semaphore;
} rtems_binary_semaphore;

#define RTEMS_BINARY_SEMAPHORE_INITIALIZER( name ) \
  { _SEMAPHORE_NAMED_INITIALIZER( name, 0 ) }

static __inline void rtems_binary_semaphore_init(
  rtems_binary_semaphore *binary_semaphore,
  const char             *name
)
{
  _Semaphore_Initialize_named( &binary_semaphore->Semaphore, name, 0 );
}

static __inline const char *rtems_binary_semaphore_get_name(
  const rtems_binary_semaphore *binary_semaphore
)
{
  return binary_semaphore->Semaphore._Queue._name;
}

static __inline void rtems_binary_semaphore_set_name(
  rtems_binary_semaphore *binary_semaphore,
  const char             *name
)
{
  binary_semaphore->Semaphore._Queue._name = name;
}

static __inline void rtems_binary_semaphore_wait(
  rtems_binary_semaphore *binary_semaphore
)
{
  _Semaphore_Wait( &binary_semaphore->Semaphore );
}

static __inline int rtems_binary_semaphore_wait_timed_ticks(
  rtems_binary_semaphore *binary_semaphore,
  uint32_t                ticks
)
{
  return _Semaphore_Wait_timed_ticks( &binary_semaphore->Semaphore, ticks );
}

static __inline int rtems_binary_semaphore_try_wait(
  rtems_binary_semaphore *binary_semaphore
)
{
  return _Semaphore_Try_wait( &binary_semaphore->Semaphore );
}

static __inline void rtems_binary_semaphore_post(
  rtems_binary_semaphore *binary_semaphore
)
{
  _Semaphore_Post_binary( &binary_semaphore->Semaphore );
}

static __inline void rtems_binary_semaphore_destroy(
  rtems_binary_semaphore *binary_semaphore
)
{
  _Semaphore_Destroy( &binary_semaphore->Semaphore );
}

/** @} */

__END_DECLS

#endif /* _RTEMS_THREAD_H */

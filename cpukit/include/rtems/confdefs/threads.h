/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSApplicationConfiguration
 *
 * @brief Evaluate Thread Configuration Options
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

#ifndef _RTEMS_CONFDEFS_THREADS_H
#define _RTEMS_CONFDEFS_THREADS_H

#ifndef __CONFIGURATION_TEMPLATE_h
#error "Do not include this file directly, use <rtems/confdefs.h> instead"
#endif

#ifdef CONFIGURE_INIT

#include <rtems/confdefs/bdbuf.h>
#include <rtems/confdefs/extensions.h>
#include <rtems/confdefs/percpu.h>
#include <rtems/confdefs/scheduler.h>
#include <rtems/confdefs/unlimited.h>
#include <rtems/score/thread.h>
#include <rtems/posix/threadsup.h>
#include <rtems/rtems/tasksdata.h>

#ifndef CONFIGURE_MAXIMUM_TASKS
  #define CONFIGURE_MAXIMUM_TASKS 0
#endif

#define _CONFIGURE_TASKS ( CONFIGURE_MAXIMUM_TASKS + _CONFIGURE_LIBBLOCK_TASKS )

#ifndef CONFIGURE_MAXIMUM_POSIX_THREADS
  #define CONFIGURE_MAXIMUM_POSIX_THREADS 0
#endif

#if CONFIGURE_MAXIMUM_POSIX_THREADS > 0
  #include <rtems/posix/pthread.h>
#endif

#if !defined(CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION) \
  && CONFIGURE_MAXIMUM_TASKS == 0 \
  && CONFIGURE_MAXIMUM_POSIX_THREADS == 0
  #error "You must define one of CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION, CONFIGURE_MAXIMUM_TASKS, and CONFIGURE_MAXIMUM_POSIX_THREADS"
#endif

#if !defined(CONFIGURE_RTEMS_INIT_TASKS_TABLE) \
  && !defined(CONFIGURE_POSIX_INIT_THREAD_TABLE) \
  && !defined(CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION)
  #error "You must define one of CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION, CONFIGURE_RTEMS_INIT_TASKS_TABLE, and CONFIGURE_POSIX_INIT_THREAD_TABLE"
#endif

#ifndef CONFIGURE_MAXIMUM_THREAD_NAME_SIZE
  #define CONFIGURE_MAXIMUM_THREAD_NAME_SIZE THREAD_DEFAULT_MAXIMUM_NAME_SIZE
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef union {
  Scheduler_Node Base;
  #ifdef CONFIGURE_SCHEDULER_CBS
    Scheduler_CBS_Node CBS;
  #endif
  #ifdef CONFIGURE_SCHEDULER_EDF
    Scheduler_EDF_Node EDF;
  #endif
  #ifdef CONFIGURE_SCHEDULER_EDF_SMP
    Scheduler_EDF_SMP_Node EDF_SMP;
  #endif
  #ifdef CONFIGURE_SCHEDULER_PRIORITY
    Scheduler_priority_Node Priority;
  #endif
  #ifdef CONFIGURE_SCHEDULER_SIMPLE_SMP
    Scheduler_SMP_Node Simple_SMP;
  #endif
  #ifdef CONFIGURE_SCHEDULER_PRIORITY_SMP
    Scheduler_priority_SMP_Node Priority_SMP;
  #endif
  #ifdef CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP
    Scheduler_priority_affinity_SMP_Node Priority_affinity_SMP;
  #endif
  #ifdef CONFIGURE_SCHEDULER_STRONG_APA
    Scheduler_strong_APA_Node Strong_APA;
  #endif
  #ifdef CONFIGURE_SCHEDULER_USER_PER_THREAD
    CONFIGURE_SCHEDULER_USER_PER_THREAD User;
  #endif
} Configuration_Scheduler_node;

#ifdef RTEMS_SMP
  const size_t _Scheduler_Node_size = sizeof( Configuration_Scheduler_node );
#endif

const size_t _Thread_Maximum_name_size = CONFIGURE_MAXIMUM_THREAD_NAME_SIZE;

struct Thread_Configured_control {
  Thread_Control Control;
  #if CONFIGURE_MAXIMUM_USER_EXTENSIONS > 0
    void *extensions[ CONFIGURE_MAXIMUM_USER_EXTENSIONS + 1 ];
  #endif
  Configuration_Scheduler_node Scheduler_nodes[ _CONFIGURE_SCHEDULER_COUNT ];
  RTEMS_API_Control API_RTEMS;
  #ifdef RTEMS_POSIX_API
    POSIX_API_Control API_POSIX;
  #endif
  #if CONFIGURE_MAXIMUM_THREAD_NAME_SIZE > 1
    char name[ CONFIGURE_MAXIMUM_THREAD_NAME_SIZE ];
  #endif
  #ifdef _CONFIGURE_ENABLE_NEWLIB_REENTRANCY
    struct _reent Newlib;
  #else
    struct { /* Empty */ } Newlib;
  #endif
};

const Thread_Control_add_on _Thread_Control_add_ons[] = {
  {
    offsetof( Thread_Configured_control, Control.Scheduler.nodes ),
    offsetof( Thread_Configured_control, Scheduler_nodes )
  }, {
    offsetof(
      Thread_Configured_control,
      Control.API_Extensions[ THREAD_API_RTEMS ]
    ),
    offsetof( Thread_Configured_control, API_RTEMS )
  }, {
    offsetof(
      Thread_Configured_control,
      Control.libc_reent
    ),
    offsetof( Thread_Configured_control, Newlib )
  }
  #if CONFIGURE_MAXIMUM_THREAD_NAME_SIZE > 1
    , {
      offsetof(
        Thread_Configured_control,
        Control.Join_queue.Queue.name
      ),
      offsetof( Thread_Configured_control, name )
    }
  #endif
  #ifdef RTEMS_POSIX_API
    , {
      offsetof(
        Thread_Configured_control,
        Control.API_Extensions[ THREAD_API_POSIX ]
      ),
      offsetof( Thread_Configured_control, API_POSIX )
    }
  #endif
};

const size_t _Thread_Control_add_on_count =
  RTEMS_ARRAY_SIZE( _Thread_Control_add_ons );

#ifdef RTEMS_SMP
  struct Thread_queue_Configured_heads {
    Thread_queue_Heads Heads;
      Thread_queue_Priority_queue Priority[ _CONFIGURE_SCHEDULER_COUNT ];
  };

  const size_t _Thread_queue_Heads_size =
    sizeof( Thread_queue_Configured_heads );
#endif

const size_t _Thread_Initial_thread_count =
  rtems_resource_maximum_per_allocation( _CONFIGURE_TASKS ) +
  rtems_resource_maximum_per_allocation( CONFIGURE_MAXIMUM_POSIX_THREADS );

#if defined(RTEMS_MULTIPROCESSING) && defined(CONFIGURE_MP_APPLICATION)
  #define _CONFIGURE_MPCI_RECEIVE_SERVER_COUNT 1
#else
  #define _CONFIGURE_MPCI_RECEIVE_SERVER_COUNT 0
#endif

THREAD_INFORMATION_DEFINE(
  _Thread,
  OBJECTS_INTERNAL_API,
  OBJECTS_INTERNAL_THREADS,
  _CONFIGURE_MAXIMUM_PROCESSORS + _CONFIGURE_MPCI_RECEIVE_SERVER_COUNT
);

#if _CONFIGURE_TASKS > 0
  THREAD_INFORMATION_DEFINE(
    _RTEMS_tasks,
    OBJECTS_CLASSIC_API,
    OBJECTS_RTEMS_TASKS,
    _CONFIGURE_TASKS
  );
#endif

#if CONFIGURE_MAXIMUM_POSIX_THREADS > 0
  const size_t _POSIX_Threads_Minimum_stack_size =
    CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE;

  THREAD_INFORMATION_DEFINE(
    _POSIX_Threads,
    OBJECTS_POSIX_API,
    OBJECTS_POSIX_THREADS,
    CONFIGURE_MAXIMUM_POSIX_THREADS
  );
#endif

#ifdef __cplusplus
}
#endif

#endif /* CONFIGURE_INIT */

#endif /* _RTEMS_CONFDEFS_THREADS_H */

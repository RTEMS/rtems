/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSApplicationConfiguration
 *
 * @brief Evaluate MPCI Configuration Options
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

#ifndef _RTEMS_CONFDEFS_MPCI_H
#define _RTEMS_CONFDEFS_MPCI_H

#ifndef __CONFIGURATION_TEMPLATE_h
#error "Do not include this file directly, use <rtems/confdefs.h> instead"
#endif

#ifdef CONFIGURE_INIT

#ifdef RTEMS_MULTIPROCESSING

#ifdef CONFIGURE_MP_APPLICATION

#include <rtems/confdefs/threads.h>

#ifndef CONFIGURE_EXTRA_MPCI_RECEIVE_SERVER_STACK
  #define CONFIGURE_EXTRA_MPCI_RECEIVE_SERVER_STACK 0
#endif

#ifndef CONFIGURE_MP_NODE_NUMBER
  #define CONFIGURE_MP_NODE_NUMBER NODE_NUMBER
#endif

#ifndef CONFIGURE_MP_MAXIMUM_NODES
  #define CONFIGURE_MP_MAXIMUM_NODES 2
#endif

#ifndef CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS
  #define CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS 32
#endif

#ifndef CONFIGURE_MP_MAXIMUM_PROXIES
  #define CONFIGURE_MP_MAXIMUM_PROXIES 32
#endif

#ifndef CONFIGURE_MP_MPCI_TABLE_POINTER
  #include <mpci.h>

  #define CONFIGURE_MP_MPCI_TABLE_POINTER &MPCI_table
#endif

#if CONFIGURE_MP_NODE_NUMBER < 1
  #error "CONFIGURE_MP_NODE_NUMBER must be greater than or equal to one"
#endif

#if CONFIGURE_MP_NODE_NUMBER > CONFIGURE_MP_MAXIMUM_NODES
  #error "CONFIGURE_MP_NODE_NUMBER must be less than or equal to CONFIGURE_MP_MAXIMUM_NODES"
#endif

#ifdef __cplusplus
extern "C" {
#endif

Objects_MP_Control _Objects_MP_Controls[
  CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS
];

struct Thread_Configured_proxy_control {
  Thread_Proxy_control Control;
  Thread_queue_Configured_heads Heads;
};

static Thread_Configured_proxy_control _Thread_MP_Configured_proxies[
  CONFIGURE_MP_MAXIMUM_PROXIES
];

Thread_Configured_proxy_control * const _Thread_MP_Proxies =
  &_Thread_MP_Configured_proxies[ 0 ];

const MPCI_Configuration _MPCI_Configuration = {
  CONFIGURE_MP_NODE_NUMBER,
  CONFIGURE_MP_MAXIMUM_NODES,
  CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS,
  CONFIGURE_MP_MAXIMUM_PROXIES,
  CONFIGURE_EXTRA_MPCI_RECEIVE_SERVER_STACK,
  CONFIGURE_MP_MPCI_TABLE_POINTER
};

char _MPCI_Receive_server_stack[
  CONFIGURE_MINIMUM_TASK_STACK_SIZE
    + CONFIGURE_EXTRA_MPCI_RECEIVE_SERVER_STACK
    + CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK
    + CPU_ALL_TASKS_ARE_FP * CONTEXT_FP_SIZE
] RTEMS_ALIGNED( CPU_INTERRUPT_STACK_ALIGNMENT )
RTEMS_SECTION( ".rtemsstack.mpci" );

#ifdef __cplusplus
}
#endif

#endif /* CONFIGURE_MP_APPLICATION */

#else /* RTEMS_MULTIPROCESSING */

#ifdef CONFIGURE_MP_APPLICATION
  #error "CONFIGURE_MP_APPLICATION must not be defined if multiprocessing is disabled"
#endif

#endif /* RTEMS_MULTIPROCESSING */

#endif /* CONFIGURE_INIT */

#endif /* _RTEMS_CONFDEFS_MPCI_H */

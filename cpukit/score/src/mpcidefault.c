/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreMPCI
 *
 * @brief This source file contains a definition of
 *   ::_Objects_MP_Controls, ::_Thread_MP_Proxies, and
 *   ::_MPCI_Receive_server_stack for an application configuration with only
 *   the local MPCI node and no MP objects.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/mpci.h>
#include <rtems/score/context.h>
#include <rtems/score/objectdata.h>
#include <rtems/score/stack.h>
#include <rtems/score/thread.h>

Objects_MP_Control _Objects_MP_Controls[ 0 ];

Thread_Configured_proxy_control * const _Thread_MP_Proxies;

const MPCI_Configuration _MPCI_Configuration = {
  1,                        /* local node number */
  1,                        /* maximum number nodes in system */
  0,                        /* maximum number global objects */
  0,                        /* maximum number proxies */
  STACK_MINIMUM_SIZE,       /* MPCI receive server stack size */
  NULL                      /* pointer to MPCI address table */
};

char _MPCI_Receive_server_stack[
  STACK_MINIMUM_SIZE
    + CPU_MPCI_RECEIVE_SERVER_EXTRA_STACK
    + CPU_ALL_TASKS_ARE_FP * CONTEXT_FP_SIZE
] RTEMS_ALIGNED( CPU_INTERRUPT_STACK_ALIGNMENT )
RTEMS_SECTION( ".rtemsstack.mpci" );

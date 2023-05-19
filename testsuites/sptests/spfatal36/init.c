/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2022 embedded brains GmbH & Co. KG
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

#include "../spfatal_support/spfatal.h"

#include <rtems/sysinit.h>
#include <rtems/score/heapimpl.h>
#include <rtems/score/wkspace.h>

#define FATAL_ERROR_TEST_NAME "36"

#define FATAL_ERROR_DESCRIPTION "failure in idle task storage allocation"

#define FATAL_ERROR_EXPECTED_SOURCE INTERNAL_ERROR_CORE

#define FATAL_ERROR_EXPECTED_ERROR \
  INTERNAL_ERROR_NO_MEMORY_FOR_IDLE_TASK_STORAGE

static void force_error( void )
{
  RTEMS_UNREACHABLE();
}

static void empty_workspace( void )
{
  (void) _Heap_Greedy_allocate( &_Workspace_Area, NULL, 0 );
}

RTEMS_SYSINIT_ITEM(
  empty_workspace,
  RTEMS_SYSINIT_IDLE_THREADS,
  RTEMS_SYSINIT_ORDER_FIRST
);

#include "../spfatal_support/spfatalimpl.h"

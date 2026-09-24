/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup TestsuitesUserext
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

#include "tc-userext.h"

#include <rtems/test.h>

/**
 * @defgroup TestsuitesUserext spec:/testsuites/userext
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This validation test suite contains a test cases related to the
 *   invocation of user extensions.
 *
 * @{
 */

const char rtems_test_name[] = "TestsuitesUserext";

#define CONFIGURE_MAXIMUM_PROCESSORS 2

#define CONFIGURE_INITIAL_EXTENSIONS                 \
  { .thread_begin = ThreadBeginExtension0,           \
    .thread_create = ThreadCreateExtension0,         \
    .thread_delete = ThreadDeleteExtension0,         \
    .thread_exitted = ThreadExittedExtension0,       \
    .thread_restart = ThreadRestartExtension0,       \
    .thread_start = ThreadStartExtension0,           \
    .thread_switch = ThreadSwitchExtension0,         \
    .thread_terminate = ThreadTerminateExtension0 }, \
  {                                                  \
    .thread_begin = ThreadBeginExtension1,           \
    .thread_create = ThreadCreateExtension1,         \
    .thread_delete = ThreadDeleteExtension1,         \
    .thread_exitted = ThreadExittedExtension1,       \
    .thread_restart = ThreadRestartExtension1,       \
    .thread_start = ThreadStartExtension1,           \
    .thread_switch = ThreadSwitchExtension1,         \
    .thread_terminate = ThreadTerminateExtension1    \
  }

#define CONFIGURE_IDLE_TASK_BODY IdleBody

#include "ts-default.h"

/** @} */

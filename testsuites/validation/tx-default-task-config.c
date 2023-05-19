/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This source file contains the definition of ::DefaultTaskConfig.
 */

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

#include "tx-support.h"
#include "ts-config.h"

#define TASK_ATTRIBUTES RTEMS_DEFAULT_ATTRIBUTES

RTEMS_ALIGNED( RTEMS_TASK_STORAGE_ALIGNMENT )
static char DefaultTaskStorage[
  RTEMS_TASK_STORAGE_SIZE(
    TEST_MAXIMUM_TLS_SIZE + TEST_MINIMUM_STACK_SIZE,
    TASK_ATTRIBUTES
  )
];

const rtems_task_config DefaultTaskConfig = {
  .name = rtems_build_name( 'D', 'T', 'S', 'K' ),
  .initial_priority = 1,
  .storage_area = DefaultTaskStorage,
  .storage_size = sizeof( DefaultTaskStorage ),
  .maximum_thread_local_storage_size = TEST_MAXIMUM_TLS_SIZE,
  .initial_modes = RTEMS_DEFAULT_MODES,
  .attributes = TASK_ATTRIBUTES
};

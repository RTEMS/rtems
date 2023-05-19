/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup TestsuitesFatalSchedulerRequiresExactlyOneProcessor
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tr-fatal-scheduler-requires-exactly-one-processor.h"
#include "ts-config.h"

#include <rtems/test.h>

/**
 * @defgroup TestsuitesFatalSchedulerRequiresExactlyOneProcessor \
 *   spec:/testsuites/fatal-scheduler-requires-exactly-one-processor
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This validation test suite uses an application configuration which
 *   triggers a fatal error during system initialization.
 *
 * @{
 */

const char rtems_test_name[] = "TestsuitesFatalSchedulerRequiresExactlyOneProcessor";

#define FATAL_SYSINIT_RUN \
  ScoreSmpValFatalSchedulerRequiresExactlyOneProcessor_Run

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS 2

#include <rtems/score/schedulerpriority.h>

#define CONFIGURE_SCHEDULER_PRIORITY

#include <rtems/scheduler.h>

RTEMS_SCHEDULER_PRIORITY( a, 256 );

#define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
  RTEMS_SCHEDULER_TABLE_PRIORITY( a, TEST_SCHEDULER_A_NAME )

#define CONFIGURE_SCHEDULER_ASSIGNMENTS \
  RTEMS_SCHEDULER_ASSIGN( 0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY ), \
  RTEMS_SCHEDULER_ASSIGN( 0, RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY )

#include "ts-fatal-sysinit.h"

/** @} */

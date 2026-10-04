/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This header file provides the functions to test the
 *   @ref RTEMSScoreThreadQueue.
 *
 * The thread queue test framework validates directives which block on a thread
 * queue, for example a semaphore obtain, a mutex seize, a condition variable
 * wait, or a futex wait.  The runner task controls up to nine worker tasks
 * through the events of TQEvent.  A worker processes its events in a fixed
 * order and sets its done flag at the end.
 *
 * A test case validates a directive with these steps:
 *
 * - Embed a TQContext, TQSemContext, TQMtxContext, or TQCondContext in the
 *   context of the test case.
 *
 * - In the setup, set the discipline, the wait behaviour, and the handlers.
 *   The enqueue handler calls the directive under test and returns its status
 *   built with STATUS_BUILD().  Use TQConvertStatusPOSIX() as the status
 *   converter of a POSIX directive and TQConvertStatusClassic() for a Classic
 *   API directive.
 *
 * - Call TQInitialize() in the setup and TQDestroy() in the teardown.
 *
 * - In a post-condition check, call a reusable test run, for example
 *   ScoreTqReqEnqueueFifo_Run().  The test runs drive the workers through the
 *   handlers and call TQReset() themselves.
 *
 * The reusable test runs are in the tr-tq-enqueue-*.c, tr-tq-flush-*.c,
 * tr-tq-surrender-*.c, tr-tq-timeout-*.c, tr-sem-*.c, tr-mtx-*.c, and
 * tr-cond-wait.c files.  See tc-futex-wait.c for a directive with the POSIX
 * status and tc-sys-lock.c for a mutex.
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

#include <rtems/test-thread-queue.h>

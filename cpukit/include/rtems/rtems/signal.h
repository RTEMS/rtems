/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief This header file defines the parts of the Signal Manager API.
 */

/*
 * Copyright (C) 2020, 2021 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (C) 1988, 2008 On-Line Applications Research Corporation (OAR)
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

/* Generated from spec:/rtems/signal/if/header */

#ifndef _RTEMS_RTEMS_SIGNAL_H
#define _RTEMS_RTEMS_SIGNAL_H

#include <rtems/rtems/asr.h>
#include <rtems/rtems/modes.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/signal/if/group */

/**
 * @defgroup RTEMSAPIClassicSignal Signal Manager
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief The Signal Manager provides the capabilities required for
 *   asynchronous communication.
 */

/* Generated from spec:/rtems/signal/if/catch */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief Establishes an asynchronous signal routine (ASR) for the calling
 *   task.
 *
 * @param asr_handler is the handler to process an asynchronous signal set.
 *
 * @param mode_set is the task mode while an asynchronous signal set is
 *   processed by the handler.  See rtems_task_mode().
 *
 * This directive establishes an asynchronous signal routine (ASR) for the
 * calling task.  The ``asr_handler`` parameter specifies the entry point of
 * the ASR.  A task may have at most one handler installed at a time.  The most
 * recently installed handler is used.  When ``asr_handler`` is NULL, the ASR
 * for the calling task is invalidated and all pending signals are cleared.
 * Any signals sent to a task with an invalid ASR are discarded.  The
 * ``mode_set`` parameter specifies the execution mode for the ASR.  This
 * execution mode supersedes the task's execution mode while the ASR is
 * executing.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_NOT_IMPLEMENTED The #RTEMS_NO_PREEMPT was set in
 *   ``mode_set`` and the system configuration had no implementation for this
 *   mode.
 *
 * @retval ::RTEMS_NOT_IMPLEMENTED The RTEMS_INTERRUPT_LEVEL() was set to a
 *   positive level in ``mode_set`` and the system configuration had no
 *   implementation for this mode.
 *
 * @par Notes
 * @parblock
 * It is strongly recommended to disable ASR processing during ASR processing
 * by setting #RTEMS_NO_ASR in ``mode_set``, otherwise a recursion may happen
 * during ASR processing.  Uncontrolled recursion may lead to stack overflows.
 *
 * Using the same mutex (in particular a recursive mutex) in normal task
 * context and during ASR processing may result in undefined behaviour.
 *
 * Asynchronous signal handlers can access thread-local storage (TLS).  When
 * thread-local storage is shared between normal task context and ASR
 * processing, it may be protected by disabled interrupts.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within task context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
rtems_status_code rtems_signal_catch(
  rtems_asr_entry asr_handler,
  rtems_mode      mode_set
);

/* Generated from spec:/rtems/signal/if/send */

/**
 * @ingroup RTEMSAPIClassicSignal
 *
 * @brief Sends the signal set to the task.
 *
 * @param id is the identifier of the target task to receive the signal set.
 *
 * @param signal_set is the signal set to send.
 *
 * This directive sends the signal set, ``signal_set``, to the target task
 * identified by ``id``.
 *
 * If a caller sends a signal set to a task with an invalid ASR, then an error
 * code is returned to the caller.  If a caller sends a signal set to a task
 * whose ASR is valid but disabled, then the signal set will be caught and left
 * pending for the ASR to process when it is enabled.  If a caller sends a
 * signal set to a task with an ASR that is both valid and enabled, then the
 * signal set is caught and the ASR will execute the next time the task is
 * dispatched to run.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_NUMBER The ``signal_set`` parameter was 0.
 *
 * @retval ::RTEMS_INVALID_ID There was no task associated with the identifier
 *   specified by ``id``.
 *
 * @retval ::RTEMS_NOT_DEFINED The target task had no valid ASR installed.
 *
 * @par Notes
 * @parblock
 * Sending a signal set to a task has no effect on that task's state.  If a
 * signal set is sent to a blocked task, then the task will remain blocked and
 * the signals will be processed when the task becomes the running task.
 *
 * Sending a signal set to a global task which does not reside on the local
 * node will generate a request telling the remote node to send the signal set
 * to the specified task.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within interrupt context.
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * When the directive operates on a local object, the directive will not
 *   cause the calling task to be preempted.
 *
 * * When the directive operates on a remote object, the directive sends a
 *   message to the remote node and waits for a reply.  This will preempt the
 *   calling task.
 * @endparblock
 */
rtems_status_code rtems_signal_send(
  rtems_id         id,
  rtems_signal_set signal_set
);

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_SIGNAL_H */

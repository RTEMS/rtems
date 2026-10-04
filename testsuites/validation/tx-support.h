/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This header file provides the support functions for the validation
 *   test cases.
 *
 * The functions support the validation test cases.  Most of them call a
 * directive and check its status, so that a test case needs less code.  A
 * failure of a T_assert check ends the test case.  A failure of a T_quiet
 * check records the failure, and the test case continues.
 *
 * The functions serve these needs:
 *
 * - Tasks: CreateTask(), StartTask(), DeleteTask(), and the functions which
 *   get and set the priority, the home scheduler, and the processor affinity
 *   of a task.
 *
 * - Control of workers: SendEvents(), ReceiveAnyEvents(), and
 *   ReceiveAllEvents().
 *
 * - Task life: RestartTask(), RequestLifeChangesWithinISR(),
 *   SetCancelability(), and DisableCancelability().
 *
 * - Mutexes: CreateMutex(), ObtainMutex(), ReleaseMutex(), and DeleteMutex().
 *
 * - Time: ClockTick(), TimecounterTick(), SetTimecountCounter(),
 *   SetGetTimecountHandler(), UnsetClock(), GetTaskTimerInfo(), and
 *   DaysFromCivil().
 *
 * - Memory: MemoryAllocationFailWhen(), MemorySave(), and MemoryRestore().
 *
 * - Interrupts: CallWithinISR(), CallWithinISRSubmit(),
 *   GetTestableInterruptVector(), and RaiseSoftwareInterrupt().
 *
 * - Fatal errors and extensions: SetFatalHandler(), SetTaskSwitchExtension(),
 *   and ClearExtensionCalls().
 *
 * - SMP synchronization: WaitForExecutionStop(), WaitForIntendToBlock(),
 *   WaitForHeir(), and the ticket lock functions.
 *
 * - Test windows: SetFlag(), GetFlag(), WaitForFlag(), WaitForBlockedState(),
 *   and CallCounterObserve().
 *
 * A test case restores what it changed.  It restores the runner task with
 * RestoreRunnerMode(), RestoreRunnerPriority(), RestoreRunnerScheduler(), and
 * RestoreRunnerASR().  It removes a fatal handler, a thread switch extension,
 * and a timecount handler which it set.  It restores the memory areas which it
 * changed.
 *
 * The runner task starts each test case with the priority PRIO_DEFAULT, which
 * is one.  The values of Priority assume a runner with the priority
 * PRIO_NORMAL.  A worker with PRIO_HIGH or a higher priority then preempts the
 * runner, and a worker with PRIO_LOW or a lower priority does not.  A test
 * case with workers therefore sets the priority of the runner to PRIO_NORMAL
 * with SetSelfPriority() in its setup.  It restores the priority with
 * RestoreRunnerPriority() in its teardown.  TQInitialize() and TQDestroy() do
 * that for the thread queue tests.
 *
 * With the priority PRIO_DEFAULT, only PRIO_PSEUDO_ISR is higher than the
 * priority of the runner.  In uniprocessor configurations, a started worker
 * then runs only after the runner blocks or lowers its priority.
 *
 * In uniprocessor configurations, several wait functions return at once.  The
 * state which they wait for is then reached already.
 */

/*
 * Copyright (C) 2021, 2026 embedded brains GmbH & Co. KG
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

#include <rtems/test-support.h>

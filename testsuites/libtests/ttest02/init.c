/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
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

#include <rtems/test.h>
#include <rtems/test-info.h>

#include <string.h>

#include <rtems.h>

static void
prepare(void *arg)
{
	Atomic_Uint *state;

	state = arg;
	_Atomic_Store_uint(state, 0, ATOMIC_ORDER_RELAXED);
}

static void
action(void *arg)
{
	Atomic_Uint *state;
	unsigned int expected;
	bool success_0;
	bool success_1;

	state = arg;

	/*
	 * This code models a critical section in the operating system.  The
	 * interrupt should happen between the two atomic operations.
	 */
	expected = 0;
	success_0 = _Atomic_Compare_exchange_uint(state, &expected, 1,
	    ATOMIC_ORDER_RELAXED, ATOMIC_ORDER_RELAXED);
	expected = 1;
	success_1 = _Atomic_Compare_exchange_uint(state, &expected, 2,
	    ATOMIC_ORDER_RELAXED, ATOMIC_ORDER_RELAXED);
	T_quiet_true(success_0);
	T_quiet_true(success_1);

	T_interrupt_test_busy_wait_for_interrupt();
}

static T_interrupt_test_state
interrupt(void *arg)
{
	Atomic_Uint *state;
	unsigned int expected;

	if (T_interrupt_test_get_state() != T_INTERRUPT_TEST_ACTION) {
		return T_INTERRUPT_TEST_CONTINUE;
	}

	state = arg;
	expected = 1;

	if (_Atomic_Compare_exchange_uint(state, &expected, expected,
	    ATOMIC_ORDER_RELAXED, ATOMIC_ORDER_RELAXED)) {
		return T_INTERRUPT_TEST_DONE;
	} else if (expected == 0) {
		return T_INTERRUPT_TEST_EARLY;
	} else {
		T_quiet_eq_uint(expected, 2);
		return T_INTERRUPT_TEST_LATE;
	}
}

static const T_interrupt_test_config done_config = {
	.prepare = prepare,
	.action = action,
	.interrupt = interrupt,
	.max_iteration_count = 10000
};

T_TEST_CASE(TestInterruptDone)
{
	int i;

	for (i = 0; i < 10; ++i) {
		Atomic_Uint action_state;
		T_interrupt_test_state state;

		state = T_interrupt_test(&done_config, &action_state);
		T_eq_int(state, T_INTERRUPT_TEST_DONE);
	}
}

static const T_interrupt_test_config timeout_config = {
	.interrupt = interrupt,
	.action = action
};

T_TEST_CASE(TestInterruptTimeout)
{
	Atomic_Uint action_state;
	T_interrupt_test_state state;

	T_plan(1);
	state = T_interrupt_test(&timeout_config, &action_state);
	T_step_eq_int(0, state, T_INTERRUPT_TEST_TIMEOUT);
}

static void
fatal(void *arg)
{
	(void)arg;
	T_plan(1);
	T_step(0);
	T_stop();
}

static const T_interrupt_test_config fatal_config = {
	.prepare = fatal,
	.action = action,
	.interrupt = interrupt,
	.max_iteration_count = 10000
};

T_TEST_CASE(TestInterruptFatal)
{
	Atomic_Uint action_state;

	T_interrupt_test(&fatal_config, &action_state);
	T_unreachable();
}

static void
suspend(void *arg)
{
	rtems_status_code sc;
	rtems_id *id;

	id = arg;
	T_plan(2);
	sc = rtems_task_suspend(*id);
	T_step_rsc_success(1, sc);
}

static T_interrupt_test_state
do_nothing(void *arg)
{
	(void)arg;
	return T_INTERRUPT_TEST_ACTION;
}

static void
resume(void *arg)
{
	T_interrupt_test_state state;

	state = T_interrupt_test_change_state(T_INTERRUPT_TEST_ACTION,
	    T_INTERRUPT_TEST_DONE);

	if (state == T_INTERRUPT_TEST_ACTION) {
		rtems_status_code sc;
		rtems_id *id;

		id = arg;
		sc = rtems_task_resume(*id);
		T_step_rsc_success(0, sc);
	}
}

static const T_interrupt_test_config blocked_config = {
	.action = suspend,
	.interrupt = do_nothing,
	.blocked = resume,
	.max_iteration_count = 10000
};

T_TEST_CASE(TestInterruptBlocked)
{
	T_interrupt_test_state state;
	rtems_id id;

	T_plan(1);
	id = rtems_task_self();
	state = T_interrupt_test(&blocked_config, &id);
	T_step_eq_int(0, state, T_INTERRUPT_TEST_DONE);
}

T_TEST_CASE(TestThreadSwitch)
{
	T_thread_switch_log_2 log_2;
	T_thread_switch_log_4 log_4;
	T_thread_switch_log_10 log_10;
	T_thread_switch_log *log;
	uint32_t executing;
	uint32_t heir;
	size_t i;

	memset(&log_2, 0xff, sizeof(log_2));
	log = T_thread_switch_record_2(&log_2);
	T_null(log);
	T_eq_sz(log_2.header.recorded, 0);
	T_eq_sz(log_2.header.capacity, 2);
	T_eq_u64(log_2.header.switches, 0);

	memset(&log_4, 0xff, sizeof(log_4));
	log = T_thread_switch_record_4(&log_4);
	T_eq_ptr(&log->header, &log_2.header);
	T_eq_sz(log_4.header.recorded, 0);
	T_eq_sz(log_4.header.capacity, 4);
	T_eq_u64(log_4.header.switches, 0);

	memset(&log_10, 0xff, sizeof(log_10));
	log = T_thread_switch_record_10(&log_10);
	T_eq_ptr(&log->header, &log_4.header);
	T_eq_sz(log_10.header.recorded, 0);
	T_eq_sz(log_10.header.capacity, 10);
	T_eq_u64(log_10.header.switches, 0);

	for (i = 0; i < 6; ++i) {
		rtems_status_code sc;

		sc = rtems_task_wake_after(2);
		T_rsc_success(sc);
	}

	log = T_thread_switch_record(NULL);
	T_eq_ptr(&log->header, &log_10.header);
	T_eq_sz(log->header.recorded, 10);
	T_eq_sz(log->header.capacity, 10);
	T_eq_u64(log->header.switches, 12);
	executing = rtems_task_self();
	T_eq_u32(log->events[0].executing, executing);
	T_ne_u32(log->events[0].heir, 0);
	heir = log->events[0].heir;
	T_eq_u32(log->events[0].cpu, 0);
	T_ne_u64(log->events[0].instant, 0);

	for (i = 1; i < 10; ++i) {
		uint32_t tmp;

		tmp = executing;
		executing = heir;
		heir = tmp;

		T_eq_u32(log->events[i].executing, executing);
		T_eq_u32(log->events[i].heir, heir);
		T_eq_u32(log->events[i].cpu, 0);
		T_gt_u64(log->events[i].instant, log->events[i - 1].instant);
	}
}

const char rtems_test_name[] = "TTEST 2";

static void
Init(rtems_task_argument argument)
{
	rtems_test_run(argument, TEST_STATE);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

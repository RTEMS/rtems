#include <t.h>

#include <rtems.h>

static void
wakeup(rtems_id id, void *arg)
{
	rtems_status_code sc;
	rtems_id *task;

	(void)id;
	task = arg;
	sc = rtems_event_send(*task, RTEMS_EVENT_0);
	T_step_rsc_success(3, sc);
	T_step_false(4, T_is_runner(), "ISR is runner");
}

T_TEST_CASE(timer)
{
	rtems_status_code sc;
	rtems_id id;
	rtems_id task;
	rtems_event_set events;

	T_plan(8);
	T_step_true(0, T_is_runner(), "test body is not runner");

	sc = rtems_timer_create(rtems_build_name('T', 'E', 'S', 'T'), &id);
	T_step_assert_rsc_success(1, sc);

	/*
	 * Make sure that the next step is not immediately interrupted by the
	 * clock interrupt.
	 */
	(void)rtems_task_wake_after(1);

	task = rtems_task_self();
	sc = rtems_timer_fire_after(id, 1, wakeup, &task);
	T_step_rsc_success(2, sc);

	events = 0;
	sc = rtems_event_receive(RTEMS_EVENT_0, RTEMS_WAIT | RTEMS_EVENT_ALL,
	    RTEMS_NO_TIMEOUT, &events);
	T_step_rsc_success(5, sc);
	T_step_eq_u32(6, events, RTEMS_EVENT_0);

	sc = rtems_timer_delete(id);
	T_step_rsc_success(7, sc);
}

T_TEST_CASE(rsc)
{
	T_plan(6);
	T_rsc(RTEMS_INVALID_ID, RTEMS_INVALID_ID);
	T_rsc(RTEMS_INVALID_NUMBER, RTEMS_INVALID_ID);
	T_quiet_rsc(RTEMS_INVALID_ID, RTEMS_INVALID_ID);
	T_quiet_rsc(RTEMS_INVALID_NUMBER, RTEMS_INVALID_ID);
	T_assert_rsc(RTEMS_INVALID_ID, RTEMS_INVALID_ID);
	T_assert_rsc(RTEMS_INVALID_NUMBER, RTEMS_INVALID_ID);
}

T_TEST_CASE(rsc_success)
{
	T_plan(6);
	T_rsc_success(RTEMS_SUCCESSFUL);
	T_rsc_success(RTEMS_INVALID_NUMBER);
	T_quiet_rsc_success(RTEMS_SUCCESSFUL);
	T_quiet_rsc_success(RTEMS_INVALID_NUMBER);
	T_assert_rsc_success(RTEMS_SUCCESSFUL);
	T_assert_rsc_success(RTEMS_INVALID_NUMBER);
}

/*
 * The license is at the end of the file to be able to use the test code and
 * output in examples in the documentation.  This is also the reason for the
 * dual licensing.  The license for RTEMS documentation is CC-BY-SA-4.0.
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause OR CC-BY-SA-4.0
 *
 * Copyright (C) 2018, 2019 embedded brains GmbH
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
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * Creative Commons Attribution-ShareAlike 4.0 International Public License as
 * published by Creative Commons, PO Box 1866, Mountain View, CA 94042
 * (https://creativecommons.org/licenses/by-sa/4.0/legalcode).
 */

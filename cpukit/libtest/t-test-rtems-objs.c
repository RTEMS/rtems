/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief RTEMS Objects Support for Test Framework
 */

/*
 * Copyright (C) 2018 embedded brains GmbH (http://www.embedded-brains.de)
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

#undef __STRICT_ANSI__

#include "t-test-rtems.h"

#include <rtems/test.h>

#include <inttypes.h>

#include <rtems/score/threadimpl.h>

T_thread_timer_state
T_get_thread_timer_state(uint32_t id)
{
	Thread_Control *the_thread;
	ISR_lock_Context lock_context;
	T_thread_timer_state state;

	the_thread = _Thread_Get(id, &lock_context);
	if (the_thread == NULL) {
		return T_THREAD_TIMER_NO_THREAD;
	}

	switch (_Watchdog_Get_state(&the_thread->Timer.Watchdog)) {
		case WATCHDOG_SCHEDULED_BLACK:
		case WATCHDOG_SCHEDULED_RED:
			state = T_THREAD_TIMER_SCHEDULED;
			break;
		case WATCHDOG_PENDING:
			state = T_THREAD_TIMER_PENDING;
			break;
		default:
			state = T_THREAD_TIMER_INACTIVE;
			break;
	}

	_ISR_lock_ISR_enable(&lock_context);
	return state;
}

Objects_Maximum
T_objects_count(Objects_APIs api, uint16_t cls)
{
	const Objects_Information *information;
	Objects_Maximum count;

	information = _Objects_Get_information(api, cls);

	_RTEMS_Lock_allocator();

	if (information != NULL) {
		count = _Objects_Active_count(information);
	} else {
		count = 0;
	}

	_RTEMS_Unlock_allocator();

	return count;
}

void
T_objects_check(Objects_APIs api, uint16_t cls,
    Objects_Maximum *expected, const char *name)
{
	Objects_Maximum count;
	int32_t delta;

	count = T_objects_count(api, cls);
	delta = (int32_t)count - (int32_t)*expected;

	if (delta != 0) {
		*expected = count;
		T_check(&T_special, false, "%s leak (%" PRIi32 ")", name,
		    delta);
	}
}

static Objects_Maximum T_barrier_count;

static void
T_rtems_barriers_run_initialize(void)
{
	T_barrier_count = T_objects_count(OBJECTS_CLASSIC_API,
	    OBJECTS_RTEMS_BARRIERS);
}

static void
T_rtems_barriers_case_end(void)
{
	T_objects_check(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_BARRIERS,
	    &T_barrier_count, "RTEMS barrier");
}

void
T_check_rtems_barriers(T_event event, const char *name)
{
	(void)name;

	switch (event) {
	case T_EVENT_RUN_INITIALIZE_EARLY:
		T_rtems_barriers_run_initialize();
		break;
	case T_EVENT_CASE_END:
		T_rtems_barriers_case_end();
		break;
	default:
		break;
	};
}

static Objects_Maximum T_extension_count;

static void
T_rtems_extensions_run_initialize(void)
{
	T_extension_count = T_objects_count(OBJECTS_CLASSIC_API,
	    OBJECTS_RTEMS_EXTENSIONS);
}

static void
T_rtems_extensions_case_end(void)
{
	T_objects_check(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_EXTENSIONS,
	    &T_extension_count, "RTEMS extension");
}

void
T_check_rtems_extensions(T_event event, const char *name)
{
	(void)name;

	switch (event) {
	case T_EVENT_RUN_INITIALIZE_EARLY:
		T_rtems_extensions_run_initialize();
		break;
	case T_EVENT_CASE_END:
		T_rtems_extensions_case_end();
		break;
	default:
		break;
	};
}

static Objects_Maximum T_mq_count;

static void
T_rtems_message_queues_run_initialize(void)
{
	T_mq_count = T_objects_count(OBJECTS_CLASSIC_API,
	    OBJECTS_RTEMS_MESSAGE_QUEUES);
}

static void
T_rtems_message_queues_case_end(void)
{
	T_objects_check(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_MESSAGE_QUEUES,
	    &T_mq_count, "RTEMS message queue");
}

void
T_check_rtems_message_queues(T_event event, const char *name)
{
	(void)name;

	switch (event) {
	case T_EVENT_RUN_INITIALIZE_EARLY:
		T_rtems_message_queues_run_initialize();
		break;
	case T_EVENT_CASE_END:
		T_rtems_message_queues_case_end();
		break;
	default:
		break;
	};
}

static Objects_Maximum T_part_count;

static void
T_rtems_partitions_run_initialize(void)
{
	T_part_count = T_objects_count(OBJECTS_CLASSIC_API,
	    OBJECTS_RTEMS_PARTITIONS);
}

static void
T_rtems_partitions_case_end(void)
{
	T_objects_check(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_PARTITIONS,
	    &T_part_count, "RTEMS partition");
}

void
T_check_rtems_partitions(T_event event, const char *name)
{
	(void)name;

	switch (event) {
	case T_EVENT_RUN_INITIALIZE_EARLY:
		T_rtems_partitions_run_initialize();
		break;
	case T_EVENT_CASE_END:
		T_rtems_partitions_case_end();
		break;
	default:
		break;
	};
}

static Objects_Maximum T_period_count;

static void
T_rtems_periods_run_initialize(void)
{
	T_period_count = T_objects_count(OBJECTS_CLASSIC_API,
	    OBJECTS_RTEMS_PERIODS);
}

static void
T_rtems_periods_case_end(void)
{
	T_objects_check(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_PERIODS,
	    &T_period_count, "RTEMS period");
}

void
T_check_rtems_periods(T_event event, const char *name)
{
	(void)name;

	switch (event) {
	case T_EVENT_RUN_INITIALIZE_EARLY:
		T_rtems_periods_run_initialize();
		break;
	case T_EVENT_CASE_END:
		T_rtems_periods_case_end();
		break;
	default:
		break;
	};
}

static Objects_Maximum T_region_count;

static void
T_rtems_regions_run_initialize(void)
{
	T_region_count = T_objects_count(OBJECTS_CLASSIC_API,
	    OBJECTS_RTEMS_REGIONS);
}

static void
T_rtems_regions_case_end(void)
{
	T_objects_check(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_REGIONS,
	    &T_region_count, "RTEMS region");
}

void
T_check_rtems_regions(T_event event, const char *name)
{
	(void)name;

	switch (event) {
	case T_EVENT_RUN_INITIALIZE_EARLY:
		T_rtems_regions_run_initialize();
		break;
	case T_EVENT_CASE_END:
		T_rtems_regions_case_end();
		break;
	default:
		break;
	};
}

static Objects_Maximum T_sema_count;

static void
T_rtems_semaphores_run_initialize(void)
{
	T_sema_count = T_objects_count(OBJECTS_CLASSIC_API,
	    OBJECTS_RTEMS_SEMAPHORES);
}

static void
T_rtems_semaphores_case_end(void)
{
	T_objects_check(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_SEMAPHORES,
	    &T_sema_count, "RTEMS semaphore");
}

void
T_check_rtems_semaphores(T_event event, const char *name)
{
	(void)name;

	switch (event) {
	case T_EVENT_RUN_INITIALIZE_EARLY:
		T_rtems_semaphores_run_initialize();
		break;
	case T_EVENT_CASE_END:
		T_rtems_semaphores_case_end();
		break;
	default:
		break;
	};
}

static Objects_Maximum T_task_count;

static void
T_rtems_tasks_run_initialize(void)
{
	T_task_count = T_objects_count(OBJECTS_CLASSIC_API,
	    OBJECTS_RTEMS_TASKS);
}

static void
T_rtems_tasks_case_end(void)
{
	_RTEMS_Lock_allocator();
	_Thread_Kill_zombies();
	_RTEMS_Unlock_allocator();

	T_objects_check(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_TASKS,
	    &T_task_count, "RTEMS task");
}

void
T_check_rtems_tasks(T_event event, const char *name)
{
	(void)name;

	switch (event) {
	case T_EVENT_RUN_INITIALIZE_EARLY:
		T_rtems_tasks_run_initialize();
		break;
	case T_EVENT_CASE_END:
		T_rtems_tasks_case_end();
		break;
	default:
		break;
	};
}

static Objects_Maximum T_timer_count;

static void
T_rtems_timers_run_initialize(void)
{
	T_timer_count = T_objects_count(OBJECTS_CLASSIC_API,
	    OBJECTS_RTEMS_TIMERS);
}

static void
T_rtems_timers_case_end(void)
{
	T_objects_check(OBJECTS_CLASSIC_API, OBJECTS_RTEMS_TIMERS,
	    &T_timer_count, "RTEMS timer");
}

void
T_check_rtems_timers(T_event event, const char *name)
{
	(void)name;

	switch (event) {
	case T_EVENT_RUN_INITIALIZE_EARLY:
		T_rtems_timers_run_initialize();
		break;
	case T_EVENT_CASE_END:
		T_rtems_timers_case_end();
		break;
	default:
		break;
	};
}

void *
T_seize_objects(rtems_status_code (*create)(void *, uint32_t *), void *arg)
{
	void *objects;

	objects = NULL;

	while (true) {
		rtems_status_code sc;
		rtems_id id;

		id = 0;
		sc = (*create)(arg, &id);

		if (sc == RTEMS_SUCCESSFUL) {
			const Objects_Information *info;
			Objects_Control *obj;

			info = _Objects_Get_information_id(id);
			T_quiet_assert_not_null(info);
			obj = _Objects_Get_no_protection(id, info);
			T_quiet_assert_not_null(obj);
			obj->Node.next = objects;
			objects = obj;
		} else {
			T_quiet_rsc(sc, RTEMS_TOO_MANY);
			break;
		}
	}

	return objects;
}

void
T_surrender_objects(void **objects_p, rtems_status_code (*delete)(uint32_t))
{
	void *objects;

	objects = *objects_p;
	*objects_p = NULL;

	while (objects != NULL) {
		Objects_Control *obj;
		rtems_status_code sc;

		obj = objects;
		objects = _Chain_Next(&obj->Node);
		_Chain_Set_off_chain(&obj->Node);

		sc = (*delete)(obj->id);
		T_quiet_rsc_success(sc);
	}
}

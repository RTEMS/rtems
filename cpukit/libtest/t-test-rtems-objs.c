/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2018 embedded brains GmbH
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

#include <t.h>

#include <inttypes.h>

#include <rtems/score/objectimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/posix/keyimpl.h>

static Objects_Maximum
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

static void
T_objects_check(Objects_APIs api, uint16_t cls,
    Objects_Maximum *expected, const char *name)
{
	Objects_Maximum count;
	int32_t delta;

	count = T_objects_count(api, cls);
	delta = (int32_t)count - (int32_t)*expected;

	if (delta != 0) {
		*expected = count;
		T_check_true(false, NULL, "%s leak (%" PRIi32 ")", name, delta);
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

static Objects_Maximum T_posix_key_count;

static ssize_t T_posix_key_value_count;

static POSIX_Keys_Control *
T_get_next_posix_key(Objects_Id *id)
{
	return (POSIX_Keys_Control *)
	    _Objects_Get_next(*id, &_POSIX_Keys_Information, id);
}

static ssize_t
T_get_active_posix_key_value_pairs(void)
{
	ssize_t count;
	Objects_Id id;
	POSIX_Keys_Control *the_key;

	count = 0;
	id = OBJECTS_ID_INITIAL_INDEX;

	while ((the_key = T_get_next_posix_key(&id)) != NULL ) {
		count += (ssize_t)
		    _Chain_Node_count_unprotected(&the_key->Key_value_pairs);
		_Objects_Allocator_unlock();
	}

	return count;
}

static void
T_posix_keys_run_initialize(void)
{
	T_posix_key_count = T_objects_count(OBJECTS_POSIX_API,
	    OBJECTS_POSIX_KEYS);
	T_posix_key_value_count = T_get_active_posix_key_value_pairs();
}

static void
T_posix_keys_case_end(void)
{
	ssize_t count;
	ssize_t delta;

	T_objects_check(OBJECTS_POSIX_API, OBJECTS_POSIX_KEYS,
	    &T_posix_key_count, "POSIX key");

	count = T_get_active_posix_key_value_pairs();
	delta = count - T_posix_key_value_count;

	if (delta != 0) {
		T_posix_key_value_count = count;
		T_check_true(false, NULL, "POSIX key value pair leak (%zi)", delta);
	}
}

void
T_check_posix_keys(T_event event, const char *name)
{
	(void)name;

	switch (event) {
	case T_EVENT_RUN_INITIALIZE_EARLY:
		T_posix_keys_run_initialize();
		break;
	case T_EVENT_CASE_END:
		T_posix_keys_case_end();
		break;
	default:
		break;
	};
}

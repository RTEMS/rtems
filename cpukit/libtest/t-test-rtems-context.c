/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestFrameworkImpl
 *
 * @brief This source file contains the implementation of
 *   T_check_task_context().
 */

/*
 * Copyright (C) 2019 embedded brains GmbH & Co. KG
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

#include <rtems/test.h>

#include <rtems.h>
#include <rtems/score/isrlevel.h>
#include <rtems/score/percpu.h>
#include <rtems/score/threaddispatch.h>

#include <inttypes.h>

#ifdef RTEMS_SMP
static rtems_id T_runner_scheduler;
#endif

static rtems_task_priority T_runner_priority;

static void
T_initialize_runner_properties(void)
{
	rtems_status_code sc;

#ifdef RTEMS_SMP
	sc = rtems_task_get_scheduler(RTEMS_SELF, &T_runner_scheduler);
	T_quiet_rsc_success(sc);
#endif

	sc = rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY,
	    &T_runner_priority);
	T_quiet_rsc_success(sc);
}

static void
T_do_check_task_context(void)
{
	rtems_task_priority prio;
	rtems_status_code sc;
	uint32_t v;
	rtems_event_set events;
#ifdef RTEMS_SMP
	rtems_id id;
#endif

	v = _Per_CPU_Get_snapshot()->thread_dispatch_disable_level;
	T_check(&T_special, v == 0,
	    "Wrong thread dispatch disabled level (%" PRIu32 ")", v);

	v = _Per_CPU_Get_snapshot()->isr_nest_level;
	T_check(&T_special, v == 0,
	    "Wrong ISR nest level (%" PRIu32 ")", v);

	v = _ISR_Get_level();
	T_check(&T_special, v == 0,
	    "Wrong ISR level (%" PRIu32 ")", v);

#ifdef RTEMS_SMP
	id = 0;
	sc = rtems_task_get_scheduler(RTEMS_SELF, &id);
	T_quiet_rsc_success(sc);
	T_check(&T_special, id == T_runner_scheduler,
	    "Wrong runner scheduler, expected ID %08" PRIx32 ", actual ID %08"
	    PRIx32, T_runner_scheduler, id);
#endif

	prio = 0;
	sc = rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY,
	    &prio);
	T_quiet_rsc_success(sc);
	T_check(&T_special, prio == T_runner_priority,
	    "Wrong runner priority, expected %" PRIu32 ", actual %"
	    PRIu32, T_runner_priority, prio);

	sc = rtems_event_receive(RTEMS_ALL_EVENTS,
	    RTEMS_NO_WAIT | RTEMS_EVENT_ANY, 0, &events);
	T_quiet_rsc( sc, RTEMS_UNSATISFIED );

	sc = rtems_event_system_receive(RTEMS_ALL_EVENTS,
	    RTEMS_NO_WAIT | RTEMS_EVENT_ANY, 0, &events);
	T_quiet_rsc( sc, RTEMS_UNSATISFIED );
}

void
T_check_task_context(T_event event, const char *name)
{
	(void)name;

	switch (event) {
	case T_EVENT_RUN_INITIALIZE_LATE:
		T_initialize_runner_properties();
		/* Fall through */
	case T_EVENT_CASE_END:
		T_do_check_task_context();
		break;
	default:
		break;
	};
}

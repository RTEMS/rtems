/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2019 embedded brains GmbH
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

#include <rtems/score/isrlevel.h>
#include <rtems/score/percpu.h>
#include <rtems/score/threaddispatch.h>

#include <inttypes.h>

static void
T_do_check_task_context(void)
{
	uint32_t v;

	v = _Per_CPU_Get_snapshot()->thread_dispatch_disable_level;
	T_check_true(v == 0, NULL,
	    "Wrong thread dispatch disabled level (%" PRIu32 ")", v);

	v = _Per_CPU_Get_snapshot()->isr_nest_level;
	T_check_true(v == 0, NULL,
	    "Wrong ISR nest level (%" PRIu32 ")", v);

	v = _ISR_Get_level();
	T_check_true(v == 0, NULL,
	    "Wrong ISR level (%" PRIu32 ")", v);
}

void
T_check_task_context(T_event event, const char *name)
{
	(void)name;

	switch (event) {
	case T_EVENT_RUN_INITIALIZE_LATE:
	case T_EVENT_CASE_END:
		T_do_check_task_context();
		break;
	default:
		break;
	};
}

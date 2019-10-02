/*
 * SPDX-License-Identifier: BSD-2-Clause
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
 */

#include <t.h>

#include <sys/time.h>
#include <string.h>

#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/timecounter.h>
#include <rtems/sysinit.h>

#include "t-self-test.h"

#include <tmacros.h>

const char rtems_test_name[] = "TTEST 1";

#define test_assert(e) (e) ? (void)0 : test_failed(__LINE__, #e)

RTEMS_LINKER_ROSET(t_self_test, const char *);

typedef enum {
	CENSOR_PASS_THROUGH,
	CENSOR_DISCARD
} censor_state;

typedef struct {
	const char *c;
	size_t case_begin_count;
	size_t case_end_count;
	struct timecounter tc;
	T_putchar putchar;
	void *putchar_arg;
	const char *censor_c;
	censor_state censor_state;
} test_context;

static test_context test_instance;

static void
test_failed(int line, const char *e)
{
	printk("FAILED:%i:%s\n", line, e);
	rtems_test_exit(1);
}

static void
test_putchar(int c, void *arg)
{
	test_context *ctx;

	ctx = arg;

	if (c != '\r' && ctx->c != NULL) {
		test_assert(*ctx->c == c);
		++ctx->c;
	}

	rtems_putc((char)c);
}

static void
case_early(const char *name)
{
	test_context *ctx;
	const char **item;
	ssize_t n;

	ctx = &test_instance;
	++ctx->case_begin_count;
	n = strlen(name);

	RTEMS_LINKER_SET_FOREACH(t_self_test, item) {
		const char *to;

		to = *item;

		if (strncmp(name, to, n) == 0 && to[n] == ':') {
			ctx->c = to + n + 1;
			return;
		}
	}

	test_assert(0);
}

static void
case_late(const char *name)
{
	test_context *ctx;

	ctx = &test_instance;
	++ctx->case_end_count;
	test_assert(ctx->c != NULL);
	test_assert(*ctx->c == '\0');
	ctx->c = NULL;
}

static const char censored_init[] = "A:ttest01\n"
"S:Platform:RTEMS\n"
"S:Compiler:*"
"S:Version:*"
"S:BSP:*"
"S:RTEMS_DEBUG:*"
"S:RTEMS_MULTIPROCESSING:*"
"S:RTEMS_POSIX_API:*"
"S:RTEMS_PROFILING:*"
"S:RTEMS_SMP:*";

static void
censor_putchar(int c, void *arg)
{
	test_context *ctx;

	ctx = arg;

	if (*ctx->censor_c == '\0') {
		T_putchar discard_putchar;
		void *discard_putchar_arg;

		(*ctx->putchar)(c, ctx->putchar_arg);
		T_set_putchar(ctx->putchar, ctx->putchar_arg, &discard_putchar,
		   &discard_putchar_arg);
		return;
	}

	switch (ctx->censor_state) {
	case CENSOR_PASS_THROUGH:
		if (*ctx->censor_c == '*') {
			(*ctx->putchar)('*', ctx->putchar_arg);
			ctx->censor_state = CENSOR_DISCARD;
		} else if (c == *ctx->censor_c) {
			(*ctx->putchar)(c, ctx->putchar_arg);
			++ctx->censor_c;
		}
		break;
	case CENSOR_DISCARD:
		if (c == '\n') {
			(*ctx->putchar)(c, ctx->putchar_arg);
			ctx->censor_state = CENSOR_PASS_THROUGH;
			++ctx->censor_c;
		}
		break;
	}
}

static void
run_initialize(void)
{
	test_context *ctx;

	ctx = &test_instance;
	ctx->censor_c = censored_init;
	T_set_putchar(censor_putchar, ctx, &ctx->putchar, &ctx->putchar_arg);
}

static const char expected_final[] = "Z:ttest01:C:341:N:1316:F:790:D:0.682999\n"
"Y:ReportHash:SHA256:62d6f3b37299137932ea2c2f0505c8b8f12b95749c81d5af19570e9470203475\n";

static void
run_finalize(void)
{
	test_context *ctx;

	ctx = &test_instance;
	ctx->c = expected_final;
}

static void
test_action(T_event event, const char *name)
{
	(void)name;

	switch (event) {
	case T_EVENT_CASE_EARLY:
		case_early(name);
		break;
	case T_EVENT_CASE_LATE:
		case_late(name);
		break;
	case T_EVENT_RUN_INITIALIZE_EARLY:
		run_initialize();
		break;
	case T_EVENT_RUN_FINALIZE_EARLY:
		run_finalize();
		break;
	default:
		break;
	};
}

static Atomic_Uint counter = ATOMIC_INITIALIZER_UINT(0);

static T_time
now(void)
{
	T_time t;

	t = _Atomic_Fetch_add_uint(&counter, 1, ATOMIC_ORDER_RELAXED);
	return t * SBT_1MS;
}

static uint32_t
get_timecount(struct timecounter *tc)
{
	return 0;
}

static void
install_timecounter(void)
{
	test_context *ctx;

	ctx = &test_instance;
	ctx->tc.tc_get_timecount = get_timecount;
	ctx->tc.tc_counter_mask = 0xffffffff;
	ctx->tc.tc_frequency = 1000000000;
	ctx->tc.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER + 1;
	rtems_timecounter_install(&ctx->tc);
}

RTEMS_SYSINIT_ITEM(install_timecounter, RTEMS_SYSINIT_DEVICE_DRIVERS,
    RTEMS_SYSINIT_ORDER_FIRST);

static char buffer[512];

static const T_action actions[] = {
	T_report_hash_sha256,
	test_action,
	T_check_file_descriptors,
	T_check_rtems_barriers,
	T_check_rtems_extensions,
	T_check_rtems_message_queues,
	T_check_rtems_partitions,
	T_check_rtems_periods,
	T_check_rtems_regions,
	T_check_rtems_semaphores,
	T_check_rtems_tasks,
	T_check_rtems_timers,
	T_check_posix_keys
};

static const T_config config = {
	.name = "ttest01",
	.buf = buffer,
	.buf_size = sizeof(buffer),
	.putchar = test_putchar,
	.putchar_arg = &test_instance,
	.verbosity = T_VERBOSE,
	.now = now,
	.action_count = T_ARRAY_SIZE(actions),
	.actions = actions
};

static void
Init(rtems_task_argument arg)
{
	test_context *ctx;
	int exit_code;
	size_t case_count;

	(void)arg;
	TEST_BEGIN();
	ctx = &test_instance;
	test_assert(!T_is_runner());
	T_register();
	test_assert(!T_is_runner());
	exit_code = T_main(&config);
	test_assert(exit_code == 1);
	test_assert(!T_is_runner());
	case_count = RTEMS_LINKER_SET_ITEM_COUNT(t_self_test);
	test_assert(ctx->case_begin_count == case_count);
	test_assert(ctx->case_end_count == case_count);
	TEST_END();
	rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_BARRIERS 1
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES 1
#define CONFIGURE_MAXIMUM_PARTITIONS 1
#define CONFIGURE_MAXIMUM_PERIODS 1
#define CONFIGURE_MAXIMUM_REGIONS 1
#define CONFIGURE_MAXIMUM_SEMAPHORES 1
#define CONFIGURE_MAXIMUM_TASKS 2
#define CONFIGURE_MAXIMUM_TIMERS 1
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 1

#define CONFIGURE_MAXIMUM_POSIX_KEYS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

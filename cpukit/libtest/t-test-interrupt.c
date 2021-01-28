/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestFrameworkImpl
 *
 * @brief Implementation of T_interrupt_test().
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

#include <rtems/score/atomic.h>
#include <rtems/score/percpu.h>
#include <rtems/score/thread.h>
#include <rtems/score/timecounter.h>
#include <rtems/score/timestampimpl.h>
#include <rtems/score/userextimpl.h>
#include <rtems/score/watchdogimpl.h>

#ifdef RTEMS_SMP
#include <rtems/score/smpimpl.h>
#endif

typedef T_interrupt_test_state (*T_interrupt_test_handler)(void *);

#define T_INTERRUPT_SAMPLE_COUNT 8

typedef struct {
	uint_fast32_t one_tick_busy;
	int64_t t0;
	Thread_Control *self;
	Atomic_Uint state;
	void (*prepare)(void *);
	void (*action)(void *);
	T_interrupt_test_state (*interrupt)(void *);
	void (*blocked)(void *);
	void *arg;
#ifdef RTEMS_SMP
	Per_CPU_Job job;
	Per_CPU_Job_context job_context;
#endif
	Watchdog_Control wdg;
	User_extensions_Control ext;
	T_fixture_node node;
} T_interrupt_context;

typedef struct {
	int64_t t;
	int64_t d;
} T_interrupt_clock_time;

static void
T_interrupt_sort(T_interrupt_clock_time *ct, size_t n)
{
	size_t i;

	/* Bubble sort */
	for (i = 1; i < n ; ++i) {
		size_t j;

		for (j = 0; j < n - i; ++j) {
			 if (ct[j].d > ct[j + 1].d) {
				T_interrupt_clock_time tmp;

				tmp = ct[j];
				ct[j] = ct[j + 1];
				ct[j + 1] = tmp;
			 }
		}
	}
}

static int64_t
T_interrupt_time_close_to_tick(void)
{
	Watchdog_Interval c0;
	Watchdog_Interval c1;
	T_interrupt_clock_time ct[12];
	Timestamp_Control t;
	int32_t ns_per_tick;
	size_t i;
	size_t n;

	ns_per_tick = (int32_t)_Watchdog_Nanoseconds_per_tick;
	n = RTEMS_ARRAY_SIZE(ct);
	c0 = _Watchdog_Ticks_since_boot;

	for (i = 0; i < n; ++i) {
		do {
			c1 = _Watchdog_Ticks_since_boot;
			t = _Timecounter_Sbinuptime();
		} while (c0 == c1);

		c0 = c1;
		ct[i].t = sbttons(t);
	}

	for (i = 1; i < n; ++i) {
		int64_t d;

		d = (ct[i].t - ct[1].t) % ns_per_tick;

		if (d > ns_per_tick / 2) {
			d -= ns_per_tick;
		}

		ct[i].d = d;
	}

	/*
	 * Use the median and not the arithmetic mean since on simulator
	 * platforms there may be outliers.
	 */
	T_interrupt_sort(&ct[1], n - 1);
	return ct[1 + (n - 1) / 2].t;
}

static void
T_interrupt_watchdog(Watchdog_Control *wdg)
{
	T_interrupt_context *ctx;
	ISR_Level level;
	T_interrupt_test_state state;
	unsigned int expected;

	ctx = RTEMS_CONTAINER_OF(wdg, T_interrupt_context, wdg);

	_ISR_Local_disable(level);
	_Watchdog_Per_CPU_insert_ticks(&ctx->wdg,
	    _Watchdog_Get_CPU(&ctx->wdg), 1);
	_ISR_Local_enable(level);

	state = (*ctx->interrupt)(ctx->arg);

	expected = T_INTERRUPT_TEST_ACTION;
	_Atomic_Compare_exchange_uint(&ctx->state, &expected,
	    state, ATOMIC_ORDER_RELAXED, ATOMIC_ORDER_RELAXED);
}

static void
T_interrupt_watchdog_insert(T_interrupt_context *ctx)
{
	ISR_Level level;

	_ISR_Local_disable(level);
	_Watchdog_Per_CPU_insert_ticks(&ctx->wdg, _Per_CPU_Get(), 1);
	_ISR_Local_enable(level);
}

static void
T_interrupt_watchdog_remove(T_interrupt_context *ctx)
{
	ISR_Level level;

	_ISR_Local_disable(level);
	_Watchdog_Per_CPU_remove_ticks(&ctx->wdg);
	_ISR_Local_enable(level);
}

static void
T_interrupt_init_once(T_interrupt_context *ctx)
{
	ctx->t0 = T_interrupt_time_close_to_tick();
	ctx->one_tick_busy = T_get_one_clock_tick_busy();
}

static T_interrupt_test_state
T_interrupt_continue(void *arg)
{
	(void)arg;
	return T_INTERRUPT_TEST_CONTINUE;
}

static void
T_interrupt_do_nothing(void *arg)
{
	(void)arg;
}

#ifdef RTEMS_SMP
static void
T_interrupt_blocked(void *arg)
{
	T_interrupt_context *ctx;

	ctx = arg;
	(*ctx->blocked)(ctx->arg);
}
#endif

static void T_interrupt_thread_switch(Thread_Control *, Thread_Control *);

static T_interrupt_context T_interrupt_instance = {
	.interrupt = T_interrupt_continue,
	.blocked = T_interrupt_do_nothing,
#ifdef RTEMS_SMP
	.job = {
		.context = &T_interrupt_instance.job_context
	},
	.job_context = {
		.handler = T_interrupt_blocked,
		.arg = &T_interrupt_instance
	},
#endif
	.wdg = WATCHDOG_INITIALIZER(T_interrupt_watchdog),
	.ext = {
		.Callouts = {
			.thread_switch = T_interrupt_thread_switch
		}
	}
};

T_interrupt_test_state
T_interrupt_test_change_state(T_interrupt_test_state expected_state,
    T_interrupt_test_state desired_state)
{
	T_interrupt_context *ctx;
	unsigned int expected;

	ctx = &T_interrupt_instance;
	expected = expected_state;
	_Atomic_Compare_exchange_uint(&ctx->state, &expected,
	    desired_state, ATOMIC_ORDER_RELAXED, ATOMIC_ORDER_RELAXED);

	return (T_interrupt_test_state)expected;
}

T_interrupt_test_state
T_interrupt_test_get_state(void)
{
	T_interrupt_context *ctx;

	ctx = &T_interrupt_instance;
	return (T_interrupt_test_state)_Atomic_Load_uint(&ctx->state,
	    ATOMIC_ORDER_RELAXED);
}

void
T_interrupt_test_busy_wait_for_interrupt(void)
{
	T_interrupt_context *ctx;
	unsigned int state;

	ctx = &T_interrupt_instance;

	do {
		state = _Atomic_Load_uint(&ctx->state, ATOMIC_ORDER_RELAXED);
	} while (state == T_INTERRUPT_TEST_ACTION);
}

static void
T_interrupt_thread_switch(Thread_Control *executing, Thread_Control *heir)
{
	T_interrupt_context *ctx;

	(void)heir;
	ctx = &T_interrupt_instance;

	if (ctx->self == executing) {
		T_interrupt_test_state state;

		state = (T_interrupt_test_state)_Atomic_Load_uint(&ctx->state,
		    ATOMIC_ORDER_RELAXED);

		if (state != T_INTERRUPT_TEST_INITIAL) {
#ifdef RTEMS_SMP
			Per_CPU_Control *cpu_self;

			/*
			 * In SMP configurations, the thread switch extension
			 * runs in a very restricted environment.  Interrupts
			 * are disabled and the caller owns the per-CPU lock.
			 * In order to avoid deadlocks at SMP lock level, we
			 * have to use an SMP job which runs later in the
			 * context of the inter-processor interrupt.
			 */
			cpu_self = _Per_CPU_Get();
			_Per_CPU_Add_job(cpu_self, &ctx->job);
			_SMP_Send_message(_Per_CPU_Get_index(cpu_self),
			    SMP_MESSAGE_PERFORM_JOBS);
#else
			(*ctx->blocked)(ctx->arg);
#endif
		}
	}
}

static T_interrupt_context *
T_interrupt_setup(const T_interrupt_test_config *config, void *arg)
{
	T_interrupt_context *ctx;

	T_quiet_assert_not_null(config->action);
	T_quiet_assert_not_null(config->interrupt);
	ctx = &T_interrupt_instance;
	ctx->self = _Thread_Get_executing();
	ctx->arg = arg;
	ctx->interrupt = config->interrupt;

	if (config->blocked != NULL) {
		ctx->blocked = config->blocked;
	}

	if (ctx->t0 == 0) {
		T_interrupt_init_once(ctx);
	}

	_User_extensions_Add_set(&ctx->ext);
	T_interrupt_watchdog_insert(ctx);
	return ctx;
}

static void
T_interrupt_teardown(void *arg)
{
	T_interrupt_context *ctx;

	ctx = arg;
	ctx->interrupt = T_interrupt_continue;
	ctx->blocked = T_interrupt_do_nothing;
	T_interrupt_watchdog_remove(ctx);
	_User_extensions_Remove_set(&ctx->ext);
	ctx->self = NULL;
	ctx->arg = NULL;
}

static const T_fixture T_interrupt_fixture = {
	.teardown = T_interrupt_teardown,
	.initial_context = &T_interrupt_instance
};

T_interrupt_test_state
T_interrupt_test(const T_interrupt_test_config *config, void *arg)
{
	T_interrupt_context *ctx;
	uint_fast32_t lower_bound[T_INTERRUPT_SAMPLE_COUNT];
	uint_fast32_t upper_bound[T_INTERRUPT_SAMPLE_COUNT];
	uint_fast32_t lower_sum;
	uint_fast32_t upper_sum;
	int32_t ns_per_tick;
	size_t sample;
	uint32_t iter;

	ctx = T_interrupt_setup(config, arg);
	T_push_fixture(&ctx->node, &T_interrupt_fixture);
	ns_per_tick = (int32_t)_Watchdog_Nanoseconds_per_tick;
	lower_sum = 0;
	upper_sum = T_INTERRUPT_SAMPLE_COUNT * ctx->one_tick_busy;

	for (sample = 0; sample < T_INTERRUPT_SAMPLE_COUNT; ++sample) {
		lower_bound[sample] = 0;
		upper_bound[sample] = ctx->one_tick_busy;
	}

	sample = 0;

	for (iter = 0; iter < config->max_iteration_count; ++iter) {
		T_interrupt_test_state state;
		int64_t t;
		int64_t d;
		Timestamp_Control s1;
		Timestamp_Control s0;
		uint_fast32_t busy;
		uint_fast32_t delta;

		if (config->prepare != NULL) {
			(*config->prepare)(arg);
		}

		/*
		 * We use some sort of a damped bisection to find the right
		 * interrupt time point.
		 */
		busy = (lower_sum + upper_sum) /
		    (2 * T_INTERRUPT_SAMPLE_COUNT);

		t = sbttons(_Timecounter_Sbinuptime());
		d = (t - ctx->t0) % ns_per_tick;
		t += ns_per_tick / 4 - d;

		if (d > ns_per_tick / 8) {
			t += ns_per_tick;
		}

		/*
		 * The s1 value is a future time point close to 25% of a clock
		 * tick interval.
		 */
		s1 = nstosbt(t);

		/*
		 * The path from here to the action call must avoid anything
		 * which can cause jitters.  We wait until 25% of the clock
		 * tick interval are elapsed using the timecounter.  Then we do
		 * a busy wait and call the action.  The interrupt time point
		 * is controlled by the busy count.
		 */

		do {
			s0 = _Timecounter_Sbinuptime();
		} while (s0 < s1);

		_Atomic_Store_uint(&ctx->state, T_INTERRUPT_TEST_ACTION,
		    ATOMIC_ORDER_RELAXED);
		T_busy(busy);
		(*config->action)(arg);

		state = (T_interrupt_test_state)
		    _Atomic_Exchange_uint(&ctx->state,
		    T_INTERRUPT_TEST_INITIAL, ATOMIC_ORDER_RELAXED);

		if (state == T_INTERRUPT_TEST_DONE) {
			break;
		}

		/* Adjust the lower/upper bound of the bisection interval */
		if (state == T_INTERRUPT_TEST_EARLY) {
			uint_fast32_t lower;

			upper_sum -= upper_bound[sample];
			upper_sum += busy;
			upper_bound[sample] = busy;

			/* Round down to make sure no underflow happens */
			lower = lower_bound[sample];
			delta = lower / 32;
			lower_sum -= delta;
			lower_bound[sample] = lower - delta;

			sample = (sample + 1) % T_INTERRUPT_SAMPLE_COUNT;
		} else if (state == T_INTERRUPT_TEST_LATE) {
			uint_fast32_t upper;

			lower_sum -= lower_bound[sample];
			lower_sum += busy;
			lower_bound[sample] = busy;

			/*
			 * The one tick busy count value is not really
			 * trustable on some platforms.  Allow the upper bound
			 * to grow over this value in time.
			 */
			upper = upper_bound[sample];
			delta = (upper + 31) / 32;
			upper_sum += delta;
			upper_bound[sample] = upper + delta;

			sample = (sample + 1) % T_INTERRUPT_SAMPLE_COUNT;
		}
	}

	T_pop_fixture();

	if (iter == config->max_iteration_count) {
		return T_INTERRUPT_TEST_TIMEOUT;
	}

	return T_INTERRUPT_TEST_DONE;
}

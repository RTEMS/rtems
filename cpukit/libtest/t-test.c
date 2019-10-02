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

#define _GNU_SOURCE

#include <t.h>

#include <sys/queue.h>
#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <setjmp.h>
#include <stdatomic.h>

#ifdef __rtems__
#include <rtems/score/io.h>
#include <rtems/score/percpu.h>
#include <rtems/score/smp.h>
#include <rtems/score/threadimpl.h>
#include <rtems/linkersets.h>
#include <rtems/version.h>
#else
#include "t-test-printf.h"
#endif /* __rtems__ */

#define T_LINE_SIZE 128

#define T_SCOPE_SIZE 5

typedef struct {
	pthread_spinlock_t lock;
	char *buf;
	unsigned int buf_mask;
	atomic_uint buf_head;
	atomic_uint buf_tail;
	void (*putchar)(int, void *);
	void *putchar_arg;
	T_verbosity verbosity;
	const T_case_context *registered_cases;
	const T_case_context *current_case;
	void *fixture_context;
	LIST_HEAD(, T_destructor) destructors;
	T_time case_begin_time;
	atomic_uint planned_steps;
	atomic_uint steps;
	atomic_uint failures;
	jmp_buf case_begin_context;
	unsigned int overall_cases;
	unsigned int overall_steps;
	unsigned int overall_failures;
	T_time run_begin_time;
#ifdef __rtems__
	Thread_Control *runner_thread;
	const Per_CPU_Control *runner_cpu;
#else
	bool runner_valid;
	pthread_t runner_thread;
#endif
	const T_config *config;
} T_context;

static T_context T_instance;

typedef struct {
	char *s;
	size_t n;
} T_putchar_string_context;

static void
T_putchar_string(int c, void *arg)
{
	T_putchar_string_context *sctx;
	char *s;
	size_t n;

	sctx = arg;
	s = sctx->s;
	n = sctx->n;

	if (n == 1) {
		c = '\0';
	}

	if (n > 1) {
		*s = (char)c;
		++s;
		--n;
	}

	sctx->s = s;
	sctx->n = n;
}

int
T_snprintf(char *s, size_t n, char const *fmt, ...)
{
	va_list ap;
	int len;
	T_putchar_string_context sctx = {
		.s = s,
		.n = n
	};

	va_start(ap, fmt);
	len = _IO_Vprintf(T_putchar_string, &sctx, fmt, ap);
	va_end(ap);

	if (sctx.n > 0) {
		*sctx.s = '\0';
	}

	return len;
}

static int
T_vprintf_direct(char const *fmt, va_list ap)
{
	T_context *ctx;
	unsigned int head;
	unsigned int tail;

	ctx = &T_instance;

	head = atomic_load_explicit(&ctx->buf_head, memory_order_acquire);
	tail = atomic_load_explicit(&ctx->buf_tail, memory_order_relaxed);

	while (head != tail) {
		(*ctx->putchar)(ctx->buf[tail], ctx->putchar_arg);
		tail = (tail + 1) & ctx->buf_mask;
	}

	atomic_store_explicit(&ctx->buf_tail, tail, memory_order_relaxed);

	return _IO_Vprintf(ctx->putchar, ctx->putchar_arg, fmt, ap);
}

static int
T_vprintf_buffered(char const *fmt, va_list ap)
{
	unsigned int len;
	T_context *ctx;
	char buf[T_LINE_SIZE];
	T_putchar_string_context sctx = {
		.s = buf,
		.n = sizeof(buf)
	};
	unsigned int head;
	unsigned int tail;
	unsigned int mask;
	unsigned int capacity;

	len = (unsigned int)_IO_Vprintf(T_putchar_string, &sctx, fmt, ap);

	if (len >= sizeof(buf)) {
		len = sizeof(buf) - 1;
	}

	ctx = &T_instance;
	pthread_spin_lock(&ctx->lock);
	head = atomic_load_explicit(&ctx->buf_head, memory_order_relaxed);
	tail = atomic_load_explicit(&ctx->buf_tail, memory_order_relaxed);
	mask = ctx->buf_mask;
	capacity = (tail - head - 1) & mask;

	if (len <= capacity) {
		unsigned int todo;
		char *c;

		todo = len;
		c = buf;

		while (todo > 0) {
			ctx->buf[head] = *c;
			head = (head + 1) & mask;
			--todo;
			++c;
		}

		atomic_store_explicit(&ctx->buf_head, head,
		    memory_order_release);
	} else {
		/* If it does not fit into the buffer, discard everything */
		len = 0;
	}

	pthread_spin_unlock(&ctx->lock);
	return (int)len;
}

int
T_vprintf(char const *fmt, va_list ap)
{
	int len;

	if (T_is_runner()) {
		len = T_vprintf_direct(fmt, ap);
	} else {
		len = T_vprintf_buffered(fmt, ap);
	}

	return len;
}

static int
T_cpu(void)
{
#if defined(__rtems__)
	return (int)_SMP_Get_current_processor();
#elif defined(__linux__)
	return sched_getcpu();
#else
	return 0;
#endif
}

#if defined(__rtems__)
static const char *
T_object_name_to_string(Objects_Name name, char *buf)
{
	uint32_t on;
	size_t i;
	int s;

	on = name.name_u32;
	i = 0;

	for (s = 24; s >= 0; s -= 8) {
		unsigned char c;

		c = (unsigned char)(on >> s);

		if (c >= '!' && c <= '~') {
			buf[i] = (char)c;
			++i;
		}
	}

	buf[i] = '\0';
	return buf;
}

static const char *
T_thread_name(const Thread_Control *th, char *buf)
{
	if (th != NULL) {
		const char *name;

		name = th->Join_queue.Queue.name;

		if (name != NULL && name[0] != '\0') {
			return name;
		} else {
			return T_object_name_to_string(th->Object.name, buf);
		}
	} else {
		buf[0] = '?';
		buf[1] = '\0';
		return buf;
	}
}
#endif

static const char *
T_scope(char *buf)
{
	const char *r;

#if defined(__rtems__)
	ISR_Level level;
	const Per_CPU_Control *cpu_self;

	_ISR_Local_disable(level);
	cpu_self = _Per_CPU_Get();

	if (cpu_self->isr_nest_level == 0) {
		Thread_Control *executing;

		executing = _Per_CPU_Get_executing(cpu_self);
		_ISR_Local_enable(level);
		r = T_thread_name(executing, buf);
	} else {
		_ISR_Local_enable(level);
		buf[0] = 'I';
		buf[1] = 'S';
		buf[2] = 'R';
		buf[3] = '\0';
		r = buf;
	}
#elif defined(__linux__)
	static __thread char name[128];

	(void)buf;

	if (name[0] == '\0') {
		pthread_getname_np(pthread_self(), name, sizeof(name));
	}

	r = &name[0];
#else
	buf[0] = '?';
	buf[1] = '\0';
	r = buf;
#endif

	return r;
}

static void
T_set_runner(T_context *ctx)
{
#ifdef __rtems__
	ISR_Level level;
	const Per_CPU_Control *cpu_self;

	_ISR_Local_disable(level);
	cpu_self = _Per_CPU_Get();
	ctx->runner_cpu = cpu_self;

	if (cpu_self->isr_nest_level == 0) {
		ctx->runner_thread = _Per_CPU_Get_executing(cpu_self);
	} else {
		ctx->runner_thread = NULL;
	}

	_ISR_Local_enable(level);
#else
	ctx->runner_valid = true;
	ctx->runner_thread = pthread_self();
#endif
}

int
T_printf(char const *fmt, ...)
{
	va_list ap;
	int len;

	va_start(ap, fmt);
	len = T_vprintf(fmt, ap);
	va_end(ap);

	return len;
}

void
T_log(T_verbosity verbosity, char const *fmt, ...)
{
	T_context *ctx;

	ctx = &T_instance;

	if (ctx->verbosity >= verbosity) {
		va_list ap;

		T_printf("L:");
		va_start(ap, fmt);
		T_vprintf(fmt, ap);
		va_end(ap);
		T_printf("\n");
	}
}

static unsigned int
T_fetch_add_step(T_context *ctx)
{
	return atomic_fetch_add_explicit(&ctx->steps, 1, memory_order_relaxed);
}

static unsigned int
T_add_failure(T_context *ctx)
{
	return atomic_fetch_add_explicit(&ctx->failures, 1,
	    memory_order_relaxed);
}

static void
T_stop(T_context *ctx)
{
	const T_case_context *tc;

	tc = ctx->current_case;

	if (tc != NULL) {
		const T_fixture *fixture;

		fixture = tc->fixture;

		if (fixture != NULL && fixture->stop != NULL) {
			(*fixture->stop)(ctx->fixture_context);
		}
	}

	longjmp(ctx->case_begin_context, 1);
}

void T_plan(unsigned int planned_steps)
{
	T_context *ctx;
	unsigned int expected;
	bool success;

	ctx = &T_instance;
	expected = UINT_MAX;
	success = atomic_compare_exchange_strong_explicit(&ctx->planned_steps,
	    &expected, planned_steps, memory_order_relaxed,
	    memory_order_relaxed);
	T_check_true(success, NULL, "planned steps (%u) already set", expected);
}

void
T_case_register(T_case_context *tc)
{
	T_context *ctx;

	ctx = &T_instance;
	tc->next = ctx->registered_cases;
	ctx->registered_cases = tc;
}

T_verbosity
T_set_verbosity(T_verbosity verbosity)
{
	T_context *ctx;
	T_verbosity previous;

	ctx = &T_instance;
	previous = ctx->verbosity;
	ctx->verbosity = verbosity;

	return previous;
}

void *
T_fixture_context(void)
{
	return T_instance.fixture_context;
}

void
T_set_fixture_context(void *context)
{
	T_instance.fixture_context = context;
}

const char *
T_case_name(void)
{
	const T_case_context *tc;

	tc = T_instance.current_case;

	if (tc != NULL) {
		return tc->name;
	} else {
		return "?";
	}
}

void
T_check_true(bool ok, const T_check_context *t, const char *fmt, ...)
{
	T_context *ctx;
	va_list ap;
	char scope[T_SCOPE_SIZE];

	ctx = &T_instance;

	if (t != NULL) {
		unsigned int step;

		if ((t->flags & T_CHECK_QUIET) == 0) {
			step = T_fetch_add_step(ctx);
		} else {
			step = UINT_MAX;
		}

		if ((t->flags & T_CHECK_STEP_FLAG) != 0 &&
		     step != T_CHECK_STEP_FROM_FLAGS(t->flags)) {
			T_add_failure(ctx);
			T_printf("F:%u:%i:%s:%s:%i:planned step (%u)\n", step,
			    T_cpu(), T_scope(scope), t->file, t->line,
			    T_CHECK_STEP_FROM_FLAGS(t->flags));
		} else if (!ok) {
			T_add_failure(ctx);

			if (ctx->verbosity >= T_NORMAL) {
				if ((t->flags & T_CHECK_QUIET) == 0) {
					T_printf("F:%u:%i:%s:%s:%i:",
					    step, T_cpu(), T_scope(scope),
					    t->file, t->line);
				} else {
					T_printf("F:*:%i:%s:%s:%i:", T_cpu(),
					    T_scope(scope), t->file, t->line);
				}

				va_start(ap, fmt);
				T_vprintf(fmt, ap);
				va_end(ap);

				T_printf("\n");
			}

			if ((t->flags & T_CHECK_STOP) != 0) {
				T_stop(ctx);
			}
		} else if ((t->flags & T_CHECK_QUIET) == 0 &&
		    ctx->verbosity >= T_VERBOSE) {
			T_printf("P:%u:%i:%s:%s:%i\n", step, T_cpu(),
			    T_scope(scope), t->file, t->line);
		}
	} else if (!ok) {
		T_add_failure(ctx);

		T_printf("F:*:%i:%s:*:*:", T_cpu(), T_scope(scope));

		va_start(ap, fmt);
		T_vprintf(fmt, ap);
		va_end(ap);

		T_printf("\n");
	}
}

static void
T_do_log(T_context *ctx, T_verbosity verbosity, char const *fmt, ...)
{
	if (ctx->verbosity >= verbosity) {
		va_list ap;

		va_start(ap, fmt);
		T_vprintf(fmt, ap);
		va_end(ap);
	}
}

static void
T_system(T_context *ctx)
{
#if defined(__rtems__)
	T_do_log(ctx, T_QUIET, "S:Platform:RTEMS\n");
	T_do_log(ctx, T_QUIET, "S:Compiler:" __VERSION__ "\n");
	T_do_log(ctx, T_QUIET, "S:Version:%s\n", rtems_version());
	T_do_log(ctx, T_QUIET, "S:BSP:%s\n", rtems_board_support_package());
#if RTEMS_DEBUG
	T_do_log(ctx, T_QUIET, "S:RTEMS_DEBUG:1\n");
#else
	T_do_log(ctx, T_QUIET, "S:RTEMS_DEBUG:0\n");
#endif
#if RTEMS_MULTIPROCESSING
	T_do_log(ctx, T_QUIET, "S:RTEMS_MULTIPROCESSING:1\n");
#else
	T_do_log(ctx, T_QUIET, "S:RTEMS_MULTIPROCESSING:0\n");
#endif
#if RTEMS_POSIX_API
	T_do_log(ctx, T_QUIET, "S:RTEMS_POSIX_API:1\n");
#else
	T_do_log(ctx, T_QUIET, "S:RTEMS_POSIX_API:0\n");
#endif
#if RTEMS_PROFILING
	T_do_log(ctx, T_QUIET, "S:RTEMS_PROFILING:1\n");
#else
	T_do_log(ctx, T_QUIET, "S:RTEMS_PROFILING:0\n");
#endif
#if RTEMS_SMP
	T_do_log(ctx, T_QUIET, "S:RTEMS_SMP:1\n");
#else
	T_do_log(ctx, T_QUIET, "S:RTEMS_SMP:0\n");
#endif
#elif defined(__linux__)
	T_do_log(ctx, T_QUIET, "S:Platform:Linux\n");
	T_do_log(ctx, T_QUIET, "S:Compiler:" __VERSION__ "\n");
#else
	T_do_log(ctx, T_QUIET, "S:Platform:POSIX\n");
#ifdef __VERSION__
	T_do_log(ctx, T_QUIET, "S:Compiler:" __VERSION__ "\n");
#endif
#endif
}

void
T_add_destructor(T_destructor *dtor, void (*destroy)(T_destructor *))
{
	T_context *ctx;

	dtor->destroy = destroy;
	ctx = &T_instance;
	pthread_spin_lock(&ctx->lock);
	LIST_INSERT_HEAD(&ctx->destructors, dtor, node);
	pthread_spin_unlock(&ctx->lock);
}

void
T_remove_destructor(T_destructor *dtor)
{
	T_context *ctx;

	ctx = &T_instance;
	pthread_spin_lock(&ctx->lock);
	LIST_REMOVE(dtor, node);
	pthread_spin_unlock(&ctx->lock);
}

static void
T_call_destructors(const T_context *ctx)
{
	T_destructor *dtor;

#ifdef __linux__
	while (!LIST_EMPTY(&ctx->destructors)) {
		dtor = LIST_FIRST(&ctx->destructors);
		LIST_REMOVE(dtor, node);
		(*dtor->destroy)(dtor);
	}
#else
	T_destructor *tmp;

	LIST_FOREACH_SAFE(dtor, &ctx->destructors, node, tmp) {
		(*dtor->destroy)(dtor);
	}
#endif
}

static void
T_actions_forward(const T_config *config, T_event event, const char *name)
{
	const T_action *actions;
	size_t n;
	size_t i;

	actions = config->actions;
	n = config->action_count;

	for (i = 0; i < n; ++i) {
		(*actions[i])(event, name);
	}
}

static void
T_actions_backward(const T_config *config, T_event event,
    const char *name)
{
	const T_action *actions;
	size_t n;
	size_t i;

	actions = config->actions;
	n = config->action_count;

	for (i = 0; i < n; ++i) {
		(*actions[n - i - 1])(event, name);
	}
}

static T_context *
T_do_run_initialize(const T_config *config)
{
	T_context *ctx;

	ctx = &T_instance;

	pthread_spin_init(&ctx->lock, PTHREAD_PROCESS_PRIVATE);

	ctx->config = config;
	ctx->buf = config->buf;

	if (config->buf_size > 0 &&
	    (config->buf_size & (config->buf_size - 1)) == 0) {
		ctx->buf_mask = config->buf_size - 1;
	} else {
		ctx->buf_mask = 0;
	}

	atomic_store_explicit(&ctx->buf_head, 0, memory_order_relaxed);
	ctx->buf_tail = 0;
	ctx->putchar = config->putchar;
	ctx->putchar_arg = config->putchar_arg;
	ctx->verbosity = config->verbosity;
	atomic_store_explicit(&ctx->steps, 0, memory_order_relaxed);
	atomic_store_explicit(&ctx->failures, 0, memory_order_relaxed);
	ctx->overall_cases = 0;
	ctx->overall_steps = 0;
	ctx->overall_failures = 0;

	T_set_runner(ctx);
	T_actions_forward(config, T_EVENT_RUN_INITIALIZE_EARLY, config->name);
	T_do_log(ctx, T_QUIET, "A:%s\n", config->name);
	T_system(ctx);
	ctx->run_begin_time = (*config->now)();
	T_actions_forward(config, T_EVENT_RUN_INITIALIZE_LATE, config->name);

	return ctx;
}

static void
T_do_case_begin(T_context *ctx, const T_case_context *tc)
{
	const T_config *config;
	const T_fixture *fixture;

	config = ctx->config;
	fixture = tc->fixture;
	ctx->verbosity = config->verbosity;
	ctx->current_case = tc;
	LIST_INIT(&ctx->destructors);
	atomic_store_explicit(&ctx->planned_steps, UINT_MAX,
	    memory_order_relaxed);
	atomic_store_explicit(&ctx->steps, 0, memory_order_relaxed);
	atomic_store_explicit(&ctx->failures, 0, memory_order_relaxed);

	T_actions_forward(config, T_EVENT_CASE_EARLY, tc->name);
	T_do_log(ctx, T_NORMAL, "B:%s\n", tc->name);
	ctx->case_begin_time = (*config->now)();
	T_actions_forward(config, T_EVENT_CASE_BEGIN, tc->name);

	if (fixture != NULL) {
		ctx->fixture_context = fixture->initial_context;

		if (fixture->setup != NULL) {
			(*fixture->setup)(ctx->fixture_context);
		}
	}
}

static void
T_do_case_end(T_context *ctx, const T_case_context *tc)
{
	const T_config *config;
	const T_fixture *fixture;
	unsigned int planned_steps;
	unsigned int steps;
	unsigned int failures;
	T_time delta;
	T_time_string ts;

	config = ctx->config;
	fixture = tc->fixture;

	if (fixture != NULL && fixture->teardown != NULL) {
		(*fixture->teardown)(ctx->fixture_context);
	}

	T_call_destructors(ctx);
	T_actions_backward(config, T_EVENT_CASE_END, tc->name);

	planned_steps = atomic_fetch_add_explicit(&ctx->planned_steps,
	    0, memory_order_relaxed);
	steps = atomic_fetch_add_explicit(&ctx->steps, 0,
	    memory_order_relaxed);
	failures = atomic_fetch_add_explicit(&ctx->failures, 0,
	    memory_order_relaxed);

	if (planned_steps != UINT_MAX && planned_steps != steps &&
	    failures == 0) {
		++failures;

		if (ctx->verbosity >= T_NORMAL) {
			char scope[T_SCOPE_SIZE];

			T_printf("F:*:%i:%s:*:*:actual steps (%u), "
			    "planned steps (%u)\n", T_cpu(),
			    T_scope(scope), steps, planned_steps);
		}
	}

	delta = (*config->now)() - ctx->case_begin_time;
	T_do_log(ctx, T_QUIET, "E:%s:N:%u:F:%u:D:%s\n",
	    tc->name, steps, failures, T_time_to_string_us(delta, ts));

	++ctx->overall_cases;
	ctx->overall_steps += steps;
	ctx->overall_failures += failures;

	T_actions_backward(config, T_EVENT_CASE_LATE, tc->name);
}

static void
T_run_case(T_context *ctx, const T_case_context *tc)
{
	T_do_case_begin(ctx, tc);

	if (setjmp(ctx->case_begin_context) == 0) {
		(*tc->body)();
	}

	T_do_case_end(ctx, tc);
}

static void
T_do_run_all(T_context *ctx)
{
	const T_case_context *tc;

	tc = ctx->registered_cases;

	while (tc != NULL) {
		T_run_case(ctx, tc);
		tc = tc->next;
	}
}

static bool
T_do_run_finalize(T_context *ctx)
{
	const T_config *config;
	T_time delta;
	T_time_string ts;

	config = ctx->config;
	T_actions_backward(config, T_EVENT_RUN_FINALIZE_EARLY, config->name);
	delta = (*config->now)() - ctx->run_begin_time;
	T_do_log(ctx, T_QUIET, "Z:%s:C:%u:N:%u:F:%u:D:%s\n", config->name,
	    ctx->overall_cases, ctx->overall_steps, ctx->overall_failures,
	    T_time_to_string_us(delta, ts));
	T_actions_backward(config, T_EVENT_RUN_FINALIZE_LATE, config->name);
#ifdef __rtems__
	ctx->runner_thread = NULL;
	ctx->runner_cpu = NULL;
#else
	ctx->runner_valid = false;
#endif
	pthread_spin_destroy(&ctx->lock);

	return ctx->overall_failures == 0;
}

int
T_main(const T_config *config)
{
	T_context *ctx;

	ctx = T_do_run_initialize(config);
	T_do_run_all(ctx);

	return T_do_run_finalize(ctx) ? 0 : 1;
}

bool T_is_runner(void)
{
	T_context *ctx;
	bool is_runner;
#ifdef __rtems__
	ISR_Level level;
	const Per_CPU_Control *cpu_self;
#endif

	ctx = &T_instance;
#ifdef __rtems__
	_ISR_Local_disable(level);
	cpu_self = _Per_CPU_Get();

	if (ctx->runner_thread != NULL) {
		is_runner = cpu_self->isr_nest_level == 0 &&
		    _Per_CPU_Get_executing(cpu_self) == ctx->runner_thread;
	} else {
		is_runner = cpu_self == ctx->runner_cpu;
	}

	_ISR_Local_enable(level);
#else
	is_runner = ctx->runner_valid &&
	    pthread_equal(pthread_self(), ctx->runner_thread) != 0;
#endif

	return is_runner;
}

#ifdef __rtems__
RTEMS_LINKER_ROSET(_T, T_case_context *);
#endif /* __rtems__ */

void T_register(void)
{
#ifdef __rtems__
	T_case_context * const *tc;

	RTEMS_LINKER_SET_FOREACH(_T, tc) {
		T_case_register(*tc);
	}
#endif /* __rtems__ */
}

void
T_run_initialize(const T_config *config)
{
	(void)T_do_run_initialize(config);
}

void
T_run_all(void)
{
	T_do_run_all(&T_instance);
}

void
T_run_by_name(const char *name)
{
	T_context *ctx;
	const T_case_context *tc;

	ctx = &T_instance;
	tc = ctx->registered_cases;

	while (tc != NULL) {
		if (strcmp(tc->name, name) == 0) {
			T_run_case(ctx, tc);
			break;
		}

		tc = tc->next;
	}
}

static T_case_context default_case;

void
T_case_begin(const char *name, const T_fixture *fixture)
{
	T_case_context *tc;

	tc = &default_case;
	tc->name = name;
	tc->fixture = fixture;
	T_do_case_begin(&T_instance, tc);
}

void
T_case_end(void)
{
	T_case_context *tc;

	tc = &default_case;
	T_do_case_end(&T_instance, tc);
}

bool
T_run_finalize(void)
{
	return T_do_run_finalize(&T_instance);
}

T_time
T_case_begin_time(void)
{
	return T_instance.case_begin_time;
}

void
T_set_putchar(T_putchar new_putchar, void *new_arg, T_putchar *old_putchar,
    void **old_arg)
{
	T_context *ctx;

	ctx = &T_instance;
	*old_putchar = ctx->putchar;
	*old_arg = ctx->putchar_arg;
	ctx->putchar = new_putchar;
	ctx->putchar_arg = new_arg;
}

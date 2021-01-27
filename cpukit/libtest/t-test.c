/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2018, 2020 embedded brains GmbH
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

#include <rtems/test.h>

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
	T_fixture_node *fixtures;
	T_fixture_node case_fixture;
	LIST_HEAD(, T_destructor) destructors;
	T_time case_begin_time;
	atomic_uint planned_steps;
	atomic_uint steps;
	atomic_uint failures;
	bool case_begin_context_valid;
	jmp_buf case_begin_context;
	unsigned int fixture_steps;
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

const T_check_context T_special = {
	.file = "*",
	.line = -1,
	.flags = T_CHECK_FMT | T_CHECK_QUIET
};

static bool
T_do_is_runner(T_context *ctx)
{
	bool is_runner;
#ifdef __rtems__
	ISR_Level level;
	const Per_CPU_Control *cpu_self;
#endif

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

bool T_is_runner(void)
{
	return T_do_is_runner(&T_instance);
}

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

static void
T_output_buffer_drain(T_context *ctx)
{
	unsigned int head;
	unsigned int tail;

	head = atomic_load_explicit(&ctx->buf_head, memory_order_acquire);
	tail = atomic_load_explicit(&ctx->buf_tail, memory_order_relaxed);

	while (head != tail) {
		(*ctx->putchar)(ctx->buf[tail], ctx->putchar_arg);
		tail = (tail + 1) & ctx->buf_mask;
	}

	atomic_store_explicit(&ctx->buf_tail, tail, memory_order_relaxed);
}

static unsigned int
T_output_buffer_fill(T_context *ctx, const char *buf, unsigned int len)
{
	unsigned int head;
	unsigned int tail;
	unsigned int mask;
	unsigned int capacity;

	pthread_spin_lock(&ctx->lock);
	head = atomic_load_explicit(&ctx->buf_head, memory_order_relaxed);
	tail = atomic_load_explicit(&ctx->buf_tail, memory_order_relaxed);
	mask = ctx->buf_mask;
	capacity = (tail - head - 1) & mask;

	if (len <= capacity) {
		unsigned int todo;
		const char *c;

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
	return len;
}

static int
T_vprintf_direct(char const *fmt, va_list ap)
{
	T_context *ctx;

	ctx = &T_instance;
	T_output_buffer_drain(ctx);
	return _IO_Vprintf(ctx->putchar, ctx->putchar_arg, fmt, ap);
}

static int
T_vprintf_buffered(char const *fmt, va_list ap)
{
	char buf[T_LINE_SIZE];
	T_putchar_string_context sctx = {
		.s = buf,
		.n = sizeof(buf)
	};
	unsigned int len;

	len = (unsigned int)_IO_Vprintf(T_putchar_string, &sctx, fmt, ap);

	if (len >= sizeof(buf)) {
		len = sizeof(buf) - 1;
	}

	return (int)T_output_buffer_fill(&T_instance, buf, len);
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
T_do_puts(T_context *ctx, const char *buf, size_t len)
{
	if (T_do_is_runner(ctx)) {
		size_t i;

		T_output_buffer_drain(ctx);

		for (i = 0; i < len; ++i) {
			(*ctx->putchar)(buf[i], ctx->putchar_arg);
		}
	} else {
		len = T_output_buffer_fill(ctx, buf, len);
	}

	return (int)len;
}

int
T_puts(const char *buf, size_t len)
{
	return T_do_puts(&T_instance, buf, len);
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

size_t
T_str_copy(char *dst, const char *src, size_t n)
{
	size_t i;

	i = 0;

	while (*src != '\0' && i < n) {
		*dst = *src;
		++dst;
		++src;
		++i;
	}

	return i;
}

#if defined(__rtems__)
static size_t
T_object_name_to_string(char *dst, Objects_Name name, size_t n)
{
	uint32_t on;
	size_t i;
	int s;

	on = name.name_u32;
	i = 0;

	for (s = 24; s >= 0; s -= 8) {
		unsigned char c;

		c = (unsigned char)(on >> s);

		if (c >= '!' && c <= '~' && i < n) {
			*dst = (char)c;
			++dst;
			++i;
		}
	}

	return i;
}

static size_t
T_thread_name(char *dst, const Thread_Control *th, size_t n)
{
	if (th != NULL) {
		const char *name;

		name = th->Join_queue.Queue.name;

		if (name != NULL && name[0] != '\0') {
			return T_str_copy(dst, name, n);
		}

		return T_object_name_to_string(dst, th->Object.name, n);
	}

	return T_str_copy(dst, "?", n);
}
#endif

static size_t
T_scope(T_context *ctx, char *dst, size_t n)
{
	T_fixture_node *node;
	size_t len;

#if defined(__rtems__)
	ISR_Level level;
	const Per_CPU_Control *cpu_self;

	_ISR_Local_disable(level);
	cpu_self = _Per_CPU_Get();

	if (cpu_self->isr_nest_level == 0) {
		Thread_Control *executing;

		executing = _Per_CPU_Get_executing(cpu_self);
		_ISR_Local_enable(level);
		len = T_thread_name(dst, executing, n);
	} else {
		_ISR_Local_enable(level);
		len = T_str_copy(dst, "ISR", n);
	}

#elif defined(__linux__)
	static __thread char name[128];

	if (name[0] == '\0') {
		pthread_getname_np(pthread_self(), name, sizeof(name));
	}

	len = T_str_copy(dst, name, n);
#else
	len = T_str_copy(dst, "?", n);
#endif

	dst += len;
	n -= len;
	node = &ctx->case_fixture;

	do {
		const T_fixture *fixture;

		fixture = node->fixture;

		if (fixture != NULL && fixture->scope != NULL) {
			size_t m;

			m = (*fixture->scope)(node->context, dst, n);
			dst += m;
			n -= m;
			len += m;
		}

		node = node->previous;
	} while (node != NULL);

	return len;
}

static void
T_do_make_runner(T_context *ctx)
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

void
T_make_runner(void)
{
	T_do_make_runner(&T_instance);
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

T_NO_RETURN static void
T_do_stop(T_context *ctx)
{
	T_fixture_node *node;

	node = ctx->fixtures;

	while (node != NULL) {
		const T_fixture *fixture;
		void *node_context;

		fixture = node->fixture;
		node_context = node->context;
		node = node->next;

		if (fixture != NULL && fixture->stop != NULL) {
			(*fixture->stop)(node_context);
		}
	}

	if (T_do_is_runner(ctx) && ctx->case_begin_context_valid) {
		longjmp(ctx->case_begin_context, 1);
	} else {
		T_actions_backward(ctx->config, T_EVENT_CASE_STOP,
		    T_case_name());
#ifdef __GNUC__
		__builtin_unreachable();
#endif
	}
}

T_NO_RETURN void
T_stop(void)
{
	T_do_stop(&T_instance);
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
	T_check(&T_special, success, "planned steps (%u) already set",
	    expected);
}

const T_fixture T_empty_fixture;

void
T_push_plan(T_fixture_node *node, unsigned int planned_steps)
{
	T_push_fixture(node, &T_empty_fixture);
	T_plan(planned_steps);
}

void
T_pop_plan(void)
{
	T_pop_fixture();
}

void
T_check_step(const T_check_context *t, unsigned int expected)
{
	T_check_context tt;

	tt = *t;
	tt.flags |= T_CHECK_STEP(expected);
	T_check(&tt, true, "actual step is not equal to expected step (%u)",
	    expected);
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
	return T_instance.fixtures->context;
}

void
T_set_fixture_context(void *context)
{
	T_instance.fixtures->context = context;
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

static const char *
T_file(const T_check_context *t)
{
	const char *file;

	file = strrchr(t->file, '/');

	if (file == NULL) {
		return t->file;
	}

	return file + 1;
}

static const char T_planned_step_fmt[] = "planned step (%u)";

static void
T_check_putc(int c, void *arg)
{
	T_putchar_string_context *sctx;
	size_t n;

	sctx = arg;
	n = sctx->n;

	if (n > 0) {
		char *s;

		s = sctx->s;
		*s = (char)c;
		sctx->s = s + 1;
		sctx->n = n - 1;
	}
}

static void
T_check_print_steps(T_context *ctx, T_putchar_string_context *sctx,
    unsigned int step)
{
	T_fixture_node *node;

	node = &ctx->case_fixture;

	while (true) {
		node = node->previous;

		if (node != NULL) {
			_IO_Printf(T_check_putc, sctx, "%u.",
			    node->next_steps);
		} else {
			break;
		}
	}

	if (step != UINT_MAX) {
		_IO_Printf(T_check_putc, sctx, "%u", step);
	} else {
		T_check_putc('*', sctx);
	}
}

void
T_check(const T_check_context *t, bool ok, ...)
{
	T_context *ctx;
	va_list ap;
	char line[T_LINE_SIZE];
	unsigned int step;
	int line_number;
	const char *fmt;

	ctx = &T_instance;

	if ((t->flags & T_CHECK_QUIET) == 0) {
		step = T_fetch_add_step(ctx);
	} else {
		step = UINT_MAX;
	}

	va_start(ap, ok);
	line[0] = '\0';
	line_number = -1;
	fmt = NULL;

	if ((t->flags & T_CHECK_STEP_FLAG) != 0 &&
	     step != T_CHECK_STEP_FROM_FLAGS(t->flags)) {
		T_add_failure(ctx);
		line[0] = 'F';
		line_number = t->line;
		fmt = T_planned_step_fmt;
	} else if (!ok) {
		T_add_failure(ctx);

		if (ctx->verbosity >= T_NORMAL) {
			line[0] = 'F';
			line_number = t->line;

			if ((t->flags & T_CHECK_FMT) != 0) {
				fmt = va_arg(ap, const char *);
			}
		}
	} else if ((t->flags & T_CHECK_QUIET) == 0 &&
	    ctx->verbosity >= T_VERBOSE) {
		line[0] = 'P';
		line_number = t->line;
	}

	if (line[0] != '\0') {
		T_putchar_string_context sctx;
		size_t chunk;

		sctx.n = sizeof(line) - 1;
		sctx.s = &line[1];
		T_check_putc(':', &sctx);
		T_check_print_steps(ctx, &sctx, step);
		_IO_Printf(T_check_putc, &sctx, ":%i:", T_cpu());
		chunk = T_scope(ctx, sctx.s, sctx.n);
		sctx.s += chunk;
		sctx.n -= chunk;
		T_check_putc(':', &sctx);
		chunk = T_str_copy(sctx.s, T_file(t), sctx.n);
		sctx.s += chunk;
		sctx.n -= chunk;
		T_check_putc(':', &sctx);

		if (line_number >= 0) {
			_IO_Printf(T_check_putc, &sctx, "%i", line_number);
		} else {
			T_check_putc('*', &sctx);
		}

		if (fmt != NULL) {
			if (fmt == T_planned_step_fmt) {
				T_check_putc(':', &sctx);
				_IO_Printf(T_check_putc, &sctx, fmt,
				    T_CHECK_STEP_FROM_FLAGS(t->flags));
			} else {
				T_check_putc(':', &sctx);
				_IO_Vprintf(T_check_putc, &sctx, fmt, ap);
			}
		}

		T_check_putc('\n', &sctx);
		line[sizeof(line) - 1] = '\n';
		T_puts(&line[0], sizeof(line) - sctx.n);
	}

	if (!ok && (t->flags & T_CHECK_STOP) != 0) {
		T_do_stop(ctx);
	}

	va_end(ap);
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

	ctx->fixtures = &ctx->case_fixture;
	atomic_store_explicit(&ctx->buf_head, 0, memory_order_relaxed);
	ctx->buf_tail = 0;
	ctx->putchar = config->putchar;
	ctx->putchar_arg = config->putchar_arg;
	ctx->verbosity = config->verbosity;
	ctx->overall_cases = 0;
	ctx->overall_steps = 0;
	ctx->overall_failures = 0;

	T_do_make_runner(ctx);
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
	ctx->fixtures = &ctx->case_fixture;
	LIST_INIT(&ctx->destructors);
	atomic_store_explicit(&ctx->planned_steps, UINT_MAX,
	    memory_order_relaxed);
	atomic_store_explicit(&ctx->steps, 0, memory_order_relaxed);
	atomic_store_explicit(&ctx->failures, 0, memory_order_relaxed);
	ctx->fixture_steps = 0;

	T_actions_forward(config, T_EVENT_CASE_EARLY, tc->name);
	T_do_log(ctx, T_NORMAL, "B:%s\n", tc->name);
	ctx->case_begin_time = (*config->now)();
	T_actions_forward(config, T_EVENT_CASE_BEGIN, tc->name);

	if (fixture != NULL) {
		ctx->case_fixture.fixture = fixture;
		ctx->case_fixture.context = fixture->initial_context;

		if (fixture->setup != NULL) {
			(*fixture->setup)(ctx->case_fixture.context);
		}
	}
}

static void
T_check_steps(unsigned int planned_steps, unsigned int steps,
    unsigned int failures)
{
	if (planned_steps != UINT_MAX && planned_steps != steps &&
	    failures == 0) {
		T_check(&T_special, false, "actual steps (%u), "
		    "planned steps (%u)", steps, planned_steps);
	}
}

static void T_do_pop_fixture(T_context *);

static void
T_do_case_end(T_context *ctx, const T_case_context *tc)
{
	const T_config *config;
	unsigned int planned_steps;
	unsigned int steps;
	unsigned int failures;
	T_time delta;
	T_time_string ts;

	while (ctx->fixtures != NULL) {
		T_do_pop_fixture(ctx);
	}

	T_call_destructors(ctx);
	config = ctx->config;
	T_actions_backward(config, T_EVENT_CASE_END, tc->name);

	planned_steps = atomic_fetch_add_explicit(&ctx->planned_steps,
	    0, memory_order_relaxed);
	steps = atomic_fetch_add_explicit(&ctx->steps, 0,
	    memory_order_relaxed);
	failures = atomic_fetch_add_explicit(&ctx->failures, 0,
	    memory_order_relaxed);
	T_check_steps(planned_steps, steps, failures);

	failures = atomic_load_explicit(&ctx->failures, memory_order_relaxed);
	delta = (*config->now)() - ctx->case_begin_time;
	steps += ctx->fixture_steps;
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
	if (setjmp(ctx->case_begin_context) == 0) {
		ctx->case_begin_context_valid = true;
		T_do_case_begin(ctx, tc);
		(*tc->body)();
	}

	ctx->case_begin_context_valid = false;
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

T_time
T_now(void)
{
	T_context *ctx;
	const T_config *config;

	ctx = &T_instance;
	config = ctx->config;
	return (*config->now)();
}

void *
T_push_fixture(T_fixture_node *node, const T_fixture *fixture)
{
	T_context *ctx;
	T_fixture_node *old;
	void *context;

	ctx = &T_instance;
	old = ctx->fixtures;
	old->previous = node;
	node->next = old;
	node->previous = NULL;
	node->fixture = fixture;
	context = fixture->initial_context;
	node->context = context;
	node->next_planned_steps = atomic_exchange_explicit(
	    &ctx->planned_steps, UINT_MAX, memory_order_relaxed);
	node->next_steps = atomic_exchange_explicit(&ctx->steps, 0,
	    memory_order_relaxed);
	node->failures = atomic_fetch_add_explicit(&ctx->failures, 0,
	    memory_order_relaxed);
	ctx->fixtures = node;

	if (fixture->setup != NULL) {
		(*fixture->setup)(context);
	}

	return context;
}

static void
T_do_pop_fixture(T_context *ctx)
{
	T_fixture_node *node;
	const T_fixture *fixture;
	T_fixture_node *next;

	node = ctx->fixtures;
	next = node->next;
	ctx->fixtures = next;
	fixture = node->fixture;

	if (fixture != NULL && fixture->teardown != NULL) {
		(*fixture->teardown)(node->context);
	}

	if (next != NULL) {
		unsigned int planned_steps;
		unsigned int steps;
		unsigned int failures;

		next->previous = NULL;
		planned_steps = atomic_exchange_explicit(&ctx->planned_steps,
		    node->next_planned_steps, memory_order_relaxed);
		steps = atomic_exchange_explicit(&ctx->steps, node->next_steps,
		    memory_order_relaxed);
		failures = atomic_fetch_add_explicit(&ctx->failures, 0,
		    memory_order_relaxed);
		ctx->fixture_steps += steps;
		T_check_steps(planned_steps, steps, node->failures - failures);
	}

	memset(node, 0, sizeof(*node));
}

void
T_pop_fixture(void)
{
	T_do_pop_fixture(&T_instance);
}

size_t
T_get_scope(const char * const * const *desc, char *buf, size_t n,
    const size_t *second_indices)
{
	size_t c;
	size_t i;

	c = n;
	i = 0;

	while (true) {
		const char * const *desc2;
		size_t m;

		desc2 = desc[i];

		if (desc2 == NULL) {
			break;
		}

		if (c > 1) {
			buf[0] = '/';
			--c;
			++buf;
		} else {
			break;
		}

		m = T_str_copy(buf, desc2[second_indices[i]], c);
		buf += m;
		c -= m;
		++i;
	}

	return n - c;
}

#ifdef __BIGGEST_ALIGNMENT__
#define T_BIGGEST_ALIGNMENT __BIGGEST_ALIGNMENT__
#else
#define T_BIGGEST_ALIGNMENT sizeof(long long)
#endif

typedef struct __attribute__((__aligned__(T_BIGGEST_ALIGNMENT))) {
	T_destructor base;
	void (*destroy)(void *);
} T_malloc_destructor;

static void
T_malloc_destroy(T_destructor *base)
{
	T_malloc_destructor *dtor;

	dtor = (T_malloc_destructor *)(uintptr_t)base;

	if (dtor->destroy != NULL) {
		(*dtor->destroy)(dtor + 1);
	}

	(*T_instance.config->deallocate)(dtor);
}

static void *
T_do_malloc(size_t size, void (*destroy)(void *))
{
	T_malloc_destructor *dtor;
	size_t new_size;

	new_size = sizeof(*dtor) + size;
	if (new_size <= size) {
		return NULL;
	}

	if (T_instance.config->allocate == NULL) {
		return NULL;
	}

	dtor = (*T_instance.config->allocate)(new_size);
	if (dtor != NULL) {
		dtor->destroy = destroy;
		T_add_destructor(&dtor->base, T_malloc_destroy);
		++dtor;
	}

	return dtor;
}

void *
T_malloc(size_t size)
{
	return T_do_malloc(size, NULL);
}

void *
T_calloc(size_t nelem, size_t elsize)
{
	return T_zalloc(nelem * elsize, NULL);
}

void *
T_zalloc(size_t size, void (*destroy)(void *))
{
	void *p;

	p = T_do_malloc(size, destroy);
	if (p != NULL) {
		p = memset(p, 0, size);
	}

	return p;
}

void
T_free(void *ptr)
{
	T_malloc_destructor *dtor;

	dtor = ptr;
	--dtor;
	T_remove_destructor(&dtor->base);
	(*T_instance.config->deallocate)(dtor);
}

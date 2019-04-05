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

#include <t.h>

#include <alloca.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include <rtems.h>

#define WAKEUP_EVENT RTEMS_EVENT_0

typedef struct {
	struct T_measure_runtime_context *master;
	rtems_id id;
	volatile unsigned int *chunk;
} load_context;

struct T_measure_runtime_context {
	T_destructor destructor;
	size_t sample_count;
	T_ticks *samples;
	size_t cache_line_size;
	size_t chunk_size;
	volatile unsigned int *chunk;
	rtems_id runner;
	uint32_t load_count;
	load_context *load_contexts;
};

static unsigned int
dirty_data_cache(volatile unsigned int *chunk, size_t chunk_size,
    size_t cache_line_size, unsigned int token)
{
	size_t m;
	size_t k;
	size_t i;

	m = chunk_size / sizeof(chunk[0]);
	k = cache_line_size / sizeof(chunk[0]);

	for (i = 0; i < m; i += k) {
		chunk[i] = i + token;
	}

	return i + token;
}

static void
wait_for_worker(void)
{
	rtems_event_set events;

	(void)rtems_event_receive(WAKEUP_EVENT, RTEMS_EVENT_ALL | RTEMS_WAIT,
	    RTEMS_NO_TIMEOUT, &events);
}

static void
wakeup_master(const T_measure_runtime_context *ctx)
{
	(void)rtems_event_send(ctx->runner, WAKEUP_EVENT);
}

static void
suspend_worker(const load_context *lctx)
{
	(void)rtems_task_suspend(lctx->id);
}

static void
restart_worker(const load_context *lctx)
{
	(void)rtems_task_restart(lctx->id, (rtems_task_argument)lctx);
	wait_for_worker();
}

static void
load_worker(rtems_task_argument arg)
{
	const load_context *lctx;
	T_measure_runtime_context *ctx;
	unsigned int token;
	volatile unsigned int *chunk;
	size_t chunk_size;
	size_t cache_line_size;

	lctx = (const load_context *)arg;
	ctx = lctx->master;
	chunk = lctx->chunk;
	chunk_size = ctx->chunk_size;
	cache_line_size = ctx->cache_line_size;
	token = (unsigned int)rtems_scheduler_get_processor();

	token = dirty_data_cache(chunk, chunk_size, cache_line_size, token);
	wakeup_master(ctx);

	while (true) {
		token = dirty_data_cache(chunk, chunk_size, cache_line_size,
		    token);
	}
}

static void
destroy_worker(const T_measure_runtime_context *ctx)
{
	uint32_t load;

	for (load = 0; load < ctx->load_count; ++load) {
		const load_context *lctx;

		lctx = &ctx->load_contexts[load];

		if (lctx->chunk != ctx->chunk) {
			free(RTEMS_DEVOLATILE(unsigned int *, lctx->chunk));
		}


		if (lctx->id != 0) {
			rtems_task_delete(lctx->id);
		}
	}
}

static void
destroy(T_destructor *dtor)
{
	T_measure_runtime_context *ctx;

	ctx = (T_measure_runtime_context *)dtor;
	destroy_worker(ctx);
	free(ctx);
}

static void *
add_offset(const volatile void *p, uintptr_t o)
{
	return (void *)((uintptr_t)p + o);
}

static void *
align_up(const volatile void *p, uintptr_t a)
{
	return (void *)(((uintptr_t)p + a - 1) & ~(a - 1));
}

T_measure_runtime_context *
T_measure_runtime_create(const T_measure_runtime_config *config)
{
	T_measure_runtime_context *ctx;
	size_t sample_size;
	size_t cache_line_size;
	size_t chunk_size;
	size_t load_size;
	uint32_t load_count;
	bool success;
	uint32_t i;
#ifdef RTEMS_SMP
	cpu_set_t cpu;
#endif

	sample_size = config->sample_count * sizeof(ctx->samples[0]);

	cache_line_size = rtems_cache_get_data_line_size();

	if (cache_line_size == 0) {
		cache_line_size = 8;
	}

	chunk_size = rtems_cache_get_data_cache_size(0);

	if (chunk_size == 0) {
		chunk_size = cache_line_size;
	}

	chunk_size *= 2;

	load_count = rtems_scheduler_get_processor_maximum();
	load_size = load_count * sizeof(ctx->load_contexts[0]);

	ctx = malloc(sizeof(*ctx) + sample_size + load_size + chunk_size +
	    2 * cache_line_size);

	if (ctx == NULL) {
		return NULL;
	}

	ctx->sample_count = config->sample_count;
	ctx->samples = add_offset(ctx, sizeof(*ctx));
	ctx->cache_line_size = cache_line_size;
	ctx->chunk_size = chunk_size;
	ctx->chunk = add_offset(ctx->samples, sample_size);
	ctx->runner = rtems_task_self();
	ctx->load_count = load_count;
	ctx->load_contexts = add_offset(ctx->chunk, chunk_size);
	ctx->samples = align_up(ctx->samples, cache_line_size);
	ctx->chunk = align_up(ctx->chunk, cache_line_size);

	memset(ctx->load_contexts, 0, load_size);
	success = true;

	for (i = 0; i < load_count; ++i) {
		rtems_status_code sc;
		rtems_id id;
		load_context *lctx;
#ifdef RTEMS_SMP
		rtems_task_priority priority;
		rtems_id scheduler;

		sc = rtems_scheduler_ident_by_processor(i, &scheduler);
		if (sc != RTEMS_SUCCESSFUL) {
			continue;
		}
#endif

		sc = rtems_task_create(rtems_build_name('L', 'O', 'A', 'D'),
		    RTEMS_MAXIMUM_PRIORITY - 1, RTEMS_MINIMUM_STACK_SIZE,
		    RTEMS_DEFAULT_MODES, RTEMS_DEFAULT_ATTRIBUTES, &id);
		if (sc != RTEMS_SUCCESSFUL) {
			success = false;
			break;
		}

		lctx = &ctx->load_contexts[i];

		lctx->master = ctx;
		lctx->id = id;

		lctx->chunk = malloc(chunk_size);
		if (lctx->chunk == NULL) {
			lctx->chunk = ctx->chunk;
		}

#ifdef RTEMS_SMP
		(void)rtems_scheduler_get_maximum_priority(scheduler, &priority);
		(void)rtems_task_set_scheduler(id, scheduler, priority - 1);

		CPU_ZERO(&cpu);
		CPU_SET((int)i, &cpu);
		(void)rtems_task_set_affinity(id, sizeof(cpu), &cpu);
#endif

		(void)rtems_task_start(id, load_worker,
		    (rtems_task_argument)lctx);

		wait_for_worker();
		suspend_worker(lctx);
	}

	if (success) {
#ifdef RTEMS_SMP
		CPU_ZERO(&cpu);
		CPU_SET(0, &cpu);
		(void)rtems_task_set_affinity(RTEMS_SELF, sizeof(cpu), &cpu);
#endif
	} else {
		destroy(&ctx->destructor);
		return NULL;
	}

	T_add_destructor(&ctx->destructor, destroy);
	return ctx;
}

static int
cmp(const void *ap, const void *bp)
{
	T_ticks a;
	T_ticks b;

	a = *(const T_ticks *)ap;
	b = *(const T_ticks *)bp;

	if (a < b) {
		return -1;
	} else if (a > b) {
		return 1;
	} else {
		return 0;
	}
}

static void
measure_variant_begin(const char *name, const char *variant)
{
	T_printf("M:B:%s\n", name);
	T_printf("M:V:%s\n", variant);
}

static T_time
accumulate(const T_ticks *samples, size_t sample_count)
{
	T_time a;
	size_t i;

	a = 0;

	for (i = 0; i < sample_count; ++i) {
		a += T_ticks_to_time(samples[i]);
	}

	return a;
}

static T_ticks
median_absolute_deviation(T_ticks *samples, size_t sample_count)
{
	T_ticks median;
	size_t i;

	median = samples[sample_count / 2];

	for (i = 0; i < sample_count / 2; ++i) {
		samples[i] = median - samples[i];
	}

	for (; i < sample_count; ++i) {
		samples[i] = samples[i] - median;
	}

	qsort(samples, sample_count, sizeof(samples[0]), cmp);
	return samples[sample_count / 2];
}

static void
report_sorted_samples(const T_measure_runtime_context *ctx)
{
	size_t sample_count;
	const T_ticks *samples;
	T_time_string ts;
	T_ticks last;
	T_ticks v;
	size_t count;
	size_t i;

	sample_count = ctx->sample_count;
	samples = ctx->samples;
	last = 0;
	--last;
	count = 0;

	for (i = 0; i < sample_count; ++i) {
		v = samples[i];
		++count;

		if (v != last) {
			uint32_t sa;
			uint32_t sb;
			uint32_t nsa;
			uint32_t nsb;
			T_time t;

			T_time_to_seconds_and_nanoseconds(T_ticks_to_time(last),
			    &sa, &nsa);
			t = T_ticks_to_time(v);
			T_time_to_seconds_and_nanoseconds(t, &sb, &nsb);

			if (sa != sb || nsa != nsb) {
				T_printf("M:S:%zu:%s\n", count,
				    T_time_to_string_ns(t, ts));
				count = 0;
			}

			last = v;
		}
	}

	if (count > 0) {
		T_printf("M:S:%zu:%s\n", count,
		    T_ticks_to_string_ns(last, ts));
	}
}

static void
measure_variant_end(const T_measure_runtime_context *ctx,
    const T_measure_runtime_request *req, T_time begin)
{
	size_t sample_count;
	T_ticks *samples;
	T_time_string ts;
	T_time d;
	T_ticks v;
	T_time a;

	sample_count = ctx->sample_count;
	samples = ctx->samples;
	d = T_now() - begin;
	a = accumulate(samples, sample_count);
	qsort(samples, sample_count, sizeof(samples[0]), cmp);
	T_printf("M:N:%zu\n", sample_count);

	if ((req->flags & T_MEASURE_RUNTIME_REPORT_SAMPLES) != 0) {
		report_sorted_samples(ctx);
	}

	v = samples[0];
	T_printf("M:MI:%s\n", T_ticks_to_string_ns(v, ts));
	v = samples[(1 * sample_count) / 100];
	T_printf("M:P1:%s\n", T_ticks_to_string_ns(v, ts));
	v = samples[(1 * sample_count) / 4];
	T_printf("M:Q1:%s\n", T_ticks_to_string_ns(v, ts));
	v = samples[sample_count / 2];
	T_printf("M:Q2:%s\n", T_ticks_to_string_ns(v, ts));
	v = samples[(3 * sample_count) / 4];
	T_printf("M:Q3:%s\n", T_ticks_to_string_ns(v, ts));
	v = samples[(99 * sample_count) / 100];
	T_printf("M:P99:%s\n", T_ticks_to_string_ns(v, ts));
	v = samples[sample_count - 1];
	T_printf("M:MX:%s\n", T_ticks_to_string_ns(v, ts));
	v = median_absolute_deviation(samples, sample_count);
	T_printf("M:MAD:%s\n", T_ticks_to_string_ns(v, ts));
	T_printf("M:D:%s\n", T_time_to_string_ns(a, ts));
	T_printf("M:E:%s:D:%s\n", req->name, T_time_to_string_ns(d, ts));
}

static void
fill_data_cache(volatile unsigned int *chunk, size_t chunk_size,
    size_t cache_line_size)
{
	size_t m;
	size_t k;
	size_t i;

	m = chunk_size / sizeof(chunk[0]);
	k = cache_line_size / sizeof(chunk[0]);

	for (i = 0; i < m; i += k) {
		chunk[i];
	}
}

static void
dirty_call(void (*body)(void *), void *arg)
{
	void *space;

	/* Ensure that we use an untouched stack area */
	space = alloca(1024);
	RTEMS_OBFUSCATE_VARIABLE(space);

	(*body)(arg);
}

static void
setup(const T_measure_runtime_request *req, void *arg)
{
	if (req->setup != NULL) {
		(*req->setup)(arg);
	}
}

static bool
teardown(const T_measure_runtime_request *req, void *arg, T_ticks *delta,
    uint32_t tic, uint32_t toc, unsigned int retry,
    unsigned int maximum_retries)
{
	if (req->teardown == NULL) {
		return tic == toc || retry >= maximum_retries;
	}

	return (*req->teardown)(arg, delta, tic, toc, retry);
}

static unsigned int
get_maximum_retries(const T_measure_runtime_request *req)
{
	return (req->flags & T_MEASURE_RUNTIME_ALLOW_CLOCK_ISR) != 0 ? 1 : 0;
}

static void
measure_valid_cache(T_measure_runtime_context *ctx,
    const T_measure_runtime_request *req)
{
	size_t sample_count;
	T_ticks *samples;
	void (*body)(void *);
	void *arg;
	size_t i;
	T_time begin;

	measure_variant_begin(req->name, "ValidCache");
	begin = T_now();
	sample_count = ctx->sample_count;
	samples = ctx->samples;
	body = req->body;
	arg = req->arg;

	for (i = 0; i < sample_count; ++i) {
		unsigned int maximum_retries;
		unsigned int retry;

		maximum_retries = get_maximum_retries(req);
		retry = 0;

		while (true) {
			rtems_interval tic;
			rtems_interval toc;
			T_ticks t0;
			T_ticks t1;

			setup(req, arg);
			fill_data_cache(ctx->chunk, ctx->chunk_size,
			    ctx->cache_line_size);

			tic = rtems_clock_get_ticks_since_boot();
			t0 = T_tick();
			(*body)(arg);
			t1 = T_tick();
			toc = rtems_clock_get_ticks_since_boot();
			samples[i] = t1 - t0;

			if (teardown(req, arg, &samples[i], tic, toc, retry,
			    maximum_retries)) {
				break;
			}

			++retry;
		}
	}

	measure_variant_end(ctx, req, begin);
}

static void
measure_hot_cache(T_measure_runtime_context *ctx,
    const T_measure_runtime_request *req)
{
	size_t sample_count;
	T_ticks *samples;
	void (*body)(void *);
	void *arg;
	size_t i;
	T_time begin;

	measure_variant_begin(req->name, "HotCache");
	begin = T_now();
	sample_count = ctx->sample_count;
	samples = ctx->samples;
	body = req->body;
	arg = req->arg;

	for (i = 0; i < sample_count; ++i) {
		unsigned int maximum_retries;
		unsigned int retry;

		maximum_retries = get_maximum_retries(req);
		retry = 0;

		while (true) {
			rtems_interval tic;
			rtems_interval toc;
			T_ticks t0;
			T_ticks t1;

			setup(req, arg);

			tic = rtems_clock_get_ticks_since_boot();
			t0 = T_tick();
			(*body)(arg);
			t1 = T_tick();
			toc = rtems_clock_get_ticks_since_boot();
			samples[i] = t1 - t0;

			(void)teardown(req, arg, &samples[i], tic, toc, retry,
			    0);
			setup(req, arg);

			tic = rtems_clock_get_ticks_since_boot();
			t0 = T_tick();
			(*body)(arg);
			t1 = T_tick();
			toc = rtems_clock_get_ticks_since_boot();
			samples[i] = t1 - t0;

			if (teardown(req, arg, &samples[i], tic, toc, retry,
			    maximum_retries)) {
				break;
			}

			++retry;
		}
	}

	measure_variant_end(ctx, req, begin);
}

static void
measure_dirty_cache(T_measure_runtime_context *ctx,
    const T_measure_runtime_request *req)
{
	size_t sample_count;
	T_ticks *samples;
	void (*body)(void *);
	void *arg;
	size_t i;
	T_time begin;
	size_t token;

	measure_variant_begin(req->name, "DirtyCache");
	begin = T_now();
	sample_count = ctx->sample_count;
	samples = ctx->samples;
	body = req->body;
	arg = req->arg;
	token = 0;

	for (i = 0; i < sample_count; ++i) {
		unsigned int maximum_retries;
		unsigned int retry;

		maximum_retries = get_maximum_retries(req);
		retry = 0;

		while (true) {
			rtems_interval tic;
			rtems_interval toc;
			T_ticks t0;
			T_ticks t1;

			setup(req, arg);
			token = dirty_data_cache(ctx->chunk, ctx->chunk_size,
			    ctx->cache_line_size, token);
			rtems_cache_invalidate_entire_instruction();

			tic = rtems_clock_get_ticks_since_boot();
			t0 = T_tick();
			dirty_call(body, arg);
			t1 = T_tick();
			toc = rtems_clock_get_ticks_since_boot();
			samples[i] = t1 - t0;

			if (teardown(req, arg, &samples[i], tic, toc, retry,
			    maximum_retries)) {
				break;
			}

			++retry;
		}
	}

	measure_variant_end(ctx, req, begin);
}

#ifdef __sparc__
/*
 * Use recursive function calls to wake sure that we cause window overflow
 * traps in the body.  Try to make it hard for the compiler to optimize the
 * recursive function away.
 */
static T_ticks
recursive_load_call(void (*body)(void *), void *arg, int n)
{
	T_ticks delta;

	RTEMS_OBFUSCATE_VARIABLE(n);

	if (n > 0) {
		delta = recursive_load_call(body, arg, n - 1);
	} else {
		T_ticks t0;
		T_ticks t1;

		t0 = T_tick();
		dirty_call(body, arg);
		t1 = T_tick();

		delta = t1 - t0;
	}

	RTEMS_OBFUSCATE_VARIABLE(delta);
	return delta;
}
#else
static T_ticks
load_call(void (*body)(void *), void *arg)
{
	T_ticks t0;
	T_ticks t1;

	t0 = T_tick();
	dirty_call(body, arg);
	t1 = T_tick();

	return t1 - t0;
}
#endif

static void
measure_load_variant(T_measure_runtime_context *ctx,
    const T_measure_runtime_request *req,
    const load_context *lctx, uint32_t load)
{
	size_t sample_count;
	T_ticks *samples;
	void (*body)(void *);
	void *arg;
	size_t i;
	T_time begin;
	size_t token;

	measure_variant_begin(req->name, "Load");
	T_printf("M:L:%" PRIu32 "\n", load + 1);
	begin = T_now();
	sample_count = ctx->sample_count;
	samples = ctx->samples;
	body = req->body;
	arg = req->arg;
	token = 0;

	restart_worker(lctx);

	for (i = 0; i < sample_count; ++i) {
		unsigned int maximum_retries;
		unsigned int retry;

		maximum_retries = get_maximum_retries(req);
		retry = 0;

		while (true) {
			rtems_interval tic;
			rtems_interval toc;
			T_ticks delta;

			setup(req, arg);
			token = dirty_data_cache(ctx->chunk, ctx->chunk_size,
			    ctx->cache_line_size, token);
			rtems_cache_invalidate_entire_instruction();

			tic = rtems_clock_get_ticks_since_boot();
#ifdef __sparc__
			delta = recursive_load_call(body, arg,
			    SPARC_NUMBER_OF_REGISTER_WINDOWS - 3);
#else
			delta = load_call(body, arg);
#endif
			toc = rtems_clock_get_ticks_since_boot();
			samples[i] = delta;

			if (teardown(req, arg, &samples[i], tic, toc, retry,
			    maximum_retries)) {
				break;
			}

			++retry;
		}
	}

	measure_variant_end(ctx, req, begin);
}

static void
measure_load(T_measure_runtime_context *ctx,
    const T_measure_runtime_request *req)
{
	const load_context *lctx;
	uint32_t load;

#ifdef RTEMS_SMP
	for (load = 0; load < ctx->load_count - 1; ++load) {
		lctx = &ctx->load_contexts[load];

		if (lctx->id != 0) {
			if ((req->flags &
			    T_MEASURE_RUNTIME_DISABLE_MINOR_LOAD) == 0) {
				measure_load_variant(ctx, req, lctx, load);
			} else {
				restart_worker(lctx);
			}
		}
	}
#endif

	if ((req->flags & T_MEASURE_RUNTIME_DISABLE_MAX_LOAD) == 0) {
		load = ctx->load_count - 1;
		lctx = &ctx->load_contexts[load];

		if (lctx->id != 0) {
			measure_load_variant(ctx, req, lctx, load);
		}
	}

	for (load = 0; load < ctx->load_count; ++load) {
		lctx = &ctx->load_contexts[load];

		if (lctx->id != 0) {
			suspend_worker(lctx);
		}
	}
}

void
T_measure_runtime(T_measure_runtime_context *ctx,
    const T_measure_runtime_request *req)
{
	/*
	 * Do ValidCache variant before HotCache to get a good overall cache
	 * state for the HotCache variant.
	 */
	if ((req->flags & T_MEASURE_RUNTIME_DISABLE_VALID_CACHE) == 0) {
		measure_valid_cache(ctx, req);
	}

	if ((req->flags & T_MEASURE_RUNTIME_DISABLE_HOT_CACHE) == 0) {
		measure_hot_cache(ctx, req);
	}

	if ((req->flags & T_MEASURE_RUNTIME_DISABLE_DIRTY_CACHE) == 0) {
		measure_dirty_cache(ctx, req);
	}

	measure_load(ctx, req);
}

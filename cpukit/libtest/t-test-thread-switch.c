/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestFrameworkImpl
 *
 * @brief This source file contains the implementation of the thread switch
 *   recorder.
 */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
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

#include <rtems.h>
#include <rtems/score/percpu.h>
#include <rtems/score/smp.h>
#include <rtems/score/thread.h>
#include <rtems/score/userextimpl.h>

typedef struct {
	RTEMS_INTERRUPT_LOCK_MEMBER(lock)
	T_thread_switch_log *active;
	User_extensions_Control ext;
	T_destructor dtor;
} T_thread_switch_context;

static void T_thread_switch_recorder(Thread_Control *, Thread_Control *);

static T_thread_switch_context T_thread_switch_instance = {
#ifdef RTEMS_SMP
	.lock = RTEMS_INTERRUPT_LOCK_INITIALIZER("Test Thread Switches"),
#endif
	.ext = {
		.Callouts = {
			.thread_switch = T_thread_switch_recorder
		}
	}
};

static void
T_thread_switch_destroy(T_destructor *dtor)
{
	T_thread_switch_context *ctx;

	ctx = RTEMS_CONTAINER_OF(dtor, T_thread_switch_context, dtor);
	_User_extensions_Remove_set(&ctx->ext);
	_Chain_Set_off_chain(&ctx->ext.Node);
}

static void
T_thread_switch_recorder(Thread_Control *executing, Thread_Control *heir)
{
	rtems_interrupt_lock_context lock_context;
	T_thread_switch_context *ctx;
	T_thread_switch_log *log;

	ctx = &T_thread_switch_instance;

#ifdef RTEMS_SMP
	if (ctx->active == NULL) {
		return;
	}
#endif

	rtems_interrupt_lock_acquire_isr(&ctx->lock, &lock_context);
	log = ctx->active;

	if (log != NULL) {
		size_t recorded;

		++log->header.switches;
		recorded = log->header.recorded;

		if (recorded < log->header.capacity) {
			log->header.recorded = recorded + 1;
			log->events[recorded].executing = executing->Object.id;
			log->events[recorded].heir = heir->Object.id;
			log->events[recorded].cpu =
				_SMP_Get_current_processor();
			log->events[recorded].instant = T_now();
		}
	}

	rtems_interrupt_lock_release_isr(&ctx->lock, &lock_context);
}

T_thread_switch_log *
T_thread_switch_record(T_thread_switch_log *log)
{
	rtems_interrupt_lock_context lock_context;
	T_thread_switch_log *previous;
	T_thread_switch_context *ctx;

	ctx = &T_thread_switch_instance;

	if (_Chain_Is_node_off_chain(&ctx->ext.Node)) {
		_User_extensions_Add_set(&ctx->ext);
		T_add_destructor(&ctx->dtor, T_thread_switch_destroy);
	}

	if (log != NULL) {
		log->header.recorded = 0;
		log->header.switches = 0;
	}

	rtems_interrupt_lock_acquire(&ctx->lock, &lock_context);
	previous = ctx->active;
	ctx->active = log;
	rtems_interrupt_lock_release(&ctx->lock, &lock_context);

	return previous;
}

T_thread_switch_log *
T_thread_switch_record_2(T_thread_switch_log_2 *log)
{
	log->header.capacity = T_ARRAY_SIZE(log->events);
	return T_thread_switch_record((T_thread_switch_log *)log);
}

T_thread_switch_log *
T_thread_switch_record_4(T_thread_switch_log_4 *log)
{
	log->header.capacity = T_ARRAY_SIZE(log->events);
	return T_thread_switch_record((T_thread_switch_log *)log);
}

T_thread_switch_log *
T_thread_switch_record_10(T_thread_switch_log_10 *log)
{
	log->header.capacity = T_ARRAY_SIZE(log->events);
	return T_thread_switch_record((T_thread_switch_log *)log);
}

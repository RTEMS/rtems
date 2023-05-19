/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestFrameworkImpl
 *
 * @brief This source file contains the implementation of the scheduler test
 *   support API.
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

#include <rtems/test-scheduler.h>

#include <rtems.h>
#include <rtems/score/percpu.h>
#include <rtems/score/schedulerimpl.h>

typedef struct {
	RTEMS_INTERRUPT_LOCK_MEMBER(lock)
	T_scheduler_log *active;
	T_scheduler_event_handler handler;
	void *arg;
} T_scheduler_context;

static T_scheduler_context T_scheduler_instance = {
#ifdef RTEMS_SMP
	.lock = RTEMS_INTERRUPT_LOCK_INITIALIZER("Test Scheduler"),
#endif
	.active = NULL
};

void
T_scheduler_set_event_handler(T_scheduler_event_handler handler, void *arg)
{
	T_scheduler_context *ctx;
	rtems_interrupt_lock_context lock_context;

	ctx = &T_scheduler_instance;

	rtems_interrupt_lock_acquire(&ctx->lock, &lock_context);
	ctx->handler = handler;
	ctx->arg = arg;
	rtems_interrupt_lock_release(&ctx->lock, &lock_context);
}

static void
T_scheduler_before_operation(T_scheduler_event *event)
{
	T_scheduler_context *ctx;
	rtems_interrupt_lock_context lock_context;
	T_scheduler_log *log;
	T_scheduler_event_handler handler;
	void *arg;
	Per_CPU_Control *cpu_self;

	ctx = &T_scheduler_instance;
	log = ctx->active;
	handler = ctx->handler;

	if (log == NULL && handler == NULL) {
		return;
	}

	rtems_interrupt_lock_interrupt_disable(&lock_context);
	cpu_self = _Per_CPU_Get();
	event->cpu = _Per_CPU_Get_index( cpu_self );

	if (_Per_CPU_Is_ISR_in_progress(cpu_self)) {
		event->executing = NULL;
	} else {
		event->executing = _Per_CPU_Get_executing(cpu_self);
	}

	event->instant = T_now();

	rtems_interrupt_lock_acquire_isr(&ctx->lock, &lock_context);
	handler = ctx->handler;
	arg = ctx->arg;
	rtems_interrupt_lock_release(&ctx->lock, &lock_context);

	if (handler != NULL) {
		(*handler)(arg, event, T_SCHEDULER_BEFORE);
	}
}

static void
T_scheduler_record_event(T_scheduler_event *event)
{
	T_scheduler_context *ctx;
	rtems_interrupt_lock_context lock_context;
	T_scheduler_log *log;
	T_scheduler_event_handler handler;

	ctx = &T_scheduler_instance;
	log = ctx->active;
	handler = ctx->handler;

	if (log == NULL && handler == NULL) {
		return;
	}

	rtems_interrupt_lock_acquire(&ctx->lock, &lock_context);

#ifdef RTEMS_SMP
	handler = ctx->handler;
#endif

	if (handler != NULL) {
		void *arg;

		arg = ctx->arg;

		rtems_interrupt_lock_release(&ctx->lock, &lock_context);
		(*handler)(arg, event, T_SCHEDULER_AFTER);
		rtems_interrupt_lock_acquire(&ctx->lock, &lock_context);
	}

#ifdef RTEMS_SMP
	log = ctx->active;
#endif

	if (log != NULL) {
		size_t recorded;

		++log->header.operations;
		recorded = log->header.recorded;

		if (recorded < log->header.capacity) {
			log->header.recorded = recorded + 1;
			log->events[recorded] = *event;
		}
	}

	rtems_interrupt_lock_release(&ctx->lock, &lock_context);
}

T_scheduler_log *
T_scheduler_record(T_scheduler_log *log)
{
	rtems_interrupt_lock_context lock_context;
	T_scheduler_log *previous;
	T_scheduler_context *ctx;

	if (log != NULL) {
		log->header.recorded = 0;
		log->header.operations = 0;
	}

	ctx = &T_scheduler_instance;

	rtems_interrupt_lock_acquire(&ctx->lock, &lock_context);
	previous = ctx->active;
	ctx->active = log;
	rtems_interrupt_lock_release(&ctx->lock, &lock_context);

	return previous;
}

T_scheduler_log *
T_scheduler_record_2(T_scheduler_log_2 *log)
{
	log->header.capacity = T_ARRAY_SIZE(log->events);
	return T_scheduler_record((T_scheduler_log *)log);
}

T_scheduler_log *
T_scheduler_record_4(T_scheduler_log_4 *log)
{
	log->header.capacity = T_ARRAY_SIZE(log->events);
	return T_scheduler_record((T_scheduler_log *)log);
}

T_scheduler_log *
T_scheduler_record_10(T_scheduler_log_10 *log)
{
	log->header.capacity = T_ARRAY_SIZE(log->events);
	return T_scheduler_record((T_scheduler_log *)log);
}

T_scheduler_log *
T_scheduler_record_20(T_scheduler_log_20 *log)
{
	log->header.capacity = T_ARRAY_SIZE(log->events);
	return T_scheduler_record((T_scheduler_log *)log);
}

T_scheduler_log *
T_scheduler_record_40(T_scheduler_log_40 *log)
{
	log->header.capacity = T_ARRAY_SIZE(log->events);
	return T_scheduler_record((T_scheduler_log *)log);
}

const T_scheduler_event T_scheduler_event_null;

const T_scheduler_event *
T_scheduler_next(T_scheduler_header *header, T_scheduler_operation operation,
    size_t *index)
{
	T_scheduler_log *log;
	size_t i;

	log = (T_scheduler_log *)header;

	if (log == NULL) {
		return &T_scheduler_event_null;
	}

	i = *index;

	while (i < log->header.recorded) {
		if (operation == T_SCHEDULER_ANY ||
		    operation == log->events[i].operation) {
			*index = i + 1;
			return &log->events[i];
		}

		++i;
	}

	return &T_scheduler_event_null;
}

const T_scheduler_event *
T_scheduler_next_any(T_scheduler_header *header, size_t *index)
{
	return T_scheduler_next(header, T_SCHEDULER_ANY, index);
}

void
T_scheduler_initialize(const Scheduler_Control *scheduler)
{
	T_scheduler_event event = {
		.operation = T_SCHEDULER_INITIALIZE
	};
	const Scheduler_Operations *operations;

	operations = &T_scheduler_operations[_Scheduler_Get_index(scheduler)];
	T_scheduler_before_operation(&event);
	(*operations->initialize)(scheduler);
	T_scheduler_record_event(&event);
}

void
T_scheduler_schedule(const Scheduler_Control *scheduler,
   Thread_Control *thread)
{
	T_scheduler_event event = {
		.operation = T_SCHEDULER_SCHEDULE,
		.thread = thread
	};
	const Scheduler_Operations *operations;

	operations = &T_scheduler_operations[_Scheduler_Get_index(scheduler)];
	T_scheduler_before_operation(&event);
	(*operations->schedule)(scheduler, thread);
	T_scheduler_record_event(&event);
}

void
T_scheduler_yield(const Scheduler_Control *scheduler, Thread_Control *thread,
    Scheduler_Node *node)
{
	T_scheduler_event event = {
		.operation = T_SCHEDULER_YIELD,
		.thread = thread,
		.node = node
	};
	const Scheduler_Operations *operations;

	operations = &T_scheduler_operations[_Scheduler_Get_index(scheduler)];
	T_scheduler_before_operation(&event);
	(*operations->yield)(scheduler, thread, node);
	T_scheduler_record_event(&event);
}

void
T_scheduler_block(const Scheduler_Control *scheduler, Thread_Control *thread,
    Scheduler_Node *node)
{
	T_scheduler_event event = {
		.operation = T_SCHEDULER_BLOCK,
		.thread = thread,
		.node = node
	};
	const Scheduler_Operations *operations;

	operations = &T_scheduler_operations[_Scheduler_Get_index(scheduler)];
	T_scheduler_before_operation(&event);
	(*operations->block)(scheduler, thread, node);
	T_scheduler_record_event(&event);
}

void
T_scheduler_unblock(const Scheduler_Control *scheduler, Thread_Control *thread,
    Scheduler_Node *node)
{
	T_scheduler_event event = {
		.operation = T_SCHEDULER_UNBLOCK,
		.thread = thread,
		.node = node
	};
	const Scheduler_Operations *operations;

	operations = &T_scheduler_operations[_Scheduler_Get_index(scheduler)];
	T_scheduler_before_operation(&event);
	(*operations->unblock)(scheduler, thread, node);
	T_scheduler_record_event(&event);
}

void
T_scheduler_update_priority(const Scheduler_Control *scheduler,
    Thread_Control *thread, Scheduler_Node *node)
{
	T_scheduler_event event = {
		.operation = T_SCHEDULER_UPDATE_PRIORITY,
		.thread = thread,
		.node = node
	};
	const Scheduler_Operations *operations;

	operations = &T_scheduler_operations[_Scheduler_Get_index(scheduler)];
	T_scheduler_before_operation(&event);
	(*operations->update_priority)(scheduler, thread, node);
	T_scheduler_record_event(&event);
}

Priority_Control T_scheduler_map_priority(const Scheduler_Control *scheduler,
    Priority_Control priority)
{
	T_scheduler_event event = {
		.operation = T_SCHEDULER_MAP_PRIORITY
	};
	const Scheduler_Operations *operations;

	operations = &T_scheduler_operations[_Scheduler_Get_index(scheduler)];
	event.map_unmap_priority.in = priority;
	T_scheduler_before_operation(&event);
	event.map_unmap_priority.out = (*operations->map_priority)(scheduler,
	    priority);
	T_scheduler_record_event(&event);
	return event.map_unmap_priority.out;
}

Priority_Control T_scheduler_unmap_priority(const Scheduler_Control *scheduler,
    Priority_Control priority)
{
	T_scheduler_event event = {
		.operation = T_SCHEDULER_UNMAP_PRIORITY
	};
	const Scheduler_Operations *operations;

	operations = &T_scheduler_operations[_Scheduler_Get_index(scheduler)];
	event.map_unmap_priority.in = priority;
	T_scheduler_before_operation(&event);
	event.map_unmap_priority.out = (*operations->unmap_priority)(scheduler,
	    priority);
	T_scheduler_record_event(&event);
	return event.map_unmap_priority.out;
}

void
T_scheduler_node_initialize(const Scheduler_Control *scheduler,
    Scheduler_Node *node, Thread_Control *thread, Priority_Control priority)
{
	T_scheduler_event event = {
		.operation = T_SCHEDULER_NODE_INITIALIZE,
		.thread = thread,
		.node = node
	};
	const Scheduler_Operations *operations;

	operations = &T_scheduler_operations[_Scheduler_Get_index(scheduler)];
	T_scheduler_before_operation(&event);
	(*operations->node_initialize)(scheduler, node, thread, priority);
	T_scheduler_record_event(&event);
}

void
T_scheduler_node_destroy(const Scheduler_Control *scheduler,
    Scheduler_Node *node)
{
	T_scheduler_event event = {
		.operation = T_SCHEDULER_NODE_DESTROY,
		.node = node
	};
	const Scheduler_Operations *operations;

	operations = &T_scheduler_operations[_Scheduler_Get_index(scheduler)];
	T_scheduler_before_operation(&event);
	(*operations->node_destroy)(scheduler, node);
	T_scheduler_record_event(&event);
}

void
T_scheduler_release_job(const Scheduler_Control *scheduler,
    Thread_Control *thread, Priority_Node *priority, uint64_t deadline,
    Thread_queue_Context *queue_context)
{
	T_scheduler_event event = {
		.operation = T_SCHEDULER_RELEASE_JOB,
		.thread = thread
	};
	const Scheduler_Operations *operations;

	operations = &T_scheduler_operations[_Scheduler_Get_index(scheduler)];
	event.release_job.priority = priority;
	event.release_job.deadline = deadline;
	T_scheduler_before_operation(&event);
	(*operations->release_job)(scheduler, thread, priority, deadline, queue_context);
	T_scheduler_record_event(&event);
}

void
T_scheduler_cancel_job(const Scheduler_Control *scheduler,
    Thread_Control *thread, Priority_Node *priority,
    Thread_queue_Context *queue_context)
{
	T_scheduler_event event = {
		.operation = T_SCHEDULER_CANCEL_JOB,
		.thread = thread
	};
	const Scheduler_Operations *operations;

	operations = &T_scheduler_operations[_Scheduler_Get_index(scheduler)];
	event.cancel_job.priority = priority;
	T_scheduler_before_operation(&event);
	(*operations->cancel_job)(scheduler, thread, priority, queue_context);
	T_scheduler_record_event(&event);
}

void
T_scheduler_start_idle(const Scheduler_Control *scheduler,
    Thread_Control *thread, Per_CPU_Control *cpu)
{
	T_scheduler_event event = {
		.operation = T_SCHEDULER_START_IDLE,
		.thread = thread
	};
	const Scheduler_Operations *operations;

	operations = &T_scheduler_operations[_Scheduler_Get_index(scheduler)];
	T_scheduler_before_operation(&event);
	(*operations->start_idle)(scheduler, thread, cpu);
	T_scheduler_record_event(&event);
}

#ifdef RTEMS_SMP
bool
T_scheduler_ask_for_help(const Scheduler_Control *scheduler,
    Thread_Control *thread, Scheduler_Node *node)
{
	T_scheduler_event event = {
		.operation = T_SCHEDULER_ASK_FOR_HELP,
		.thread = thread,
		.node = node
	};
	const Scheduler_Operations *operations;

	operations = &T_scheduler_operations[_Scheduler_Get_index(scheduler)];
	T_scheduler_before_operation(&event);
	event.ask_for_help.success = (*operations->ask_for_help)(scheduler,
	    thread, node);
	T_scheduler_record_event(&event);
	return event.ask_for_help.success;
}

void
T_scheduler_reconsider_help_request(const Scheduler_Control *scheduler,
    Thread_Control *thread, Scheduler_Node *node)
{
	T_scheduler_event event = {
		.operation = T_SCHEDULER_RECONSIDER_HELP_REQUEST,
		.thread = thread,
		.node = node
	};
	const Scheduler_Operations *operations;

	operations = &T_scheduler_operations[_Scheduler_Get_index(scheduler)];
	T_scheduler_before_operation(&event);
	(*operations->reconsider_help_request)(scheduler, thread, node);
	T_scheduler_record_event(&event);
}

void
T_scheduler_withdraw_node(const Scheduler_Control *scheduler,
    Thread_Control *thread, Scheduler_Node *node,
    Thread_Scheduler_state next_state)
{
	T_scheduler_event event = {
		.operation = T_SCHEDULER_WITHDRAW_NODE,
		.thread = thread,
		.node = node
	};
	const Scheduler_Operations *operations;

	operations = &T_scheduler_operations[_Scheduler_Get_index(scheduler)];
	event.withdraw_node.next_state = next_state;
	T_scheduler_before_operation(&event);
	(*operations->withdraw_node)(scheduler, thread, node, next_state);
	T_scheduler_record_event(&event);
}

void
T_scheduler_make_sticky(const Scheduler_Control *scheduler, Thread_Control *thread,
    Scheduler_Node *node)
{
	T_scheduler_event event = {
		.operation = T_SCHEDULER_MAKE_STICKY,
		.thread = thread,
		.node = node
	};
	const Scheduler_Operations *operations;

	operations = &T_scheduler_operations[_Scheduler_Get_index(scheduler)];
	T_scheduler_before_operation(&event);
	(*operations->make_sticky)(scheduler, thread, node);
	T_scheduler_record_event(&event);
}

void
T_scheduler_clean_sticky(const Scheduler_Control *scheduler, Thread_Control *thread,
    Scheduler_Node *node)
{
	T_scheduler_event event = {
		.operation = T_SCHEDULER_CLEAN_STICKY,
		.thread = thread,
		.node = node
	};
	const Scheduler_Operations *operations;

	operations = &T_scheduler_operations[_Scheduler_Get_index(scheduler)];
	T_scheduler_before_operation(&event);
	(*operations->clean_sticky)(scheduler, thread, node);
	T_scheduler_record_event(&event);
}

void
T_scheduler_pin(const Scheduler_Control *scheduler, Thread_Control *thread,
    Scheduler_Node *node, Per_CPU_Control *cpu)
{
	T_scheduler_event event = {
		.operation = T_SCHEDULER_PIN,
		.thread = thread,
		.node = node
	};
	const Scheduler_Operations *operations;

	operations = &T_scheduler_operations[_Scheduler_Get_index(scheduler)];
	event.pin_unpin.cpu = cpu;
	T_scheduler_before_operation(&event);
	(*operations->pin)(scheduler, thread, node, cpu);
	T_scheduler_record_event(&event);
}

void
T_scheduler_unpin(const Scheduler_Control *scheduler, Thread_Control *thread,
    Scheduler_Node *node, Per_CPU_Control *cpu)
{
	T_scheduler_event event = {
		.operation = T_SCHEDULER_UNPIN,
		.thread = thread,
		.node = node
	};
	const Scheduler_Operations *operations;

	operations = &T_scheduler_operations[_Scheduler_Get_index(scheduler)];
	event.pin_unpin.cpu = cpu;
	T_scheduler_before_operation(&event);
	(*operations->unpin)(scheduler, thread, node, cpu);
	T_scheduler_record_event(&event);
}

void
T_scheduler_add_processor(const Scheduler_Control *scheduler,
    Thread_Control *idle)
{
	T_scheduler_event event = {
		.operation = T_SCHEDULER_ADD_PROCESSOR,
		.thread = idle
	};
	const Scheduler_Operations *operations;

	operations = &T_scheduler_operations[_Scheduler_Get_index(scheduler)];
	event.add_processor.idle = idle;
	T_scheduler_before_operation(&event);
	(*operations->add_processor)(scheduler, idle);
	T_scheduler_record_event(&event);
}

Thread_Control *
T_scheduler_remove_processor(const Scheduler_Control *scheduler,
    Per_CPU_Control *cpu)
{
	T_scheduler_event event = {
		.operation = T_SCHEDULER_REMOVE_PROCESSOR
	};
	const Scheduler_Operations *operations;

	operations = &T_scheduler_operations[_Scheduler_Get_index(scheduler)];
	event.remove_processor.cpu = cpu;
	T_scheduler_before_operation(&event);
	event.remove_processor.idle =
	    (*operations->remove_processor)(scheduler, cpu);
	T_scheduler_record_event(&event);
	return event.remove_processor.idle;
}

Status_Control
T_scheduler_set_affinity(const Scheduler_Control *scheduler,
    Thread_Control *thread, Scheduler_Node *node,
    const Processor_mask *affinity)
{
	T_scheduler_event event = {
		.operation = T_SCHEDULER_SET_AFFINITY,
		.thread = thread,
		.node = node
	};
	const Scheduler_Operations *operations;

	operations = &T_scheduler_operations[_Scheduler_Get_index(scheduler)];
	event.set_affinity.affinity = *affinity;
	T_scheduler_before_operation(&event);
	event.set_affinity.status = (*operations->set_affinity)(scheduler,
	    thread, node, affinity);
	T_scheduler_record_event(&event);
	return event.set_affinity.status;
}
#endif

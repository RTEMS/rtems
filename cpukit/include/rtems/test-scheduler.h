/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestFramework
 *
 * @brief This header file defines the scheduler test support API.
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

#ifndef _RTEMS_TEST_SCHEDULER_H
#define _RTEMS_TEST_SCHEDULER_H

#include <rtems/test.h>
#include <rtems/score/processormask.h>
#include <rtems/score/scheduler.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSTestFramework
 *
 * @{
 */

typedef enum {
	T_SCHEDULER_NOP,
	T_SCHEDULER_ADD_PROCESSOR,
	T_SCHEDULER_ANY,
	T_SCHEDULER_ASK_FOR_HELP,
	T_SCHEDULER_BLOCK,
	T_SCHEDULER_CANCEL_JOB,
	T_SCHEDULER_CLEAN_STICKY,
	T_SCHEDULER_INITIALIZE,
	T_SCHEDULER_MAKE_STICKY,
	T_SCHEDULER_MAP_PRIORITY,
	T_SCHEDULER_NODE_DESTROY,
	T_SCHEDULER_NODE_INITIALIZE,
	T_SCHEDULER_PIN,
	T_SCHEDULER_RECONSIDER_HELP_REQUEST,
	T_SCHEDULER_RELEASE_JOB,
	T_SCHEDULER_REMOVE_PROCESSOR,
	T_SCHEDULER_SCHEDULE,
	T_SCHEDULER_SET_AFFINITY,
	T_SCHEDULER_START_IDLE,
	T_SCHEDULER_UNBLOCK,
	T_SCHEDULER_UNMAP_PRIORITY,
	T_SCHEDULER_UNPIN,
	T_SCHEDULER_UPDATE_PRIORITY,
	T_SCHEDULER_WITHDRAW_NODE,
	T_SCHEDULER_YIELD
} T_scheduler_operation;

typedef struct {
	Thread_Control *executing;
	uint32_t cpu;
	T_time instant;
	T_scheduler_operation operation;
	Thread_Control *thread;
	Scheduler_Node *node;
	union {
		struct {
			Priority_Control in;
			Priority_Control out;
		} map_unmap_priority;
		struct {
			bool success;
		} ask_for_help;
		struct {
#ifdef RTEMS_SMP
			Thread_Scheduler_state next_state;
#else
			int next_state;
#endif
		} withdraw_node;
		struct {
			struct Per_CPU_Control *cpu;
		} pin_unpin;
		struct {
			Thread_Control *idle;
		} add_processor;
		struct {
			struct Per_CPU_Control *cpu;
			Thread_Control *idle;
		} remove_processor;
		struct {
			Priority_Node *priority;
			uint64_t deadline;
		} release_job;
		struct {
			Priority_Node *priority;
		} cancel_job;
		struct {
			Processor_mask affinity;
			Status_Control status;
		} set_affinity;
	};
} T_scheduler_event;

typedef struct {
	size_t recorded;
	size_t capacity;
	uint64_t operations;
} T_scheduler_header;

typedef struct {
	T_scheduler_header header;
	T_scheduler_event events[T_ZERO_LENGTH_ARRAY];
} T_scheduler_log;

typedef struct {
	T_scheduler_header header;
	T_scheduler_event events[2];
} T_scheduler_log_2;

typedef struct {
	T_scheduler_header header;
	T_scheduler_event events[4];
} T_scheduler_log_4;

typedef struct {
	T_scheduler_header header;
	T_scheduler_event events[10];
} T_scheduler_log_10;

typedef struct {
	T_scheduler_header header;
	T_scheduler_event events[20];
} T_scheduler_log_20;

typedef struct {
	T_scheduler_header header;
	T_scheduler_event events[40];
} T_scheduler_log_40;

T_scheduler_log *T_scheduler_record(T_scheduler_log *);

T_scheduler_log *T_scheduler_record_2(T_scheduler_log_2 *);

T_scheduler_log *T_scheduler_record_4(T_scheduler_log_4 *);

T_scheduler_log *T_scheduler_record_10(T_scheduler_log_10 *);

T_scheduler_log *T_scheduler_record_20(T_scheduler_log_20 *);

T_scheduler_log *T_scheduler_record_40(T_scheduler_log_40 *);

typedef enum {
	T_SCHEDULER_BEFORE,
	T_SCHEDULER_AFTER
} T_scheduler_when;

typedef void (*T_scheduler_event_handler)(void *, const T_scheduler_event *,
    T_scheduler_when);

void T_scheduler_set_event_handler(T_scheduler_event_handler, void *);

extern const T_scheduler_event T_scheduler_event_null;

const T_scheduler_event *T_scheduler_next(T_scheduler_header *,
    T_scheduler_operation, size_t *);

const T_scheduler_event *T_scheduler_next_any(T_scheduler_header *,
    size_t *);

void T_scheduler_initialize(const Scheduler_Control *);

void T_scheduler_schedule(const Scheduler_Control *, Thread_Control *);

void T_scheduler_yield(const Scheduler_Control *, Thread_Control *,
    Scheduler_Node *);

void T_scheduler_block(const Scheduler_Control *, Thread_Control *,
    Scheduler_Node *);

void T_scheduler_unblock(const Scheduler_Control *, Thread_Control *,
    Scheduler_Node *);

void T_scheduler_update_priority(const Scheduler_Control *, Thread_Control *,
    Scheduler_Node *);

Priority_Control T_scheduler_map_priority(const Scheduler_Control *,
    Priority_Control);

Priority_Control T_scheduler_unmap_priority(const Scheduler_Control *,
    Priority_Control);

void T_scheduler_node_initialize(const Scheduler_Control *, Scheduler_Node *,
    Thread_Control *, Priority_Control);

void T_scheduler_node_destroy(const Scheduler_Control *, Scheduler_Node *);

void T_scheduler_release_job(const Scheduler_Control *, Thread_Control *,
    Priority_Node *, uint64_t, Thread_queue_Context *);

void T_scheduler_cancel_job(const Scheduler_Control *, Thread_Control *,
    Priority_Node *, Thread_queue_Context *);

void T_scheduler_start_idle(const Scheduler_Control *, Thread_Control *,
    struct Per_CPU_Control *);

#ifdef RTEMS_SMP
bool T_scheduler_ask_for_help(const Scheduler_Control *, Thread_Control *,
    Scheduler_Node *);

void T_scheduler_reconsider_help_request(const Scheduler_Control *,
    Thread_Control *, Scheduler_Node *);

void T_scheduler_withdraw_node(const Scheduler_Control *, Thread_Control *,
    Scheduler_Node *, Thread_Scheduler_state);

void T_scheduler_make_sticky(const Scheduler_Control *, Thread_Control *,
    Scheduler_Node *);

void T_scheduler_clean_sticky(const Scheduler_Control *, Thread_Control *,
    Scheduler_Node *);

void T_scheduler_pin(const Scheduler_Control *, Thread_Control *,
    Scheduler_Node *, struct Per_CPU_Control *);

void T_scheduler_unpin(const Scheduler_Control *, Thread_Control *,
    Scheduler_Node *, struct Per_CPU_Control *);

void T_scheduler_add_processor(const Scheduler_Control *, Thread_Control *);

Thread_Control *T_scheduler_remove_processor(const Scheduler_Control *,
    struct Per_CPU_Control *);

Status_Control T_scheduler_set_affinity(const Scheduler_Control *,
    Thread_Control *, Scheduler_Node *, const Processor_mask *);
#endif

#ifdef RTEMS_SMP
#define T_SCHEDULER_ENTRY_POINTS { T_scheduler_initialize, \
    T_scheduler_schedule, T_scheduler_yield, T_scheduler_block, \
    T_scheduler_unblock, T_scheduler_update_priority, \
    T_scheduler_map_priority, T_scheduler_unmap_priority, \
    T_scheduler_ask_for_help, T_scheduler_reconsider_help_request, \
    T_scheduler_withdraw_node, T_scheduler_make_sticky, \
    T_scheduler_clean_sticky, T_scheduler_pin, T_scheduler_unpin, \
    T_scheduler_add_processor, T_scheduler_remove_processor, \
    T_scheduler_node_initialize, T_scheduler_node_destroy, \
    T_scheduler_release_job, T_scheduler_cancel_job, \
    T_scheduler_start_idle, T_scheduler_set_affinity }
#else
#define T_SCHEDULER_ENTRY_POINTS { T_scheduler_initialize, \
    T_scheduler_schedule, T_scheduler_yield, T_scheduler_block, \
    T_scheduler_unblock, T_scheduler_update_priority, \
    T_scheduler_map_priority, T_scheduler_unmap_priority, \
    T_scheduler_node_initialize, T_scheduler_node_destroy, \
    T_scheduler_release_job, T_scheduler_cancel_job, \
    T_scheduler_start_idle }
#endif

extern const Scheduler_Operations T_scheduler_operations[];

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_TEST_SCHEDULER_H */

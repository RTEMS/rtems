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

#include <rtems/score/heapimpl.h>
#include <rtems/score/wkspace.h>
#include <rtems/malloc.h>

typedef struct {
	Heap_Information_block heap_info;
	Heap_Information_block workspace_info;
} T_resource_heap_context;

static T_resource_heap_context T_resource_heap_instance;

static void
T_get_heap_info(Heap_Control *heap, Heap_Information_block *info)
{
	_Heap_Get_information(heap, info);
	memset(&info->Stats, 0, sizeof(info->Stats));
}

static void
T_heap_run_initialize(void)
{
	T_resource_heap_context *ctx;

	ctx = &T_resource_heap_instance;
	T_get_heap_info(&_Workspace_Area, &ctx->workspace_info);

	if (!rtems_configuration_get_unified_work_area()) {
		T_get_heap_info(RTEMS_Malloc_Heap, &ctx->heap_info);
	}
}

static void
T_heap_case_end(void)
{
	T_resource_heap_context *ctx;
	Heap_Information_block info;
	bool ok;

	ctx = &T_resource_heap_instance;

	T_get_heap_info(&_Workspace_Area, &info);
	ok = memcmp(&info, &ctx->workspace_info, sizeof(info)) == 0;

	if (!ok) {
		const char *where;

		if (rtems_configuration_get_unified_work_area()) {
			where = "workspace or heap";
		} else {
			where = "workspace";
		}

		T_check_true(ok, NULL, "memory leak in %s", where);
		memcpy(&ctx->workspace_info, &info, sizeof(info));
	}

	if (!rtems_configuration_get_unified_work_area()) {
		T_get_heap_info(RTEMS_Malloc_Heap, &info);
		ok = memcmp(&info, &ctx->heap_info, sizeof(info)) == 0;

		if (!ok) {
			T_check_true(ok, NULL, "memory leak in heap");
			memcpy(&ctx->heap_info, &info, sizeof(info));
		}
	}
}

void
T_check_heap(T_event event, const char *name)
{
	(void)name;

	switch (event) {
	case T_EVENT_RUN_INITIALIZE_EARLY:
		T_heap_run_initialize();
		break;
	case T_EVENT_CASE_END:
		T_heap_case_end();
		break;
	default:
		break;
	};
}

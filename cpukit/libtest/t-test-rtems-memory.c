/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestFrameworkImpl
 *
 * @brief This source file contains the implementation of
 *   T_memory_allocate(), T_memory_deallocate(), and T_memory_action().
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

#include <rtems/test.h>

#include <rtems/score/memory.h>
#include <rtems/sysinit.h>

static Memory_Area T_memory_areas[1];

static Memory_Information T_memory_information =
    MEMORY_INFORMATION_INITIALIZER(T_memory_areas);

static void
T_memory_initialize(void)
{
	const Memory_Information *mem;
	Memory_Area *area;
	void *begin;
	uintptr_t size;

	mem = _Memory_Get();
	area = _Memory_Get_area(mem, 0);
	begin = _Memory_Get_free_begin(area);
	size = _Memory_Get_free_size(area) / 2;
	_Memory_Consume(area, size);
	_Memory_Initialize_by_size(&T_memory_areas[0], begin, size);
}

RTEMS_SYSINIT_ITEM(T_memory_initialize, RTEMS_SYSINIT_WORKSPACE,
    RTEMS_SYSINIT_ORDER_FIRST);

void *
T_memory_allocate(size_t size)
{
	return _Memory_Allocate(&T_memory_information, size,
	    CPU_HEAP_ALIGNMENT);
}

void
T_memory_deallocate(void *ptr)
{
	(void)ptr;
}

void
T_memory_action(T_event event, const char *name)
{
	Memory_Area *area;

	(void)name;

	switch (event) {
	case T_EVENT_CASE_BEGIN:
		area = &T_memory_areas[0];
		_Memory_Set_free_begin(area,
		    RTEMS_DECONST(void *, _Memory_Get_begin(area)));
		break;
	default:
		break;
	};
}

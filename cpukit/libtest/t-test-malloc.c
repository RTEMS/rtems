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

#include <stdlib.h>

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

	free(dtor);
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

	dtor = malloc(new_size);
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
	free(dtor);
}

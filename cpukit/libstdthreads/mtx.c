/*-
 * Copyright (c) 2011 Ed Schouten <ed@FreeBSD.org>
 * Copyright (c) 2015 embedded brains GmbH <info@embedded-brains.de>
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD r279326 2015-02-26T16:39:57Z$
 */

#include <threads.h>
#include <sys/lock.h>
#include <errno.h>

void
mtx_destroy(mtx_t *mtx)
{

	_Mutex_recursive_Destroy(mtx);
}

int
mtx_init(mtx_t *mtx, int type)
{

	(void)type;
	_Mutex_recursive_Initialize(mtx);
	return (thrd_success);
}

int
mtx_lock(mtx_t *mtx)
{

	_Mutex_recursive_Acquire(mtx);
	return (thrd_success);
}

int
mtx_timedlock(mtx_t *__restrict mtx, const struct timespec *__restrict ts)
{

	switch (_Mutex_recursive_Acquire_timed(mtx, ts)) {
	case 0:
		return (thrd_success);
	case ETIMEDOUT:
		return (thrd_timedout);
	default:
		return (thrd_error);
	}
}

int
mtx_trylock(mtx_t *mtx)
{

	switch (_Mutex_recursive_Try_acquire(mtx)) {
	case 0:
		return (thrd_success);
	default:
		return (thrd_busy);
	}
}

int
mtx_unlock(mtx_t *mtx)
{

	_Mutex_recursive_Release(mtx);
	return (thrd_success);
}

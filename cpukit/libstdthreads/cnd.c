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
 * $FreeBSD r228904 2011-12-26T21:51:53Z$
 */

#include <threads.h>
#include <errno.h>

int
cnd_broadcast(cnd_t *cond)
{

	_Condition_Broadcast(cond);
	return (thrd_success);
}

void
cnd_destroy(cnd_t *cond)
{

	_Condition_Destroy(cond);
}

int
cnd_init(cnd_t *cond)
{

	_Condition_Initialize(cond);
	return (thrd_success);
}

int
cnd_signal(cnd_t *cond)
{

	_Condition_Signal(cond);
	return (thrd_success);
}

int
cnd_timedwait(cnd_t *__restrict cond, mtx_t *__restrict mtx,
    const struct timespec *__restrict ts)
{

	switch (_Condition_Wait_recursive_timed(cond, mtx, ts)) {
	case 0:
		return (thrd_success);
	case ETIMEDOUT:
		return (thrd_timedout);
	default:
		return (thrd_error);
	}
}

int
cnd_wait(cnd_t *cond, mtx_t *mtx)
{

	_Condition_Wait_recursive(cond, mtx);
	return (thrd_success);
}

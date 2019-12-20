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

#include <rtems/libio_.h>

static int T_open_fds;

static int
T_count_open_fds(void)
{
	int free_count;
	rtems_libio_t *iop;

	free_count = 0;
	rtems_libio_lock();

	iop = rtems_libio_iop_free_head;
	while (iop != NULL) {
		++free_count;
		iop = iop->data1;
	}

	rtems_libio_unlock();
	return (int)rtems_libio_number_iops - free_count;
}

static void
T_check_open_fds(void)
{
	int open_fds;
	int delta;

	open_fds = T_count_open_fds();
	delta = open_fds - T_open_fds;

	if (delta != 0) {
		T_open_fds = open_fds;
		T_check_true(false, NULL, "file descriptor leak (%+i)", delta);
	}
}

void
T_check_file_descriptors(T_event event, const char *name)
{
	(void)name;

	switch (event) {
	case T_EVENT_RUN_INITIALIZE_EARLY:
		T_open_fds = T_count_open_fds();
		break;
	case T_EVENT_CASE_END:
		T_check_open_fds();
		break;
	default:
		break;
	};
}

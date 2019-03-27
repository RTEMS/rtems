#include <t.h>

#include <stdlib.h>

#include <rtems.h>

T_TEST_CASE(missing_sema_delete)
{
	rtems_status_code sc;
	rtems_id id;

	sc = rtems_semaphore_create(rtems_build_name('S', 'E', 'M', 'A'), 0,
	    RTEMS_COUNTING_SEMAPHORE, 0, &id);
	T_rsc_success(sc);
}

T_TEST_CASE(missing_free)
{
	void *p;

	p = malloc(1);
	T_not_null(p);
}

#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>

T_TEST_CASE(missing_close)
{
	int fd;

	fd = open("/", O_RDONLY);
	T_ge_int(fd, 0);
}

T_TEST_CASE(missing_barrier_delete)
{
	rtems_status_code sc;
	rtems_id id;

	sc = rtems_barrier_create(rtems_build_name('T', 'E', 'S', 'T'),
	    RTEMS_DEFAULT_ATTRIBUTES, 1, &id);
	T_rsc_success(sc);
}

T_TEST_CASE(missing_extension_delete)
{
	rtems_status_code sc;
	rtems_id id;
	rtems_extensions_table table;

	memset(&table, 0, sizeof(table));
	sc = rtems_extension_create(rtems_build_name('T', 'E', 'S', 'T'),
	    &table, &id);
	T_rsc_success(sc);
}

T_TEST_CASE(missing_mq_delete)
{
	rtems_status_code sc;
	rtems_id id;

	sc = rtems_message_queue_create(rtems_build_name('T', 'E', 'S', 'T'),
	    1, 1, RTEMS_DEFAULT_ATTRIBUTES, &id);
	T_rsc_success(sc);
}

T_TEST_CASE(missing_part_delete)
{
	rtems_status_code sc;
	rtems_id id;
	long buf[32];

	sc = rtems_partition_create(rtems_build_name('T', 'E', 'S', 'T'), buf,
	    sizeof(buf), sizeof(buf), RTEMS_DEFAULT_ATTRIBUTES, &id);
	T_rsc_success(sc);
}

T_TEST_CASE(missing_period_delete)
{
	rtems_status_code sc;
	rtems_id id;

	sc = rtems_rate_monotonic_create(rtems_build_name('T', 'E', 'S', 'T'),
	    &id);
	T_rsc_success(sc);
}

T_TEST_CASE(missing_region_delete)
{
	rtems_status_code sc;
	rtems_id id;
	long buf[32];

	sc = rtems_region_create(rtems_build_name('T', 'E', 'S', 'T'), buf,
	    sizeof(buf), 1, RTEMS_DEFAULT_ATTRIBUTES, &id);
	T_rsc_success(sc);
}

T_TEST_CASE(missing_task_delete)
{
	rtems_status_code sc;
	rtems_id id;

	sc = rtems_task_create(rtems_build_name('T', 'E', 'S', 'T'), 1,
	    RTEMS_MINIMUM_STACK_SIZE, RTEMS_DEFAULT_MODES,
	    RTEMS_DEFAULT_ATTRIBUTES, &id);
	T_rsc_success(sc);
}

T_TEST_CASE(missing_timer_delete)
{
	rtems_status_code sc;
	rtems_id id;

	sc = rtems_timer_create(rtems_build_name('T', 'E', 'S', 'T'), &id);
	T_rsc_success(sc);
}

T_TEST_CASE(missing_key_delete)
{
	pthread_key_t key;
	int eno;

	eno = pthread_key_create(&key, NULL);
	T_psx_success(eno);

	eno = pthread_setspecific(key, &key);
	T_psx_success(eno);
}

/*
 * The license is at the end of the file to be able to use the test code and
 * output in examples in the documentation.  This is also the reason for the
 * dual licensing.  The license for RTEMS documentation is CC-BY-SA-4.0.
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause OR CC-BY-SA-4.0
 *
 * Copyright (C) 2018, 2019 embedded brains GmbH
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
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * Creative Commons Attribution-ShareAlike 4.0 International Public License as
 * published by Creative Commons, PO Box 1866, Mountain View, CA 94042
 * (https://creativecommons.org/licenses/by-sa/4.0/legalcode).
 */

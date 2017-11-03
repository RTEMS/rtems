/*
 * Copyright (c) 2014, 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>

#include <rtems.h>
#include <rtems/libcsupport.h>

#include "tmacros.h"

const char rtems_test_name[] = "SPTLS 2";

static thread_local long i123 = 123;

alignas(256) static thread_local long a256 = 256;

static thread_local long i0;

alignas(512) static thread_local long a512;

static void clobber()
{
	i123 = 0xdead0001;
	a256 = 0xdead0002;
	i0 = 0xdead0003;
	a512 = 0xdead0004;
}

static long f456(bool clobber)
{
	static thread_local long fi456 = 456;

	if (clobber) {
		fi456 = 0xdead0003;
	}

	return fi456;
}

static long f0(bool clobber)
{
	static thread_local long fi0;

	if (clobber) {
		fi0 = 0xdead0004;
	}

	return fi0;
}

class C {
	public:
		static long c789()
		{
			return ci789;
		}

		static long c0()
		{
			return ci0;
		}

		static void clobber()
		{
			ci789 = 0xdead0005;
			ci0 = 0xdead0006;
		}

	private:
		static thread_local long ci789;

		static thread_local long ci0;
};

thread_local long C::ci789 = 789;

thread_local long C::ci0;

class A {
	public:
		A(long i)
			: ii(i), c(gc)
		{
			++gc;
		}

		~A()
		{
			--gc;
		}

		long i() const
		{
			return ii;
		}

		void clobber()
		{
			ii = ~ii;
			c = ~c;
		}

		long counter() const
		{
			return c;
		}

		static long globalCounter()
		{
			return gc;
		}

	private:
		static long gc;

		long ii;

		long c;
};

long A::gc;

static volatile long mc;

static thread_local A a1(mc + 1);
static thread_local A a2(mc + 2);
static thread_local A a3(mc + 3);

static void checkTLSValues()
{
	rtems_test_assert(i123 == 123);
	rtems_test_assert(a256 == 256);
	rtems_test_assert((a256 & 255) == 0);
	rtems_test_assert(i0 == 0);
	rtems_test_assert(a512 == 0);
	rtems_test_assert((a512 & 511) == 0);
	rtems_test_assert(f456(false) == 456);
	rtems_test_assert(f0(false) == 0);
	rtems_test_assert(C::c789() == 789);
	rtems_test_assert(C::c0() == 0);
	rtems_test_assert(a1.i() == 1);
	rtems_test_assert(a2.i() == 2);
	rtems_test_assert(a3.i() == 3);
}

static rtems_id masterTask;

static void wakeUpMaster()
{
	rtems_status_code sc = rtems_event_transient_send(masterTask);
	rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void waitForWorker()
{
	rtems_status_code sc = rtems_event_transient_receive(
		RTEMS_WAIT,
		RTEMS_NO_TIMEOUT
	);
	rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void worker(rtems_task_argument arg)
{
	wakeUpMaster();
	checkTLSValues();

	const long gc = static_cast<long>(arg);

	rtems_test_assert(A::globalCounter() == gc + 3);

	rtems_test_assert(a1.counter() == gc + 0);
	rtems_test_assert(a2.counter() == gc + 1);
	rtems_test_assert(a3.counter() == gc + 2);

	clobber();
	f456(true);
	f0(true);
	C::clobber();
	a1.clobber();
	a2.clobber();
	a3.clobber();

	wakeUpMaster();

	(void) rtems_task_suspend(RTEMS_SELF);
	rtems_test_assert(false);
}

static void testWorkerTask()
{
	checkTLSValues();

	rtems_id id;
	rtems_status_code sc = rtems_task_create(
		rtems_build_name('T', 'A', 'S', 'K'),
		2,
		RTEMS_MINIMUM_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_DEFAULT_ATTRIBUTES,
		&id
	);
	rtems_test_assert(sc == RTEMS_SUCCESSFUL);

	const long gc = A::globalCounter();

	sc = rtems_task_start(id, worker, gc);
	rtems_test_assert(sc == RTEMS_SUCCESSFUL);

	waitForWorker();
	rtems_test_assert(A::globalCounter() == gc);

	waitForWorker();
	rtems_test_assert(A::globalCounter() == gc + 3);

	sc = rtems_task_restart(id, gc);
	rtems_test_assert(sc == RTEMS_SUCCESSFUL);

	waitForWorker();
	rtems_test_assert(A::globalCounter() == gc);

	waitForWorker();
	rtems_test_assert(A::globalCounter() == gc + 3);

	sc = rtems_task_delete(id);
	rtems_test_assert(sc == RTEMS_SUCCESSFUL);

	rtems_test_assert(A::globalCounter() == gc);

	checkTLSValues();
}

extern "C" void Init(rtems_task_argument arg)
{
	TEST_BEGIN();

	printf("A::globalCounter() = %li\n", A::globalCounter());

	checkTLSValues();

	printf("A::globalCounter() = %li\n", A::globalCounter());

	masterTask = rtems_task_self();

	testWorkerTask();

	rtems_resource_snapshot snapshot;
	rtems_resource_snapshot_take(&snapshot);

	testWorkerTask();

	rtems_test_assert(rtems_resource_snapshot_check(&snapshot));

	TEST_END();

	exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 2
#define CONFIGURE_MAXIMUM_SEMAPHORES 3

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MAXIMUM_POSIX_KEYS 2
#define CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS 2

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

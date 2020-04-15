/**
 * @file
 *
 * @ingroup psxtests
 *
 * @brief Test that the workspace size estimate is correct.
 */

/*
 * Copyright (c) 2014. On-Line Applications Research Corporation (OAR).
 * Copyright (c) 2011-2014 embedded brains GmbH.  All rights reserved.
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

#define _GNU_SOURCE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/test.h>
#include <tmacros.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <mqueue.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>

#include <rtems/libcsupport.h>

const char rtems_test_name[] = "PSXCONFIG 1";

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 5

#define CONFIGURE_MAXIMUM_POSIX_KEYS 23
#if CONFIGURE_MAXIMUM_POSIX_KEYS > 0
  #define CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS CONFIGURE_MAXIMUM_POSIX_KEYS
#endif

#define CONFIGURE_MAXIMUM_BARRIERS 2
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES 7
#define CONFIGURE_MAXIMUM_PARTITIONS 37
#define CONFIGURE_MAXIMUM_PERIODS 41
#define CONFIGURE_MAXIMUM_REGIONS 43
#define CONFIGURE_MAXIMUM_SEMAPHORES 47
#define CONFIGURE_MAXIMUM_TASKS 11
#define CONFIGURE_MAXIMUM_TIMERS 59
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 17

#define POSIX_MQ_COUNT 5
#ifdef RTEMS_POSIX_API
#define CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS 7
#endif
#define CONFIGURE_MAXIMUM_POSIX_SEMAPHORES 41
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE CPU_STACK_MINIMUM_SIZE

#define CONFIGURE_MAXIMUM_POSIX_THREADS 3
#ifdef RTEMS_POSIX_API
#define CONFIGURE_MAXIMUM_POSIX_TIMERS 47
#endif

#ifndef CONFIGURE_MAXIMUM_TASKS
  #define CONFIGURE_MAXIMUM_TASKS 1
#endif

#if CONFIGURE_MAXIMUM_MESSAGE_QUEUES > 0
  #define MQ_0_COUNT 2
  #define MQ_1_COUNT 3
  #define MQ_2_COUNT 5
  #define MQ_3_COUNT 7
  #define MQ_4_COUNT 11
  #define MQ_5_COUNT 13
  #define MQ_6_COUNT 17

  #define MQ_0_SIZE 101
  #define MQ_1_SIZE 103
  #define MQ_2_SIZE 107
  #define MQ_3_SIZE 109
  #define MQ_4_SIZE 113
  #define MQ_5_SIZE 127
  #define MQ_6_SIZE 131

  #define MQ_BUFFER_MEMORY \
    (CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(MQ_0_COUNT, MQ_0_SIZE) \
      + CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(MQ_1_COUNT, MQ_1_SIZE) \
      + CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(MQ_2_COUNT, MQ_2_SIZE) \
      + CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(MQ_3_COUNT, MQ_3_SIZE) \
      + CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(MQ_4_COUNT, MQ_4_SIZE) \
      + CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(MQ_5_COUNT, MQ_5_SIZE) \
      + CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(MQ_6_COUNT, MQ_6_SIZE))

  static const uint8_t mq_counts [CONFIGURE_MAXIMUM_MESSAGE_QUEUES] = {
    MQ_0_COUNT,
    MQ_1_COUNT,
    MQ_2_COUNT,
    MQ_3_COUNT,
    MQ_4_COUNT,
    MQ_5_COUNT,
    MQ_6_COUNT
  };

  static const uint8_t mq_sizes [CONFIGURE_MAXIMUM_MESSAGE_QUEUES] = {
    MQ_0_SIZE,
    MQ_1_SIZE,
    MQ_2_SIZE,
    MQ_3_SIZE,
    MQ_4_SIZE,
    MQ_5_SIZE,
    MQ_6_SIZE
  };
#else
  #define MQ_BUFFER_MEMORY 0
#endif

#if POSIX_MQ_COUNT > 0
  #define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES POSIX_MQ_COUNT

  #define POSIX_MQ_0_COUNT 2
  #define POSIX_MQ_1_COUNT 3
  #define POSIX_MQ_2_COUNT 5
  #define POSIX_MQ_3_COUNT 7
  #define POSIX_MQ_4_COUNT 11

  #define POSIX_MQ_0_SIZE 137
  #define POSIX_MQ_1_SIZE 139
  #define POSIX_MQ_2_SIZE 149
  #define POSIX_MQ_3_SIZE 151
  #define POSIX_MQ_4_SIZE 157

  #define POSIX_MQ_BUFFER_MEMORY \
    (CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(POSIX_MQ_0_COUNT, POSIX_MQ_0_SIZE) \
      + CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(POSIX_MQ_1_COUNT, POSIX_MQ_1_SIZE) \
      + CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(POSIX_MQ_2_COUNT, POSIX_MQ_2_SIZE) \
      + CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(POSIX_MQ_3_COUNT, POSIX_MQ_3_SIZE) \
      + CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(POSIX_MQ_4_COUNT, POSIX_MQ_4_SIZE))

  static const uint8_t posix_mq_counts [POSIX_MQ_COUNT] = {
    POSIX_MQ_0_COUNT,
    POSIX_MQ_1_COUNT,
    POSIX_MQ_2_COUNT,
    POSIX_MQ_3_COUNT,
    POSIX_MQ_4_COUNT
  };

  static const uint8_t posix_mq_sizes [POSIX_MQ_COUNT] = {
    POSIX_MQ_0_SIZE,
    POSIX_MQ_1_SIZE,
    POSIX_MQ_2_SIZE,
    POSIX_MQ_3_SIZE,
    POSIX_MQ_4_SIZE
  };
#else
  #define POSIX_MQ_BUFFER_MEMORY 0
#endif

#define CONFIGURE_MESSAGE_BUFFER_MEMORY \
  (MQ_BUFFER_MEMORY + POSIX_MQ_BUFFER_MEMORY)

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

static rtems_task Init(rtems_task_argument argument);

#include <rtems/confdefs.h>

typedef struct {
  uint64_t data [16];
} area;

#if CONFIGURE_MAXIMUM_PARTITIONS > 0
  static area partition_areas [CONFIGURE_MAXIMUM_PARTITIONS];
#endif

#if CONFIGURE_MAXIMUM_REGIONS > 0
  static area region_areas [CONFIGURE_MAXIMUM_REGIONS];
#endif

static char posix_name [_POSIX_PATH_MAX + 1];

static char *get_posix_name(char a, char b, char c, int i)
{
  posix_name [_POSIX_PATH_MAX - 4] = a;
  posix_name [_POSIX_PATH_MAX - 3] = b;
  posix_name [_POSIX_PATH_MAX - 2] = c;
  posix_name [_POSIX_PATH_MAX - 1] = 'A' + i;

  return posix_name;
}

static void *posix_thread(void *arg RTEMS_UNUSED)
{
  rtems_test_assert(0);
}

static void posix_key_dtor(void *key RTEMS_UNUSED)
{
  /* Do nothing */
}

static void print_info(void)
{
  Heap_Information_block info;
  bool ok = rtems_workspace_get_information(&info);
  rtems_test_assert(ok);

  printf(
    "used blocks = %" PRIuPTR ", "
    "largest used block = %" PRIuPTR ", "
    "used space = %" PRIuPTR "\n"
    "free blocks = %" PRIuPTR ", "
    "largest free block = %" PRIuPTR ", "
    "free space = %" PRIuPTR "\n",
    info.Used.number,
    info.Used.largest,
    info.Used.total,
    info.Free.number,
    info.Free.largest,
    info.Free.total
  );
}

static rtems_task Init(rtems_task_argument argument)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  int eno = 0;
  rtems_id id = RTEMS_ID_NONE;
  rtems_name name = rtems_build_name('C', 'O', 'N', 'F');
  rtems_extensions_table table;
  rtems_resource_snapshot snapshot;
  int i = 0;
  pthread_key_t key;

  TEST_BEGIN();

  memset(posix_name, 'P', sizeof(posix_name) - 1);

  print_info();

  rtems_resource_snapshot_take(&snapshot);
  rtems_test_assert(rtems_resource_snapshot_equal(&snapshot, &snapshot));
  rtems_test_assert(rtems_resource_snapshot_check(&snapshot));

#if CONFIGURE_MAXIMUM_FILE_DESCRIPTORS > 0
  for (i = 3; i < CONFIGURE_MAXIMUM_FILE_DESCRIPTORS; ++i) {
    int oflag = O_WRONLY | O_CREAT | O_TRUNC;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    char path [] = { 'F', 'I', 'L', 'E', 'A' + i, '\0' };
    int fd = open(path, oflag, mode);
    rtems_test_assert(fd == i);
  }
  rtems_resource_snapshot_take(&snapshot);
  rtems_test_assert(
    snapshot.open_files == CONFIGURE_MAXIMUM_FILE_DESCRIPTORS
  );
#endif

#if CONFIGURE_MAXIMUM_USER_EXTENSIONS > 0
  memset(&table, 0, sizeof(table));
  for (i = 0; i < CONFIGURE_MAXIMUM_USER_EXTENSIONS; ++i) {
    sc = rtems_extension_create(name, &table, &id);
    directive_failed(sc, "rtems_extension_create");
  }
  rtems_resource_snapshot_take(&snapshot);
  rtems_test_assert(
    snapshot.rtems_api.active_extensions == CONFIGURE_MAXIMUM_USER_EXTENSIONS
  );
#endif

#if CONFIGURE_MAXIMUM_POSIX_KEYS > 0
  for (i = 0; i < CONFIGURE_MAXIMUM_POSIX_KEYS; ++i) {
    eno = pthread_key_create(&key, posix_key_dtor);
    rtems_test_assert(eno == 0);

    eno = pthread_setspecific(key, (void *) (uintptr_t)(i + 1));
    rtems_test_assert(eno == 0);
  }
  eno = pthread_key_create(&key, posix_key_dtor);
  rtems_test_assert(eno == EAGAIN);
  rtems_resource_snapshot_take(&snapshot);
  rtems_test_assert(
    snapshot.active_posix_keys == CONFIGURE_MAXIMUM_POSIX_KEYS
  );
  rtems_test_assert(
    snapshot.active_posix_key_value_pairs == CONFIGURE_MAXIMUM_POSIX_KEYS
  );
#else
  (void) key;
#endif

#if CONFIGURE_MAXIMUM_BARRIERS > 0
  for (i = 0; i < CONFIGURE_MAXIMUM_BARRIERS; ++i) {
    sc = rtems_barrier_create(name, RTEMS_DEFAULT_ATTRIBUTES, 1, &id);
    directive_failed(sc, "rtems_barrier_create");
  }
  rtems_resource_snapshot_take(&snapshot);
  rtems_test_assert(
    snapshot.rtems_api.active_barriers == CONFIGURE_MAXIMUM_BARRIERS
  );
#endif

#if CONFIGURE_MAXIMUM_MESSAGE_QUEUES > 0
  for (i = 0; i < CONFIGURE_MAXIMUM_MESSAGE_QUEUES; ++i) {
    sc = rtems_message_queue_create(
      name,
      mq_counts [i],
      mq_sizes [i],
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed(sc, "rtems_message_queue_create");
  }
  rtems_resource_snapshot_take(&snapshot);
  rtems_test_assert(
    snapshot.rtems_api.active_message_queues
      == CONFIGURE_MAXIMUM_MESSAGE_QUEUES
  );
#endif

#if CONFIGURE_MAXIMUM_PARTITIONS > 0
  for (i = 0; i < CONFIGURE_MAXIMUM_PARTITIONS; ++i) {
    sc = rtems_partition_create(
      name,
      partition_areas + i,
      sizeof(partition_areas [0]),
      sizeof(partition_areas [0]),
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed(sc, "rtems_partition_create");
  }
  rtems_resource_snapshot_take(&snapshot);
  rtems_test_assert(
    snapshot.rtems_api.active_partitions == CONFIGURE_MAXIMUM_PARTITIONS
  );
#endif

#if CONFIGURE_MAXIMUM_PERIODS > 0
  for (i = 0; i < CONFIGURE_MAXIMUM_PERIODS; ++i) {
    sc = rtems_rate_monotonic_create(name, &id);
    directive_failed(sc, "rtems_rate_monotonic_create");
  }
  rtems_resource_snapshot_take(&snapshot);
  rtems_test_assert(
    snapshot.rtems_api.active_periods == CONFIGURE_MAXIMUM_PERIODS
  );
#endif

#if CONFIGURE_MAXIMUM_REGIONS > 0
  for (i = 0; i < CONFIGURE_MAXIMUM_REGIONS; ++i) {
    sc = rtems_region_create(
      name,
      region_areas + i,
      sizeof(region_areas [0]),
      1,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed(sc, "rtems_region_create");
  }
  rtems_resource_snapshot_take(&snapshot);
  rtems_test_assert(
    snapshot.rtems_api.active_regions == CONFIGURE_MAXIMUM_REGIONS
  );
#endif

#if CONFIGURE_MAXIMUM_SEMAPHORES > 0
  for (i = 0; i < CONFIGURE_MAXIMUM_SEMAPHORES; ++i) {
    sc = rtems_semaphore_create(
      name,
      0,
      RTEMS_DEFAULT_ATTRIBUTES,
      0,
      &id
    );
    directive_failed(sc, "rtems_semaphore_create");
  }
  rtems_resource_snapshot_take(&snapshot);
  rtems_test_assert(
    snapshot.rtems_api.active_semaphores >= CONFIGURE_MAXIMUM_SEMAPHORES
  );
#endif

#if CONFIGURE_MAXIMUM_TASKS > 0
  for (i = 1; i < CONFIGURE_MAXIMUM_TASKS; ++i) {
    sc = rtems_task_create(
      name,
      RTEMS_MINIMUM_PRIORITY,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_LOCAL | RTEMS_FLOATING_POINT,
      &id
    );
    directive_failed(sc, "rtems_task_create");
  }
  rtems_resource_snapshot_take(&snapshot);
  rtems_test_assert(
    snapshot.rtems_api.active_tasks == CONFIGURE_MAXIMUM_TASKS
  );
#endif

#if CONFIGURE_MAXIMUM_TIMERS > 0
  for (i = 0; i < CONFIGURE_MAXIMUM_TIMERS; ++i) {
    sc = rtems_timer_create(name, &id);
    directive_failed(sc, "rtems_timer_create");
  }
  rtems_resource_snapshot_take(&snapshot);
  rtems_test_assert(
    snapshot.rtems_api.active_timers == CONFIGURE_MAXIMUM_TIMERS
  );
#endif

#if POSIX_MQ_COUNT > 0
  for (i = 0; i < POSIX_MQ_COUNT; ++i) {
    int oflag = O_RDWR | O_CREAT | O_EXCL;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    struct mq_attr attr = {
      .mq_maxmsg = posix_mq_counts [i],
      .mq_msgsize = posix_mq_sizes [i]
    };
    char *path = get_posix_name('M', 'S', 'Q', i);
    mqd_t mq = mq_open(path, oflag, mode, &attr);
    rtems_test_assert(mq >= 0);
  }
  rtems_resource_snapshot_take(&snapshot);
  rtems_test_assert(
    snapshot.posix_api.active_message_queues
      == CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES
  );
#endif

#if CONFIGURE_MAXIMUM_POSIX_SEMAPHORES > 0
  for (i = 0; i < CONFIGURE_MAXIMUM_POSIX_SEMAPHORES; ++i) {
    int oflag = O_RDWR | O_CREAT | O_EXCL;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    unsigned int value = 0;
    char *path = get_posix_name('S', 'E', 'M', i);
    sem_t *sem = sem_open(path, oflag, mode, value);
    rtems_test_assert(sem != SEM_FAILED);
  }
  rtems_resource_snapshot_take(&snapshot);
  rtems_test_assert(
    snapshot.posix_api.active_semaphores == CONFIGURE_MAXIMUM_POSIX_SEMAPHORES
  );
#endif

#if CONFIGURE_MAXIMUM_POSIX_THREADS > 0
  for (i = 0; i < CONFIGURE_MAXIMUM_POSIX_THREADS; ++i) {
    pthread_t thread;
    pthread_attr_t attr;
    size_t stack_size;

    eno = pthread_create(&thread, NULL, posix_thread, NULL);
    rtems_test_assert(eno == 0);

    eno = pthread_getattr_np(thread, &attr);
    rtems_test_assert(eno == 0);

    eno = pthread_attr_getstacksize(&attr, &stack_size);
    rtems_test_assert(eno == 0);
    rtems_test_assert(stack_size == CPU_STACK_MINIMUM_SIZE);

    eno = pthread_attr_destroy(&attr);
    rtems_test_assert(eno == 0);
  }
  rtems_resource_snapshot_take(&snapshot);
  rtems_test_assert(
    snapshot.posix_api.active_threads == CONFIGURE_MAXIMUM_POSIX_THREADS
  );
#endif

#if CONFIGURE_MAXIMUM_POSIX_TIMERS > 0
  for (i = 0; i < CONFIGURE_MAXIMUM_POSIX_TIMERS; ++i) {
    timer_t timer_id;
    int rv;

    rv = timer_create(CLOCK_REALTIME, NULL, &timer_id);
    rtems_test_assert(rv == 0);
  }
  rtems_resource_snapshot_take(&snapshot);
  rtems_test_assert(
    snapshot.posix_api.active_timers == CONFIGURE_MAXIMUM_POSIX_TIMERS
  );
#endif

  printf("object creation done\n");
  print_info();

  TEST_END();

  rtems_test_exit(0);
}

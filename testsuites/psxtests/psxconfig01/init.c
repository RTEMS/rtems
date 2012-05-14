/**
 * @file
 *
 * @ingroup psxtests
 *
 * @brief Test that the workspace size estimate is correct.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

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

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 5

#define CONFIGURE_MAXIMUM_PARTITIONS 37
#define CONFIGURE_MAXIMUM_PERIODS 41
#define CONFIGURE_MAXIMUM_REGIONS 43
#define CONFIGURE_MAXIMUM_SEMAPHORES 47
#define CONFIGURE_MAXIMUM_TASKS 11
#define CONFIGURE_MAXIMUM_TASK_VARIABLES 13
#define CONFIGURE_MAXIMUM_TIMERS 59
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS 17

#define CONFIGURE_MAXIMUM_POSIX_BARRIERS 31
#define CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES 29
#define CONFIGURE_MAXIMUM_POSIX_KEYS 23
#define POSIX_MQ_COUNT 5
#define CONFIGURE_MAXIMUM_POSIX_MUTEXES 19
#define CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS 7
#define CONFIGURE_MAXIMUM_POSIX_RWLOCKS 31
#define CONFIGURE_MAXIMUM_POSIX_SEMAPHORES 41
#define CONFIGURE_MAXIMUM_POSIX_SPINLOCKS 17
#define CONFIGURE_MAXIMUM_POSIX_THREADS 3
#define CONFIGURE_MAXIMUM_POSIX_TIMERS 47

#ifndef CONFIGURE_MAXIMUM_TASKS
  #define CONFIGURE_MAXIMUM_TASKS 1
#endif

#ifdef CONFIGURE_MAXIMUM_MESSAGE_QUEUES
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

#ifdef POSIX_MQ_COUNT
  #define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUE_DESCRIPTORS POSIX_MQ_COUNT
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

typedef struct {
  uint64_t data [16];
} area;

#ifdef CONFIGURE_MAXIMUM_PARTITIONS
  static area partition_areas [CONFIGURE_MAXIMUM_PARTITIONS];
#endif

#ifdef CONFIGURE_MAXIMUM_REGIONS
  static area region_areas [CONFIGURE_MAXIMUM_REGIONS];
#endif

static char posix_name [NAME_MAX];

static void *task_var;

static char *get_posix_name(char a, char b, char c, int i)
{
  posix_name [NAME_MAX - 5] = a;
  posix_name [NAME_MAX - 4] = b;
  posix_name [NAME_MAX - 3] = c;
  posix_name [NAME_MAX - 2] = 'A' + i;

  return posix_name;
}

static void task_var_dtor(void *var __attribute__((unused)))
{
  /* Do nothing */
}

static void *posix_thread(void *arg __attribute__((unused)))
{
  rtems_test_assert(0);
}

static void posix_key_dtor(void *key __attribute__((unused)))
{
  /* Do nothing */
}

static void print_info(void)
{
  Heap_Information_block info;
  bool ok = rtems_workspace_get_information(&info);
  rtems_test_assert(ok);

  printf(
    "used blocks = %" PRIu32 ", "
    "largest used block = %" PRIu32 ", "
    "used space = %" PRIu32 "\n"
    "free blocks = %" PRIu32 ", "
    "largest free block = %" PRIu32 ", "
    "free space = %" PRIu32 "\n",
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
  int rv = 0;
  rtems_id id = RTEMS_ID_NONE;
  rtems_name name = rtems_build_name('C', 'O', 'N', 'F');
  rtems_extensions_table table;
  int i = 0;

  puts("\n\n*** POSIX TEST CONFIG 01 ***");

  memset(posix_name, 'P', sizeof(posix_name) - 1);

  print_info();

#ifdef CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS
  for (i = 3; i < CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS; ++i) {
    int oflag = O_WRONLY | O_CREAT | O_TRUNC;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    char path [] = { 'F', 'I', 'L', 'E', 'A' + i, '\0' };
    int fd = open(path, oflag, mode);
    rtems_test_assert(fd == i);
  }
#endif

#ifdef CONFIGURE_MAXIMUM_USER_EXTENSIONS
  memset(&table, 0, sizeof(table));
  for (i = 0; i < CONFIGURE_MAXIMUM_USER_EXTENSIONS; ++i) {
    sc = rtems_extension_create(name, &table, &id);
    directive_failed(sc, "rtems_extension_create");
  }
#endif

#ifdef CONFIGURE_MAXIMUM_BARRIERS
  for (i = 0; i < CONFIGURE_MAXIMUM_BARRIERS; ++i) {
    sc = rtems_barrier_create(name, RTEMS_DEFAULT_ATTRIBUTES, 1, &id);
    directive_failed(sc, "rtems_barrier_create");
  }
#endif

#ifdef CONFIGURE_MAXIMUM_MESSAGE_QUEUES
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
#endif

#ifdef CONFIGURE_MAXIMUM_PARTITIONS
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
#endif

#ifdef CONFIGURE_MAXIMUM_PERIODS
  for (i = 0; i < CONFIGURE_MAXIMUM_PERIODS; ++i) {
    sc = rtems_rate_monotonic_create(name, &id);
    directive_failed(sc, "rtems_rate_monotonic_create");
  }
#endif

#ifdef CONFIGURE_MAXIMUM_REGIONS
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
#endif

#ifdef CONFIGURE_MAXIMUM_SEMAPHORES
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
#endif

#ifdef CONFIGURE_MAXIMUM_TASKS
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
#endif

#ifdef CONFIGURE_MAXIMUM_TASK_VARIABLES
  for (i = 0; i < CONFIGURE_MAXIMUM_TASK_VARIABLES; ++i) {
    sc = rtems_task_variable_add(RTEMS_SELF, &task_var, task_var_dtor);
    directive_failed(sc, "rtems_task_variable_add");
  }
#endif

#ifdef CONFIGURE_MAXIMUM_TIMERS
  for (i = 0; i < CONFIGURE_MAXIMUM_TIMERS; ++i) {
    sc = rtems_timer_create(name, &id);
    directive_failed(sc, "rtems_timer_create");
  }
#endif

#ifdef CONFIGURE_MAXIMUM_POSIX_BARRIERS
  for (i = 0; i < CONFIGURE_MAXIMUM_POSIX_BARRIERS; ++i) {
    pthread_barrier_t barrier;
    eno = pthread_barrier_init(&barrier, NULL, 1);
    rtems_test_assert(eno == 0);
  }
#endif

#ifdef CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES
  for (i = 0; i < CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES; ++i) {
    pthread_cond_t cond;
    eno = pthread_cond_init(&cond, NULL);
    rtems_test_assert(eno == 0);
  }
#endif

#ifdef CONFIGURE_MAXIMUM_POSIX_KEYS
  for (i = 0; i < CONFIGURE_MAXIMUM_POSIX_KEYS; ++i) {
    pthread_key_t key;
    eno = pthread_key_create(&key, posix_key_dtor);
    rtems_test_assert(eno == 0);
  }
#endif

#ifdef POSIX_MQ_COUNT
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
#endif

#ifdef CONFIGURE_MAXIMUM_POSIX_MUTEXES
  for (i = 0; i < CONFIGURE_MAXIMUM_POSIX_MUTEXES; ++i) {
    pthread_mutex_t mutex;
    eno = pthread_mutex_init(&mutex, NULL);
    rtems_test_assert(eno == 0);
  }
#endif

#ifdef CONFIGURE_MAXIMUM_POSIX_RWLOCKS
  for (i = 0; i < CONFIGURE_MAXIMUM_POSIX_RWLOCKS; ++i) {
    pthread_rwlock_t rwlock;
    eno = pthread_rwlock_init(&rwlock, NULL);
    rtems_test_assert(eno == 0);
  }
#endif

#ifdef CONFIGURE_MAXIMUM_POSIX_SEMAPHORES
  for (i = 0; i < CONFIGURE_MAXIMUM_POSIX_SEMAPHORES; ++i) {
    int oflag = O_RDWR | O_CREAT | O_EXCL;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    unsigned int value = 0;
    char *path = get_posix_name('S', 'E', 'M', i);
    sem_t *sem = sem_open(path, oflag, mode, value);
    rtems_test_assert(sem != SEM_FAILED);
  }
#endif

#ifdef CONFIGURE_MAXIMUM_POSIX_SPINLOCKS
  for (i = 0; i < CONFIGURE_MAXIMUM_POSIX_SPINLOCKS; ++i) {
    pthread_spinlock_t spinlock;
    eno = pthread_spin_init(&spinlock, 0);
    rtems_test_assert(eno == 0);
  }
#endif

#ifdef CONFIGURE_MAXIMUM_POSIX_THREADS
  for (i = 0; i < CONFIGURE_MAXIMUM_POSIX_THREADS; ++i) {
    pthread_t thread;
    eno = pthread_create(&thread, NULL, posix_thread, NULL);
    rtems_test_assert(eno == 0);
  }
#endif

#ifdef CONFIGURE_MAXIMUM_POSIX_TIMERS
  for (i = 0; i < CONFIGURE_MAXIMUM_POSIX_TIMERS; ++i) {
    timer_t timer_id;
    rv = timer_create(CLOCK_REALTIME, NULL, &timer_id);
    rtems_test_assert(rv == 0);
  }
#endif

  printf("object creation done\n");
  print_info();

  puts("*** END OF POSIX TEST CONFIG 01 ***");

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_DRIVERS 2

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

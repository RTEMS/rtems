/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2022 Prashanth S (fishesprashanth@gmail.com)  All rights reserved.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/error.h>
#include <sched.h>
#include <tmacros.h>
#include <unistd.h>
#include <fcntl.h>

#include <dev/can/can.h>

#define TASKS (12)

#define CAN_DEV_FILE "/dev/can-loopback"
#define NUM_TEST_MSGS (0xf)

#define NEXT_TASK_NAME(c1, c2, c3, c4)  \
                 if (c4 == '9') {       \
                   if (c3 == '9') {     \
                     if (c2 == 'z') {   \
                       if (c1 == 'z') { \
                         printf("not enough task letters for names !!!\n"); \
                         exit( 1 );     \
                       } else           \
                         c1++;          \
                       c2 = 'a';        \
                     } else             \
                      c2++;             \
                     c3 = '0';          \
                   } else               \
                     c3++;              \
                   c4 = '0';            \
                 }                      \
                 else                   \
                   c4++                 \

static void test_task(rtems_task_argument);
int can_loopback_init(const char *);
int create_task(int);

static rtems_id task_id[TASKS];
static rtems_id task_test_status[TASKS] = {[0 ... (TASKS - 1)] = false};

const char rtems_test_name[] = "CAN test TX, RX with CAN loopback driver";

/*FIXME: Should Implement one more test application for the 
 * RTR support
 *
 * For testing, the number of successful read and write 
 * count is verified.
 */
static void test_task(rtems_task_argument data)
{
  //sleep so that other tasks will be created.
  sleep(1);

  int fd, task_num = (uint32_t)data;
  uint32_t count = 0, msg_size;

  struct can_msg msg;

  printf("CAN tx and rx for %s\n", CAN_DEV_FILE);

  fd = open(CAN_DEV_FILE, O_RDWR);
  if (fd < 0) {
    printf("open error: task = %u %s: %s\n", task_num, CAN_DEV_FILE, strerror(errno));
  }

  rtems_test_assert(fd >= 0);

  for (int i = 0; i < NUM_TEST_MSGS; i++) {
    printf("test_task %u\n", task_num);

    msg.id = task_num;
    //FIXME: Implement Test cases for other flags also.
    msg.flags = 0;
    msg.len = (i + 1) % 9;

    for (int j = 0; j < msg.len; j++) {
      msg.data[j] = 'a' + j;
    }
  
    msg_size = ((char *)&msg.data[msg.len] - (char *)&msg);

    printf("calling write task = %u\n", task_num);

    count = write(fd, &msg, sizeof(msg));
    rtems_test_assert(count == msg_size);
    printf("task = %u write count = %u\n", task_num, count);

    printf("calling read task = %u\n", task_num);
    count = read(fd, &msg, sizeof(msg));
    rtems_test_assert(count > 0);
    printf("task = %u read count = %u\n", task_num, count);

    printf("received message\n");
    can_print_msg(&msg);

    sleep(1);
  }
  close(fd);

  task_test_status[task_num] = true;

  printf("task exited = %u\n", task_num);
  rtems_task_exit();
}

int create_task(int i)
{
  printf("Creating task %d\n", i);

  rtems_status_code result;
  rtems_name name;    

  char c1 = 'a';
  char c2 = 'a'; 
  char c3 = '1';
  char c4 = '1';

  name = rtems_build_name(c1, c2, c3, c4);

  result = rtems_task_create(name,
                             1,
                             RTEMS_MINIMUM_STACK_SIZE,
                             RTEMS_PREEMPT | RTEMS_TIMESLICE,
                             RTEMS_FIFO | RTEMS_FLOATING_POINT,
                             &task_id[i]);
  if (result != RTEMS_SUCCESSFUL) {
    printf("rtems_task_create error: %s\n", rtems_status_text(result));
    rtems_test_assert(result == RTEMS_SUCCESSFUL);
  }

  printf("number = %3" PRIi32 ", id = %08" PRIxrtems_id ", starting, ", i, task_id[i]);

  fflush(stdout);

  printf("starting task\n");
  result = rtems_task_start(task_id[i],
                            test_task,
                            (rtems_task_argument)i);

  if (result != RTEMS_SUCCESSFUL) {
    printf("rtems_task_start failed %s\n", rtems_status_text(result));
    rtems_test_assert(result == RTEMS_SUCCESSFUL);
  }

  NEXT_TASK_NAME(c1, c2, c3, c4);

  return result;
}

static rtems_task Init(
  rtems_task_argument ignored
)
{
  printf("Init\n");

  int ret;

  rtems_print_printer_fprintf_putc(&rtems_test_printer);

  TEST_BEGIN();

  rtems_task_priority old_priority;
  rtems_mode          old_mode;

  rtems_task_set_priority(RTEMS_SELF, RTEMS_MAXIMUM_PRIORITY - 1, &old_priority);
  rtems_task_mode(RTEMS_PREEMPT,  RTEMS_PREEMPT_MASK, &old_mode);

  ret = can_loopback_init(CAN_DEV_FILE);
  if (ret != RTEMS_SUCCESSFUL) {
    printf("%s failed\n", rtems_test_name);
    rtems_test_assert(ret == RTEMS_SUCCESSFUL);
  }

  for (int i = 0; i < TASKS; i++) {  
    create_task(i);
  }

  /* Do not exit untill all the tasks are exited */
  while (1) {
    int flag = 0;
    for (int i = 0; i < TASKS; i++) {
      if (task_test_status[i] == false) {
        printf("task not exited = %d\n", i);
        sleep(1);
        flag = 1;
        break;
      }
    }
    if (flag == 0) {
      break;
    }
  }

  TEST_END();

  rtems_test_exit( 0 );
}


#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT                     

#define CONFIGURE_MAXIMUM_TASKS (TASKS + TASKS)

#define CONFIGURE_MAXIMUM_SEMAPHORES 10

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS (TASKS * 2)

#define CONFIGURE_TICKS_PER_TIMESLICE       100                                 

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

/* SPDX-License-Identifier: BSD-2-Clause */

/*  Test1
 *
 *  This test uses a hack to disable suto-extend then checks to see only the
 *  requested number of objects are allocated.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
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

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "system.h"
#include "tmacros.h"

#include <rtems/score/objectimpl.h>

void test1()
{
  Objects_Maximum      objects_per_block;
  rtems_status_code    result;
  uint32_t             task_count = 0;
  Objects_Information *the_information;

  char              c1 = 'a';
  char              c2 = 'a';
  char              c3 = '0';
  char              c4 = '0';

  printf( "\n TEST1 : auto-extend disabled.\n" );

  /*
   * This is a major hack and only recommended for a test. Doing this
   * saves having another test.
   */

  the_information =
    _Objects_Information_table[OBJECTS_CLASSIC_API][OBJECTS_RTEMS_TASKS];
  objects_per_block = the_information->objects_per_block;
  the_information->objects_per_block = 0;
  the_information->allocate = _Objects_Allocate_static;

  while (task_count < MAX_TASKS)
  {
    rtems_name name;

    printf(" TEST1 : creating task '%c%c%c%c', ", c1, c2, c3, c4);

    name = rtems_build_name(c1, c2, c3, c4);

    result = rtems_task_create(name,
                               10,
                               RTEMS_MINIMUM_STACK_SIZE,
                               RTEMS_DEFAULT_ATTRIBUTES,
                               RTEMS_LOCAL,
                               &task_id[task_count]);

    if (status_code_bad(result))
      break;

    printf("number = %3" PRIi32 ", id = %08" PRIxrtems_id ", starting, ", task_count, task_id[task_count]);

    fflush(stdout);
    result = rtems_task_start(task_id[task_count],
                              test_task,
                              (rtems_task_argument) task_count);

    if (status_code_bad(result))
      break;

    /*
     *  Update the name.
     */

    NEXT_TASK_NAME(c1, c2, c3, c4);

    task_count++;
  }

  if (task_count >= MAX_TASKS)
    printf( "\nMAX_TASKS too small for work-space size, please make larger !!\n\n" );

  if (task_count != (TASK_ALLOCATION_SIZE - 1)) {
    printf( " FAIL1 : the number of tasks does not equal the expected size -\n"
            "           task created = %" PRIi32 ", required number = %i\n",
            task_count, TASK_ALLOCATION_SIZE);
    exit( 1 );
  }

  destroy_all_tasks("TEST1");

  the_information->objects_per_block = objects_per_block;
  the_information->allocate = _Thread_Allocate_unlimited;

  printf( " TEST1 : completed\n" );
}

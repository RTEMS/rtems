/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @ingroup sptests
 *
 *  @brief This is the init for spcpuset01.
 */

/*
 * Copyright (C) 1989-2013 On-Line Applications Research Corporation (OAR).
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

/*
 *  Fully exercise CPU_SET() methods
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include <rtems.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "system.h"

const char rtems_test_name[] = "SPCPUSET 1";

static void test_cpu_zero_case_1(void)
{
  size_t i;

  /*
   * Set to all zeros and verify
   */
  puts( "Exercise CPU_ZERO, CPU_ISSET, and CPU_COUNT" );
  CPU_ZERO(&set3);

  /* test if all bits clear */
  for (i=0 ; i<CPU_SETSIZE ; i++) {
    rtems_test_assert( CPU_ISSET(i, &set3) == 0 );
  }
  rtems_test_assert( CPU_COUNT(&set3) == 0 );

}

static void test_cpu_fill_case_1(void)
{
  size_t i;

  /*
   * Set to all zeros and verify
   */
  puts( "Exercise CPU_FILL, CPU_ISSET, and CPU_COUNT" );
  CPU_FILL(&set1);

  /* test if all bits clear */
  for (i=0 ; i<CPU_SETSIZE ; i++) {
    rtems_test_assert( CPU_ISSET(i, &set1) == 1 );
  }
  rtems_test_assert( CPU_COUNT(&set1) == _NCPUBITS );
}

static void test_cpu_equal_case_1(void)
{
  /*
   * CPU_EQUAL
   */
  puts( "Exercise CPU_ZERO, CPU_EQUAL, CPU_CMP, and CPU_EMPTY" );
  CPU_ZERO(&set1);
  CPU_ZERO(&set2);

  /* test that all bits are equal */
  rtems_test_assert( CPU_EQUAL(&set1, &set2) );

  /* compare all bits */
  rtems_test_assert( !CPU_CMP(&set1, &set2) );

  /* compare all bits */
  rtems_test_assert( CPU_EMPTY(&set1) );
}

static void test_cpu_set_case_1(size_t cpu)
{
  size_t i;

  /*
   * Set to all zeros and verify
   */
  DPRINT( "Exercise CPU_ZERO, CPU_SET(%zu), and CPU_ISSET\n", cpu );
  CPU_ZERO(&set1);
  CPU_SET(cpu, &set1);

  /* test if all bits except 1 clear */
  for (i=0 ; i<CPU_SETSIZE ; i++) {
    if (i==cpu)
      rtems_test_assert( CPU_ISSET(i, &set1) == 1 );
    else
      rtems_test_assert( CPU_ISSET(i, &set1) == 0 );

     rtems_test_assert( ! CPU_EMPTY(&set1) );
  }
}

static void test_cpu_clr_case_1(size_t cpu)
{
  size_t i;

  /*
   * Set to all zeros and verify
   */
  DPRINT( "Exercise CPU_FILL, CPU_CLR(%zu), and CPU_ISSET\n", cpu );
  CPU_FILL(&set1);
  CPU_CLR(cpu, &set1);

  /* test if all bits except 5 are set */
  for (i=0 ; i<CPU_SETSIZE ; i++) {
    if (i==cpu)
      rtems_test_assert( CPU_ISSET(i, &set1) == 0 );
    else
      rtems_test_assert( CPU_ISSET(i, &set1) == 1 );
  }
}

static void test_cpu_copy_case_1(void)
{
  size_t i;

  /*
   * CPU_EQUAL
   */
  puts( "Exercise CPU_ZERO, CPU_COPY, and CPU_ISSET" );
  CPU_ZERO(&set1);
  CPU_FILL(&set2);

  CPU_COPY(&set1, &set2);

  /* test if all bits clear in set2 */
  for (i=0 ; i<CPU_SETSIZE ; i++) {
    rtems_test_assert( CPU_ISSET(i, &set2) == 0 );
  }
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  size_t    i;

  TEST_BEGIN();

  test_cpu_zero_case_1();
  test_cpu_fill_case_1();
  test_cpu_equal_case_1();
  test_cpu_copy_case_1();

  for (i=0 ; i<CPU_SETSIZE ; i++) {
    test_cpu_set_case_1(i);
    test_cpu_clr_case_1(i);
  }

  cpuset_logic_test();

  TEST_END();
  exit( 0 );
}

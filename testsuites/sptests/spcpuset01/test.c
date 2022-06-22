/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @ingroup sptests
 *
 *  @brief This is the test for spcpuset01.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/*
 *  Fully exercise CPU_SET() methods
 */
#include <rtems.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "system.h"

void test_cpu_and_case_1(size_t cpu1, size_t cpu2);
void test_cpu_nand_case_1(size_t cpu1, size_t cpu2);
void test_cpu_or_case_1(size_t cpu1, size_t cpu2);
void test_cpu_xor_case_1(size_t cpu1, size_t cpu2);
static void test_logic01_setup(size_t cpu1, size_t cpu2);

/*
 *  Make these global so they can always be referenced. Optimization tends
 *  to make them hard to see when on the stack.
 */
cpu_set_t set1;
cpu_set_t set2;
cpu_set_t set3;


void test_cpu_and_case_1(size_t cpu1, size_t cpu2)
{
  size_t i;

  /*  AND set1 and set2 */
  DPRINT( "Exercise CPU_AND with bits %zd,%zd\n", cpu1, cpu2 );
  CPU_AND(&set3, &set1, &set2);

  /* test if all bits clear except cpu1 */
  for (i=0 ; i<CPU_SETSIZE ; i++) {
    if (i== cpu1)
      rtems_test_assert( CPU_ISSET(i, &set3) == 1 );
    else
      rtems_test_assert( CPU_ISSET(i, &set3) == 0 );
  }

}

void test_cpu_nand_case_1(size_t cpu1, size_t cpu2)
{
  size_t i;

  /*
   * FreeBSD renamed CPU_NAND to CPU_ANDNOT.  This change was included in
   * Newlib at some point in time.
   */
#ifdef __BIT_ANDNOT2
   /*  ANDNOT set1 and set2 */
  DPRINT( "Exercise CPU_ANDNOT with bits %zd,%zd\n", cpu1, cpu2 );
  CPU_ANDNOT(&set3, &set1, &set2);
#else
   /*  NAND set1 and set2 */
  DPRINT( "Exercise CPU_NAND with bits %zd,%zd\n", cpu1, cpu2 );
  CPU_NAND(&set3, &set1, &set2);
#endif

  /* test if all bits clear except cpu1 */
  for (i=0 ; i<CPU_SETSIZE ; i++) {
    if (i== cpu2)
      rtems_test_assert( CPU_ISSET(i, &set3) == 1 );
    else
      rtems_test_assert( CPU_ISSET(i, &set3) == 0 );
  }
}

void test_cpu_or_case_1(size_t cpu1, size_t cpu2)
{
  size_t i;

  /*  OR set1 and set2 */
  DPRINT( "Exercise CPU_OR with bits %zd,%zd\n", cpu1, cpu2 );
  CPU_OR(&set3, &set1, &set2);

  /* test if all bits clear except cpu1 */
  for (i=0 ; i<CPU_SETSIZE ; i++) {
    if ((i== cpu1) || (i==cpu2))
      rtems_test_assert( CPU_ISSET(i, &set3) == 1 );
    else
      rtems_test_assert( CPU_ISSET(i, &set3) == 0 );
  }
}

void test_cpu_xor_case_1(size_t cpu1, size_t cpu2)
{
  size_t i;

  /*  XOR set1 and set2 */
  DPRINT( "Exercise CPU_XOR with bits %zd,%zd\n", cpu1, cpu2 );
  CPU_XOR(&set3, &set1, &set2);

  /* test if all bits clear except cpu1 */
  for (i=0 ; i<CPU_SETSIZE ; i++) {
    if (i==cpu2)
      rtems_test_assert( CPU_ISSET(i, &set3) == 1 );
    else
      rtems_test_assert( CPU_ISSET(i, &set3) == 0 );
  }
}

static void test_logic01_setup(size_t cpu1, size_t cpu2)
{
  /*
   * Clear all bits except cpu1 in both sets and cpu2 in set1 only
   */
  CPU_ZERO(&set1);
  CPU_SET(cpu1, &set1);
  CPU_SET(cpu2, &set1);
  CPU_COPY(&set1, &set2);
  CPU_CLR(cpu2, &set2);
}

void cpuset_logic_test()
{
  size_t    i,j;

  for (i=0 ; i<CPU_SETSIZE ; i++) {
    for (j=0 ; j<CPU_SETSIZE ; j++) {
      if (i != j){
        test_logic01_setup(i,j);
        test_cpu_and_case_1(i, j);
        test_cpu_nand_case_1(i, j);
        test_cpu_or_case_1(i, j);
        test_cpu_xor_case_1(i, j);
      }
    }
  }
}

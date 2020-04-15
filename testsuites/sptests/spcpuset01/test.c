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

   /*  NAND set1 and set2 */
  DPRINT( "Exercise CPU_NAND with bits %zd,%zd\n", cpu1, cpu2 );
  CPU_NAND(&set3, &set1, &set2);

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

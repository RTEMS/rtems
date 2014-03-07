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


#if defined(__RTEMS_HAVE_SYS_CPUSET_H__)
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
  rtems_test_assert( CPU_CMP(&set1, &set2) );

  /* compare all bits */
  rtems_test_assert( CPU_EMPTY(&set1) );
}

static void test_cpu_set_case_1(size_t cpu)
{
  size_t i;

  /*
   * Set to all zeros and verify
   */
  printf( "Exercise CPU_ZERO, CPU_SET(%u), and CPU_ISET\n", cpu );
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
  printf( "Exercise CPU_FILL, CPU_CLR(%u), and CPU_ISET\n", cpu );
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
  puts( "Exercise CPU_ZERO, CPU_COPY, and CPU_ISET" );
  CPU_ZERO(&set1);
  CPU_FILL(&set2);

  CPU_COPY(&set2, &set1);

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

  puts( "*** CPUSET01 Test ***" );

  test_cpu_zero_case_1();
  test_cpu_fill_case_1();
  test_cpu_equal_case_1();
  test_cpu_copy_case_1();

  for (i=0 ; i<CPU_SETSIZE ; i++) {
    test_cpu_set_case_1(i);
    test_cpu_clr_case_1(i);
  }

  cpuset_logic_test();

  puts( "*** END OF CPUSET01 Test ***" );
  exit( 0 );
}
#else
#error "Init - No cpuset"
rtems_task Init(
  rtems_task_argument ignored
)
{
  puts( "*** CPUSET01 Test ***" );
  puts( "  cpuset not supported\n" );
  puts( "*** END OF CPUSET01 Test ***" );
  exit( 0 );
}
#endif

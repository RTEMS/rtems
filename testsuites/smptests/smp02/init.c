/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"

#include <stdio.h>

rtems_task Init(
  rtems_task_argument argument
)
{
  int               i;
  char              ch;
  int               cpu_num;
  rtems_id          id;
  rtems_status_code status;
  char              str[80];

  locked_print_initialize();
  locked_printf( "\n\n***  SMP02 TEST ***\n" );

  /* Create/verify synchronisation semaphore */
  status = rtems_semaphore_create(
    rtems_build_name ('S', 'E', 'M', '1'),
    1,                                             
    RTEMS_LOCAL                   |
    RTEMS_SIMPLE_BINARY_SEMAPHORE |
    RTEMS_PRIORITY,
    1,
    &Semaphore);
  directive_failed( status, "rtems_semaphore_create" );

  /* Lock semaphore */
  status = rtems_semaphore_obtain( Semaphore, RTEMS_WAIT, 0);
  directive_failed( status,"rtems_semaphore_obtain of SEM1\n");

  for ( i=1; i < rtems_smp_get_number_of_processors(); i++ ){

    /* Create and start tasks for each CPU */
    ch = '0' + i;

    status = rtems_task_create(
      rtems_build_name( 'T', 'A', ch, ' ' ),
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );

    cpu_num = bsp_smp_processor_id();
    locked_printf(" CPU %d start task TA%c\n", cpu_num, ch);
    status = rtems_task_start( id, Test_task, i+1 );
    directive_failed( status, str );
  }

  /*
   * Release the semaphore, allowing the blocked tasks to start.
   */
  status = rtems_semaphore_release( Semaphore );
  directive_failed( status,"rtems_semaphore_release of SEM1\n");
  

  /* 
   * Wait for log full. print the log and end the program.
   */  
  while (Log_index < LOG_SIZE)
    ;
 
  for (i=0; i< LOG_SIZE; i++) {
    if ( Log[i].IsLocked ) {
      locked_printf(
        " CPU %d Task TA%" PRIu32 " Obtain\n", 
        Log[i].cpu_num,
        Log[i].task_index
      );
    } else {
      locked_printf(
        " CPU %d Task TA%" PRIu32 " Release\n", 
        Log[i].cpu_num,
        Log[i].task_index
      );
    }
  }

  locked_printf( "*** END OF TEST SMP02 ***\n" );
  rtems_test_exit( 0 );
}

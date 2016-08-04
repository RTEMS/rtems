/**
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
/*  updated for triple test, 2003/11/06, Erik Adli */

#include "system.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* CPU usage and Rate monotonic manger statistics */
#include "rtems/cpuuse.h"
//
// Periods for the various tasks [seconds]
#define PERIOD_TASK_1          10000
#define PERIOD_TASK_2          2000


// Task Id
#define ID_TASK_1           0
#define ID_TASK_2          	1

// Execution time for each task
#define task_1_normal_et	6000
#define task_2_normal_et	1000

// TASK 1
rtems_task Task_1(
  rtems_task_argument unused
)
{
  rtems_status_code status;
  rtems_name        period_name; 
  rtems_id          RM_period;
  rtems_id selfid=rtems_task_self();
  double            start, end; //sttime, entime;
  int		    tsk_counter = 0;
  int		    startTick = 0;

  /*create period*/

  period_name = rtems_build_name( 'P', 'E', 'R', '1' );
  status = rtems_rate_monotonic_create( period_name, &RM_period );
  if( RTEMS_SUCCESSFUL != status ) {
    printf("RM failed with status: %d\n", status);
    exit(1);
  }

	while( 1 ) {
    /*this part for release offset*/
		status = rtems_rate_monotonic_period( RM_period,PERIOD_TASK_1);

		if(tsk_counter == 2){
			
				status = rtems_rate_monotonic_delete(RM_period);
				if(status != RTEMS_SUCCESSFUL){
					printf("BUG: Cannot delete the period 1\n");
					exit(1);
				}
                  
				status=rtems_task_delete(selfid);
				if(status != RTEMS_SUCCESSFUL){
					printf("BUG: Cannot delete the task 1\n");
					exit(1);
				}

		}else{
					
			startTick = rtems_clock_get_ticks_since_boot();
			start = startTick  / (double)tick_per_second;

	    printf("Task 1 starts current second %.6f.\n", start);

			LOOP(task_1_normal_et,task_id);
		
			end = rtems_clock_get_ticks_since_boot()/(double)tick_per_second;

			printf("					Task 1 ends at current second  %.6f .\n", end);
 			tsk_counter += 1;			
		}
	}
}

// TASK 2
rtems_task Task_2(
  rtems_task_argument unused
)
{
  rtems_id selfid=rtems_task_self();
  rtems_status_code status;
  rtems_name        period_name; 
  rtems_id          RM_period;
  double 	          start, end;
  int		            tsk_counter = 0;
  int		            startTick = 0;

  /*create period*/

  period_name = rtems_build_name( 'P', 'E', 'R', '2' );
  status = rtems_rate_monotonic_create( period_name, &RM_period );
  if( RTEMS_SUCCESSFUL != status ) {
    printf("RM failed with status: %d\n", status);
    exit(1);
  }

	while( 1 ) {
 
		/*this part for release offset*/
		status = rtems_rate_monotonic_period( RM_period, PERIOD_TASK_2);

		if(experiment_flag ==0){
			status = rtems_rate_monotonic_delete(RM_period);
			if(status != RTEMS_SUCCESSFUL){
				printf("BUG: Cannot delete the period 2\n");
				exit(1);
			}
      exit(1); //before delete itself, otherwise this task is dead.
			status=rtems_task_delete(selfid);
			if(status != RTEMS_SUCCESSFUL){
				printf("BUG: Cannot delete the task 2\n");
				exit(1);
			}		
		}else{

			startTick = rtems_clock_get_ticks_since_boot();
			start = startTick  / (double)tick_per_second;

			printf("Task 2 starts at current second %.6f.\n", start);

			LOOP(task_2_normal_et,task_id);

			end = rtems_clock_get_ticks_since_boot()/(double)tick_per_second;

			printf("					%d Task 2 ends at current second %.6f.\n", tsk_counter+1, end);
	    
			tsk_counter += 1;
			    
      if(tsk_counter == testnumber){        
        rtems_rate_monotonic_report_statistics();
        experiment_flag=0;
      }
		}
	}
}


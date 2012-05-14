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

#include "system.h"

void Loop(void) {
  volatile int i;

  for (i=0; i<300000; i++);
}

void LogSemaphore(
  bool      obtained,
  int       cpu_num,
  uint32_t  task_index
){
  if (Log_index < LOG_SIZE) { 
    /* Log the information */
    Log[ Log_index ].IsLocked   = obtained;
    Log[ Log_index ].cpu_num    = cpu_num;
    Log[ Log_index ].task_index = task_index;
    Log_index++;
  }
}

rtems_task Test_task(
  rtems_task_argument task_index
)
{
  int               cpu_num;
  rtems_status_code sc;

  cpu_num = bsp_smp_processor_id();

  do {

    /* Poll to obtain the synchronization semaphore */
    do {
      sc = rtems_semaphore_obtain( Semaphore, RTEMS_NO_WAIT, 0 );
    } while (sc != RTEMS_SUCCESSFUL );

    LogSemaphore(true, cpu_num, task_index);
    LogSemaphore(false, cpu_num, task_index);
  
    rtems_semaphore_release( Semaphore );
  } while(1);
}

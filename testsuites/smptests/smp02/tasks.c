/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2011.
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

#include "system.h"

static void LogSemaphore(
  bool      obtained,
  uint32_t  cpu_num,
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
  uint32_t          cpu_num;
  rtems_status_code sc;

  cpu_num = rtems_scheduler_get_processor();

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

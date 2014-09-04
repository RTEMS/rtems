/*
  ------------------------------------------------------------------------

  Copyright Objective Design Systems Pty Ltd, 2002
  All rights reserved Objective Design Systems Pty Ltd, 2002
  Chris Johns (ccj@acm.org)

  COPYRIGHT (c) 1989-2009.
  On-Line Applications Research Corporation (OAR).

  The license and distribution terms for this file may be
  found in the file LICENSE in this distribution.

  This software with is provided ``as is'' and with NO WARRANTY.

  ------------------------------------------------------------------------

  RTEMS Performance Monitoring and Measurement Framework.

  This is the Capture Engine component.
rtems_status_code rtems_capture_user_extension_open(void);
rtems_status_code rtems_capture_user_extension_close(void);


*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <rtems/rtems/tasksimpl.h>

#include "captureimpl.h"

#include <rtems/score/statesimpl.h>
#include <rtems/score/todimpl.h>


/*
 * RTEMS Capture User Extension Data.
 */
static rtems_id                 capture_id;

static bool 
rtems_capture_create_task (rtems_tcb* current_task, 
                           rtems_tcb* new_task);

static void
rtems_capture_start_task (rtems_tcb* current_task,
                          rtems_tcb* started_task);

static void
rtems_capture_restart_task (rtems_tcb* current_task,
                            rtems_tcb* restarted_task);

static void
rtems_capture_delete_task (rtems_tcb* current_task,
                           rtems_tcb* deleted_task);

static void
rtems_capture_switch_task (rtems_tcb* current_task,
                           rtems_tcb* heir_task);

static void
rtems_capture_begin_task (rtems_tcb* begin_task);

static void
rtems_capture_exitted_task (rtems_tcb* exitted_task);

static void
rtems_capture_terminated_task (rtems_tcb* terminated_task);

static const rtems_extensions_table capture_extensions = {
  .thread_create    = rtems_capture_create_task,
  .thread_start     = rtems_capture_start_task,
  .thread_restart   = rtems_capture_restart_task,
  .thread_delete    = rtems_capture_delete_task,
  .thread_switch    = rtems_capture_switch_task,
  .thread_begin     = rtems_capture_begin_task,
  .thread_exitted   = rtems_capture_exitted_task,
  .fatal            = NULL,
  .thread_terminate = rtems_capture_terminated_task
};

static inline void rtems_capture_record (
  rtems_tcb*    tcb,
  uint32_t      events
)
{
  rtems_capture_record_t*  rec;
  void*                    ptr;
  size_t                   size = sizeof(*rec);

  if (rtems_capture_filter( tcb, events) )
    return;

  rtems_capture_begin_add_record (tcb, events, size, &ptr);
  rtems_capture_end_add_record ( ptr );
}


rtems_status_code rtems_capture_user_extension_open(void)
{
  rtems_status_code sc;
  rtems_name        name;
  int               index;

  /*
   * Register the user extension handlers for the CAPture Engine.
   */
  name = rtems_build_name ('C', 'A', 'P', 'E');
  sc   = rtems_extension_create (name, &capture_extensions, &capture_id);
  if (sc != RTEMS_SUCCESSFUL)
    capture_id = 0;
  else {
    index = rtems_object_id_get_index (capture_id);
    rtems_capture_set_extension_index( index );
  }

  return sc;
}

rtems_status_code rtems_capture_user_extension_close(void)
{
  rtems_status_code sc;
  sc = rtems_extension_delete (capture_id);
  return sc;
}

/*
 * This function is called when a task is created.
 */
static bool
rtems_capture_create_task (rtems_tcb* ct,
                           rtems_tcb* nt)
{
  /*
   * The task pointers may not be known as the task may have
   * been created before the capture engine was open. Add them.
   */

  if (!rtems_capture_task_recorded (ct))
    rtems_capture_record_task (ct);

  /*
   * Create the new task's capture control block.
   */
  rtems_capture_record_task (nt);

  if (rtems_capture_trigger (ct, nt, RTEMS_CAPTURE_CREATE))
  {
    rtems_capture_record (ct, RTEMS_CAPTURE_CREATED_BY_EVENT);
    rtems_capture_record (nt, RTEMS_CAPTURE_CREATED_EVENT);
  }

  return 1 == 1;
}

/*
 * This function is called when a task is started.
 */
static void
rtems_capture_start_task (rtems_tcb* ct,
                          rtems_tcb* st)
{
  /*
   * The task pointers may not be known as the task may have
   * been created before the capture engine was open. Add them.
   */

  if (!rtems_capture_task_recorded (ct))
    rtems_capture_record_task (ct);

  if (st == NULL)
    rtems_capture_record_task (st);

  if (rtems_capture_trigger (ct, st, RTEMS_CAPTURE_START))
  {
    rtems_capture_record (ct, RTEMS_CAPTURE_STARTED_BY_EVENT);
    rtems_capture_record (st, RTEMS_CAPTURE_STARTED_EVENT);
  }
}

/*
 * This function is called when a task is restarted.
 */
static void
rtems_capture_restart_task (rtems_tcb* ct,
                            rtems_tcb* rt)
{
  /*
   * The task pointers may not be known as the task may have
   * been created before the capture engine was open. Add them.
   */

  if (!rtems_capture_task_recorded (ct))
    rtems_capture_record_task (ct);

  if (!rtems_capture_task_recorded (rt))
    rtems_capture_record_task (rt);

  if (rtems_capture_trigger (ct, rt, RTEMS_CAPTURE_RESTART))
  {
    rtems_capture_record (ct, RTEMS_CAPTURE_RESTARTED_BY_EVENT);
    rtems_capture_record (rt, RTEMS_CAPTURE_RESTARTED_EVENT);
  }
}

/*
 * This function is called when a task is deleted.
 */
static void
rtems_capture_delete_task (rtems_tcb* ct,
                           rtems_tcb* dt)
{
  if (!rtems_capture_task_recorded (ct))
    rtems_capture_record_task (ct);

  if (!rtems_capture_task_recorded (dt))
    rtems_capture_record_task (dt);

  if (rtems_capture_trigger (ct, dt, RTEMS_CAPTURE_DELETE))
  {
    rtems_capture_record (ct, RTEMS_CAPTURE_DELETED_BY_EVENT);
    rtems_capture_record (dt, RTEMS_CAPTURE_DELETED_EVENT);
  }
}

/*
 * This function is called when a task is begun.
 */
static void
rtems_capture_begin_task (rtems_tcb* bt)
{
  /*
   * The task pointers may not be known as the task may have
   * been created before the capture engine was open. Add them.
   */

  if (!rtems_capture_task_recorded (bt))
    rtems_capture_record_task (bt);

  if (rtems_capture_trigger (NULL, bt, RTEMS_CAPTURE_BEGIN))
    rtems_capture_record (bt, RTEMS_CAPTURE_BEGIN_EVENT);
}

/*
 * This function is called when a task is exitted. That is
 * returned rather than was deleted.
 */
static void
rtems_capture_exitted_task (rtems_tcb* et)
{
  /*
   * The task pointers may not be known as the task may have
   * been created before the capture engine was open. Add them.
   */

  if (!rtems_capture_task_recorded (et))
    rtems_capture_record_task (et);

  if (rtems_capture_trigger (NULL, et, RTEMS_CAPTURE_EXITTED))
    rtems_capture_record (et, RTEMS_CAPTURE_EXITTED_EVENT);
}

/*
 * This function is called when a termination request is identified.
 */
static void
rtems_capture_terminated_task (rtems_tcb* tt)
{
  /*
   * The task pointers may not be known as the task may have
   * been created before the capture engine was open. Add them.
   */

  if (!rtems_capture_task_recorded (tt))
    rtems_capture_record_task (tt);

  if (rtems_capture_trigger (NULL, tt, RTEMS_CAPTURE_TERMINATED))
    rtems_capture_record (tt, RTEMS_CAPTURE_TERMINATED_EVENT);
}

/*
 * This function is called when a context is switched.
 */
static void
rtems_capture_switch_task (rtems_tcb* ct,
                           rtems_tcb* ht)
{
  uint32_t flags = rtems_capture_get_flags();

  /*
   * Only perform context switch trace processing if tracing is
   * enabled.
   */
  if (flags & RTEMS_CAPTURE_ON)
  {
    rtems_capture_time_t time;

    if (!rtems_capture_task_recorded (ct))
      rtems_capture_record_task (ct);

    if (!rtems_capture_task_recorded (ht))
      rtems_capture_record_task (ht);

    /*
     * Update the execution time. Assume the time will not overflow
     * for now. This may need to change.
     */
    rtems_capture_get_time (&time);

    if (rtems_capture_trigger (ct, ht, RTEMS_CAPTURE_SWITCH))
    {
      rtems_capture_record (ct, RTEMS_CAPTURE_SWITCHED_OUT_EVENT);
      rtems_capture_record (ht, RTEMS_CAPTURE_SWITCHED_IN_EVENT);
    }
  }
}

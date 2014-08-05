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
  rtems_capture_task_t* task,
  uint32_t              events
)
{
  rtems_capture_record_t*  rec;

  if (rtems_capture_filter( task, events) )
    return;
  
  rtems_capture_begin_add_record (task, events, sizeof(*rec), &rec);

  rtems_capture_end_add_record ( rec );
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
rtems_capture_create_task (rtems_tcb* current_task,
                           rtems_tcb* new_task)
{
  rtems_capture_task_t* ct;
  rtems_capture_task_t* nt;
  int                   index = rtems_capture_get_extension_index();

  ct = current_task->extensions[index];

  /*
   * The task pointers may not be known as the task may have
   * been created before the capture engine was open. Add them.
   */

  if (ct == NULL)
    ct = rtems_capture_create_capture_task (current_task);

  /*
   * Create the new task's capture control block.
   */
  nt = rtems_capture_create_capture_task (new_task);

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
rtems_capture_start_task (rtems_tcb* current_task,
                          rtems_tcb* started_task)
{
  /*
   * Get the capture task control block so we can trace this
   * event.
   */
  rtems_capture_task_t* ct;
  rtems_capture_task_t* st;
  int                   index = rtems_capture_get_extension_index();

  ct = current_task->extensions[index];
  st = started_task->extensions[index];

  /*
   * The task pointers may not be known as the task may have
   * been created before the capture engine was open. Add them.
   */

  if (ct == NULL)
    ct = rtems_capture_create_capture_task (current_task);

  if (st == NULL)
    st = rtems_capture_create_capture_task (started_task);

  if (rtems_capture_trigger (ct, st, RTEMS_CAPTURE_START))
  {
    rtems_capture_record (ct, RTEMS_CAPTURE_STARTED_BY_EVENT);
    rtems_capture_record (st, RTEMS_CAPTURE_STARTED_EVENT);
  }

  rtems_capture_init_stack_usage (st);
}

/*
 * This function is called when a task is restarted.
 */
static void
rtems_capture_restart_task (rtems_tcb* current_task,
                            rtems_tcb* restarted_task)
{
  /*
   * Get the capture task control block so we can trace this
   * event.
   */
  rtems_capture_task_t* ct;
  rtems_capture_task_t* rt;
  int                   index = rtems_capture_get_extension_index();

  ct = current_task->extensions[index];
  rt = restarted_task->extensions[index];

  /*
   * The task pointers may not be known as the task may have
   * been created before the capture engine was open. Add them.
   */

  if (ct == NULL)
    ct = rtems_capture_create_capture_task (current_task);

  if (rt == NULL)
    rt = rtems_capture_create_capture_task (restarted_task);

  if (rtems_capture_trigger (ct, rt, RTEMS_CAPTURE_RESTART))
  {
    rtems_capture_record (ct, RTEMS_CAPTURE_RESTARTED_BY_EVENT);
    rtems_capture_record (rt, RTEMS_CAPTURE_RESTARTED_EVENT);
  }

  rtems_capture_task_stack_usage (rt);
  rtems_capture_init_stack_usage (rt);
}

/*
 * This function is called when a task is deleted.
 */
static void
rtems_capture_delete_task (rtems_tcb* current_task,
                           rtems_tcb* deleted_task)
{
  /*
   * Get the capture task control block so we can trace this
   * event.
   */
  rtems_capture_task_t* ct;
  rtems_capture_task_t* dt;
  int                   index = rtems_capture_get_extension_index();

  /*
   * The task pointers may not be known as the task may have
   * been created before the capture engine was open. Add them.
   */

  ct = current_task->extensions[index];
  dt = deleted_task->extensions[index];

  if (ct == NULL)
    ct = rtems_capture_create_capture_task (current_task);

  if (dt == NULL)
    dt = rtems_capture_create_capture_task (deleted_task);

  if (rtems_capture_trigger (ct, dt, RTEMS_CAPTURE_DELETE))
  {
    rtems_capture_record (ct, RTEMS_CAPTURE_DELETED_BY_EVENT);
    rtems_capture_record (dt, RTEMS_CAPTURE_DELETED_EVENT);
  }

  rtems_capture_task_stack_usage (dt);

  /*
   * This task's tcb will be invalid. This signals the
   * task has been deleted.
   */
  dt->tcb = 0;

  rtems_capture_destroy_capture_task (dt);
}

/*
 * This function is called when a task is begun.
 */
static void
rtems_capture_begin_task (rtems_tcb* begin_task)
{
  /*
   * Get the capture task control block so we can trace this
   * event.
   */
  rtems_capture_task_t* bt;
  int                   index = rtems_capture_get_extension_index();

  bt = begin_task->extensions[index];

  /*
   * The task pointers may not be known as the task may have
   * been created before the capture engine was open. Add them.
   */

  if (bt == NULL)
    bt = rtems_capture_create_capture_task (begin_task);

  if (rtems_capture_trigger (NULL, bt, RTEMS_CAPTURE_BEGIN))
    rtems_capture_record (bt, RTEMS_CAPTURE_BEGIN_EVENT);
}

/*
 * This function is called when a task is exitted. That is
 * returned rather than was deleted.
 */
static void
rtems_capture_exitted_task (rtems_tcb* exitted_task)
{
  /*
   * Get the capture task control block so we can trace this
   * event.
   */
  rtems_capture_task_t* et;
  int                   index = rtems_capture_get_extension_index();

  et = exitted_task->extensions[index];

  /*
   * The task pointers may not be known as the task may have
   * been created before the capture engine was open. Add them.
   */

  if (et == NULL)
    et = rtems_capture_create_capture_task (exitted_task);

  if (rtems_capture_trigger (NULL, et, RTEMS_CAPTURE_EXITTED))
    rtems_capture_record (et, RTEMS_CAPTURE_EXITTED_EVENT);

  rtems_capture_task_stack_usage (et);
}

/*
 * This function is called when a termination request is identified.
 */
static void
rtems_capture_terminated_task (rtems_tcb* terminated_task)
{
  /*
   * Get the capture task control block so we can trace this
   * event.
   */
  rtems_capture_task_t* tt;
  int                   index = rtems_capture_get_extension_index();

  tt = terminated_task->extensions[index];

  /*
   * The task pointers may not be known as the task may have
   * been created before the capture engine was open. Add them.
   */

  if (tt == NULL)
    tt = rtems_capture_create_capture_task (terminated_task);

  if (rtems_capture_trigger (NULL, tt, RTEMS_CAPTURE_TERMINATED))
    rtems_capture_record (tt, RTEMS_CAPTURE_TERMINATED_EVENT);

  rtems_capture_task_stack_usage (tt);
}

/*
 * This function is called when a context is switched.
 */
static void
rtems_capture_switch_task (rtems_tcb* current_task,
                           rtems_tcb* heir_task)
{
  uint32_t flags = rtems_capture_get_flags();
  int      index = rtems_capture_get_extension_index();

  /*
   * Only perform context switch trace processing if tracing is
   * enabled.
   */
  if (flags & RTEMS_CAPTURE_ON)
  {
    rtems_capture_time_t time;

    /*
     * Get the cpature task control block so we can update the
     * reference and perform any watch or trigger functions.
     * The task pointers may not be known as the task may have
     * been created before the capture engine was open. Add them.
     */
    rtems_capture_task_t* ct;
    rtems_capture_task_t* ht;


    if (_States_Is_dormant (current_task->current_state))
    {
      rtems_id ct_id = current_task->Object.id;
      ct = rtems_capture_find_capture_task( ct_id );
    }
    else
    {
      ct = current_task->extensions[index];

      if (ct == NULL)
        ct = rtems_capture_create_capture_task (current_task);
    }

    ht = heir_task->extensions[index];

    if (ht == NULL)
      ht = rtems_capture_create_capture_task (heir_task);

    /*
     * Update the execution time. Assume the time will not overflow
     * for now. This may need to change.
     */
    rtems_capture_get_time (&time);

    /*
     * We could end up with null pointers for both the current task
     * and the heir task.
     */

    if (ht)
    {
      ht->in++;
      ht->time_in = time;
    }

    if (ct)
    {
      ct->out++;
      if (ct->time_in)
        ct->time += time - ct->time_in;
    }

    if (rtems_capture_trigger (ct, ht, RTEMS_CAPTURE_SWITCH))
    {
      rtems_capture_record (ct, RTEMS_CAPTURE_SWITCHED_OUT_EVENT);
      rtems_capture_record (ht, RTEMS_CAPTURE_SWITCHED_IN_EVENT);
    }
  }
}

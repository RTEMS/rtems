/*
  ------------------------------------------------------------------------

  COPYRIGHT (c) 1997
  Objective Design Systems Ltd Pty (ODS)
  All rights reserved (R) Objective Design Systems Ltd Pty

  The license and distribution terms for this file may be found in the
  file LICENSE in this distribution or at
  http://www.rtems.com/license/LICENSE.

  ------------------------------------------------------------------------

  rtemsEvent class.

  This class allows the user to send and receive RTEMS events to a task.

  ------------------------------------------------------------------------ */

#if !defined(_rtemsEvent_h_)
#define _rtemsEvent_h_

#include <rtems++/rtemsStatusCode.h>
#include <rtems++/rtemsTask.h>

/* ----
    rtemsEvent
*/

class rtemsEvent
  : public rtemsStatusCode
{
public:
  // attribute a task can have

  enum WaitMode { wait = RTEMS_WAIT,
                  no_wait = RTEMS_NO_WAIT};
  enum Condition { any = RTEMS_EVENT_ANY,
                   all = RTEMS_EVENT_ALL};

  // only the first 4 characters of the name are taken

  // connect to a task
  rtemsEvent(const char* name, uint32_t node = RTEMS_SEARCH_ALL_NODES);

  // copy and default constructors
  rtemsEvent(const rtemsEvent& event);
  rtemsEvent();

  virtual ~rtemsEvent();

  // connect to an existing task object, will not be the owner
  const rtemsEvent& operator=(const rtemsEvent& event);
  virtual const rtems_status_code connect(const char *name,
                                          const uint32_t node = RTEMS_SEARCH_ALL_NODES);

  // send an event
  inline const rtems_status_code send(const rtems_id task,
                                      const rtems_event_set events);
  inline const rtems_status_code send(const rtemsTask& task,
                                      const rtems_event_set events) ;
  inline const rtems_status_code send(const rtems_event_set events);

  // receive an event, can block a task if no events waiting
  inline const rtems_status_code receive(const rtems_event_set event_in,
                                         rtems_event_set& event_out,
                                         const rtems_interval micro_secs = 0,
                                         const WaitMode wait = wait,
                                         const Condition condition = any);

  // object id, and name
  const rtems_id task_id_is() const { return id; }
  const rtems_name task_name_is() const { return name; }

private:
  // task name
  rtems_name name;

  // the rtems task id, object handle
  rtems_id id;

};

const rtems_status_code rtemsEvent::send(const rtems_id task,
                                         const rtems_event_set events)
{
  set_status_code(rtems_event_send(task, events));
  return last_status_code();
}

const rtems_status_code rtemsEvent::send(const rtemsTask& task,
                                         const rtems_event_set events)
{
  set_status_code(rtems_event_send(task.id_is(), events));
  return last_status_code();
}

const rtems_status_code rtemsEvent::send(const rtems_event_set events)
{
  set_status_code(rtems_event_send(id, events));
  return last_status_code();
}

const rtems_status_code rtemsEvent::receive(const rtems_event_set event_in,
                                            rtems_event_set& event_out,
                                            const rtems_interval micro_secs,
                                            const WaitMode wait,
                                            const Condition condition)
{
  rtems_interval usecs = micro_secs &&
    (micro_secs < rtems_configuration_get_microseconds_per_tick()) ?
    rtems_configuration_get_microseconds_per_tick() : micro_secs;
  set_status_code(rtems_event_receive(event_in,
                                      wait | condition,
                                      RTEMS_MICROSECONDS_TO_TICKS(usecs),
                                      &event_out));
  return last_status_code();
}

#endif  // _rtemsEvent_h_

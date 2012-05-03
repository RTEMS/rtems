/*
  ------------------------------------------------------------------------

  COPYRIGHT (c) 1997
  Objective Design Systems Ltd Pty (ODS)
  All rights reserved (R) Objective Design Systems Ltd Pty

  The license and distribution terms for this file may be found in the
  file LICENSE in this distribution or at
  http://www.rtems.com/license/LICENSE.

  ------------------------------------------------------------------------

  rtemsTask class.

  This class allows the user to create a RTEMS task, or to access and
  manage an already existing task.

  The first constructor with the task parameters creates a RTEMS task
  object. The destructor of this object also deletes the task
  object. The last status code should be checked after construction to
  see if the create completed successfully.

  The second constructor connects to an existing task object. The last
  status code should be checked after construction to see if the
  task existed.

  The third constructor is a copy constructor. Connects to an existing
  object which is in scope.

  The RTEMS id is set to self in the default construction.

  The creation of the task object can be defered until after
  construction. This allows for static task objects to be created.

  RTEMS should be initialised before static constructors run, how-ever
  threads will will not. You need to watch the start-order.

  A task object can change state from an owner of a task to being
  connected to a task.

  Task objects connected to another task do not receive notification
  when the task connected to changes state.

  The sleep methods operate on the current thread not the task
  reference by this object.

  Mode control is through the rtemsTaskMode class.

  The rtemsTask class reserved notepad register 31.

  ------------------------------------------------------------------------ */

#if !defined(_rtemsTask_h_)
#define _rtemsTask_h_

#include <rtems++/rtemsStatusCode.h>

/* ----
    rtemsTask
*/

class rtemsTask
  : public rtemsStatusCode
{
public:
  enum FloatingPoint { fpoff = RTEMS_NO_FLOATING_POINT,
                       fpon = RTEMS_FLOATING_POINT };
  enum Scope { local = RTEMS_LOCAL,
               global = RTEMS_GLOBAL };

  // only the first 4 characters of the name are taken

  // creates a task
  rtemsTask(const char* name,
            const rtems_task_priority initial_priority,
            const uint32_t stack_size,
            const rtems_mode preemption = RTEMS_NO_PREEMPT,
            const rtems_mode timeslice = RTEMS_NO_TIMESLICE,
            const rtems_mode asr = RTEMS_NO_ASR,
            const rtems_interrupt_level interrupt_level = 0,
            const FloatingPoint floating_point = fpoff,
            const Scope scope = local);

  // connects to a task
  rtemsTask(const char *name, const uint32_t node = RTEMS_SEARCH_ALL_NODES);

  // copy and default constructors
  rtemsTask(const rtemsTask& task);
  rtemsTask();

  // only the creator's destructor will delete the actual object
  virtual ~rtemsTask();

  // create or destroy (delete) the task
  virtual const rtems_status_code create(const char* name,
                                         const rtems_task_priority initial_priority,
                                         const uint32_t stack_size,
                                         const rtems_mode preemption = RTEMS_NO_PREEMPT,
                                         const rtems_mode timeslice = RTEMS_NO_TIMESLICE,
                                         const rtems_mode asr = RTEMS_NO_ASR,
                                         const rtems_interrupt_level interrupt_level = 0,
                                         const FloatingPoint floating_point = fpoff,
                                         const Scope scope = local);
  virtual const rtems_status_code destroy();

  // connect to an existing task object, will not be the owner
  const rtemsTask& operator=(const rtemsTask& task);
  virtual const rtems_status_code connect(const char *name,
                                          const uint32_t node = RTEMS_SEARCH_ALL_NODES);

  // run control
  virtual const rtems_status_code start(const rtems_task_argument argument);
  virtual const rtems_status_code restart(const rtems_task_argument argument);
  virtual const rtems_status_code suspend();
  virtual const rtems_status_code resume();

  // sleep control, the timeout is in micro-seconds
  virtual const rtems_status_code wake_after(const rtems_interval micro_secs);
  virtual const rtems_status_code wake_when(const rtems_time_of_day& tod);

  // priority control
  const rtems_status_code get_priority(rtems_task_priority& priority);
  const rtems_status_code set_priority(const rtems_task_priority priority);
  const rtems_status_code set_priority(const rtems_task_priority priority,
                                       rtems_task_priority& old_priority);

  // notepad control
  const rtems_status_code get_note(const uint32_t notepad,
                                   uint32_t& note);
  const rtems_status_code set_note(const uint32_t notepad,
                                   const uint32_t note);

  // object id, and name
  const rtems_id id_is() const { return id; }
  const rtems_name name_is() const { return name; }
  const char *name_string() const { return name_str; }

protected:

  // task entry point
  virtual void body(rtems_task_argument argument);

private:

  // make the object to point to RTEMS_SELF
  void make_self();

  // task name
  rtems_name name;
  char name_str[5];

  // owner, true if this object owns the task
  // will delete the task when it destructs
  bool owner;

  // the rtems id, object handle
  rtems_id id;

  // the argument for the task, this class uses the actual argument
  // passed to RTEMS
  rtems_task_argument argument;

  // common entry point to the task
  static rtems_task origin(rtems_task_argument argument);
};

#endif  // _rtemsTask_h_

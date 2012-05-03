/*
  ------------------------------------------------------------------------

  COPYRIGHT (c) 1997
  Objective Design Systems Ltd Pty (ODS)
  All rights reserved (R) Objective Design Systems Ltd Pty
  
  The license and distribution terms for this file may be found in the
  file LICENSE in this distribution or at
  http://www.rtems.com/license/LICENSE.

  ------------------------------------------------------------------------

  See header file.

  ------------------------------------------------------------------------
*/

#include <cstring>
#include <rtems++/rtemsTask.h>
// include to allow it to be compiled
#include <rtems++/rtemsTaskMode.h>

/* ----
    rtemsTask
*/

rtemsTask::rtemsTask(const char* tname,
                     const rtems_task_priority initial_priority,
                     const uint32_t stack_size,
                     const rtems_mode preemption,
                     const rtems_mode timeslice,
                     const rtems_mode asr,
                     const rtems_interrupt_level interrupt_level,
                     const FloatingPoint floating_point,
                     const Scope scope)
  : name(rtems_build_name('S', 'E', 'L', 'F')),
    owner(true),
    id(RTEMS_SELF),
    argument(0)
{
  strcpy(name_str, "SELF");
  create(tname,
         initial_priority,
         stack_size,
         preemption,
         timeslice,
         asr,
         interrupt_level,
         floating_point,
         scope);
}

rtemsTask::rtemsTask(const char *tname, uint32_t node)
  : name(rtems_build_name('S', 'E', 'L', 'F')),
    owner(false),
    id(RTEMS_SELF),
    argument(0)
{
  strcpy(name_str, "SELF");
  connect(tname, node);
}

rtemsTask::rtemsTask(const rtemsTask& task)
  : name(rtems_build_name('S', 'E', 'L', 'F')),
    owner(false),
    id(RTEMS_SELF),
    argument(0)
{
  name = task.name;
  strcpy(name_str, task.name_str);
  argument = task.argument;
  id = task.id;
}

rtemsTask::rtemsTask()
  : name(rtems_build_name('S', 'E', 'L', 'F')),
    owner(false),
    id(RTEMS_SELF),
    argument(0)
{
  strcpy(name_str, "SELF");
}

rtemsTask::~rtemsTask()
{
  destroy();
}

void rtemsTask::make_self()
{
  strcpy(name_str, "SELF");
  name = rtems_build_name('S', 'E', 'L', 'F');
  id = RTEMS_SELF;
  owner = false;
}

const rtems_status_code rtemsTask::create(const char* tname,
                                          const rtems_task_priority initial_priority,
                                          const uint32_t stack_size,
                                          const rtems_mode preemption,
                                          const rtems_mode timeslice,
                                          const rtems_mode asr,
                                          const rtems_interrupt_level interrupt_level,
                                          const FloatingPoint floating_point,
                                          const Scope scope)
{
  if (id)
    return set_status_code(RTEMS_ILLEGAL_ON_SELF);

  owner = true;

  strcpy(name_str, "    ");
  for (int c = 0; (c < 4) && (tname[c] != '\0'); c++)
    name_str[c] = tname[c];
  name = rtems_build_name(name_str[0],
                          name_str[1],
                          name_str[2],
                          name_str[3]);

  // protect the values that be set as the parameters are not enums
  set_status_code(rtems_task_create(name,
                                    initial_priority,
                                    stack_size,
                                    (preemption & RTEMS_PREEMPT_MASK) |
                                    (timeslice & RTEMS_TIMESLICE_MASK) |
                                    (asr & RTEMS_ASR_MASK) |
                                    (interrupt_level & RTEMS_INTERRUPT_MASK),
                                    floating_point | scope,
                                    &id));

  if (unsuccessful())
  {
    make_self();
  }
  
  return last_status_code();
}

const rtems_status_code rtemsTask::destroy()
{
  if (id && owner)
  {
    set_status_code(rtems_task_delete(id));
    make_self();
  }
  else
    set_status_code(RTEMS_NOT_OWNER_OF_RESOURCE);
  
  return last_status_code();
}
  
const rtemsTask& rtemsTask::operator=(const rtemsTask& task)
{
  if (!owner)
  {
    name = task.name;
    strcpy(name_str, task.name_str);
    argument = task.argument;
    id = task.id;
  }
  return *this;
}

const rtems_status_code rtemsTask::connect(const char *sname,
                                           const uint32_t node)
{
  if (id && owner)
    return set_status_code(RTEMS_UNSATISFIED);

  // change state to not owner
  owner = false;
  
  strcpy(name_str, "    ");
  for (int c = 0; (c < 4) && (sname[c] != '\0'); c++)
    name_str[c] = sname[c];
  name = rtems_build_name(name_str[0],
                          name_str[1],
                          name_str[2],
                          name_str[3]);
  
  set_status_code(rtems_task_ident(name, node, &id));

  if (unsuccessful())
  {
    make_self();
  }
  
  return last_status_code();
}

const rtems_status_code rtemsTask::start(const rtems_task_argument arg)
{
  if (owner)
  {
    argument = arg;
    // pass the this pointer as the argument
    set_status_code(rtems_task_start(id,
                                     origin,
                                     (rtems_task_argument) this));
  }
  else
    set_status_code(RTEMS_NOT_OWNER_OF_RESOURCE);
  return last_status_code();
}
      
const rtems_status_code rtemsTask::restart(const rtems_task_argument arg)
{
  if (owner)
  {
    argument = arg;
    set_status_code(rtems_task_restart(id, (rtems_task_argument) this));
  }
  else
    set_status_code(RTEMS_NOT_OWNER_OF_RESOURCE);
  
  return last_status_code();
}
  
const rtems_status_code rtemsTask::suspend()
{
  return set_status_code(rtems_task_suspend(id));
}

const rtems_status_code rtemsTask::resume()
{
  return set_status_code(rtems_task_resume(id));
}

const rtems_status_code rtemsTask::wake_after(const rtems_interval micro_secs)
{
  rtems_interval usecs =
    (micro_secs < rtems_configuration_get_microseconds_per_tick()) ?
    rtems_configuration_get_microseconds_per_tick() : micro_secs;
  return set_status_code(rtems_task_wake_after(RTEMS_MICROSECONDS_TO_TICKS(usecs)));
}

const rtems_status_code rtemsTask::wake_when(const rtems_time_of_day& tod)
{
  return set_status_code(rtems_task_wake_when((rtems_time_of_day*) &tod));
}

const rtems_status_code rtemsTask::get_priority(rtems_task_priority& priority)
{
  return set_status_code(rtems_task_set_priority(id,
                                                 RTEMS_CURRENT_PRIORITY,
                                                 &priority));
}

const rtems_status_code rtemsTask::set_priority(const rtems_task_priority priority)
{
  rtems_task_priority old_priority;
  return set_status_code(rtems_task_set_priority(id,
                                                 priority,
                                                 &old_priority));
}

const rtems_status_code rtemsTask::set_priority(const rtems_task_priority priority,
                                                rtems_task_priority& old_priority)
{
  return set_status_code(rtems_task_set_priority(id,
                                                 priority,
                                                 &old_priority));
}
  
const rtems_status_code rtemsTask::get_note(const uint32_t notepad,
                                            uint32_t& note)
{
  return set_status_code(rtems_task_get_note(id, notepad, &note));
}

const rtems_status_code rtemsTask::set_note(const uint32_t notepad,
                                            const uint32_t note)
{
  return set_status_code(rtems_task_set_note(id, notepad, note));
}

void rtemsTask::body(rtems_task_argument )
{
}

rtems_task rtemsTask::origin(rtems_task_argument argument)
{
  rtemsTask *task = (rtemsTask*) argument;
  task->body(task->argument);
}

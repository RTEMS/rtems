/*
  ------------------------------------------------------------------------

  COPYRIGHT (c) 1997
  Objective Design Systems Ltd Pty (ODS)
  All rights reserved (R) Objective Design Systems Ltd Pty

  The license and distribution terms for this file may be found in the
  file LICENSE in this distribution or at
  http://www.rtems.com/license/LICENSE.

  ------------------------------------------------------------------------

  rtemsSemaphore class.

  This class allows the user to create a RTEMS semaphore, or to use an
  already existing semaphore. The type of semaphore is decitated by
  the constructor used.

  The first constructor with the semaphore parameters creates a RTEMS
  semaphore object. The destructor of this object also deletes the
  semaphore object. The last status code should be checked after
  construction to see if the semaphore create was successfull.

  The second constructor connects to an existing. The last status code
  should be checked after construction to see if the semaphore
  existed.

  The third constructor is a copy constructor. Connects to an existing
  object which is in scope.

  ------------------------------------------------------------------------ */

#if !defined(_rtemsSemaphore_h_)
#define _rtemsSemaphore_h_

#include <rtems++/rtemsStatusCode.h>

/* ----
    rtemsSemaphore
*/

class rtemsSemaphore
  : public rtemsStatusCode
{
public:
  // attribute a semaphore can have
  enum WaitMode { wait_by_fifo = RTEMS_FIFO,
                  wait_by_priority = RTEMS_PRIORITY };
  enum Type { binary = RTEMS_BINARY_SEMAPHORE,
              counting = RTEMS_COUNTING_SEMAPHORE };
  enum Priority { no_priority_inherit = RTEMS_NO_INHERIT_PRIORITY,
                  inherit_priority = RTEMS_INHERIT_PRIORITY };
  enum Ceiling { no_priority_ceiling = RTEMS_NO_PRIORITY_CEILING,
                 priority_ceiling = RTEMS_PRIORITY_CEILING };
  enum Scope { local = RTEMS_LOCAL,
               global = RTEMS_GLOBAL };

  // only the first 4 characters of the name are taken,
  // the counter must be set to 1 for binary semaphores

  // create a semaphore object
  rtemsSemaphore(const char* name,
                 const Scope scope = local,
                 const uint32_t counter = 1,
                 const WaitMode wait_mode = wait_by_fifo,
                 const Type type = binary,
                 const Priority priority = no_priority_inherit,
                 const Ceiling ceiling = no_priority_ceiling,
                 const rtems_task_priority priority_ceiling = 0);

  // connect to an existing semaphore object by name
  rtemsSemaphore(const char *name, const uint32_t node);

  // attach this object to an other objects semaphore
  rtemsSemaphore(const rtemsSemaphore& semaphore);
  rtemsSemaphore();

  // only the creator's destructor will delete the actual object
  virtual ~rtemsSemaphore();

  // create or destroy (delete) a semaphore
  virtual const rtems_status_code create(const char* name,
                                         const Scope scope = local,
                                         const uint32_t counter = 1,
                                         const WaitMode wait_mode = wait_by_fifo,
                                         const Type type = binary,
                                         const Priority priority = no_priority_inherit,
                                         const Ceiling ceiling = no_priority_ceiling,
                                         const rtems_task_priority priority_ceiling = 0);
  virtual const rtems_status_code destroy();

  // connect to an existing semaphore object, will not be the owner
  const rtemsSemaphore& operator=(const rtemsSemaphore& semaphore);
  virtual const rtems_status_code connect(const char *name, uint32_t node);

  // obtain the semaphore, timeout is in micro-seconds
  inline const rtems_status_code obtain(bool wait = true,
                                        const uint32_t micro_secs = RTEMS_NO_TIMEOUT);

  // release the semaphore, blocks threads eligble
  inline const rtems_status_code release();

  // object id, and name
  const rtems_id id_is() const { return id; }
  const rtems_name name_is() const { return name; }
  const char *name_string() const { return name_str; }

private:

  // make the object reference no valid RTEMS object
  void make_invalid();

  // semaphore name
  rtems_name name;
  char name_str[5];

  // owner, true if this object owns the semaphore
  // will delete the semaphore when it destructs
  bool owner;

  // the rtems id, object handle
  rtems_id id;
};

const rtems_status_code rtemsSemaphore::obtain(const bool wait,
                                               const uint32_t micro_secs)
{
  rtems_interval usecs = micro_secs &&
    (micro_secs < rtems_configuration_get_microseconds_per_tick()) ?
    rtems_configuration_get_microseconds_per_tick() : micro_secs;

  return
    set_status_code(rtems_semaphore_obtain(id,
                                           wait ? RTEMS_WAIT : RTEMS_NO_WAIT,
                                           RTEMS_MICROSECONDS_TO_TICKS(usecs)));
}

const rtems_status_code rtemsSemaphore::release(void)
{
  return set_status_code(rtems_semaphore_release(id));
}

#endif  // _rtemsSemaphore_h_

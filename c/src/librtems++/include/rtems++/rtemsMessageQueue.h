/*
  ------------------------------------------------------------------------

  COPYRIGHT (c) 1997
  Objective Design Systems Ltd Pty (ODS)
  All rights reserved (R) Objective Design Systems Ltd Pty

  The license and distribution terms for this file may be found in the
  file LICENSE in this distribution or at
  http://www.rtems.com/license/LICENSE.

  ------------------------------------------------------------------------

  rtemsMessageQueue class.

  This class allows the user to create a RTEMS message queue, or to
  access and manage an already existing message queue.

  The first constructor with the message queue parameters creates a
  RTEMS message queue object. The destructor of this object also
  deletes the message queue object. The last status code should be
  checked after construction to see if the create completed
  successfully.

  The second constructor connects to an existing message queue
  object. The last status code should be checked after construction to
  see if the message queue existed.

  The third constructor is a copy constructor. Connects to an existing
  object which is in scope.

  The fourth constructor allows for the message queue to be created
  after construction, or to connect to a message queue later.

  ------------------------------------------------------------------------ */

#if !defined(_rtemsMessageQueue_h_)
#define _rtemsMessageQueue_h_

#include <rtems++/rtemsStatusCode.h>

/* ----
    rtemsMessageQueue
*/

class rtemsMessageQueue
  : public rtemsStatusCode
{
public:
  // attribute a message queue can have
  enum WaitMode { wait_by_fifo = RTEMS_FIFO,
                  wait_by_priority = RTEMS_PRIORITY };
  enum Scope { local = RTEMS_LOCAL,
               global = RTEMS_GLOBAL };

  // only the first 4 characters of the name are taken

  // creates a message queue
  rtemsMessageQueue(const char* name,
                    const uint32_t count,
                    const size_t max_message_size,
                    const WaitMode wait_mode = wait_by_fifo,
                    const Scope scope = local);

  // connects to a message queue
  rtemsMessageQueue(const char *name, const uint32_t node = RTEMS_SEARCH_ALL_NODES);

  // copy and default constructors
  rtemsMessageQueue(const rtemsMessageQueue& message_queue);
  rtemsMessageQueue();

  // only the creator's destructor will delete the actual object
  virtual ~rtemsMessageQueue();

  // create or destroy (delete) the message queue
  virtual const rtems_status_code create(const char* name,
                                         const uint32_t count,
                                         const size_t max_message_size,
                                         const WaitMode wait_mode = wait_by_fifo,
                                         const Scope scope = local);
  virtual const rtems_status_code destroy();

  // connect to an existing message queue object, will not be the owner
  const rtemsMessageQueue& operator=(const rtemsMessageQueue& message_queue);
  virtual const rtems_status_code connect(const char *name,
                                          const uint32_t node = RTEMS_SEARCH_ALL_NODES);

  // send a message of size from the buffer
  inline const rtems_status_code send(const void *buffer,
                                      const size_t size);
  inline const rtems_status_code urgent(const void *buffer,
                                        const size_t size);
  inline const rtems_status_code broadcast(const void *buffer,
                                           const size_t size,
                                           uint32_t& count);

  // receive a message of size, the timeout is in micro-secs
  inline const rtems_status_code receive(const void *buffer,
                                         size_t& size,
                                         rtems_interval micro_secs = RTEMS_NO_TIMEOUT,
                                         bool wait = true);

  // flush a message queue, returning the number of messages dropped
  inline const rtems_status_code flush(uint32_t& size);

  // object id, and name
  const rtems_id id_is() const { return id; }
  const rtems_name name_is() const { return name; }
  const char *name_string() const { return name_str; }

private:

  // make this object reference an invalid RTEMS object
  void make_invalid();

  // message queue name
  rtems_name name;
  char name_str[5];

  // owner, true if this object owns the message queue
  // will delete the message queue when it destructs
  bool owner;

  // the rtems id, object handle
  rtems_id id;
};

const rtems_status_code rtemsMessageQueue::send(const void *buffer,
                                                const size_t size)
{
  return set_status_code(rtems_message_queue_send(id, (void*) buffer, size));
}

const rtems_status_code rtemsMessageQueue::urgent(const void *buffer,
                                                  const size_t size)
{
  return set_status_code(rtems_message_queue_urgent(id, (void*) buffer, size));
}

const rtems_status_code rtemsMessageQueue::broadcast(const void *buffer,
                                                     const size_t size,
                                                     uint32_t& count)
{
  return set_status_code(rtems_message_queue_broadcast(id,
                                                       (void*) buffer,
                                                       size,
                                                       &count));
}

const rtems_status_code rtemsMessageQueue::receive(const void *buffer,
                                                   size_t& size,
                                                   rtems_interval micro_secs,
                                                   bool wait)
{
  rtems_interval usecs = micro_secs &&
    (micro_secs < rtems_configuration_get_microseconds_per_tick()) ?
    rtems_configuration_get_microseconds_per_tick() : micro_secs;
  return set_status_code(rtems_message_queue_receive(id,
                                                     (void*) buffer,
                                                     &size,
                                                     wait ? RTEMS_WAIT : RTEMS_NO_WAIT,
                                                     RTEMS_MICROSECONDS_TO_TICKS(usecs)));
}

const rtems_status_code rtemsMessageQueue::flush(uint32_t& count)
{
  return set_status_code(rtems_message_queue_flush(id, &count));
}

#endif  // _rtemsMessageQueue_h_

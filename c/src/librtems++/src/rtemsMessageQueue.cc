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
#include <rtems++/rtemsMessageQueue.h>

/* ----
   rtemsMessageQueue
*/

rtemsMessageQueue::rtemsMessageQueue(const char* mqname,
                                     const uint32_t count,
                                     const size_t max_message_size,
                                     const WaitMode wait_mode,
                                     const Scope scope)
  : name(0),
    owner(true),
    id(0)
{
  strcpy(name_str, "NOID");
  create(mqname, count, max_message_size, wait_mode, scope);
}

rtemsMessageQueue::rtemsMessageQueue(const char *mqname,
                                     const uint32_t node)
  : name(0),
    owner(false),
    id(0)
{
  strcpy(name_str, "NOID");
  connect(mqname, node);
}

rtemsMessageQueue::rtemsMessageQueue(const rtemsMessageQueue& message_queue)
  : name(0),
    owner(false),
    id(0)
{
  name = message_queue.name;
  strcpy(name_str, message_queue.name_str);
  id = message_queue.id;
}

rtemsMessageQueue::rtemsMessageQueue()
  : name(0),
    owner(false),
    id(0)
{
  strcpy(name_str, "NOID");
}

rtemsMessageQueue::~rtemsMessageQueue()
{
  destroy();
}

void rtemsMessageQueue::make_invalid()
{
  strcpy(name_str, "NOID");
  name = 0;
  id = 0;
  owner = false;
}

const rtems_status_code rtemsMessageQueue::create(const char* mqname,
                                                  const uint32_t count,
                                                  const size_t max_message_size,
                                                  const WaitMode wait_mode,
                                                  const Scope scope)
{
  if (id)
    return set_status_code(RTEMS_ILLEGAL_ON_SELF);

  owner = true;
  
  strcpy(name_str, "    ");
  for (int c = 0; (c < 4) && (mqname[c] != '\0'); c++)
    name_str[c] = mqname[c];
  name = rtems_build_name(name_str[0],
                          name_str[1],
                          name_str[2],
                          name_str[3]);
  
  set_status_code(rtems_message_queue_create(name,
                                             count,
                                             max_message_size,
                                             scope | wait_mode,
                                             &id));

  if (unsuccessful())
  {
    make_invalid();
  }
  
  return last_status_code();
}

const rtems_status_code rtemsMessageQueue::destroy()
{
  if (id && owner)
  {
    set_status_code(rtems_message_queue_delete(id));
    make_invalid();
  }
  else
    set_status_code(RTEMS_NOT_OWNER_OF_RESOURCE);
  
  return last_status_code();
}

const rtemsMessageQueue& rtemsMessageQueue::operator=(const rtemsMessageQueue& message_queue)
{
  if (!owner)
  {
    name = message_queue.name;
    strcpy(name_str, message_queue.name_str);
    id = message_queue.id;
  }

  return *this;
}
  
const rtems_status_code rtemsMessageQueue::connect(const char *mqname,
                                                   const uint32_t node)
{
  if (id && owner)
    return set_status_code(RTEMS_UNSATISFIED);

  // change state to not owner
  owner = false;
  
  strcpy(name_str, "    ");
  for (int c = 0; (c < 4) && (mqname[c] != '\0'); c++)
    name_str[c] = mqname[c];
  name = rtems_build_name(name_str[0],
                          name_str[1],
                          name_str[2],
                          name_str[3]);
  
  set_status_code(rtems_message_queue_ident(name, node, &id));

  if (unsuccessful())
  {
    make_invalid();
  }
  
  return last_status_code();
}

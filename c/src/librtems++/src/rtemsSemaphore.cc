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
#include <rtems++/rtemsSemaphore.h>

/* ----
    rtemsSemaphore
*/

rtemsSemaphore::rtemsSemaphore(const char* sname,
                               const Scope scope,
                               const uint32_t counter,
                               const WaitMode wait_mode,
                               const Type type,
                               const Priority priority,
                               const Ceiling ceiling,
                               const rtems_task_priority priority_ceiling)
  : name(0),
    owner(true),
    id(0)
{
  strcpy(name_str, "NOID");
  create(sname,
         scope,
         counter,
         wait_mode,
         type,
         priority,
         ceiling,
         priority_ceiling);  
}

rtemsSemaphore::rtemsSemaphore(const char *sname, const uint32_t node)
  : name(0),
    owner(false),
    id(0)
{
  strcpy(name_str, "NOID");
  connect(sname, node);
}

rtemsSemaphore::rtemsSemaphore(const rtemsSemaphore& semaphore)
  : name(0),
    owner(false),
    id(0)
{
  name = semaphore.name;
  strcpy(name_str, semaphore.name_str);
  id = semaphore.id;
}

rtemsSemaphore::rtemsSemaphore()
  : name(0),
    owner(false),
    id(0)
{
  strcpy(name_str, "NOID");
}

rtemsSemaphore::~rtemsSemaphore()
{
  destroy();
}

void rtemsSemaphore::make_invalid()
{
  strcpy(name_str, "NOID");
  name = 0;
  id = 0;
  owner = false;
}

const rtems_status_code rtemsSemaphore::create(const char* sname,
                                               const Scope scope,
                                               const uint32_t counter,
                                               const WaitMode wait_mode,
                                               const Type type,
                                               const Priority priority,
                                               const Ceiling ceiling,
                                               const rtems_task_priority priority_ceiling)
{
  if (id)
    return set_status_code(RTEMS_ILLEGAL_ON_SELF);

  owner = true;
  
  strcpy(name_str, "    ");
  for (int c = 0; (c < 4) && (sname[c] != '\0'); c++)
    name_str[c] = sname[c];
  name = rtems_build_name(name_str[0],
                          name_str[1],
                          name_str[2],
                          name_str[3]);
  
  set_status_code(rtems_semaphore_create(name,
                                         counter,
                                         scope | wait_mode | type | priority | ceiling,
                                         priority_ceiling,
                                         &id));

  if (unsuccessful())
  {
    make_invalid();
  }
  
  return last_status_code();
}

const rtems_status_code rtemsSemaphore::destroy()
{
  if (id && owner)
  {
    set_status_code(rtems_semaphore_delete(id));
    make_invalid();
  }
  else
    set_status_code(RTEMS_NOT_OWNER_OF_RESOURCE);
  
  return last_status_code();
}

const rtemsSemaphore& rtemsSemaphore::operator=(const rtemsSemaphore& semaphore)
{
  if (!owner)
  {
    name = semaphore.name;
    id = semaphore.id;
  }
  return *this;
}

const rtems_status_code rtemsSemaphore::connect(const char *sname,
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
  
  set_status_code(rtems_semaphore_ident(name, node, &id));

  if (unsuccessful())
  {
    make_invalid();
  }
  
  return last_status_code();
}

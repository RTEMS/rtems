/*
 *  RTEMS Object Helper -- Obtain Name of API
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/rtems/object.h>

#include <rtems/assoc.h>

rtems_assoc_t rtems_object_api_internal_assoc[] = {
  { "Thread",                  OBJECTS_INTERNAL_THREADS},
  { "Mutex",                   OBJECTS_INTERNAL_MUTEXES},
};

rtems_assoc_t rtems_object_api_classic_assoc[] = {
  { "Task",                    OBJECTS_RTEMS_TASKS},
  { "Timer",                   OBJECTS_RTEMS_TIMERS},
  { "Semaphore",               OBJECTS_RTEMS_SEMAPHORES},
  { "Message Queue",           OBJECTS_RTEMS_MESSAGE_QUEUES},
  { "Partition",               OBJECTS_RTEMS_PARTITIONS},
  { "Region",                  OBJECTS_RTEMS_REGIONS},
  { "Port",                    OBJECTS_RTEMS_PORTS},
  { "Period",                  OBJECTS_RTEMS_PERIODS},
  { "Extension",               OBJECTS_RTEMS_EXTENSIONS},
  { "Barrier",                 OBJECTS_RTEMS_BARRIERS},
};

#ifdef RTEMS_POSIX_API
rtems_assoc_t rtems_object_api_posix_assoc[] = {
  { "Thread",                  OBJECTS_POSIX_THREADS},
  { "Key",                     OBJECTS_POSIX_KEYS},
  { "Interrupt",               OBJECTS_POSIX_INTERRUPTS},
  { "Queued fd",               OBJECTS_POSIX_MESSAGE_QUEUE_FDS},
  { "Message Queue",           OBJECTS_POSIX_MESSAGE_QUEUES},
  { "Mutex",                   OBJECTS_POSIX_MUTEXES},
  { "Semaphore",               OBJECTS_POSIX_SEMAPHORES},
  { "Condition Variable",      OBJECTS_POSIX_CONDITION_VARIABLES},
  { "Timer",                   OBJECTS_POSIX_TIMERS},
  { "Barrier",                 OBJECTS_POSIX_BARRIERS},
  { "Spinlock",                OBJECTS_POSIX_SPINLOCKS},
  { "RWLock",                  OBJECTS_POSIX_RWLOCKS},
};
#endif

#ifdef RTEMS_ITRON_API
rtems_assoc_t rtems_object_api_itron_assoc[] = {
  { "Task",                    OBJECTS_ITRON_TASKS},
  { "Event Flag",              OBJECTS_ITRON_EVENTFLAGS},
  { "Mailbox",                 OBJECTS_ITRON_MAILBOXES},
  { "Message Buffer",          OBJECTS_ITRON_MESSAGE_BUFFERS},
  { "Port",                    OBJECTS_ITRON_PORTS},
  { "Semaphore",               OBJECTS_ITRON_SEMAPHORES},
  { "Variable Memory Pool",    OBJECTS_ITRON_VARIABLE_MEMORY_POOLS},
  { "Fixed Memory Pool",       OBJECTS_ITRON_FIXED_MEMORY_POOLS},
};
#endif

const char *rtems_object_get_api_class_name(
  uint32_t api, 
  uint32_t class
)
{
  const rtems_assoc_t *api_assoc;
  const rtems_assoc_t *class_assoc;
  
  switch (api) {
    case OBJECTS_INTERNAL_API:
      api_assoc = rtems_object_api_internal_assoc;
      break;
    case OBJECTS_CLASSIC_API:
      api_assoc = rtems_object_api_classic_assoc;
      break;
    case OBJECTS_POSIX_API:
      api_assoc = rtems_object_api_posix_assoc;
      break;
    case OBJECTS_ITRON_API:
      api_assoc = rtems_object_api_itron_assoc;
      break;
    default:
      return "BAD API";
  }
  class_assoc = rtems_assoc_ptr_by_local( api_assoc, class );
  if ( class_assoc )
    return class_assoc->name;
  return "BAD CLASS";
}

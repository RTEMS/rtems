/*
 * RTEMS Monitor task support
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/monitor.h>

#include <stdio.h>
#include <string.h>    /* memcpy() */

void
rtems_monitor_task_canonical(
    rtems_monitor_task_t  *canonical_task,
    void                  *thread_void
)
{
    Thread_Control       *rtems_thread = (Thread_Control *) thread_void;
    RTEMS_API_Control    *api;

    api = rtems_thread->API_Extensions[ THREAD_API_RTEMS ];
    
    canonical_task->entry = rtems_thread->Start.entry_point;
    canonical_task->argument = rtems_thread->Start.numeric_argument;
    canonical_task->stack = rtems_thread->Start.Initial_stack.area;
    canonical_task->stack_size = rtems_thread->Start.Initial_stack.size;
    canonical_task->priority = rtems_thread->current_priority;
    canonical_task->state = rtems_thread->current_state;
    canonical_task->wait_id = rtems_thread->Wait.id;
    canonical_task->events = api->pending_events;

/* XXX modes and attributes only exist in the RTEMS API .. */
/* XXX not directly in the core thread.. they will have to be derived */
/* XXX if they are important enough to include anymore.   */
    canonical_task->modes = 0; /* XXX FIX ME.... rtems_thread->current_modes; */
    canonical_task->attributes = 0 /* XXX FIX ME rtems_thread->API_Extensions[ THREAD_API_RTEMS ]->attribute_set */;
    (void) memcpy(canonical_task->notepad, api ->Notepads, sizeof(canonical_task->notepad));
/* XXX more to fix */
/*
    (void) memcpy(&canonical_task->wait_args, &rtems_thread->Wait.Extra, sizeof(canonical_task->wait_args));
*/
}


void
rtems_monitor_task_dump_header(
    boolean verbose
)
{
    printf("\
  ID       NAME   PRIO   STAT   MODES  EVENTS   WAITID  WAITARG  NOTES\n");
/*23456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
0         1         2         3         4         5         6         7       */
           
    rtems_monitor_separator();
}

/*
 */

void
rtems_monitor_task_dump(
    rtems_monitor_task_t *monitor_task,
    boolean  verbose
)
{
    int length = 0;

    length += rtems_monitor_dump_id(monitor_task->id);
    length += rtems_monitor_pad(11, length);
    length += rtems_monitor_dump_name(monitor_task->name);
    length += rtems_monitor_pad(18, length);
    length += rtems_monitor_dump_priority(monitor_task->priority);
    length += rtems_monitor_pad(24, length);
    length += rtems_monitor_dump_state(monitor_task->state);
    length += rtems_monitor_pad(31, length);
    length += rtems_monitor_dump_modes(monitor_task->modes);
    length += rtems_monitor_pad(39, length);
    length += rtems_monitor_dump_events(monitor_task->events);
    if (monitor_task->wait_id)
    {
        length += rtems_monitor_pad(47, length);
        length += rtems_monitor_dump_id(monitor_task->wait_id);
        length += rtems_monitor_pad(57, length);
        length += rtems_monitor_dump_hex(monitor_task->wait_args);
    }

    length += rtems_monitor_pad(65, length);
    length += rtems_monitor_dump_notepad(monitor_task->notepad);
    printf("\n");
}


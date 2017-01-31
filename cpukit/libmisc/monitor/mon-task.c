/*
 * RTEMS Monitor task support
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/monitor.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>

#include <stdio.h>
#include <string.h>    /* memcpy() */

static void
rtems_monitor_task_wait_info(
    rtems_monitor_task_t *canonical_task,
    Thread_Control       *rtems_thread
)
{
    Thread_queue_Context      queue_context;
    const Thread_queue_Queue *queue;

    _Thread_queue_Context_initialize( &queue_context );
    _Thread_Wait_acquire( rtems_thread, &queue_context );

    queue = rtems_thread->Wait.queue;

    if ( queue != NULL ) {
      _Thread_queue_Queue_get_name_and_id(
        queue,
        canonical_task->wait_name,
        sizeof(canonical_task->wait_name),
        &canonical_task->wait_id
      );
    } else {
      canonical_task->wait_id = 0;
      canonical_task->wait_name[0] = '\0';
    }

    _Thread_Wait_release( rtems_thread, &queue_context );
}

void
rtems_monitor_task_canonical(
    rtems_monitor_task_t  *canonical_task,
    const void            *thread_void
)
{
    Thread_Control    *rtems_thread;
    RTEMS_API_Control *api;

    rtems_thread =
      RTEMS_DECONST( Thread_Control *, (const Thread_Control *) thread_void );

    api = rtems_thread->API_Extensions[ THREAD_API_RTEMS ];

    _Thread_Get_name(
      rtems_thread,
      canonical_task->name_string,
      sizeof( canonical_task->name_string )
    );

    rtems_monitor_task_wait_info( canonical_task, rtems_thread );

    canonical_task->state = rtems_thread->current_state;
    canonical_task->entry = rtems_thread->Start.Entry;
    canonical_task->stack = rtems_thread->Start.Initial_stack.area;
    canonical_task->stack_size = rtems_thread->Start.Initial_stack.size;
    canonical_task->cpu = _Per_CPU_Get_index( _Thread_Get_CPU( rtems_thread ) );
    canonical_task->priority = _Thread_Get_priority( rtems_thread );
    canonical_task->events = api->Event.pending_events;
    /*
     * FIXME: make this optionally cpu_time_executed
     */
#if 0
    canonical_task->ticks = rtems_thread->cpu_time_executed;
#else
    canonical_task->ticks = 0;
#endif

/* XXX modes and attributes only exist in the RTEMS API .. */
/* XXX not directly in the core thread.. they will have to be derived */
/* XXX if they are important enough to include anymore.   */
    canonical_task->modes = 0; /* XXX FIX ME.... rtems_thread->current_modes; */
    canonical_task->attributes = 0 /* XXX FIX ME rtems_thread->API_Extensions[ THREAD_API_RTEMS ]->attribute_set */;
}


void
rtems_monitor_task_dump_header(
    bool verbose RTEMS_UNUSED
)
{
    fprintf(stdout,"\
ID         NAME       CPU PRI STATE  MODES    EVENTS WAITID   WAITQUEUE\n"); /*
0a010004   SHLL         0 100 READY  P:T:nA     NONE 00000000 00000000 [DFLT] */

    rtems_monitor_separator();
}

/*
 */

void
rtems_monitor_task_dump(
    rtems_monitor_task_t *monitor_task,
    bool                  verbose RTEMS_UNUSED
)
{
    int length = 0;

    length += rtems_monitor_dump_id(monitor_task->id);
    length += rtems_monitor_pad(11, length);
    length += fprintf(stdout, "%s", monitor_task->name_string);
    length += rtems_monitor_pad(21, length);
    length += rtems_monitor_dump_decimal(monitor_task->cpu);
    length += rtems_monitor_pad(26, length);
    length += rtems_monitor_dump_priority(monitor_task->priority);
    length += rtems_monitor_pad(30, length);
    length += rtems_monitor_dump_state(monitor_task->state);
    length += rtems_monitor_pad(37, length);
    length += rtems_monitor_dump_modes(monitor_task->modes);
    length += rtems_monitor_pad(44, length);
    length += rtems_monitor_dump_events(monitor_task->events);
    length += rtems_monitor_pad(53, length);
    length += rtems_monitor_dump_id(monitor_task->wait_id);
    length += rtems_monitor_pad(62, length);
    length += fprintf(stdout, "%s", monitor_task->wait_name);

    fprintf(stdout,"\n");
}

/*
 * Print misc stuff for the monitor dump routines
 * Each routine returns the number of characters it output.
 *
 * TODO:
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/monitor.h>
#include <rtems/assoc.h>

#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>

void
rtems_monitor_separator(void)
{
    fprintf(stdout,"------------------------------------------------------------------------------\n");
}

uint32_t
rtems_monitor_pad(
    uint32_t    destination_column,
    uint32_t    current_column
)
{
    int pad_length;

    if (destination_column <= current_column)
        pad_length = 1;
    else
        pad_length = destination_column - current_column;

    return fprintf(stdout,"%*s", pad_length, "");
}

int
rtems_monitor_dump_decimal(uint32_t   num)
{
    return fprintf(stdout,"%4" PRId32, num);
}

int
rtems_monitor_dump_addr(const void *addr)
{
    return fprintf(stdout,"%08" PRIxPTR, (intptr_t) addr);
}

int
rtems_monitor_dump_hex(uint32_t   num)
{
    return fprintf(stdout,"0x%" PRIx32, num);
}

static int
rtems_monitor_dump_assoc_bitfield(
    const rtems_assoc_t *ap,
    const char          *separator,
    uint32_t             value
  )
{
    uint32_t   b;
    uint32_t   length = 0;
    const char *name;

    for (b = 1; b; b <<= 1)
        if (b & value)
        {
            if (length)
                length += fprintf(stdout,"%s", separator);

            name = rtems_assoc_name_by_local(ap, b);

            if (name)
                length += fprintf(stdout,"%s", name);
            else
                length += fprintf(stdout,"0x%" PRIx32, b);
        }

    return length;
}

int
rtems_monitor_dump_id(rtems_id id)
{
    return fprintf(stdout,"%08" PRIx32, id);
}

int
rtems_monitor_dump_name(rtems_id id)
{
    char name_buffer[18] = "????";

    rtems_object_get_name( id, sizeof(name_buffer), name_buffer );

    return fprintf(stdout, "%s", name_buffer);
}

int
rtems_monitor_dump_priority(rtems_task_priority priority)
{
    return fprintf(stdout,"%3" PRId32, priority);
}

int
rtems_monitor_dump_state(States_Control state)
{
    char buf[16];

    rtems_assoc_thread_states_to_string(state, buf, sizeof(buf));
    return fprintf(stdout, "%s", buf);
}

static const rtems_assoc_t rtems_monitor_attribute_assoc[] = {
    { "GL",  RTEMS_GLOBAL, 0 },
    { "PR",  RTEMS_PRIORITY, 0 },
    { "FL",  RTEMS_FLOATING_POINT, 0 },
    { "BI",  RTEMS_BINARY_SEMAPHORE, 0 },
    { "SB",  RTEMS_SIMPLE_BINARY_SEMAPHORE, 0 },
    { "IN",  RTEMS_INHERIT_PRIORITY, 0 },
    { "CE",  RTEMS_PRIORITY_CEILING, 0 },
    { "AR",  RTEMS_BARRIER_AUTOMATIC_RELEASE, 0 },
    { "ST",  RTEMS_SYSTEM_TASK, 0 },
    { 0, 0, 0 },
};

int
rtems_monitor_dump_attributes(rtems_attribute attributes)
{
    int   length = 0;

    if (attributes == RTEMS_DEFAULT_ATTRIBUTES)  /* value is 0 */
        length += fprintf(stdout,"DEFAULT");

    length += rtems_monitor_dump_assoc_bitfield(rtems_monitor_attribute_assoc,
                                                ":",
                                                attributes);
    return length;
}

static const rtems_assoc_t rtems_monitor_modes_assoc[] = {
    { "nP",     RTEMS_NO_PREEMPT, 0 },
    { "T",      RTEMS_TIMESLICE, 0 },
    { "nA",     RTEMS_NO_ASR, 0 },
    { 0, 0, 0 },
};

int
rtems_monitor_dump_modes(rtems_mode modes)
{
    uint32_t   length = 0;

    if (modes == RTEMS_DEFAULT_MODES)  /* value is 0 */
        length += fprintf(stdout,"P:T:nA");

    length += rtems_monitor_dump_assoc_bitfield(rtems_monitor_modes_assoc,
                                                ":",
                                                modes);
    return length;
}

int
rtems_monitor_dump_events(rtems_event_set events)
{
    if (events == 0)
        return fprintf(stdout,"  NONE  ");

    return fprintf(stdout,"%08" PRIx32, events);
}

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
rtems_monitor_dump_addr(void *addr)
{
    return fprintf(stdout,"0x%p", addr);
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
#if defined(RTEMS_USE_16_BIT_OBJECT)
    return fprintf(stdout,"%08" PRIx16, id);
#else
    return fprintf(stdout,"%08" PRIx32, id);
#endif
}

int
rtems_monitor_dump_name(rtems_id id)
{
    char name_buffer[18] = "????";

    rtems_object_get_name( id, sizeof(name_buffer), name_buffer );

    return fprintf( stdout, name_buffer );
}

int
rtems_monitor_dump_priority(rtems_task_priority priority)
{
    return fprintf(stdout,"%3" PRId32, priority);
}


static const rtems_assoc_t rtems_monitor_state_assoc[] = {
    { "DORM",   STATES_DORMANT, 0 },
    { "SUSP",   STATES_SUSPENDED, 0 },
    { "TRANS",  STATES_TRANSIENT, 0 },
    { "DELAY",  STATES_DELAYING, 0 },
    { "Wtime",  STATES_WAITING_FOR_TIME, 0 },
    { "Wbuf",   STATES_WAITING_FOR_BUFFER, 0 },
    { "Wseg",   STATES_WAITING_FOR_SEGMENT, 0 },
    { "Wmsg" ,  STATES_WAITING_FOR_MESSAGE, 0 },
    { "Wevnt",  STATES_WAITING_FOR_EVENT, 0 },
    { "Wsem",   STATES_WAITING_FOR_SEMAPHORE, 0 },
    { "Wmutex", STATES_WAITING_FOR_MUTEX, 0 },
    { "Wcvar",  STATES_WAITING_FOR_CONDITION_VARIABLE, 0 },
    { "Wjatx",  STATES_WAITING_FOR_JOIN_AT_EXIT, 0 },
    { "Wrpc",   STATES_WAITING_FOR_RPC_REPLY, 0 },
    { "WRATE",  STATES_WAITING_FOR_PERIOD, 0 },
    { "Wsig",   STATES_WAITING_FOR_SIGNAL, 0 },
    { "Wbar",   STATES_WAITING_FOR_BARRIER, 0 },
    { "Wrwlk",  STATES_WAITING_FOR_RWLOCK, 0 },
    { "Wisig",  STATES_INTERRUPTIBLE_BY_SIGNAL, 0 },
    { 0, 0, 0 },
};

int
rtems_monitor_dump_state(States_Control state)
{
    int   length = 0;

    if (state == STATES_READY)  /* assoc doesn't deal with this as it is 0 */
        length += fprintf(stdout,"READY");

    length += rtems_monitor_dump_assoc_bitfield(rtems_monitor_state_assoc,
                                                ":",
                                                state);
    return length;
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

static const rtems_assoc_t rtems_monitor_events_assoc[] = {
    { "0",   RTEMS_EVENT_0, 0 },
    { "1",   RTEMS_EVENT_1, 0 },
    { "2",   RTEMS_EVENT_2, 0 },
    { "3",   RTEMS_EVENT_3, 0 },
    { "4",   RTEMS_EVENT_4, 0 },
    { "5",   RTEMS_EVENT_5, 0 },
    { "6",   RTEMS_EVENT_6, 0 },
    { "7",   RTEMS_EVENT_7, 0 },
    { "8",   RTEMS_EVENT_8, 0 },
    { "9",   RTEMS_EVENT_9, 0 },
    { "10",  RTEMS_EVENT_10, 0 },
    { "11",  RTEMS_EVENT_11, 0 },
    { "12",  RTEMS_EVENT_12, 0 },
    { "13",  RTEMS_EVENT_13, 0 },
    { "14",  RTEMS_EVENT_14, 0 },
    { "15",  RTEMS_EVENT_15, 0 },
    { "16",  RTEMS_EVENT_16, 0 },
    { "17",  RTEMS_EVENT_17, 0 },
    { "18",  RTEMS_EVENT_18, 0 },
    { "19",  RTEMS_EVENT_19, 0 },
    { "20",  RTEMS_EVENT_20, 0 },
    { "21",  RTEMS_EVENT_21, 0 },
    { "22",  RTEMS_EVENT_22, 0 },
    { "23",  RTEMS_EVENT_23, 0 },
    { "24",  RTEMS_EVENT_24, 0 },
    { "25",  RTEMS_EVENT_25, 0 },
    { "26",  RTEMS_EVENT_26, 0 },
    { "27",  RTEMS_EVENT_27, 0 },
    { "28",  RTEMS_EVENT_28, 0 },
    { "29",  RTEMS_EVENT_29, 0 },
    { "30",  RTEMS_EVENT_30, 0 },
    { "31",  RTEMS_EVENT_31, 0 },
    { 0, 0, 0 },
};

int
rtems_monitor_dump_events(rtems_event_set events)
{
    if (events == EVENT_SETS_NONE_PENDING)  /* value is 0 */
        return fprintf(stdout,"  NONE  ");

    return fprintf(stdout,"%08" PRIx32, events);
}

int
rtems_monitor_dump_notepad(uint32_t   *notepad)
{
    int   length = 0;
    int i;

    for (i=0; i < RTEMS_NUMBER_NOTEPADS; i++)
        if (notepad[i])
            length += fprintf(stdout,"%d: 0x%" PRIx32, i, notepad[i]);

    return length;
}

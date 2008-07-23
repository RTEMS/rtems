/*
 * Print misc stuff for the monitor dump routines
 * Each routine returns the number of characters it output.
 *
 * TODO:
 *
 *  $Id$
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
rtems_monitor_dump_hex(uint32_t   num)
{
    return fprintf(stdout,"0x%" PRIx32, num);
}

int
rtems_monitor_dump_assoc_bitfield(
    rtems_assoc_t *ap,
    char          *separator,
    uint32_t       value
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
    char name_buffer[18];

    rtems_object_get_name( id, sizeof(name_buffer), name_buffer );  

    return fprintf( stdout, name_buffer );
}

int
rtems_monitor_dump_priority(rtems_task_priority priority)
{
    return fprintf(stdout,"%3" PRId32, priority);
}


rtems_assoc_t rtems_monitor_state_assoc[] = {
    { "DORM",   STATES_DORMANT },
    { "SUSP",   STATES_SUSPENDED },
    { "TRANS",  STATES_TRANSIENT },
    { "DELAY",  STATES_DELAYING },
    { "Wtime",  STATES_WAITING_FOR_TIME },
    { "Wbuf",   STATES_WAITING_FOR_BUFFER },
    { "Wseg",   STATES_WAITING_FOR_SEGMENT },
    { "Wmsg" ,  STATES_WAITING_FOR_MESSAGE },
    { "Wevnt",  STATES_WAITING_FOR_EVENT },
    { "Wsem",   STATES_WAITING_FOR_SEMAPHORE },
    { "Wmutex", STATES_WAITING_FOR_MUTEX },
    { "Wcvar",  STATES_WAITING_FOR_CONDITION_VARIABLE },
    { "Wjatx",  STATES_WAITING_FOR_JOIN_AT_EXIT },
    { "Wrpc",   STATES_WAITING_FOR_RPC_REPLY },
    { "WRATE",  STATES_WAITING_FOR_PERIOD },
    { "Wsig",   STATES_WAITING_FOR_SIGNAL },
    { "Wbar",   STATES_WAITING_FOR_BARRIER },
    { "Wrwlk",  STATES_WAITING_FOR_RWLOCK },
    { "Wisig",  STATES_INTERRUPTIBLE_BY_SIGNAL },
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

rtems_assoc_t rtems_monitor_attribute_assoc[] = {
    { "GL",  RTEMS_GLOBAL },
    { "PR",  RTEMS_PRIORITY },
    { "FL",  RTEMS_FLOATING_POINT },
    { "BI",  RTEMS_BINARY_SEMAPHORE },
    { "SB",  RTEMS_SIMPLE_BINARY_SEMAPHORE },
    { "IN",  RTEMS_INHERIT_PRIORITY },
    { "CE",  RTEMS_PRIORITY_CEILING },
    { "AR",  RTEMS_BARRIER_AUTOMATIC_RELEASE },
    { "ST",  RTEMS_SYSTEM_TASK },
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

rtems_assoc_t rtems_monitor_modes_assoc[] = {
    { "nP",     RTEMS_NO_PREEMPT },
    { "T",      RTEMS_TIMESLICE },
    { "nA",     RTEMS_NO_ASR },
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

rtems_assoc_t rtems_monitor_events_assoc[] = {
    { "0",   RTEMS_EVENT_0 },
    { "1",   RTEMS_EVENT_1 },
    { "2",   RTEMS_EVENT_2 },
    { "3",   RTEMS_EVENT_3 },
    { "4",   RTEMS_EVENT_4 },
    { "5",   RTEMS_EVENT_5 },
    { "6",   RTEMS_EVENT_6 },
    { "7",   RTEMS_EVENT_7 },
    { "8",   RTEMS_EVENT_8 },
    { "9",   RTEMS_EVENT_9 },
    { "10",  RTEMS_EVENT_10 },
    { "11",  RTEMS_EVENT_11 },
    { "12",  RTEMS_EVENT_12 },
    { "13",  RTEMS_EVENT_13 },
    { "14",  RTEMS_EVENT_14 },
    { "15",  RTEMS_EVENT_15 },
    { "16",  RTEMS_EVENT_16 },
    { "17",  RTEMS_EVENT_17 },
    { "18",  RTEMS_EVENT_18 },
    { "19",  RTEMS_EVENT_19 },
    { "20",  RTEMS_EVENT_20 },
    { "21",  RTEMS_EVENT_21 },
    { "22",  RTEMS_EVENT_22 },
    { "23",  RTEMS_EVENT_23 },
    { "24",  RTEMS_EVENT_24 },
    { "25",  RTEMS_EVENT_25 },
    { "26",  RTEMS_EVENT_26 },
    { "27",  RTEMS_EVENT_27 },
    { "28",  RTEMS_EVENT_28 },
    { "29",  RTEMS_EVENT_29 },
    { "30",  RTEMS_EVENT_30 },
    { "31",  RTEMS_EVENT_31 },
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

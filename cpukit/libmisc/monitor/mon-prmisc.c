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

void
rtems_monitor_separator(void)
{
    printf("------------------------------------------------------------------------------\n");
}

unsigned32
rtems_monitor_pad(
    unsigned32  destination_column,
    unsigned32  current_column
)
{
    int pad_length;
    
    if (destination_column <= current_column)
        pad_length = 1;
    else
        pad_length = destination_column - current_column;

    return printf("%*s", pad_length, "");
}

unsigned32
rtems_monitor_dump_char(rtems_unsigned8 ch)
{
    if (isprint(ch))
        return printf("%c", ch);
    else
        return printf("%02x", ch);
}

unsigned32
rtems_monitor_dump_decimal(unsigned32 num)
{
    return printf("%4d", num);
}

unsigned32
rtems_monitor_dump_hex(unsigned32 num)
{
    return printf("0x%x", num);
}

unsigned32
rtems_monitor_dump_assoc_bitfield(
    rtems_assoc_t *ap,
    char          *separator,
    unsigned32     value
  )
{  
    unsigned32 b;
    unsigned32 length = 0;
    const char *name;

    for (b = 1; b; b <<= 1)
        if (b & value)
        {
            if (length)
                length += printf("%s", separator);

            name = rtems_assoc_name_by_local(ap, b);
            
            if (name)
                length += printf("%s", name);
            else
                length += printf("0x%x", b);
        }
        
    return length;
}

unsigned32
rtems_monitor_dump_id(rtems_id id)
{
    return printf("%08x", id);
}

unsigned32
rtems_monitor_dump_name(rtems_name name)
{
    unsigned32 i;
    unsigned32 length = 0;
    union {
        unsigned32 ui;
        char       c[4];
    } u;
    
    u.ui = (rtems_unsigned32) name;
    
#if (CPU_BIG_ENDIAN == TRUE)
    for (i=0; i<sizeof(u.c); i++)
        length += rtems_monitor_dump_char(u.c[i]);
#else
    for (i=0; i<sizeof(u.c); i++)
        length += rtems_monitor_dump_char(u.c[sizeof(u.c)-1-i]);
#endif
    return length;
}

unsigned32
rtems_monitor_dump_priority(rtems_task_priority priority)
{
    return printf("%3d", priority);
}


rtems_assoc_t rtems_monitor_state_assoc[] = {
    { "DORM",   STATES_DORMANT },
    { "SUSP",   STATES_SUSPENDED },
    { "TRANS",  STATES_TRANSIENT },
    { "DELAY",  STATES_DELAYING },
    { "Wbuf",   STATES_WAITING_FOR_BUFFER },
    { "Wseg",   STATES_WAITING_FOR_SEGMENT },
    { "Wmsg" ,  STATES_WAITING_FOR_MESSAGE },
    { "Wevnt",  STATES_WAITING_FOR_EVENT },
    { "Wsem",   STATES_WAITING_FOR_SEMAPHORE },
    { "Wtime",  STATES_WAITING_FOR_TIME },
    { "Wrpc",   STATES_WAITING_FOR_RPC_REPLY },
    { "Wmutex", STATES_WAITING_FOR_MUTEX },
    { "Wcvar",  STATES_WAITING_FOR_CONDITION_VARIABLE },
    { "Wjatx",  STATES_WAITING_FOR_JOIN_AT_EXIT },
    { "Wsig",   STATES_WAITING_FOR_SIGNAL },
    { "WRATE",  STATES_WAITING_FOR_PERIOD },
    { "Wisig",  STATES_INTERRUPTIBLE_BY_SIGNAL },
    { 0, 0, 0 },
};

unsigned32
rtems_monitor_dump_state(States_Control state)
{
    unsigned32 length = 0;

    if (state == STATES_READY)  /* assoc doesn't deal with this as it is 0 */
        length += printf("READY");
    
    length += rtems_monitor_dump_assoc_bitfield(rtems_monitor_state_assoc,
                                                ":",
                                                state);
    return length;
}

rtems_assoc_t rtems_monitor_attribute_assoc[] = {
    { "FL",  RTEMS_FLOATING_POINT },
    { "GL",  RTEMS_GLOBAL },
    { "PR",  RTEMS_PRIORITY },
    { "BI",  RTEMS_BINARY_SEMAPHORE },
    { "IN",  RTEMS_INHERIT_PRIORITY },
    { 0, 0, 0 },
};

unsigned32
rtems_monitor_dump_attributes(rtems_attribute attributes)
{
    unsigned32 length = 0;

    if (attributes == RTEMS_DEFAULT_ATTRIBUTES)  /* value is 0 */
        length += printf("DEFAULT");
    
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

unsigned32
rtems_monitor_dump_modes(rtems_mode modes)
{
    unsigned32 length = 0;

    if (modes == RTEMS_DEFAULT_MODES)  /* value is 0 */
        length += printf("P:T:nA");
    
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

unsigned32
rtems_monitor_dump_events(rtems_event_set events)
{
    unsigned32 length = 0;

    if (events == EVENT_SETS_NONE_PENDING)  /* value is 0 */
        length += printf("NONE");
    
    length += rtems_monitor_dump_assoc_bitfield(rtems_monitor_events_assoc,
                                                ":",
                                                events);
    return length;
}

unsigned32
rtems_monitor_dump_notepad(unsigned32 *notepad)
{
    unsigned32 length = 0;
    int i;

    for (i=0; i < RTEMS_NUMBER_NOTEPADS; i++)
        if (notepad[i])
            length += printf("%d: 0x%x ", i, notepad[i]);

    return length;
}

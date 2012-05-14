#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/monitor.h>

#include <stdio.h>

void
rtems_monitor_queue_canonical(
    rtems_monitor_queue_t *canonical_queue,
    void                  *queue_void
)
{
    Message_queue_Control *rtems_queue = (Message_queue_Control *) queue_void;

    canonical_queue->attributes = rtems_queue->attribute_set;
    canonical_queue->maximum_message_size = rtems_queue->message_queue.maximum_message_size;
    canonical_queue->maximum_pending_messages = rtems_queue->message_queue.maximum_pending_messages;
    canonical_queue->number_of_pending_messages = rtems_queue->message_queue.number_of_pending_messages;
}

void
rtems_monitor_queue_dump_header(
    bool verbose __attribute__((unused))
)
{
    fprintf(stdout,"\
  ID       NAME   ATTRIBUTES   PEND   MAXPEND  MAXSIZE\n");
/*23456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
0         1         2         3         4         5         6         7       */
    rtems_monitor_separator();
}


/*
 * Dump out the "next" queue indicated by 'id'.
 * Returns next one to check.
 * Returns RTEMS_OBJECT_ID_FINAL when all done
 */

void
rtems_monitor_queue_dump(
    rtems_monitor_queue_t *monitor_queue,
    bool  verbose __attribute__((unused))
)
{
    uint32_t              length = 0;

    length += rtems_monitor_dump_id(monitor_queue->id);
    length += rtems_monitor_pad(11, length);
    length += rtems_monitor_dump_name(monitor_queue->id);
    length += rtems_monitor_pad(19, length);
    length += rtems_monitor_dump_attributes(monitor_queue->attributes);
    length += rtems_monitor_pad(31, length);
    length += rtems_monitor_dump_decimal(monitor_queue->number_of_pending_messages);
    length += rtems_monitor_pad(39, length);
    length += rtems_monitor_dump_decimal(monitor_queue->maximum_pending_messages);
    length += rtems_monitor_pad(48, length);
    length += rtems_monitor_dump_decimal(monitor_queue->maximum_message_size);

    fprintf(stdout,"\n");
}

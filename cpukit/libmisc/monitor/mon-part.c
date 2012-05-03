/*
 * RTEMS Monitor partition support
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include "monitor.h"
#include <rtems/rtems/attr.inl>
#include <stdio.h>
#include <string.h>    /* memcpy() */

void
rtems_monitor_part_canonical(
    rtems_monitor_part_t  *canonical_part,
    void                  *part_void
)
{
    Partition_Control *rtems_part = (Partition_Control *) part_void;

    canonical_part->attribute = rtems_part->attribute_set;
    canonical_part->start_addr = rtems_part->starting_address;
    canonical_part->length = rtems_part->length;
    canonical_part->buf_size = rtems_part->buffer_size;
    canonical_part->used_blocks = rtems_part->number_of_used_blocks;
}


void
rtems_monitor_part_dump_header(
    bool verbose __attribute__((unused))
)
{
    printf("\
  ID       NAME   ATTR        STARTADDR LENGTH    BUF_SIZE  USED_BLOCKS\n");
/*23456789 123456789 123456789 123456789 123456789 123456789 123456789 1234
          1         2         3         4         5         6         7    */

    rtems_monitor_separator();
}

/*
 */

void
rtems_monitor_part_dump(
    rtems_monitor_part_t *monitor_part,
    bool  verbose __attribute__((unused))
)
{
    int length = 0;

    length += rtems_monitor_dump_id(monitor_part->id);
    length += rtems_monitor_pad(11, length);
    length += rtems_monitor_dump_name(monitor_part->id);
    length += rtems_monitor_pad(18, length);
    length += rtems_monitor_dump_attributes(monitor_part->attribute);
    length += rtems_monitor_pad(30, length);
    length += rtems_monitor_dump_addr(monitor_part->start_addr);
    length += rtems_monitor_pad(40, length);
    length += rtems_monitor_dump_hex(monitor_part->length);
    length += rtems_monitor_pad(50, length);
    length += rtems_monitor_dump_hex(monitor_part->buf_size);
    length += rtems_monitor_pad(60, length);
    length += rtems_monitor_dump_hex(monitor_part->used_blocks);
    printf("\n");
}


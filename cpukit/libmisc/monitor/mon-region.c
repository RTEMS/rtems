/*
 * RTEMS Monitor region support
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
rtems_monitor_region_canonical(
    rtems_monitor_region_t  *canonical_region,
    void                  *region_void
)
{
    Region_Control *rtems_region = (Region_Control *) region_void;

    canonical_region->attribute = rtems_region->attribute_set;
    canonical_region->start_addr = rtems_region->starting_address;
    canonical_region->length = rtems_region->length;
    canonical_region->page_size = rtems_region->page_size;
    canonical_region->max_seg_size = rtems_region->maximum_segment_size;
    canonical_region->used_blocks = rtems_region->number_of_used_blocks;
}


void
rtems_monitor_region_dump_header(
    bool verbose __attribute__((unused))
)
{
    printf("\
  ID       NAME   ATTR        STARTADDR LENGTH    PAGE_SIZE USED_BLOCKS\n");
/*23456789 123456789 123456789 123456789 123456789 123456789 123456789 1234
          1         2         3         4         5         6         7    */

    rtems_monitor_separator();
}

/*
 */

void
rtems_monitor_region_dump(
    rtems_monitor_region_t *monitor_region,
    bool  verbose __attribute__((unused))
)
{
    int length = 0;

    length += rtems_monitor_dump_id(monitor_region->id);
    length += rtems_monitor_pad(11, length);
    length += rtems_monitor_dump_name(monitor_region->id);
    length += rtems_monitor_pad(18, length);
    length += rtems_monitor_dump_attributes(monitor_region->attribute);
    length += rtems_monitor_pad(30, length);
    length += rtems_monitor_dump_addr(monitor_region->start_addr);
    length += rtems_monitor_pad(40, length);
    length += rtems_monitor_dump_hex(monitor_region->length);
    length += rtems_monitor_pad(50, length);
    length += rtems_monitor_dump_hex(monitor_region->page_size);
    length += rtems_monitor_pad(60, length);
    length += rtems_monitor_dump_hex(monitor_region->used_blocks);
    printf("\n");
}


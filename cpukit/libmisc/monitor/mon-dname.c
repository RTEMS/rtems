/*
 * RTEMS monitor driver names support.
 *
 * There are 2 "driver" things the monitor knows about.
 *
 *    1. Regular RTEMS drivers.
 *         This is a table indexed by major device number and
 *         containing driver entry points only.
 *
 *    2. Driver name table.
 *         A separate table of names for drivers.
 *         The table converts driver names to a major number
 *         as index into the driver table and a minor number
 *         for an argument to driver.
 *
 *  Drivers are displayed with 'driver' command.
 *  Names are displayed with 'dname' command.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#include <rtems.h>

#include <rtems/monitor.h>

#include <stdio.h>
#include <stdlib.h>             /* strtoul() */
#include <string.h>		/* strncpy() */

#define DATACOL 15
#define CONTCOL DATACOL		/* continued col */

void
rtems_monitor_dname_canonical(
    rtems_monitor_dname_t  *canonical_dname,
    void                  *dname_void
)
{
    rtems_driver_name_t    *np = (rtems_driver_name_t *) dname_void;

    (void) strncpy(canonical_dname->name_string, np->device_name, sizeof(canonical_dname->name_string));
    canonical_dname->major = np->major;
    canonical_dname->minor = np->minor;
}    

void *
rtems_monitor_dname_next(
    void                   *object_information,
    rtems_monitor_dname_t  *canonical_dname,
    rtems_id               *next_id
)
{
    rtems_unsigned32      n = rtems_get_index(*next_id);
    rtems_driver_name_t  *table = _IO_Driver_name_table;
    rtems_driver_name_t  *np = 0;

/* XXX should we be using _IO_Number_of_devices */
    for (np = table + n ; n<_IO_Number_of_devices; n++, np++)
        if (np->device_name)
            goto done;
    
    *next_id = RTEMS_OBJECT_ID_FINAL;
    return 0;

done:
    _Thread_Disable_dispatch();

    /*
     * dummy up a fake id and name for this item
     */

    canonical_dname->id = n;
    canonical_dname->name = rtems_build_name('-', '-', '-', '-');

    *next_id += 1;
    return np;
}    

void
rtems_monitor_dname_dump_header(
    boolean verbose
)
{
    printf("\
  Major:Minor   Name\n");
/*23456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
0         1         2         3         4         5         6         7       */
    rtems_monitor_separator();
}

void
rtems_monitor_dname_dump(
    rtems_monitor_dname_t *monitor_dname,
    boolean                 verbose
)
{
    unsigned32             length = 0;

    length += rtems_monitor_pad(6, length);
    length += rtems_monitor_dump_hex(monitor_dname->major);
    length += printf(":");
    length += rtems_monitor_dump_hex(monitor_dname->minor);

    length += rtems_monitor_pad(16, length);
    length += printf("%.*s",
                     (int) sizeof(monitor_dname->name_string),
                     (char *) monitor_dname->name_string);

    length += printf("\n");
    length = 0;
}

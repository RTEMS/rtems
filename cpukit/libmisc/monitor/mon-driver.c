/*
 * RTEMS monitor IO (device drivers) support
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
 *  Names are displayed with 'name' command.
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

#define DATACOL 15
#define CONTCOL DATACOL		/* continued col */


void
rtems_monitor_driver_canonical(
    rtems_monitor_driver_t *canonical_driver,
    void                   *driver_void
)
{
    rtems_driver_address_table *d = (rtems_driver_address_table *) driver_void;

    rtems_monitor_symbol_canonical_by_value(&canonical_driver->initialization,
                                            (void *) d->initialization_entry);

    rtems_monitor_symbol_canonical_by_value(&canonical_driver->open,
                                            (void *) d->open_entry);
    rtems_monitor_symbol_canonical_by_value(&canonical_driver->close,
                                            (void *) d->close_entry);
    rtems_monitor_symbol_canonical_by_value(&canonical_driver->read,
                                            (void *) d->read_entry);
    rtems_monitor_symbol_canonical_by_value(&canonical_driver->write,
                                            (void *) d->write_entry);
    rtems_monitor_symbol_canonical_by_value(&canonical_driver->control,
                                            (void *) d->control_entry);
}


void *
rtems_monitor_driver_next(
    void                  *object_info,
    rtems_monitor_driver_t *canonical_driver,
    rtems_id              *next_id
)
{
    rtems_configuration_table *c = _Configuration_Table;
    rtems_unsigned32 n = rtems_get_index(*next_id);

    if (n >= c->number_of_device_drivers)
        goto failed;
    
    _Thread_Disable_dispatch();

    /*
     * dummy up a fake id and name for this item
     */

    canonical_driver->id = n;
    canonical_driver->name = rtems_build_name('-', '-', '-', '-');

    *next_id += 1;
    return (void *) (c->Device_driver_table + n);

failed:
    *next_id = RTEMS_OBJECT_ID_FINAL;
    return 0;
}


void
rtems_monitor_driver_dump_header(
    boolean verbose
)
{
    printf("\
  Major      Entry points\n");
/*23456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
0         1         2         3         4         5         6         7       */
    rtems_monitor_separator();
}

void
rtems_monitor_driver_dump(
    rtems_monitor_driver_t *monitor_driver,
    boolean                 verbose
)
{
    unsigned32          length = 0;

    length += printf("  %d", monitor_driver->id);

    length += rtems_monitor_pad(13, length);
    length += printf("init: ");
    length += rtems_monitor_symbol_dump(&monitor_driver->initialization, verbose);
    length += printf(";  control: ");
    length += rtems_monitor_symbol_dump(&monitor_driver->control, verbose);
    length += printf("\n");
    length = 0;

    length += rtems_monitor_pad(13, length);

    length += printf("open: ");
    length += rtems_monitor_symbol_dump(&monitor_driver->open, verbose);
    length += printf(";  close: ");
    length += rtems_monitor_symbol_dump(&monitor_driver->close, verbose);
    length += printf("\n");
    length = 0;

    length += rtems_monitor_pad(13, length);

    length += printf("read: ");
    length += rtems_monitor_symbol_dump(&monitor_driver->read, verbose);
    length += printf(";  write: ");
    length += rtems_monitor_symbol_dump(&monitor_driver->write, verbose);
    length += printf("\n");
    length = 0;
}

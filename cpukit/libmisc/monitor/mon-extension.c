/*
 *	@(#)extension.c	1.3 - 95/07/31
 *	
 *
 * RTEMS Monitor extension support
 *
 *  $Id$
 */

#include <rtems.h>
#include "monitor.h"

#include <stdio.h>

void
rtems_monitor_extension_canonical(
    rtems_monitor_extension_t *canonical_extension,
    void                  *extension_void
)
{
    Extension_Control     *rtems_extension = (Extension_Control *) extension_void;
    rtems_extensions_table *e = &rtems_extension->Extension.Callouts;

    rtems_monitor_symbol_canonical_by_value(&canonical_extension->create,
                                            e->thread_create);

    rtems_monitor_symbol_canonical_by_value(&canonical_extension->start,
                                            e->thread_start);
    rtems_monitor_symbol_canonical_by_value(&canonical_extension->restart,
                                            e->thread_restart);
    rtems_monitor_symbol_canonical_by_value(&canonical_extension->delete,
                                            e->thread_delete);
    rtems_monitor_symbol_canonical_by_value(&canonical_extension->tswitch,
                                            e->thread_switch);
    rtems_monitor_symbol_canonical_by_value(&canonical_extension->begin,
                                            e->thread_begin);
    rtems_monitor_symbol_canonical_by_value(&canonical_extension->exitted,
                                            e->thread_exitted);
    rtems_monitor_symbol_canonical_by_value(&canonical_extension->fatal,
                                            e->fatal);
}

void
rtems_monitor_extension_dump_header(
    boolean verbose
)
{
    printf("\
  ID       NAME\n");
/*23456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
0         1         2         3         4         5         6         7       */
           
    rtems_monitor_separator();
}


/*
 * Dump out the canonical form
 */

void
rtems_monitor_extension_dump(
    rtems_monitor_extension_t *monitor_extension,
    boolean  verbose
)
{
    unsigned32           length = 0;

    length += rtems_monitor_dump_id(monitor_extension->id);
    length += rtems_monitor_pad(11, length);
    length += rtems_monitor_dump_name(monitor_extension->name);

    length += rtems_monitor_pad(18, length);
    length += printf("create: ");
    length += rtems_monitor_symbol_dump(&monitor_extension->create, verbose);
    length += printf(";  start: ");
    length += rtems_monitor_symbol_dump(&monitor_extension->start, verbose);
    length += printf(";  restart: ");
    length += rtems_monitor_symbol_dump(&monitor_extension->restart, verbose);
    length += printf("\n");
    length = 0;

    length += rtems_monitor_pad(18, length);
    length += printf("delete: ");
    length += rtems_monitor_symbol_dump(&monitor_extension->delete, verbose);
    length += printf(";  switch: ");
    length += rtems_monitor_symbol_dump(&monitor_extension->tswitch, verbose);
    length += printf(";  begin: ");
    length += rtems_monitor_symbol_dump(&monitor_extension->begin, verbose);
    length += printf("\n");
    length = 0;

    length += rtems_monitor_pad(18, length);
    length += printf("exitted: ");
    length += rtems_monitor_symbol_dump(&monitor_extension->exitted, verbose);
    length += printf(";  fatal: ");
    length += rtems_monitor_symbol_dump(&monitor_extension->fatal, verbose);
    length += printf("\n");
    length = 0;
    printf("\n");
}

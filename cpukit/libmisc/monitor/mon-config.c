/*
 * RTEMS Config display support
 *
 * TODO
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/monitor.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>             /* strtoul() */

#define DATACOL 15
#define CONTCOL DATACOL		/* continued col */

/*
 * Fill in entire monitor config table
 * for sending to a remote monitor or printing on the local system
 */

void
rtems_monitor_config_canonical(
    rtems_monitor_config_t *canonical_config,
    const void             *config_void
)
{
    const rtems_api_configuration_table *r;

    r = rtems_configuration_get_rtems_api_configuration();

    canonical_config->work_space_size = rtems_configuration_get_work_space_size();
    canonical_config->maximum_tasks = rtems_configuration_get_maximum_tasks();
    canonical_config->maximum_timers = rtems_configuration_get_maximum_timers();
    canonical_config->maximum_semaphores = rtems_configuration_get_maximum_semaphores();
    canonical_config->maximum_message_queues = rtems_configuration_get_maximum_message_queues();
    canonical_config->maximum_partitions = rtems_configuration_get_maximum_partitions();
    canonical_config->maximum_regions = rtems_configuration_get_maximum_regions();
    canonical_config->maximum_ports = rtems_configuration_get_maximum_ports();
    canonical_config->maximum_periods = rtems_configuration_get_maximum_periods();
    canonical_config->maximum_extensions = rtems_configuration_get_maximum_extensions();
    canonical_config->microseconds_per_tick = rtems_configuration_get_microseconds_per_tick();
    canonical_config->ticks_per_timeslice = rtems_configuration_get_ticks_per_timeslice();
    canonical_config->number_of_initialization_tasks = r->number_of_initialization_tasks;
}

/*
 * This is easy, since there is only 1 (altho we could get them from
 *    other nodes...)
 */

const void *
rtems_monitor_config_next(
    void                  *object_info RTEMS_UNUSED,
    rtems_monitor_config_t *canonical_config RTEMS_UNUSED,
    rtems_id              *next_id
)
{
    int n = rtems_object_id_get_index(*next_id);

    if (n >= 1)
        goto failed;

    _Objects_Allocator_lock();

    *next_id += 1;
    return (const void *) (uintptr_t) 1;

failed:
    *next_id = RTEMS_OBJECT_ID_FINAL;
    return 0;
}


void
rtems_monitor_config_dump_header(
    bool verbose RTEMS_UNUSED
)
{
    fprintf(stdout,"\
INITIAL (startup) Configuration Info\n");
/*23456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
0         1         2         3         4         5         6         7       */
    rtems_monitor_separator();
}


int
rtems_monitor_config_dump(
    rtems_monitor_config_t *monitor_config,
    bool                 verbose RTEMS_UNUSED
)
{
    int     length = 0;

    length = 0;
    length += fprintf(stdout,"WORKSPACE");
    length += rtems_monitor_pad(DATACOL, length);
    length += fprintf(stdout,"start: %p;  size: 0x%" PRIx32 " (%" PRId32 ")\n",
                     monitor_config->work_space_start,
                     monitor_config->work_space_size,
                     monitor_config->work_space_size);

    length = 0;
    length += fprintf(stdout,"TIME");
    length += rtems_monitor_pad(DATACOL, length);
    length += fprintf(stdout,"usec/tick: %" PRId32 ";  tick/timeslice: %" PRId32 ";  tick/sec: %" PRId32 "\n",
                     monitor_config->microseconds_per_tick,
                     monitor_config->ticks_per_timeslice,
                     1000000 / monitor_config->microseconds_per_tick);

    length = 0;
    length += fprintf(stdout,"MAXIMUMS");
    length += rtems_monitor_pad(DATACOL, length);
    length += fprintf(stdout,"tasks: %" PRId32 "%c;  timers: %" PRId32 "%c;  sems: %" PRId32 "%c;  que's: %" PRId32 "%c;  ext's: %" PRId32 "%c;\n",
                     monitor_config->maximum_tasks & ~OBJECTS_UNLIMITED_OBJECTS,
                     (monitor_config->maximum_tasks & OBJECTS_UNLIMITED_OBJECTS) == 0 ? ' ' : '+',
                     monitor_config->maximum_timers & ~OBJECTS_UNLIMITED_OBJECTS,
                     (monitor_config->maximum_timers & OBJECTS_UNLIMITED_OBJECTS) == 0 ? ' ' : '+',
                     monitor_config->maximum_semaphores & ~OBJECTS_UNLIMITED_OBJECTS,
                     (monitor_config->maximum_semaphores & OBJECTS_UNLIMITED_OBJECTS) == 0 ? ' ' : '+',
                     monitor_config->maximum_message_queues & ~OBJECTS_UNLIMITED_OBJECTS,
                     (monitor_config->maximum_message_queues & OBJECTS_UNLIMITED_OBJECTS) == 0 ? ' ' : '+',
                     monitor_config->maximum_extensions & ~OBJECTS_UNLIMITED_OBJECTS,
                     (monitor_config->maximum_extensions & OBJECTS_UNLIMITED_OBJECTS) == 0 ? ' ' : '+');
    length = 0;
    length += rtems_monitor_pad(CONTCOL, length);
    length += fprintf(stdout,"partitions: %" PRId32 "%c;  regions: %" PRId32 "%c;  ports: %" PRId32 "%c;  periods: %" PRId32 "%c;\n",
                     monitor_config->maximum_partitions & ~OBJECTS_UNLIMITED_OBJECTS,
                     (monitor_config->maximum_partitions & OBJECTS_UNLIMITED_OBJECTS) == 0 ? ' ' : '+',
                     monitor_config->maximum_regions & ~OBJECTS_UNLIMITED_OBJECTS,
                     (monitor_config->maximum_regions & OBJECTS_UNLIMITED_OBJECTS) == 0 ? ' ' : '+',
                     monitor_config->maximum_ports & ~OBJECTS_UNLIMITED_OBJECTS,
                     (monitor_config->maximum_ports & OBJECTS_UNLIMITED_OBJECTS) == 0 ? ' ' : '+',
                     monitor_config->maximum_periods & ~OBJECTS_UNLIMITED_OBJECTS,
                     (monitor_config->maximum_periods & OBJECTS_UNLIMITED_OBJECTS) == 0 ? ' ' : '+');
    return length;
}

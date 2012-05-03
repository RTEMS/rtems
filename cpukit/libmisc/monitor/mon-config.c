/*
 * RTEMS Config display support
 *
 * TODO
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
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
    void                   *config_void
)
{
    rtems_configuration_table *c = (rtems_configuration_table *) config_void;
    rtems_api_configuration_table *r = &Configuration_RTEMS_API;

    canonical_config->work_space_start = c->work_space_start;
    canonical_config->work_space_size = c->work_space_size;
    canonical_config->maximum_tasks = r->maximum_tasks;
    canonical_config->maximum_timers = r->maximum_timers;
    canonical_config->maximum_semaphores = r->maximum_semaphores;
    canonical_config->maximum_message_queues = r->maximum_message_queues;
    canonical_config->maximum_partitions = r->maximum_partitions;
    canonical_config->maximum_regions = r->maximum_regions;
    canonical_config->maximum_ports = r->maximum_ports;
    canonical_config->maximum_periods = r->maximum_periods;
    canonical_config->maximum_extensions = c->maximum_extensions;
    canonical_config->microseconds_per_tick = c->microseconds_per_tick;
    canonical_config->ticks_per_timeslice = c->ticks_per_timeslice;
    canonical_config->number_of_initialization_tasks = r->number_of_initialization_tasks;
}

/*
 * This is easy, since there is only 1 (altho we could get them from
 *    other nodes...)
 */

void *
rtems_monitor_config_next(
    void                  *object_info __attribute__((unused)),
    rtems_monitor_config_t *canonical_config __attribute__((unused)),
    rtems_id              *next_id
)
{
    rtems_configuration_table *c = &Configuration;
    int n = rtems_object_id_get_index(*next_id);

    if (n >= 1)
        goto failed;

    _Thread_Disable_dispatch();

    *next_id += 1;
    return (void *) c;

failed:
    *next_id = RTEMS_OBJECT_ID_FINAL;
    return 0;
}


void
rtems_monitor_config_dump_header(
    bool verbose __attribute__((unused))
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
    bool                 verbose __attribute__((unused))
)
{
    int     length = 0;

    length = 0;
    length += fprintf(stdout,"WORKSPACE");
    length += rtems_monitor_pad(DATACOL, length);
    length += fprintf(stdout,"start: %p;  size: 0x%" PRIx32 "\n",
                     monitor_config->work_space_start,
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
    length += fprintf(stdout,"tasks: %" PRId32 ";  timers: %" PRId32 ";  sems: %" PRId32 ";  que's: %" PRId32 ";  ext's: %" PRId32 "\n",
                     monitor_config->maximum_tasks,
                     monitor_config->maximum_timers,
                     monitor_config->maximum_semaphores,
                     monitor_config->maximum_message_queues,
                     monitor_config->maximum_extensions);
    length = 0;
    length += rtems_monitor_pad(CONTCOL, length);
    length += fprintf(stdout,"partitions: %" PRId32 ";  regions: %" PRId32 ";  ports: %" PRId32 ";  periods: %" PRId32 "\n",
                     monitor_config->maximum_partitions,
                     monitor_config->maximum_regions,
                     monitor_config->maximum_ports,
                     monitor_config->maximum_periods);
    return length;
}

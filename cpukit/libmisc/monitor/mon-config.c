/*
 * RTEMS Config display support
 *
 * TODO
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
    rtems_api_configuration_table *r = c->RTEMS_api_configuration;

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
    void                  *object_info,
    rtems_monitor_config_t *canonical_config,
    rtems_id              *next_id
)
{
    rtems_configuration_table *c = _Configuration_Table;
    int n = rtems_get_index(*next_id);

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
    boolean verbose
)
{
    printf("\
INITIAL (startup) Configuration Info\n");
/*23456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
0         1         2         3         4         5         6         7       */
    rtems_monitor_separator();
}


void
rtems_monitor_config_dump(
    rtems_monitor_config_t *monitor_config,
    boolean                 verbose
)
{
    unsigned32   length = 0;

    length = 0;
    length += printf("WORKSPACE");
    length += rtems_monitor_pad(DATACOL, length);
    length += printf("start: %p;  size: 0x%x\n",
                     monitor_config->work_space_start,
                     monitor_config->work_space_size);

    length = 0;
    length += printf("TIME");
    length += rtems_monitor_pad(DATACOL, length);
    length += printf("usec/tick: %d;  tick/timeslice: %d;  tick/sec: %d\n",
                     monitor_config->microseconds_per_tick,
                     monitor_config->ticks_per_timeslice,
                     1000000 / monitor_config->microseconds_per_tick);

    length = 0;
    length += printf("MAXIMUMS");
    length += rtems_monitor_pad(DATACOL, length);
    length += printf("tasks: %d;  timers: %d;  sems: %d;  que's: %d;  ext's: %d\n",
                     monitor_config->maximum_tasks,
                     monitor_config->maximum_timers,
                     monitor_config->maximum_semaphores,
                     monitor_config->maximum_message_queues,
                     monitor_config->maximum_extensions);
    length = 0;
    length += rtems_monitor_pad(CONTCOL, length);
    length += printf("partitions: %d;  regions: %d;  ports: %d;  periods: %d\n",
                     monitor_config->maximum_partitions,
                     monitor_config->maximum_regions,
                     monitor_config->maximum_ports,
                     monitor_config->maximum_periods);
}

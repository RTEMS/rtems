/*
 * RTEMS Monitor init task support
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define __RTEMS_VIOLATE_KERNEL_VISIBILITY__
#include <rtems.h>
#include <rtems/monitor.h>

#include <inttypes.h>
#include <stdio.h>

/*
 * As above, but just for init tasks
 */
void
rtems_monitor_init_task_canonical(
    rtems_monitor_init_task_t *canonical_itask,
    void                  *itask_void
)
{
    rtems_initialization_tasks_table *rtems_itask = itask_void;

    rtems_monitor_symbol_canonical_by_value(&canonical_itask->entry,
                                            (void *) rtems_itask->entry_point);

    canonical_itask->argument = rtems_itask->argument;
    canonical_itask->stack_size = rtems_itask->stack_size;
    canonical_itask->priority = rtems_itask->initial_priority;
    canonical_itask->modes = rtems_itask->mode_set;
    canonical_itask->attributes = rtems_itask->attribute_set;
}

void *
rtems_monitor_init_task_next(
    void                  *object_info __attribute__((unused)),
    rtems_monitor_init_task_t *canonical_init_task,
    rtems_id              *next_id
)
{
    rtems_initialization_tasks_table *itask;
    uint32_t   n = rtems_object_id_get_index(*next_id);

    if (n >= Configuration_RTEMS_API.number_of_initialization_tasks)
        goto failed;

    _Thread_Disable_dispatch();

    itask = Configuration_RTEMS_API.User_initialization_tasks_table + n;

    /*
     * dummy up a fake id and name for this item
     */

    canonical_init_task->id = n;
    canonical_init_task->name = itask->name;

    *next_id += 1;
    return (void *) itask;

failed:
    *next_id = RTEMS_OBJECT_ID_FINAL;
    return 0;
}


void
rtems_monitor_init_task_dump_header(
    bool verbose __attribute__((unused))
)
{
    fprintf(stdout,"\
  #    NAME   ENTRY        ARGUMENT    PRIO   MODES  ATTRIBUTES   STACK SIZE\n");
/*23456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
0         1         2         3         4         5         6         7       */
    rtems_monitor_separator();
}

/*
 */

void
rtems_monitor_init_task_dump(
    rtems_monitor_init_task_t *monitor_itask,
    bool  verbose
)
{
    int length = 0;

    length += rtems_monitor_dump_decimal(monitor_itask->id);

    length += rtems_monitor_pad(7, length);
    length += rtems_monitor_dump_name(monitor_itask->id);

    length += rtems_monitor_pad(14, length);
    length += rtems_monitor_symbol_dump(&monitor_itask->entry, verbose);

    length += rtems_monitor_pad(25, length);
    length += fprintf(stdout,"%" PRId32 " [0x%" PRIx32 "]",
      monitor_itask->argument, monitor_itask->argument);

    length += rtems_monitor_pad(39, length);
    length += rtems_monitor_dump_priority(monitor_itask->priority);

    length += rtems_monitor_pad(46, length);
    length += rtems_monitor_dump_modes(monitor_itask->modes);

    length += rtems_monitor_pad(54, length);
    length += rtems_monitor_dump_attributes(monitor_itask->attributes);

    length += rtems_monitor_pad(66, length);
    length += fprintf(stdout,"%" PRId32 " [0x%" PRIx32 "]",
      monitor_itask->stack_size, monitor_itask->stack_size);

    fprintf(stdout,"\n");
}

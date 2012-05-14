/*
 * RTEMS Monitor "manager" support.
 * Used to traverse object (chain) lists and print them out.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/monitor.h>

#include <stdio.h>

/*
 * "next" routine for all objects that are RTEMS manager objects
 */

void *
rtems_monitor_manager_next(
    void      *table_void,
    void      *canonical,
    rtems_id  *next_id
)
{
    Objects_Information     *table = table_void;
    rtems_monitor_generic_t *copy;
    Objects_Control         *object = 0;
    Objects_Locations        location;

    /*
     * When we are called, it must be local
     */

#if defined(RTEMS_MULTIPROCESSING)
    if ( ! _Objects_Is_local_id(*next_id) )
        goto done;
#endif

    object = _Objects_Get_next(table, *next_id, &location, next_id);

    if (object)
    {
        copy = (rtems_monitor_generic_t *) canonical;
        copy->id = object->id;
        copy->name = object->name.name_u32;
    }

#if defined(RTEMS_MULTIPROCESSING)
done:
#endif
    return object;
}

/*
 *	@(#)manager.c	1.2 - 95/07/31
 *	
 *
 * RTEMS Monitor "manager" support.
 * Used to traverse object (chain) lists and print them out.
 *  $Id$
 */

#include <rtems.h>
#include "monitor.h"

#include <stdio.h>

#include <monitor.h>

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
    unsigned32               location;

    /*
     * When we are called, it must be local
     */
       
    if ( ! _Objects_Is_local_id(*next_id))
        goto done;
    
    object = _Objects_Get_next(table, *next_id, &location, next_id);

    if (object)
    {
        copy = (rtems_monitor_generic_t *) canonical;
        copy->id = object->id;
        copy->name = table->name_table[rtems_get_index(copy->id)];
    }    
    
done:
    return object;
}


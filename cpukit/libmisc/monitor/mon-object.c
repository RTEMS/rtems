/*
 * RTEMS Monitor "object" support.
 *
 * Used to traverse object lists and print them out.
 * An object can be an RTEMS object (chain based stuff) or
 * a "misc" object such as a device driver.
 *
 * Each object has its own file in this directory (eg: extension.c)
 * That file provides routines to convert a "native" structure
 * to its canonical form, print a canonical structure, etc.
 *
 * TODO:
 *     should allow for non-numeric id's???
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
#include <string.h>             /* memcpy() */

#define NUMELEMS(arr)	(sizeof(arr) / sizeof(arr[0]))

/*
 * add:
 *     next
 */

rtems_monitor_object_info_t rtems_monitor_object_info[] =
{
    { RTEMS_MONITOR_OBJECT_CONFIG,
      (void *) 0,
      sizeof(rtems_monitor_config_t),
      (rtems_monitor_object_next_fn)        rtems_monitor_config_next,
      (rtems_monitor_object_canonical_fn)   rtems_monitor_config_canonical,
      (rtems_monitor_object_dump_header_fn) rtems_monitor_config_dump_header,
      (rtems_monitor_object_dump_fn)        rtems_monitor_config_dump,
    },
    { RTEMS_MONITOR_OBJECT_MPCI,
      (void *) 0,
#if defined(RTEMS_MULTIPROCESSING)
      sizeof(rtems_monitor_mpci_t),
      (rtems_monitor_object_next_fn)        rtems_monitor_mpci_next,
      (rtems_monitor_object_canonical_fn)   rtems_monitor_mpci_canonical,
      (rtems_monitor_object_dump_header_fn) rtems_monitor_mpci_dump_header,
      (rtems_monitor_object_dump_fn)        rtems_monitor_mpci_dump,
#else
      0,
      (rtems_monitor_object_next_fn)        0,
      (rtems_monitor_object_canonical_fn)   0,
      (rtems_monitor_object_dump_header_fn) 0,
      (rtems_monitor_object_dump_fn)        0,
#endif
    },
    { RTEMS_MONITOR_OBJECT_INIT_TASK,
      (void *) 0,
      sizeof(rtems_monitor_init_task_t),
      (rtems_monitor_object_next_fn)        rtems_monitor_init_task_next,
      (rtems_monitor_object_canonical_fn)   rtems_monitor_init_task_canonical,
      (rtems_monitor_object_dump_header_fn) rtems_monitor_init_task_dump_header,
      (rtems_monitor_object_dump_fn)        rtems_monitor_init_task_dump,
    },
    { RTEMS_MONITOR_OBJECT_TASK,
      (void *) &_RTEMS_tasks_Information,
      sizeof(rtems_monitor_task_t),
      (rtems_monitor_object_next_fn)        rtems_monitor_manager_next,
      (rtems_monitor_object_canonical_fn)   rtems_monitor_task_canonical,
      (rtems_monitor_object_dump_header_fn) rtems_monitor_task_dump_header,
      (rtems_monitor_object_dump_fn)        rtems_monitor_task_dump,
    },
    { RTEMS_MONITOR_OBJECT_QUEUE,
      (void *) &_Message_queue_Information,
      sizeof(rtems_monitor_queue_t),
      (rtems_monitor_object_next_fn)        rtems_monitor_manager_next,
      (rtems_monitor_object_canonical_fn)   rtems_monitor_queue_canonical,
      (rtems_monitor_object_dump_header_fn) rtems_monitor_queue_dump_header,
      (rtems_monitor_object_dump_fn)        rtems_monitor_queue_dump,
    },
    { RTEMS_MONITOR_OBJECT_EXTENSION,
      (void *) &_Extension_Information,
      sizeof(rtems_monitor_extension_t),
      (rtems_monitor_object_next_fn)        rtems_monitor_manager_next,
      (rtems_monitor_object_canonical_fn)   rtems_monitor_extension_canonical,
      (rtems_monitor_object_dump_header_fn) rtems_monitor_extension_dump_header,
      (rtems_monitor_object_dump_fn)        rtems_monitor_extension_dump,
    },
    { RTEMS_MONITOR_OBJECT_DRIVER,
      (void *) 0,
      sizeof(rtems_monitor_driver_t),
      (rtems_monitor_object_next_fn)        rtems_monitor_driver_next,
      (rtems_monitor_object_canonical_fn)   rtems_monitor_driver_canonical,
      (rtems_monitor_object_dump_header_fn) rtems_monitor_driver_dump_header,
      (rtems_monitor_object_dump_fn)        rtems_monitor_driver_dump,
    },
    { RTEMS_MONITOR_OBJECT_DNAME,
      /* XXX now that the driver name table is allocated from the */
      /* XXX Workspace, this does not work */
      (void *) 0,
      /* (void *) _IO_Driver_name_table, */
      sizeof(rtems_monitor_dname_t),
      (rtems_monitor_object_next_fn)        rtems_monitor_dname_next,
      (rtems_monitor_object_canonical_fn)   rtems_monitor_dname_canonical,
      (rtems_monitor_object_dump_header_fn) rtems_monitor_dname_dump_header,
      (rtems_monitor_object_dump_fn)        rtems_monitor_dname_dump,
    },
};

/*
 * Allow id's to be specified without the node number or
 * type for convenience.
 */

rtems_id
rtems_monitor_id_fixup(
    rtems_id            id,
    unsigned32          default_node,
    rtems_monitor_object_type_t type
)
{
    unsigned32  node;
    
    node = rtems_get_node(id);
    if (node == 0)
    {
        if (rtems_get_class(id) != OBJECTS_NO_CLASS)
            type = rtems_get_class(id);

        id = _Objects_Build_id(
          OBJECTS_CLASSIC_API, type, default_node, rtems_get_index(id));
    }
    return id;
}


rtems_monitor_object_info_t *
rtems_monitor_object_lookup(
    rtems_monitor_object_type_t type
)
{
    rtems_monitor_object_info_t *p;
    for (p = &rtems_monitor_object_info[0];
         p < &rtems_monitor_object_info[NUMELEMS(rtems_monitor_object_info)];
         p++)
    {
        if (p->type == type)
            return p;
    }
    return 0;
}

rtems_id
rtems_monitor_object_canonical_next_remote(
    rtems_monitor_object_type_t type,
    rtems_id            id,
    void               *canonical
)
{
    rtems_id                        next_id;
    rtems_status_code               status;
    rtems_monitor_server_request_t  request;
    rtems_monitor_server_response_t response;

    /*
     * Send request
     */
    
    request.command = RTEMS_MONITOR_SERVER_CANONICAL;
    request.argument0 = (unsigned32) type;
    request.argument1 = (unsigned32) id;

    status = rtems_monitor_server_request(rtems_get_node(id), &request, &response);
    if (status != RTEMS_SUCCESSFUL)
        goto failed;

    /*
     * process response
     */
    
    next_id = (rtems_id) response.result0;
    if (next_id != RTEMS_OBJECT_ID_FINAL)
        (void) memcpy(canonical, &response.payload, response.result1);

    return next_id;

failed:
    return RTEMS_OBJECT_ID_FINAL;

}


rtems_id
rtems_monitor_object_canonical_next(
    rtems_monitor_object_info_t *info,
    rtems_id                     id,
    void                        *canonical
)
{
    rtems_id                     next_id;
    void                        *raw_item;

    if ( ! _Objects_Is_local_id(id))
        next_id = rtems_monitor_object_canonical_next_remote(info->type,
                                                             id,
                                                             canonical);
    else
    {
        next_id = id;
        
        raw_item = (void *) info->next(info->object_information,
                                       canonical,
                                       &next_id);

        if (raw_item)
        {
            info->canonical(canonical, raw_item);
            _Thread_Enable_dispatch();
        }    
    }    
    return next_id;
}


/*
 * this is routine server invokes locally to get the type
 */

rtems_id
rtems_monitor_object_canonical_get(
    rtems_monitor_object_type_t  type,
    rtems_id             id,
    void                *canonical,
    unsigned32          *size_p
)
{
    rtems_monitor_object_info_t *info;
    rtems_id                     next_id;

    *size_p = 0;

    info = rtems_monitor_object_lookup(type);

    if (info == 0)
        return RTEMS_OBJECT_ID_FINAL;

    next_id = rtems_monitor_object_canonical_next(info, id, canonical);
    *size_p = info->size;

    return next_id;
}


void
rtems_monitor_object_dump_1(
    rtems_monitor_object_info_t *info,
    rtems_id                     id,
    boolean                      verbose
)
{
    rtems_id next_id;
    rtems_monitor_union_t canonical;

    if ((next_id = rtems_monitor_object_canonical_next(
                                     info,
                                     id,
                                     &canonical)) != RTEMS_OBJECT_ID_FINAL)
    {
        /*
         * If the one we actually got is the one we wanted, then
         * print it out.
         * For ones that have an id field, this works fine,
         * for all others, always dump it out.
         *
         * HACK: the way we determine whether there is an id is a hack.
         *
         * by the way: the reason we try to not have an id, is that some
         *   of the canonical structures are almost too big for shared
         *   memory driver (eg: mpci)
         */
        
        if ((info->next != rtems_monitor_manager_next) ||
            (id == canonical.generic.id))
            info->dump(&canonical, verbose);
    }
}

void
rtems_monitor_object_dump_all(
    rtems_monitor_object_info_t *info,
    boolean                      verbose
)
{
    rtems_id next_id;
    rtems_monitor_union_t canonical;

    next_id = RTEMS_OBJECT_ID_INITIAL(OBJECTS_CLASSIC_API, info->type, rtems_monitor_default_node);

    while ((next_id = rtems_monitor_object_canonical_next(
                                         info,
                                         next_id,
                                         &canonical)) != RTEMS_OBJECT_ID_FINAL)
    {
        info->dump(&canonical, verbose);
    }
}

void
rtems_monitor_object_cmd(
    int        argc,
    char     **argv,
    unsigned32 command_arg,
    boolean    verbose
)
{
    int arg;
    rtems_monitor_object_info_t *info = 0;
    rtems_monitor_object_type_t  type = (rtems_monitor_object_type_t) command_arg;
    
    /* what is the default type? */
    type = (rtems_monitor_object_type_t) command_arg;

    if (argc == 1)
    {
        if (type == RTEMS_MONITOR_OBJECT_INVALID)
        {
            printf("A type must be specified to \"dump all\"\n");
            goto done;
        }
        
        info = rtems_monitor_object_lookup(type);
        if (info == 0)
            goto not_found;

        if (info->dump_header)
            info->dump_header(verbose);
        rtems_monitor_object_dump_all(info, verbose);
    }
    else
    {
        unsigned32          default_node = rtems_monitor_default_node;
        rtems_monitor_object_type_t last_type = RTEMS_MONITOR_OBJECT_INVALID;
        rtems_id            id;

        for (arg=1; argv[arg]; arg++)
        {
            id = (rtems_id) strtoul(argv[arg], 0, 16);
            id = rtems_monitor_id_fixup(id, default_node, type);
            type = (rtems_monitor_object_type_t) rtems_get_class(id);

            /*
             * Allow the item type to change in the middle
             * of the command.  If the type changes, then
             * just dump out a new header and keep on going.
             */
            if (type != last_type)
            {
                info = rtems_monitor_object_lookup(type);
                if (info == 0)
                    goto not_found;
            
                if (info->dump_header)
                    info->dump_header(verbose);
            }

            if (info == 0)
            {
not_found:      printf("Invalid or unsupported type %d\n", type);
                goto done;
            }

            rtems_monitor_object_dump_1(info, id, verbose);

            default_node = rtems_get_node(id);
            
            last_type = type;
        }
    }
done:
    return;
}

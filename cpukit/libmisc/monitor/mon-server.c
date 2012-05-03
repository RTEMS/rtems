/*
 * RTEMS monitor server (handles requests for info from RTEMS monitors
 *             running on other nodes)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <rtems/monitor.h>

/*
 * Various id's for the server
 */

rtems_id  rtems_monitor_server_task_id;
rtems_id  rtems_monitor_server_request_queue_id;	/* our server */
rtems_id *rtems_monitor_server_request_queue_ids;       /* all servers */
rtems_id  rtems_monitor_server_response_queue_id;       /* our server */


/*
 * Send a request to a server task
 */

rtems_status_code
rtems_monitor_server_request(
    uint32_t                         server_node,
    rtems_monitor_server_request_t  *request,
    rtems_monitor_server_response_t *response
)
{
    rtems_id          server_id;
    rtems_status_code status;
    size_t            size;

    /*
     * What is id of monitor on target node?
     * Look it up if we don't know it yet.
     */

    server_id = rtems_monitor_server_request_queue_ids[server_node];
    if (server_id == 0)
    {
        status = rtems_message_queue_ident(RTEMS_MONITOR_QUEUE_NAME,
                                           server_node,
                                           &server_id);
        if (status != RTEMS_SUCCESSFUL)
        {
            rtems_error(status, "ident of remote server failed");
            goto done;
        }

        rtems_monitor_server_request_queue_ids[server_node] = server_id;
    }

    request->return_id = rtems_monitor_server_response_queue_id;

    status = rtems_message_queue_send(server_id, request, sizeof(*request));
    if (status != RTEMS_SUCCESSFUL)
    {
        rtems_error(status, "monitor server request send failed");
        goto done;
    }

    /*
     * Await response, if requested
     */

    if (response)
    {
        status = rtems_message_queue_receive(rtems_monitor_server_response_queue_id,
                                             response,
                                             &size,
                                             RTEMS_WAIT,
                                             100);
        if (status != RTEMS_SUCCESSFUL)
        {
            rtems_error(status, "server did not respond");

            /* maybe server task was restarted; look it up again next time */
            rtems_monitor_server_request_queue_ids[server_node] = 0;

            goto done;
        }

        if (response->command != RTEMS_MONITOR_SERVER_RESPONSE)
        {
            status = RTEMS_INCORRECT_STATE;
            goto done;
        }
    }

done:
    return status;
}



/*
 * monitor server task
 */

void
rtems_monitor_server_task(
    rtems_task_argument monitor_flags __attribute__((unused))
)
{
    rtems_monitor_server_request_t  request;
    rtems_monitor_server_response_t response;
    rtems_status_code               status;
    size_t                          size;

    for (;;)
    {
        status = rtems_message_queue_receive(
                        rtems_monitor_server_request_queue_id,
                        &request,
                        &size,
                        RTEMS_WAIT,
                        (rtems_interval) 0);

        if (status != RTEMS_SUCCESSFUL)
        {
            rtems_error(status, "monitor server msg queue receive error");
            goto failed;
        }

        if (size != sizeof(request))
        {
            rtems_error(0, "monitor server bad size on receive");
            goto failed;
        }

        switch (request.command)
        {
            case RTEMS_MONITOR_SERVER_CANONICAL:
            {
                rtems_monitor_object_type_t object_type;
                rtems_id            id;
                rtems_id            next_id;

                object_type = (rtems_monitor_object_type_t) request.argument0;
                id          = (rtems_id)            request.argument1;
                next_id = rtems_monitor_object_canonical_get(object_type,
                                                             id,
                                                             &response.payload,
                                                             &size);

                response.command = RTEMS_MONITOR_SERVER_RESPONSE;
                response.result0 = next_id;
                response.result1 = size;

#define SERVER_OVERHEAD  (offsetof(rtems_monitor_server_response_t, \
                                         payload))

                status = rtems_message_queue_send(request.return_id,
                                                  &response,
                                                  size + SERVER_OVERHEAD);
                if (status != RTEMS_SUCCESSFUL)
                {
                    rtems_error(status, "response send failed");
                    goto failed;
                }
                break;
            }

            default:
            {
                rtems_error(0, "invalid command to monitor server: %d", request.command);
                goto failed;
            }
        }
    }

failed:
    rtems_task_delete(RTEMS_SELF);
}


/*
 * Kill off any old server
 * Not sure if this is useful, but it doesn't help
 */

void
rtems_monitor_server_kill(void)
{
    if (rtems_monitor_server_task_id)
        rtems_task_delete(rtems_monitor_server_task_id);
    rtems_monitor_server_task_id = 0;

    if (rtems_monitor_server_request_queue_id)
        rtems_message_queue_delete(rtems_monitor_server_request_queue_id);
    rtems_monitor_server_request_queue_ids = 0;

    if (rtems_monitor_server_response_queue_id)
        rtems_message_queue_delete(rtems_monitor_server_response_queue_id);
    rtems_monitor_server_response_queue_id = 0;

    if (rtems_monitor_server_request_queue_ids)
        free(rtems_monitor_server_request_queue_ids);
    rtems_monitor_server_request_queue_ids = 0;
}


void
rtems_monitor_server_init(
    uint32_t   monitor_flags __attribute__((unused))
)
{
    #if defined(RTEMS_MULTIPROCESSING)
    rtems_status_code status;

    if (_System_state_Is_multiprocessing    &&
        (_Configuration_MP_table->maximum_nodes > 1))
    {
        uint32_t   maximum_nodes = _Configuration_MP_table->maximum_nodes;

        /*
         * create the msg que our server will listen
         * Since we only get msgs from other RTEMS monitors, we just
         * need reserve space for 1 msg from each node.
         */

        status = rtems_message_queue_create(
                       RTEMS_MONITOR_QUEUE_NAME,
                       maximum_nodes,
                       sizeof(rtems_monitor_server_request_t),
                       RTEMS_GLOBAL,
                       &rtems_monitor_server_request_queue_id);

        if (status != RTEMS_SUCCESSFUL)
        {
            rtems_error(status, "could not create monitor server message queue");
            goto done;
        }

        /*
         * create the msg que our responses will come on
         * Since monitor just does one thing at a time, we only need 1 item
         * message queue.
         */

        status = rtems_message_queue_create(
                       RTEMS_MONITOR_RESPONSE_QUEUE_NAME,
                       1, /* depth */
                       sizeof(rtems_monitor_server_response_t),
                       RTEMS_GLOBAL,
                       &rtems_monitor_server_response_queue_id);

        if (status != RTEMS_SUCCESSFUL)
        {
            rtems_error(status, "could not create monitor response message queue");
            goto done;
        }

        /* need an id for queue of each other server we might talk to */
        /* indexed by node, so add 1 to maximum_nodes */
        rtems_monitor_server_request_queue_ids =
                   (rtems_id *) malloc((maximum_nodes + 1) * sizeof(rtems_id));
        (void) memset(rtems_monitor_server_request_queue_ids,
                      0,
                      (maximum_nodes + 1) * sizeof(rtems_id));

        rtems_monitor_server_request_queue_ids[rtems_monitor_node] =
                   rtems_monitor_server_request_queue_id;

        /*
         * create the server task
         */
        status = rtems_task_create(RTEMS_MONITOR_SERVER_NAME,
                                   1,
                                   0 /* default stack */,
                                   RTEMS_INTERRUPT_LEVEL(0),
                                   RTEMS_DEFAULT_ATTRIBUTES,
                                   &rtems_monitor_server_task_id);
        if (status != RTEMS_SUCCESSFUL)
        {
            rtems_error(status, "could not create monitor server task");
            goto done;
        }

        /*
         * Start the server task
         */
        status = rtems_task_start(rtems_monitor_server_task_id,
                                  rtems_monitor_server_task,
                                  monitor_flags);
        if (status != RTEMS_SUCCESSFUL)
        {
            rtems_error(status, "could not start monitor server");
            goto done;
        }
    }

done:
    #endif
    return;
}

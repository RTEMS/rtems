/**
 * @file
 *
 * @brief Initializes message_queue Manager Related Data Structures
 * @ingroup POSIX_MQUEUE_P Message Queues Private Support Information
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>

#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <limits.h>

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/watchdog.h>
#include <rtems/seterr.h>
#include <rtems/posix/mqueueimpl.h>
#include <rtems/posix/time.h>

/*
 *  _POSIX_Message_queue_Manager_initialization
 *
 *  This routine initializes all message_queue manager related data structures.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 */

void _POSIX_Message_queue_Manager_initialization(void)
{
  _Objects_Initialize_information(
    &_POSIX_Message_queue_Information, /* object information table */
    OBJECTS_POSIX_API,                 /* object API */
    OBJECTS_POSIX_MESSAGE_QUEUES,      /* object class */
    Configuration_POSIX_API.maximum_message_queues,
                                /* maximum objects of this class */
    sizeof( POSIX_Message_queue_Control ),
                                /* size of this object's control block */
    true,                       /* true if names for this object are strings */
    _POSIX_PATH_MAX             /* maximum length of each object's name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    false,                      /* true if this is a global object class */
    NULL                        /* Proxy extraction support callout */
#endif
  );
  _Objects_Initialize_information(
    &_POSIX_Message_queue_Information_fds,
    OBJECTS_POSIX_API,
    OBJECTS_POSIX_MESSAGE_QUEUE_FDS,
    Configuration_POSIX_API.maximum_message_queue_descriptors,
    sizeof( POSIX_Message_queue_Control_fd ),
                                /* size of this object's control block */
    true,                       /* true if names for this object are strings */
    NAME_MAX                    /* maximum length of each object's name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    false,                      /* true if this is a global object class */
    NULL                        /* Proxy extraction support callout */
#endif
  );
}

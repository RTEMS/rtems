/**
 * @file
 *
 * @ingroup RTEMSMedia
 *
 * @brief Media API.
 */

/*
 * Copyright (c) 2009, 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef RTEMS_MEDIA_H
#define RTEMS_MEDIA_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSMedia Media Manager
 *
 * @brief Removable media support.
 *
 * The media manager may be used to maintain the life cycle of a removable
 * media.  Currently only disk devices are supported.  The initiator posts an
 * event to the media manager and it will respond with appropriate default
 * actions.  For example a disk attach will lead to inspection of the partition
 * table and mounted file systems.  Clients can register listeners to react to
 * events.
 */
/**@{**/

#define RTEMS_MEDIA_MOUNT_BASE "/media"

#define RTEMS_MEDIA_DELIMITER '-'

/**
 * Disk life cycle events:
 * @dot
 *   digraph disk_events {
 *     "DISK ATTACH" -> "PARTITION INQUIRY";
 *     "DISK ATTACH" -> "MOUNT";
 *     "PARTITION INQUIRY" -> "PARTITION ATTACH";
 *     "PARTITION INQUIRY" -> "DISK DETACH";
 *     "PARTITION ATTACH" -> "MOUNT";
 *     "MOUNT" -> "UNMOUNT";
 *     "UNMOUNT" -> "PARTITION DETACH";
 *     "UNMOUNT" -> "DISK DETACH";
 *     "PARTITION DETACH" -> "DISK DETACH";
 *   }
 * @enddot
 */
typedef enum {
  RTEMS_MEDIA_EVENT_DISK_ATTACH,
  RTEMS_MEDIA_EVENT_DISK_DETACH,
  RTEMS_MEDIA_EVENT_MOUNT,
  RTEMS_MEDIA_EVENT_UNMOUNT,
  RTEMS_MEDIA_EVENT_PARTITION_INQUIRY,
  RTEMS_MEDIA_EVENT_PARTITION_ATTACH,
  RTEMS_MEDIA_EVENT_PARTITION_DETACH,
  RTEMS_MEDIA_EVENT_ERROR
} rtems_media_event;

/**
 * Normal state transition:
 * @dot
 *   digraph state {
 *     INQUIRY -> READY [label="all listeners\nreturned successful"];
 *     INQUIRY -> ABORTED [label="otherwise"];
 *     READY -> SUCCESS [label="the worker\nreturned successful"];
 *     READY -> FAILED [label="otherwise"];
 *   }
 * @enddot
 */
typedef enum {
  RTEMS_MEDIA_STATE_INQUIRY,
  RTEMS_MEDIA_STATE_READY,
  RTEMS_MEDIA_STATE_ABORTED,
  RTEMS_MEDIA_STATE_SUCCESS,
  RTEMS_MEDIA_STATE_FAILED,
  RTEMS_MEDIA_ERROR_DISK_UNKNOWN,
  RTEMS_MEDIA_ERROR_DISK_EXISTS,
  RTEMS_MEDIA_ERROR_DISK_OR_PARTITION_UNKNOWN,
  RTEMS_MEDIA_ERROR_DISK_OR_PARTITION_EXISTS,
  RTEMS_MEDIA_ERROR_PARTITION_UNKNOWN,
  RTEMS_MEDIA_ERROR_PARTITION_ORPHAN,
  RTEMS_MEDIA_ERROR_PARTITION_DETACH_WITH_MOUNT,
  RTEMS_MEDIA_ERROR_PARTITION_WITH_UNKNOWN_DISK,
  RTEMS_MEDIA_ERROR_MOUNT_POINT_UNKNOWN,
  RTEMS_MEDIA_ERROR_MOUNT_POINT_EXISTS,
  RTEMS_MEDIA_ERROR_MOUNT_POINT_ORPHAN
} rtems_media_state;

/**
 * @brief Event listener.
 *
 * The listener will be called with the @a listener_arg passed to
 * rtems_media_listener_add().
 *
 * Source and destination values for each event and state:
 * <table>
 *   <tr><th>Event</th><th>State</th><th>Source</th><th>Destination</th></tr>
 *   <tr>
 *     <td rowspan="5">RTEMS_MEDIA_EVENT_DISK_ATTACH</td>
 *     <td>RTEMS_MEDIA_STATE_INQUIRY</td><td>driver name</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_READY</td><td>driver name</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_ABORTED</td><td>driver name</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_SUCCESS</td><td>driver name</td><td>disk path</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_FAILED</td><td>driver name</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td rowspan="5">RTEMS_MEDIA_EVENT_DISK_DETACH</td>
 *     <td>RTEMS_MEDIA_STATE_INQUIRY</td><td>disk path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_READY</td><td>disk path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_ABORTED</td><td>disk path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_SUCCESS</td><td>disk path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_FAILED</td><td>disk path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td rowspan="5">RTEMS_MEDIA_EVENT_PARTITION_INQUIRY</td>
 *     <td>RTEMS_MEDIA_STATE_INQUIRY</td><td>disk path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_READY</td><td>disk path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_ABORTED</td><td>disk path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_SUCCESS</td><td>disk path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_FAILED</td><td>disk path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td rowspan="5">RTEMS_MEDIA_EVENT_PARTITION_ATTACH</td>
 *     <td>RTEMS_MEDIA_STATE_INQUIRY</td><td>disk path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_READY</td><td>disk path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_ABORTED</td><td>disk path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_SUCCESS</td>
 *     <td>disk path</td><td>partition path</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_FAILED</td><td>disk path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td rowspan="5">RTEMS_MEDIA_EVENT_PARTITION_DETACH</td>
 *     <td>RTEMS_MEDIA_STATE_INQUIRY</td><td>partition path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_READY</td><td>partition path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_ABORTED</td><td>partition path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_SUCCESS</td><td>partition path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_FAILED</td><td>partition path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td rowspan="5">RTEMS_MEDIA_EVENT_MOUNT</td>
 *     <td>RTEMS_MEDIA_STATE_INQUIRY</td>
 *     <td>disk or partition path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_READY</td>
 *     <td>disk or partition path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_ABORTED</td>
 *     <td>disk or partition path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_SUCCESS</td>
 *     <td>disk or partition path</td><td>mount path</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_FAILED</td>
 *     <td>disk or partition path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td rowspan="5">RTEMS_MEDIA_EVENT_UNMOUNT</td>
 *     <td>RTEMS_MEDIA_STATE_INQUIRY</td><td>mount path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_READY</td><td>mount path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_ABORTED</td><td>mount path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_SUCCESS</td><td>mount path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_STATE_FAILED</td><td>mount path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td rowspan="11">RTEMS_MEDIA_EVENT_ERROR</td>
 *     <td>RTEMS_MEDIA_ERROR_DISK_UNKNOWN</td><td>disk path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_ERROR_DISK_EXISTS</td><td>disk path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_ERROR_DISK_OR_PARTITION_UNKNOWN</td>
 *     <td>disk or partition path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_ERROR_DISK_OR_PARTITION_EXISTS</td>
 *     <td>disk or partition path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_ERROR_PARTITION_UNKNOWN</td>
 *     <td>partition path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_ERROR_PARTITION_ORPHAN</td>
 *     <td>partition path</td><td>disk path</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_ERROR_PARTITION_DETACH_WITH_MOUNT</td>
 *     <td>partition path</td><td>mount path</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_ERROR_PARTITION_WITH_UNKNOWN_DISK</td>
 *     <td>partition path</td><td>disk path</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_ERROR_MOUNT_POINT_UNKNOWN</td>
 *     <td>mount path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_ERROR_MOUNT_POINT_EXISTS</td>
 *     <td>mount path</td><td>NULL</td>
 *   </tr>
 *   <tr>
 *     <td>RTEMS_MEDIA_ERROR_MOUNT_POINT_ORPHAN</td>
 *     <td>mount path</td><td>disk path</td>
 *   </tr>
 * </table>
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_IO_ERROR In the inquiry state this will abort the action.
 */
typedef rtems_status_code (*rtems_media_listener)(
  rtems_media_event event,
  rtems_media_state state,
  const char *src,
  const char *dest,
  void *listener_arg
);

/**
 * @brief Do the work corresponding to an event.
 *
 * The @a state will be
 * - RTEMS_MEDIA_STATE_READY, or
 * - RTEMS_MEDIA_STATE_ABORTED.
 *
 * It will be called with the @a src and @a worker_arg arguments passed to
 * rtems_media_post_event().
 *
 * The destination shall be returned in @a dest in case of success.  It shall
 * be allocated with malloc().
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_IO_ERROR Failure.
 */
typedef rtems_status_code (*rtems_media_worker)(
  rtems_media_state state,
  const char *src,
  char **dest,
  void *worker_arg
);

/**
 * @name Base
 */
/**@{**/

/**
 * @brief Initializes the media manager.
 *
 * Calling this function more than once will have no effects.  There is no
 * protection against concurrent access.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_NO_MEMORY Not enough resources.
 */
rtems_status_code rtems_media_initialize(void);

/**
 * @brief Adds the @a listener with argument @a listener_arg.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_NO_MEMORY Not enough memory.
 * @retval RTEMS_TOO_MANY Such a listener is already present.
 */
rtems_status_code rtems_media_listener_add(
  rtems_media_listener listener,
  void *listener_arg
);

/**
 * @brief Removes the @a listener with argument @a listener_arg.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID No such listener is present.
 */
rtems_status_code rtems_media_listener_remove(
  rtems_media_listener listener,
  void *listener_arg
);

/**
 * @brief Posts the @a event with source @a src.
 *
 * The @a worker will be called with the @a worker_arg argument.
 *
 * The destination will be returned in @a dest in case of success.  It will be
 * allocated with malloc() and should be freed if not needed anymore.
 *
 * The work will be done by the calling thread.  You can avoid this if you use
 * the media server via rtems_media_server_post_event().
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_UNSATISFIED One or more listeners aborted the action.
 * @retval RTEMS_IO_ERROR The worker returned with an error status.
 */
rtems_status_code rtems_media_post_event(
  rtems_media_event event,
  const char *src,
  char **dest,
  rtems_media_worker worker,
  void *worker_arg
);

/** @} */

/**
 * @name Server
 */
/**@{**/

/**
 * @brief Initializes the media manager and media server.
 *
 * It creates a server task with the @a priority, @a stack_size, @a modes, and
 * @a attributes parameters.
 *
 * Calling this function more than once will have no effects.  There is no
 * protection against concurrent access.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_NO_MEMORY Not enough resources.
 */
rtems_status_code rtems_media_server_initialize(
  rtems_task_priority priority,
  size_t stack_size,
  rtems_mode modes,
  rtems_attribute attributes
);

/**
 * @brief Sends an event message to the media server.
 *
 * @see See rtems_media_post_event().
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_NO_MEMORY Not enough resources to notify the media server.
 * @retval RTEMS_NOT_CONFIGURED Media server is not initialized.
 */
rtems_status_code rtems_media_server_post_event(
  rtems_media_event event,
  const char *src,
  rtems_media_worker worker,
  void *worker_arg
);

/**
 * @brief See rtems_media_server_post_event().
 */
static inline rtems_status_code rtems_media_server_disk_attach(
  const char *driver_name,
  rtems_media_worker worker,
  void *worker_arg
)
{
  return rtems_media_server_post_event(
    RTEMS_MEDIA_EVENT_DISK_ATTACH,
    driver_name,
    worker,
    worker_arg
  );
}

/**
 * @brief See rtems_media_server_post_event().
 */
static inline rtems_status_code rtems_media_server_disk_detach(
  const char *disk_path
)
{
  return rtems_media_server_post_event(
    RTEMS_MEDIA_EVENT_DISK_DETACH,
    disk_path,
    NULL,
    NULL
  );
}

/** @} */

/**
 * @name Path Construction
 */
/**@{**/

/**
 * @brief Creates a new path as "prefix/name-major".
 *
 * @return New string, or @c NULL if no memory is available.
 */
char *rtems_media_create_path(
  const char *prefix,
  const char *name,
  rtems_device_major_number major
);

/**
 * @brief Replaces the prefix of the @a path with @a new_prefix.
 *
 * The prefix is everything up to the last '/'.
 *
 * @return New string, or @c NULL if no memory is available.
 */
char *rtems_media_replace_prefix(const char *new_prefix, const char *path);

/**
 * @brief Appends the @a minor number to the @a path resulting in "path-minor".
 *
 * @return New string, or @c NULL if no memory is available.
 */
char *rtems_media_append_minor(
  const char *path,
  rtems_device_minor_number minor
);

/** @} */

/**
 * @name Support
 */
/**@{**/

/**
 * @brief Returns the device identifier for the device located at
 * @a device_path in @a device_identifier.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID No device at this path.
 */
rtems_status_code rtems_media_get_device_identifier(
  const char *device_path,
  dev_t *device_identifier
);

const char *rtems_media_event_description(rtems_media_event event);

const char *rtems_media_state_description(rtems_media_state state);

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTEMS_MEDIA_H */

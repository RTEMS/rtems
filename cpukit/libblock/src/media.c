/**
 * @file
 *
 * @ingroup RTEMSMedia
 *
 * @brief Media implementation.
 */

/*
 * Copyright (c) 2009-2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <string.h>
#include <assert.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/bdbuf.h>
#include <rtems/blkdev.h>
#include <rtems/bdpart.h>
#include <rtems/libio.h>

#include <rtems/media.h>

typedef struct {
  rtems_bdpart_partition *partitions;
  size_t *count;
} partition_table;

typedef struct {
  size_t index;
  rtems_blkdev_bnum begin;
  rtems_blkdev_bnum count;
} partition;

typedef struct media_item {
  rtems_chain_node node;
  struct media_item *parent;
  char *disk_path;
  char *mount_path;
} media_item;

typedef struct listener_item {
  rtems_chain_node node;
  rtems_media_listener listener;
  void *listener_arg;
} listener_item;

static RTEMS_CHAIN_DEFINE_EMPTY(listener_item_chain);

static RTEMS_CHAIN_DEFINE_EMPTY(media_item_chain);

static rtems_id media_mutex = RTEMS_ID_NONE;

static rtems_status_code lock(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_semaphore_obtain(media_mutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL) {
    sc = RTEMS_NOT_CONFIGURED;
  }

  return sc;
}

static void unlock(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_semaphore_release(media_mutex);
  assert(sc == RTEMS_SUCCESSFUL);
}

static listener_item *find_listener(
  rtems_media_listener listener,
  void *listener_arg
)
{
  rtems_chain_node *node = rtems_chain_first(&listener_item_chain);

  while (!rtems_chain_is_tail(&listener_item_chain, node)) {
    listener_item *item = (listener_item *) node;

    if (item->listener == listener && item->listener_arg == listener_arg) {
      return item;
    }

    node = rtems_chain_next(node);
  }

  return NULL;
}

rtems_status_code rtems_media_listener_add(
  rtems_media_listener listener,
  void *listener_arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = lock();
  if (sc == RTEMS_SUCCESSFUL) {
    listener_item *item = find_listener(listener, listener_arg);

    if (item == NULL) {
      item = malloc(sizeof(*item));
      if (item != NULL) {
        item->listener = listener;
        item->listener_arg = listener_arg;
        rtems_chain_append_unprotected(&listener_item_chain, &item->node);
      } else {
        sc = RTEMS_NO_MEMORY;
      }
    } else {
      sc = RTEMS_TOO_MANY;
    }

    unlock();
  }

  return sc;
}

rtems_status_code rtems_media_listener_remove(
  rtems_media_listener listener,
  void *listener_arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = lock();
  if (sc == RTEMS_SUCCESSFUL) {
    listener_item *item = find_listener(listener, listener_arg);

    if (item != NULL) {
      rtems_chain_extract_unprotected(&item->node);
      free(item);
    } else {
      sc = RTEMS_INVALID_ID;
    }

    unlock();
  }

  return sc;
}

static rtems_status_code notify(
  rtems_media_event event,
  rtems_media_state state,
  const char *src,
  const char *dest
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_status_code rsc = RTEMS_SUCCESSFUL;
  rtems_chain_node *node = rtems_chain_first(&listener_item_chain);

  while (!rtems_chain_is_tail(&listener_item_chain, node)) {
    listener_item *item = (listener_item *) node;

    sc = (*item->listener)(event, state, src, dest, item->listener_arg);
    if (sc != RTEMS_SUCCESSFUL) {
      rsc = sc;
    }

    node = rtems_chain_next(node);
  }

  return rsc;
}

static void error(
  rtems_media_state state,
  const char *src,
  const char *dest
)
{
  notify(RTEMS_MEDIA_EVENT_ERROR, state, src, dest);
}

static media_item *get_media_item(
  const char *disk_path,
  const char *mount_path
)
{
  rtems_chain_node *node = rtems_chain_first(&media_item_chain);

  while (!rtems_chain_is_tail(&media_item_chain, node)) {
    media_item *item = (media_item *) node;

    if (
      (disk_path == NULL || strcmp(disk_path, item->disk_path) == 0)
        && (mount_path == NULL || strcmp(mount_path, item->mount_path) == 0)
    ) {
      return item;
    }

    node = rtems_chain_next(node);
  }

  return NULL;
}

static void free_item(media_item *item)
{
  rtems_chain_extract(&item->node);
  free(item->mount_path);
  free(item);
}

static void create_item(
  media_item *parent,
  const char *disk_path,
  const char *mount_path
)
{
  size_t disk_path_size = strlen(disk_path) + 1;
  media_item *item = malloc(sizeof(*item) + disk_path_size);

  if (item != NULL) {
    if (mount_path != NULL) {
      item->mount_path = strdup(mount_path);

      if (item->mount_path == NULL) {
        free(item);

        return;
      }
    } else {
      item->mount_path = NULL;
    }

    item->parent = parent;
    item->disk_path = (char *) item + sizeof(*item);
    memcpy(item->disk_path, disk_path, disk_path_size);
    rtems_chain_append(&media_item_chain, &item->node);
  }
}

static void remove_mount_point(const char *mount_path)
{
  media_item *item = get_media_item(NULL, mount_path);

  if (item != NULL) {
    free(item->mount_path);
    item->mount_path = NULL;
  } else {
    error(RTEMS_MEDIA_ERROR_MOUNT_POINT_UNKNOWN, mount_path, NULL);
  }
}

static void remove_partition(const char *partition_path)
{
  media_item *item = get_media_item(partition_path, NULL);

  if (item != NULL) {
    if (item->mount_path != NULL) {
      error(
        RTEMS_MEDIA_ERROR_PARTITION_DETACH_WITH_MOUNT,
        partition_path,
        item->mount_path
      );
    }
    free_item(item);
  } else {
    error(RTEMS_MEDIA_ERROR_PARTITION_UNKNOWN, partition_path, NULL);
  }
}

static void remove_disk(const char *disk_path)
{
  media_item *item = get_media_item(disk_path, NULL);

  if (item != NULL) {
    rtems_chain_node *node = rtems_chain_first(&media_item_chain);

    while (!rtems_chain_is_tail(&media_item_chain, node)) {
      media_item *child = (media_item *) node;

      node = rtems_chain_next(node);

      if (child->parent == item) {
        if (child->mount_path != NULL) {
          error(
            RTEMS_MEDIA_ERROR_MOUNT_POINT_ORPHAN,
            child->mount_path,
            disk_path
          );
        }
        error(RTEMS_MEDIA_ERROR_PARTITION_ORPHAN, child->disk_path, disk_path);
        free_item(child);
      }
    }

    free_item(item);
  } else {
    error(RTEMS_MEDIA_ERROR_DISK_UNKNOWN, disk_path, NULL);
  }
}

static void add_disk(const char *disk_path)
{
  media_item *item = get_media_item(disk_path, NULL);

  if (item != NULL) {
    error(RTEMS_MEDIA_ERROR_DISK_EXISTS, disk_path, NULL);
    remove_disk(disk_path);
  }

  create_item(NULL, disk_path, NULL);
}

static void add_partition(const char *disk_path, const char *partition_path)
{
  media_item *item = get_media_item(partition_path, NULL);
  media_item *parent = get_media_item(disk_path, NULL);

  if (item != NULL) {
    error(RTEMS_MEDIA_ERROR_DISK_OR_PARTITION_EXISTS, partition_path, NULL);
    remove_disk(partition_path);
  }

  if (parent != NULL) {
    create_item(parent, partition_path, NULL);
  } else {
    error(
      RTEMS_MEDIA_ERROR_PARTITION_WITH_UNKNOWN_DISK,
      partition_path,
      disk_path
    );
  }
}

static void add_mount_point(const char *disk_path, const char *mount_path)
{
  media_item *item = get_media_item(disk_path, NULL);

  if (item != NULL) {
    if (item->mount_path != NULL) {
      error(RTEMS_MEDIA_ERROR_MOUNT_POINT_EXISTS, item->mount_path, NULL);
      free(item->mount_path);
    }
    item->mount_path = strdup(mount_path);
  } else {
    error(RTEMS_MEDIA_ERROR_DISK_OR_PARTITION_UNKNOWN, disk_path, NULL);
  }
}

static bool is_add_state(rtems_media_state state)
{
  return state == RTEMS_MEDIA_STATE_SUCCESS;
}

static bool is_remove_state(rtems_media_state state)
{
  return state == RTEMS_MEDIA_STATE_SUCCESS
    || state == RTEMS_MEDIA_STATE_FAILED;
}

static rtems_status_code remember_event(
  rtems_media_event event,
  rtems_media_state state,
  const char *src,
  const char *dest
)
{
  switch (event) {
    case RTEMS_MEDIA_EVENT_DISK_ATTACH:
      if (is_add_state(state)) {
        add_disk(dest);
      }
      break;
    case RTEMS_MEDIA_EVENT_PARTITION_ATTACH:
      if (is_add_state(state)) {
        add_partition(src, dest);
      }
      break;
    case RTEMS_MEDIA_EVENT_MOUNT:
      if (is_add_state(state)) {
        add_mount_point(src, dest);
      }
      break;
    case RTEMS_MEDIA_EVENT_UNMOUNT:
      if (is_remove_state(state)) {
        remove_mount_point(src);
      }
      break;
    case RTEMS_MEDIA_EVENT_PARTITION_DETACH:
      if (is_remove_state(state)) {
        remove_partition(src);
      }
      break;
    case RTEMS_MEDIA_EVENT_DISK_DETACH:
      if (is_remove_state(state)) {
        remove_disk(src);
      }
      break;
    default:
      break;
  }

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code process_event(
  rtems_media_event event,
  const char *src,
  char **dest_ptr,
  rtems_media_worker worker,
  void *worker_arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_media_state state = RTEMS_MEDIA_STATE_FAILED;
  char *dest = NULL;

  sc = notify(event, RTEMS_MEDIA_STATE_INQUIRY, src, NULL);
  if (sc == RTEMS_SUCCESSFUL) {
    state = RTEMS_MEDIA_STATE_READY;
  } else {
    state = RTEMS_MEDIA_STATE_ABORTED;
  }

  sc = (*worker)(state, src, &dest, worker_arg);
  if (state == RTEMS_MEDIA_STATE_READY) {
    if (sc == RTEMS_SUCCESSFUL) {
      state = RTEMS_MEDIA_STATE_SUCCESS;
    } else {
      state = RTEMS_MEDIA_STATE_FAILED;
    }
  }

  notify(event, state, src, dest);
  remember_event(event, state, src, dest);

  if (state == RTEMS_MEDIA_STATE_SUCCESS) {
    sc = RTEMS_SUCCESSFUL;
  } else if (state == RTEMS_MEDIA_STATE_ABORTED) {
    sc = RTEMS_UNSATISFIED;
  } else {
    sc = RTEMS_IO_ERROR;
  }

  if (dest_ptr != NULL && sc == RTEMS_SUCCESSFUL) {
    *dest_ptr = dest;
  } else {
    free(dest);
  }

  return sc;
}

static rtems_status_code mount_worker(
  rtems_media_state state,
  const char *src,
  char **dest,
  void *worker_arg
)
{
  int rv = 0;

  if (state == RTEMS_MEDIA_STATE_READY) {
    char *mount_path = NULL;

    if (worker_arg == NULL) {
      mount_path = rtems_media_replace_prefix(RTEMS_MEDIA_MOUNT_BASE, src);
    } else {
      mount_path = strdup(worker_arg);
    }

    if (mount_path == NULL) {
      return RTEMS_IO_ERROR;
    }

    rv = rtems_mkdir(mount_path, S_IRWXU | S_IRWXG | S_IRWXO);
    if (rv != 0) {
      free(mount_path);

      return RTEMS_IO_ERROR;
    }

    rv = mount(
      src,
      mount_path,
      RTEMS_FILESYSTEM_TYPE_DOSFS,
      RTEMS_FILESYSTEM_READ_WRITE,
      NULL
    );
    if (rv != 0) {
      rmdir(mount_path);
      free(mount_path);

      return RTEMS_IO_ERROR;
    }

    *dest = mount_path;
  }

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code do_mount(
  const char *src,
  char **dest_ptr,
  rtems_media_worker worker,
  void *worker_arg
)
{
  if (worker == NULL) {
    worker = mount_worker;
  }

  return process_event(
    RTEMS_MEDIA_EVENT_MOUNT,
    src,
    dest_ptr,
    worker,
    worker_arg
  );
}

static rtems_status_code do_partition_attach(
  const char *src,
  char **dest_ptr,
  rtems_media_worker worker,
  void *worker_arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  char *part_path = NULL;

  if (worker != NULL) {
    sc = process_event(
      RTEMS_MEDIA_EVENT_PARTITION_ATTACH,
      src,
      &part_path,
      worker,
      worker_arg
    );

    if (sc == RTEMS_SUCCESSFUL) {
      sc = do_mount(part_path, NULL, NULL, NULL);
    }
  } else {
    sc = RTEMS_INVALID_ADDRESS;
  }

  if (dest_ptr != NULL && sc == RTEMS_SUCCESSFUL) {
    *dest_ptr = part_path;
  } else {
    free(part_path);
  }

  return sc;
}

static rtems_status_code partition_attach_worker(
  rtems_media_state state,
  const char *src,
  char **dest,
  void *worker_arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (state == RTEMS_MEDIA_STATE_READY) {
    partition *part = worker_arg;
    char *part_path = rtems_media_append_minor(src, part->index);

    if (part_path == NULL) {
      return RTEMS_IO_ERROR;
    }

    sc = rtems_blkdev_create_partition(
      part_path,
      src,
      part->begin,
      part->count
    );
    if (sc != RTEMS_SUCCESSFUL) {
      free(part_path);

      return RTEMS_IO_ERROR;
    }

    *dest = part_path;
  }

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code attach_and_mount_partitions(
  const char *disk_path,
  const rtems_bdpart_partition *partitions,
  size_t count
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  size_t i = 0;

  for (i = 0; i < count; ++i) {
    partition part_desc = {
      .index = i,
      .begin = partitions [i].begin,
      .count = partitions [i].end - partitions [i].begin
    };
    char *part_path = NULL;

    sc = process_event(
      RTEMS_MEDIA_EVENT_PARTITION_ATTACH,
      disk_path,
      &part_path,
      partition_attach_worker,
      &part_desc
    );

    if (sc == RTEMS_SUCCESSFUL) {
      sc = do_mount(part_path, NULL, NULL, NULL);
    }

    free(part_path);
  }

  return sc;
}

static rtems_status_code partition_inquiry_worker(
  rtems_media_state state,
  const char *src,
  char **dest __attribute__((unused)),
  void *worker_arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (state == RTEMS_MEDIA_STATE_READY) {
    partition_table *pt = worker_arg;
    rtems_bdpart_format format;

    sc = rtems_bdpart_read(src, &format, pt->partitions, pt->count);
    if (sc != RTEMS_SUCCESSFUL || *pt->count == 0) {
      return RTEMS_IO_ERROR;
    }
  }

  return RTEMS_SUCCESSFUL;
}

static rtems_status_code do_partition_inquiry(
  const char *src,
  char **dest_ptr,
  rtems_media_worker worker,
  void *worker_arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (worker == NULL) {
    rtems_bdpart_partition partitions [RTEMS_BDPART_PARTITION_NUMBER_HINT];
    size_t count = RTEMS_BDPART_PARTITION_NUMBER_HINT;
    partition_table pt = {
     .partitions = partitions,
     .count = &count
    };

    sc = process_event(
      RTEMS_MEDIA_EVENT_PARTITION_INQUIRY,
      src,
      dest_ptr,
      partition_inquiry_worker,
      &pt
    );

    if (sc == RTEMS_SUCCESSFUL) {
      sc = attach_and_mount_partitions(src, partitions, count);
    }
  } else {
    sc = process_event(
      RTEMS_MEDIA_EVENT_PARTITION_INQUIRY,
      src,
      dest_ptr,
      worker,
      worker_arg
    );
  }

  return sc;
}

static rtems_status_code do_disk_attach(
  const char *src,
  char **dest_ptr,
  rtems_media_worker worker,
  void *worker_arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_status_code rsc = RTEMS_SUCCESSFUL;
  char *disk_path = NULL;

  if (worker != NULL) {
    rsc = process_event(
      RTEMS_MEDIA_EVENT_DISK_ATTACH,
      src,
      &disk_path,
      worker,
      worker_arg
    );
    
    if (rsc == RTEMS_SUCCESSFUL) {
      sc = do_mount(disk_path, NULL, NULL, NULL);

      if (sc != RTEMS_SUCCESSFUL) {
        do_partition_inquiry(disk_path, NULL, NULL, NULL);
      }
    }
  } else {
    rsc = RTEMS_INVALID_ADDRESS;
  }

  if (dest_ptr != NULL && rsc == RTEMS_SUCCESSFUL) {
    *dest_ptr = disk_path;
  } else {
    free(disk_path);
  }

  return rsc;
}

static rtems_status_code unmount_worker(
  rtems_media_state state,
  const char *src,
  char **dest __attribute__((unused)),
  void *worker_arg __attribute__((unused))
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (state == RTEMS_MEDIA_STATE_READY) {
    int rv = unmount(src);

    if (rv == 0) {
      rv = rmdir(src);
      if (rv != 0) {
        sc = RTEMS_IO_ERROR;
      }
    } else {
      sc = RTEMS_IO_ERROR;
    }
  }

  return sc;
}

static rtems_status_code do_unmount(
  const char *src,
  char **dest_ptr,
  rtems_media_worker worker,
  void *worker_arg
)
{
  if (worker == NULL) {
    worker = unmount_worker;
    worker_arg = NULL;
  }

  return process_event(
    RTEMS_MEDIA_EVENT_UNMOUNT,
    src,
    dest_ptr,
    worker,
    worker_arg
  );
}

static rtems_status_code disk_detach_worker(
  rtems_media_state state,
  const char *src,
  char **dest __attribute__((unused)),
  void *worker_arg __attribute__((unused))
)
{
  rtems_status_code rsc = RTEMS_SUCCESSFUL;

  if (state == RTEMS_MEDIA_STATE_READY) {
    int rv = unlink(src);

    if (rv != 0) {
      rsc = RTEMS_IO_ERROR;
    }
  }

  return rsc;
}

static rtems_status_code detach_item(rtems_media_event event, media_item *item)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_status_code rsc = RTEMS_SUCCESSFUL;

  if (item->mount_path != NULL) {
    sc = do_unmount(item->mount_path, NULL, NULL, NULL);
    if (sc != RTEMS_SUCCESSFUL) {
      rsc = RTEMS_IO_ERROR;
    }
  }

  sc = process_event(event, item->disk_path, NULL, disk_detach_worker, NULL);
  if (sc != RTEMS_SUCCESSFUL) {
    rsc = RTEMS_IO_ERROR;
  }

  return rsc;
}

static rtems_status_code detach_parent_item(media_item *parent)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_status_code rsc = RTEMS_SUCCESSFUL;

  rtems_chain_node *node = rtems_chain_first(&media_item_chain);

  while (!rtems_chain_is_tail(&media_item_chain, node)) {
    media_item *child = (media_item *) node;

    node = rtems_chain_next(node);

    if (child->parent == parent) {
      sc = detach_item(RTEMS_MEDIA_EVENT_PARTITION_DETACH, child);
      if (sc != RTEMS_SUCCESSFUL) {
        rsc = RTEMS_IO_ERROR;
      }
    }
  }

  sc = detach_item(RTEMS_MEDIA_EVENT_DISK_DETACH, parent);
  if (sc != RTEMS_SUCCESSFUL) {
    rsc = RTEMS_IO_ERROR;
  }

  return rsc;
}

static rtems_status_code do_disk_detach(
  const char *src,
  char **dest_ptr,
  rtems_media_worker worker,
  void *worker_arg
)
{
  if (worker == NULL) {
    media_item *parent = get_media_item(src, NULL);

    if (parent != NULL) {
      return detach_parent_item(parent);
    }

    worker = disk_detach_worker;
    worker_arg = NULL;
  }

  return process_event(
    RTEMS_MEDIA_EVENT_DISK_DETACH,
    src,
    dest_ptr,
    worker,
    worker_arg
  );
}

static rtems_status_code do_partition_detach(
  const char *src,
  char **dest_ptr,
  rtems_media_worker worker,
  void *worker_arg
)
{
  if (worker == NULL) {
    media_item *item = get_media_item(src, NULL);

    if (item != NULL) {
      return detach_item(RTEMS_MEDIA_EVENT_PARTITION_DETACH, item);
    }

    worker = disk_detach_worker;
    worker_arg = NULL;
  }

  return process_event(
    RTEMS_MEDIA_EVENT_PARTITION_DETACH,
    src,
    dest_ptr,
    worker,
    worker_arg
  );
}

rtems_status_code rtems_media_post_event(
  rtems_media_event event,
  const char *src,
  char **dest_ptr,
  rtems_media_worker worker,
  void *worker_arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = lock();
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  switch (event) {
    case RTEMS_MEDIA_EVENT_DISK_ATTACH:
      sc = do_disk_attach(src, dest_ptr, worker, worker_arg);
      break;
    case RTEMS_MEDIA_EVENT_DISK_DETACH:
      sc = do_disk_detach(src, dest_ptr, worker, worker_arg);
      break;
    case RTEMS_MEDIA_EVENT_MOUNT:
      sc = do_mount(src, dest_ptr, worker, worker_arg);
      break;
    case RTEMS_MEDIA_EVENT_UNMOUNT:
      sc = do_unmount(src, dest_ptr, worker, worker_arg);
      break;
    case RTEMS_MEDIA_EVENT_PARTITION_INQUIRY:
      sc = do_partition_inquiry(src, dest_ptr, worker, worker_arg);
      break;
    case RTEMS_MEDIA_EVENT_PARTITION_ATTACH:
      sc = do_partition_attach(src, dest_ptr, worker, worker_arg);
      break;
    case RTEMS_MEDIA_EVENT_PARTITION_DETACH:
      sc = do_partition_detach(src, dest_ptr, worker, worker_arg);
      break;
    default:
      sc = RTEMS_INVALID_ID;
      break;
  }

  unlock();

  return sc;
}

rtems_status_code rtems_media_initialize(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (media_mutex == RTEMS_ID_NONE) {
    sc = rtems_semaphore_create(
      rtems_build_name('M', 'D', 'I', 'A'),
      1,
      RTEMS_LOCAL | RTEMS_PRIORITY
        | RTEMS_INHERIT_PRIORITY | RTEMS_BINARY_SEMAPHORE,
      0,
      &media_mutex
    );
    if (sc != RTEMS_SUCCESSFUL) {
      sc = RTEMS_NO_MEMORY;
    }
  }

  return sc;
}

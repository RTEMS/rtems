/**
 * @file
 *
 * @ingroup rtems_disk
 *
 * @brief Block device disk management implementation.
 */

/*
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * Copyright (c) 2009-2012 embedded brains GmbH.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/diskdevs.h>
#include <rtems/blkdev.h>
#include <rtems/bdbuf.h>

#define DISKTAB_INITIAL_SIZE 8

/* Table of disk devices having the same major number */
typedef struct rtems_disk_device_table {
  rtems_disk_device **minor; /* minor-indexed disk device table */
  rtems_device_minor_number size; /* Number of entries in the table */
} rtems_disk_device_table;

/* Pointer to [major].minor[minor] indexed array of disk devices */
static rtems_disk_device_table *disktab;

/* Number of allocated entries in disktab table */
static rtems_device_major_number disktab_size;

/* Mutual exclusion semaphore for disk devices table */
static rtems_id diskdevs_mutex;

/* diskdevs data structures protection flag.
 * Normally, only table lookup operations performed. It is quite fast, so
 * it is possible to done lookups when interrupts are disabled, avoiding
 * obtaining the semaphore. This flags sets immediately after entering in
 * mutex-protected section and cleared before leaving this section in
 * "big" primitives like add/delete new device etc. Lookup function first
 * disable interrupts and check this flag. If it is set, lookup function
 * will be blocked on semaphore and lookup operation will be performed in
 * semaphore-protected code. If it is not set (very-very frequent case),
 * we can do lookup safely, enable interrupts and return result.
 */
static volatile bool diskdevs_protected;

static rtems_status_code
disk_lock(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_semaphore_obtain(diskdevs_mutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  if (sc == RTEMS_SUCCESSFUL) {
    diskdevs_protected = true;

    return RTEMS_SUCCESSFUL;
  } else {
    return RTEMS_NOT_CONFIGURED;
  }
}

static void
disk_unlock(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  diskdevs_protected = false;

  sc = rtems_semaphore_release(diskdevs_mutex);
  if (sc != RTEMS_SUCCESSFUL) {
    /* FIXME: Error number */
    rtems_fatal_error_occurred(0xdeadbeef);
  }
}

static rtems_disk_device *
get_disk_entry(dev_t dev, bool lookup_only)
{
  rtems_device_major_number major = 0;
  rtems_device_minor_number minor = 0;

  rtems_filesystem_split_dev_t(dev, major, minor);

  if (major < disktab_size && disktab != NULL) {
    rtems_disk_device_table *dtab = disktab + major;

    if (minor < dtab->size && dtab->minor != NULL) {
      rtems_disk_device *dd = dtab->minor [minor];

      if (dd != NULL && !lookup_only) {
        if (!dd->deleted) {
          ++dd->uses;
        } else {
          dd = NULL;
        }
      }

      return dd;
    }
  }

  return NULL;
}

static rtems_disk_device **
create_disk_table_entry(dev_t dev)
{
  rtems_device_major_number major = 0;
  rtems_device_minor_number minor = 0;

  rtems_filesystem_split_dev_t(dev, major, minor);

  if (major >= disktab_size) {
    rtems_disk_device_table *table = disktab;
    rtems_device_major_number old_size = disktab_size;
    rtems_device_major_number new_size = 2 * old_size;

    if (major >= new_size) {
      new_size = major + 1;
    }

    table = realloc(table, new_size * sizeof(*table));
    if (table == NULL) {
      return NULL;
    }

    memset(table + old_size, 0, (new_size - old_size) * sizeof(*table));
    disktab = table;
    disktab_size = new_size;
  }

  if (disktab [major].minor == NULL || minor >= disktab[major].size) {
    rtems_disk_device **table = disktab [major].minor;
    rtems_device_minor_number old_size = disktab [major].size;
    rtems_device_minor_number new_size = 0;

    if (old_size == 0) {
      new_size = DISKTAB_INITIAL_SIZE;
    } else {
      new_size = 2 * old_size;
    }
    if (minor >= new_size) {
      new_size = minor + 1;
    }

    table = realloc(table, new_size * sizeof(*table));
    if (table == NULL) {
      return NULL;
    }

    memset(table + old_size, 0, (new_size - old_size) * sizeof(*table));
    disktab [major].minor = table;
    disktab [major].size = new_size;
  }

  return disktab [major].minor + minor;
}

static rtems_status_code
create_disk(
  dev_t dev,
  const char *name,
  rtems_disk_device **dd_ptr,
  char **alloc_name_ptr
)
{
  rtems_disk_device **dd_entry = create_disk_table_entry(dev);
  rtems_disk_device *dd = NULL;
  char *alloc_name = NULL;

  if (dd_entry == NULL) {
    return RTEMS_NO_MEMORY;
  }

  if (*dd_entry != NULL) {
    return RTEMS_RESOURCE_IN_USE;
  }

  dd = malloc(sizeof(*dd));
  if (dd == NULL) {
    return RTEMS_NO_MEMORY;
  }

  if (name != NULL) {
    alloc_name = strdup(name);

    if (alloc_name == NULL) {
      free(dd);

      return RTEMS_NO_MEMORY;
    }
  }

  if (name != NULL) {
    if (mknod(alloc_name, 0777 | S_IFBLK, dev) < 0) {
      free(alloc_name);
      free(dd);
      return RTEMS_UNSATISFIED;
    }
  }

  *dd_entry = dd;
  *dd_ptr = dd;
  *alloc_name_ptr = alloc_name;

  return RTEMS_SUCCESSFUL;
}

static int null_handler(
  rtems_disk_device *dd,
  uint32_t req,
  void *argp
)
{
  return -1;
}

rtems_status_code rtems_disk_create_phys(
  dev_t dev,
  uint32_t block_size,
  rtems_blkdev_bnum block_count,
  rtems_block_device_ioctl handler,
  void *driver_data,
  const char *name
)
{
  rtems_disk_device *dd = NULL;
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  char *alloc_name = NULL;

  if (handler == NULL) {
    return RTEMS_INVALID_ADDRESS;
  }

  sc = disk_lock();
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  sc = create_disk(dev, name, &dd, &alloc_name);
  if (sc != RTEMS_SUCCESSFUL) {
    disk_unlock();

    return sc;
  }

  sc = rtems_disk_init_phys(
    dd,
    block_size,
    block_count,
    handler,
    driver_data
  );

  dd->dev = dev;
  dd->name = alloc_name;

  if (sc != RTEMS_SUCCESSFUL) {
    dd->ioctl = null_handler;
    rtems_disk_delete(dev);
    disk_unlock();

    return sc;
  }

  disk_unlock();

  return RTEMS_SUCCESSFUL;
}

static bool
is_physical_disk(const rtems_disk_device *dd)
{
  return dd->phys_dev == dd;
}

rtems_status_code rtems_disk_create_log(
  dev_t dev,
  dev_t phys,
  rtems_blkdev_bnum block_begin,
  rtems_blkdev_bnum block_count,
  const char *name
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_disk_device *phys_dd = NULL;
  rtems_disk_device *dd = NULL;
  char *alloc_name = NULL;

  sc = disk_lock();
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  phys_dd = get_disk_entry(phys, true);
  if (phys_dd == NULL) {
    disk_unlock();

    return RTEMS_INVALID_ID;
  }

  sc = create_disk(dev, name, &dd, &alloc_name);
  if (sc != RTEMS_SUCCESSFUL) {
    disk_unlock();

    return sc;
  }

  sc = rtems_disk_init_log(
    dd,
    phys_dd,
    block_begin,
    block_count
  );

  dd->dev = dev;
  dd->name = alloc_name;

  ++phys_dd->uses;

  if (sc != RTEMS_SUCCESSFUL) {
    dd->ioctl = null_handler;
    rtems_disk_delete(dev);
    disk_unlock();

    return sc;
  }

  disk_unlock();

  return RTEMS_SUCCESSFUL;
}

static void
free_disk_device(rtems_disk_device *dd)
{
  if (is_physical_disk(dd)) {
    (*dd->ioctl)(dd, RTEMS_BLKIO_DELETED, NULL);
  }
  if (dd->name != NULL) {
    unlink(dd->name);
    free(dd->name);
  }
  free(dd);
}

static void
rtems_disk_cleanup(rtems_disk_device *disk_to_remove)
{
  rtems_disk_device *const physical_disk = disk_to_remove->phys_dev;
  rtems_device_major_number major = 0;
  rtems_device_minor_number minor = 0;

  if (physical_disk->deleted) {
    dev_t dev = physical_disk->dev;
    unsigned deleted_count = 0;

    for (major = 0; major < disktab_size; ++major) {
      rtems_disk_device_table *dtab = disktab + major;

      for (minor = 0; minor < dtab->size; ++minor) {
        rtems_disk_device *dd = dtab->minor [minor];

        if (dd != NULL && dd->phys_dev->dev == dev && dd != physical_disk) {
          if (dd->uses == 0) {
            ++deleted_count;
            dtab->minor [minor] = NULL;
            free_disk_device(dd);
          } else {
            dd->deleted = true;
          }
        }
      }
    }

    physical_disk->uses -= deleted_count;
    if (physical_disk->uses == 0) {
      rtems_filesystem_split_dev_t(physical_disk->dev, major, minor);
      disktab [major].minor [minor] = NULL;
      free_disk_device(physical_disk);
    }
  } else {
    if (disk_to_remove->uses == 0) {
      --physical_disk->uses;
      rtems_filesystem_split_dev_t(disk_to_remove->dev, major, minor);
      disktab [major].minor [minor] = NULL;
      free_disk_device(disk_to_remove);
    }
  }
}

rtems_status_code
rtems_disk_delete(dev_t dev)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_disk_device *dd = NULL;

  sc = disk_lock();
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  dd = get_disk_entry(dev, true);
  if (dd == NULL) {
    disk_unlock();

    return RTEMS_INVALID_ID;
  }

  dd->deleted = true;
  rtems_disk_cleanup(dd);

  disk_unlock();

  return RTEMS_SUCCESSFUL;
}

rtems_disk_device *
rtems_disk_obtain(dev_t dev)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_disk_device *dd = NULL;
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  if (!diskdevs_protected) {
    /* Frequent and quickest case */
    dd = get_disk_entry(dev, false);
    rtems_interrupt_enable(level);
  } else {
    rtems_interrupt_enable(level);

    sc = disk_lock();
    if (sc == RTEMS_SUCCESSFUL) {
      dd = get_disk_entry(dev, false);
      disk_unlock();
    }
  }

  return dd;
}

rtems_status_code
rtems_disk_release(rtems_disk_device *dd)
{
  rtems_interrupt_level level;
  dev_t dev = dd->dev;
  unsigned uses = 0;
  bool deleted = false;

  rtems_interrupt_disable(level);
  uses = --dd->uses;
  deleted = dd->deleted;
  rtems_interrupt_enable(level);

  if (uses == 0 && deleted) {
    rtems_disk_delete(dev);
  }

  return RTEMS_SUCCESSFUL;
}

rtems_disk_device *
rtems_disk_next(dev_t dev)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_disk_device_table *dtab = NULL;
  rtems_device_major_number major = 0;
  rtems_device_minor_number minor = 0;

  if (dev != (dev_t) -1) {
    rtems_filesystem_split_dev_t(dev, major, minor);

    /* If minor wraps around */
    if ((minor + 1) < minor) {
      /* If major wraps around */
      if ((major + 1) < major) {
        return NULL;
      }
      ++major;
      minor = 0;
    } else {
      ++minor;
    }
  }

  sc = disk_lock();
  if (sc != RTEMS_SUCCESSFUL) {
    return NULL;
  }

  if (major >= disktab_size) {
    disk_unlock();

    return NULL;
  }

  dtab = disktab + major;
  while (true) {
    if (dtab->minor == NULL || minor >= dtab->size) {
       minor = 0;
       ++major;
       if (major >= disktab_size) {
         disk_unlock();

         return NULL;
       }
       dtab = disktab + major;
    } else if (dtab->minor [minor] == NULL) {
      ++minor;
    } else {
      ++dtab->minor [minor]->uses;
      disk_unlock();

      return dtab->minor [minor];
    }
  }
}

rtems_status_code
rtems_disk_io_initialize(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_device_major_number size = DISKTAB_INITIAL_SIZE;

  if (disktab_size > 0) {
    return RTEMS_SUCCESSFUL;
  }

  disktab = calloc(size, sizeof(rtems_disk_device_table));
  if (disktab == NULL) {
    return RTEMS_NO_MEMORY;
  }

  diskdevs_protected = false;
  sc = rtems_semaphore_create(
    rtems_build_name('D', 'D', 'E', 'V'),
    1,
    RTEMS_FIFO | RTEMS_BINARY_SEMAPHORE | RTEMS_NO_INHERIT_PRIORITY
      | RTEMS_NO_PRIORITY_CEILING | RTEMS_LOCAL,
    0,
    &diskdevs_mutex
  );
  if (sc != RTEMS_SUCCESSFUL) {
    free(disktab);

    return RTEMS_NO_MEMORY;
  }

  sc = rtems_bdbuf_init();
  if (sc != RTEMS_SUCCESSFUL) {
    rtems_semaphore_delete(diskdevs_mutex);
    free(disktab);

    return RTEMS_UNSATISFIED;
  }

  disktab_size = size;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code
rtems_disk_io_done(void)
{
  rtems_device_major_number major = 0;
  rtems_device_minor_number minor = 0;

  for (major = 0; major < disktab_size; ++major) {
    rtems_disk_device_table *dtab = disktab + major;

    for (minor = 0; minor < dtab->size; ++minor) {
      rtems_disk_device *dd = dtab->minor [minor];

      if (dd != NULL) {
        free_disk_device(dd);
      }
    }
    free(dtab->minor);
  }
  free(disktab);

  rtems_semaphore_delete(diskdevs_mutex);

  diskdevs_mutex = RTEMS_ID_NONE;
  disktab = NULL;
  disktab_size = 0;

  return RTEMS_SUCCESSFUL;
}

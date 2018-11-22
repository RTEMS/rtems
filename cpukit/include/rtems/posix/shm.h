/**
 * @file
 *
 * @brief Internal Support for POSIX Shared Memory
 */

/*
 * Copyright (c) 2016 Gedare Bloom.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_SHM_H
#define _RTEMS_POSIX_SHM_H

#include <rtems/score/objectdata.h>
#include <rtems/score/threadq.h>

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup POSIXShmPrivate POSIX Shared Memory Private Support
 *
 * @ingroup POSIXAPI
 *
 * Internal implementation support for POSIX shared memory.
 * @{
 */
typedef struct POSIX_Shm_Object_operations POSIX_Shm_Object_operations;
extern const POSIX_Shm_Object_operations _POSIX_Shm_Object_operations;

/**
 * @brief Encapsulation for the storage and manipulation of shm objects.
 */
typedef struct {
  /**
   * @brief The handle is private for finding object storage.
   */
  void                               *handle;

  /**
   * @brief The number of bytes allocated to the object. A size of 0 with
   * a handle of NULL means no object is allocated.
   */
  size_t                              size;

  /**
   * @brief Implementation-specific operations on shm objects.
   */
  const POSIX_Shm_Object_operations  *ops;
} POSIX_Shm_Object;

/**
 * @brief Operations on POSIX Shared Memory Objects.
 */
struct POSIX_Shm_Object_operations {
  /**
   * @brief Allocates a new @a shm_obj with initial @a size.
   *
   * New shared memory is initialized to zeroes.
   *
   * Returns 0 for success.
   */
  int ( *object_create ) ( POSIX_Shm_Object *shm_obj, size_t size );

  /**
   * @brief Changes the @a shm_obj size to @a size.
   *
   * Zeroes out the portion of the shared memory object that shrinks or grows.
   *
   * Returns 0 for success.
   */
  int ( *object_resize ) ( POSIX_Shm_Object *shm_obj, size_t size );

  /**
   * @brief Deletes the @a shm_obj.
   *
   * Zeroes out the memory.
   *
   * Returns 0 for success.
   */
  int ( *object_delete ) ( POSIX_Shm_Object *shm_obj );

  /**
   * @brief Copies up to @count bytes of the @a shm_obj data into @a buf.
   *
   * Returns the number of bytes read (copied) into @a buf.
   */
  int ( *object_read ) ( POSIX_Shm_Object *shm_obj, void *buf, size_t count );

  /**
   * @brief Maps a shared memory object.
   *
   * Establishes a memory mapping between the shared memory object and the
   * caller.
   *
   * Returns the mapped address of the object.
   */
  void * ( *object_mmap ) ( POSIX_Shm_Object *shm_obj, size_t len, int prot, off_t off);
};

/**
 * @brief Control for a POSIX Shared Memory Object
 */
typedef struct {
   Objects_Control      Object;
   Thread_queue_Control Wait_queue;

   int                  reference_count;

   POSIX_Shm_Object     shm_object;

   uid_t                uid;
   gid_t                gid;
   mode_t               mode;
   int                  oflag;

   time_t               atime;
   time_t               mtime;
   time_t               ctime;
} POSIX_Shm_Control;

/**
 * @brief The POSIX Shared Memory objects information.
 */
extern Objects_Information _POSIX_Shm_Information;

/**
 * @brief Macro to define the objects information for the POSIX Shared Memory
 * objects.
 *
 * This macro should only be used by <rtems/confdefs.h>.
 *
 * @param max The configured object maximum (the OBJECTS_UNLIMITED_OBJECTS flag
 * may be set).
 */
#define POSIX_SHM_INFORMATION_DEFINE( max ) \
  OBJECTS_INFORMATION_DEFINE( \
    _POSIX_Shm, \
    OBJECTS_POSIX_API, \
    OBJECTS_POSIX_SHMS, \
    POSIX_Shm_Control, \
    max, \
    _POSIX_PATH_MAX, \
    NULL \
  )

/**
 * @brief object_create operation for shm objects stored in RTEMS Workspace.
 */
extern int _POSIX_Shm_Object_create_from_workspace(
  POSIX_Shm_Object *shm_obj,
  size_t size
);

/**
 * @brief object_delete operation for shm objects stored in RTEMS Workspace.
 */
extern int _POSIX_Shm_Object_delete_from_workspace( POSIX_Shm_Object *shm_obj );

/**
 * @brief object_resize operation for shm objects stored in RTEMS Workspace.
 */
extern int _POSIX_Shm_Object_resize_from_workspace(
  POSIX_Shm_Object *shm_obj,
  size_t size
);

/**
 * @brief object_read operation for shm objects stored in RTEMS Workspace.
 */
extern int _POSIX_Shm_Object_read_from_workspace(
  POSIX_Shm_Object *shm_obj,
  void *buf,
  size_t count
);

/**
 * @brief object_mmap operation for shm objects stored in RTEMS Workspace.
 */
extern void * _POSIX_Shm_Object_mmap_from_workspace(
  POSIX_Shm_Object *shm_obj,
  size_t len,
  int prot,
  off_t off
);

/**
 * @brief object_create operation for shm objects stored in C program heap.
 */
extern int _POSIX_Shm_Object_create_from_heap(
  POSIX_Shm_Object *shm_obj,
  size_t size
);

/**
 * @brief object_delete operation for shm objects stored in C program heap.
 */
extern int _POSIX_Shm_Object_delete_from_heap( POSIX_Shm_Object *shm_obj );

/**
 * @brief object_resize operation for shm objects stored in C program heap.
 */
extern int _POSIX_Shm_Object_resize_from_heap(
  POSIX_Shm_Object *shm_obj,
  size_t size
);

/**
 * @brief object_read operation for shm objects stored in C program heap.
 */
extern int _POSIX_Shm_Object_read_from_heap(
  POSIX_Shm_Object *shm_obj,
  void *buf,
  size_t count
);

/**
 * @brief object_mmap operation for shm objects stored in C program heap.
 */
extern void * _POSIX_Shm_Object_mmap_from_heap(
  POSIX_Shm_Object *shm_obj,
  size_t len,
  int prot,
  off_t off
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

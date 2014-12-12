/**
 * @file
 * 
 * @brief Standard C Library Support
 * 
 * This include file contains the information regarding the
 * RTEMS specific support for the standard C library.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_LIBCSUPPORT_H
#define _RTEMS_RTEMS_LIBCSUPPORT_H

#include <sys/types.h>
#include <stdint.h>

#include <rtems/score/heap.h>
#include <rtems/rtems/tasks.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup libcsupport Standard C Library Support
 *
 * @brief RTEMS Specific Support for the Standard C Library
 *
 */
/**@{**/

extern void malloc_dump(void);

/**
 * @brief Malloc walk.
 */
extern bool malloc_walk(int source, bool printf_enabled);

/**
 * @brief Set malloc heap pointer.
 * 
 * This routine is primarily used for debugging. 
 */
void malloc_set_heap_pointer(Heap_Control *new_heap);

/**
 * @brief Get malloc heap pointer.
 * 
 * This routine is primarily used for debugging. 
 */
Heap_Control *malloc_get_heap_pointer( void );
extern void libc_init(void);
extern int  host_errno(void);
extern void fix_syscall_errno(void);

/**
 * @brief Get free malloc information.
 * 
 * Find amount of free heap remaining
 */
extern size_t malloc_free_space(void);

/**
 * @brief Get malloc status information.
 * 
 * Find amount of free heap remaining.
 */
extern int malloc_info(Heap_Information_block *the_info);

/*
 *  Prototypes required to install newlib reentrancy user extension
 */
bool newlib_create_hook(
  rtems_tcb *current_task,
  rtems_tcb *creating_task
);

#define __RTEMS_NEWLIB_BEGIN 0

void newlib_terminate_hook(
  rtems_tcb *current_task
);

#define RTEMS_NEWLIB_EXTENSION \
{ \
  newlib_create_hook,     /* rtems_task_create  */ \
  0,                      /* rtems_task_start   */ \
  0,                      /* rtems_task_restart */ \
  0,                      /* rtems_task_delete  */ \
  0,                      /* task_switch  */ \
  __RTEMS_NEWLIB_BEGIN,   /* task_begin   */ \
  0,                      /* task_exitted */ \
  0,                      /* fatal        */ \
  newlib_terminate_hook   /* thread terminate */ \
}

typedef struct {
  uint32_t active_barriers;
  uint32_t active_extensions;
  uint32_t active_message_queues;
  uint32_t active_partitions;
  uint32_t active_periods;
  uint32_t active_ports;
  uint32_t active_regions;
  uint32_t active_semaphores;
  uint32_t active_tasks;
  uint32_t active_timers;
} rtems_resource_rtems_api;

typedef struct {
  uint32_t active_barriers;
  uint32_t active_condition_variables;
  uint32_t active_message_queues;
  uint32_t active_message_queue_descriptors;
  uint32_t active_mutexes;
  uint32_t active_rwlocks;
  uint32_t active_semaphores;
  uint32_t active_spinlocks;
  uint32_t active_threads;
  uint32_t active_timers;
} rtems_resource_posix_api;

typedef struct {
  Heap_Information_block workspace_info;
  Heap_Information_block heap_info;
  uint32_t active_posix_key_value_pairs;
  uint32_t active_posix_keys;
  rtems_resource_rtems_api rtems_api;
  rtems_resource_posix_api posix_api;
  int open_files;
} rtems_resource_snapshot;

/**
 * @brief Tasks a snapshot of the resource usage of the system.
 *
 * @param[out] snapshot The snapshot of used resources.
 *
 * @see rtems_resource_snapshot_equal() and rtems_resource_snapshot_check().
 *
 * @code
 * #include <assert.h>
 *
 * #include <rtems/libcsupport.h>
 *
 * void example(void)
 * {
 *   rtems_resource_snapshot before;
 *
 *   test_setup();
 *   rtems_resource_snapshot_take(&before);
 *   test();
 *   assert(rtems_resource_snapshot_check(&before));
 *   test_cleanup();
 * }
 * @endcode
 */
void rtems_resource_snapshot_take(rtems_resource_snapshot *snapshot);

/**
 * @brief Compares two resource snapshots for equality.
 *
 * @param[in] a One resource snapshot.
 * @param[in] b Another resource snapshot.
 *
 * @retval true The resource snapshots are equal.
 * @retval false Otherwise.
 *
 * @see rtems_resource_snapshot_take().
 */
bool rtems_resource_snapshot_equal(
  const rtems_resource_snapshot *a,
  const rtems_resource_snapshot *b
);

/**
 * @brief Takes a new resource snapshot and checks that it is equal to the
 * given resource snapshot.
 *
 * @param[in] snapshot The resource snapshot used for comparison with the new
 * resource snapshot.
 *
 * @retval true The resource snapshots are equal.
 * @retval false Otherwise.
 *
 * @see rtems_resource_snapshot_take().
 */
bool rtems_resource_snapshot_check(const rtems_resource_snapshot *snapshot);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

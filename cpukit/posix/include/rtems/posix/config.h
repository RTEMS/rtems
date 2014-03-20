/**
 * @file
 *
 * @brief User Defined Configuration Parameters Specific For The POSIX API
 * 
 * This include file contains the table of user defined configuration
 * parameters specific for the POSIX API.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_CONFIG_H
#define _RTEMS_POSIX_CONFIG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ClassicConfig Configuration
 *
 *  @ingroup ClassicRTEMS
 *
 *  This encapsulates functionality related to the application's configuration
 *  of the Classic API including the maximum number of each class of objects.
 */
/**@{*/

/**
 *  For now, we are only allowing the user to specify the entry point
 *  and stack size for POSIX initialization threads.
 */
typedef struct {
  /** This is the entry point for a POSIX initialization thread. */
  void       *(*thread_entry)(void *);
  /** This is the stack size for a POSIX initialization thread. */
  int       stack_size;
} posix_initialization_threads_table;

/**
 *  The following records define the POSIX Configuration Table.
 *  The information contained in this table is required in all
 *  RTEMS systems which include POSIX threads support, whether
 *  single or multiprocessor.  This table primarily defines the
 *  following:
 *
 *     + required number of each object type
 */
typedef struct {
  /**
   * This field contains the maximum number of POSIX API
   * threads which are configured for this application.
   */
  uint32_t                            maximum_threads;

  /**
   * This field contains the maximum number of POSIX API
   * mutexes which are configured for this application.
   */
  uint32_t                            maximum_mutexes;

  /**
   * This field contains the maximum number of POSIX API
   * condition variables which are configured for this application.
   */
  uint32_t                            maximum_condition_variables;

  /**
   * This field contains the maximum number of POSIX API
   * timers which are configured for this application.
   */
  uint32_t                            maximum_timers;

  /**
   * This field contains the maximum number of POSIX API
   * queued signals which are configured for this application.
   */
  uint32_t                            maximum_queued_signals;

  /**
   * This field contains the maximum number of POSIX API
   * message queues which are configured for this application.
   */
  uint32_t                            maximum_message_queues;

  /**
   * This field contains the maximum number of POSIX API
   * message queue file descriptors which are configured
   * for this application.
   *
   * @note There can be one or more file descriptors used with
   *       each message queue. This value should be greater than
   *       or equal to the number of message queues.
   */
  uint32_t                            maximum_message_queue_descriptors;

  /**
   * This field contains the maximum number of POSIX API
   * semaphores which are configured for this application.
   */
  uint32_t                            maximum_semaphores;

  /**
   * This field contains the maximum number of POSIX API
   * barriers which are configured for this application.
   */
  uint32_t                            maximum_barriers;

  /**
   * This field contains the maximum number of POSIX API
   * read/write locks which are configured for this application.
   */
  uint32_t                            maximum_rwlocks;

  /**
   * This field contains the maximum number of POSIX API
   * spinlocks which are configured for this application.
   */
  uint32_t                            maximum_spinlocks;

  /**
   * This field contains the number of POSIX API Initialization
   * threads listed in @a User_initialization_thread_table.
   */
  uint32_t                            number_of_initialization_threads;

  /**
   * This field contains the list of POSIX API Initialization threads.
   */
  posix_initialization_threads_table *User_initialization_threads_table;
} posix_api_configuration_table;

/**
 * @brief POSIX API configuration table.
 *
 * This is the POSIX API Configuration Table expected to be generated
 * by confdefs.h.
 */
extern posix_api_configuration_table Configuration_POSIX_API;

/**@}*/
#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

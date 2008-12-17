/**
 * @file rtems/posix/config.h
 *
 *  This include file contains the table of user defined configuration
 *  parameters specific for the POSIX API.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_POSIX_CONFIG_H
#define _RTEMS_POSIX_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  XXX
 *
 *  The following records define the Configuration Table.  The
 *  information contained in this table is required in all
 *  RTEMS systems, whether single or multiprocessor.  This
 *  table primarily defines the following:
 *
 *     + required number of each object type
 */

/*
 *  For now, we are only allowing the user to specify the entry point
 *  and stack size for POSIX initialization threads.
 */

typedef struct {
  void       *(*thread_entry)(void *);
  int       stack_size;
} posix_initialization_threads_table;

typedef struct {
  int                                 maximum_threads;
  int                                 maximum_mutexes;
  int                                 maximum_condition_variables;
  int                                 maximum_keys;
  int                                 maximum_timers;
  int                                 maximum_queued_signals;
  int                                 maximum_message_queues;
  int                                 maximum_semaphores;
  int                                 maximum_barriers;
  int                                 maximum_rwlocks;
  int                                 maximum_spinlocks;
  int                                 number_of_initialization_threads;
  posix_initialization_threads_table *User_initialization_threads_table;
} posix_api_configuration_table;

/**
 *  @brief POSIX API Configuration Table
 *
 *  This is the POSIX API Configuration Table expected to be generated
 *  by confdefs.h.
 */
extern posix_api_configuration_table Configuration_POSIX_API;

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

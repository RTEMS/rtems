/*  config.h
 *
 *  This include file contains the table of user defined configuration
 *  parameters specific for the POSIX API.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __RTEMS_POSIX_CONFIGURATION_h
#define __RTEMS_POSIX_CONFIGURATION_h

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
 *  for posix initialization threads.
 */

typedef struct {
  void       *(*thread_entry)(void *);
} posix_initialization_threads_table;

typedef struct {
  int                                 maximum_threads;
  int                                 maximum_mutexes;
  int                                 maximum_condition_variables;
  int                                 maximum_keys;
  int                                 maximum_timers;
  int                                 maximum_queued_signals;
  int                                 number_of_initialization_threads;
  posix_initialization_threads_table *User_initialization_threads_table;
} posix_api_configuration_table;

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

/*  config.h
 *
 *  This include file contains the table of user defined configuration
 *  parameters.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __RTEMS_CONFIGURATION_h
#define __RTEMS_CONFIGURATION_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/object.h>
#include <rtems/thread.h>

/*
 *  The following records define the Initialization Tasks Table.
 *  Each entry contains the information required by RTEMS to
 *  create and start a user task automatically at executive
 *  initialization time.
 */

typedef struct {
  Objects_Name          name;              /* task name */
  unsigned32            stack_size;        /* task stack size */
  rtems_task_priority   initial_priority;  /* task priority */
  rtems_attribute       attribute_set;     /* task attributes */
  rtems_task_entry      entry_point;       /* task entry point */
  rtems_mode            mode_set;          /* task initial mode */
  unsigned32            argument;          /* task argument */
} rtems_initialization_tasks_table;

/*
 *
 *  The following defines the types for:
 *
 *    + major and minor numbers
 *    + the return type of a device driver entry
 *    + a pointer to a device driver entry
 *    + an entry in the the Device Driver Address Table.  Each entry in this
 *      table corresponds to an application provided device driver and
 *      defines the entry points for that device driver.
 */

typedef unsigned32 rtems_device_major_number;
typedef unsigned32 rtems_device_minor_number;

typedef rtems_status_code rtems_device_driver;

typedef rtems_device_driver ( *rtems_device_driver_entry )(
                 rtems_device_major_number,
                 rtems_device_minor_number,
                 void *
             );

typedef struct {
  rtems_device_driver_entry initialization; /* initialization procedure */
  rtems_device_driver_entry open;           /* open request procedure */
  rtems_device_driver_entry close;          /* close request procedure */
  rtems_device_driver_entry read;           /* read request procedure */
  rtems_device_driver_entry write;          /* write request procedure */
  rtems_device_driver_entry control;        /* special functions procedure */
}   rtems_driver_address_table;

/*
 *  The following records defines the User Extension Table.
 *  This table defines the application dependent routines which
 *  are invoked at critical points in the life of each task and
 *  the system as a whole.
 */

typedef void rtems_extension;

typedef rtems_extension ( *rtems_task_create_extension )(
                 rtems_tcb *,
                 rtems_tcb *
             );

typedef rtems_extension ( *rtems_task_delete_extension )(
                 rtems_tcb *,
                 rtems_tcb *
             );

typedef rtems_extension ( *rtems_task_start_extension )(
                 rtems_tcb *,
                 rtems_tcb *
             );

typedef rtems_extension ( *rtems_task_restart_extension )(
                 rtems_tcb *,
                 rtems_tcb *
             );

typedef rtems_extension ( *rtems_task_switch_extension )(
                 rtems_tcb *,
                 rtems_tcb *
             );

typedef rtems_extension ( *rtems_task_begin_extension )(
                 rtems_tcb *
             );

typedef rtems_extension ( *rtems_task_exitted_extension )(
                 rtems_tcb *
             );

typedef rtems_extension ( *rtems_fatal_extension )(
                 unsigned32
             );

typedef struct {
  rtems_task_create_extension  rtems_task_create;
  rtems_task_start_extension   rtems_task_start;
  rtems_task_restart_extension rtems_task_restart;
  rtems_task_delete_extension  rtems_task_delete;
  rtems_task_switch_extension  task_switch;
  rtems_task_begin_extension   task_begin;
  rtems_task_exitted_extension task_exitted;
  rtems_fatal_extension        fatal;
}   rtems_extensions_table;

/*
 *  The following records define the Multiprocessor Communications
 *  Interface (MPCI) Table.  This table defines the user-provided
 *  MPCI which is a required part of a multiprocessor RTEMS system.
 *
 *  For non-blocking local operations that become remote operations,
 *  we need a timeout.  This is a per-driver timeout: default_timeout
 */

/* XXX FORWARD REFERENCES */

typedef struct Configuration_Table    rtems_configuration_table;
typedef struct Configuration_Table_MP rtems_multiprocessing_table;

typedef void rtems_mpci_entry;

typedef rtems_mpci_entry ( *rtems_mpci_initialization_entry )(
                 rtems_configuration_table *,
                 rtems_cpu_table *,
                 rtems_multiprocessing_table *
             );

typedef rtems_mpci_entry ( *rtems_mpci_get_packet_entry )(
                 rtems_packet_prefix **
             );

typedef rtems_mpci_entry ( *rtems_mpci_return_packet_entry )(
                 rtems_packet_prefix *
             );

typedef rtems_mpci_entry ( *rtems_mpci_send_entry )(
                 unsigned32,
                 rtems_packet_prefix *
             );

typedef rtems_mpci_entry ( *rtems_mpci_receive_entry )(
                 rtems_packet_prefix **
             );

typedef struct {
  unsigned32                      default_timeout;        /* in ticks */
  unsigned32                      maximum_packet_size;
  rtems_mpci_initialization_entry initialization;
  rtems_mpci_get_packet_entry     get_packet;
  rtems_mpci_return_packet_entry  return_packet;
  rtems_mpci_send_entry           send_packet;
  rtems_mpci_receive_entry        receive_packet;
} rtems_mpci_table;

/*
 *  The following records define the Multiprocessor Configuration
 *  Table.  This table defines the multiprocessor system
 *  characteristics which must be known by RTEMS in a multiprocessor
 *  system.
 */

struct Configuration_Table_MP {
  unsigned32  node;                   /* local node number */
  unsigned32  maximum_nodes;          /* maximum # nodes in system */
  unsigned32  maximum_global_objects; /* maximum # global objects */
  unsigned32  maximum_proxies;        /* maximum # proxies */
  rtems_mpci_table *User_mpci_table;  /* pointer to MPCI table */
};

/*
 *  The following records define the Configuration Table.  The
 *  information contained in this table is required in all
 *  RTEMS systems, whether single or multiprocessor.  This
 *  table primarily defines the following:
 *
 *     + location and size of the RTEMS Workspace
 *     + required number of each object type
 *     + microseconds per clock tick
 *     + clock ticks per task timeslice
 */

struct Configuration_Table {
  void                             *work_space_start;
  unsigned32                        work_space_size;
  unsigned32                        maximum_tasks;
  unsigned32                        maximum_timers;
  unsigned32                        maximum_semaphores;
  unsigned32                        maximum_message_queues;
  unsigned32                        maximum_partitions;
  unsigned32                        maximum_regions;
  unsigned32                        maximum_ports;
  unsigned32                        maximum_periods;
  unsigned32                        maximum_extensions;
  unsigned32                        microseconds_per_tick;
  unsigned32                        ticks_per_timeslice;
  unsigned32                        number_of_initialization_tasks;
  rtems_initialization_tasks_table *User_initialization_tasks_table;
  unsigned32                        number_of_device_drivers;
  rtems_driver_address_table       *Device_driver_table;
  rtems_extensions_table           *User_extension_table;
  rtems_multiprocessing_table      *User_multiprocessing_table;
};

/*
 *  The following defines the default Multiprocessing Configuration
 *  Table.  This table is used in a single processor system.
 */

extern const rtems_multiprocessing_table
  _Configuration_Default_multiprocessing_table;

/*
 *  The following define the internal pointers to the user's
 *  configuration information.
 */

EXTERN rtems_configuration_table   *_Configuration_Table;
EXTERN rtems_multiprocessing_table *_Configuration_MP_table;
EXTERN rtems_mpci_table            *_Configuration_MPCI_table;

/*
 *
 *  _Configuration_Handler_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this handler.
 */

STATIC INLINE void _Configuration_Handler_initialization(
  rtems_configuration_table   *configuration_table,
  rtems_multiprocessing_table *multiprocessing_table,
  rtems_mpci_table            *users_mpci_table
);

/*
 *  _Configuration_Is_multiprocessing
 *
 *  DESCRIPTION:
 *
 *  This function determines if a multiprocessing application has been
 *  configured, if so, TRUE is returned, otherwise FALSE is returned.
 */

STATIC INLINE boolean _Configuration_Is_multiprocessing( void );

/*
 *  _Configuration_Is_null_driver_address_table_pointer
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_table is NULL and FALSE otherwise.
 */

STATIC INLINE boolean _Configuration_Is_null_driver_address_table_pointer(
  rtems_driver_address_table *the_table
);

/*
 *  _Configuration_Is_null_extension_table_pointer
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_table is NULL and FALSE otherwise.
 */

STATIC INLINE boolean _Configuration_Is_null_extension_table_pointer(
  rtems_extensions_table *the_table
);

/*
 *  _Configuration_Is_null_initialization_tasks_table_pointer
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_table is NULL and FALSE otherwise.
 */

STATIC INLINE boolean
    _Configuration_Is_null_initialization_tasks_table_pointer(
  rtems_initialization_tasks_table *the_table
);

#include <rtems/config.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

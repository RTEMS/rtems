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

#include <rtems/posix/config.h>
#include <rtems/rtems/config.h>

/*  XXX <rtems/rtems/config.h> should cover these
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>
#include <rtems/rtems/tasks.h>
*/

#include <rtems/extension.h>
#include <rtems/io.h>
#include <rtems/score/mpci.h>

/*
 *  The following records define the Multiprocessor Configuration
 *  Table.  This table defines the multiprocessor system
 *  characteristics which must be known by RTEMS in a multiprocessor
 *  system.
 */

typedef struct {
  unsigned32    node;                   /* local node number */
  unsigned32    maximum_nodes;          /* maximum # nodes in system */
  unsigned32    maximum_global_objects; /* maximum # global objects */
  unsigned32    maximum_proxies;        /* maximum # proxies */
  MPCI_Control *User_mpci_table;        /* pointer to MPCI table */
} rtems_multiprocessing_table;

/*
 *  The following records define the Configuration Table.  The
 *  information contained in this table is required in all
 *  RTEMS systems, whether single or multiprocessor.  This
 *  table primarily defines the following:
 *
 *     + location and size of the RTEMS Workspace
 *     + microseconds per clock tick
 *     + clock ticks per task timeslice
 *     + required number of each object type for each API configured
 */

typedef struct {
  void                             *work_space_start;
  unsigned32                        work_space_size;
  unsigned32                        maximum_extensions;
  unsigned32                        microseconds_per_tick;
  unsigned32                        ticks_per_timeslice;
  unsigned32                        maximum_devices;
  unsigned32                        number_of_device_drivers;
  rtems_driver_address_table       *Device_driver_table;
  rtems_extensions_table           *User_extension_table;
  rtems_multiprocessing_table      *User_multiprocessing_table;
  rtems_api_configuration_table    *RTEMS_api_configuration;
  posix_api_configuration_table    *Posix_api_configuration;
} rtems_configuration_table;

/*
 *  The following are provided strictly for the convenience of
 *  the user.  They are not used in RTEMS itself.
 */

EXTERN rtems_configuration_table    *_Configuration_Table;
EXTERN rtems_multiprocessing_table  *_Configuration_MP_table;

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

/*
 *  Some basic filesystem types
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __rtems_fstypes_h
#define __rtems_fstypes_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  File descriptor Table Information
 */

/* Forward declarations */

/* FIXME: shouldn't this better not be here? */ 
typedef struct rtems_libio_tt rtems_libio_t;

typedef struct rtems_filesystem_location_info_tt
    rtems_filesystem_location_info_t;

struct rtems_filesystem_mount_table_entry_tt;
typedef struct rtems_filesystem_mount_table_entry_tt
    rtems_filesystem_mount_table_entry_t;

typedef struct _rtems_filesystem_file_handlers_r 
    rtems_filesystem_file_handlers_r;
typedef struct _rtems_filesystem_operations_table 
    rtems_filesystem_operations_table;

/*
 * Structure used to determine a location/filesystem in the tree.
 */
  
struct rtems_filesystem_location_info_tt
{
   void                                 *node_access;
   rtems_filesystem_file_handlers_r     *handlers;
   rtems_filesystem_operations_table	*ops;
   rtems_filesystem_mount_table_entry_t *mt_entry;
};

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

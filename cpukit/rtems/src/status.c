/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/rtems/status.h>

/**
 *  This array is used to map SuperCore Object Handler return
 *  codes to Classic API status codes.
 */

const rtems_status_code _Status_Object_name_errors_to_status[] = {
  /** This maps OBJECTS_SUCCESSFUL to RTEMS_SUCCESSFUL. */
  RTEMS_SUCCESSFUL,
  /** This maps OBJECTS_INVALID_NAME to RTEMS_INVALID_NAME. */
  RTEMS_INVALID_NAME,
  /** This maps OBJECTS_INVALID_ADDRESS to RTEMS_INVALID_NAME. */
  RTEMS_INVALID_ADDRESS,
  /** This maps OBJECTS_INVALID_ID to RTEMS_INVALID_ADDRESS. */
  RTEMS_INVALID_ID,
  /** This maps OBJECTS_INVALID_NODE to RTEMS_INVALID_NODE. */
  RTEMS_INVALID_NODE
};

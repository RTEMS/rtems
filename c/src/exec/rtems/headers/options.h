/*  options.h
 *
 *  This include file contains information which defines the
 *  options available on many directives.
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

#ifndef __RTEMS_OPTIONS_h
#define __RTEMS_OPTIONS_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  The following type defines the control block used to manage
 *  option sets.
 */

typedef unsigned32 rtems_option;

/*
 *  The following constants define the individual options which may
 *  be used to compose an option set.
 */

#define RTEMS_DEFAULT_OPTIONS   0x00000000

#define RTEMS_WAIT      0x00000000        /* wait on resource */
#define RTEMS_NO_WAIT   0x00000001        /* do not wait on resource */

#define RTEMS_EVENT_ALL 0x00000000        /* wait for all events */
#define RTEMS_EVENT_ANY 0x00000002        /* wait on any event */

/*
 *  _Options_Is_no_wait
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the RTEMS_NO_WAIT option is enabled in
 *  option_set, and FALSE otherwise.
 *
 */

STATIC INLINE boolean _Options_Is_no_wait (
  rtems_option option_set
);

/*
 *  _Options_Is_any
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the RTEMS_EVENT_ANY option is enabled in
 *  OPTION_SET, and FALSE otherwise.
 *
 */

STATIC INLINE boolean _Options_Is_any (
  rtems_option option_set
);

#include <rtems/options.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

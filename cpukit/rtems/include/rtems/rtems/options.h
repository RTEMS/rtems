/*  options.h
 *
 *  This include file contains information which defines the
 *  options available on many directives.
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

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/options.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

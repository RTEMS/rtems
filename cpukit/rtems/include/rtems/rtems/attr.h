/*  attr.h
 *
 *  This include file contains all information about the Object Attributes
 *  Handler.
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

#ifndef __RTEMS_ATTRIBUTES_h
#define __RTEMS_ATTRIBUTES_h

#ifdef __cplusplus
extern "C" {
#endif

/* types */

typedef unsigned32 rtems_attribute;

/* constants */

#define RTEMS_DEFAULT_ATTRIBUTES  0x00000000

#define RTEMS_NO_FLOATING_POINT   0x00000000 /* don't use FP HW */
#define RTEMS_FLOATING_POINT      0x00000001 /* utilize coprocessor */

#define RTEMS_LOCAL               0x00000000 /* local resource */
#define RTEMS_GLOBAL              0x00000002 /* global resource */

#define RTEMS_FIFO                0x00000000 /* process RTEMS_FIFO */
#define RTEMS_PRIORITY            0x00000004 /* process by priority */

#define RTEMS_SEMAPHORE_CLASS         0x00000030 /* mask */
#define RTEMS_COUNTING_SEMAPHORE      0x00000000
#define RTEMS_BINARY_SEMAPHORE        0x00000010
#define RTEMS_SIMPLE_BINARY_SEMAPHORE 0x00000020

#define RTEMS_NO_INHERIT_PRIORITY 0x00000000
#define RTEMS_INHERIT_PRIORITY    0x00000040

#define RTEMS_NO_PRIORITY_CEILING 0x00000000
#define RTEMS_PRIORITY_CEILING    0x00000080

#define RTEMS_APPLICATION_TASK    0x00000000
#define RTEMS_SYSTEM_TASK         0x00000100


#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
#define ATTRIBUTES_NOT_SUPPORTED       0
#else
#define ATTRIBUTES_NOT_SUPPORTED       RTEMS_FLOATING_POINT
#endif

#if ( CPU_ALL_TASKS_ARE_FP == TRUE )
#define ATTRIBUTES_REQUIRED            RTEMS_FLOATING_POINT
#else
#define ATTRIBUTES_REQUIRED            0
#endif

/*
 *  _Attributes_Handler_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs initialization for this handler.
 *
 *  NOTE: There is no initialization required in C.  Conditional compilation
 *        takes care of this in C.
 */

#define _Attributes_Handler_initialization()

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/attr.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

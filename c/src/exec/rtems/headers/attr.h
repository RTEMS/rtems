/*  attr.h
 *
 *  This include file contains all information about the Object Attributes
 *  Handler.
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

#ifndef __RTEMS_ATTRIBUTES_h
#define __RTEMS_ATTRIBUTES_h

#ifdef __cplusplus
extern "C" {
#endif

/* types */

typedef unsigned32 rtems_attribute;

/* constants */

#define RTEMS_DEFAULT_ATTRIBUTES            0x00000000

#define RTEMS_NO_FLOATING_POINT   0x00000000 /* don't use FP HW */
#define RTEMS_FLOATING_POINT      0x00000001 /* utilize coprocessor */

#define RTEMS_LOCAL               0x00000000 /* local resource */
#define RTEMS_GLOBAL              0x00000002 /* global resource */

#define RTEMS_FIFO                0x00000000 /* process RTEMS_FIFO */
#define RTEMS_PRIORITY            0x00000004 /* process by priority */

#define RTEMS_COUNTING_SEMAPHORE  0x00000000
#define RTEMS_BINARY_SEMAPHORE    0x00000010

#define RTEMS_NO_INHERIT_PRIORITY 0x00000000
#define RTEMS_INHERIT_PRIORITY    0x00000020

#if ( CPU_HARDWARE_FP == TRUE )
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

/*
 *  _Attributes_Set
 *
 *  DESCRIPTION:
 *
 *  This function sets the requested new_attributes in the attribute_set
 *  passed in.  The result is returned to the user.
 */

STATIC INLINE rtems_attribute _Attributes_Set (
   rtems_attribute new_attributes,
   rtems_attribute attribute_set
);

/*
 *  _Attributes_Clear
 *
 *  DESCRIPTION:
 *
 *  This function clears the requested new_attributes in the attribute_set
 *  passed in.  The result is returned to the user.
 */

STATIC INLINE rtems_attribute _Attributes_Clear (
   rtems_attribute attribute_set,
   rtems_attribute mask
);

/*
 *  _Attributes_Is_floating_point
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the floating point attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */

STATIC INLINE boolean _Attributes_Is_floating_point(
  rtems_attribute attribute_set
);

/*
 *  _Attributes_Is_global
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the global object attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */

STATIC INLINE boolean _Attributes_Is_global(
  rtems_attribute attribute_set
);

/*
 *  _Attributes_Is_priority
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the priority attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */

STATIC INLINE boolean _Attributes_Is_priority(
  rtems_attribute attribute_set
);

#if 0
/*
 *  _Attributes_Is_limit
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the limited attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */

STATIC INLINE boolean _Attributes_Is_limit(
  rtems_attribute attribute_set
);
#endif

/*
 *  _Attributes_Is_binary_semaphore
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the binary semaphore attribute is
 *  enabled in the attribute_set and FALSE otherwise.
 */

STATIC INLINE boolean _Attributes_Is_binary_semaphore(
  rtems_attribute attribute_set
);

/*
 *  _Attributes_Is_inherit_priority
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the priority inheritance attribute
 *  is enabled in the attribute_set and FALSE otherwise.
 */

STATIC INLINE boolean _Attributes_Is_inherit_priority(
  rtems_attribute attribute_set
);

#include <rtems/attr.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

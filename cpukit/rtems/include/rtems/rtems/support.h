/*  support.h
 *
 *  This include file contains information about support functions for
 *  the RTEMS API.
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

#ifndef __RTEMS_RTEMS_SUPPORT_h
#define __RTEMS_RTEMS_SUPPORT_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/types.h>

/*
 *  rtems_is_name_valid
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the name is valid, and FALSE otherwise.
 */

STATIC INLINE rtems_boolean rtems_is_name_valid (
  rtems_name  name
);

/*
 *  rtems_build_name
 *
 *  DESCRIPTION:
 *
 *  This function returns an object name composed of the four characters
 *  C1, C2, C3, and C4.
 *
 *  NOTE:
 *
 *  This must be implemented as a macro for use in Configuration Tables.
 *
 */
 
#define rtems_build_name( _C1, _C2, _C3, _C4 ) \
  ( (_C1) << 24 | (_C2) << 16 | (_C3) << 8 | (_C4) )
 
/*
 *  rtems_name_to_characters
 *
 *  DESCRIPTION:
 *
 *  This function breaks the object name into the four component
 *  characters C1, C2, C3, and C4.
 *
 */
 
STATIC INLINE void rtems_name_to_characters(
  rtems_name    name,
  char         *c1,
  char         *c2,
  char         *c3,
  char         *c4
);

#include <rtems/support.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

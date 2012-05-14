/**
 * @file
 *
 * @ingroup ClassicRTEMS
 *
 * @brief Classic API support.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
 
#ifndef _RTEMS_RTEMS_SUPPORT_H
# error "Never use <rtems/rtems/support.inl> directly; include <rtems/rtems/support.h> instead."
#endif

#ifndef _RTEMS_RTEMS_SUPPORT_INL
#define _RTEMS_RTEMS_SUPPORT_INL

/**
 * @addtogroup ClassicRTEMS
 *
 * @{
 */

/**
 * @brief Returns @c true if the name is valid, and @c false otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_is_name_valid (
  rtems_name name
)
{
  return ( name != 0 );
}

/**
 * @brief Breaks the object name into the four component characters @a c1,
 * @a c2, @a c3, and @a c4.
 */
RTEMS_INLINE_ROUTINE void rtems_name_to_characters(
  rtems_name    name,
  char         *c1,
  char         *c2,
  char         *c3,
  char         *c4
)
{
  *c1 = (char) ((name >> 24) & 0xff);
  *c2 = (char) ((name >> 16) & 0xff);
  *c3 = (char) ((name >> 8) & 0xff);
  *c4 = (char) ( name & 0xff);
}

/** @} */

#endif
/* end of include file */

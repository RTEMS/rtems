/*  support.inl
 *
 *  This include file contains the macros implementation of all
 *  of the inlined routines specific to the RTEMS API.
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
 
#ifndef __RTEMS_SUPPORT_inl
#define __RTEMS_SUPPORT_inl

/*PAGE
 *
 *  rtems_is_name_valid
 *
 */

#define rtems_is_name_valid( _name ) \
  ( (_name) != 0 )

/*PAGE
 *
 *  rtems_name_to_characters
 *
 */

#define rtems_name_to_characters( _name, _c1, _c2, _c3, _c4 ) \
  { \
    (*(_c1) = ((_name) >> 24) & 0xff; \
    (*(_c2) = ((_name) >> 16) & 0xff; \
    (*(_c3) = ((_name) >> 8) & 0xff; \
    (*(_c4) = ((_name)) & 0xff; \
  }

#endif
/* end of include file */

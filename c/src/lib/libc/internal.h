/*  internal.h
 *
 *  This include file contains internal information
 *  for the RTEMS C library support which is needed across
 *  files.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __INTERNAL_LIBC_h
#define __INTERNAL_LIBC_h

#ifdef __cplusplus
extern "C" {
#endif

void MY_task_set_note(
  rtems_tcb        *tcb,
  rtems_unsigned32 notepad,
  rtems_unsigned32 note
);

rtems_unsigned32 MY_task_get_note(
  rtems_tcb        *tcb,
  rtems_unsigned32  notepad
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

/*  internal.h
 *
 *  This include file contains internal information
 *  for the RTEMS C library support which is needed across
 *  files.
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

/*  libcsupport.h
 *
 *  This include file contains the information regarding the
 *  RTEMS specific support for the standard C library.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __LIBC_SUPPORT_h
#define __LIBC_SUPPORT_h

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

void RTEMS_Malloc_Initialize(
  void   *start,
  size_t  length,
  size_t  sbrk_amount
);

extern void malloc_dump(void);
extern void malloc_walk(size_t source, size_t printf_enabled);
extern void libc_init(int reentrant);
extern int  host_errno(void);
extern void fix_syscall_errno(void);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

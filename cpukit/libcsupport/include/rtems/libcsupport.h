/*  libcsupport.h
 *
 *  This include file contains the information regarding the
 *  RTEMS specific support for the standard C library.
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

extern void libc_init(int reentrant);

#ifdef __cplusplus
}
#endif

extern int  host_errno(void);
extern void fix_syscall_errno(void);

#endif
/* end of include file */

/**
 * @file rtems/libcsupport.h
 */

/*  libcsupport.h
 *
 *  This include file contains the information regarding the
 *  RTEMS specific support for the standard C library.
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

#ifndef _RTEMS_RTEMS_LIBCSUPPORT_H
#define _RTEMS_RTEMS_LIBCSUPPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <sys/types.h>

void RTEMS_Malloc_Initialize(
  void   *start,
  size_t  length,
  size_t  sbrk_amount
);

extern void malloc_dump(void);
extern void malloc_walk(size_t source, size_t printf_enabled);
extern void libc_init(void);
extern int  host_errno(void);
extern void fix_syscall_errno(void);
extern size_t malloc_free_space(void);
extern void open_dev_console(void);

/*
 *  Prototypes required to install newlib reentrancy user extension
 */
rtems_boolean libc_create_hook(
  rtems_tcb *current_task,
  rtems_tcb *creating_task
);

#if defined(RTEMS_UNIX) && !defined(hpux)
  rtems_extension libc_begin_hook(rtems_tcb *current_task);
  #define __RTEMS_NEWLIB_BEGIN libc_begin_hook
#else
  #define __RTEMS_NEWLIB_BEGIN 0
#endif

rtems_extension libc_delete_hook(
  rtems_tcb *current_task,
  rtems_tcb *deleted_task
);

#define RTEMS_NEWLIB_EXTENSION \
{ \
  libc_create_hook,                            /* rtems_task_create  */ \
  0,                                           /* rtems_task_start   */ \
  0,                                           /* rtems_task_restart */ \
  libc_delete_hook,                            /* rtems_task_delete  */ \
  0,                                           /* task_switch  */ \
  __RTEMS_NEWLIB_BEGIN,                        /* task_begin   */ \
  0,                                           /* task_exitted */ \
  0                                            /* fatal        */ \
}

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

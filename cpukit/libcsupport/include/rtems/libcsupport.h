/**
 * @file rtems/libcsupport.h
 *
 * This include file contains the information regarding the
 * RTEMS specific support for the standard C library.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_LIBCSUPPORT_H
#define _RTEMS_RTEMS_LIBCSUPPORT_H

#include <sys/types.h>
#include <stdint.h>

#include <rtems/score/heap.h>
#include <rtems/rtems/tasks.h>

#ifdef __cplusplus
extern "C" {
#endif

void RTEMS_Malloc_Initialize(
  void *heap_begin,
  uintptr_t heap_size,
  size_t sbrk_amount
);

extern void malloc_dump(void);
extern bool malloc_walk(int source, bool printf_enabled);
void malloc_set_heap_pointer(Heap_Control *new_heap);
Heap_Control *malloc_get_heap_pointer( void );
extern void libc_init(void);
extern int  host_errno(void);
extern void fix_syscall_errno(void);
extern size_t malloc_free_space(void);
extern void open_dev_console(void);

extern int malloc_info(Heap_Information_block *the_info);

/*
 *  Prototypes required to install newlib reentrancy user extension
 */
bool newlib_create_hook(
  rtems_tcb *current_task,
  rtems_tcb *creating_task
);

#define __RTEMS_NEWLIB_BEGIN 0

void newlib_delete_hook(
  rtems_tcb *current_task,
  rtems_tcb *deleted_task
);

#define RTEMS_NEWLIB_EXTENSION \
{ \
  newlib_create_hook,     /* rtems_task_create  */ \
  0,                      /* rtems_task_start   */ \
  0,                      /* rtems_task_restart */ \
  newlib_delete_hook,     /* rtems_task_delete  */ \
  0,                      /* task_switch  */ \
  __RTEMS_NEWLIB_BEGIN,   /* task_begin   */ \
  0,                      /* task_exitted */ \
  0                       /* fatal        */ \
}

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

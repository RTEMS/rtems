/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#ifndef __FSTEST_SUPPORT_H
#define __FSTEST_SUPPORT_H


#ifdef __cplusplus
extern "C" {
#endif

extern rtems_task Init(
  rtems_task_argument ignored
);

extern void test(void);

extern void test_initialize_filesystem(void);

extern void test_shutdown_filesystem(void);
#ifdef __cplusplus
};
#endif


#endif

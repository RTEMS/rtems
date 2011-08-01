
/*
 *
 *  $Id$
 */
#ifndef __FSTEST_SUPPORT_H
#define __FSTEST_SUPPORT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif 
#include "pmacros.h"

#include "rtems.h"

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

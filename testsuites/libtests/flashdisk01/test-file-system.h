/*
 * Copyright (c) 2010-2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef TEST_FILE_SYSTEM_H
#define TEST_FILE_SYSTEM_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef int (*test_file_system_mount_handler)(
  const char *disk_path,
  const char *mount_path,
  void *arg
);

typedef int (*test_file_system_format_handler)(
  const char *disk_path,
  void *arg
);

typedef struct {
  test_file_system_mount_handler mount;
  test_file_system_format_handler format;
} test_file_system_handler;

void test_file_system_with_handler(
  unsigned index,
  const char *disk_path,
  const char *mount_path,
  const test_file_system_handler *handler,
  void *handler_arg
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TEST_FILE_SYSTEM_H */

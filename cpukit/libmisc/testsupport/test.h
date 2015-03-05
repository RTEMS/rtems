/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_TEST_H
#define _RTEMS_TEST_H

#include <rtems.h>
#include <rtems/bspIo.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSTest Test Support
 *
 * @brief Test support functions.
 *
 * @{
 */

/**
 * @brief Each test must define a test name string.
 */
extern const char rtems_test_name[];

/**
 * @brief Fatal extension for tests.
 */
void rtems_test_fatal_extension(
  rtems_fatal_source source,
  bool is_internal,
  rtems_fatal_code code
);

/**
 * @brief Initial extension for tests.
 */
#define RTEMS_TEST_INITIAL_EXTENSION \
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL, rtems_test_fatal_extension }

/**
 * @brief Prints a begin of test message.
 *
 * @param[in] printf_func The formatted output function.
 * @param[in, out] printf_arg The formatted output function argument.
 *
 * @returns As specified by printf().
 */
int rtems_test_begin_with_plugin(
  rtems_printk_plugin_t printf_func,
  void *printf_arg
);

/**
 * @brief Prints a begin of test message using printf().
 *
 * @returns As specified by printf().
 */
static inline int rtems_test_begin(void)
{
  return rtems_test_begin_with_plugin(rtems_printf_plugin, NULL);
}

/**
 * @brief Prints a begin of test message using printk().
 *
 * @returns As specified by printf().
 */
static inline int rtems_test_begink(void)
{
  return rtems_test_begin_with_plugin(printk_plugin, NULL);
}

/**
 * @brief Prints an end of test message.
 *
 * @param[in] printf_func The formatted output function.
 * @param[in, out] printf_arg The formatted output function argument.
 *
 * @returns As specified by printf().
 */
int rtems_test_end_with_plugin(
  rtems_printk_plugin_t printf_func,
  void *printf_arg
);

/**
 * @brief Prints an end of test message using printf().
 *
 * @returns As specified by printf().
 */
static inline int rtems_test_end(void)
{
  return rtems_test_end_with_plugin(rtems_printf_plugin, NULL);
}

/**
 * @brief Prints an end of test message using printk().
 *
 * @returns As specified by printf().
 */
static inline int rtems_test_endk(void)
{
  return rtems_test_end_with_plugin(printk_plugin, NULL);
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_TEST_H */

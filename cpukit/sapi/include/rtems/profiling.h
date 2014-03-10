/**
 * @file
 *
 * @ingroup Profiling
 *
 * @brief Profiling API
 */

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
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_PROFILING_H
#define _RTEMS_PROFILING_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup Profiling Profiling Support
 *
 * @brief The profiling support offers functions to report profiling
 * information available in the system.
 *
 * Profiling support is by default disabled.  It must be enabled via the
 * configure command line with the <tt>--enable-profiling</tt> option.  In this
 * case the RTEMS_PROFILING pre-processor symbol is defined and profiling
 * statistics will be gathered during system run-time.  The profiling support
 * increases the time of critical sections and has some memory overhead.  The
 * overhead should be acceptable for most applications.  The aim of the
 * profiling implementation is to be available even for production systems so
 * that verification is simplified.
 *
 * Profiling information includes critical timing values such as the maximum
 * time of disabled thread dispatching which is a measure for the thread
 * dispatch latency.  On SMP configurations statistics of all SMP locks in the
 * system are available.
 *
 * Profiling information can be retrieved via rtems_profiling_iterate() and
 * reported as an XML dump via rtems_profiling_report_xml().  These functions
 * are always available, but actual profiling data is only available if enabled
 * at build configuration time.
 *
 * @{
 */

/**
 * @brief Type of profiling data.
 */
typedef enum {
} rtems_profiling_type;

/**
 * @brief The profiling data header.
 */
typedef struct {
  /**
   * @brief The profiling data type.
   */
  rtems_profiling_type type;
} rtems_profiling_header;

/**
 * @brief Collection of profiling data.
 */
typedef union {
  /**
   * @brief Header to specify the actual profiling data.
   */
  rtems_profiling_header header;
} rtems_profiling_data;

/**
 * @brief Visitor function for the profiling iteration.
 *
 * @param[in, out] arg The visitor argument.
 * @param[in] data The current profiling data.
 *
 * @see rtems_profiling_iterate().
 */
typedef void (*rtems_profiling_visitor)(
  void *arg,
  const rtems_profiling_data *data
);

/**
 * @brief Iterates through all profiling data of the system.
 *
 * @param[in] visitor The visitor.
 * @param[in, out] visitor_arg The visitor argument.
 */
void rtems_profiling_iterate(
  rtems_profiling_visitor visitor,
  void *visitor_arg
);

/**
 * @brief Function for formatted output.
 *
 * @param[in, out] arg Some argument.
 * @param[in] format The output format as specified by printf().
 * @param[in] ... More parameters according to format.
 *
 * @returns As specified by printf().
 *
 * @see rtems_profiling_report_xml().
 */
typedef int (*rtems_profiling_printf)(void *arg, const char *format, ...);

/**
 * @brief Reports profiling data as XML.
 *
 * @param[in] name The name of the profiling report.
 * @param[in] printf_func The formatted output function.
 * @param[in, out] printf_arg The formatted output function argument.
 * @param[in] indentation_level The current indentation level.
 * @param[in] indentation The string used for indentation.
 *
 * @returns As specified by printf().
 */
int rtems_profiling_report_xml(
  const char *name,
  rtems_profiling_printf printf_func,
  void *printf_arg,
  uint32_t indentation_level,
  const char *indentation
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_PROFILING_H */

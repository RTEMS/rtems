/*
 *  COPYRIGHT (c) 2012-2014 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems_rtl
 *
 * @brief RTEMS Run-Time Linker ELF Trace Support.
 */

#if !defined (_RTEMS_RTL_TRACE_H_)
#define _RTEMS_RTL_TRACE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdbool.h>
#include <stdint.h>

/**
 * Set to 1 to build trace support in to the RTL code.
 */
#define RTEMS_RTL_TRACE 1

/**
 * The type of the mask.
 */
typedef uint32_t rtems_rtl_trace_mask;

/**
 * List of tracing bits for the various parts of the link editor.
 */
#define RTEMS_RTL_TRACE_DETAIL                 (1UL << 0)
#define RTEMS_RTL_TRACE_WARNING                (1UL << 1)
#define RTEMS_RTL_TRACE_LOAD                   (1UL << 2)
#define RTEMS_RTL_TRACE_UNLOAD                 (1UL << 3)
#define RTEMS_RTL_TRACE_SECTION                (1UL << 4)
#define RTEMS_RTL_TRACE_SYMBOL                 (1UL << 5)
#define RTEMS_RTL_TRACE_RELOC                  (1UL << 6)
#define RTEMS_RTL_TRACE_GLOBAL_SYM             (1UL << 7)
#define RTEMS_RTL_TRACE_LOAD_SECT              (1UL << 8)
#define RTEMS_RTL_TRACE_ALLOCATOR              (1UL << 9)
#define RTEMS_RTL_TRACE_UNRESOLVED             (1UL << 10)
#define RTEMS_RTL_TRACE_CACHE                  (1UL << 11)
#define RTEMS_RTL_TRACE_ALL                    (0xffffffffUL & ~(RTEMS_RTL_TRACE_CACHE))

/**
 * Call to check if this part is bring traced. If RTEMS_RTL_TRACE is defined to
 * 0 the code is dead code elminiated when built with -Os, -O2, or higher.
 *
 * @param mask The part of the API to trace.
 * @retval true Tracing is active for the mask.
 * @retval false Do not trace.
 */
#if RTEMS_RTL_TRACE
bool rtems_rtl_trace (rtems_rtl_trace_mask mask);
#else
#define rtems_rtl_trace(_m) (0)
#endif

/**
 * Set the mask.
 *
 * @param mask The mask bits to set.
 * @return The previous mask.
 */
#if RTEMS_RTL_TRACE
rtems_rtl_trace_mask rtems_rtl_trace_set_mask (rtems_rtl_trace_mask mask);
#else
#define rtems_rtl_trace_set_mask(_m)
#endif

/**
 * Clear the mask.
 *
 * @param mask The mask bits to clear.
 * @return The previous mask.
 */
#if RTEMS_RTL_TRACE
rtems_rtl_trace_mask rtems_rtl_trace_clear_mask (rtems_rtl_trace_mask mask);
#else
#define rtems_rtl_trace_clear_mask(_m)
#endif

/**
 * Add shell trace shell command.
 */
#if RTEMS_RTL_TRACE
int rtems_rtl_trace_shell_command (int argc, char *argv[]);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

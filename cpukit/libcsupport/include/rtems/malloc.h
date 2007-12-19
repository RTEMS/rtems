/**
 * @file rtems/malloc.h
 */

/*
 *  RTEMS Malloc Extensions
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 * $ld:
 */

#ifndef _RTEMS_MALLOC_H
#define _RTEMS_MALLOC_H

#include <rtems.h>
#include <rtems/bspIo.h>

#include <stdint.h>

/*
 *  Malloc Statistics Structure
 */
typedef struct {
    uint32_t    space_available;             /* current size of malloc area */
    uint32_t    malloc_calls;                /* # calls to malloc */
    uint32_t    memalign_calls;              /* # calls to memalign */
    uint32_t    free_calls;
    uint32_t    realloc_calls;
    uint32_t    calloc_calls;
    uint32_t    max_depth;		     /* most ever malloc'd at 1 time */
    uintmax_t   lifetime_allocated;
    uintmax_t   lifetime_freed;
} rtems_malloc_statistics_t;

/*
 *  Malloc statistics plugin
 */
typedef struct {
  void (*initialize)(void);
  void (*at_malloc)(void *);
  void (*at_free)(void *);
} rtems_malloc_statististics_functions_t;

extern rtems_malloc_statististics_functions_t 
  rtems_malloc_statistics_helpers_table;
extern rtems_malloc_statististics_functions_t *rtems_malloc_statistics_helpers;

/*
 *  Malloc boundary support plugin
 */
typedef struct {
  void     (*initialize)(void);
  uint32_t (*overhead)(void);
  void     (*at_malloc)(void *, size_t);
  void     (*at_free)(void *);
  void     (*at_realloc)(void *, size_t);
} rtems_malloc_boundary_functions_t;

extern rtems_malloc_boundary_functions_t rtems_malloc_boundary_helpers_table;
extern rtems_malloc_boundary_functions_t *rtems_malloc_boundary_helpers;



/** @brief Print Malloc Statistic Usage Report
 *
 *  This method fills in the called provided malloc statistics area.
 *
 *  @return This method returns 0 if successful and -1 on error.
 */
int malloc_get_statistics(
  rtems_malloc_statistics_t *stats
);

/** @brief Print Malloc Statistic Usage Report
 *
 *  This method prints a malloc statistics report.
 *
 *  @note It uses printk to print the report.
 */
void malloc_report_statistics(void);

/** @brief Print Malloc Statistic Usage Report 
 *
 *  This method prints a malloc statistics report.
 *
 *  @param[in] context is the context to pass to the print handler 
 *  @param[in] print is the print handler 
 *
 *  @note It uses the CALLER's routine to print the report.
 */
void malloc_report_statistics_with_plugin(
  void                  *context,
  rtems_printk_plugin_t  print
);

#endif

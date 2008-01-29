/**
 * @file rtems/malloc.h
 */

/*
 *  RTEMS Malloc Extensions
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 * $Id$
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
} rtems_malloc_statistics_functions_t;

extern rtems_malloc_statistics_functions_t 
  rtems_malloc_statistics_helpers_table;
extern rtems_malloc_statistics_functions_t *rtems_malloc_statistics_helpers;

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

/*
 *  Malloc Heap Extension (sbrk) plugin
 */
typedef struct {
  void *(*initialize)(void *, size_t);
  void *(*extend)(size_t);
} rtems_malloc_sbrk_functions_t;

extern rtems_malloc_sbrk_functions_t rtems_malloc_sbrk_helpers_table;
extern rtems_malloc_sbrk_functions_t *rtems_malloc_sbrk_helpers;

/*
 * Malloc Plugin to Dirty Memory at Allocation Time
 */
typedef void (*rtems_malloc_dirtier_t)(void *, size_t);
extern rtems_malloc_dirtier_t *rtems_malloc_dirty_helper;

/** @brief Dirty memory function
 *  
 *  This method fills the specified area with a non-zero pattern
 *  to aid in debugging programs which do not initialize their
 *  memory allocated from the heap.
 */
void rtems_malloc_dirty_memory(
  void   *start,
  size_t  size
);

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

/**
 *
 *  This method is a help memalign implementation which does all
 *  error checking done by posix_memalign() EXCEPT it does NOT
 *  place numeric restrictions on the alignment value.
 *
 *  @param[in] pointer points to the user pointer
 *  @param[in] alignment is the desired alignment
 *  @param[in] size is the allocation request size in bytes
 *
 *  @return This methods returns zero on success and a POSIX errno
 *          value to indicate the failure condition.  On success
 *          *pointer will contain the address of the allocated memory.
 */
int rtems_memalign(
  void   **pointer,
  size_t   alignment,
  size_t   size
);

#endif

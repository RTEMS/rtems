/**
 * @file rtems/malloc.h
 *
 * This file defines the interface to RTEMS extensions to the Malloc Family.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_MALLOC_H
#define _RTEMS_MALLOC_H

#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/libcsupport.h> /* for malloc_walk() */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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
extern rtems_malloc_dirtier_t rtems_malloc_dirty_helper;

/**
 *  @brief Dirty memory function
 *
 *  This method fills the specified area with a non-zero pattern
 *  to aid in debugging programs which do not initialize their
 *  memory allocated from the heap.
 */
void rtems_malloc_dirty_memory(
  void   *start,
  size_t  size
);

/**
 *  @brief Print Malloc Statistic Usage Report
 *
 *  This method fills in the called provided malloc statistics area.
 *
 *  @return This method returns 0 if successful and -1 on error.
 */
int malloc_get_statistics(
  rtems_malloc_statistics_t *stats
);

/**
 *  @brief Print Malloc Statistic Usage Report
 *
 *  This method prints a malloc statistics report.
 *
 *  @note It uses printk to print the report.
 */
void malloc_report_statistics(void);

/**
 *  @brief Print Malloc Statistic Usage Report
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
 *  @brief RTEMS variation on Aligned Memory Allocation
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

/**
 * @brief Allocates a memory area of size @a size bytes from the heap.
 *
 * If the alignment parameter @a alignment is not equal to zero, the allocated
 * memory area will begin at an address aligned by this value.
 *
 * If the boundary parameter @a boundary is not equal to zero, the allocated
 * memory area will fulfill a boundary constraint.  The boundary value
 * specifies the set of addresses which are aligned by the boundary value.  The
 * interior of the allocated memory area will not contain an element of this
 * set.  The begin or end address of the area may be a member of the set.
 *
 * A size value of zero will return a unique address which may be freed with
 * free().
 *
 * The memory allocated by this function can be released with a call to free().
 *
 * @return A pointer to the begin of the allocated memory area, or @c NULL if
 * no memory is available or the parameters are inconsistent.
 */
void *rtems_heap_allocate_aligned_with_boundary(
  size_t size,
  uintptr_t alignment,
  uintptr_t boundary
);

/**
 * @brief Extends the memory available for the heap using the memory area
 * starting at @a area_begin of size @a area_size bytes.
 *
 * There are no alignment requirements.  The memory area must be big enough to
 * contain some maintainance blocks.  It must not overlap parts of the current
 * heap areas.  Disconnected subordinate heap areas will lead to used blocks
 * which cover the gaps.  Extending with an inappropriate memory area will
 * corrupt the heap.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ADDRESS Invalid memory area.
 */
rtems_status_code rtems_heap_extend(
  void *area_begin,
  uintptr_t area_size
);

/**
 * @brief Greedy allocate that empties the heap.
 *
 * Afterward the heap has at most @a block_count allocateable blocks of sizes
 * specified by @a block_sizes.  The @a block_sizes must point to an array with
 * @a block_count members.  All other blocks are used.
 *
 * @see rtems_heap_greedy_free().
 */
void *rtems_heap_greedy_allocate(
  const uintptr_t *block_sizes,
  size_t block_count
);

/**
 * @brief Frees space of a greedy allocation.
 *
 * The @a opaque argument must be the return value of
 * rtems_heap_greedy_allocate().
 */
void rtems_heap_greedy_free( void *opaque );

#ifdef __cplusplus
}
#endif

#endif

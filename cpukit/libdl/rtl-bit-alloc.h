/*
 *  COPYRIGHT (c) 2019 Chris Johns <chrisj@rtems.org>
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
 * @brief RTEMS Run-Time Linker Bit Allocator Header
 */

#if !defined (_RTEMS_RTL_BIT_ALLOC_H_)
#define _RTEMS_RTL_BIT_ALLOC_H_

#include <rtems/rtl/rtl-fwd.h>
#include <rtems/rtl/rtl-obj-fwd.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Bit Allocator data
 */
typedef struct rtems_rtl_bit_alloc
{
  uint8_t*  base;       /**< The memory being allocated. */
  size_t    size;       /**< The number of bytes of memory being managed. */
  uint32_t* bits;       /**< The bit map indicating which blocks are allocated. */
  size_t    block_size; /**< The size of a block, the minimum allocation unit. */
  size_t    blocks;     /**< The number of blocks in the memory. */
} rtems_rtl_bit_alloc;

/**
 * Open a bit allocator. The allocator allocates block_size pieces of the memory
 * being managed.
 *
 * @param base The memory to managem NULL to allocate a piece of memory.
 * @param size The size of the memory being managed in bytes.
 * @param block_size The minimum allocation unit in bytes.
 * @param used The amount of memory already in use in bytes.
 * @retval rtems_rtl_bit_alloc The bit allocator structure.
 */
rtems_rtl_bit_alloc* rtems_rtl_bit_alloc_open (void*  base,
                                               size_t size,
                                               size_t block_size,
                                               size_t used);

/**
 * Close the bit allocator.
 *
 * @param balloc The allocator to close.
 */

void rtems_rtl_bit_alloc_close (rtems_rtl_bit_alloc* balloc);

/**
 * Allocate a piece of memory being managed. The size is in bytes are is rounded
 * up the next block size.
 *
 * @param balloc The allocator.
 * @param size Number of bytes to allocate.
 * @return void* The memory if the allocation is successful else NULL if there
 *               is no more memory.
 */
void* rtems_rtl_bit_alloc_balloc (rtems_rtl_bit_alloc* balloc, size_t size);

/**
 * Free an allocated memory block. The size is required because the allocator does not
 * contain any state information.
 */
void rtems_rtl_bit_alloc_bfree (rtems_rtl_bit_alloc* balloc, void* addr, size_t size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

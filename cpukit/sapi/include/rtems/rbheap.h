/**
 * @file
 *
 * @brief Red-Black Tree Heap API
 */

/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RBHEAP_H
#define _RTEMS_RBHEAP_H

#include <rtems.h>
#include <rtems/chain.h>
#include <rtems/rbtree.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RBHeap Red-Black Tree Heap
 *
 * @ingroup ClassicRTEMS
 *
 * @brief Red-Black Tree Heap API.
 *
 * The red-black tree heap provides a memory allocator suitable to implement
 * the malloc() and free() interface.  It uses a first-fit allocation strategy.
 * In the red-black tree heap the administration data structures are not
 * contained in the managed memory area.  Thus writing beyond the boundaries of
 * a chunk does not damage the data to maintain the heap.  This can be used for
 * example in a task stack allocator which protects the task stacks from access
 * by other tasks.  The allocated and free memory parts of the managed area are
 * called chunks.  Each chunk needs a descriptor which is stored outside of the
 * managed area.
 */
/**@{*/

/**
 * @brief Red-black heap chunk descriptor.
 */
typedef struct {
  /**
   * This chain node can be used in two chains
   *  - the chain of spare chunk descriptors and
   *  - the chain of free chunks in the managed memory area.
   *
   * In case this chain node is not part of a chain, the chunk represents a
   * used chunk in the managed memory area.
   */
  rtems_chain_node chain_node;

  /**
   * Tree node for chunks that represent a part of the managed memory area.
   * These chunks are either free or used.
   */
  rtems_rbtree_node tree_node;

  /**
   * Begin address of the chunk.  The address alignment it specified in the
   * @ref rtems_rbheap_control.
   */
  uintptr_t begin;

  /**
   * Size of the chunk in bytes.
   */
  uintptr_t size;
} rtems_rbheap_chunk;

typedef struct rtems_rbheap_control rtems_rbheap_control;

/**
 * @brief Handler to extend the available chunk descriptors.
 *
 * This handler is called when no more chunk descriptors are available.  An
 * example implementation is this:
 *
 * @code
 * void extend_descriptors_with_malloc(rtems_rbheap_control *control)
 * {
 *   rtems_rbheap_chunk *chunk = malloc(sizeof(*chunk));
 *
 *   if (chunk != NULL) {
 *     rtems_rbheap_add_to_spare_descriptor_chain(control, chunk);
 *   }
 * }
 * @endcode
 *
 * @see rtems_rbheap_extend_descriptors_never() and
 * rtems_rbheap_extend_descriptors_with_malloc().
 */
typedef void (*rtems_rbheap_extend_descriptors)(rtems_rbheap_control *control);

/**
 * @brief Red-black heap control.
 */
struct rtems_rbheap_control {
  /**
   * Chain of free chunks in the managed memory area.
   */
  rtems_chain_control free_chunk_chain;

  /**
   * Chain of free chunk descriptors.  Descriptors are consumed during
   * allocation and may be produced during free if contiguous chunks can be
   * coalesced.  In case of descriptor starvation the @ref extend_descriptors
   * handler will be called.
   */
  rtems_chain_control spare_descriptor_chain;

  /**
   * Tree of chunks representing the state of the managed memory area.
   */
  rtems_rbtree_control chunk_tree;

  /**
   * Minimum chunk begin alignment in bytes.
   */
  uintptr_t alignment;

  /**
   * Handler to extend the available chunk descriptors.
   */
  rtems_rbheap_extend_descriptors extend_descriptors;

  /**
   * User specified argument handler for private handler data.
   */
  void *handler_arg;
};

/**
 * @brief Initializes the red-black tree heap @a control.
 *
 * @param[in, out] control The red-black tree heap.
 * @param[in] area_begin The managed memory area begin.
 * @param[in] area_size The managed memory area size.
 * @param[in] alignment The minimum chunk alignment.
 * @param[in] extend_descriptors The handler to extend the available chunk
 * descriptors.
 * @param[in] handler_arg The handler argument.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ADDRESS The memory area is invalid.
 * @retval RTEMS_NO_MEMORY Not enough chunk descriptors.
 */
rtems_status_code rtems_rbheap_initialize(
  rtems_rbheap_control *control,
  void *area_begin,
  uintptr_t area_size,
  uintptr_t alignment,
  rtems_rbheap_extend_descriptors extend_descriptors,
  void *handler_arg
);

/**
 * @brief Allocates a chunk of memory of at least @a size bytes from the
 * red-black tree heap @a control.
 *
 * The chunk begin is aligned by the value specified in
 * rtems_rbheap_initialize().
 *
 * @param[in, out] control The red-black tree heap.
 * @param[in] size The requested chunk size in bytes.
 *
 * @retval NULL Not enough free space in the heap.
 * @retval otherwise Pointer to allocated chunk of memory.
 */
void *rtems_rbheap_allocate(rtems_rbheap_control *control, size_t size);

/**
 * @brief Frees a chunk of memory @a ptr allocated from the red-black tree heap
 * @a control.
 *
 * @param[in, out] control The red-black tree heap.
 * @param[in] ptr The pointer to the chunk of memory.
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID The chunk of memory is not a valid chunk in the
 * red-black tree heap.
 * @retval RTEMS_INCORRECT_STATE The chunk of memory is not in the right state.
 */
rtems_status_code rtems_rbheap_free(rtems_rbheap_control *control, void *ptr);

static inline rtems_chain_control *rtems_rbheap_get_spare_descriptor_chain(
  rtems_rbheap_control *control
)
{
  return &control->spare_descriptor_chain;
}

static inline void rtems_rbheap_add_to_spare_descriptor_chain(
  rtems_rbheap_control *control,
  rtems_rbheap_chunk *chunk
)
{
  rtems_chain_control *chain =
    rtems_rbheap_get_spare_descriptor_chain(control);

  rtems_chain_prepend_unprotected(chain, &chunk->chain_node);
}

static inline void rtems_rbheap_set_extend_descriptors(
  rtems_rbheap_control *control,
  rtems_rbheap_extend_descriptors extend_descriptors
)
{
  control->extend_descriptors = extend_descriptors;
}

static inline void *rtems_rbheap_get_handler_arg(
  const rtems_rbheap_control *control
)
{
  return control->handler_arg;
}

static inline void rtems_rbheap_set_handler_arg(
  rtems_rbheap_control *control,
  void *handler_arg
)
{
  control->handler_arg = handler_arg;
}

/**
 * @brief Chunk descriptor extend handler that does nothing.
 */
void rtems_rbheap_extend_descriptors_never(rtems_rbheap_control *control);

/**
 * @brief Chunk descriptor extend handler that uses malloc().
 */
void rtems_rbheap_extend_descriptors_with_malloc(
  rtems_rbheap_control *control
);

/** @} */

/* Private API */

#define rtems_rbheap_chunk_of_node(node) \
  RTEMS_CONTAINER_OF(node, rtems_rbheap_chunk, tree_node)

static inline bool rtems_rbheap_is_chunk_free(const rtems_rbheap_chunk *chunk)
{
  return !rtems_chain_is_node_off_chain(&chunk->chain_node);
}

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RBHEAP_H */

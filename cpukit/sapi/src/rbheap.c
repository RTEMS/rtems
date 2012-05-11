/**
 * @file
 *
 * @ingroup RBHeap
 *
 * @brief Red-Black Tree Heap implementation.
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
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/rbheap.h>

#include <stdlib.h>

static uintptr_t align_up(uintptr_t alignment, uintptr_t value)
{
  uintptr_t excess = value % alignment;

  if (excess > 0) {
    value += alignment - excess;
  }

  return value;
}

static uintptr_t align_down(uintptr_t alignment, uintptr_t value)
{
  uintptr_t excess = value % alignment;

  return value - excess;
}

static int chunk_compare(const rtems_rbtree_node *a, const rtems_rbtree_node *b)
{
  const rtems_rbheap_chunk *left = rtems_rbheap_chunk_of_node(a);
  const rtems_rbheap_chunk *right = rtems_rbheap_chunk_of_node(b);

  return (int) (left->begin - right->begin);
}

static rtems_rbheap_chunk *get_chunk(rtems_rbheap_control *control)
{
  rtems_chain_control *chain = &control->spare_descriptor_chain;
  rtems_chain_node *chunk = rtems_chain_get_unprotected(chain);

  if (chunk == NULL) {
    (*control->extend_descriptors)(control);
    chunk = rtems_chain_get_unprotected(chain);
  }

  return (rtems_rbheap_chunk *) chunk;
}

static void add_to_chain(
  rtems_chain_control *chain,
  rtems_rbheap_chunk *chunk
)
{
  rtems_chain_prepend_unprotected(chain, &chunk->chain_node);
}

static void insert_into_tree(
  rtems_rbtree_control *tree,
  rtems_rbheap_chunk *chunk
)
{
  _RBTree_Insert_unprotected(tree, &chunk->tree_node);
}

rtems_status_code rtems_rbheap_initialize(
  rtems_rbheap_control *control,
  void *area_begin,
  uintptr_t area_size,
  uintptr_t alignment,
  rtems_rbheap_extend_descriptors extend_descriptors,
  void *handler_arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (alignment > 0) {
    uintptr_t begin = (uintptr_t) area_begin;
    uintptr_t end = begin + area_size;
    uintptr_t aligned_begin = align_up(alignment, begin);
    uintptr_t aligned_end = align_down(alignment, end);

    if (begin < end && begin <= aligned_begin && aligned_begin < aligned_end) {
      rtems_chain_control *free_chain = &control->free_chunk_chain;
      rtems_rbtree_control *chunk_tree = &control->chunk_tree;
      rtems_rbheap_chunk *first = NULL;

      rtems_chain_initialize_empty(free_chain);
      rtems_chain_initialize_empty(&control->spare_descriptor_chain);
      rtems_rbtree_initialize_empty(chunk_tree, chunk_compare, true);
      control->alignment = alignment;
      control->handler_arg = handler_arg;
      control->extend_descriptors = extend_descriptors;

      first = get_chunk(control);
      if (first != NULL) {
        first->begin = aligned_begin;
        first->size = aligned_end - aligned_begin;
        add_to_chain(free_chain, first);
        insert_into_tree(chunk_tree, first);
      } else {
        sc = RTEMS_NO_MEMORY;
      }
    } else {
      sc = RTEMS_INVALID_ADDRESS;
    }
  } else {
    sc = RTEMS_INVALID_NUMBER;
  }

  return sc;
}

static rtems_rbheap_chunk *search_free_chunk(
  rtems_chain_control *free_chain,
  size_t size
)
{
  rtems_chain_node *current = rtems_chain_first(free_chain);
  const rtems_chain_node *tail = rtems_chain_tail(free_chain);
  rtems_rbheap_chunk *big_enough = NULL;

  while (current != tail && big_enough == NULL) {
    rtems_rbheap_chunk *free_chunk = (rtems_rbheap_chunk *) current;

    if (free_chunk->size >= size) {
      big_enough = free_chunk;
    }

    current = rtems_chain_next(current);
  }

  return big_enough;
}

void *rtems_rbheap_allocate(rtems_rbheap_control *control, size_t size)
{
  void *ptr = NULL;
  rtems_chain_control *free_chain = &control->free_chunk_chain;
  rtems_rbtree_control *chunk_tree = &control->chunk_tree;
  uintptr_t alignment = control->alignment;
  uintptr_t aligned_size = align_up(alignment, size);

  if (size > 0 && size <= aligned_size) {
    rtems_rbheap_chunk *free_chunk = search_free_chunk(free_chain, aligned_size);

    if (free_chunk != NULL) {
      uintptr_t free_size = free_chunk->size;

      if (free_size > aligned_size) {
        rtems_rbheap_chunk *new_chunk = get_chunk(control);

        if (new_chunk != NULL) {
          uintptr_t new_free_size = free_size - aligned_size;

          free_chunk->size = new_free_size;
          new_chunk->begin = free_chunk->begin + new_free_size;
          new_chunk->size = aligned_size;
          rtems_chain_set_off_chain(&new_chunk->chain_node);
          insert_into_tree(chunk_tree, new_chunk);
          ptr = (void *) new_chunk->begin;
        }
      } else {
        rtems_chain_extract_unprotected(&free_chunk->chain_node);
        rtems_chain_set_off_chain(&free_chunk->chain_node);
        ptr = (void *) free_chunk->begin;
      }
    }
  }

  return ptr;
}

#define NULL_PAGE rtems_rbheap_chunk_of_node(NULL)

static rtems_rbheap_chunk *find(rtems_rbtree_control *chunk_tree, uintptr_t key)
{
  rtems_rbheap_chunk chunk = { .begin = key };

  return rtems_rbheap_chunk_of_node(
    _RBTree_Find_unprotected(chunk_tree, &chunk.tree_node)
  );
}

static rtems_rbheap_chunk *get_next(
  const rtems_rbheap_chunk *chunk,
  RBTree_Direction dir
)
{
  return rtems_rbheap_chunk_of_node(
    _RBTree_Next_unprotected(&chunk->tree_node, dir)
  );
}

static void check_and_merge(
  rtems_chain_control *free_chain,
  rtems_rbtree_control *chunk_tree,
  rtems_rbheap_chunk *a,
  rtems_rbheap_chunk *b
)
{
  if (b != NULL_PAGE && rtems_rbheap_is_chunk_free(b)) {
    if (b->begin < a->begin) {
      rtems_rbheap_chunk *t = a;

      a = b;
      b = t;
    }

    a->size += b->size;
    rtems_chain_extract_unprotected(&b->chain_node);
    add_to_chain(free_chain, b);
    _RBTree_Extract_unprotected(chunk_tree, &b->tree_node);
  }
}

rtems_status_code rtems_rbheap_free(rtems_rbheap_control *control, void *ptr)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (ptr != NULL) {
    rtems_chain_control *free_chain = &control->free_chunk_chain;
    rtems_rbtree_control *chunk_tree = &control->chunk_tree;
    rtems_rbheap_chunk *chunk = find(chunk_tree, (uintptr_t) ptr);

    if (chunk != NULL_PAGE) {
      if (!rtems_rbheap_is_chunk_free(chunk)) {
        rtems_rbheap_chunk *pred = get_next(chunk, RBT_LEFT);
        rtems_rbheap_chunk *succ = get_next(chunk, RBT_RIGHT);

        check_and_merge(free_chain, chunk_tree, chunk, succ);
        add_to_chain(free_chain, chunk);
        check_and_merge(free_chain, chunk_tree, chunk, pred);
      } else {
        sc = RTEMS_INCORRECT_STATE;
      }
    } else {
      sc = RTEMS_INVALID_ID;
    }
  }

  return sc;
}

void rtems_rbheap_extend_descriptors_never(rtems_rbheap_control *control)
{
  /* Do nothing */
}

void rtems_rbheap_extend_descriptors_with_malloc(rtems_rbheap_control *control)
{
  rtems_rbheap_chunk *chunk = malloc(sizeof(*chunk));

  if (chunk != NULL) {
    rtems_rbheap_add_to_spare_descriptor_chain(control, chunk);
  }
}

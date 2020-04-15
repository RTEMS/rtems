/**
 * @file
 *
 * @ingroup RBHeap
 *
 * @brief Red-Black Tree Heap implementation.
 */

/*
 * Copyright (c) 2012-2015 embedded brains GmbH.  All rights reserved.
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

#ifdef HAVE_CONFIG_H
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

static rtems_rbtree_compare_result chunk_compare(
  const rtems_rbtree_node *a,
  const rtems_rbtree_node *b
)
{
  const rtems_rbheap_chunk *left = rtems_rbheap_chunk_of_node(a);
  const rtems_rbheap_chunk *right = rtems_rbheap_chunk_of_node(b);

  return (rtems_rbtree_compare_result)
    ((left->begin >> 1) - (right->begin >> 1));
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
  rtems_chain_initialize_node(&chunk->chain_node);
  rtems_chain_prepend_unprotected(chain, &chunk->chain_node);
}

static void insert_into_tree(
  rtems_rbtree_control *tree,
  rtems_rbheap_chunk *chunk
)
{
  rtems_rbtree_insert(tree, &chunk->tree_node, chunk_compare, true);
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
  uintptr_t begin = (uintptr_t) area_begin;
  uintptr_t end = begin + area_size;
  uintptr_t aligned_begin;
  uintptr_t aligned_end;

  /*
   * Ensure that the alignment is at least two, so that we can keep
   * chunk_compare() that simple.
   */
  alignment = alignment < 2 ? 2 : alignment;

  aligned_begin = align_up(alignment, begin);
  aligned_end = align_down(alignment, end);

  if (begin < end && begin <= aligned_begin && aligned_begin < aligned_end) {
    rtems_chain_control *free_chain = &control->free_chunk_chain;
    rtems_rbtree_control *chunk_tree = &control->chunk_tree;
    rtems_rbheap_chunk *first = NULL;

    rtems_chain_initialize_empty(free_chain);
    rtems_chain_initialize_empty(&control->spare_descriptor_chain);
    rtems_rbtree_initialize_empty(chunk_tree);
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
    rtems_rbtree_find(chunk_tree, &chunk.tree_node, chunk_compare, true)
  );
}

static rtems_rbheap_chunk *pred(const rtems_rbheap_chunk *chunk)
{
  return rtems_rbheap_chunk_of_node(
    rtems_rbtree_predecessor(&chunk->tree_node)
  );
}

static rtems_rbheap_chunk *succ(const rtems_rbheap_chunk *chunk)
{
  return rtems_rbheap_chunk_of_node(
    rtems_rbtree_successor(&chunk->tree_node)
  );
}

static void check_and_merge(
  rtems_rbheap_control *control,
  rtems_rbheap_chunk *a,
  rtems_rbheap_chunk *b,
  rtems_rbheap_chunk *c
)
{
  if (c != NULL_PAGE && rtems_rbheap_is_chunk_free(c)) {
    a->size += b->size;
    rtems_chain_extract_unprotected(&b->chain_node);
    rtems_rbheap_add_to_spare_descriptor_chain(control, b);
    rtems_rbtree_extract(&control->chunk_tree, &b->tree_node);
  }
}

rtems_status_code rtems_rbheap_free(rtems_rbheap_control *control, void *ptr)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (ptr != NULL) {
    rtems_rbheap_chunk *chunk = find(&control->chunk_tree, (uintptr_t) ptr);

    if (chunk != NULL_PAGE) {
      if (!rtems_rbheap_is_chunk_free(chunk)) {
        rtems_rbheap_chunk *other;

        add_to_chain(&control->free_chunk_chain, chunk);
        other = succ(chunk);
        check_and_merge(control, chunk, other, other);
        other = pred(chunk);
        check_and_merge(control, other, chunk, other);
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

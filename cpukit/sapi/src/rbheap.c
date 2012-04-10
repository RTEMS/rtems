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

static uintptr_t align_up(uintptr_t page_alignment, uintptr_t value)
{
  uintptr_t excess = value % page_alignment;

  if (excess > 0) {
    value += page_alignment - excess;
  }

  return value;
}

static uintptr_t align_down(uintptr_t page_alignment, uintptr_t value)
{
  uintptr_t excess = value % page_alignment;

  return value - excess;
}

static int page_compare(const rtems_rbtree_node *a, const rtems_rbtree_node *b)
{
  const rtems_rbheap_page *left = rtems_rbheap_page_of_node(a);
  const rtems_rbheap_page *right = rtems_rbheap_page_of_node(b);

  return (int) (left->begin - right->begin);
}

static rtems_rbheap_page *get_page(rtems_rbheap_control *control)
{
  rtems_chain_control *pool_chain = &control->pool_chain;
  rtems_chain_node *page = rtems_chain_get(pool_chain);

  if (page == NULL) {
    (*control->extend_page_pool)(control);
    page = rtems_chain_get(pool_chain);
  }

  return (rtems_rbheap_page *) page;
}

static void add_to_chain(
  rtems_chain_control *chain,
  rtems_rbheap_page *page
)
{
  rtems_chain_prepend_unprotected(chain, &page->chain_node);
}

static void insert_into_tree(
  rtems_rbtree_control *tree,
  rtems_rbheap_page *page
)
{
  _RBTree_Insert_unprotected(tree, &page->tree_node);
}

rtems_status_code rtems_rbheap_initialize(
  rtems_rbheap_control *control,
  void *area_begin,
  uintptr_t area_size,
  uintptr_t page_alignment,
  rtems_rbheap_extend_page_pool extend_page_pool,
  void *handler_arg
)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  if (page_alignment > 0) {
    uintptr_t begin = (uintptr_t) area_begin;
    uintptr_t end = begin + area_size;
    uintptr_t aligned_begin = align_up(page_alignment, begin);
    uintptr_t aligned_end = align_down(page_alignment, end);

    if (begin < end && begin <= aligned_begin && aligned_begin < aligned_end) {
      rtems_chain_control *free_chain = &control->free_chain;
      rtems_rbtree_control *page_tree = &control->page_tree;
      rtems_rbheap_page *first = NULL;

      rtems_chain_initialize_empty(free_chain);
      rtems_chain_initialize_empty(&control->pool_chain);
      rtems_rbtree_initialize_empty(page_tree, page_compare, true);
      control->page_alignment = page_alignment;
      control->handler_arg = handler_arg;
      control->extend_page_pool = extend_page_pool;

      first = get_page(control);
      if (first != NULL) {
        first->begin = aligned_begin;
        first->size = aligned_end - aligned_begin;
        add_to_chain(free_chain, first);
        insert_into_tree(page_tree, first);
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

static rtems_rbheap_page *search_free_page(
  rtems_chain_control *free_chain,
  size_t size
)
{
  rtems_chain_node *current = rtems_chain_first(free_chain);
  const rtems_chain_node *tail = rtems_chain_tail(free_chain);
  rtems_rbheap_page *big_enough = NULL;

  while (current != tail && big_enough == NULL) {
    rtems_rbheap_page *free_page = (rtems_rbheap_page *) current;

    if (free_page->size >= size) {
      big_enough = free_page;
    }

    current = rtems_chain_next(current);
  }

  return big_enough;
}

void *rtems_rbheap_allocate(rtems_rbheap_control *control, size_t size)
{
  void *ptr = NULL;
  rtems_chain_control *free_chain = &control->free_chain;
  rtems_rbtree_control *page_tree = &control->page_tree;
  uintptr_t page_alignment = control->page_alignment;
  uintptr_t aligned_size = align_up(page_alignment, size);

  if (size > 0 && size <= aligned_size) {
    rtems_rbheap_page *free_page = search_free_page(free_chain, aligned_size);

    if (free_page != NULL) {
      uintptr_t free_size = free_page->size;

      if (free_size > aligned_size) {
        rtems_rbheap_page *new_page = get_page(control);

        if (new_page != NULL) {
          uintptr_t new_free_size = free_size - aligned_size;

          free_page->size = new_free_size;
          new_page->begin = free_page->begin + new_free_size;
          new_page->size = aligned_size;
          rtems_chain_set_off_chain(&new_page->chain_node);
          insert_into_tree(page_tree, new_page);
          ptr = (void *) new_page->begin;
        }
      } else {
        rtems_chain_extract_unprotected(&free_page->chain_node);
        rtems_chain_set_off_chain(&free_page->chain_node);
        ptr = (void *) free_page->begin;
      }
    }
  }

  return ptr;
}

#define NULL_PAGE rtems_rbheap_page_of_node(NULL)

static rtems_rbheap_page *find(rtems_rbtree_control *page_tree, uintptr_t key)
{
  rtems_rbheap_page page = { .begin = key };

  return rtems_rbheap_page_of_node(
    _RBTree_Find_unprotected(page_tree, &page.tree_node)
  );
}

static rtems_rbheap_page *get_next(
  const rtems_rbtree_control *page_tree,
  const rtems_rbheap_page *page,
  RBTree_Direction dir
)
{
  return rtems_rbheap_page_of_node(
    _RBTree_Next(page_tree, &page->tree_node, dir)
  );
}

static void check_and_merge(
  rtems_chain_control *free_chain,
  rtems_rbtree_control *page_tree,
  rtems_rbheap_page *a,
  rtems_rbheap_page *b
)
{
  if (b != NULL_PAGE && rtems_rbheap_is_page_free(b)) {
    if (b->begin < a->begin) {
      rtems_rbheap_page *t = a;

      a = b;
      b = t;
    }

    a->size += b->size;
    rtems_chain_extract_unprotected(&b->chain_node);
    add_to_chain(free_chain, b);
    _RBTree_Extract_unprotected(page_tree, &b->tree_node);
  }
}

rtems_status_code rtems_rbheap_free(rtems_rbheap_control *control, void *ptr)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_chain_control *free_chain = &control->free_chain;
  rtems_rbtree_control *page_tree = &control->page_tree;
  rtems_rbheap_page *page = find(page_tree, (uintptr_t) ptr);

  if (page != NULL_PAGE) {
    if (!rtems_rbheap_is_page_free(page)) {
      rtems_rbheap_page *pred = get_next(page_tree, page, RBT_LEFT);
      rtems_rbheap_page *succ = get_next(page_tree, page, RBT_RIGHT);

      check_and_merge(free_chain, page_tree, page, succ);
      add_to_chain(free_chain, page);
      check_and_merge(free_chain, page_tree, page, pred);
    } else {
      sc = RTEMS_INCORRECT_STATE;
    }
  } else {
    sc = RTEMS_INVALID_ID;
  }

  return sc;
}

void rtems_rbheap_extend_page_pool_never(rtems_rbheap_control *control)
{
  /* Do nothing */
}

void rtems_rbheap_extend_page_pool_with_malloc(rtems_rbheap_control *control)
{
  rtems_rbheap_page *page = malloc(sizeof(*page));

  if (page != NULL) {
    rtems_chain_control *pool_chain = rtems_rbheap_get_pool_chain(control);

    add_to_chain(pool_chain, page);
  }
}

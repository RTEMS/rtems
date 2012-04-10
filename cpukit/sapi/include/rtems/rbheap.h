/**
 * @file
 *
 * @ingroup RBHeap
 *
 * @brief Red-Black Tree Heap API.
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
 * @brief Red-Black Tree Heap API.
 *
 * In the Red-Black Tree Heap the administration data structures are not
 * contained in the managed memory area.  This can be used for example in a
 * task stack allocator which protects the task stacks from access by other
 * tasks.
 *
 * @{
 */

typedef struct {
  rtems_chain_node chain_node;
  rtems_rbtree_node tree_node;
  uintptr_t begin;
  uintptr_t size;
} rtems_rbheap_page;

typedef struct rtems_rbheap_control rtems_rbheap_control;

typedef void (*rtems_rbheap_extend_page_pool)(rtems_rbheap_control *control);

struct rtems_rbheap_control {
  rtems_chain_control free_chain;
  rtems_chain_control pool_chain;
  rtems_rbtree_control page_tree;
  uintptr_t page_alignment;
  rtems_rbheap_extend_page_pool extend_page_pool;
  void *handler_arg;
};

rtems_status_code rtems_rbheap_initialize(
  rtems_rbheap_control *control,
  void *area_begin,
  uintptr_t area_size,
  uintptr_t page_alignment,
  rtems_rbheap_extend_page_pool extend_page_pool,
  void *handler_arg
);

void *rtems_rbheap_allocate(rtems_rbheap_control *control, size_t size);

rtems_status_code rtems_rbheap_free(rtems_rbheap_control *control, void *ptr);

static inline rtems_chain_control *rtems_rbheap_get_pool_chain(
  rtems_rbheap_control *control
)
{
  return &control->pool_chain;
}

static inline void rtems_rbheap_set_extend_page_pool(
  rtems_rbheap_control *control,
  rtems_rbheap_extend_page_pool extend_page_pool
)
{
  control->extend_page_pool = extend_page_pool;
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

void rtems_rbheap_extend_page_pool_never(rtems_rbheap_control *control);

void rtems_rbheap_extend_page_pool_with_malloc(rtems_rbheap_control *control);

/** @} */

/* Private API */

#define rtems_rbheap_page_of_node(node) \
  rtems_rbtree_container_of(node, rtems_rbheap_page, tree_node)

static inline bool rtems_rbheap_is_page_free(const rtems_rbheap_page *page)
{
  return !rtems_chain_is_node_off_chain(&page->chain_node);
}

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RBHEAP_H */

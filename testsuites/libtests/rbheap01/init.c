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

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "tmacros.h"

#include <rtems.h>
#include <rtems/rbheap.h>

/* forward declarations to avoid warnings */
static rtems_task Init(rtems_task_argument argument);

#define PAGE_SIZE 1024

#define PAGE_COUNT 8

static char area [PAGE_SIZE * PAGE_COUNT + PAGE_SIZE - 1];

static rtems_rbheap_chunk chunks [PAGE_COUNT];

static void extend_descriptors(rtems_rbheap_control *control)
{
  rtems_chain_control *chain =
    rtems_rbheap_get_spare_descriptor_chain(control);

  rtems_rbheap_set_extend_descriptors(
    control,
    rtems_rbheap_extend_descriptors_never
  );

  rtems_chain_initialize(
    chain,
    chunks,
    PAGE_COUNT,
    sizeof(chunks [0])
  );
}

static uintptr_t idx(const rtems_rbheap_chunk *chunk)
{
  uintptr_t base = (uintptr_t) area;
  uintptr_t excess = base % PAGE_SIZE;

  if (excess > 0) {
    base += PAGE_SIZE - excess;
  }

  return (chunk->begin - base) / PAGE_SIZE;
}

typedef struct {
  const uintptr_t *index_current;
  const uintptr_t *index_end;
  const bool *free_current;
  const bool *free_end;
} chunk_visitor_context;

static bool chunk_visitor(
  const RBTree_Node *node,
  RBTree_Direction dir,
  void *visitor_arg
)
{
  rtems_rbheap_chunk *chunk = rtems_rbheap_chunk_of_node(node);
  chunk_visitor_context *context = visitor_arg;

  rtems_test_assert(context->index_current != context->index_end);
  rtems_test_assert(context->free_current != context->free_end);

  rtems_test_assert(idx(chunk) == *context->index_current);
  rtems_test_assert(rtems_rbheap_is_chunk_free(chunk) == *context->free_current);

  ++context->index_current;
  ++context->free_current;

  return false;
}

static void test_init_chunk_alignment(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_rbheap_control control;

  sc = rtems_rbheap_initialize(
    &control,
    area,
    sizeof(area),
    0,
    extend_descriptors,
    NULL
  );
  rtems_test_assert(sc == RTEMS_INVALID_NUMBER);
}

static void test_init_begin_greater_than_end(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_rbheap_control control;

  sc = rtems_rbheap_initialize(
    &control,
    (void *) PAGE_SIZE,
    (uintptr_t) -PAGE_SIZE,
    PAGE_SIZE,
    extend_descriptors,
    NULL
  );
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);
}

static void test_init_begin_greater_than_aligned_begin(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_rbheap_control control;

  sc = rtems_rbheap_initialize(
    &control,
    (void *) -(PAGE_SIZE / 2),
    PAGE_SIZE,
    PAGE_SIZE,
    extend_descriptors,
    NULL
  );
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);
}

static void test_init_aligned_begin_greater_than_aligned_end(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_rbheap_control control;

  sc = rtems_rbheap_initialize(
    &control,
    (void *) PAGE_SIZE,
    PAGE_SIZE / 2,
    PAGE_SIZE,
    extend_descriptors,
    NULL
  );
  rtems_test_assert(sc == RTEMS_INVALID_ADDRESS);
}

static void test_init_empty_descriptors(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_rbheap_control control;

  sc = rtems_rbheap_initialize(
    &control,
    (void *) PAGE_SIZE,
    PAGE_SIZE,
    PAGE_SIZE,
    rtems_rbheap_extend_descriptors_never,
    NULL
  );
  rtems_test_assert(sc == RTEMS_NO_MEMORY);
}

static void test_chunk_tree(
  const rtems_rbheap_control *control,
  const uintptr_t *index_begin,
  const uintptr_t *index_end,
  const bool *free_begin,
  const bool *free_end
)
{
  chunk_visitor_context context = {
    .index_current = index_begin,
    .index_end = index_end,
    .free_current = free_begin,
    .free_end = free_end
  };

  _RBTree_Iterate_unprotected(
    &control->chunk_tree,
    RBT_RIGHT,
    chunk_visitor,
    &context
  );
}

#define TEST_PAGE_TREE(control, indices, frees) \
  test_chunk_tree( \
    control, \
    indices, \
    &indices [sizeof(indices) / sizeof(indices [0])], \
    frees, \
    &frees [sizeof(frees) / sizeof(frees [0])] \
  )

static void test_init_successful(rtems_rbheap_control *control)
{
  static const uintptr_t indices [] = {
    0
  };
  static const bool frees [] = {
    true
  };

  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_rbheap_initialize(
    control,
    area,
    sizeof(area),
    PAGE_SIZE,
    extend_descriptors,
    NULL
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  TEST_PAGE_TREE(control, indices, frees);
}

static void test_alloc_and_free_one(void)
{
  static const uintptr_t indices_0 [] = {
    0,
    PAGE_COUNT - 1
  };
  static const bool frees_0 [] = {
    true,
    false
  };
  static const uintptr_t indices_1 [] = {
    0
  };
  static const bool frees_1 [] = {
    true,
  };

  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_rbheap_control control;
  void *ptr = NULL;

  test_init_successful(&control);

  ptr = rtems_rbheap_allocate(&control, PAGE_SIZE);
  rtems_test_assert(ptr != NULL);

  TEST_PAGE_TREE(&control, indices_0, frees_0);

  sc = rtems_rbheap_free(&control, ptr);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  TEST_PAGE_TREE(&control, indices_1, frees_1);
}

static void test_alloc_zero(void)
{
  static const uintptr_t indices [] = {
    0
  };
  static const bool frees [] = {
    true
  };

  rtems_rbheap_control control;
  void *ptr = NULL;

  test_init_successful(&control);

  ptr = rtems_rbheap_allocate(&control, 0);
  rtems_test_assert(ptr == NULL);

  TEST_PAGE_TREE(&control, indices, frees);
}

static void test_alloc_huge_chunk(void)
{
  static const uintptr_t indices [] = {
    0
  };
  static const bool frees [] = {
    true
  };

  rtems_rbheap_control control;
  void *ptr = NULL;

  test_init_successful(&control);

  ptr = rtems_rbheap_allocate(&control, (PAGE_COUNT + 1) * PAGE_SIZE);
  rtems_test_assert(ptr == NULL);

  TEST_PAGE_TREE(&control, indices, frees);
}

static void test_alloc_one_chunk(void)
{
  static const uintptr_t indices_0 [] = {
    0
  };
  static const bool frees_0 [] = {
    false
  };
  static const uintptr_t indices_1 [] = {
    0
  };
  static const bool frees_1 [] = {
    true,
  };

  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_rbheap_control control;
  void *ptr = NULL;

  test_init_successful(&control);

  ptr = rtems_rbheap_allocate(&control, PAGE_COUNT * PAGE_SIZE);
  rtems_test_assert(ptr != NULL);

  TEST_PAGE_TREE(&control, indices_0, frees_0);

  sc = rtems_rbheap_free(&control, ptr);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  TEST_PAGE_TREE(&control, indices_1, frees_1);
}

static void test_alloc_many_chunks(void)
{
  static const uintptr_t indices_0 [] = {
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7
  };
  static const bool frees_0 [] = {
    false,
    false,
    false,
    false,
    false,
    false,
    false,
    false
  };
  static const uintptr_t indices_1 [] = {
    0
  };
  static const bool frees_1 [] = {
    true,
  };

  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_rbheap_control control;
  void *ptr [PAGE_COUNT];
  void *null = NULL;
  int i = 0;

  test_init_successful(&control);

  for (i = 0; i < PAGE_COUNT; ++i) {
    ptr [i] = rtems_rbheap_allocate(&control, PAGE_SIZE);
    rtems_test_assert(ptr [i] != NULL);
  }

  TEST_PAGE_TREE(&control, indices_0, frees_0);

  null = rtems_rbheap_allocate(&control, PAGE_SIZE);
  rtems_test_assert(null == NULL);

  TEST_PAGE_TREE(&control, indices_0, frees_0);

  for (i = 0; i < PAGE_COUNT; ++i) {
    sc = rtems_rbheap_free(&control, ptr [i]);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  TEST_PAGE_TREE(&control, indices_1, frees_1);
}

static void test_free_null(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_rbheap_control control;

  test_init_successful(&control);

  sc = rtems_rbheap_free(&control, NULL);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);
}

static void test_free_invalid(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_rbheap_control control;

  test_init_successful(&control);

  sc = rtems_rbheap_free(&control, (void *) 1);
  rtems_test_assert(sc == RTEMS_INVALID_ID);
}

static void test_free_double(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_rbheap_control control;
  void *ptr = NULL;

  test_init_successful(&control);

  ptr = rtems_rbheap_allocate(&control, PAGE_COUNT * PAGE_SIZE);
  rtems_test_assert(ptr != NULL);

  sc = rtems_rbheap_free(&control, ptr);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  sc = rtems_rbheap_free(&control, ptr);
  rtems_test_assert(sc == RTEMS_INCORRECT_STATE);
}

enum {
  LOW,
  LEFT,
  MIDDLE,
  RIGHT,
  HIGH
};

static void test_free_merge_left_or_right(bool left)
{
  static const uintptr_t indices_0 [] = {
    0,
    3,
    4,
    5,
    6,
    7
  };
  static const bool frees_0 [] = {
    true,
    false,
    false,
    false,
    false,
    false
  };
  static const uintptr_t indices_1_left [] = {
    0,
    3,
    4,
    5,
    6,
    7
  };
  static const bool frees_1_left [] = {
    true,
    false,
    true,
    false,
    false,
    false
  };
  static const uintptr_t indices_1_right [] = {
    0,
    3,
    4,
    5,
    6,
    7
  };
  static const bool frees_1_right [] = {
    true,
    false,
    false,
    false,
    true,
    false
  };
  static const uintptr_t indices_2_left [] = {
    0,
    3,
    4,
    6,
    7
  };
  static const bool frees_2_left [] = {
    true,
    false,
    true,
    false,
    false
  };
  static const uintptr_t indices_2_right [] = {
    0,
    3,
    4,
    5,
    7
  };
  static const bool frees_2_right [] = {
    true,
    false,
    false,
    true,
    false
  };

  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_rbheap_control control;
  void *ptr [HIGH + 1];
  int dir = left ? LEFT : RIGHT;
  int i = 0;

  test_init_successful(&control);

  for (i = sizeof(ptr) / sizeof(ptr [0]) - 1; i >= 0; --i) {
    ptr [i] = rtems_rbheap_allocate(&control, PAGE_SIZE);
    rtems_test_assert(ptr [i] != NULL);
  }

  TEST_PAGE_TREE(&control, indices_0, frees_0);

  sc = rtems_rbheap_free(&control, ptr [dir]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  if (left) {
    TEST_PAGE_TREE(&control, indices_1_left, frees_1_left);
  } else {
    TEST_PAGE_TREE(&control, indices_1_right, frees_1_right);
  }

  sc = rtems_rbheap_free(&control, ptr [MIDDLE]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  if (left) {
    TEST_PAGE_TREE(&control, indices_2_left, frees_2_left);
  } else {
    TEST_PAGE_TREE(&control, indices_2_right, frees_2_right);
  }
}

static void Init(rtems_task_argument arg)
{
  puts("\n\n*** TEST RBHEAP 1 ***");

  test_init_chunk_alignment();
  test_init_begin_greater_than_end();
  test_init_begin_greater_than_aligned_begin();
  test_init_aligned_begin_greater_than_aligned_end();
  test_init_empty_descriptors();
  test_alloc_and_free_one();
  test_alloc_zero();
  test_alloc_huge_chunk();
  test_alloc_one_chunk();
  test_alloc_many_chunks();
  test_free_null();
  test_free_invalid();
  test_free_double();
  test_free_merge_left_or_right(true);
  test_free_merge_left_or_right(false);

  puts("*** END OF TEST RBHEAP 1 ***");

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

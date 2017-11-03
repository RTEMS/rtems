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

#include "tmacros.h"

#include <rtems.h>
#include <rtems/rbheap.h>
#include <rtems/malloc.h>
#include <rtems/score/rbtreeimpl.h>

const char rtems_test_name[] = "RBHEAP 1";

/* forward declarations to avoid warnings */
static rtems_task Init(rtems_task_argument argument);

#define TEST_PAGE_SIZE 1024

#define TEST_PAGE_COUNT 8

static char area [TEST_PAGE_SIZE * TEST_PAGE_COUNT + TEST_PAGE_SIZE - 1];

static rtems_rbheap_chunk chunks [TEST_PAGE_COUNT];

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
    TEST_PAGE_COUNT,
    sizeof(chunks [0])
  );
}

static uintptr_t idx(const rtems_rbheap_chunk *chunk)
{
  uintptr_t base = (uintptr_t) area;
  uintptr_t excess = base % TEST_PAGE_SIZE;

  if (excess > 0) {
    base += TEST_PAGE_SIZE - excess;
  }

  return (chunk->begin - base) / TEST_PAGE_SIZE;
}

typedef struct {
  uintptr_t index;
  bool free;
} chunk_descriptor;

typedef struct {
  const chunk_descriptor *chunk_current;
  const chunk_descriptor *chunk_end;
} chunk_visitor_context;

static bool chunk_visitor(
  const RBTree_Node *node,
  void *visitor_arg
)
{
  rtems_rbheap_chunk *chunk = rtems_rbheap_chunk_of_node(node);
  chunk_visitor_context *context = visitor_arg;
  const chunk_descriptor *current = context->chunk_current;

  rtems_test_assert(current != context->chunk_end);

  rtems_test_assert(idx(chunk) == current->index);
  rtems_test_assert(rtems_rbheap_is_chunk_free(chunk) == current->free);

  context->chunk_current = current + 1;

  return false;
}

static void test_init_begin_greater_than_end(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_rbheap_control control;

  sc = rtems_rbheap_initialize(
    &control,
    (void *) TEST_PAGE_SIZE,
    (uintptr_t) -TEST_PAGE_SIZE,
    TEST_PAGE_SIZE,
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
    (void *) -(TEST_PAGE_SIZE / 2),
    TEST_PAGE_SIZE,
    TEST_PAGE_SIZE,
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
    (void *) TEST_PAGE_SIZE,
    TEST_PAGE_SIZE / 2,
    TEST_PAGE_SIZE,
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
    (void *) TEST_PAGE_SIZE,
    TEST_PAGE_SIZE,
    TEST_PAGE_SIZE,
    rtems_rbheap_extend_descriptors_never,
    NULL
  );
  rtems_test_assert(sc == RTEMS_NO_MEMORY);
}

static void test_chunk_tree(
  const rtems_rbheap_control *control,
  const chunk_descriptor *chunk_begin,
  size_t chunk_count
)
{
  chunk_visitor_context context = {
    .chunk_current = chunk_begin,
    .chunk_end = chunk_begin + chunk_count
  };

  rtems_test_assert(
    rtems_chain_node_count_unprotected(&control->spare_descriptor_chain)
      == TEST_PAGE_COUNT - chunk_count
  );

  _RBTree_Iterate(
    &control->chunk_tree,
    chunk_visitor,
    &context
  );
}

#define TEST_PAGE_TREE(control, chunks) \
  test_chunk_tree( \
    control, \
    chunks, \
    RTEMS_ARRAY_SIZE(chunks) \
  )

static void test_init_successful(rtems_rbheap_control *control)
{
  static const chunk_descriptor chunks [] = {
    { 0, true }
  };

  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_rbheap_initialize(
    control,
    area,
    sizeof(area),
    TEST_PAGE_SIZE,
    extend_descriptors,
    NULL
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  TEST_PAGE_TREE(control, chunks);
}

static void test_alloc_and_free_one(void)
{
  static const chunk_descriptor chunks_0 [] = {
    { 0, true },
    { TEST_PAGE_COUNT - 1, false }
  };
  static const chunk_descriptor chunks_1 [] = {
    { 0, true }
  };

  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_rbheap_control control;
  void *ptr = NULL;

  test_init_successful(&control);

  ptr = rtems_rbheap_allocate(&control, TEST_PAGE_SIZE);
  rtems_test_assert(ptr != NULL);

  TEST_PAGE_TREE(&control, chunks_0);

  sc = rtems_rbheap_free(&control, ptr);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  TEST_PAGE_TREE(&control, chunks_1);
}

static void test_alloc_zero(void)
{
  static const chunk_descriptor chunks [] = {
    { 0, true }
  };

  rtems_rbheap_control control;
  void *ptr = NULL;

  test_init_successful(&control);

  ptr = rtems_rbheap_allocate(&control, 0);
  rtems_test_assert(ptr == NULL);

  TEST_PAGE_TREE(&control, chunks);
}

static void test_alloc_huge_chunk(void)
{
  static const chunk_descriptor chunks [] = {
    { 0, true }
  };

  rtems_rbheap_control control;
  void *ptr = NULL;

  test_init_successful(&control);

  ptr = rtems_rbheap_allocate(&control, (TEST_PAGE_COUNT + 1) * TEST_PAGE_SIZE);
  rtems_test_assert(ptr == NULL);

  TEST_PAGE_TREE(&control, chunks);
}

static void test_alloc_one_chunk(void)
{
  static const chunk_descriptor chunks_0 [] = {
    { 0, false }
  };
  static const chunk_descriptor chunks_1 [] = {
    { 0, true },
  };

  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_rbheap_control control;
  void *ptr = NULL;

  test_init_successful(&control);

  ptr = rtems_rbheap_allocate(&control, TEST_PAGE_COUNT * TEST_PAGE_SIZE);
  rtems_test_assert(ptr != NULL);

  TEST_PAGE_TREE(&control, chunks_0);

  sc = rtems_rbheap_free(&control, ptr);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  TEST_PAGE_TREE(&control, chunks_1);
}

static void test_alloc_many_chunks(void)
{
  static const chunk_descriptor chunks_0 [] = {
    { 0, false },
    { 1, false },
    { 2, false },
    { 3, false },
    { 4, false },
    { 5, false },
    { 6, false },
    { 7, false }
  };
  static const chunk_descriptor chunks_1 [] = {
    { 0, true }
  };

  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_rbheap_control control;
  void *ptr [TEST_PAGE_COUNT];
  void *null = NULL;
  int i = 0;

  test_init_successful(&control);

  for (i = 0; i < TEST_PAGE_COUNT; ++i) {
    ptr [i] = rtems_rbheap_allocate(&control, TEST_PAGE_SIZE);
    rtems_test_assert(ptr [i] != NULL);
  }

  TEST_PAGE_TREE(&control, chunks_0);

  null = rtems_rbheap_allocate(&control, TEST_PAGE_SIZE);
  rtems_test_assert(null == NULL);

  TEST_PAGE_TREE(&control, chunks_0);

  for (i = 0; i < TEST_PAGE_COUNT; ++i) {
    sc = rtems_rbheap_free(&control, ptr [i]);
    rtems_test_assert(sc == RTEMS_SUCCESSFUL);
  }

  TEST_PAGE_TREE(&control, chunks_1);
}

static void test_alloc_misaligned(void)
{
  rtems_rbheap_control control;
  void *p;

  test_init_successful(&control);

  p = rtems_rbheap_allocate(&control, TEST_PAGE_SIZE - 1);
  rtems_test_assert(p != NULL);
}

static void test_alloc_with_malloc_extend(void)
{
  rtems_status_code sc;
  rtems_rbheap_control control;
  void *p;
  void *opaque;

  sc = rtems_rbheap_initialize(
    &control,
    area,
    sizeof(area),
    TEST_PAGE_SIZE,
    rtems_rbheap_extend_descriptors_with_malloc,
    NULL
  );
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  opaque = rtems_heap_greedy_allocate(NULL, 0);

  p = rtems_rbheap_allocate(&control, TEST_PAGE_SIZE);
  rtems_test_assert(p == NULL);

  rtems_heap_greedy_free(opaque);

  p = rtems_rbheap_allocate(&control, TEST_PAGE_SIZE);
  rtems_test_assert(p != NULL);
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

  ptr = rtems_rbheap_allocate(&control, TEST_PAGE_COUNT * TEST_PAGE_SIZE);
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
  static const chunk_descriptor chunks_0 [] = {
    { 0, true },
    { 3, false },
    { 4, false },
    { 5, false },
    { 6, false },
    { 7, false }
  };
  static const chunk_descriptor chunks_1_left [] = {
    { 0, true },
    { 3, false },
    { 4, true },
    { 5, false },
    { 6, false },
    { 7, false }
  };
  static const chunk_descriptor chunks_1_right [] = {
    { 0, true },
    { 3, false },
    { 4, false },
    { 5, false },
    { 6, true },
    { 7, false }
  };
  static const chunk_descriptor chunks_2_left [] = {
    { 0, true },
    { 3, false },
    { 4, true },
    { 6, false },
    { 7, false }
  };
  static const chunk_descriptor chunks_2_right [] = {
    { 0, true },
    { 3, false },
    { 4, false },
    { 5, true },
    { 7, false }
  };

  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_rbheap_control control;
  void *ptr [HIGH + 1];
  int dir = left ? LEFT : RIGHT;
  int i = 0;

  test_init_successful(&control);

  for (i = sizeof(ptr) / sizeof(ptr [0]) - 1; i >= 0; --i) {
    ptr [i] = rtems_rbheap_allocate(&control, TEST_PAGE_SIZE);
    rtems_test_assert(ptr [i] != NULL);
  }

  TEST_PAGE_TREE(&control, chunks_0);

  sc = rtems_rbheap_free(&control, ptr [dir]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  if (left) {
    TEST_PAGE_TREE(&control, chunks_1_left);
  } else {
    TEST_PAGE_TREE(&control, chunks_1_right);
  }

  sc = rtems_rbheap_free(&control, ptr [MIDDLE]);
  rtems_test_assert(sc == RTEMS_SUCCESSFUL);

  if (left) {
    TEST_PAGE_TREE(&control, chunks_2_left);
  } else {
    TEST_PAGE_TREE(&control, chunks_2_right);
  }
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test_init_begin_greater_than_end();
  test_init_begin_greater_than_aligned_begin();
  test_init_aligned_begin_greater_than_aligned_end();
  test_init_empty_descriptors();
  test_alloc_and_free_one();
  test_alloc_zero();
  test_alloc_huge_chunk();
  test_alloc_one_chunk();
  test_alloc_many_chunks();
  test_alloc_misaligned();
  test_alloc_with_malloc_extend();
  test_free_null();
  test_free_invalid();
  test_free_double();
  test_free_merge_left_or_right(true);
  test_free_merge_left_or_right(false);

  TEST_END();

  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

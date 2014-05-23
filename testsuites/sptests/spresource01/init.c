/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#include <stdio.h>

#include <rtems.h>
#include <rtems/score/resourceimpl.h>

#include "tmacros.h"

const char rtems_test_name[] = "SPRESOURCE 1";

static Resource_Control rt[16];

static Resource_Node nt[16];

#define RESOURCE_COUNT RTEMS_ARRAY_SIZE(rt)

#define NODE_COUNT RTEMS_ARRAY_SIZE(nt)

static size_t ri(Resource_Control *r)
{
  return (size_t) (r - &rt[0]);
}

static size_t ni(Resource_Node *n)
{
  return (size_t) (n - &nt[0]);
}

static void print_node(Resource_Node *n);

static void print_resource(Resource_Control *r)
{
  Chain_Control *rivals = &r->Rivals;

  if (!_Chain_Is_empty(rivals)) {
    const Chain_Node *tail = _Chain_Immutable_tail(rivals);
    Resource_Node *last = (Resource_Node *) _Chain_First(rivals);
    Resource_Node *next = (Resource_Node *) _Chain_Next(&last->Node);

    printf(
      "  subgraph {\n"
      "    rank=same;\n"
      "    n%zi [style=filled, fillcolor=green];\n"
      "    r%zi -> n%zi;\n",
      ni(last),
      ri(r),
      ni(last)
    );

    while (&next->Node != tail) {
      printf(
        "    n%zi [style=filled, fillcolor=green];\n"
        "    n%zi -> n%zi;\n",
        ni(next),
        ni(last),
        ni(next)
      );

      last = next;
      next = (Resource_Node *) _Chain_Next(&next->Node);
    }

    printf("  }\n");

    next = (Resource_Node *) _Chain_First(rivals);
    while (&next->Node != tail) {
      print_node(next);

      next = (Resource_Node *) _Chain_Next(&next->Node);
    }
  }
}

static void print_node(Resource_Node *n)
{
  Chain_Control *resources = &n->Resources;

  if (!_Chain_Is_empty(resources)) {
    const Chain_Node *tail = _Chain_Immutable_tail(resources);
    Resource_Control *last = (Resource_Control *) _Chain_First(resources);
    Resource_Control *next = (Resource_Control *) _Chain_Next(&last->Node);

    printf("  n%zi -> r%zi;\n", ni(n), ri(last));
    print_resource(last);

    while (&next->Node != tail) {
      printf("  r%zi -> r%zi;\n", ri(last), ri(next));
      print_resource(next);

      last = next;
      next = (Resource_Control *) _Chain_Next(&next->Node);
    }
  }
}

static const size_t node_seq[NODE_COUNT - 1] =
  { 1, 2, 4, 6, 8, 15, 3, 12, 10, 11, 14, 13, 7, 5, 9 };

static bool visitor_all(Resource_Node *n, void *arg)
{
  size_t *index = arg;
  size_t i = *index;

  printf("n%zi\n", ni(n));

  rtems_test_assert(i < RTEMS_ARRAY_SIZE(node_seq));
  rtems_test_assert(node_seq[i] == ni(n));

  *index = i + 1;

  return false;
}

static bool visitor_seven(Resource_Node *n, void *arg)
{
  size_t *index = arg;
  size_t i = *index;

  printf("n%zi\n", ni(n));

  rtems_test_assert(i < 7);
  rtems_test_assert(node_seq[i] == ni(n));

  *index = i + 1;

  return i + 1 >= 7;
}

static const bool owns_resources[RESOURCE_COUNT] = {
  true, true, false, true, false, false, false, false, true, false, false,
  false, false, false, true, false
};

static void add_resource(Resource_Node *n, Resource_Control *r)
{
  _Resource_Node_add_resource(n, r);
  _Resource_Set_owner(r, n);
}

static void add_rival(Resource_Control *r, Resource_Node *n)
{
  _Resource_Add_rival(r, n);
  _Resource_Node_set_dependency(n, r);
  _Resource_Node_set_root(n, &nt[0]);
}

static void test_resource_iterate(void)
{
  Resource_Node *root = &nt[0];
  size_t i;

  printf("digraph {\n");

  for (i = 0; i < RESOURCE_COUNT; ++i) {
    _Resource_Initialize(&rt[i]);
  }

  for (i = 0; i < NODE_COUNT; ++i) {
    _Resource_Node_initialize(&nt[i]);
  }

  add_resource(&nt[0], &rt[12]);
  add_resource(&nt[0], &rt[11]);
  add_resource(&nt[0], &rt[6]);
  add_resource(&nt[0], &rt[3]);
  add_resource(&nt[0], &rt[2]);
  add_resource(&nt[0], &rt[1]);
  add_resource(&nt[0], &rt[0]);

  add_resource(&nt[1], &rt[9]);
  add_resource(&nt[1], &rt[8]);
  add_resource(&nt[1], &rt[7]);
  add_resource(&nt[1], &rt[5]);

  add_resource(&nt[3], &rt[15]);
  add_resource(&nt[3], &rt[13]);
  add_resource(&nt[3], &rt[10]);

  add_resource(&nt[8], &rt[14]);

  add_resource(&nt[14], &rt[4]);

  add_rival(&rt[0], &nt[1]);
  add_rival(&rt[0], &nt[2]);
  add_rival(&rt[0], &nt[4]);
  add_rival(&rt[0], &nt[6]);
  add_rival(&rt[0], &nt[8]);
  add_rival(&rt[0], &nt[15]);

  add_rival(&rt[1], &nt[7]);

  add_rival(&rt[5], &nt[3]);
  add_rival(&rt[5], &nt[12]);

  add_rival(&rt[7], &nt[11]);
  add_rival(&rt[7], &nt[14]);

  add_rival(&rt[8], &nt[13]);

  add_rival(&rt[12], &nt[5]);
  add_rival(&rt[12], &nt[9]);

  add_rival(&rt[15], &nt[10]);

  for (i = 0; i < NODE_COUNT; ++i) {
    rtems_test_assert(
      _Resource_Node_owns_resources(&nt[i]) == owns_resources[i]
    );
  }

  printf("  n%zi [style=filled, fillcolor=green];\n", ni(root));
  print_node(root);

  printf("}\n");

  i = 0;
  _Resource_Iterate(root, visitor_all, &i);
  rtems_test_assert(i == 15);

  i = 0;
  _Resource_Iterate(root, visitor_seven, &i);
  rtems_test_assert(i == 7);
}

static void test_resource_simple(void)
{
  Resource_Control r0;
  Resource_Control r1;
  Resource_Node n0;

  _Resource_Node_initialize(&n0);
  rtems_test_assert(!_Resource_Node_owns_resources(&n0));
  rtems_test_assert(_Resource_Node_get_root(&n0) == &n0);
  rtems_test_assert(n0.dependency == NULL);

  _Resource_Node_set_root(&n0, NULL);
  rtems_test_assert(_Resource_Node_get_root(&n0) == NULL);
  _Resource_Node_set_root(&n0, &n0);
  rtems_test_assert(_Resource_Node_get_root(&n0) == &n0);

  _Resource_Initialize(&r0);
  rtems_test_assert(_Resource_Get_owner(&r0) == NULL);
  rtems_test_assert(_Chain_Is_empty(&r0.Rivals));

  _Resource_Set_owner(&r0, &n0);
  rtems_test_assert(_Resource_Get_owner(&r0) == &n0);
  _Resource_Set_owner(&r0, NULL);
  rtems_test_assert(_Resource_Get_owner(&r0) == NULL);

  _Resource_Initialize(&r1);

  _Resource_Node_add_resource(&n0, &r0);
  rtems_test_assert(_Resource_Node_owns_resources(&n0));
  rtems_test_assert(_Resource_Is_most_recently_obtained(&r0, &n0));

  _Resource_Node_add_resource(&n0, &r1);
  rtems_test_assert(_Resource_Node_owns_resources(&n0));
  rtems_test_assert(!_Resource_Is_most_recently_obtained(&r0, &n0));
  rtems_test_assert(_Resource_Is_most_recently_obtained(&r1, &n0));

  _Resource_Extract(&r1);
  rtems_test_assert(_Resource_Node_owns_resources(&n0));
  rtems_test_assert(_Resource_Is_most_recently_obtained(&r0, &n0));
  rtems_test_assert(!_Resource_Is_most_recently_obtained(&r1, &n0));

  _Resource_Extract(&r0);
  rtems_test_assert(!_Resource_Node_owns_resources(&n0));
  rtems_test_assert(!_Resource_Is_most_recently_obtained(&r0, &n0));
  rtems_test_assert(!_Resource_Is_most_recently_obtained(&r1, &n0));

  _Resource_Add_rival(&r0, &n0);
  rtems_test_assert(!_Chain_Is_empty(&r0.Rivals));

  _Resource_Node_extract(&n0);
  rtems_test_assert(_Chain_Is_empty(&r0.Rivals));
}

static void Init(rtems_task_argument arg)
{
  TEST_BEGIN();

  test_resource_simple();
  test_resource_iterate();

  TEST_END();
  rtems_test_exit(0);
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>

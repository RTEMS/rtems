/*
 * Copyright (c) 2013 Zhongwei Yao.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/freechain.h>

const char rtems_test_name[] = "SPFREECHAIN 1";

typedef struct {
  Chain_Node Node;
  int x;
} test_node;

static rtems_task Init(rtems_task_argument ignored)
{
    Freechain_Control fc;
    test_node *node;
    test_node node2;

    TEST_BEGIN();

    _Freechain_Initialize(&fc, &node2, 1, sizeof(node2));
    rtems_test_assert(_Chain_Node_count_unprotected(&fc.Free) == 1);
    rtems_test_assert(_Chain_First(&fc.Free) == &node2.Node);
    rtems_test_assert(_Chain_Last(&fc.Free) == &node2.Node);

    _Freechain_Initialize(&fc, NULL, 0, sizeof(test_node));
    rtems_test_assert(_Chain_Is_empty(&fc.Free));

    rtems_test_assert(_Freechain_Get(&fc, NULL, 0, sizeof(test_node)) == NULL);

    rtems_test_assert(_Freechain_Get(&fc, malloc, 1, SIZE_MAX) == NULL);

    /* check whether freechain put and get works correctly*/

    _Freechain_Put(&fc, NULL);

    puts( "INIT - Get node from freechain - OK" );
    node = _Freechain_Get(&fc, malloc, 1, sizeof(test_node));
    node->x = 1;

    puts( "INIT - Put node back to freechain - OK" );
    _Freechain_Put(&fc, node);

    puts( "INIT - Verify freechain node put and get - OK" );
    node = _Freechain_Get(&fc, NULL, 0, sizeof(test_node));
    rtems_test_assert(node->x == 1);

    TEST_END();
    rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */

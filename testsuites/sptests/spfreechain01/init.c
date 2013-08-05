/*
 * Copyright (c) 2013 Zhongwei Yao.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/freechain.h>
#include <rtems/score/wkspace.h>
#include <rtems/malloc.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
bool my_freechain_extend_with_nothing( Freechain_Control *freechain );
bool my_freechain_extend_heap( Freechain_Control *freechain );
bool my_freechain_extend_workspace( Freechain_Control *freechain );
void my_freechain_init_heap( Freechain_Control *freechain );
void my_freechain_init_workspace( Freechain_Control *freechain );

typedef struct {
  Freechain_Control super_fc;
  size_t bump_count;
} MyFreechain;

typedef struct {
  Chain_Node ch_node;
  int x;
} test_node;

bool my_freechain_extend_with_nothing( Freechain_Control *freechain )
{
    return NULL;
}

/* user defined extend handle, it allocates memory on heap. */
bool my_freechain_extend_heap( Freechain_Control *freechain )
{
  MyFreechain *self = (MyFreechain *)freechain;
  size_t node_size = sizeof(test_node);
  size_t size = self->bump_count * node_size;
  int i;
  test_node *nodes = malloc(size);

  if (!nodes) {
    printf( "INIT - Unable to allocate free chain of size: %d\n", size );
    return NULL;
  }

  puts( "INIT - Allocate node from heap in user defined freechain extend"
        " - OK" );
  for ( i = 0; i < self->bump_count; i++ ) {
      _Freechain_Put(freechain,
                          nodes + i);
  }
  return true;
}

/* user defined extend handle, it allocates memory on workspace. */
bool my_freechain_extend_workspace( Freechain_Control *freechain )
{
  MyFreechain *self = (MyFreechain *)freechain;
  size_t node_size = sizeof(test_node);
  size_t size = self->bump_count * node_size;
  int i;
  test_node *nodes = _Workspace_Allocate(size);

  if (!nodes) {
    printf( "INIT - Unable to allocate free chain of size: %d\n", size );
    return NULL;
  }

  puts( "INIT - Allocate node from workspace in user defined freechain extend"
        " - OK" );

  for ( i = 0; i < self->bump_count; i++ ) {
      _Freechain_Put(freechain,
                          nodes + i);
  }
  return true;
}

void my_freechain_init_heap( Freechain_Control *freechain )
{
  MyFreechain *self = (MyFreechain *)freechain;
  self->bump_count = 5;
  size_t size = self->bump_count * sizeof(test_node);
  test_node *nodes = malloc(size);

  _Chain_Initialize(
    &freechain->Freechain,
    nodes,
    self->bump_count,
    sizeof(test_node)
    );
}

void my_freechain_init_workspace( Freechain_Control *freechain )
{
  MyFreechain *self = (MyFreechain *)freechain;
  self->bump_count = 7;
  size_t size = self->bump_count * sizeof(test_node);
  test_node *nodes = _Workspace_Allocate(size);

  _Chain_Initialize(
    &freechain->Freechain,
    nodes,
    self->bump_count,
    sizeof(test_node)
    );
}

rtems_task Init(
  rtems_task_argument ignored
                )
{
    puts( "*** START OF RTEMS FREECHAIN API TEST ***" );

    test_node *test_node_p;
    MyFreechain myfc;
    Freechain_Control *fc_p = (Freechain_Control *)&myfc;
    int i;

    /* check whether freechain put and get works correctly*/
    _Freechain_Initialize(fc_p,
                          &my_freechain_extend_with_nothing);
    my_freechain_init_heap(fc_p);

    puts( "INIT - Get node from freechain - OK" );
    test_node_p = (test_node *)_Freechain_Get(fc_p);
    test_node_p->x = 1;

    puts( "INIT - Put node back to freechain - OK" );
    _Freechain_Put(fc_p, (void *)test_node_p);

    puts( "INIT - Verify freechain node put and get - OK" );
    test_node_p = (test_node *)_Freechain_Get(fc_p);
    if(test_node_p->x != 1) {
      puts( "INIT - ERROR ON FREECHAIN GET AND PUT" );
      rtems_test_exit(0);
    }

    /* check whether freechain extend handle on heap works correctly */
    _Freechain_Initialize(fc_p,
                          &my_freechain_extend_heap);
    my_freechain_init_heap(fc_p);

    puts( "INIT - Get more than intialized nodes from freechain on heap - OK" );

    for ( i = 0; i < myfc.bump_count * 2; i++ ) {
        test_node_p = (test_node *)_Freechain_Get(fc_p);
        if (!test_node_p) {
            puts( "INIT - Get node from freechain failed - FAILED" );
            rtems_test_exit(0);
        }
    }

    /* check whether freechain extend handle in workspace works correctly */
    _Freechain_Initialize(fc_p,
                          &my_freechain_extend_workspace);
    my_freechain_init_workspace(fc_p);

    puts( "INIT - Get more than intialized nodes from freechain in workspace"
          " - OK" );

    for ( i = 0; i < myfc.bump_count * 2; i++ ) {
        test_node_p = (test_node *)_Freechain_Get(fc_p);
        if (!test_node_p) {
            puts( "INIT - Get node from freechain failed - FAILED" );
            rtems_test_exit(0);
        }
    }

    puts( "*** END OF RTEMS FREECHAIN API TEST ***" );
    rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_MEMORY_OVERHEAD sizeof(test_node)
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */

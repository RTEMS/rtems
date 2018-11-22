/**
 * @file
 *
 * @brief Thread-Specific Data Key Create
 * @ingroup POSIXAPI
 */

/*
 * COPYRIGHT (c) 1989-2010.
 * On-Line Applications Research Corporation (OAR).
 * Copyright (c) 2016 embedded brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/keyimpl.h>
#include <rtems/score/userextimpl.h>
#include <rtems/score/wkspace.h>
#include <rtems/sysinit.h>

#include <errno.h>

/**
 *  17.1.1 Thread-Specific Data Key Create, P1003.1c/Draft 10, p. 163
 */
int pthread_key_create(
  pthread_key_t  *key,
  void          (*destructor)( void * )
)
{
  POSIX_Keys_Control  *the_key;

  the_key = _POSIX_Keys_Allocate();

  if ( !the_key ) {
    _Objects_Allocator_unlock();
    return EAGAIN;
  }

  the_key->destructor = destructor;
  _Chain_Initialize_empty( &the_key->Key_value_pairs );
  _Objects_Open_u32( &_POSIX_Keys_Information, &the_key->Object, 0 );
  *key = the_key->Object.id;
  _Objects_Allocator_unlock();
  return 0;
}

Freechain_Control _POSIX_Keys_Keypool;

static uint32_t _POSIX_Keys_Get_keypool_bump_count( void )
{
  uint32_t max;

  max = _POSIX_Keys_Key_value_pair_maximum;
  return _Objects_Is_unlimited( max ) ?
    _Objects_Maximum_per_allocation( max ) : 0;
}

static uint32_t _POSIX_Keys_Get_initial_keypool_size( void )
{
  uint32_t max;

  max = _POSIX_Keys_Key_value_pair_maximum;
  return _Objects_Maximum_per_allocation( max );
}

static void _POSIX_Keys_Initialize_keypool( void )
{
  _Freechain_Initialize(
    &_POSIX_Keys_Keypool,
    _POSIX_Keys_Key_value_pairs,
    _POSIX_Keys_Get_initial_keypool_size(),
    sizeof( _POSIX_Keys_Key_value_pairs[ 0 ] )
  );
}

POSIX_Keys_Key_value_pair * _POSIX_Keys_Key_value_allocate( void )
{
  return (POSIX_Keys_Key_value_pair *) _Freechain_Get(
    &_POSIX_Keys_Keypool,
    _Workspace_Allocate,
    _POSIX_Keys_Get_keypool_bump_count(),
    sizeof( POSIX_Keys_Key_value_pair )
  );
}

static void _POSIX_Keys_Run_destructors( Thread_Control *the_thread )
{
  while ( true ) {
    ISR_lock_Context  lock_context;
    RBTree_Node      *node;

    _Objects_Allocator_lock();
    _POSIX_Keys_Key_value_acquire( the_thread, &lock_context );

    node = _RBTree_Root( &the_thread->Keys.Key_value_pairs );
    if ( node != NULL ) {
      POSIX_Keys_Key_value_pair *key_value_pair;
      pthread_key_t              key;
      void                      *value;
      POSIX_Keys_Control        *the_key;
      void                    ( *destructor )( void * );

      key_value_pair = POSIX_KEYS_RBTREE_NODE_TO_KEY_VALUE_PAIR( node );
      key = key_value_pair->key;
      value = key_value_pair->value;
      _RBTree_Extract(
        &the_thread->Keys.Key_value_pairs,
        &key_value_pair->Lookup_node
      );

      _POSIX_Keys_Key_value_release( the_thread, &lock_context );
      _POSIX_Keys_Key_value_free( key_value_pair );

      the_key = _POSIX_Keys_Get( key );
      _Assert( the_key != NULL );
      destructor = the_key->destructor;

      _Objects_Allocator_unlock();

      if ( destructor != NULL && value != NULL ) {
        ( *destructor )( value );
      }
    } else {
      _POSIX_Keys_Key_value_release( the_thread, &lock_context );
      _Objects_Allocator_unlock();
      break;
    }
  }
}

static void _POSIX_Keys_Restart_run_destructors(
  Thread_Control *executing,
  Thread_Control *the_thread
)
{
  (void) executing;
  _POSIX_Keys_Run_destructors( the_thread );
}

static User_extensions_Control _POSIX_Keys_Extensions = {
  .Callouts = {
    .thread_restart = _POSIX_Keys_Restart_run_destructors,
    .thread_terminate = _POSIX_Keys_Run_destructors
  }
};

/**
 * @brief This routine performs the initialization necessary for this manager.
 */
static void _POSIX_Keys_Manager_initialization(void)
{
  _Objects_Initialize_information( &_POSIX_Keys_Information );
  _POSIX_Keys_Initialize_keypool();
  _User_extensions_Add_API_set( &_POSIX_Keys_Extensions );
}

RTEMS_SYSINIT_ITEM(
  _POSIX_Keys_Manager_initialization,
  RTEMS_SYSINIT_POSIX_KEYS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

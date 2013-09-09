/**
 * @file
 *
 * @brief Set Specific Key
 * @ingroup POSIXAPI
 */

/*
 * Copyright (c) 2012 Zhongwei Yao.
 * COPYRIGHT (c) 1989-2007.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/keyimpl.h>
#include <rtems/posix/threadsup.h>
#include <rtems/score/chainimpl.h>

#include <errno.h>

/*
 *  17.1.2 Thread-Specific Data Management, P1003.1c/Draft 10, p. 165
 */

int pthread_setspecific(
  pthread_key_t  key,
  const void    *value
)
{
  POSIX_Keys_Control          *the_key;
  Objects_Locations            location;
  POSIX_Keys_Key_value_pair   *value_pair_ptr;
  POSIX_API_Control           *api;

  the_key = _POSIX_Keys_Get( key, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      value_pair_ptr = _POSIX_Keys_Key_value_pair_allocate();

      if ( !value_pair_ptr ) {
        _Objects_Put( &the_key->Object );

        return ENOMEM;
      }

      value_pair_ptr->key = key;
      value_pair_ptr->thread_id = _Thread_Executing->Object.id;
      value_pair_ptr->value = value;
      if ( _RBTree_Insert_unprotected( &_POSIX_Keys_Key_value_lookup_tree,
                                       &(value_pair_ptr->Key_value_lookup_node) ) ) {
        _Freechain_Put( (Freechain_Control *)&_POSIX_Keys_Keypool,
                        (void *) value_pair_ptr );
        _Objects_Put( &the_key->Object );

        return EAGAIN;
      }

      /** append rb_node to the thread API extension's chain */
      api = (POSIX_API_Control *)\
       (_Thread_Executing->API_Extensions[THREAD_API_POSIX]);
      _Chain_Append_unprotected( &api->Key_Chain, &value_pair_ptr->Key_values_per_thread_node );

      _Objects_Put( &the_key->Object );

      return 0;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:   /* should never happen */
#endif
    case OBJECTS_ERROR:
      break;
  }

  return EINVAL;
}

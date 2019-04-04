/**
 * @file
 *
 * @ingroup RTEMSScoreFreechain
 *
 * @brief Freechain Handler Implementation
 */

/*
 * Copyright (c) 2013 Gedare Bloom.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/freechain.h>
#include <rtems/score/assert.h>

void *_Freechain_Get(
  Freechain_Control   *freechain,
  Freechain_Allocator  allocator,
  size_t               number_nodes_to_extend,
  size_t               node_size
)
{
  _Assert( node_size >= sizeof( Chain_Node ) );

  if ( _Chain_Is_empty( &freechain->Free ) && number_nodes_to_extend > 0 ) {
    void *starting_address;

    starting_address = ( *allocator )( number_nodes_to_extend * node_size );
    number_nodes_to_extend *= ( starting_address != NULL );

    _Chain_Initialize(
      &freechain->Free,
      starting_address,
      number_nodes_to_extend,
      node_size
    );
  }

  return _Chain_Get_unprotected( &freechain->Free );
}

void _Freechain_Put( Freechain_Control *freechain, void *node )
{
  if ( node != NULL ) {
    _Chain_Initialize_node( node );
    _Chain_Prepend_unprotected( &freechain->Free, node );
  }
}

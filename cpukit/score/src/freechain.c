/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreFreechain
 *
 * @brief This source file contains the implementation of
 *   _Freechain_Extend(), _Freechain_Get(), and _Freechain_Put().
 */

/*
 * Copyright (c) 2013 Gedare Bloom.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/freechainimpl.h>
#include <rtems/score/assert.h>

void *_Freechain_Extend(
  Freechain_Control   *freechain,
  Freechain_Allocator  allocator,
  size_t               number_nodes_to_extend,
  size_t               node_size
)
{
  void *starting_address;

  starting_address = ( *allocator )( number_nodes_to_extend * node_size );
  number_nodes_to_extend *= ( starting_address != NULL );

  _Chain_Initialize(
    &freechain->Free,
    starting_address,
    number_nodes_to_extend,
    node_size
  );

  return starting_address;
}

void *_Freechain_Get(
  Freechain_Control   *freechain,
  Freechain_Allocator  allocator,
  size_t               number_nodes_to_extend,
  size_t               node_size
)
{
  _Assert( node_size >= sizeof( Chain_Node ) );

  if ( _Chain_Is_empty( &freechain->Free ) && number_nodes_to_extend > 0 ) {
    _Freechain_Extend(
      freechain,
      allocator,
      number_nodes_to_extend,
      node_size
    );
  }

  return _Chain_Get_unprotected( &freechain->Free );
}

void _Freechain_Put( Freechain_Control *freechain, void *node )
{
  if ( node != NULL ) {
    _Freechain_Push( freechain, node );
  }
}

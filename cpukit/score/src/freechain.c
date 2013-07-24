/**
 * @file
 *
 * @ingroup ScoreFreechain
 *
 * @brief Freechain Handler Implementation
 */

/*
 * Copyright (c) 2013 Gedare Bloom.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/freechain.h>
#include <rtems/score/chainimpl.h>

void _Freechain_Initialize(
    Freechain_Control *freechain,
    Freechain_Extend   extend
)
{
  _Chain_Initialize_empty( &freechain->Freechain );
  freechain->extend = extend;
}

void *_Freechain_Get(Freechain_Control *freechain)
{
  if ( _Chain_Is_empty( &freechain->Freechain ) ) {
    if ( !( *freechain->extend )( freechain ) ) {
      return NULL;
    }
  }

  return _Chain_Get_first_unprotected( &freechain->Freechain );
}

void _Freechain_Put( Freechain_Control *freechain, void *node )
{
  _Chain_Prepend_unprotected( &freechain->Freechain, node );
}

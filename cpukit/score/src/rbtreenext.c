/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreRBTree
 *
 * @brief This source file contains the implementation of
 *   _RBTree_Successor().
 */

/*
 * Copyright (C) 2012 embedded brains GmbH & Co. KG
 * Copyright (C) 2002 Niels Provos <provos@citi.umich.edu>
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

/*
 * The red-black tree code of this file derives from the macros of the
 * FreeBSD <sys/tree.h> file of 2020.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/rbtreeimpl.h>
#include <rtems/score/basedefs.h>

RBTree_Node *_RBTree_Successor( const RBTree_Node *node )
{
  RBTree_Node *elm;

  elm = RTEMS_DECONST( RBTree_Node *, node );

  if ( _RBTree_Right( elm ) ) {
    elm = _RBTree_Right( elm );

    while ( _RBTree_Left( elm ) ) {
      elm = _RBTree_Left( elm );
    }
  } else {
    if (
      _RBTree_Parent( elm ) && ( elm == _RBTree_Left( _RBTree_Parent( elm ) ) )
    ) {
      elm = _RBTree_Parent( elm );
    } else {
      while (
        _RBTree_Parent( elm ) &&
        ( elm == _RBTree_Right( _RBTree_Parent( elm ) ) )
      ) {
        elm = _RBTree_Parent( elm );
      }

      elm = _RBTree_Parent( elm );
    }
  }

  return elm;
}

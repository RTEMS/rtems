/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreRBTree
 *
 * @brief This source file contains the implementation of
 *   _RBTree_Insert_color().
 */

/*
 * Copyright (C) 2010-2012 Gedare Bloom.
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

void _RBTree_Insert_color( RBTree_Control *head, RBTree_Node *elm )
{
  RBTree_Node *parent;
  RBTree_Node *gparent;
  RBTree_Node *tmp;

  while ( _RBTree_Is_red( parent = _RBTree_Parent( elm ) ) ) {
    gparent = _RBTree_Parent( parent );

    if ( parent == _RBTree_Left( gparent ) ) {
      tmp = _RBTree_Right( gparent );

      if ( _RBTree_Is_red( tmp ) ) {
        _RBTree_Set_color( tmp, RTEMS_RB_BLACK );
        _RBTree_Set_black_red( parent, gparent );
        elm = gparent;
        continue;
      }

      if ( _RBTree_Right( parent ) == elm ) {
        (void) _RBTree_Parent_rotate_left( gparent, parent );
        tmp = parent;
        parent = elm;
        elm = tmp;
      }

      _RBTree_Set_black_red( parent, gparent );
      (void) _RBTree_Rotate_right( head, gparent );
    } else {
      tmp = _RBTree_Left( gparent );

      if ( _RBTree_Is_red( tmp ) ) {
        _RBTree_Set_color( tmp, RTEMS_RB_BLACK );
        _RBTree_Set_black_red( parent, gparent );
        elm = gparent;
        continue;
      }

      if ( _RBTree_Left( parent ) == elm ) {
        (void) _RBTree_Parent_rotate_right( gparent, parent );
        tmp = parent;
        parent = elm;
        elm = tmp;
      }

      _RBTree_Set_black_red( parent, gparent );
      (void) _RBTree_Rotate_left( head, gparent );
    }
  }

  _RBTree_Set_color( _RBTree_Root( head ), RTEMS_RB_BLACK );
}

/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreFreechain
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreBarrier which are only used by the implementation.
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

#ifndef _RTEMS_SCORE_FREECHAINIMPL_H
#define _RTEMS_SCORE_FREECHAINIMPL_H

#include <rtems/score/freechain.h>
#include <rtems/score/basedefs.h>
#include <rtems/score/chainimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreFreechain
 *
 * @{
 */

/**
 * @brief Allocator function.
 */
typedef void *( *Freechain_Allocator )( size_t size );

/**
 * @brief Initializes a freechain.
 *
 * This routine initializes the freechain control structure to manage a chain
 * of nodes.  In the case the freechain is empty the extend handler is called to
 * get more nodes.
 *
 * @param[out] freechain The freechain control to initialize.
 * @param[out] initial_nodes Array with the initial nodes.
 * @param number_nodes The initial number of nodes.
 * @param node_size The node size.
 */
static inline void _Freechain_Initialize(
  Freechain_Control   *freechain,
  void                *initial_nodes,
  size_t               number_nodes,
  size_t               node_size
)
{
  _Chain_Initialize(
    &freechain->Free,
    initial_nodes,
    number_nodes,
    node_size
  );
}

/**
 * @brief Return true if the freechain is empty, otherwise false
 *
 * @param freechain The freechain control.
 */
static inline bool _Freechain_Is_empty(
  const Freechain_Control *freechain
)
{
  return _Chain_Is_empty( &freechain->Free );
}

/**
 * @brief Pops a node from the freechain.
 *
 * The freechain shall not be empty.
 *
 * @param freechain The freechain control.
 */
static inline void *_Freechain_Pop( Freechain_Control *freechain )
{
  return _Chain_Get_first_unprotected( &freechain->Free );
}

/**
 * @brief Pushes a node back to the freechain.
 *
 * @param freechain The freechain control.
 * @param node The node to push back.  The node shall not be @c NULL.
 */
void static inline _Freechain_Push(
  Freechain_Control *freechain,
  void              *node
)
{
  _Chain_Initialize_node( node );
  _Chain_Prepend_unprotected( &freechain->Free, node );
}

/**
 * @brief Extend the freechain with new nodes.
 *
 * @param freechain The freechain control.
 * @param allocator The allocator function.
 * @param number_nodes_to_extend The number of nodes to extend.
 * @param node_size The node size.
 *
 * @retval NULL The extend operation failed.
 * @retval nodes Pointer to the new nodes.
 */
void *_Freechain_Extend(
  Freechain_Control   *freechain,
  Freechain_Allocator  allocator,
  size_t               number_nodes_to_extend,
  size_t               node_size
);

/**
 * @brief Gets a node from the freechain.
 *
 * @param[in, out] freechain The freechain control.
 * @param allocator The allocator function.
 * @param number_nodes_to_extend The number of nodes in the case an extend is
 *   necessary due to an empty freechain.
 * @param[in] node_size The node size.
 *
 * @retval NULL The freechain is empty and the extend operation failed.
 * @retval pointer Pointer to a node.  The node ownership passes to the
 * caller.
 */
void *_Freechain_Get(
  Freechain_Control   *freechain,
  Freechain_Allocator  allocator,
  size_t               number_nodes_to_extend,
  size_t               node_size
);

/**
 * @brief Puts a node back onto the freechain.
 *
 * @param[in, out] freechain The freechain control.
 * @param[out] node The node to put back.  The node may be @c NULL, in this case
 *   the function does nothing.
 */
void _Freechain_Put(
  Freechain_Control *freechain,
  void              *node
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

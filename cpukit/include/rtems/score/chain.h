/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreChain
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreChain which are used by the implementation and the API.
 */

/*
 *  Copyright (c) 2010 embedded brains GmbH & Co. KG
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_SCORE_CHAIN_H
#define _RTEMS_SCORE_CHAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScoreChain Chain Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the Chain Handler implementation.
 *
 * The Chain Handler is used to manage sets of entities.  This handler
 * provides two data structures.  The Chain Node data structure is included
 * as the first part of every data structure that will be placed on
 * a chain.  The second data structure is Chain Control which is used
 * to manage a set of Chain Nodes.
 *
 * @{
 */

/**
 *  @brief This structure represents a chain node.
 *
 *  This is used to manage each element (node) which is placed
 *  on a chain.
 *
 *  @note Typically, a more complicated structure will use the
 *        chain package.  The more complicated structure will
 *        include a chain node as the first element in its
 *        control structure.  It will then call the chain package
 *        with a pointer to that node element.  The node pointer
 *        and the higher level structure start at the same address
 *        so the user can cast the pointers back and forth.
 *
 */
typedef struct Chain_Node {
  /** This points to the node after this one on this chain. */
  struct Chain_Node *next;
  /** This points to the node immediate prior to this one on this chain. */
  struct Chain_Node *previous;
} Chain_Node;

/**
 * @brief This union represents a chain control block.
 *
 * This is used to manage a chain.  A chain consists of a doubly
 * linked list of zero or more nodes.
 *
 * @note This implementation does not require special checks for
 *   manipulating the first and last elements on the chain.
 *   To accomplish this the @a Chain_Control structure is
 *   treated as two overlapping @ref Chain_Node structures.
 */
typedef union {
  struct {
    Chain_Node Node;
    Chain_Node *fill;
  } Head;

  struct {
    Chain_Node *fill;
    Chain_Node Node;
  } Tail;
} Chain_Control;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

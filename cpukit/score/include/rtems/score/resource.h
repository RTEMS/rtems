/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_RESOURCE_H
#define _RTEMS_SCORE_RESOURCE_H

#include <rtems/score/basedefs.h>
#include <rtems/score/chain.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup ScoreResource Resource Handler
 *
 * @ingroup Score
 *
 * @brief Support for resource dependency management.
 *
 * A resource is something that has at most one owner at a time and may have
 * multiple rivals in case an owner is present.  The owner and rivals are
 * impersonated via resource nodes.  A resource is represented via the resource
 * control structure.  The resource controls and nodes are organized as trees.
 * It is possible to detect deadlocks via such a resource tree.  The
 * _Resource_Iterate() function can be used to iterate through such a resource
 * tree starting at a top node.
 *
 * The following diagram shows an example resource tree with sixteen resource
 * nodes n0 up to n15 and sixteen resources r0 up to r15.  The root of this
 * tree is n0.  As a use case threads can be associated with resource nodes.
 * In this case a thread represented by node n0 owns resources r0, r1, r2, r3,
 * r6, r11 and r12 and is in the ready state.  The threads represented by nodes
 * n1 up to n15 wait directly or indirectly via resources owned by n0 and are
 * in a blocked state.
 *
 * @dot
 * digraph {
 *   n0 [style=filled, fillcolor=green];
 *   n0 -> r0;
 *   subgraph {
 *     rank=same;
 *     n1 [style=filled, fillcolor=green];
 *     r0 -> n1;
 *     n2 [style=filled, fillcolor=green];
 *     n1 -> n2;
 *     n4 [style=filled, fillcolor=green];
 *     n2 -> n4;
 *     n6 [style=filled, fillcolor=green];
 *     n4 -> n6;
 *     n8 [style=filled, fillcolor=green];
 *     n6 -> n8;
 *     n15 [style=filled, fillcolor=green];
 *     n8 -> n15;
 *   }
 *   n1 -> r5;
 *   subgraph {
 *     rank=same;
 *     n3 [style=filled, fillcolor=green];
 *     r5 -> n3;
 *     n12 [style=filled, fillcolor=green];
 *     n3 -> n12;
 *   }
 *   n3 -> r10;
 *   r10 -> r13;
 *   r13 -> r15;
 *   subgraph {
 *     rank=same;
 *     n10 [style=filled, fillcolor=green];
 *     r15 -> n10;
 *   }
 *   r5 -> r7;
 *   subgraph {
 *     rank=same;
 *     n11 [style=filled, fillcolor=green];
 *     r7 -> n11;
 *     n14 [style=filled, fillcolor=green];
 *     n11 -> n14;
 *   }
 *   n14 -> r4;
 *   r7 -> r8;
 *   subgraph {
 *     rank=same;
 *     n13 [style=filled, fillcolor=green];
 *     r8 -> n13;
 *   }
 *   r8 -> r9;
 *   n8 -> r14;
 *   r0 -> r1;
 *   subgraph {
 *     rank=same;
 *     n7 [style=filled, fillcolor=green];
 *     r1 -> n7;
 *   }
 *   r1 -> r2;
 *   r2 -> r3;
 *   r3 -> r6;
 *   r6 -> r11;
 *   r11 -> r12;
 *   subgraph {
 *     rank=same;
 *     n5 [style=filled, fillcolor=green];
 *     r12 -> n5;
 *     n9 [style=filled, fillcolor=green];
 *     n5 -> n9;
 *   }
 * }
 * @enddot
 *
 * The following example illustrates a deadlock situation.  The root of the
 * tree tries to get ownership of a resource owned by one of its children.
 *
 * @dot
 * digraph {
 *   n0 [style=filled, fillcolor=green];
 *   n0 -> r0;
 *   subgraph {
 *     rank=same;
 *     n1 [style=filled, fillcolor=green];
 *     r0 -> n1;
 *   }
 *   n1 -> r1;
 *   n0 -> r1 [label=deadlock, style=dotted];
 * }
 * @enddot
 *
 * @{
 */

typedef struct Resource_Node Resource_Node;

typedef struct Resource_Control Resource_Control;

/**
 * @brief Resource node to reflect ownership of resources and a dependency on a
 * resource.
 */
struct Resource_Node {
  /**
   * @brief Node to build a chain of rivals depending on a resource.
   *
   * @see Resource_Control::Rivals.
   */
  Chain_Node Node;

  /**
   * @brief A chain of resources owned by this node.
   *
   * @see Resource_Control::Node.
   */
  Chain_Control Resources;

  /**
   * @brief Reference to a resource in case this node has to wait for ownership
   * of this resource.
   *
   * It is @c NULL in case this node has no open resource dependency.
   */
  Resource_Control *dependency;

  /**
   * @brief Reference to the root of the resource tree.
   *
   * The root references itself.
   */
  Resource_Node *root;
};

/**
 * @brief Resource control to manage ownership and rival nodes depending on a
 * resource.
 */
struct Resource_Control {
  /**
   * @brief Node to build a chain of resources owned by a resource node.
   *
   * @see Resource_Node::Resources.
   */
  Chain_Node Node;

  /**
   * @brief A chain of rivals waiting for resource ownership.
   *
   * @see Resource_Node::Node.
   */
  Chain_Control Rivals;

  /**
   * @brief The owner of this resource.
   */
  Resource_Node *owner;
};

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_RESOURCE_H */

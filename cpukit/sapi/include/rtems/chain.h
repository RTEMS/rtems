/**
 * @file rtems/chain.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Chain API in RTEMS. The chain is a double linked list that
 *  is part of the Super Core. This is the published interface to that
 *  code.
 *
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_CHAIN_H
#define _RTEMS_CHAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/system.h>
#include <rtems/score/chain.h>

/**
 * @typedef rtems_chain_node
 *
 * A node that can be manipulated in the chain.
 */
typedef Chain_Node rtems_chain_node;

/**
 * @typedef rtems_chain_control
 *
 * The chain's control anchors the chain.
 */
typedef Chain_Control rtems_chain_control;

/**
 *  @brief Chain initializer for an empty chain with designator @a name.
 */
#define RTEMS_CHAIN_INITIALIZER_EMPTY(name) \
  CHAIN_INITIALIZER_EMPTY(name)

/**
 *  @brief Chain definition for an empty chain with designator @a name.
 */
#define RTEMS_CHAIN_DEFINE_EMPTY(name) \
  CHAIN_DEFINE_EMPTY(name)

#include <rtems/chain.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

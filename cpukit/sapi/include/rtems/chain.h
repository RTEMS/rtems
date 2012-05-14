/**
 * @file
 *
 * @ingroup ClassicChains
 *
 * @brief Chain API.
 */

/*
 *  Copyright (c) 2010 embedded brains GmbH.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_CHAIN_H
#define _RTEMS_CHAIN_H

#include <rtems/system.h>
#include <rtems/score/chain.h>
#include <rtems/rtems/event.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicChains Chains
 *
 * @ingroup ClassicRTEMS
 *
 * @brief Chain API.
 *
 * @{
 */

typedef Chain_Node rtems_chain_node;

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

/** @} */

#include <rtems/chain.inl>

/**
 * @addtogroup ClassicChains
 *
 * @{
 */

/**
 * @brief Appends the @a node to the @a chain and sends the @a events to the
 * @a task if the @a chain was empty before the append.
 *
 * @see rtems_chain_append_with_empty_check() and rtems_event_send().
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID No such task.
 */
rtems_status_code rtems_chain_append_with_notification(
  rtems_chain_control *chain,
  rtems_chain_node *node,
  rtems_id task,
  rtems_event_set events
);

/**
 * @brief Prepends the @a node to the @a chain and sends the @a events to the
 * @a task if the @a chain was empty before the prepend.
 *
 * @see rtems_chain_prepend_with_empty_check() and rtems_event_send().
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID No such task.
 */
rtems_status_code rtems_chain_prepend_with_notification(
  rtems_chain_control *chain,
  rtems_chain_node *node,
  rtems_id task,
  rtems_event_set events
);

/**
 * @brief Gets the first @a node of the @a chain and sends the @a events to the
 * @a task if the @a chain is empty after the get.
 *
 * @see rtems_chain_get_with_empty_check() and rtems_event_send().
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID No such task.
 */
rtems_status_code rtems_chain_get_with_notification(
  rtems_chain_control *chain,
  rtems_id task,
  rtems_event_set events,
  rtems_chain_node **node
);

/**
 * @brief Gets the first @a node of the @a chain and sends the @a events to the
 * @a task if the @a chain is empty afterwards.
 *
 * @see rtems_chain_get() and rtems_event_receive().
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_TIMEOUT Timeout.
 */
rtems_status_code rtems_chain_get_with_wait(
  rtems_chain_control *chain,
  rtems_event_set events,
  rtems_interval timeout,
  rtems_chain_node **node
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

/*
 *  COPYRIGHT (c) 2012 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems_rtld
 *
 * @brief RTEMS Run-Time Link Editor Chain Iterator
 *
 * A means of executing an iterator on a chain.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "rtl-chain-iterator.h"

bool
rtems_rtl_chain_iterate (rtems_chain_control* chain,
                         rtems_chain_iterator iterator,
                         void*                data)
{
  rtems_chain_node* node = rtems_chain_first (chain);
  while (!rtems_chain_is_tail (chain, node))
  {
    rtems_chain_node* next_node = rtems_chain_next (node);
    if (!iterator (node, data))
      return false;
    node = next_node;
  }
  return true;
}

/**
 * Count iterator.
 */
static bool
rtems_rtl_count_iterator (rtems_chain_node* node, void* data)
{
  int* count = data;
  ++(*count);
  return true;
}

int
rtems_rtl_chain_count (rtems_chain_control* chain)
{
  int count = 0;
  rtems_rtl_chain_iterate (chain, rtems_rtl_count_iterator, &count);
  return count;
}

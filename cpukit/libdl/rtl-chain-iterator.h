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
 * @ingroup rtems_rtl
 *
 * @brief RTEMS Run-Time Linker Chain Iterator.
 */

#if !defined (_RTEMS_RTL_CHAIN_ITERATOR_H_)
#define _RTEMS_RTL_CHAIN_ITERATOR_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <rtems/chain.h>

/**
 * Chain iterator handler.
 */
typedef bool (*rtems_chain_iterator) (rtems_chain_node* node, void* data);

/**
 * Iterate a chain of nodes invoking the iterator handler. Supply a data
 * pointer the iterator moves data between the invoker and the iterator.
 *
 * The iterator allows removal of the node from the chain.
 *
 * @param chain The chain of nodes to iterator over.
 * @param iterator The iterator handler called once for each node.
 * @param data Pointer to the data used by the iterator.
 * @retval true The whole chain was iterated over.
 * @retval false The iterator returned false.
 */
bool
rtems_rtl_chain_iterate (rtems_chain_control* chain,
                         rtems_chain_iterator iterator,
                         void*                data);

/**
 * Count the number of nodes on the chain.
 *
 * @param chain The chain to count the nodes of.
 * @return int The number of nodes.
 */
int rtems_rtl_chain_count (rtems_chain_control* chain);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtems_rtl
 *
 * @brief RTEMS Run-Time Linker Chain Iterator.
 */

/*
 *  COPYRIGHT (c) 2012 Chris Johns <chrisj@rtems.org>
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

/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtems_rtld
 *
 * @brief RTEMS Run-Time Link Editor Chain Iterator
 *
 * A means of executing an iterator on a chain.
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

#ifdef HAVE_CONFIG_H
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

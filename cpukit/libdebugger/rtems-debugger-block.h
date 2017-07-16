/*
 * Copyright (c) 2016 Chris Johns <chrisj@rtems.org>.
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Debugger for RTEMS.
 */

#ifndef _RTEMS_DEBUGGER_BLOCK_h
#define _RTEMS_DEBUGGER_BLOCK_h

#include <rtems/rtems-debugger.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * DB server block manages a block of re-sizable memory. The block only
 * grows. As more threads enter the system the block becomes the peak and then
 * sits at that level.
 */
typedef struct rtems_debugger_block
{
  void*  block;  /**< The block of memory. */
  size_t step;   /**< The step size the block is increased by. */
  size_t size;   /**< The size of the elements in the block. */
  size_t count;  /**< The number of elements in the block. */
  size_t level;  /**< The usage level in the block. */
} rtems_debugger_block;

/**
 * Create a block.
 */
extern int rtems_debugger_block_create(rtems_debugger_block* block,
				       size_t                step,
				       size_t                size);

/**
 * Destroy a block.
 */
extern int rtems_debugger_block_destroy(rtems_debugger_block* block);

/**
 * Resize a block.
 */
extern int rtems_debugger_block_resize(rtems_debugger_block* block);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif

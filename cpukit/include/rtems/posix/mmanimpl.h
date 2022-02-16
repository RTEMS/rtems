/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Internal Support for POSIX 1003.1b 6.3.1 - map pages of memory
 *
 */

/*
 * Copyright (c) 2012 Chris Johns
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

#ifndef _RTEMS_POSIX_MMANIMPL_H
#define _RTEMS_POSIX_MMANIMPL_H

#include <rtems/libio_.h>
#include <rtems/chain.h> /* FIXME: use score chains for proper layering? */
#include <rtems/posix/shm.h>

#ifdef __cplusplus
extern "C" {
#endif

/* FIXME: add Doxygen */

/**
 * Every mmap'ed region has a mapping.
 */
typedef struct mmap_mappings_s {
  rtems_chain_node   node;  /**< The mapping chain's node */
  void*              addr;  /**< The address of the mapped memory */
  size_t             len;   /**< The length of memory mapped */
  int                flags; /**< The mapping flags */
  POSIX_Shm_Control *shm;   /**< The shared memory object or NULL */
} mmap_mapping;

extern rtems_chain_control mmap_mappings;

static inline void mmap_mappings_lock_obtain( void )
{
  rtems_libio_lock();
}

static inline void mmap_mappings_lock_release( void )
{
  rtems_libio_unlock();
}

#ifdef __cplusplus
}
#endif

#endif

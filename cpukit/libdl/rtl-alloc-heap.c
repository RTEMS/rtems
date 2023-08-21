/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtems_rtl
 *
 * @brief RTEMS Run-Time Linker Allocator for the standard heap.
 */

/*
 *  COPYRIGHT (c) 2012,2023 Chris Johns <chrisj@rtems.org>
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

#include <stdlib.h>

#include "rtl-alloc-heap.h"

#include <rtems/score/apimutex.h>

void
rtems_rtl_alloc_heap (rtems_rtl_alloc_cmd cmd,
                      rtems_rtl_alloc_tag tag,
                      void**              address,
                      size_t              size)
{
  switch (cmd)
  {
    case RTEMS_RTL_ALLOC_NEW:
      *address = malloc (size);
      break;
    case RTEMS_RTL_ALLOC_DEL:
      free (*address);
      *address = NULL;
      break;
    case RTEMS_RTL_ALLOC_RESIZE:
      *address = realloc (*address, size);
      break;
    case RTEMS_RTL_ALLOC_LOCK:
      _RTEMS_Lock_allocator();
      break;
    case RTEMS_RTL_ALLOC_UNLOCK:
      _RTEMS_Unlock_allocator();
      break;
    default:
      break;
  }
}

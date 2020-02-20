/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSApplicationConfiguration
 *
 * @brief Configuration Options Workspace Support Macros
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

#ifndef _RTEMS_CONFDEFS_WKSPACESUPPORT_H
#define _RTEMS_CONFDEFS_WKSPACESUPPORT_H

#ifndef __CONFIGURATION_TEMPLATE_h
#error "Do not include this file directly, use <rtems/confdefs.h> instead"
#endif

#ifdef CONFIGURE_INIT

#include <rtems/score/heap.h>

#define _Configure_Zero_or_one( _number ) ( ( _number ) != 0 ? 1 : 0 )

#define _Configure_Align_up( _val, _align ) \
  ( ( ( _val ) + ( _align ) - 1) - ( ( _val ) + ( _align ) - 1 ) % ( _align ) )

#define _CONFIGURE_HEAP_MIN_BLOCK_SIZE \
  _Configure_Align_up( sizeof( Heap_Block ), CPU_HEAP_ALIGNMENT )

#define _Configure_From_workspace( _size ) \
  ( (uintptr_t) ( _Configure_Zero_or_one( _size ) \
    * _Configure_Align_up( \
       _size + HEAP_BLOCK_HEADER_SIZE, \
      _CONFIGURE_HEAP_MIN_BLOCK_SIZE \
    ) ) )

#endif /* CONFIGURE_INIT */

#endif /* _RTEMS_CONFDEFS_WKSPACESUPPORT_H */

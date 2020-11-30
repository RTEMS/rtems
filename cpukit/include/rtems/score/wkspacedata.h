/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreWorkspace
 *
 * @brief This header file provides data structures used by the implementation
 *   and the @ref RTEMSImplApplConfig to define ::_Workspace_Size,
 *   ::_Workspace_Is_unified, and ::_Workspace_Malloc_initializer.
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

#ifndef _RTEMS_SCORE_WKSPACEDATA_H
#define _RTEMS_SCORE_WKSPACEDATA_H

#include <rtems/score/basedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Heap_Control;

/**
 * @addtogroup RTEMSScoreWorkspace
 *
 * @{
 */

/**
 * @brief The workspace size in bytes.
 *
 * This constant is defined by the application configuration via
 * <rtems/confdefs.h>.
 */
extern const uintptr_t _Workspace_Size;

/**
 * @brief Indicates if the workspace and C program heap are unified.
 *
 * This constant is defined by the application configuration via
 * <rtems/confdefs.h>.
 */
extern const bool _Workspace_Is_unified;

/**
 * @brief Initializes the C Program Heap separated from the RTEMS Workspace.
 *
 * @return Returns the heap control used for the C Program Heap.
 */
struct Heap_Control *_Workspace_Malloc_initialize_separate( void );

/**
 * @brief Initializes the C Program Heap so that it is unified with the RTEMS
 * Workspace.
 *
 * @return Returns the heap control used for the C Program Heap.
 */
struct Heap_Control *_Workspace_Malloc_initialize_unified( void );

/**
 * @brief This constant provides the C Program Heap initialization handler.
 *
 * This constant is defined by the application configuration option
 * #CONFIGURE_UNIFIED_WORK_AREAS via <rtems/confdefs.h> or a default
 * configuration.
 */
extern struct Heap_Control *( * const _Workspace_Malloc_initializer )( void );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_WKSPACEDATA_H */

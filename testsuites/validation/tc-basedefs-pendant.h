/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsBasedefsValBasedefs
 *
 * @brief Helper file to verify the requirements towards the basedefs.
 *
 * This file contains solely *private* declarations shared by
 * (rtems/)testsuites/validation/tc-basedefs-0.c and
 * (rtems/)testsuites/validation/tc-basedefs-pendant.c.
 */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
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

#ifndef _TC_BASEDEFS_PENDANT_H
#define _TC_BASEDEFS_PENDANT_H

/* The define is for the RTEMS_SYMBOL_NAME tests */
#ifndef __USER_LABEL_PREFIX__
#define __USER_LABEL_PREFIX__
#endif

#include <rtems.h>

/* Remove for C++ code */
#ifdef __cplusplus
extern "C" {
#endif

#define GLOBAL_SYMBOL global_symbol
RTEMS_DECLARE_GLOBAL_SYMBOL( GLOBAL_SYMBOL );

uintptr_t basedefs_get_global_symbol( void );
int basedefs_use_prefixed_symbol_name( void );
int basedefs_use_prefixed_upper_symbol_name( void );
int basedefs_use_prefixed_symbol_id( void );
int basedefs_weak_alias_0_func( int i );
int basedefs_weak_alias_1_func( int i );
int basedefs_weak_0_func( void );
int basedefs_weak_1_func( void );

extern const volatile int basedefs_weak_0_var;

extern const volatile int basedefs_weak_1_var;

RTEMS_MALLOCLIKE void *
basedefs_malloclike_func( size_t size );

RTEMS_ALLOC_SIZE_2( 1, 2 ) void *
basedefs_alloc_size_2_func( size_t size0, size_t size1 );

RTEMS_ALLOC_ALIGN( 3 ) void *
basedefs_alloc_align_func( size_t size, void **p, size_t alignment );

RTEMS_ALLOC_SIZE( 1 ) void *
basedefs_alloc_size_func( size_t size );

RTEMS_ALLOC_SIZE_2( 1, 2 ) void *
basedefs_alloc_size_2_func( size_t size0, size_t size1 );

void basedefs_free( void *ptr );

/* Remove for C++ code */
#ifdef __cplusplus
}
#endif

#endif /* _TC_BASEDEFS_PENDANT_H */

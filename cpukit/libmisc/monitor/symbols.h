/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief RTEMS monitor symbol table functions
 *
 * Entry points for symbol table routines.
 */

/*
 * COPYRIGHT (c) 1989-2022. On-Line Applications Research Corporation (OAR).
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

#ifndef _INCLUDE_SYMBOLS_H
#define _INCLUDE_SYMBOLS_H

#include <rtems/monitor.h>

#ifdef __cplusplus
extern "C" {
#endif

struct _rtems_symbol_t {
    uint32_t   value;
    char            *name;
} ;

#define SYMBOL_STRING_BLOCK_SIZE 4080
typedef struct rtems_symbol_string_block_s {
    struct rtems_symbol_string_block_s *next;
    char   buffer[SYMBOL_STRING_BLOCK_SIZE];
} rtems_symbol_string_block_t;

struct _rtems_symbol_table_t {

    uint32_t   sorted;          /* are symbols sorted right now? */
    uint32_t   growth_factor;   /* % to grow by when needed */
    uint32_t   next;            /* next symbol slot to use when adding */
    uint32_t   size;            /* max # of symbols */

    /*
     * Symbol list -- sorted by address (when we do a lookup)
     */

    rtems_symbol_t  *addresses;         /* symbol array by address */

    /*
     * String pool, unsorted, a list of blocks of string data
     */

    rtems_symbol_string_block_t *string_buffer_head;
    rtems_symbol_string_block_t *string_buffer_current;
    uint32_t   strings_next;      /* next byte to use in this block */

} ;

#define rtems_symbol_name(sp)   ((sp)->name)
#define rtems_symbol_value(sp)  ((sp)->value)

#ifdef __cplusplus
}
#endif

#endif /* ! _INCLUDE_SYMBOLS_H */

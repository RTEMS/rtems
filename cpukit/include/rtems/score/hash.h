/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreHash
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreHash.
 */

/*
 * Copyright (C) 2021 embedded brains GmbH (http://www.embedded-brains.de)
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

#ifndef _RTEMS_SCORE_HASH_H
#define _RTEMS_SCORE_HASH_H

#include <rtems/score/basedefs.h>

#include <sha256.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScoreHash Hash Handler
 *
 * @brief This group contains the Hash Handler implementation.
 *
 * @{
 */

/**
 * @brief This constant represents the size of a hash string.
 *
 * Accounts for a NUL termination of the hash string.
 */
#define HASH_CONTROL_STRING_SIZE 45

/**
 * @brief This type represents a hash value.
 *
 * The hash value is produced by _Hash_Finalize().
 */
typedef struct {
  /**
   * @brief This member contains the hash value encoded as a base64url string.
   *
   * The string is NUL-terminated.
   */
  char chars[ HASH_CONTROL_STRING_SIZE ];
} Hash_Control;

/**
 * @brief Gets the hash value as a NUL-terminated string.
 *
 * @param hash is the hash value.
 *
 * @return Returns the hash value as a NUL-terminated string.
 */
RTEMS_INLINE_ROUTINE const char *_Hash_Get_string( const Hash_Control *hash )
{
  return &hash->chars[ 0 ];
}

/**
 * @brief This type represents the context to compute a hash value.
 */
typedef struct {
  /**
   * @brief This member contains the hash algorithm context.
   */
  SHA256_CTX Context;

  /**
   * @brief This member references a hash value if needed.
   */
  Hash_Control *hash;

  /**
   * @brief This member contains an index into the hash value bytes if needed.
   */
  size_t index;
} Hash_Context;

/**
 * @brief Initializes the hash context.
 *
 * @param[out] context is the hash context to initialize.
 */
RTEMS_INLINE_ROUTINE void _Hash_Initialize( Hash_Context *context )
{
  SHA256_Init( &context->Context );
}

/**
 * @brief Adds the data to the hash value.
 *
 * @param[in, out] context is the hash context.
 *
 * @param begin is the begin address of the data to add.
 *
 * @param size is the size of the data in bytes.
 */
RTEMS_INLINE_ROUTINE void _Hash_Add_data(
  Hash_Context *context,
  const void   *begin,
  size_t        size
)
{
  SHA256_Update( &context->Context, begin, size );
}

/**
 * @brief Adds the string to the hash value.
 *
 * @param[in, out] context is the hash context.
 *
 * @param str is the string to add.
 */
RTEMS_INLINE_ROUTINE void _Hash_Add_string(
  Hash_Context *context,
  const char   *str
)
{
  SHA256_Update( &context->Context, str, strlen( str ) );
}

/**
 * @brief Finalizes the hash value computation and produces the hash value.
 *
 * @param[in, out] context is the hash context.
 *
 * @param[out] hash is the hash control to store the produced hash value.
 */
void _Hash_Finalize( Hash_Context *context, Hash_Control *hash );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_HASH_H */

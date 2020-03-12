/* SPDX-License-Identifier: BSD-2-Clause */

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

#ifndef _RTEMS_RECORDDUMP_H
#define _RTEMS_RECORDDUMP_H

#include <rtems/record.h>

#include <zlib.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup RTEMSRecord
 *
 * @{
 */

/**
 * @brief Handler for record dumps to output a chunk of data.
 *
 * @param arg The argument passed to rtems_record_dump().
 * @param data The begin of the data chunk.
 * @param length The length in bytes of the data chunk.
 */
typedef void ( *rtems_record_dump_chunk )(
  void       *arg,
  const void *data,
  size_t      length
);

/**
 * @brief Dumps the record header, the thread names, and all items of all
 * processors.
 *
 * @param chunk Handler to dump a chunk of data.
 * @param flush Handler to flush the data.
 * @param arg The argument for the handlers.
 */
void rtems_record_dump(
  rtems_record_dump_chunk  chunk,
  void                    *arg
);

/**
 * @brief Dumps the event records in base64 encoding.
 *
 * @param put_char The put char handler.
 * @param arg The argument for the put char handler.
 *
 * @see rtems_record_dump().
 */
void rtems_record_dump_base64( void ( *put_char )( int, void * ), void *arg );

/**
 * @brief The context for record dumps with zlib compression and base64
 * encoding.
 */
typedef struct {
  void        ( *put_char )( int, void * );
  void          *arg;
  int            out;
  unsigned char  buf[ 57 ];
  z_stream       stream;
  char          *mem_begin;
  size_t         mem_available;
  char           mem[ 0x80000 ];
} rtems_record_dump_base64_zlib_context;

/**
 * @brief Dumps the event records compressed with zlib in base64 encoding.
 *
 * @param ctx The context to perform the record dump with zlib compression and
 *   base64 encoding.  This context is too large for normal stack sizes.
 * @param put_char The put char handler.
 * @param arg The argument for the put char handler.
 *
 * @see rtems_record_dump().
 */
void rtems_record_dump_zlib_base64(
  rtems_record_dump_base64_zlib_context *ctx,
  void                                ( *put_char )( int, void * ),
  void                                  *arg
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_RECORDDUMP_H */

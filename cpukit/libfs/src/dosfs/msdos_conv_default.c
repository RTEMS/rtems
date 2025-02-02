/**
 * @file
 *
 * @ingroup DOSFS
 *
 * @brief Default Converter
 */

/*
 * Copyright (C) 2013 embedded brains GmbH & Co. KG
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
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include <rtems/endian.h>
#include <rtems/dosfs.h>
#include "fat.h"
#include "msdos.h"

static int msdos_default_utf8_to_codepage(
  rtems_dosfs_convert_control *super,
  const uint8_t               *src,
  const size_t                 src_size,
  char                        *dst,
  size_t                      *dst_size
)
{
  int    eno = 0;
  size_t bytes_to_copy = MIN( src_size, *dst_size );

  (void) super;

  *dst_size = bytes_to_copy;

  memcpy( dst, src, bytes_to_copy );

  return eno;
}

static int msdos_default_codepage_to_utf8(
  rtems_dosfs_convert_control *super,
  const char                  *src,
  const size_t                 src_size,
  uint8_t                     *dst,
  size_t                      *dst_size
)
{
  int    eno = 0;
  size_t bytes_to_copy = MIN( src_size, *dst_size );

  (void) super;

  *dst_size = bytes_to_copy;

  memcpy( dst, src, bytes_to_copy );

  return eno;
}

static int msdos_default_utf8_to_utf16(
  rtems_dosfs_convert_control *super,
  const uint8_t               *src,
  const size_t                 src_size,
  uint16_t                    *dst,
  size_t                      *dst_size
)
{
  int    eno = 0;
  size_t bytes_to_copy = MIN( src_size, *dst_size / 2);
  size_t i;

  (void) super;

  *dst_size = 2 * bytes_to_copy;

  for ( i = 0; eno == 0 && i < bytes_to_copy; ++i ) {
    uint16_t utf16_native = src[i];

    if ( utf16_native <= 127 ) {
      dst[i] = CT_LE_W( utf16_native );
    } else {
      eno = EINVAL;
    }
  }

  return eno;
}

static int msdos_default_utf16_to_utf8(
  rtems_dosfs_convert_control *super,
  const uint16_t              *src,
  const size_t                 src_size,
  uint8_t                     *dst,
  size_t                      *dst_size
)
{
  int    eno = 0;
  size_t bytes_to_copy = MIN( src_size / 2, *dst_size );
  size_t i;

  (void) super;

  *dst_size = bytes_to_copy;

  for ( i = 0; eno == 0 && i < bytes_to_copy; ++i ) {
    uint16_t utf16_le = src[i];
    uint16_t utf16_native  = CF_LE_W( utf16_le );

    if ( utf16_native <= 127 ) {
      dst[i] = (uint8_t) utf16_native;
    } else {
      eno = EINVAL;
    }
  }

  return eno;
}

static int msdos_default_normalize_and_fold(
  rtems_dosfs_convert_control *super,
  const uint8_t               *src,
  const size_t                 src_size,
  uint8_t                     *dst,
  size_t                      *dst_size
)
{
  int    eno = 0;
  size_t bytes_to_copy = MIN( src_size, *dst_size );
  size_t i;

  (void) super;

  *dst_size = bytes_to_copy;

  for ( i = 0; i < bytes_to_copy; ++i ) {
    dst[i] = tolower( src[i] );
  }

  return eno;
}

static void msdos_default_destroy(
  rtems_dosfs_convert_control *super
)
{
  free( super );
}

static const rtems_dosfs_convert_handler msdos_default_convert_handler = {
  .utf8_to_codepage = msdos_default_utf8_to_codepage,
  .codepage_to_utf8 = msdos_default_codepage_to_utf8,
  .utf8_to_utf16 = msdos_default_utf8_to_utf16,
  .utf16_to_utf8 = msdos_default_utf16_to_utf8,
  .utf8_normalize_and_fold = msdos_default_normalize_and_fold,
  .destroy = msdos_default_destroy
};

typedef struct {
  rtems_dosfs_convert_control super;
  uint8_t buffer[MSDOS_NAME_MAX_LFN_BYTES];
} msdos_default_convert_control;

rtems_dosfs_convert_control *rtems_dosfs_create_default_converter(void)
{
  msdos_default_convert_control *self = malloc( sizeof( *self ) );

  if ( self != NULL ) {
    rtems_dosfs_convert_control *super = &self->super;

    super->handler = &msdos_default_convert_handler;
    super->buffer.data = &self->buffer;
    super->buffer.size = sizeof( self->buffer );
  }

  return &self->super;
}

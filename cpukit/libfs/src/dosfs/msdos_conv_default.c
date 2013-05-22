/**
 * @file
 *
 * @ingroup DOSFS
 *
 * @brief Default Converter
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

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

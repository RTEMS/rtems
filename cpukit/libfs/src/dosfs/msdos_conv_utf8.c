/**
 * @file
 *
 * @ingroup DOSFS
 *
 * @brief UTF-8 Converter
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
 * http://www.rtems.org/license/LICENSE.
 */

#include <stddef.h>
#include <assert.h>
#include <errno.h>
#include <iconv.h>
#include <rtems/dosfs.h>
#include <utf8proc/utf8proc.h>
#include "msdos.h"

#define INVALID_ICONV_DESC ( (iconv_t) -1 )

typedef struct {
  /*
   * This structure must be the first field, since otherwise the cast
   * operations later in the file are invalid.
   */
  rtems_dosfs_convert_control super;

  iconv_t desc_codepage_to_utf8;
  iconv_t desc_utf8_to_codepage;
  iconv_t desc_utf16_to_utf8;
  iconv_t desc_utf8_to_utf16;
  uint8_t buffer[MSDOS_NAME_MAX_UTF8_LFN_BYTES];
} msdos_utf8_convert_control;

static int msdos_utf8_convert_with_iconv(
  iconv_t     desc,
  const void *src,
  size_t      src_size,
  void       *dst,
  size_t     *dst_size
)
{
  int     eno;
  size_t  inbytes_left = src_size;
  size_t  outbytes_left = *dst_size;
  char   *inbuf = (void *) (uintptr_t) src;
  char   *outbuf = dst;
  size_t  iconv_status;

  iconv_status = iconv(
    desc,
    &inbuf,
    &inbytes_left,
    &outbuf,
    &outbytes_left
  );

  *dst_size -= outbytes_left;

  if ( iconv_status == 0 ) {
    eno = 0;
  } else if ( iconv_status == (size_t) -1 ) {
    /*
     * iconv() has detected an error.  The most likely reason seems to be a too
     * small outbuf.
    */
    eno = ENOMEM;
  } else {
    /*
     * The iconv_status contains the number of characters converted in a
     * non-reversible way.  We want to use reversible conversions only.
     * Characters permitted within DOSFS names seem to be reversible.
     */
    eno = EINVAL;
  }

  return eno;
}

static int msdos_utf8_codepage_to_utf8(
  rtems_dosfs_convert_control *super,
  const char                  *src,
  size_t                       src_size,
  uint8_t                     *dst,
  size_t                      *dst_size
)
{
  msdos_utf8_convert_control *self = (msdos_utf8_convert_control *) super;

  return msdos_utf8_convert_with_iconv(
    self->desc_codepage_to_utf8,
    src,
    src_size,
    dst,
    dst_size
  );
}

static int msdos_utf8_utf8_to_codepage(
  rtems_dosfs_convert_control *super,
  const uint8_t               *src,
  size_t                       src_size,
  char                        *dst,
  size_t                      *dst_size
)
{
  msdos_utf8_convert_control *self = (msdos_utf8_convert_control *) super;

  return msdos_utf8_convert_with_iconv(
    self->desc_utf8_to_codepage,
    src,
    src_size,
    dst,
    dst_size
  );
}

static int msdos_utf8_utf16_to_utf8(
  rtems_dosfs_convert_control *super,
  const uint16_t              *src,
  size_t                       src_size,
  uint8_t                     *dst,
  size_t                      *dst_size
)
{
  msdos_utf8_convert_control *self = (msdos_utf8_convert_control *) super;

  return msdos_utf8_convert_with_iconv(
    self->desc_utf16_to_utf8,
    src,
    src_size,
    dst,
    dst_size
  );
}

static int msdos_utf8_utf8_to_utf16(
  rtems_dosfs_convert_control *super,
  const uint8_t               *src,
  size_t                       src_size,
  uint16_t                    *dst,
  size_t                      *dst_size
)
{
  msdos_utf8_convert_control *self = (msdos_utf8_convert_control *) super;

  return msdos_utf8_convert_with_iconv(
    self->desc_utf8_to_utf16,
    src,
    src_size,
    dst,
    dst_size
  );
}

static int msdos_utf8proc_errmsg_to_errno( ssize_t errcode )
{
  int eno = 0;


  switch ( errcode ) {
    case 0:
      eno = 0;
      break;
    case UTF8PROC_ERROR_NOMEM:
      eno = ENOMEM;
      break;
    case UTF8PROC_ERROR_OVERFLOW:
      eno = EOVERFLOW;
      break;
    case UTF8PROC_ERROR_INVALIDUTF8:
      eno = EINVAL;
      break;
    case UTF8PROC_ERROR_NOTASSIGNED:
      eno = EINVAL;
      break;
    case UTF8PROC_ERROR_INVALIDOPTS:
      eno = EINVAL;
      break;
    default:
      eno = ENOENT;
      break;
  }

  return eno;
}

static int msdos_utf8_normalize_and_fold(
  rtems_dosfs_convert_control *super,
  const uint8_t *src,
  const size_t   src_size,
  uint8_t       *dst,
  size_t        *dst_size
)
{
  int      eno              = 0;
  int32_t *unicode_buf      = (int32_t *) dst;
  ssize_t  unicode_buf_size = *dst_size / sizeof( *unicode_buf );
  ssize_t  unicodes_to_reencode;
  ssize_t  result;

  (void) super;

  result = utf8proc_decompose(
    src,
    (ssize_t) src_size,
    unicode_buf,
    unicode_buf_size,
    UTF8PROC_STABLE | UTF8PROC_DECOMPOSE | UTF8PROC_CASEFOLD
  );

  if ( result >= 0 ) {
    if ( result <= unicode_buf_size ) {
      unicodes_to_reencode = result;
    } else {
      unicodes_to_reencode = unicode_buf_size;
      eno = ENOMEM;
    }

    result = utf8proc_reencode(
      unicode_buf,
      unicodes_to_reencode,
      UTF8PROC_STABLE | UTF8PROC_DECOMPOSE
    );

    if ( result >= 0 ) {
      *dst_size = result;
    } else {
      eno = msdos_utf8proc_errmsg_to_errno( result );
    }
  } else {
    eno = msdos_utf8proc_errmsg_to_errno( result );
  }

  return eno;
}

static void msdos_utf8_destroy(
  rtems_dosfs_convert_control *super
)
{
  msdos_utf8_convert_control *self = (msdos_utf8_convert_control *) super;
  int                         rv;

  if ( self->desc_utf16_to_utf8 != INVALID_ICONV_DESC ) {
    rv = iconv_close( self->desc_utf16_to_utf8 );
    assert( rv == 0 );
  }

  if ( self->desc_codepage_to_utf8 != INVALID_ICONV_DESC ) {
    rv = iconv_close( self->desc_codepage_to_utf8 );
    assert( rv == 0 );
  }

  if ( self->desc_utf8_to_codepage != INVALID_ICONV_DESC ) {
    rv = iconv_close( self->desc_utf8_to_codepage );
    assert( rv == 0 );
  }

  if ( self->desc_utf8_to_utf16 != INVALID_ICONV_DESC ) {
    rv = iconv_close( self->desc_utf8_to_utf16 );
    assert( rv == 0 );
  }

  free( self );
}

static const rtems_dosfs_convert_handler msdos_utf8_convert_handler = {
  .utf8_to_codepage = msdos_utf8_utf8_to_codepage,
  .codepage_to_utf8 = msdos_utf8_codepage_to_utf8,
  .utf8_to_utf16 = msdos_utf8_utf8_to_utf16,
  .utf16_to_utf8 = msdos_utf8_utf16_to_utf8,
  .utf8_normalize_and_fold = msdos_utf8_normalize_and_fold,
  .destroy = msdos_utf8_destroy
};

rtems_dosfs_convert_control *rtems_dosfs_create_utf8_converter(
  const char *codepage
)
{
  msdos_utf8_convert_control *self = malloc( sizeof( *self ) );

  if ( self != NULL ) {
    self->desc_codepage_to_utf8 = iconv_open( "UTF-8", codepage );
    self->desc_utf8_to_codepage = iconv_open( codepage, "UTF-8" );
    self->desc_utf16_to_utf8    = iconv_open( "UTF-8", "UTF-16LE" );
    self->desc_utf8_to_utf16    = iconv_open( "UTF-16LE", "UTF-8" );

    if (
      self->desc_utf16_to_utf8 != INVALID_ICONV_DESC
        && self->desc_utf8_to_codepage != INVALID_ICONV_DESC
        && self->desc_codepage_to_utf8 != INVALID_ICONV_DESC
        && self->desc_utf8_to_utf16 != INVALID_ICONV_DESC
    ) {
      rtems_dosfs_convert_control *super = &self->super;

      super->handler = &msdos_utf8_convert_handler;
      super->buffer.data = &self->buffer;
      super->buffer.size = sizeof( self->buffer );
    } else {
      msdos_utf8_destroy( &self->super );
      self = NULL;
    }
  }

  return &self->super;
}

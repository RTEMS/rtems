/**
 * @file
 *
 * @brief Application Interface to FAT Filesystem
 *
 * @ingroup DOSFS
 */

/*
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 *  Modifications to support UTF-8 in the file system are
 *  Copyright (c) 2013 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_DOSFS_H
#define _RTEMS_DOSFS_H

#include <rtems.h>
#include <rtems/libio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rtems_dosfs_convert_control rtems_dosfs_convert_control;

/**
 * @brief Converts from UTF-8 into a specific code page.
 *
 * @param[in,out] self The convert control.
 * @param[in] src A well-formed UTF-8 string to be converted.
 * @param[in] src_size The size of the string in bytes (inludes '\\0' if any).
 * @param[out] dst The address the converted string will get copied to.
 * @param[in,out] dst_size The size of the buffer in bytes respectively the
 * number of bytes written to the buffer.
 *
 * @retval 0 Successful operation.
 * @retval EINVAL Conversion was successful, but is not reversible.
 * @retval ENOMEM Conversion failed (possibly due to insufficient buffer size).
 */
typedef int (*rtems_dosfs_utf8_to_codepage)(
  rtems_dosfs_convert_control *self,
  const uint8_t               *src,
  size_t                       src_size,
  char                        *dst,
  size_t                      *dst_size
);

/**
 * @brief Converts from a specific code page into UTF-8
 *
 * @param[in,out] self The convert control.
 * @param[in] src A well-formed string in code page format.
 * @param[in] src_size The size of the string in bytes (inludes '\\0' if any).
 * @param[out] dst The address the converted string will get copied to.
 * @param[in,out] dst_size The size of the buffer in bytes respectively the
 * number of bytes written to the buffer.
 *
 * @retval 0 Successful operation.
 * @retval EINVAL Conversion was successful, but is not reversible.
 * @retval ENOMEM Conversion failed (possibly due to insufficient buffer size).
 */
typedef int (*rtems_dosfs_codepage_to_utf8)(
  rtems_dosfs_convert_control *self,
  const char                  *src,
  size_t                       src_size,
  uint8_t                     *dst,
  size_t                      *dst_size
);

/**
 * @brief Converts from UTF-8 to UTF-16
 *
 * @param[in,out] self The convert control.
 * @param[in] src A well-formed UTF-8 string to be converted.
 * @param[in] src_size The size of the string in bytes (inludes '\\0' if any).
 * @param[out] dst The address the converted string will get copied to
 * @param[in,out] dst_size The size of the buffer in bytes respectively the
 * number of bytes written to the buffer.
 *
 * @retval 0 Successful operation.
 * @retval EINVAL Conversion was successful, but is not reversible.
 * @retval ENOMEM Conversion failed (possibly due to insufficient buffer size).
 */
typedef int (*rtems_dosfs_utf8_to_utf16)(
  rtems_dosfs_convert_control *self,
  const uint8_t               *src,
  size_t                       src_size,
  uint16_t                    *dst,
  size_t                      *dst_size
);

/**
 * @brief Converts from UTF-16 to UTF-8.
 *
 * @param[in,out] self The convert control.
 * @param[in] src A well-formed UTF-16 string to be converted.
 * @param[in] src_size The size of the string in bytes (inludes '\\0' if any).
 * @param[out] dst The address the converted string will get copied to.
 * @param[in,out] dst_size The size of the buffer in bytes respectively the
 * number of bytes written to the buffer
 *
 * @retval 0 Successful operation.
 * @retval EINVAL Conversion was successful, but is not reversible.
 * @retval ENOMEM Conversion failed (possibly due to insufficient buffer size).
 */
typedef int (*rtems_dosfs_utf16_to_utf8)(
  rtems_dosfs_convert_control *self,
  const uint16_t              *src,
  size_t                       src_size,
  uint8_t                     *dst,
  size_t                      *dst_size
);

/**
 * @brief Converts from UTF-8 to Normalized Form Canonical Decomposition.
 *
 * Does canonical decomposition of the UTF-8 string and in addition
 * also converts upper case alphabetic characters to lower case characters
 *
 * @param[in,out] self The convert control.
 * @param[in] src A well-formed UTF-8 string to be normalized and fold.
 * @param[in] src_size The size of the string in bytes (inludes '\\0' if any).
 * @param[out] dst The address the normalized and fold string will get
 * copied to.
 * @param[in,out] dst_size The size of the buffer in bytes respectively the
 * number of bytes written to the buffer.
 *
 * @retval 0 Successful operation.
 * @retval EINVAL Conversion failed.
 * @retval ENOMEM Conversion failed (possibly due to insufficient buffer size).
 * @retval EOVERFLOW Conversion failed.
 * @retval ENOENT Conversion failed.
 */
typedef int (*rtems_dosfs_utf8_normalize_and_fold)(
  rtems_dosfs_convert_control *self,
  const uint8_t               *src,
  size_t                       src_size,
  uint8_t                     *dst,
  size_t                      *dst_size
);

/**
 * @brief Destroys a convert control structure.
 *
 * @param[in,out] self The convert control for destruction.
 */
typedef void (*rtems_dosfs_convert_destroy)(
  rtems_dosfs_convert_control *self
);

/**
 * @brief FAT filesystem convert handler.
 */
typedef struct {
  rtems_dosfs_utf8_to_codepage        utf8_to_codepage;
  rtems_dosfs_codepage_to_utf8        codepage_to_utf8;
  rtems_dosfs_utf8_to_utf16           utf8_to_utf16;
  rtems_dosfs_utf16_to_utf8           utf16_to_utf8;
  rtems_dosfs_utf8_normalize_and_fold utf8_normalize_and_fold;
  rtems_dosfs_convert_destroy         destroy;
} rtems_dosfs_convert_handler;

typedef struct {
  void   *data;
  size_t  size;
} rtems_dosfs_buffer;

/**
 * @brief FAT filesystem convert control.
 *
 * Short file names are stored in the code page format.  Long file names are
 * stored as little-endian UTF-16.  The convert control determines the format
 * conversions to and from the POSIX file name strings.
 */
struct rtems_dosfs_convert_control {
  const rtems_dosfs_convert_handler *handler;
  rtems_dosfs_buffer                 buffer;
};

/**
 * @defgroup DOSFS FAT Filesystem Support
 *
 * @ingroup FileSystemTypesAndMount
 *
 * @brief FAT file system configuration support, format and mount options.
 *
 * A block device can be formatted with a FAT file system with the
 * msdos_format() function.
 *
 * The FAT file system mount operation can be controlled with FAT file system
 * specific mount options, see @ref rtems_dosfs_mount_options.
 *
 * @{
 */

/**
 * @brief Semaphore count per FAT filesystem instance.
 *
 * This can be used for system configuration via <rtems/confdefs.h>.
 */
#define RTEMS_DOSFS_SEMAPHORES_PER_INSTANCE 1

/**
 * @brief FAT filesystem mount options.
 */
typedef struct {
  /**
   * @brief Converter implementation for new file system instance.
   *
   * Before converters have been added to the RTEMS implementation of the FAT
   * file system, the implementation was:
   * - Short names were saved in code page format (as is still the case).
   * - Long names were not saved in UTF-16 format as mandated by the FAT file
   *   system specification.  Instead the character in the local encoding was
   *   stored to the low byte directly and the high byte was set to zero.
   *
   * There are a few compatibility issues due to a non-standard conform
   * implementation of the FAT file system before the UTF-8 support was added.
   * These following issues affect the default converter and the UTF-8
   * converter:
   * - Before UTF-8 support was added, it was possible to create files with the
   *   the same short name in single case and mixed case in a directory.  It
   *   was for example possible to have files "ABC" and "aBc" in a single
   *   directory.  Now this bug is fixed.
   * - Before UTF-8 support was added, it was possible to create files with a
   *   name length of slightly more than 255 characters.  Now the
   *   implementation adheres exactly to the 255 character limit.
   * - Long file names saved before UTF-8 support was added could contain
   *   non-ASCII characters in the low byte which was saved for a long name
   *   character.  With the default converter this means such files can be read
   *   only by their short file name.  With the UTF-8 converter file names will
   *   be read correctly as long as the characters written with the old
   *   implementation were Latin-1 characters.
   *
   * The following sample code demonstrates how to mount a file
   * system with UTF-8 support:
   * @code
   * #include <errno.h>
   * #include <assert.h>
   * #include <rtems/dosfs.h>
   * #include <rtems/libio.h>
   *
   * static int mount_with_utf8(
   *   const char *device_file,
   *   const char *mount_point
   * )
   * {
   *   rtems_dosfs_convert_control *convert_ctrl;
   *   int                          rv;
   *
   *   convert_ctrl = rtems_dosfs_create_utf8_converter( "CP850" );
   *
   *   if ( convert_ctrl != NULL ) {
   *     rtems_dosfs_mount_options mount_opts;
   *
   *     memset( &mount_opts, 0, sizeof( mount_opts ) );
   *     mount_opts.converter = convert_ctrl;
   *
   *     rv = mount_and_make_target_path(
   *       device_file,
   *       mount_point,
   *       RTEMS_FILESYSTEM_TYPE_DOSFS,
   *       RTEMS_FILESYSTEM_READ_WRITE,
   *       &mount_opts
   *     );
   *   } else {
   *     rv = -1;
   *     errno = ENOMEM;
   *   }
   *
   *   return rv;
   * }
   * @endcode
   *
   * In case you do not want UTF-8 support, you can simply pass a NULL pointer
   * to mount_and_make_target_path() respectively to mount() instead of the
   * mount_opts address.
   *
   * @see rtems_dosfs_create_default_converter() and
   * rtems_dosfs_create_utf8_converter().
   */
  rtems_dosfs_convert_control *converter;
} rtems_dosfs_mount_options;

/**
 * @brief Allocates and initializes a default converter.
 *
 * This default converter will accept only POSIX file names with pure ASCII
 * characters. This largely corresponds to the file name handling before the
 * optional UTF-8 support was added to the RTEMS implementation of the FAT file
 * system.  This handling is mostly backwards compatible to the previous RTEMS
 * implementation of the FAT file system.
 *
 * For backwards compatibility and the previous RTEMS implementation of the FAT
 * file system please see also @ref rtems_dosfs_mount_options and mount().
 *
 * @retval NULL Something failed.
 * @retval other Pointer to initialized converter.
 */
rtems_dosfs_convert_control *rtems_dosfs_create_default_converter(void);

/**
 * @brief Allocates and initializes a UTF-8 converter.
 *
 * This converter will assume that all file names passed to POSIX file handling
 * methods are UTF-8 strings and will convert them to the selected code page
 * for short file names and to UTF-16 for long file names.  This conversion
 * will be done during reading and writing.  These conversions correspond to
 * the specification of the FAT file system.  This handling is mostly backwards
 * compatible to the previous RTEMS implementation of the FAT file system.
 *
 * For backwards compatibility and the previous RTEMS implementation of the FAT
 * file system please see also @ref rtems_dosfs_mount_options and mount().
 *
 * One possible issue with this converter is: When reading file names which
 * have been created with other implementations of the FAT file system, it can
 * happen that during the conversion to UTF-8 a long file name becomes longer
 * and exceeds the 255 bytes limit.  In such a case only the short file name
 * will get read.
 *
 * @param[in] codepage The iconv() identification string for the used code
 * page.
 *
 * @retval NULL Something failed.
 * @retval other Pointer to initialized converter.
 */
rtems_dosfs_convert_control *rtems_dosfs_create_utf8_converter(
  const char *codepage
);

#define MSDOS_FMT_INFO_LEVEL_NONE   (0)
#define MSDOS_FMT_INFO_LEVEL_INFO   (1)
#define MSDOS_FMT_INFO_LEVEL_DETAIL (2)
#define MSDOS_FMT_INFO_LEVEL_DEBUG  (3)

/**
 * @brief FAT file system format request parameters.
 */
typedef struct {
  /**
   * @brief OEM name string or NULL.
   */
  const char *OEMName;

  /**
   * @brief Volume label string or NULL.
   */
  const char *VolLabel;

  /**
   * @brief Sectors per cluster hint.
   *
   * The format procedure may choose another value.  Use 0 as default value.
   */
  uint32_t sectors_per_cluster;

  /**
   * @brief Number of FATs hint.
   *
   * Use 0 as default value.
   */
  uint32_t fat_num;

  /**
   * @brief Minimum files in root directory for FAT12 and FAT16.
   *
   * The format procedure may choose a greater value.  Use 0 as default value.
   */
  uint32_t files_per_root_dir;

  /**
   * @brief Media code.
   *
   * Use 0 as default value.  The default media code is 0xf8.
   */
  uint8_t media;

  /**
   * @brief Quick format.
   *
   * If set to true, then do not clear data sectors to zero.
   */
  bool quick_format;

  /**
   * @brief Do not align FAT, data cluster, and root directory to a cluster
   * boundary.
   */
  bool skip_alignment;

  /**
   * @brief Synchronize device after write operations.
   */
  bool sync_device;

  /**
   * @brief The amount of info to output.
   */
  int info_level;
} msdos_format_request_param_t;

/**
 * @brief Formats a block device with a FAT file system.
 *
 * @param[in] devname The block device path.
 * @param[in] rqdata The FAT file system format request data.  Use NULL for
 * default parameters.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occurred.  The @c errno indicates the error.
 */
int msdos_format (
  const char *devname,
  const msdos_format_request_param_t *rqdata
);

/** @} */

int rtems_dosfs_initialize(rtems_filesystem_mount_table_entry_t *mt_entry,
                           const void                           *data);

#ifdef __cplusplus
}
#endif

#endif

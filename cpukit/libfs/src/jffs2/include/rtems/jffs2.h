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

#ifndef RTEMS_JFFS2_H
#define RTEMS_JFFS2_H

#include <rtems/fs.h>
#include <sys/param.h>
#include <zlib.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct rtems_jffs2_flash_control rtems_jffs2_flash_control;

/**
 * @defgroup JFFS2 Journalling Flash File System Version 2 (JFFS2) Support
 *
 * @ingroup FileSystemTypesAndMount
 *
 * @brief Mount options for the Journalling Flash File System, Version 2
 * (JFFS2).
 *
 * The application must provide flash device geometry information and flash
 * device operations in the flash control structure
 * @ref rtems_jffs2_flash_control.
 *
 * The application can optionally provide a compressor control structure to
 * enable data compression using the selected compression algorithm.
 *
 * The application must enable JFFS2 support with rtems_filesystem_register()
 * or CONFIGURE_FILESYSTEM_JFFS2 via <rtems/confdefs.h>.
 *
 * An example mount with a simple memory based flash device simulation follows.
 * The zlib is used for as the compressor.
 *
 * @code
 * #include <string.h>
 *
 * #include <rtems/jffs2.h>
 * #include <rtems/libio.h>
 *
 * #define BLOCK_SIZE (32UL * 1024UL)
 *
 * #define FLASH_SIZE (32UL * BLOCK_SIZE)
 *
 * typedef struct {
 *   rtems_jffs2_flash_control super;
 *   unsigned char area[FLASH_SIZE];
 * } flash_control;
 *
 * static flash_control *get_flash_control(rtems_jffs2_flash_control *super)
 * {
 *   return (flash_control *) super;
 * }
 *
 * static int flash_read(
 *   rtems_jffs2_flash_control *super,
 *   uint32_t offset,
 *   unsigned char *buffer,
 *   size_t size_of_buffer
 * )
 * {
 *   flash_control *self = get_flash_control(super);
 *   unsigned char *chunk = &self->area[offset];
 *
 *   memcpy(buffer, chunk, size_of_buffer);
 *
 *   return 0;
 * }
 *
 * static int flash_write(
 *   rtems_jffs2_flash_control *super,
 *   uint32_t offset,
 *   const unsigned char *buffer,
 *   size_t size_of_buffer
 * )
 * {
 *   flash_control *self = get_flash_control(super);
 *   unsigned char *chunk = &self->area[offset];
 *   size_t i;
 *
 *   for (i = 0; i < size_of_buffer; ++i) {
 *     chunk[i] &= buffer[i];
 *   }
 *
 *   return 0;
 * }
 *
 * static int flash_erase(
 *   rtems_jffs2_flash_control *super,
 *   uint32_t offset
 * )
 * {
 *   flash_control *self = get_flash_control(super);
 *   unsigned char *chunk = &self->area[offset];
 *
 *   memset(chunk, 0xff, BLOCK_SIZE);
 *
 *   return 0;
 * }
 *
 * static flash_control flash_instance = {
 *   .super = {
 *     .block_size = BLOCK_SIZE,
 *     .flash_size = FLASH_SIZE,
 *     .read = flash_read,
 *     .write = flash_write,
 *     .erase = flash_erase,
 *     .device_identifier = 0xc01dc0fe
 *   }
 * };
 *
 * static rtems_jffs2_compressor_zlib_control compressor_instance = {
 *   .super = {
 *     .compress = rtems_jffs2_compressor_zlib_compress,
 *     .decompress = rtems_jffs2_compressor_zlib_decompress
 *   }
 * };
 *
 * static const rtems_jffs2_mount_data mount_data = {
 *   .flash_control = &flash_instance.super,
 *   .compressor_control = &compressor_instance.super
 * };
 *
 * static void erase_all(void)
 * {
 *   memset(&flash_instance.area[0], 0xff, FLASH_SIZE);
 * }
 *
 * void example_jffs2_mount(const char *mount_dir)
 * {
 *   int rv;
 *
 *   erase_all();
 *
 *   rv = mount_and_make_target_path(
 *     NULL,
 *     mount_dir,
 *     RTEMS_FILESYSTEM_TYPE_JFFS2,
 *     RTEMS_FILESYSTEM_READ_WRITE,
 *     &mount_data
 *   );
 *   assert(rv == 0);
 * }
 * @endcode
 *
 * @{
 */

/**
 * @brief Read from flash operation.
 *
 * @param[in, out] self The flash control.
 * @param[in] offset The offset to read from the flash begin in bytes.
 * @param[out] buffer The buffer receiving the data.
 * @param[in] size_of_buffer The size of the buffer in bytes.
 *
 * @retval 0 Successful operation.
 * @retval -EIO An error occurred.  Please note that the value is negative.
 * @retval other All other values are reserved and must not be used.
 */
typedef int (*rtems_jffs2_flash_read)(
  rtems_jffs2_flash_control *self,
  uint32_t offset,
  unsigned char *buffer,
  size_t size_of_buffer
);

/**
 * @brief Write to flash operation.
 *
 * @param[in, out] self The flash control.
 * @param[in] offset The offset to write from the flash begin in bytes.
 * @param[in] buffer The buffer containing the data to write.
 * @param[in] size_of_buffer The size of the buffer in bytes.
 *
 * @retval 0 Successful operation.
 * @retval -EIO An error occurred.  Please note that the value is negative.
 * @retval other All other values are reserved and must not be used.
 */
typedef int (*rtems_jffs2_flash_write)(
  rtems_jffs2_flash_control *self,
  uint32_t offset,
  const unsigned char *buffer,
  size_t size_of_buffer
);

/**
 * @brief Flash erase operation.
 *
 * This operation must erase one block specified by the offset.
 *
 * @param[in, out] self The flash control.
 * @param[in] offset The offset to erase from the flash begin in bytes.
 *
 * @retval 0 Successful operation.
 * @retval -EIO An error occurred.  Please note that the value is negative.
 * @retval other All other values are reserved and must not be used.
 */
typedef int (*rtems_jffs2_flash_erase)(
  rtems_jffs2_flash_control *self,
  uint32_t offset
);

/**
 * @brief Flash destroy operation.
 *
 * The flash destroy operation is called during unmount of the file system
 * instance.  It can be used to free the resources associated with the now
 * unused flash control
 *
 * @param[in, out] self The flash control.
 */
typedef void (*rtems_jffs2_flash_destroy)(
  rtems_jffs2_flash_control *self
);

/**
 * @brief JFFS2 flash device control.
 */
struct rtems_jffs2_flash_control {
  /**
   * @brief The size in bytes of the erasable unit of the flash device.
   */
  uint32_t block_size;

  /**
   * @brief The size in bytes of the flash device.
   *
   * It must be an integral multiple of the block size.  The flash device must
   * have at least five blocks.
   */
  uint32_t flash_size;

  /**
   * @brief Read from flash operation.
   */
  rtems_jffs2_flash_read read;

  /**
   * @brief Write to flash operation.
   */
  rtems_jffs2_flash_write write;

  /**
   * @brief Flash erase operation.
   */
  rtems_jffs2_flash_erase erase;

  /**
   * @brief Flash destroy operation.
   *
   * This operation is optional and the pointer may be @c NULL.
   */
  rtems_jffs2_flash_destroy destroy;

  /**
   * @brief The device identifier of the flash device.
   *
   * It is used in combination with the inode number to uniquely identify a
   * file system node in the system.
   */
  dev_t device_identifier;
};

typedef struct rtems_jffs2_compressor_control rtems_jffs2_compressor_control;

/**
 * @brief Compress operation.
 *
 * @param[in, out] self The compressor control.
 * @param[in] data_in The uncompressed data.
 * @param[out] cdata_out Pointer to buffer with the compressed data.
 * @param[in, out] datalen On entry, the size in bytes of the uncompressed
 * data.  On exit, the size in bytes of uncompressed data which was actually
 * compressed.
 * @param[in, out] cdatalen On entry, the size in bytes available for
 * compressed data.  On exit, the size in bytes of the actually compressed
 * data.
 *
 * @return The compressor type.
 */
typedef uint16_t (*rtems_jffs2_compressor_compress)(
  rtems_jffs2_compressor_control *self,
  unsigned char *data_in,
  unsigned char *cdata_out,
  uint32_t *datalen,
  uint32_t *cdatalen
);

/**
 * @brief Decompress operation.
 *
 * @param[in, out] self The compressor control.
 * @param[in] comprtype The compressor type.
 * @param[in] cdata_in The compressed data.
 * @param[out] data_out The uncompressed data.
 * @param[in] cdatalen The size in bytes of the compressed data.
 * @param[in] datalen The size in bytes of the uncompressed data.
 *
 * @retval 0 Successful operation.
 * @retval -EIO An error occurred.  Please note that the value is negative.
 * @retval other All other values are reserved and must not be used.
 */
typedef int (*rtems_jffs2_compressor_decompress)(
  rtems_jffs2_compressor_control *self,
  uint16_t comprtype,
  unsigned char *cdata_in,
  unsigned char *data_out,
  uint32_t cdatalen,
  uint32_t datalen
);

/**
 * @brief Compressor destroy operation.
 *
 * The compressor destroy operation is called during unmount of the file system
 * instance.  It can be used to free the resources associated with the now
 * unused compressor operations.
 *
 * @param[in, out] self The compressor control.
 */
typedef void (*rtems_jffs2_compressor_destroy)(
  rtems_jffs2_compressor_control *self
);

/**
 * @brief JFFS2 compressor control.
 */
struct rtems_jffs2_compressor_control {
  /**
   * @brief Compress operation.
   */
  rtems_jffs2_compressor_compress compress;

  /**
   * @brief Decompress operation.
   */
  rtems_jffs2_compressor_decompress decompress;

  /**
   * @brief Compressor destroy operation.
   *
   * This operation is optional and the pointer may be @c NULL.
   */
  rtems_jffs2_compressor_destroy destroy;

  /**
   * @brief Compression buffer.
   */
  unsigned char buffer[PAGE_SIZE];
};

/**
 * @brief RTIME compressor compress operation.
 */
uint16_t rtems_jffs2_compressor_rtime_compress(
  rtems_jffs2_compressor_control *self,
  unsigned char *data_in,
  unsigned char *cdata_out,
  uint32_t *datalen,
  uint32_t *cdatalen
);

/**
 * @brief RTIME compressor decompress operation.
 */
int rtems_jffs2_compressor_rtime_decompress(
  rtems_jffs2_compressor_control *self,
  uint16_t comprtype,
  unsigned char *cdata_in,
  unsigned char *data_out,
  uint32_t cdatalen,
  uint32_t datalen
);

/**
 * @brief ZLIB compressor control structure.
 */
typedef struct {
  rtems_jffs2_compressor_control super;
  z_stream stream;
} rtems_jffs2_compressor_zlib_control;

/**
 * @brief ZLIB compressor compress operation.
 */
uint16_t rtems_jffs2_compressor_zlib_compress(
  rtems_jffs2_compressor_control *self,
  unsigned char *data_in,
  unsigned char *cdata_out,
  uint32_t *datalen,
  uint32_t *cdatalen
);

/**
 * @brief ZLIB compressor decompress operation.
 */
int rtems_jffs2_compressor_zlib_decompress(
  rtems_jffs2_compressor_control *self,
  uint16_t comprtype,
  unsigned char *cdata_in,
  unsigned char *data_out,
  uint32_t cdatalen,
  uint32_t datalen
);

/**
 * @brief JFFS2 mount options.
 *
 * For JFFS2 the mount options are mandatory.
 */
typedef struct {
  /**
   * @brief Flash control.
   */
  rtems_jffs2_flash_control *flash_control;

  /**
   * @brief Compressor control.
   *
   * The compressor is optional and this pointer may be @c NULL.
   */
  rtems_jffs2_compressor_control *compressor_control;
} rtems_jffs2_mount_data;

/**
 * @brief Initialization handler of the JFFS2 file system.
 *
 * @param[in, out] mt_entry The mount table entry.
 * @param[in] data The mount options are mandatory for JFFS2 and data must
 * point to a valid @ref rtems_jffs2_mount_data structure used for this file
 * system instance.
 *
 * @retval 0 Successful operation.
 * @retval -1 An error occurred.  The @c errno indicates the error.
 *
 * @see mount().
 */
int rtems_jffs2_initialize(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void *data
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTEMS_JFFS2_H */

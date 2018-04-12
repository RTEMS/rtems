/*
 *  COPYRIGHT (c) 2012, 2018 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems_rtl
 *
 * @brief RTEMS Run-Time Linker Object File cache buffers a section of the
 *        object file in a buffer to localise read performance.
 *
 * This is a simple object file cache that holds a buffer of data from the
 * offset in the file the read is requested from. Writes are not supported.
 *
 * The cache holds the file descriptor, the offset into the file and the amount
 * of valid data in the cache. If the file is ever modified the user of the
 * cache to responsible for flushing the cache. For example the cache should be
 * flused if the file is closed.
 *
 * The cache can return by reference or by value. By reference allow access to
 * the cache buffer. Do not modify the cache's data. By value will copy the
 * requested data into the user supplied buffer.
 *
 * The read by reference call allows you to probe the file's data. For example
 * a string in an object file can be an unknown length. You can request a read
 * up to the cache's size by reference. The code will attempt to have this data
 * in the buffer. If there is not enough data in the file the length will be
 * modifed to reflect this.
 *
 * You can have more than one cache for a single file all looking at different
 * parts of the file.
 */

#if !defined (_RTEMS_RTL_OBJ_CACHE_H_)
#define _RTEMS_RTL_OBJ_CACHE_H_

#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * The buffer cache.
 */
typedef struct rtems_rtl_obj_cache
{
  int      fd;        /**< The file descriptor of the data in the cache. */
  size_t   file_size; /**< The size of the file. */
  off_t    offset;    /**< The base offset of the buffer. */
  size_t   size;      /**< The size of the cache. */
  size_t   level;     /**< The amount of data in the cache. A file can be
                       * smaller than the cache file. */
  uint8_t* buffer;    /**< The buffer */
} rtems_rtl_obj_cache;

/**
 * Open a cache allocating a single buffer of the size passed. The default
 * state of the cache is flushed. No already open checks are made.
 *
 * @param cache The cache to initialise.
 * @param size The size of the cache.
 * @retval true The cache is open.
 * @retval false The cache is not open. The RTL error is set.
 */
bool rtems_rtl_obj_cache_open (rtems_rtl_obj_cache* cache, size_t size);

/**
 * Close a cache.
 *
 * @param cache The cache to close.
 */
void rtems_rtl_obj_cache_close (rtems_rtl_obj_cache* cache);

/**
 * Flush the cache. Any further read will read the data from the file.
 *
 * @param cache The cache to flush.
 */
void rtems_rtl_obj_cache_flush (rtems_rtl_obj_cache* cache);

/**
 * Read data by reference. The length contains the amount of data that should
 * be available in the cache and referenced by the buffer handle. It must be
 * less than or equal to the size of the cache. This call will return the
 * amount of data that is available. It can be less than you ask if the offset
 * and size is past the end of the file.
 *
 * @param cache The cache to reference data from.
 * @param fd The file descriptor. Must be an open file.
 * @param offset The offset in the file to reference the data to.
 * @param buffer The location to reference the data from.
 * @param length The length of data to reference. Can be modified to a
 *               lesser value and true is still returned so check it.
 * @retval true The data referenced is in the cache.
 * @retval false The read failed and the RTL error has been set.
 */
bool rtems_rtl_obj_cache_read (rtems_rtl_obj_cache* cache,
                               int                  fd,
                               off_t                offset,
                               void**               buffer,
                               size_t*              length);

/**
 * Read data by value. The data is copied to the user supplied buffer.
 *
 * @param cache The cache to read the data from.
 * @param fd The file descriptor. Must be an open file.
 * @param offset The offset in the file to read the data from.
 * @param buffer The location the data is written into.
 * @param length The length of data to read.
 * @retval true The data has been read from the cache.
 * @retval false The read failed and the RTL error has been set.
 */
bool rtems_rtl_obj_cache_read_byval (rtems_rtl_obj_cache* cache,
                                     int                  fd,
                                     off_t                offset,
                                     void*                buffer,
                                     size_t               length);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems-rfs
 *
 * RTEMS File System Data.
 *
 * Access data in the correct byte order for the specific target we are running
 * on.
 *
 * @todo Make direct access on matching byte ordered targets.
 */

#if !defined (_RTEMS_RFS_DATA_H_)
#define _RTEMS_RFS_DATA_H_

#include <stdint.h>

/**
 * Helper function to make sure we have a byte pointer.
 */
#define rtems_rfs_data_ptr(_d) ((uint8_t*)(_d))

/**
 * Helper function to get the data shifted in the correctly sized type.
 */
#define rtems_rfs_data_get(_d, _t, _o, _s) \
  (((_t)(rtems_rfs_data_ptr (_d)[_o])) << (_s))

/**
 * RFS Read Unsigned 8bit Integer
 */
#define rtems_rfs_read_u8(_d) \
  (*rtems_rfs_data_ptr (_d))

/**
 * RFS Read Unsigned 16bit Integer
 */
#define rtems_rfs_read_u16(_d) \
  (rtems_rfs_data_get (_d, uint16_t, 0, 8) | \
   rtems_rfs_data_get (_d, uint16_t, 1, 0))

/**
 * RFS Read Unsigned 32bit Integer
 */
#define rtems_rfs_read_u32(_d) \
  (rtems_rfs_data_get (_d, uint32_t, 0, 24) | \
   rtems_rfs_data_get (_d, uint32_t, 1, 16) | \
   rtems_rfs_data_get (_d, uint32_t, 2, 8) | \
   rtems_rfs_data_get (_d, uint32_t, 3, 0))

/**
 * RFS Write Unsigned 8bit Integer
 */
#define rtems_rfs_write_u8(_d, _v) \
  (*rtems_rfs_data_ptr (_d) = (uint8_t)(_v))

/**
 * RFS Write Unsigned 16bit Integer
 */
#define rtems_rfs_write_u16(_d, _v) \
  do { \
    rtems_rfs_data_ptr (_d)[0] = (uint8_t)(((uint16_t)(_v)) >> 8); \
    rtems_rfs_data_ptr (_d)[1] = (uint8_t)((_v)); \
  } while (0)

/**
 * RFS Write Unsigned 32bit Integer
 */
#define rtems_rfs_write_u32(_d, _v) \
  do { \
    rtems_rfs_data_ptr (_d)[0] = (uint8_t)(((uint32_t)(_v)) >> 24); \
    rtems_rfs_data_ptr (_d)[1] = (uint8_t)(((uint32_t)(_v)) >> 16); \
    rtems_rfs_data_ptr (_d)[2] = (uint8_t)(((uint32_t)(_v)) >> 8);  \
    rtems_rfs_data_ptr (_d)[3] = (uint8_t)((uint32_t)(_v)); \
  } while (0)

#endif

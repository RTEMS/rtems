/**
 * @file
 *
 * @ingroup FatFS
 *
 * @brief RTEMS Disk I/O Interface for FatFs
 */

/*
 Copyright (C) 2025 Sepehr Ganji <sepehrganji79@gmail.com>
*/

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/bdbuf.h>
#include <rtems/libio_.h>

#include "ff.h"
#include "diskio.h"
#include "rtems-fatfs.h"

static fatfs_volume_t volumes[ FF_VOLUMES ];

/*
 * Compiler will change name to rtems_fatfs_disk_status
 */
DSTATUS disk_status( BYTE pdrv )
{
  if ( pdrv >= FF_VOLUMES ) {
    return STA_NOINIT;
  }

  if ( !volumes[ pdrv ].initialized ) {
    return STA_NOINIT;
  }

  return 0;
}

/*
 * Compiler will change name to rtems_fatfs_disk_initialize
 */
DSTATUS disk_initialize( BYTE pdrv )
{
  if ( pdrv >= FF_VOLUMES ) {
    return STA_NOINIT;
  }
  bool is_initialized = volumes[ pdrv ].initialized;
  return is_initialized ? 0 : STA_NOINIT;
}

/*
 * Compiler will change name to rtems_fatfs_disk_read
 */
DRESULT disk_read( BYTE pdrv, BYTE *buff, LBA_t sector, UINT count )
{
  rtems_status_code   sc;
  rtems_bdbuf_buffer *bd;
  uint32_t            block_size;

  if ( disk_status( pdrv ) != 0 ) {
    return RES_NOTRDY;
  }

  rtems_disk_device *dd = volumes[ pdrv ].dd;
  block_size            = dd->block_size;

  for ( size_t i = 0; i < count; i++ ) {
    sc = rtems_bdbuf_read( dd, sector + i, &bd );
    if ( sc != RTEMS_SUCCESSFUL ) {
      return RES_ERROR;
    }

    memcpy( buff + ( i * block_size ), bd->buffer, block_size );

    sc = rtems_bdbuf_release( bd );
    if ( sc != RTEMS_SUCCESSFUL ) {
      return RES_ERROR;
    }
  }

  return RES_OK;
}

/*
 * Compiler will change name to rtems_fatfs_disk_write
 */
DRESULT disk_write( BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count )
{
  rtems_status_code   sc;
  rtems_bdbuf_buffer *bd;
  uint32_t            block_size;

  if ( disk_status( pdrv ) != 0 ) {
    return RES_NOTRDY;
  }

  rtems_disk_device *dd = volumes[ pdrv ].dd;

  block_size = dd->block_size;

  for ( size_t i = 0; i < count; i++ ) {
    sc = rtems_bdbuf_get( dd, sector + i, &bd );
    if ( sc != RTEMS_SUCCESSFUL ) {
      return RES_ERROR;
    }

    memcpy( bd->buffer, buff + ( i * block_size ), block_size );

    sc = rtems_bdbuf_release_modified( bd );
    if ( sc != RTEMS_SUCCESSFUL ) {
      return RES_ERROR;
    }
  }

  return RES_OK;
}

/*
 * Compiler will change name to rtems_fatfs_disk_ioctl
 */
DRESULT disk_ioctl( BYTE pdrv, BYTE cmd, void *buff )
{
  if ( disk_status( pdrv ) != 0 ) {
    return RES_NOTRDY;
  }

  rtems_disk_device *dd = volumes[ pdrv ].dd;

  switch ( cmd ) {
    case CTRL_SYNC: {
      rtems_status_code sc = rtems_bdbuf_syncdev( dd );
      if ( sc != RTEMS_SUCCESSFUL ) {
        return RES_ERROR;
      }
    } break;

    case GET_SECTOR_COUNT:
      *(rtems_blkdev_bnum *) buff = dd->size;
      break;

    case GET_SECTOR_SIZE:
      *(uint32_t *) buff = dd->media_block_size;
      break;

    case GET_BLOCK_SIZE:
      *(uint32_t *) buff = dd->block_size;
      break;

    case CTRL_TRIM:
      break;

    default:
      return RES_PARERR;
  }

  return RES_OK;
}

int fatfs_diskio_register_device( uint8_t pdrv, const char *device_path )
{
  struct stat        stat_buf;
  int                fd;
  rtems_disk_device *dd;
  int                rc;

  if ( pdrv >= FF_VOLUMES ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( volumes[ pdrv ].initialized ) {
    return 0;
  }

  fd = open( device_path, O_RDWR );
  if ( fd < 0 ) {
    return -1;
  }

  rc = fstat( fd, &stat_buf );
  if ( rc != 0 ) {
    close( fd );
    return -1;
  }

  if ( !S_ISBLK( stat_buf.st_mode ) ) {
    close( fd );
    rtems_set_errno_and_return_minus_one( ENXIO );
  }

  rc = rtems_disk_fd_get_disk_device( fd, &dd );
  if ( rc != 0 ) {
    close( fd );
    return -1;
  }

  volumes[ pdrv ].fd          = fd;
  volumes[ pdrv ].dd          = dd;
  volumes[ pdrv ].initialized = true;

  return 0;
}

void fatfs_diskio_unregister_device( uint8_t pdrv )
{
  if ( pdrv >= FF_VOLUMES || !volumes[ pdrv ].initialized ) {
    return;
  }

  close( volumes[ pdrv ].fd );
  volumes[ pdrv ].fd          = -1;
  volumes[ pdrv ].dd          = NULL;
  volumes[ pdrv ].initialized = false;
}

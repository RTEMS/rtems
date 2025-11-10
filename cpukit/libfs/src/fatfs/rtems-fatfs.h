/**
 * @file
 *
 * @ingroup FatFS
 *
 * @brief RTEMS FatFS headers
 */

/*
 Copyright (C) 2025 Sepehr Ganji <sepehrganji79@gmail.com>
*/

#ifndef RTEMS_FATFS_H
#define RTEMS_FATFS_H

#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include <rtems/diskdevs.h>
#include <rtems/libio.h>
#include <rtems/seterr.h>

#include "ff.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DT_REG 8
#define DT_DIR 4

typedef struct {
  int                fd;
  rtems_disk_device *dd;
  bool               initialized;
} fatfs_volume_t;

#define RTEMS_FATFS_POSIX_CACHE_SIZE 64

typedef struct {
  char   path[ 256 ];
  time_t mtime;
  time_t ctime;
  bool   valid;
} rtems_fatfs_posix_cache_entry_t;

typedef struct {
  FATFS                                   fatfs;
  rtems_recursive_mutex                   vol_mutex;
  const rtems_filesystem_file_handlers_r *file_handlers;
  const rtems_filesystem_file_handlers_r *dir_handlers;
  uint8_t                                 drive_number;
  char                                    mount_path[ 256 ];
  char                                    current_dir[ 256 ];
  rtems_fatfs_posix_cache_entry_t posix_cache[ RTEMS_FATFS_POSIX_CACHE_SIZE ];
  int                             posix_cache_next;
} rtems_fatfs_fs_info_t;

typedef struct {
  union {
    FIL file;
    DIR dir;
  } handle;
  FILINFO info;
  bool    is_directory;
  bool    is_open;
  char    path[ 256 ];
  bool    is_root_node;
  int     ref_count;
  time_t  posix_mtime;
  time_t  posix_ctime;
  bool    posix_time_valid;
} rtems_fatfs_node_t;

/* Disk I/O interface */
int  fatfs_diskio_register_device( uint8_t pdrv, const char *device_path );
void fatfs_diskio_unregister_device( uint8_t pdrv );

/* Utility functions */
static inline int rtems_fatfs_get_full_path(
  const rtems_fatfs_fs_info_t *fs_info,
  const char                  *relative_path,
  char                        *full_path,
  size_t                       path_size
)
{
  size_t mount_len = strlen( fs_info->mount_path );
  size_t rel_len   = strlen( relative_path );

  if ( mount_len + rel_len >= path_size ) {
    rtems_set_errno_and_return_minus_one( ENAMETOOLONG );
  }

  memcpy( full_path, fs_info->mount_path, mount_len );
  memcpy( full_path + mount_len, relative_path, rel_len + 1 );

  return 0;
}

static inline int rtems_fatfs_fresult_to_errno( FRESULT fr )
{
  switch ( fr ) {
    case FR_OK:
      return 0;
    case FR_DISK_ERR:
    case FR_INT_ERR:
    case FR_NOT_READY:
      return EIO;
    case FR_NO_FILE:
    case FR_NO_PATH:
      return ENOENT;
    case FR_INVALID_NAME:
      return EINVAL;
    case FR_DENIED:
      return EACCES;
    case FR_EXIST:
      return EEXIST;
    case FR_INVALID_OBJECT:
      return EBADF;
    case FR_WRITE_PROTECTED:
      return EROFS;
    case FR_INVALID_DRIVE:
    case FR_NOT_ENABLED:
    case FR_NO_FILESYSTEM:
      return ENXIO;
    case FR_MKFS_ABORTED:
      return EINVAL;
    case FR_TIMEOUT:
      return ETIMEDOUT;
    case FR_LOCKED:
      return EBUSY;
    case FR_NOT_ENOUGH_CORE:
      return ENOMEM;
    case FR_TOO_MANY_OPEN_FILES:
      return EMFILE;
    case FR_INVALID_PARAMETER:
      return EINVAL;
    default:
      return EIO;
  }
}

static inline void rtems_fatfs_unlock(
  const rtems_filesystem_mount_table_entry_t *mt_entry
)
{
  rtems_fatfs_fs_info_t *fs_info = (rtems_fatfs_fs_info_t *) mt_entry->fs_info;
  rtems_recursive_mutex_unlock( &fs_info->vol_mutex );
}

static inline int rtems_fatfs_resolve_node_path(
  const rtems_fatfs_fs_info_t                *fs_info,
  const rtems_fatfs_node_t                   *node,
  char                                       *full_path,
  size_t                                      path_size,
  const rtems_filesystem_mount_table_entry_t *mt_entry
)
{
  int
    rc = rtems_fatfs_get_full_path( fs_info, node->path, full_path, path_size );
  if ( rc != 0 && mt_entry != NULL ) {
    rtems_fatfs_unlock( mt_entry );
  }
  return rc;
}

static inline void rtems_fatfs_filinfo_to_stat(
  const FILINFO *fno,
  struct stat   *st
)
{
  const char *name       = fno->fname;
  ino_t       inode_hash = 1;

  memset( st, 0, sizeof( *st ) );

  while ( *name != '\0' ) {
    inode_hash = ( inode_hash * 33 ) + (unsigned char) *name;
    name++;
  }
  inode_hash ^= fno->fsize ^ ( fno->fdate << 16 ) ^ fno->ftime;

  st->st_ino     = ( inode_hash != 0 ) ? inode_hash : 1;
  st->st_dev     = 1;
  st->st_size    = (off_t) fno->fsize;
  st->st_blksize = 512;
  st->st_blocks  = ( st->st_size + 511 ) / 512;

  if ( fno->fdate != 0 || fno->ftime != 0 ) {
    struct tm tm;
    memset( &tm, 0, sizeof( tm ) );

    tm.tm_year = ( ( fno->fdate >> 9 ) & 0x7F ) + 80;
    tm.tm_mon  = ( ( fno->fdate >> 5 ) & 0x0F ) - 1;
    tm.tm_mday = fno->fdate & 0x1F;
    tm.tm_hour = ( fno->ftime >> 11 ) & 0x1F;
    tm.tm_min  = ( fno->ftime >> 5 ) & 0x3F;
    tm.tm_sec  = ( fno->ftime & 0x1F ) * 2;

    st->st_mtime = mktime( &tm );
    st->st_ctime = st->st_mtime;
    st->st_atime = st->st_mtime;
  }

  if ( fno->fattrib & AM_DIR ) {
    st->st_mode = S_IFDIR | 0755;
  } else {
    st->st_mode = S_IFREG | 0644;
  }

  if ( fno->fattrib & AM_RDO ) {
    st->st_mode &= ~( S_IWUSR | S_IWGRP | S_IWOTH );
  }
}

static inline void rtems_fatfs_node_to_stat_basic(
  const rtems_fatfs_node_t *node,
  struct stat              *st
)
{
  rtems_fatfs_filinfo_to_stat( &node->info, st );
}

static inline void rtems_fatfs_node_to_stat(
  const rtems_fatfs_node_t *node,
  struct stat              *st,
  rtems_fatfs_fs_info_t    *fs_info
)
{
  rtems_fatfs_filinfo_to_stat( &node->info, st );

  if ( fs_info != NULL ) {
    blksize_t cluster_size = fs_info->fatfs.csize * 512;
    st->st_blksize         = cluster_size;
    blkcnt_t clusters_used = ( st->st_size + cluster_size - 1 ) / cluster_size;
    st->st_blocks          = clusters_used * fs_info->fatfs.csize;
  }

  if ( node->posix_time_valid ) {
    st->st_mtime = node->posix_mtime;
    st->st_ctime = node->posix_ctime;
    st->st_atime = node->posix_mtime;
  } else if ( fs_info != NULL ) {
    for ( int i = 0; i < RTEMS_FATFS_POSIX_CACHE_SIZE; i++ ) {
      if ( fs_info->posix_cache[ i ].valid &&
           strcmp( fs_info->posix_cache[ i ].path, node->path ) == 0 ) {
        st->st_mtime = fs_info->posix_cache[ i ].mtime;
        st->st_ctime = fs_info->posix_cache[ i ].ctime;
        st->st_atime = fs_info->posix_cache[ i ].mtime;
        break;
      }
    }
  }
}

static inline void rtems_fatfs_lock(
  const rtems_filesystem_mount_table_entry_t *mt_entry
)
{
  rtems_fatfs_fs_info_t *fs_info = (rtems_fatfs_fs_info_t *) mt_entry->fs_info;
  rtems_recursive_mutex_lock( &fs_info->vol_mutex );
}

/* Filesystem operations - declared in rtems-fatfs-init.c */
int rtems_fatfs_initialize(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void                           *data
);

void rtems_fatfs_fsunmount_me( rtems_filesystem_mount_table_entry_t *mt_entry );

void rtems_fatfs_eval_path( rtems_filesystem_eval_path_context_t *ctx );

int rtems_fatfs_mknod(
  const rtems_filesystem_location_info_t *parentloc,
  const char                             *name,
  size_t                                  namelen,
  mode_t                                  mode,
  dev_t                                   dev
);

int rtems_fatfs_rmnod(
  const rtems_filesystem_location_info_t *parentloc,
  const rtems_filesystem_location_info_t *loc
);

int rtems_fatfs_rename(
  const rtems_filesystem_location_info_t *old_parent_loc,
  const rtems_filesystem_location_info_t *old_loc,
  const rtems_filesystem_location_info_t *new_parent_loc,
  const char                             *new_name,
  size_t                                  new_namelen
);

int rtems_fatfs_utimens(
  const rtems_filesystem_location_info_t *loc,
  struct timespec                         times[ 2 ]
);

int rtems_fatfs_statvfs(
  const rtems_filesystem_location_info_t *root_loc,
  struct statvfs                         *sb
);

bool rtems_fatfs_are_nodes_equal(
  const rtems_filesystem_location_info_t *a,
  const rtems_filesystem_location_info_t *b
);

int rtems_fatfs_clone_node( rtems_filesystem_location_info_t *loc );

void rtems_fatfs_free_node( const rtems_filesystem_location_info_t *loc );

/* File operations - declared in rtems-fatfs-file.c */
ssize_t rtems_fatfs_file_read( rtems_libio_t *iop, void *buffer, size_t count );

ssize_t rtems_fatfs_file_write(
  rtems_libio_t *iop,
  const void    *buffer,
  size_t         count
);

off_t rtems_fatfs_file_lseek( rtems_libio_t *iop, off_t offset, int whence );

int rtems_fatfs_file_fstat(
  const rtems_filesystem_location_info_t *loc,
  struct stat                            *buf
);

int rtems_fatfs_file_ftruncate( rtems_libio_t *iop, off_t length );

int rtems_fatfs_file_fsync( rtems_libio_t *iop );

/* Directory operations - declared in rtems-fatfs-dir.c */
ssize_t rtems_fatfs_dir_read( rtems_libio_t *iop, void *buffer, size_t count );

int rtems_fatfs_dir_fstat(
  const rtems_filesystem_location_info_t *loc,
  struct stat                            *buf
);

int rtems_fatfs_opendir(
  rtems_libio_t *iop,
  const char    *path,
  int            oflag,
  mode_t         mode
);

int rtems_fatfs_closedir( rtems_libio_t *iop );

int rtems_fatfs_openfile(
  rtems_libio_t *iop,
  const char    *path,
  int            oflag,
  mode_t         mode
);

int rtems_fatfs_closefile( rtems_libio_t *iop );

/* Handler tables */
extern const rtems_filesystem_operations_table rtems_fatfs_ops;
extern const rtems_filesystem_file_handlers_r  rtems_fatfs_file_handlers;
extern const rtems_filesystem_file_handlers_r  rtems_fatfs_dir_handlers;

#ifdef __cplusplus
}
#endif

#endif /* RTEMS_FATFS_H */

/**
 * @file
 *
 * @ingroup FatFS
 *
 * @brief RTEMS FATFS initialization
 */

/*
 Copyright (C) 2025 Sepehr Ganji <sepehrganji79@gmail.com>
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <time.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/bdbuf.h>
#include <rtems/diskdevs.h>
#include <rtems/fatfs.h>
#include <rtems/libio.h>
#include <rtems/libio_.h>
#include <rtems/score/chain.h>
#include <rtems/seterr.h>

#include "ff.h"
#include "rtems-fatfs.h"

static uint8_t next_drive_number = 0;

static int rtems_fatfs_build_full_path(
  const rtems_fatfs_fs_info_t *fs_info,
  const rtems_fatfs_node_t    *parent_node,
  const char                  *name,
  size_t                       namelen,
  char                        *path,
  size_t                       path_size
)
{
  size_t mount_len = strlen( fs_info->mount_path );
  size_t total_len;
  char  *write_pos = path;

  if ( parent_node == NULL ) {
    total_len = mount_len + namelen;
    if ( total_len >= path_size ) {
      rtems_set_errno_and_return_minus_one( ENAMETOOLONG );
    }
    memcpy( write_pos, fs_info->mount_path, mount_len );
    write_pos += mount_len;
    memcpy( write_pos, name, namelen );
    write_pos  += namelen;
    *write_pos  = '\0';
  } else {
    size_t parent_len = strlen( parent_node->path );
    bool   need_slash = ( strcmp( parent_node->path, "/" ) != 0 );
    total_len = mount_len + parent_len + ( need_slash ? 1 : 0 ) + namelen;

    if ( total_len >= path_size ) {
      rtems_set_errno_and_return_minus_one( ENAMETOOLONG );
    }

    memcpy( write_pos, fs_info->mount_path, mount_len );
    write_pos += mount_len;
    memcpy( write_pos, parent_node->path, parent_len );
    write_pos += parent_len;
    if ( need_slash ) {
      *write_pos = '/';
      write_pos++;
    }
    memcpy( write_pos, name, namelen );
    write_pos  += namelen;
    *write_pos  = '\0';
  }

  char *dotdot;
  while ( ( dotdot = strstr( path, "/.." ) ) != NULL ) {
    if ( dotdot[ 3 ] == '\0' || dotdot[ 3 ] == '/' ) {
      char *start = dotdot - 1;
      while ( start > path && *start != '/' ) {
        start--;
      }
      if ( start >= path && *start == '/' ) {
        char *end = dotdot + 3;
        memmove( start, end, strlen( end ) + 1 );
        if ( start == path && *start == '\0' ) {
          strcpy( path, "/" );
        }
      } else {
        break;
      }
    } else {
      break;
    }
  }

  size_t len = strlen( path );
  if ( len > 0 && len <= 2 && path[ len - 1 ] == ':' ) {
    strcat( path, "/" );
  }

  return 0;
}

static void rtems_fatfs_cache_posix_times(
  rtems_fatfs_fs_info_t *fs_info,
  const char            *path,
  time_t                 mtime,
  time_t                 ctime
)
{
  int    index    = fs_info->posix_cache_next;
  size_t path_len = strlen( path );
  if ( path_len >= sizeof( fs_info->posix_cache[ index ].path ) ) {
    path_len = sizeof( fs_info->posix_cache[ index ].path ) - 1;
  }
  memcpy( fs_info->posix_cache[ index ].path, path, path_len );
  fs_info->posix_cache[ index ].path[ path_len ] = '\0';
  fs_info->posix_cache[ index ].mtime            = mtime;
  fs_info->posix_cache[ index ].ctime            = ctime;
  fs_info->posix_cache[ index ].valid            = true;
  fs_info->posix_cache_next = ( fs_info->posix_cache_next + 1 ) %
                              RTEMS_FATFS_POSIX_CACHE_SIZE;
}

static void rtems_fatfs_set_parent_timestamps(
  const rtems_fatfs_node_t *parent_node
)
{
  if ( parent_node != NULL && parent_node->is_directory ) {
    rtems_fatfs_node_t *mutable_parent = (rtems_fatfs_node_t *) parent_node;
    time_t              current_time   = time( NULL );

    mutable_parent->posix_mtime      = current_time;
    mutable_parent->posix_ctime      = current_time;
    mutable_parent->posix_time_valid = true;
  }
}

int rtems_fatfs_initialize(
  rtems_filesystem_mount_table_entry_t *mt_entry,
  const void                           *data
)
{
  (void) data;

  rtems_fatfs_fs_info_t *fs_info   = NULL;
  rtems_fatfs_node_t    *root_node = NULL;
  FRESULT                fr;
  char                   drive_path[ 8 ];
  int                    rc = -1;

  fs_info = calloc( 1, sizeof( *fs_info ) );
  if ( fs_info == NULL ) {
    rtems_set_errno_and_return_minus_one( ENOMEM );
  }

  root_node = calloc( 1, sizeof( *root_node ) );
  if ( root_node == NULL ) {
    free( fs_info );
    rtems_set_errno_and_return_minus_one( ENOMEM );
  }

  rtems_recursive_mutex_init(
    &fs_info->vol_mutex,
    RTEMS_FILESYSTEM_TYPE_FATFS
  );

  fs_info->drive_number = next_drive_number++;
  if ( next_drive_number >= FF_VOLUMES ) {
    next_drive_number = 0;
  }

  rc = fatfs_diskio_register_device( fs_info->drive_number, mt_entry->dev );
  if ( rc != 0 ) {
    rtems_recursive_mutex_destroy( &fs_info->vol_mutex );
    free( root_node );
    free( fs_info );
    return -1;
  }

  drive_path[ 0 ] = '0' + fs_info->drive_number;
  drive_path[ 1 ] = ':';
  drive_path[ 2 ] = '\0';
  fr              = f_mount( &fs_info->fatfs, drive_path, 1 );
  if ( fr != FR_OK ) {
    fatfs_diskio_unregister_device( fs_info->drive_number );
    rtems_recursive_mutex_destroy( &fs_info->vol_mutex );
    free( root_node );
    free( fs_info );
    errno = rtems_fatfs_fresult_to_errno( fr );
    return -1;
  }

  fs_info->file_handlers = &rtems_fatfs_file_handlers;
  fs_info->dir_handlers  = &rtems_fatfs_dir_handlers;

  strncpy( fs_info->mount_path, drive_path, sizeof( fs_info->mount_path ) - 1 );
  fs_info->mount_path[ sizeof( fs_info->mount_path ) - 1 ] = '\0';

  strcpy( fs_info->current_dir, "/" );

  memset( fs_info->posix_cache, 0, sizeof( fs_info->posix_cache ) );
  fs_info->posix_cache_next = 0;

  root_node->is_directory = true;
  root_node->is_open      = false;
  root_node->is_root_node = true;
  root_node->ref_count    = 1;
  strcpy( root_node->path, "/" );

  memset( &root_node->info, 0, sizeof( root_node->info ) );
  root_node->info.fattrib = AM_DIR;
  root_node->info.fsize   = 0;
  root_node->info.fdate   = 0;
  root_node->info.ftime   = 0;
  strcpy( root_node->info.fname, "/" );

  mt_entry->fs_info                          = fs_info;
  mt_entry->ops                              = &rtems_fatfs_ops;
  mt_entry->mt_fs_root->location.node_access = root_node;
  mt_entry->mt_fs_root->location.handlers    = fs_info->dir_handlers;

  return 0;
}

void rtems_fatfs_fsunmount_me( rtems_filesystem_mount_table_entry_t *mt_entry )
{
  rtems_fatfs_fs_info_t *fs_info = (rtems_fatfs_fs_info_t *) mt_entry->fs_info;
  rtems_fatfs_node_t    *root_node = NULL;

  if ( fs_info != NULL ) {
    if ( mt_entry->mt_fs_root != NULL ) {
      root_node = (rtems_fatfs_node_t *)
                    mt_entry->mt_fs_root->location.node_access;
    }

    f_mount( NULL, fs_info->mount_path, 1 );

    fatfs_diskio_unregister_device( fs_info->drive_number );

    if ( root_node != NULL ) {
      free( root_node );
    }

    rtems_recursive_mutex_destroy( &fs_info->vol_mutex );

    free( fs_info );
  }
}

static rtems_fatfs_node_t *rtems_fatfs_node_get( rtems_fatfs_node_t *node )
{
  if ( node != NULL ) {
    node->ref_count++;
  }
  return node;
}

static void rtems_fatfs_node_put( rtems_fatfs_node_t *node )
{
  if ( node != NULL ) {
    if ( node->is_root_node ) {
      node->ref_count--;
      return;
    }

    node->ref_count--;
    if ( node->ref_count <= 0 ) {
      if ( node->is_open ) {
        if ( node->is_directory ) {
          f_closedir( &node->handle.dir );
        } else {
          f_close( &node->handle.file );
        }
      }

      free( node );
    }
  }
}

static int rtems_fatfs_find_name(
  rtems_filesystem_location_info_t *parentloc,
  const char                       *name,
  size_t                            name_len
)
{
  rtems_fatfs_fs_info_t *fs_info     = parentloc->mt_entry->fs_info;
  rtems_fatfs_node_t    *parent_node = parentloc->node_access;
  rtems_fatfs_node_t    *new_node    = NULL;
  FRESULT                fr;
  FILINFO                fno;
  char                   fatfs_path[ 256 ];
  int                    ret;

  ret = rtems_fatfs_build_full_path(
    fs_info,
    parent_node,
    name,
    name_len,
    fatfs_path,
    sizeof( fatfs_path )
  );
  if ( ret != 0 ) {
    return ret;
  }

  if ( strlen( fatfs_path ) == 3 && fatfs_path[ 1 ] == ':' &&
       fatfs_path[ 2 ] == '/' ) {
    rtems_filesystem_mount_table_entry_t *mt_entry = parentloc->mt_entry;
    if ( mt_entry && mt_entry->mt_fs_root ) {
      new_node = (rtems_fatfs_node_t *)
                   mt_entry->mt_fs_root->location.node_access;
      if ( new_node ) {
        rtems_fatfs_node_get( new_node );
        rtems_fatfs_node_put( parent_node );
        parentloc->node_access = new_node;
        return 0;
      }
    }
    rtems_set_errno_and_return_minus_one( ENOENT );
  }

  fr = f_stat( fatfs_path, &fno );
  if ( fr != FR_OK ) {
    errno = rtems_fatfs_fresult_to_errno( fr );
    return -1;
  }

  new_node = malloc( sizeof( *new_node ) );
  if ( new_node == NULL ) {
    rtems_set_errno_and_return_minus_one( ENOMEM );
  }
  memset( new_node, 0, sizeof( *new_node ) );

  new_node->info         = fno;
  new_node->is_directory = ( fno.fattrib & AM_DIR ) != 0;
  new_node->is_open      = false;
  new_node->is_root_node = false;
  new_node->ref_count    = 1;

  size_t total_len;
  if ( strcmp( parent_node->path, "/" ) == 0 ) {
    total_len = 1 + name_len; // "/" + name
    if ( total_len >= sizeof( new_node->path ) ) {
      free( new_node );
      rtems_set_errno_and_return_minus_one( ENAMETOOLONG );
    }
    new_node->path[ 0 ] = '/';
    memcpy( new_node->path + 1, name, name_len );
    new_node->path[ total_len ] = '\0';
  } else {
    size_t parent_len = strlen( parent_node->path );
    total_len         = parent_len + 1 + name_len; // parent + "/" + name
    if ( total_len >= sizeof( new_node->path ) ) {
      free( new_node );
      rtems_set_errno_and_return_minus_one( ENAMETOOLONG );
    }
    memcpy( new_node->path, parent_node->path, parent_len );
    new_node->path[ parent_len ] = '/';
    memcpy( new_node->path + parent_len + 1, name, name_len );
    new_node->path[ total_len ] = '\0';
  }

  rtems_fatfs_node_put( parent_node );

  parentloc->node_access = new_node;
  return 0;
}

static void rtems_fatfs_set_handlers( rtems_filesystem_location_info_t *loc )
{
  rtems_fatfs_fs_info_t *fs_info = loc->mt_entry->fs_info;
  rtems_fatfs_node_t    *node    = loc->node_access;

  if ( node->is_directory ) {
    loc->handlers = fs_info->dir_handlers;
  } else {
    loc->handlers = fs_info->file_handlers;
  }
}

static bool rtems_fatfs_eval_is_directory(
  rtems_filesystem_eval_path_context_t *ctx,
  void                                 *arg
)
{
  (void) arg;

  rtems_fatfs_node_t *node = (rtems_fatfs_node_t *) ctx->currentloc.node_access;
  return node != NULL && node->is_directory;
}

static rtems_filesystem_eval_path_generic_status rtems_fatfs_eval_token(
  rtems_filesystem_eval_path_context_t *ctx,
  void                                 *arg,
  const char                           *token,
  size_t                                tokenlen
)
{
  (void) arg;

  rtems_filesystem_eval_path_generic_status
    status = RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_DONE;

  if ( rtems_filesystem_is_current_directory( token, tokenlen ) ) {
    rtems_filesystem_eval_path_clear_token( ctx );
    status = RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_CONTINUE;
  } else {
    rtems_filesystem_location_info_t
      *currentloc = rtems_filesystem_eval_path_get_currentloc( ctx );

    int rc = rtems_fatfs_find_name( currentloc, token, tokenlen );

    if ( rc == 0 ) {
      rtems_filesystem_eval_path_clear_token( ctx );
      rtems_fatfs_set_handlers( currentloc );

      if ( rtems_filesystem_eval_path_has_path( ctx ) ) {
        rtems_fatfs_node_t *node = (rtems_fatfs_node_t *)
                                     currentloc->node_access;
        if ( node != NULL && !node->is_directory ) {
          rtems_filesystem_eval_path_error( ctx, ENOTDIR );
          status = RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_DONE;
        } else {
          status = RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_CONTINUE;
        }
      } else {
        status = RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_DONE;
      }
    } else {
      if ( errno == ENOENT ) {
        status = RTEMS_FILESYSTEM_EVAL_PATH_GENERIC_NO_ENTRY;
      } else {
        rtems_filesystem_eval_path_error( ctx, errno );
      }
    }
  }

  return status;
}

static const rtems_filesystem_eval_path_generic_config rtems_fatfs_eval_config =
  { .is_directory = rtems_fatfs_eval_is_directory,
    .eval_token   = rtems_fatfs_eval_token };

void rtems_fatfs_eval_path( rtems_filesystem_eval_path_context_t *ctx )
{
  rtems_filesystem_eval_path_generic( ctx, NULL, &rtems_fatfs_eval_config );
}

int rtems_fatfs_mknod(
  const rtems_filesystem_location_info_t *parentloc,
  const char                             *name,
  size_t                                  namelen,
  mode_t                                  mode,
  dev_t                                   dev
)
{
  char    full_path[ 256 ];
  FRESULT fr;
  FIL     fil;
  int     rc;

  (void) dev;

  rtems_fatfs_fs_info_t *fs_info = (rtems_fatfs_fs_info_t *)
                                     parentloc->mt_entry->fs_info;
  rtems_fatfs_node_t *parent_node = (rtems_fatfs_node_t *)
                                      parentloc->node_access;

  rc = rtems_fatfs_build_full_path(
    fs_info,
    parent_node,
    name,
    namelen,
    full_path,
    sizeof( full_path )
  );

  if ( rc != 0 ) {
    return rc;
  }

  rtems_fatfs_lock( parentloc->mt_entry );

  if ( S_ISDIR( mode ) ) {
    fr = f_mkdir( full_path );
    if ( fr == FR_OK ) {
      rtems_fatfs_set_parent_timestamps( parent_node );
    }
  } else if ( S_ISREG( mode ) ) {
    fr = f_open( &fil, full_path, FA_CREATE_NEW | FA_WRITE );
    if ( fr == FR_OK ) {
      f_close( &fil );
      rtems_fatfs_set_parent_timestamps( parent_node );
    }
  } else {
    rtems_fatfs_unlock( parentloc->mt_entry );
    rtems_set_errno_and_return_minus_one( ENOTSUP );
  }

  rtems_fatfs_unlock( parentloc->mt_entry );

  if ( fr != FR_OK ) {
    errno = rtems_fatfs_fresult_to_errno( fr );
    return -1;
  }

  return 0;
}

int rtems_fatfs_rmnod(
  const rtems_filesystem_location_info_t *parentloc,
  const rtems_filesystem_location_info_t *loc
)
{
  rtems_fatfs_fs_info_t *fs_info = NULL;
  rtems_fatfs_node_t    *node    = NULL;
  char                   full_path[ 256 ];
  FRESULT                fr;
  int                    rc;

  fs_info = (rtems_fatfs_fs_info_t *) parentloc->mt_entry->fs_info;
  node    = (rtems_fatfs_node_t *) loc->node_access;

  if ( node == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  if ( node->is_open && !node->is_directory ) {
    f_close( &node->handle.file );
    node->is_open = false;
  }

  rc = rtems_fatfs_resolve_node_path(
    fs_info,
    node,
    full_path,
    sizeof( full_path ),
    NULL
  );
  if ( rc != 0 ) {
    return rc;
  }

  rtems_fatfs_lock( parentloc->mt_entry );

  fr = f_unlink( full_path );

  rtems_fatfs_unlock( parentloc->mt_entry );

  if ( fr != FR_OK ) {
    if ( ( fr == FR_DENIED || fr == FR_INVALID_NAME ) && node->is_directory ) {
      errno = ENOTEMPTY;
    } else {
      errno = rtems_fatfs_fresult_to_errno( fr );
    }
    return -1;
  }

  return 0;
}

bool rtems_fatfs_are_nodes_equal(
  const rtems_filesystem_location_info_t *a,
  const rtems_filesystem_location_info_t *b
)
{
  rtems_fatfs_node_t *node_a = (rtems_fatfs_node_t *) a->node_access;
  rtems_fatfs_node_t *node_b = (rtems_fatfs_node_t *) b->node_access;

  if ( node_a == NULL || node_b == NULL ) {
    return false;
  }

  return strcmp( node_a->path, node_b->path ) == 0;
}

int rtems_fatfs_clone_node( rtems_filesystem_location_info_t *loc )
{
  rtems_fatfs_node_t *node = (rtems_fatfs_node_t *) loc->node_access;

  if ( node != NULL ) {
    rtems_fatfs_node_get( node );
  }

  return 0;
}

void rtems_fatfs_free_node( const rtems_filesystem_location_info_t *loc )
{
  rtems_fatfs_node_t *node = (rtems_fatfs_node_t *) loc->node_access;

  if ( node != NULL ) {
    rtems_fatfs_node_put( node );
  }
}

int rtems_fatfs_rename(
  const rtems_filesystem_location_info_t *old_parent_loc,
  const rtems_filesystem_location_info_t *old_loc,
  const rtems_filesystem_location_info_t *new_parent_loc,
  const char                             *new_name,
  size_t                                  new_namelen
)
{
  rtems_fatfs_fs_info_t *fs_info  = NULL;
  rtems_fatfs_node_t    *old_node = NULL;
  char                   old_path[ 256 ];
  char                   new_path[ 256 ];
  FRESULT                fr;
  int                    rc;

  fs_info  = (rtems_fatfs_fs_info_t *) old_parent_loc->mt_entry->fs_info;
  old_node = (rtems_fatfs_node_t *) old_loc->node_access;

  if ( old_node == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  rc = rtems_fatfs_resolve_node_path(
    fs_info,
    old_node,
    old_path,
    sizeof( old_path ),
    NULL
  );
  if ( rc != 0 ) {
    return rc;
  }

  rtems_fatfs_fs_info_t *new_fs_info = (rtems_fatfs_fs_info_t *)
                                         new_parent_loc->mt_entry->fs_info;
  rtems_fatfs_node_t *new_parent_node = (rtems_fatfs_node_t *)
                                          new_parent_loc->node_access;
  rc = rtems_fatfs_build_full_path(
    new_fs_info,
    new_parent_node,
    new_name,
    new_namelen,
    new_path,
    sizeof( new_path )
  );
  if ( rc != 0 ) {
    return rc;
  }

  rtems_fatfs_lock( old_parent_loc->mt_entry );

  fr = f_rename( old_path, new_path );

  rtems_fatfs_unlock( old_parent_loc->mt_entry );

  if ( fr != FR_OK ) {
    errno = rtems_fatfs_fresult_to_errno( fr );
    return -1;
  }

  return 0;
}

static void rtems_fatfs_update_fat_timestamp(
  const char *path,
  time_t      timestamp
)
{
  struct tm *tm_info = localtime( &timestamp );
  if ( tm_info != NULL ) {
    FILINFO fno;
    memset( &fno, 0, sizeof( fno ) );

    fno.fdate = ( ( tm_info->tm_year + 1900 - 1980 ) << 9 ) |
                ( ( tm_info->tm_mon + 1 ) << 5 ) | ( tm_info->tm_mday );
    fno.ftime = ( tm_info->tm_hour << 11 ) | ( tm_info->tm_min << 5 ) |
                ( tm_info->tm_sec >> 1 );

    f_utime( path, &fno );
  }
}

int rtems_fatfs_utimens(
  const rtems_filesystem_location_info_t *loc,
  struct timespec                         times[ 2 ]
)
{
  rtems_fatfs_fs_info_t *fs_info = (rtems_fatfs_fs_info_t *)
                                     loc->mt_entry->fs_info;
  rtems_fatfs_node_t *node = (rtems_fatfs_node_t *) loc->node_access;
  char                full_path[ 256 ];
  int                 rc;

  if ( node == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  time_t mtime        = times[ 1 ].tv_sec;
  time_t current_time = time( NULL );

  rc = rtems_fatfs_resolve_node_path(
    fs_info,
    node,
    full_path,
    sizeof( full_path ),
    NULL
  );
  if ( rc != 0 ) {
    return rc;
  }

  rtems_fatfs_lock( loc->mt_entry );
  rtems_fatfs_update_fat_timestamp( full_path, mtime );
  rtems_fatfs_unlock( loc->mt_entry );

  rtems_fatfs_cache_posix_times( fs_info, node->path, mtime, current_time );

  node->posix_mtime      = mtime;
  node->posix_ctime      = current_time;
  node->posix_time_valid = true;

  return 0;
}

/*
 * Compiler will change name to rtems_fatfs_get_fattime
 */
DWORD get_fattime( void )
{
  time_t     current_time;
  struct tm *tm_info;
  DWORD      fat_time = 0;

  current_time = time( NULL );
  tm_info      = localtime( &current_time );

  if ( tm_info != NULL ) {
    fat_time = ( (DWORD) ( tm_info->tm_year + 1900 - 1980 ) << 25 ) |
               ( (DWORD) ( tm_info->tm_mon + 1 ) << 21 ) |
               ( (DWORD) tm_info->tm_mday << 16 ) |
               ( (DWORD) tm_info->tm_hour << 11 ) |
               ( (DWORD) tm_info->tm_min << 5 ) |
               ( (DWORD) tm_info->tm_sec >> 1 );
  } else {
    fat_time = ( 0 << 25 ) | ( 1 << 21 ) | ( 1 << 16 );
  }

  return fat_time;
}

int rtems_fatfs_statvfs(
  const rtems_filesystem_location_info_t *root_loc,
  struct statvfs                         *sb
)
{
  rtems_fatfs_fs_info_t *fs_info = root_loc->mt_entry->fs_info;
  FATFS                 *fs      = &fs_info->fatfs;
  FRESULT                fr;
  DWORD                  free_clusters;
  FATFS                 *fatfs_ptr;
  char                   drive_path[ 8 ];

  if ( fs_info == NULL || sb == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  rtems_fatfs_lock( root_loc->mt_entry );

  drive_path[ 0 ] = '0' + fs_info->drive_number;
  drive_path[ 1 ] = ':';
  drive_path[ 2 ] = '\0';

  fr = f_getfree( drive_path, &free_clusters, &fatfs_ptr );
  if ( fr != FR_OK ) {
    rtems_fatfs_unlock( root_loc->mt_entry );
    errno = rtems_fatfs_fresult_to_errno( fr );
    return -1;
  }

#if FF_MAX_SS != FF_MIN_SS
  WORD sector_size = fs->ssize;
#else
  WORD sector_size = FF_MAX_SS;
#endif

  sb->f_bsize  = sector_size;
  sb->f_frsize = fs->csize * sector_size;
  sb->f_blocks = fs->n_fatent - 2;
  sb->f_bfree  = free_clusters;
  sb->f_bavail = free_clusters;
  sb->f_files  = 0;
  sb->f_ffree  = 0;
  sb->f_favail = 0;
  sb->f_flag   = 0;

#if FF_USE_LFN
  sb->f_namemax = FF_LFN_BUF - 1;
#else
  sb->f_namemax = 12;
#endif

  rtems_fatfs_unlock( root_loc->mt_entry );

  return 0;
}

const rtems_filesystem_operations_table rtems_fatfs_ops = {
  .lock_h            = rtems_fatfs_lock,
  .unlock_h          = rtems_fatfs_unlock,
  .eval_path_h       = rtems_fatfs_eval_path,
  .link_h            = rtems_filesystem_default_link,
  .are_nodes_equal_h = rtems_fatfs_are_nodes_equal,
  .mknod_h           = rtems_fatfs_mknod,
  .rmnod_h           = rtems_fatfs_rmnod,
  .fchmod_h          = rtems_filesystem_default_fchmod,
  .chown_h           = rtems_filesystem_default_chown,
  .clonenod_h        = rtems_fatfs_clone_node,
  .freenod_h         = rtems_fatfs_free_node,
  .mount_h           = rtems_fatfs_initialize,
  .unmount_h         = rtems_filesystem_default_unmount,
  .fsunmount_me_h    = rtems_fatfs_fsunmount_me,
  .utimens_h         = rtems_fatfs_utimens,
  .symlink_h         = rtems_filesystem_default_symlink,
  .readlink_h        = rtems_filesystem_default_readlink,
  .rename_h          = rtems_fatfs_rename,
  .statvfs_h         = rtems_fatfs_statvfs
};

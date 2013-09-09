/**
 * @file msdos_statvfs.c
 *
 * @brief Obtain MS-DOS filesystem information
 * @ingroup libfs_msdos MSDOS FileSystem
 */

/*
 *  Copyright (c) 2013 Andrey Mozzhuhin
 *  Copyright (c) 2013 Vitaly Belov
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include "fat.h"
#include "fat_fat_operations.h"
#include "msdos.h"

int msdos_statvfs(const rtems_filesystem_location_info_t *root_loc,
    struct statvfs *sb)
{
  msdos_fs_info_t *fs_info = root_loc->mt_entry->fs_info;
  fat_vol_t *vol = &fs_info->fat.vol;
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                              MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL)
      rtems_set_errno_and_return_minus_one(EIO);

  sb->f_bsize = FAT_SECTOR512_SIZE;
  sb->f_frsize = vol->bpc;
  sb->f_blocks = vol->data_cls;
  sb->f_bfree = 0;
  sb->f_bavail = 0;
  sb->f_files = 0;    // FAT doesn't store inodes
  sb->f_ffree = 0;
  sb->f_favail = 0;
  sb->f_flag = 0;
  sb->f_namemax = MSDOS_NAME_MAX_LNF_LEN;

  if (vol->free_cls == FAT_UNDEFINED_VALUE)
  {
    int rc;
    uint32_t cur_cl = 2;
    uint32_t value = 0;
    uint32_t data_cls_val = vol->data_cls + 2;

    for (; cur_cl < data_cls_val; ++cur_cl)
    {
      rc = fat_get_fat_cluster(&fs_info->fat, cur_cl, &value);
      if (rc != RC_OK)
      {
        rtems_semaphore_release(fs_info->vol_sema);
        return rc;
      }

      if (value == FAT_GENFAT_FREE)
      {
        sb->f_bfree++;
        sb->f_bavail++;
      }
    }
  }
  else
  {
    sb->f_bfree = vol->free_cls;
    sb->f_bavail = vol->free_cls;
  }

  rtems_semaphore_release(fs_info->vol_sema);
  return RC_OK;
}

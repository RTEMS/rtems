/**
 * @file
 *
 * @ingroup libfs_msdos MSDOS FileSystem
 *
 * @brief Obtain MS-DOS filesystem information
 */

/*
 *  Copyright (c) 2013 Andrey Mozzhuhin
 *  Copyright (c) 2013 Vitaly Belov
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include "fat.h"
#include "fat_fat_operations.h"
#include "msdos.h"

int msdos_statvfs(
  const rtems_filesystem_location_info_t *__restrict root_loc,
  struct statvfs *__restrict sb)
{
  msdos_fs_info_t *fs_info = root_loc->mt_entry->fs_info;
  fat_vol_t *vol = &fs_info->fat.vol;

  msdos_fs_lock(fs_info);

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
        msdos_fs_unlock(fs_info);
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

  msdos_fs_unlock(fs_info);
  return RC_OK;
}

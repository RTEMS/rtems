/**
 * @file
 *
 * @brief Low-level Operations on a Volume with a DOSFS FAT filesystem
 * @ingroup libfs
 */

/*
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>

#include <rtems/libio_.h>

#include "fat.h"
#include "fat_fat_operations.h"

static int
 _fat_block_release(fat_fs_info_t *fs_info);

static inline uint32_t
fat_cluster_num_to_block_num (const fat_fs_info_t *fs_info,
                              uint32_t             cln)
{
    uint32_t blk;

    if ( (cln == 0) && (fs_info->vol.type & (FAT_FAT12 | FAT_FAT16)) )
        blk = fat_sector_num_to_block_num(fs_info, fs_info->vol.rdir_loc);
    else
    {
        cln -= FAT_RSRVD_CLN;
        blk = cln << (fs_info->vol.bpc_log2 - fs_info->vol.bytes_per_block_log2);
        blk += fat_sector_num_to_block_num(fs_info, fs_info->vol.data_fsec);
    }

    return blk;
}

int
fat_buf_access(fat_fs_info_t   *fs_info,
               const uint32_t   sec_num,
               const int        op_type,
               uint8_t        **sec_buf)
{
    rtems_status_code sc = RTEMS_SUCCESSFUL;
    uint32_t          blk = fat_sector_num_to_block_num (fs_info,
                                                         sec_num);
    uint32_t          blk_ofs = fat_sector_offset_to_block_offset (fs_info,
                                                                   sec_num,
                                                                   0);

    if (fs_info->c.state == FAT_CACHE_EMPTY || fs_info->c.blk_num != sec_num)
    {
        fat_buf_release(fs_info);

        if (op_type == FAT_OP_TYPE_READ)
            sc = rtems_bdbuf_read(fs_info->vol.dd, blk, &fs_info->c.buf);
        else
            sc = rtems_bdbuf_get(fs_info->vol.dd, blk, &fs_info->c.buf);
        if (sc != RTEMS_SUCCESSFUL)
            rtems_set_errno_and_return_minus_one(EIO);
        fs_info->c.blk_num = sec_num;
        fs_info->c.modified = 0;
        fs_info->c.state = FAT_CACHE_ACTUAL;
    }
    *sec_buf = &fs_info->c.buf->buffer[blk_ofs];
    return RC_OK;
}

int
fat_buf_release(fat_fs_info_t *fs_info)
{
    rtems_status_code sc = RTEMS_SUCCESSFUL;

    if (fs_info->c.state == FAT_CACHE_EMPTY)
        return RC_OK;

    if (fs_info->c.modified)
    {
        uint32_t sec_num = fs_info->c.blk_num;
        bool     sec_of_fat = ((sec_num >= fs_info->vol.fat_loc) &&
                              (sec_num < fs_info->vol.rdir_loc));
        uint32_t blk = fat_sector_num_to_block_num(fs_info, sec_num);
        uint32_t blk_ofs = fat_sector_offset_to_block_offset(fs_info,
                                                             sec_num,
                                                             0);

        if (sec_of_fat && !fs_info->vol.mirror)
            memcpy(fs_info->sec_buf,
                   fs_info->c.buf->buffer + blk_ofs,
                   fs_info->vol.bps);

        sc = rtems_bdbuf_release_modified(fs_info->c.buf);
        if (sc != RTEMS_SUCCESSFUL)
            rtems_set_errno_and_return_minus_one(EIO);
        fs_info->c.modified = 0;

        if (sec_of_fat && !fs_info->vol.mirror)
        {
            uint8_t i;

            for (i = 1; i < fs_info->vol.fats; i++)
            {
                rtems_bdbuf_buffer *bd;

                sec_num = fs_info->c.blk_num + fs_info->vol.fat_length * i,
                blk = fat_sector_num_to_block_num(fs_info, sec_num);
                blk_ofs = fat_sector_offset_to_block_offset(fs_info,
                                                            sec_num,
                                                            0);

                if (blk_ofs == 0
                    && fs_info->vol.bps == fs_info->vol.bytes_per_block)
                {
                    sc = rtems_bdbuf_get(fs_info->vol.dd, blk, &bd);
                }
                else
                {
                    sc = rtems_bdbuf_read(fs_info->vol.dd, blk, &bd);
                }
                if ( sc != RTEMS_SUCCESSFUL)
                    rtems_set_errno_and_return_minus_one(ENOMEM);
                memcpy(bd->buffer + blk_ofs, fs_info->sec_buf, fs_info->vol.bps);
                sc = rtems_bdbuf_release_modified(bd);
                if ( sc != RTEMS_SUCCESSFUL)
                    rtems_set_errno_and_return_minus_one(ENOMEM);
            }
        }
    }
    else
    {
        sc = rtems_bdbuf_release(fs_info->c.buf);
        if (sc != RTEMS_SUCCESSFUL)
            rtems_set_errno_and_return_minus_one(EIO);
    }
    fs_info->c.state = FAT_CACHE_EMPTY;
    return RC_OK;
}

/* _fat_block_read --
 *     This function reads 'count' bytes from device filesystem is mounted on,
 *     starts at 'start+offset' position where 'start' computed in sectors
 *     and 'offset' is offset inside sector (reading may cross sectors
 *     boundary; in this case assumed we want to read sequential sector(s))
 *
 * PARAMETERS:
 *     fs_info  - FS info
 *     start    - sector num to start read from
 *     offset   - offset inside sector 'start'
 *     count    - count of bytes to read
 *     buff     - buffer provided by user
 *
 * RETURNS:
 *     bytes read on success, or -1 if error occured
 *     and errno set appropriately
 */
ssize_t
_fat_block_read(
    fat_fs_info_t                        *fs_info,
    uint32_t                              start,
    uint32_t                              offset,
    uint32_t                              count,
    void                                 *buff
    )
{
    int                     rc = RC_OK;
    ssize_t                 cmpltd = 0;
    uint32_t                sec_num = start;
    uint32_t                ofs = offset;
    uint8_t                *sec_buf;
    uint32_t                c = 0;

    while (count > 0)
    {
        rc = fat_buf_access(fs_info, sec_num, FAT_OP_TYPE_READ, &sec_buf);
        if (rc != RC_OK)
            return -1;

        c = MIN(count, (fs_info->vol.bps - ofs));
        memcpy((buff + cmpltd), (sec_buf + ofs), c);

        count -= c;
        cmpltd += c;
        sec_num++;
        ofs = 0;
    }
    return cmpltd;
}

static ssize_t
 fat_block_write(
    fat_fs_info_t                        *fs_info,
    const uint32_t                        start_blk,
    const uint32_t                        offset,
    const uint32_t                        count,
    const void                           *buf,
    const bool                            overwrite_block)
{
    int                 rc             = RC_OK;
    uint32_t            bytes_to_write = MIN(count, (fs_info->vol.bytes_per_block - offset));
    uint8_t            *blk_buf;
    uint32_t            sec_num        = fat_block_num_to_sector_num(fs_info, start_blk);

    if (0 < bytes_to_write)
    {
        if (   overwrite_block
            || (bytes_to_write == fs_info->vol.bytes_per_block))
        {
            rc = fat_buf_access(fs_info, sec_num, FAT_OP_TYPE_GET, &blk_buf);
        }
        else
            rc = fat_buf_access(fs_info, sec_num, FAT_OP_TYPE_READ, &blk_buf);

        if (RC_OK == rc)
        {
            memcpy(blk_buf + offset, buf, bytes_to_write);

            fat_buf_mark_modified(fs_info);
        }
    }
    if (RC_OK != rc)
        return rc;
    else
        return bytes_to_write;
}

/* fat_sector_write --
 *     This function write 'count' bytes to device filesystem is mounted on,
 *     starts at 'start+offset' position where 'start' computed in sectors
 *     and 'offset' is offset inside sector (writing may cross sectors
 *     boundary; in this case assumed we want to write sequential sector(s))
 *
 * PARAMETERS:
 *     fs_info  - FS info
 *     start    - sector num to start read from
 *     offset   - offset inside sector 'start'
 *     count    - count of bytes to write
 *     buff     - buffer provided by user
 *
 * RETURNS:
 *     bytes written on success, or -1 if error occured
 *     and errno set appropriately
 */
ssize_t
fat_sector_write(
    fat_fs_info_t                        *fs_info,
    uint32_t                              start,
    uint32_t                              offset,
    uint32_t                              count,
    const void                           *buff)
{
    int                 rc = RC_OK;
    ssize_t             cmpltd = 0;
    uint32_t            sec_num = start;
    uint32_t            ofs = offset;
    uint8_t            *sec_buf;
    uint32_t            c = 0;

    while(count > 0)
    {
        c = MIN(count, (fs_info->vol.bps - ofs));

        if (c == fs_info->vol.bytes_per_block)
            rc = fat_buf_access(fs_info, sec_num, FAT_OP_TYPE_GET, &sec_buf);
        else
            rc = fat_buf_access(fs_info, sec_num, FAT_OP_TYPE_READ, &sec_buf);
        if (rc != RC_OK)
            return -1;

        memcpy((sec_buf + ofs), (buff + cmpltd), c);

        fat_buf_mark_modified(fs_info);

        count -= c;
        cmpltd +=c;
        sec_num++;
        ofs = 0;
    }
    return cmpltd;
}

static ssize_t
 fat_block_set (
     fat_fs_info_t                        *fs_info,
     const uint32_t                        start_blk,
     const uint32_t                        offset,
     const uint32_t                        count,
     const uint8_t                         pattern)
{
    int                 rc             = RC_OK;
    uint32_t            bytes_to_write = MIN(count, (fs_info->vol.bytes_per_block - offset));
    uint8_t            *blk_buf;
    uint32_t            sec_num        = fat_block_num_to_sector_num(fs_info, start_blk);

    if (0 < bytes_to_write)
    {
        if (bytes_to_write == fs_info->vol.bytes_per_block)
        {
            rc = fat_buf_access(fs_info, sec_num, FAT_OP_TYPE_GET, &blk_buf);
        }
        else
            rc = fat_buf_access(fs_info, sec_num, FAT_OP_TYPE_READ, &blk_buf);

        if (RC_OK == rc)
        {
            memset(blk_buf + offset, pattern, bytes_to_write);

            fat_buf_mark_modified(fs_info);
        }
    }
    if (RC_OK != rc)
        return rc;
    else
        return bytes_to_write;
}

ssize_t
fat_cluster_set(
     fat_fs_info_t                        *fs_info,
     const uint32_t                        start_cln,
     const uint32_t                        offset,
     const uint32_t                        count,
     const uint8_t                         pattern)
{
  ssize_t             rc               = RC_OK;
  uint32_t            bytes_to_write   = MIN(count, (fs_info->vol.bpc - offset));
  uint32_t            cur_blk          = fat_cluster_num_to_block_num(fs_info, start_cln);
  uint32_t            blocks_in_offset = offset >> fs_info->vol.bytes_per_block_log2;
  uint32_t            ofs_blk          = offset - (blocks_in_offset << fs_info->vol.bytes_per_block_log2);
  ssize_t             bytes_written    = 0;
  ssize_t             ret;

  cur_blk += blocks_in_offset;

  while (   (RC_OK == rc)
         && (0 < bytes_to_write))
  {
    uint32_t c = MIN(bytes_to_write, (fs_info->vol.bytes_per_block - ofs_blk));

    ret = fat_block_set(
        fs_info,
        cur_blk,
        ofs_blk,
        c,
        pattern);
    if (c != ret)
      rc = -1;
    else
    {
        bytes_to_write -= ret;
        bytes_written  += ret;
        ++cur_blk;
    }
    ofs_blk = 0;
  }
  if (RC_OK != rc)
    return rc;
  else
    return bytes_written;
}

/* _fat_block_release --
 *     This function works around the hack that hold a bdbuf and does
 *     not release it.
 *
 * PARAMETERS:
 *     fs_info  - FS info
 *
 * RETURNS:
 *     0 on success, or -1 if error occured and errno set appropriately
 */
int
_fat_block_release(fat_fs_info_t *fs_info)
{
    return fat_buf_release(fs_info);
}

/* fat_cluster_write --
 *     This function write 'count' bytes to device filesystem is mounted on,
 *     starts at 'start+offset' position where 'start' computed in clusters
 *     and 'offset' is offset inside cluster.
 *     Writing will NOT cross cluster boundaries!
 *
 * PARAMETERS:
 *     fs_info            - FS info
 *     start_cln          - cluster number to start writing to
 *     offset             - offset inside cluster 'start'
 *     count              - count of bytes to write
 *     buff               - buffer provided by user
 *     overwrite_cluster  - true if cluster can get overwritten, false if cluster content must be kept
 *
 * RETURNS:
 *     bytes written on success, or -1 if error occured
 *     and errno set appropriately
 */
ssize_t
fat_cluster_write(
    fat_fs_info_t                        *fs_info,
    const uint32_t                        start_cln,
    const uint32_t                        offset,
    const uint32_t                        count,
    const void                           *buff,
    const bool                            overwrite_cluster)
{
    ssize_t             rc               = RC_OK;
    uint32_t            bytes_to_write   = MIN(count, (fs_info->vol.bpc - offset));
    uint32_t            cur_blk          = fat_cluster_num_to_block_num(fs_info, start_cln);
    uint32_t            blocks_in_offset = (offset >> fs_info->vol.bytes_per_block_log2);
    uint32_t            ofs_blk          = offset - (blocks_in_offset << fs_info->vol.bytes_per_block_log2);
    ssize_t             bytes_written    = 0;
    uint8_t             *buffer          = (uint8_t*)buff;
    ssize_t             ret;
    uint32_t            c;

    cur_blk += blocks_in_offset;

    while (   (RC_OK == rc)
           && (0 < bytes_to_write))
    {
      c = MIN(bytes_to_write, (fs_info->vol.bytes_per_block - ofs_blk));

      ret = fat_block_write(
          fs_info,
          cur_blk,
          ofs_blk,
          c,
          &buffer[bytes_written],
          overwrite_cluster);
      if (c != ret)
        rc = -1;
      else
      {
          bytes_to_write -= ret;
          bytes_written  += ret;
          ++cur_blk;
      }
      ofs_blk = 0;
    }
    if (RC_OK != rc)
      return rc;
    else
      return bytes_written;
}

static bool is_cluster_aligned(const fat_vol_t *vol, uint32_t sec_num)
{
    return (sec_num & (vol->spc - 1)) == 0;
}

/* fat_init_volume_info --
 *     Get inforamtion about volume on which filesystem is mounted on
 *
 * PARAMETERS:
 *     fs_info  - FS info
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured
 *     and errno set appropriately
 */
int
fat_init_volume_info(fat_fs_info_t *fs_info, const char *device)
{
    rtems_status_code   sc = RTEMS_SUCCESSFUL;
    int                 rc = RC_OK;
    register fat_vol_t *vol = &fs_info->vol;
    uint32_t            data_secs = 0;
    char                boot_rec[FAT_MAX_BPB_SIZE];
    char                fs_info_sector[FAT_USEFUL_INFO_SIZE];
    ssize_t             ret = 0;
    struct stat         stat_buf;
    int                 i = 0;
    rtems_bdbuf_buffer *block = NULL;

    vol->fd = open(device, O_RDWR);
    if (vol->fd < 0)
    {
        rtems_set_errno_and_return_minus_one(ENXIO);
    }

    rc = fstat(vol->fd, &stat_buf);
    if (rc != 0)
    {
        close(vol->fd);
        rtems_set_errno_and_return_minus_one(ENXIO);
    }

    /* Must be a block device. */
    if (!S_ISBLK(stat_buf.st_mode))
    {
        close(vol->fd);
        rtems_set_errno_and_return_minus_one(ENXIO);
    }

    /* check that device is registred as block device and lock it */
    rc = rtems_disk_fd_get_disk_device(vol->fd, &vol->dd);
    if (rc != 0) {
        close(vol->fd);
        rtems_set_errno_and_return_minus_one(ENXIO);
    }

    /* Read boot record */
    /* FIXME: Asserts FAT_MAX_BPB_SIZE < bdbuf block size */
    sc = rtems_bdbuf_read( vol->dd, 0, &block);
    if (sc != RTEMS_SUCCESSFUL)
    {
        close(vol->fd);
        rtems_set_errno_and_return_minus_one( EIO);
    }

    memcpy( boot_rec, block->buffer, FAT_MAX_BPB_SIZE);

    sc = rtems_bdbuf_release( block);
    if (sc != RTEMS_SUCCESSFUL)
    {
        close(vol->fd);
        rtems_set_errno_and_return_minus_one( EIO );
    }

    /* Evaluate boot record */
    vol->bps = FAT_GET_BR_BYTES_PER_SECTOR(boot_rec);

    if ( (vol->bps != 512)  &&
         (vol->bps != 1024) &&
         (vol->bps != 2048) &&
         (vol->bps != 4096))
    {
        close(vol->fd);
        rtems_set_errno_and_return_minus_one( EINVAL );
    }
    for (vol->sec_mul = 0, i = (vol->bps >> FAT_SECTOR512_BITS); (i & 1) == 0;
         i >>= 1, vol->sec_mul++);
    for (vol->sec_log2 = 0, i = vol->bps; (i & 1) == 0;
         i >>= 1, vol->sec_log2++);

    vol->bytes_per_block = vol->bps;
    vol->bytes_per_block_log2 = vol->sec_log2;
    vol->sectors_per_block = 1;

    vol->spc = FAT_GET_BR_SECTORS_PER_CLUSTER(boot_rec);
    /*
     * "sectors per cluster" of zero is invalid
     * (and would hang the following loop)
     */
    if (vol->spc == 0)
    {
        close(vol->fd);
        rtems_set_errno_and_return_minus_one(EINVAL);
    }

    for (vol->spc_log2 = 0, i = vol->spc; (i & 1) == 0;
         i >>= 1, vol->spc_log2++);

    /*
     * "bytes per cluster" value greater than 32K is invalid
     */
    if ((vol->bpc = vol->bps << vol->spc_log2) > MS_BYTES_PER_CLUSTER_LIMIT)
    {
        close(vol->fd);
        rtems_set_errno_and_return_minus_one(EINVAL);
    }

    for (vol->bpc_log2 = 0, i = vol->bpc; (i & 1) == 0;
         i >>= 1, vol->bpc_log2++);

    vol->fats = FAT_GET_BR_FAT_NUM(boot_rec);
    vol->fat_loc = FAT_GET_BR_RESERVED_SECTORS_NUM(boot_rec);

    vol->rdir_entrs = FAT_GET_BR_FILES_PER_ROOT_DIR(boot_rec);

    /* calculate the count of sectors occupied by the root directory */
    vol->rdir_secs = ((vol->rdir_entrs * FAT_DIRENTRY_SIZE) + (vol->bps - 1)) /
                     vol->bps;

    vol->rdir_size = vol->rdir_secs << vol->sec_log2;

    if ( (FAT_GET_BR_SECTORS_PER_FAT(boot_rec)) != 0)
        vol->fat_length = FAT_GET_BR_SECTORS_PER_FAT(boot_rec);
    else
        vol->fat_length = FAT_GET_BR_SECTORS_PER_FAT32(boot_rec);

    vol->data_fsec = vol->fat_loc + vol->fats * vol->fat_length +
                     vol->rdir_secs;

    /* for  FAT12/16 root dir starts at(sector) */
    vol->rdir_loc = vol->fat_loc + vol->fats * vol->fat_length;

    if ( (FAT_GET_BR_TOTAL_SECTORS_NUM16(boot_rec)) != 0)
        vol->tot_secs = FAT_GET_BR_TOTAL_SECTORS_NUM16(boot_rec);
    else
        vol->tot_secs = FAT_GET_BR_TOTAL_SECTORS_NUM32(boot_rec);

    data_secs = vol->tot_secs - vol->data_fsec;

    vol->data_cls = data_secs / vol->spc;

    /* determine FAT type at least */
    if ( vol->data_cls < FAT_FAT12_MAX_CLN)
    {
        vol->type = FAT_FAT12;
        vol->mask = FAT_FAT12_MASK;
        vol->eoc_val = FAT_FAT12_EOC;
    }
    else
    {
        if ( vol->data_cls < FAT_FAT16_MAX_CLN)
        {
            vol->type = FAT_FAT16;
            vol->mask = FAT_FAT16_MASK;
            vol->eoc_val = FAT_FAT16_EOC;
        }
        else if ( vol->data_cls < FAT_FAT32_MASK - 1 )
        {
            vol->type = FAT_FAT32;
            vol->mask = FAT_FAT32_MASK;
            vol->eoc_val = FAT_FAT32_EOC;
        }
        else
        {
            close(vol->fd);
            rtems_set_errno_and_return_minus_one( EINVAL );
        }
    }

    if (vol->type == FAT_FAT32)
    {
        vol->rdir_cl = FAT_GET_BR_FAT32_ROOT_CLUSTER(boot_rec);

        vol->mirror = FAT_GET_BR_EXT_FLAGS(boot_rec) & FAT_BR_EXT_FLAGS_MIRROR;
        if (vol->mirror)
            vol->afat = FAT_GET_BR_EXT_FLAGS(boot_rec) & FAT_BR_EXT_FLAGS_FAT_NUM;
        else
            vol->afat = 0;

        vol->info_sec = FAT_GET_BR_FAT32_FS_INFO_SECTOR(boot_rec);
        if( vol->info_sec == 0 )
        {
            close(vol->fd);
            rtems_set_errno_and_return_minus_one( EINVAL );
        }
        else
        {
            ret = _fat_block_read(fs_info, vol->info_sec , 0,
                                  FAT_FSI_LEADSIG_SIZE, fs_info_sector);
            if ( ret < 0 )
            {
                close(vol->fd);
                return -1;
            }

            if (FAT_GET_FSINFO_LEAD_SIGNATURE(fs_info_sector) !=
                FAT_FSINFO_LEAD_SIGNATURE_VALUE)
            {
                _fat_block_release(fs_info);
                close(vol->fd);
                rtems_set_errno_and_return_minus_one( EINVAL );
            }
            else
            {
                ret = _fat_block_read(fs_info, vol->info_sec , FAT_FSI_INFO,
                                      FAT_USEFUL_INFO_SIZE, fs_info_sector);
                if ( ret < 0 )
                {
                    _fat_block_release(fs_info);
                    close(vol->fd);
                    return -1;
                }

                vol->free_cls_in_fs_info =
                  FAT_GET_FSINFO_FREE_CLUSTER_COUNT(fs_info_sector);
                vol->free_cls = vol->free_cls_in_fs_info;
                vol->next_cl_in_fs_info =
                  FAT_GET_FSINFO_NEXT_FREE_CLUSTER(fs_info_sector);
                vol->next_cl = vol->next_cl_in_fs_info;
            }
        }
    }
    else
    {
        vol->rdir_cl = 0;
        vol->mirror = 0;
        vol->afat = 0;
        vol->free_cls = FAT_UNDEFINED_VALUE;
        vol->next_cl = FAT_UNDEFINED_VALUE;
    }

    _fat_block_release(fs_info);

    vol->afat_loc = vol->fat_loc + vol->fat_length * vol->afat;

    /* set up collection of fat-files fd */
    fs_info->vhash = calloc(FAT_HASH_SIZE, sizeof(rtems_chain_control));
    if ( fs_info->vhash == NULL )
    {
        close(vol->fd);
        rtems_set_errno_and_return_minus_one( ENOMEM );
    }

    for (i = 0; i < FAT_HASH_SIZE; i++)
        rtems_chain_initialize_empty(fs_info->vhash + i);

    fs_info->rhash = calloc(FAT_HASH_SIZE, sizeof(rtems_chain_control));
    if ( fs_info->rhash == NULL )
    {
        close(vol->fd);
        free(fs_info->vhash);
        rtems_set_errno_and_return_minus_one( ENOMEM );
    }
    for (i = 0; i < FAT_HASH_SIZE; i++)
        rtems_chain_initialize_empty(fs_info->rhash + i);

    fs_info->uino_pool_size = FAT_UINO_POOL_INIT_SIZE;
    fs_info->uino_base = (vol->tot_secs << vol->sec_mul) << 4;
    fs_info->index = 0;
    fs_info->uino = (char *)calloc(fs_info->uino_pool_size, sizeof(char));
    if ( fs_info->uino == NULL )
    {
        close(vol->fd);
        free(fs_info->vhash);
        free(fs_info->rhash);
        rtems_set_errno_and_return_minus_one( ENOMEM );
    }
    fs_info->sec_buf = (uint8_t *)calloc(vol->bps, sizeof(uint8_t));
    if (fs_info->sec_buf == NULL)
    {
        close(vol->fd);
        free(fs_info->vhash);
        free(fs_info->rhash);
        free(fs_info->uino);
        rtems_set_errno_and_return_minus_one( ENOMEM );
    }

    /*
     * If possible we will use the cluster size as bdbuf block size for faster
     * file access. This requires that certain sectors are aligned to cluster
     * borders.
     */
    if (is_cluster_aligned(vol, vol->data_fsec)
        && (FAT_FAT32 == vol->type || is_cluster_aligned(vol, vol->rdir_loc)))
    {
        sc = rtems_bdbuf_set_block_size (vol->dd, vol->bpc, true);
        if (sc == RTEMS_SUCCESSFUL)
        {
            vol->bytes_per_block = vol->bpc;
            vol->bytes_per_block_log2 = vol->bpc_log2;
            vol->sectors_per_block = vol->spc;
        }
    }

    return RC_OK;
}

/* fat_fat32_update_fsinfo_sector --
 *     Synchronize fsinfo sector for FAT32 volumes
 *
 * PARAMETERS:
 *     fs_info    - FS info
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured (errno set appropriately)
 */
static int
fat_fat32_update_fsinfo_sector(fat_fs_info_t *fs_info)
{
    ssize_t ret1 = 0, ret2 = 0;

    if (fs_info->vol.type == FAT_FAT32)
    {
        uint32_t free_count = fs_info->vol.free_cls;
        uint32_t next_free = fs_info->vol.next_cl;

        if (free_count != fs_info->vol.free_cls_in_fs_info)
        {
            uint32_t le_free_count = CT_LE_L(free_count);

            fs_info->vol.free_cls_in_fs_info = free_count;

            ret1 = fat_sector_write(fs_info,
                                    fs_info->vol.info_sec,
                                    FAT_FSINFO_FREE_CLUSTER_COUNT_OFFSET,
                                    sizeof(le_free_count),
                                    &le_free_count);
        }

        if (next_free != fs_info->vol.next_cl_in_fs_info)
        {
            uint32_t le_next_free = CT_LE_L(next_free);

            fs_info->vol.next_cl_in_fs_info = next_free;

            ret2 = fat_sector_write(fs_info,
                                    fs_info->vol.info_sec,
                                    FAT_FSINFO_NEXT_FREE_CLUSTER_OFFSET,
                                    sizeof(le_next_free),
                                    &le_next_free);
        }
    }

    if ( (ret1 < 0) || (ret2 < 0) )
        return -1;

    return RC_OK;
}

int
fat_sync(fat_fs_info_t *fs_info)
{
    int rc = RC_OK;

    rc = fat_fat32_update_fsinfo_sector(fs_info);
    if ( rc != RC_OK )
        rc = -1;

    fat_buf_release(fs_info);

    if (rtems_bdbuf_syncdev(fs_info->vol.dd) != RTEMS_SUCCESSFUL)
        rc = -1;

    return rc;
}

/* fat_shutdown_drive --
 *     Free all allocated resources and synchronize all necessary data
 *
 * PARAMETERS:
 *     fs_info  - FS info
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured
 *     and errno set appropriately
 */
int
fat_shutdown_drive(fat_fs_info_t *fs_info)
{
    int            rc = RC_OK;
    int            i = 0;

    rc = fat_sync(fs_info);
    if ( rc != RC_OK )
        rc = -1;

    for (i = 0; i < FAT_HASH_SIZE; i++)
    {
        rtems_chain_node    *node = NULL;
        rtems_chain_control *the_chain = fs_info->vhash + i;

        while ( (node = rtems_chain_get_unprotected(the_chain)) != NULL )
            free(node);
    }

    for (i = 0; i < FAT_HASH_SIZE; i++)
    {
        rtems_chain_node    *node = NULL;
        rtems_chain_control *the_chain = fs_info->rhash + i;

        while ( (node = rtems_chain_get_unprotected(the_chain)) != NULL )
            free(node);
    }

    free(fs_info->vhash);
    free(fs_info->rhash);

    free(fs_info->uino);
    free(fs_info->sec_buf);
    close(fs_info->vol.fd);

    if (rc)
        errno = EIO;
    return rc;
}

/* fat_init_clusters_chain --
 *     Zeroing contents of all clusters in the chain
 *
 * PARAMETERS:
 *     fs_info           - FS info
 *     start_cluster_num - num of first cluster in the chain
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured
 *     and errno set appropriately
 */
int
fat_init_clusters_chain(
    fat_fs_info_t                        *fs_info,
    uint32_t                              start_cln
    )
{
    int                     rc = RC_OK;
    ssize_t                 ret = 0;
    uint32_t                cur_cln = start_cln;

    while ((cur_cln & fs_info->vol.mask) < fs_info->vol.eoc_val)
    {
        ret = fat_cluster_set(fs_info, cur_cln, 0, fs_info->vol.bpc, 0);
        if ( ret != fs_info->vol.bpc )
        {
            return -1;
        }

        rc  = fat_get_fat_cluster(fs_info, cur_cln, &cur_cln);
        if ( rc != RC_OK )
        {
            return rc;
        }

    }

    return rc;
}

#define FAT_UNIQ_INO_BASE 0x0FFFFF00

#define FAT_UNIQ_INO_IS_BUSY(index, arr) \
  (((arr)[((index)>>3)]>>((index) & (8-1))) & 0x01)

#define FAT_SET_UNIQ_INO_BUSY(index, arr) \
  ((arr)[((index)>>3)] |= (0x01<<((index) & (8-1))))

#define FAT_SET_UNIQ_INO_FREE(index, arr) \
  ((arr)[((index)>>3)] &= (~(0x01<<((index) & (8-1)))))

/* fat_get_unique_ino --
 *     Allocate unique ino from unique ino pool
 *
 * PARAMETERS:
 *     fs_info  - FS info
 *
 * RETURNS:
 *     unique inode number on success, or 0 if there is no free unique inode
 *     number in the pool
 *
 * ATTENTION:
 *     0 means FAILED !!!
 *
 */
uint32_t
fat_get_unique_ino(fat_fs_info_t *fs_info)
{
    uint32_t                j = 0;
    bool                    resrc_unsuff = false;

    while (!resrc_unsuff)
    {
        for (j = 0; j < fs_info->uino_pool_size; j++)
        {
            if (!FAT_UNIQ_INO_IS_BUSY(fs_info->index, fs_info->uino))
            {
                FAT_SET_UNIQ_INO_BUSY(fs_info->index, fs_info->uino);
                return (fs_info->uino_base + fs_info->index);
            }
            fs_info->index++;
            if (fs_info->index >= fs_info->uino_pool_size)
                fs_info->index = 0;
        }

        if ((fs_info->uino_pool_size << 1) < (0x0FFFFFFF - fs_info->uino_base))
        {
            fs_info->uino_pool_size <<= 1;
            fs_info->uino = realloc(fs_info->uino, fs_info->uino_pool_size);
            if (fs_info->uino != NULL)
                fs_info->index = fs_info->uino_pool_size;
            else
                resrc_unsuff = true;
        }
        else
            resrc_unsuff = true;
    }
    return 0;
}

/* fat_free_unique_ino --
 *     Return unique ino to unique ino pool
 *
 * PARAMETERS:
 *     fs_info  - FS info
 *     ino      - inode number to free
 *
 * RETURNS:
 *     None
 */
void
fat_free_unique_ino(
    fat_fs_info_t                        *fs_info,
    uint32_t                              ino
    )
{
    FAT_SET_UNIQ_INO_FREE((ino - fs_info->uino_base), fs_info->uino);
}

/* fat_ino_is_unique --
 *     Test whether ino is from unique ino pool
 *
 * PARAMETERS:
 *     fs_info  - FS info
 *     ino   - ino to be tested
 *
 * RETURNS:
 *     true if ino is allocated from unique ino pool, false otherwise
 */
inline bool
fat_ino_is_unique(
    fat_fs_info_t                        *fs_info,
    uint32_t                              ino
    )
{

    return (ino >= fs_info->uino_base);
}

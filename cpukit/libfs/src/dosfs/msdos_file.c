/*
 *  MSDOS file handlers implementation
 *
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  @(#) $Id$
 */
#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/libio_.h>

#include "fat.h"
#include "fat_fat_operations.h"
#include "fat_file.h"

#include "msdos.h"

/* msdos_file_open --
 *     Open fat-file which correspondes to the file
 *
 * PARAMETERS:
 *     iop        - file control block
 *     pathname   - name
 *     flag       - flags
 *     mode       - mode
 *
 * RETURNS:
 *     RC_OK, if file opened successfully, or -1 if error occured
 *     and errno set appropriately
 */
int 
msdos_file_open(rtems_libio_t *iop, const char *pathname, unsigned32 flag,
                unsigned32 mode)
{
    int                rc = RC_OK; 
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info = iop->pathinfo.mt_entry->fs_info; 
    fat_file_fd_t     *fat_fd = iop->file_info;

    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        set_errno_and_return_minus_one(EIO);

    rc = fat_file_reopen(fat_fd);
    if (rc != RC_OK)
    {
        rtems_semaphore_release(fs_info->vol_sema);
        return rc; 
    }

    if (iop->flags & LIBIO_FLAGS_APPEND)
        iop->offset = fat_fd->fat_file_size;
 
    iop->size = fat_fd->fat_file_size;
  
    rtems_semaphore_release(fs_info->vol_sema);
    return RC_OK;
}

/* msdos_file_close --
 *     Close fat-file which correspondes to the file. If fat-file descriptor 
 *     which correspondes to the file is not marked "removed", synchronize 
 *     size, first cluster number, write time and date fields of the file.
 *
 * PARAMETERS:
 *     iop - file control block
 *
 * RETURNS:
 *     RC_OK, if file closed successfully, or -1 if error occured (errno set 
 *     appropriately)
 */
int 
msdos_file_close(rtems_libio_t *iop)
{
    int                rc = RC_OK; 
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info = iop->pathinfo.mt_entry->fs_info; 
    fat_file_fd_t     *fat_fd = iop->file_info;

    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT, 
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        set_errno_and_return_minus_one(EIO);
  
    /* 
     * if fat-file descriptor is not marked as "removed", synchronize 
     * size, first cluster number, write time and date fields of the file
     */
    if (!FAT_FILE_IS_REMOVED(fat_fd))
    { 
        rc = msdos_set_first_cluster_num(iop->pathinfo.mt_entry, fat_fd);
        if (rc != RC_OK)
        {
            rtems_semaphore_release(fs_info->vol_sema);
            return rc;
        }

        rc = msdos_set_file_size(iop->pathinfo.mt_entry, fat_fd);
        if (rc != RC_OK)
        { 
            rtems_semaphore_release(fs_info->vol_sema);
            return rc;
        } 

        rc = msdos_set_dir_wrt_time_and_date(iop->pathinfo.mt_entry, fat_fd);
        if (rc != RC_OK)
        {
            rtems_semaphore_release(fs_info->vol_sema);
            return rc;
        } 
    }
  
    rc = fat_file_close(iop->pathinfo.mt_entry, fat_fd);

    rtems_semaphore_release(fs_info->vol_sema);
    return rc;
}

/* msdos_file_read --
 *     This routine read from file pointed to by file control block into
 *     the specified data buffer provided by user
 *
 * PARAMETERS:
 *     iop    - file control block
 *     buffer - buffer  provided by user
 *     count  - the number of bytes to read
 *
 * RETURNS:
 *     the number of bytes read on success, or -1 if error occured (errno set 
 *     appropriately)
 */
ssize_t 
msdos_file_read(rtems_libio_t *iop, void *buffer, unsigned32 count)
{
    ssize_t            ret = 0;
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info = iop->pathinfo.mt_entry->fs_info; 
    fat_file_fd_t     *fat_fd = iop->file_info;

    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        set_errno_and_return_minus_one(EIO);

    ret = fat_file_read(iop->pathinfo.mt_entry, fat_fd, iop->offset, count, 
                        buffer);

    rtems_semaphore_release(fs_info->vol_sema);
    return ret;
}

/* msdos_file_write --
 *     This routine writes the specified data buffer into the file pointed to 
 *     by file control block.
 *
 * PARAMETERS:
 *     iop    - file control block
 *     buffer - data to write
 *     count  - count of bytes to write
 *
 * RETURNS:
 *     the number of bytes written on success, or -1 if error occured
 *     and errno set appropriately
 */
ssize_t 
msdos_file_write(rtems_libio_t *iop,const void *buffer, unsigned32 count)
{
    ssize_t            ret = 0; 
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info = iop->pathinfo.mt_entry->fs_info; 
    fat_file_fd_t     *fat_fd = iop->file_info;

    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        set_errno_and_return_minus_one(EIO);

    ret = fat_file_write(iop->pathinfo.mt_entry, fat_fd, iop->offset, count, 
                         buffer);
    if (ret < 0)
    {
        rtems_semaphore_release(fs_info->vol_sema); 
        return -1;
    }

    /* 
     * update file size in both fat-file descriptor and file control block if 
     * file was extended
     */
    if (iop->offset + ret > fat_fd->fat_file_size)
        fat_fd->fat_file_size = iop->offset + ret;

    iop->size = fat_fd->fat_file_size;

    rtems_semaphore_release(fs_info->vol_sema);
    return ret;
}

/* msdos_file_lseek --
 *     Process lseek call to the file: extend file if lseek is up to the end 
 *     of the file.
 *
 * PARAMETERS:
 *     iop    - file control block
 *     offset - new offset
 *     whence - predefine directive
 *
 * RETURNS:
 *     new offset on success, or -1 if error occured (errno set 
 *     appropriately).
 */
int 
msdos_file_lseek(rtems_libio_t *iop, off_t offset, int whence)
{
    int                rc = RC_OK;
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info = iop->pathinfo.mt_entry->fs_info; 
    fat_file_fd_t     *fat_fd = iop->file_info;
    unsigned32         real_size = 0;

    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        set_errno_and_return_minus_one(EIO);

    rc = fat_file_extend(iop->pathinfo.mt_entry, fat_fd, iop->offset, 
                         &real_size);
    if (rc != RC_OK)
    {
        rtems_semaphore_release(fs_info->vol_sema);
        return rc;
    }

    if (real_size > fat_fd->fat_file_size)
        fat_fd->fat_file_size = iop->offset = real_size;
     
    iop->size = fat_fd->fat_file_size;

    rtems_semaphore_release(fs_info->vol_sema);
    return iop->offset;
}

/* msdos_file_stat --
 *
 * PARAMETERS:
 *     loc - node description
 *     buf - stat buffer provided by user
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured (errno set appropriately)
 */
int 
msdos_file_stat(
    rtems_filesystem_location_info_t *loc,
    struct stat                      *buf
    )
{
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info = loc->mt_entry->fs_info; 
    fat_file_fd_t     *fat_fd = loc->node_access;

    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        set_errno_and_return_minus_one(EIO);
  
    buf->st_dev = fs_info->fat.vol.dev;
    buf->st_ino = fat_fd->ino;
    buf->st_mode  = S_IFREG;
    buf->st_rdev = 0ll;
    buf->st_size = fat_fd->fat_file_size;
    buf->st_blocks = fat_fd->fat_file_size >> FAT_SECTOR512_BITS;
    buf->st_blksize = fs_info->fat.vol.bps;
    buf->st_mtime = fat_fd->mtime;

    rtems_semaphore_release(fs_info->vol_sema);
    return RC_OK;
}

/* msdos_file_ftruncate --
 *     Truncate the file (if new length is greater then current do nothing).
 *
 * PARAMETERS:
 *     iop    - file control block
 *     length - new length 
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured (errno set appropriately).
 */
int 
msdos_file_ftruncate(rtems_libio_t *iop, off_t length)
{
    int                rc = RC_OK;
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info = iop->pathinfo.mt_entry->fs_info; 
    fat_file_fd_t     *fat_fd = iop->file_info;

    if (length >= fat_fd->fat_file_size)
        return RC_OK;

    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        set_errno_and_return_minus_one(EIO);
  
    rc = fat_file_truncate(iop->pathinfo.mt_entry, fat_fd, length);
    if (rc != RC_OK)
    {
        rtems_semaphore_release(fs_info->vol_sema);
        return rc;
    }

    /* 
     * fat_file_truncate do nothing if new length >= fat-file size, so update 
     * file size only if length < fat-file size
     */
    if (length < fat_fd->fat_file_size)
        iop->size = fat_fd->fat_file_size = length;
     
    rtems_semaphore_release(fs_info->vol_sema);
    return RC_OK;
}

/* msdos_file_sync --
 *     Synchronize file - synchronize file data and if file is not removed 
 *     synchronize file metadata.
 *
 * PARAMETERS:
 *     iop - file control block
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured (errno set appropriately)
 */
int
msdos_file_sync(rtems_libio_t *iop)
{
    int                rc = RC_OK;
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    fat_file_fd_t     *fat_fd = iop->file_info;
    msdos_fs_info_t   *fs_info = iop->pathinfo.mt_entry->fs_info;
  
    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        set_errno_and_return_minus_one(EIO);
  
    /* synchronize file data */
    rc = fat_file_datasync(iop->pathinfo.mt_entry, fat_fd);
    if (rc != RC_OK)
    {
        rtems_semaphore_release(fs_info->vol_sema);
        return rc;
    }

    /* 
     * if fat-file descriptor is not marked "removed" - synchronize file  
     * metadata
     */
    if (!FAT_FILE_IS_REMOVED(fat_fd))
    { 
        rc = msdos_set_first_cluster_num(iop->pathinfo.mt_entry, fat_fd);
        if (rc != RC_OK)
        {
            rtems_semaphore_release(fs_info->vol_sema);
            return rc;
        }
        rc = msdos_set_file_size(iop->pathinfo.mt_entry, fat_fd);
        if (rc != RC_OK)
        {
            rtems_semaphore_release(fs_info->vol_sema);
            return rc;
        } 
        rc = msdos_set_dir_wrt_time_and_date(iop->pathinfo.mt_entry, fat_fd);
        if (rc != RC_OK)
        {
            rtems_semaphore_release(fs_info->vol_sema);
            return rc;
        } 
    }
 
    rtems_semaphore_release(fs_info->vol_sema);
    return RC_OK;
}

/* msdos_file_datasync --
 *     Synchronize file - synchronize only file data (metadata is letf intact).
 *
 * PARAMETERS:
 *     iop - file control block
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured (errno set appropriately)
 */
int
msdos_file_datasync(rtems_libio_t *iop)
{
    int                rc = RC_OK;
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    fat_file_fd_t     *fat_fd = iop->file_info;
    msdos_fs_info_t   *fs_info = iop->pathinfo.mt_entry->fs_info;
  
    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        set_errno_and_return_minus_one(EIO);
  
    /* synchronize file data */
    rc = fat_file_datasync(iop->pathinfo.mt_entry, fat_fd);
 
    rtems_semaphore_release(fs_info->vol_sema);
    return RC_OK;
}


/* msdos_file_ioctl --
 *
 *
 * PARAMETERS:
 *     iop    - file control block
 *     ...
 *
 * RETURNS:
 *
 */
int 
msdos_file_ioctl(rtems_libio_t *iop,unsigned32 command, void *buffer)
{
    int rc = RC_OK;

    return rc;
}

/* msdos_file_rmnod --
 *     Remove node associated with a file - set up first name character to 
 *     predefined value(and write it to the disk), and mark fat-file which 
 *     correspondes to the file as "removed"
 *
 * PARAMETERS:
 *     pathloc - node description
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured (errno set appropriately)
 */
int 
msdos_file_rmnod(rtems_filesystem_location_info_t *pathloc)
{
    int                rc = RC_OK;
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info = pathloc->mt_entry->fs_info;
    fat_file_fd_t     *fat_fd = pathloc->node_access;

    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        set_errno_and_return_minus_one(EIO);

    /* mark file removed */
    rc = msdos_set_first_char4file_name(pathloc->mt_entry, fat_fd->info_cln, 
                                        fat_fd->info_ofs, 
                                        MSDOS_THIS_DIR_ENTRY_EMPTY);
    if (rc != RC_OK)
    {
        rtems_semaphore_release(fs_info->vol_sema);
        return rc;
    }

    fat_file_mark_removed(pathloc->mt_entry, fat_fd); 

    rtems_semaphore_release(fs_info->vol_sema);
    return RC_OK;
}

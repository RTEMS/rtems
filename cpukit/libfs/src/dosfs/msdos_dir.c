/**
 * @file
 *
 * @brief MSDOS Directory Handlers Implementation
 * @ingroup libfs_msdos MSDOS FileSystem
 */

/*
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 *  Modifications to support UTF-8 in the file system are
 *  Copyright (c) 2013 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <rtems/libio_.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>

#include "fat.h"
#include "fat_fat_operations.h"
#include "fat_file.h"

#include "msdos.h"



/*  msdos_dir_read --
 *      This routine will read the next directory entry based on the directory
 *      offset. The offset should be equal to -n- time the size of an
 *      individual dirent structure. If n is not an integer multiple of the
 *      sizeof a dirent structure, an integer division will be performed to
 *      determine directory entry that will be returned in the buffer. Count
 *      should reflect -m- times the sizeof dirent bytes to be placed in the
 *      buffer.
 *      If there are not -m- dirent elements from the current directory
 *      position to the end of the exisiting file, the remaining entries will
 *      be placed in the buffer and the returned value will be equal to
 *      -m actual- times the size of a directory entry.
 *
 * PARAMETERS:
 *     iop    - file control block
 *     buffer - buffer provided by user
 *     count  - count of bytes to read
 *
 * RETURNS:
 *     the number of bytes read on success, or -1 if error occured (errno
 *     set apropriately).
 */
ssize_t
msdos_dir_read(rtems_libio_t *iop, void *buffer, size_t count)
{
    int                rc = RC_OK;
    int                eno = 0;
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info = iop->pathinfo.mt_entry->fs_info;
    rtems_dosfs_convert_control *converter = fs_info->converter;
    const rtems_dosfs_convert_handler *convert_handler = converter->handler;
    fat_file_fd_t     *fat_fd = iop->pathinfo.node_access;
    fat_file_fd_t     *tmp_fat_fd = NULL;
    struct dirent      tmp_dirent;
    size_t             tmp_lfn_len = 0;
    uint16_t          *lfn_buf = converter->buffer.data;
    char              *sfn_buf = converter->buffer.data;
    const size_t       buf_size = converter->buffer.size;
    uint32_t           start = 0;
    ssize_t            ret = 0;
    ssize_t            cmpltd = 0;
    uint32_t           j = 0, i = 0;
    uint32_t           bts2rd = 0;
    uint32_t           cur_cln = 0;
    uint32_t           lfn_start = FAT_FILE_SHORT_NAME;
    uint8_t            lfn_checksum = 0;
    int                lfn_entries = 0;
    size_t             string_size = sizeof(tmp_dirent.d_name);
    bool               is_first_entry;

    /*
     * cast start and count - protect against using sizes that are not exact
     * multiples of the -dirent- size. These could result in unexpected
     * results
     */
    start = iop->offset / sizeof(struct dirent);
    count = (count / sizeof(struct dirent)) * sizeof(struct dirent);

    /*
     * optimization: we know that root directory for FAT12/16 volumes is
     * sequential set of sectors and any cluster is sequential set of sectors
     * too, so read such set of sectors is quick operation for low-level IO
     * layer.
     */
    bts2rd = (FAT_FD_OF_ROOT_DIR(fat_fd) &&
             (fs_info->fat.vol.type & (FAT_FAT12 | FAT_FAT16))) ?
             fat_fd->fat_file_size                              :
             fs_info->fat.vol.bpc;

    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        rtems_set_errno_and_return_minus_one(EIO);

    while (count > 0 && cmpltd >= 0)
    {
        /*
         * fat-file is already opened by open call, so read it
         * Always read directory fat-file from the beggining because of MSDOS
         * directories feature :( - we should count elements currently
         * present in the directory because there may be holes :)
         */
        ret = fat_file_read(&fs_info->fat, fat_fd, (j * bts2rd),
                            bts2rd, fs_info->cl_buf);
        if (ret < MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE)
        {
            rtems_semaphore_release(fs_info->vol_sema);
            rtems_set_errno_and_return_minus_one(EIO);
        }

        for (i = 0; i < ret && cmpltd >= 0; i += MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE)
        {
            char* entry = (char*) fs_info->cl_buf + i;

            /*
             * Is this directory from here on empty ?
             */
            if ((*MSDOS_DIR_ENTRY_TYPE(entry)) ==
                MSDOS_THIS_DIR_ENTRY_AND_REST_EMPTY)
            {
                rtems_semaphore_release(fs_info->vol_sema);
                return cmpltd;
            }

            /* Is the directory entry empty */
            if ((*MSDOS_DIR_ENTRY_TYPE(entry)) == MSDOS_THIS_DIR_ENTRY_EMPTY)
                continue;

            /* Is the directory entry empty a volume label */
            if (((*MSDOS_DIR_ATTR(entry)) & MSDOS_ATTR_VOLUME_ID) &&
                ((*MSDOS_DIR_ATTR(entry) & MSDOS_ATTR_LFN_MASK) != MSDOS_ATTR_LFN))
                continue;

            /*
             * Check the attribute to see if the entry is for a long file
             * name.
             */
            if ((*MSDOS_DIR_ATTR(entry) & MSDOS_ATTR_LFN_MASK) ==
                MSDOS_ATTR_LFN)
            {
                int offset_lfn;

                /*
                 * Is this is the first entry of a LFN ?
                 */
                if (lfn_start == FAT_FILE_SHORT_NAME)
                {
                    is_first_entry = true;
                    /*
                     * The first entry must have the last long entry flag set.
                     */
                    if ((*MSDOS_DIR_ENTRY_TYPE(entry) &
                         MSDOS_LAST_LONG_ENTRY) == 0)
                        continue;

                    /*
                     * Remember the start location of the long file name.
                     */
                    lfn_start =
                      ((j * bts2rd) + i) / MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE;

                    /*
                     * Get the number of entries so we can count down and
                     * also the checksum of the short entry.
                     */
                    lfn_entries = (*MSDOS_DIR_ENTRY_TYPE(entry) &
                                   MSDOS_LAST_LONG_ENTRY_MASK);
                    tmp_lfn_len = 0;
                    lfn_checksum = *MSDOS_DIR_LFN_CHECKSUM(entry);
                    memset (tmp_dirent.d_name, 0, sizeof(tmp_dirent.d_name));
                }
                else
                    is_first_entry = false;

                /*
                 * If the entry number or the check sum do not match
                 * forget this series of long directory entries. These could
                 * be orphaned entries depending on the history of the
                 * disk.
                 */
                if ((lfn_entries != (*MSDOS_DIR_ENTRY_TYPE(entry) &
                                     MSDOS_LAST_LONG_ENTRY_MASK)) ||
                    (lfn_checksum != *MSDOS_DIR_LFN_CHECKSUM(entry)))
                {
                    lfn_start = FAT_FILE_SHORT_NAME;
                    continue;
                }

                /*
                 * Extract the file name into the directory entry. The data is
                 * stored in UNICODE characters (16bit). No translation is
                 * done for the possibly partial entry.
                 * Once all entries have been assembled to a UTF-16 file name,
                 * this file name will get converted to UTF-8.
                 *
                 * The DOS maximum length is 255 characters without the
                 * trailing nul character. We need to range check the length to
                 * fit in the directory entry name field.
                 */

                lfn_entries--;
                offset_lfn = lfn_entries * MSDOS_LFN_LEN_PER_ENTRY;
                tmp_lfn_len += msdos_get_utf16_string_from_long_entry (
                  entry,
                  &lfn_buf[offset_lfn],
                  buf_size - offset_lfn,
                  is_first_entry
                );
            }
            else
            {
                fat_dir_pos_t dir_pos;

                /*
                 * Skip active entries until get the entry to start from.
                 */
                if (start)
                {
                    lfn_start = FAT_FILE_SHORT_NAME;
                    start--;
                    continue;
                }

                /*
                 * Move the entry to the return buffer
                 *
                 * unfortunately there is no method to extract ino except to
                 * open fat-file descriptor :( ... so, open it
                 */

                /* get number of cluster we are working with */
                rc = fat_file_ioctl(&fs_info->fat, fat_fd, F_CLU_NUM,
                                    j * bts2rd, &cur_cln);
                if (rc != RC_OK)
                {
                    rtems_semaphore_release(fs_info->vol_sema);
                    return rc;
                }

                fat_dir_pos_init(&dir_pos);
                dir_pos.sname.cln = cur_cln;
                dir_pos.sname.ofs = i;
                rc = fat_file_open(&fs_info->fat, &dir_pos, &tmp_fat_fd);
                if (rc != RC_OK)
                {
                    rtems_semaphore_release(fs_info->vol_sema);
                    return rc;
                }

                /* fill in dirent structure */
                /* XXX: from what and in what d_off should be computed ?! */
                tmp_dirent.d_off = start + cmpltd;
                tmp_dirent.d_reclen = sizeof(struct dirent);
                tmp_dirent.d_ino = tmp_fat_fd->ino;

                /*
                 * If a long file name check if the correct number of entries
                 * have been found and if the checksum is correct and if it is
                 * convertable to utf8 string.  If not return the short file
                 * name.
                 */
                if (lfn_start != FAT_FILE_SHORT_NAME)
                {
                    uint8_t  cs = 0;
                    uint8_t* p = (uint8_t*) entry;
                    int      i;

                    for (i = 0; i < 11; i++, p++)
                        cs = ((cs & 1) ? 0x80 : 0) + (cs >> 1) + *p;

                    if (lfn_entries || (lfn_checksum != cs))
                        lfn_start = FAT_FILE_SHORT_NAME;

                    eno = (*convert_handler->utf16_to_utf8) (
                        converter,
                        lfn_buf,
                        tmp_lfn_len,
                        (uint8_t*)(&tmp_dirent.d_name[0]),
                        &string_size);
                    if (eno == 0) {
                      tmp_dirent.d_namlen                    = string_size;
                      tmp_dirent.d_name[tmp_dirent.d_namlen] = '\0';
                    }
                    else {
                        lfn_start = FAT_FILE_SHORT_NAME;
                    }
                }

                if (lfn_start == FAT_FILE_SHORT_NAME)
                {
                    /*
                     * convert dir entry from fixed 8+3 format (without dot)
                     * to 0..8 + 1dot + 0..3 format
                     */
                    tmp_dirent.d_namlen = msdos_format_dirent_with_dot(
                        sfn_buf, entry); /* src text */
                    eno = (*convert_handler->codepage_to_utf8) (
                        converter,
                        sfn_buf,
                        tmp_dirent.d_namlen,
                        (uint8_t*)(&tmp_dirent.d_name[0]),
                        &string_size);
                    if ( 0 == eno ) {
                      tmp_dirent.d_namlen                    = string_size;
                      tmp_dirent.d_name[tmp_dirent.d_namlen] = '\0';
                    }
                    else {
                        cmpltd = -1;
                        errno  = eno;
                    }
                }

                if ( cmpltd >= 0 ) {
                    memcpy(buffer + cmpltd, &tmp_dirent, sizeof(struct dirent));

                    iop->offset = iop->offset + sizeof(struct dirent);
                    cmpltd += (sizeof(struct dirent));
                    count -= (sizeof(struct dirent));

                    /* inode number extracted, close fat-file */
                    rc = fat_file_close(&fs_info->fat, tmp_fat_fd);
                    if (rc != RC_OK)
                    {
                        rtems_semaphore_release(fs_info->vol_sema);
                        return rc;
                    }
                }
            }

            if (count <= 0)
                break;
        }
        j++;
    }

    rtems_semaphore_release(fs_info->vol_sema);
    return cmpltd;
}

/* msdos_dir_write --
 *     no write for directory
 */

/* msdos_dir_stat --
 *
 * This routine will obtain the following information concerning the current
 * directory:
 *     st_dev      device id
 *     st_ino      node serial number :)
 *     st_mode     mode extracted from the node
 *     st_size     total size in bytes
 *     st_blksize  blocksize for filesystem I/O
 *     st_blocks   number of blocks allocated
 *     stat_mtime  time of last modification
 *
 * PARAMETERS:
 *     loc - this directory
 *     buf - stat buffer provided by user
 *
 * RETURNS:
 *     RC_OK and filled stat buffer on success, or -1 if error occured (errno
 *     set apropriately).
 */
int
msdos_dir_stat(
    const rtems_filesystem_location_info_t *loc,
    struct stat *buf
)
{
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info = loc->mt_entry->fs_info;
    fat_file_fd_t     *fat_fd = loc->node_access;

    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        rtems_set_errno_and_return_minus_one(EIO);

    buf->st_dev = rtems_disk_get_device_identifier(fs_info->fat.vol.dd);
    buf->st_ino = fat_fd->ino;
    buf->st_mode  = S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO;
    buf->st_rdev = 0ll;
    buf->st_size = fat_fd->fat_file_size;
    buf->st_blocks = fat_fd->fat_file_size >> FAT_SECTOR512_BITS;
    buf->st_blksize = fs_info->fat.vol.bps;
    buf->st_mtime = fat_fd->mtime;

    rtems_semaphore_release(fs_info->vol_sema);
    return RC_OK;
}

/* msdos_dir_truncate --
 *     No truncate for directory.
 *
 * PARAMETERS:
 *
 * RETURNS:
 *
 */

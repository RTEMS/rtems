/*
 *  MSDOS directory handlers implementation
 *
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  @(#) $Id$
 */
#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <rtems/libio_.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>

#include "fat.h"
#include "fat_fat_operations.h"
#include "fat_file.h"

#include "msdos.h"

/* msdos_dir_open --
 *     Open fat-file which correspondes to the directory being opened and 
 *     set offset field of file control block to zero.
 *
 * PARAMETERS:
 *     iop        - file control block
 *     pathname   - name
 *     flag       - flags
 *     mode       - mode
 *
 * RETURNS:
 *     RC_OK, if directory opened successfully, or -1 if error occured (errno 
 *     set apropriately)
 */
int 
msdos_dir_open(rtems_libio_t *iop, const char *pathname, unsigned32 flag,
               unsigned32 mode)
{
    int                rc = RC_OK;  
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info = iop->pathinfo.mt_entry->fs_info; 
    fat_file_fd_t     *fat_fd = iop->file_info;

    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        set_errno_and_return_minus_one( EIO );
  
    rc = fat_file_reopen(fat_fd);
    if (rc != RC_OK)
    {
        rtems_semaphore_release(fs_info->vol_sema);
        return rc; 
    }

    iop->offset = 0;
    rtems_semaphore_release(fs_info->vol_sema);
    return RC_OK;
} 

/* msdos_dir_close --
 *     Close  fat-file which correspondes to the directory being closed
 *
 * PARAMETERS:
 *     iop - file control block
 *
 * RETURNS:
 *     RC_OK, if directory closed successfully, or -1 if error occured (errno 
 *     set apropriately.
 */
int 
msdos_dir_close(rtems_libio_t *iop)
{
    int                rc = RC_OK;
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info = iop->pathinfo.mt_entry->fs_info; 
    fat_file_fd_t     *fat_fd = iop->file_info;

    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        set_errno_and_return_minus_one( EIO );
  
    rc = fat_file_close(iop->pathinfo.mt_entry, fat_fd);
    if (rc != RC_OK)
    {
        rtems_semaphore_release(fs_info->vol_sema);
        return rc; 
    }

    rtems_semaphore_release(fs_info->vol_sema);
    return RC_OK;
}

/*  msdos_format_dirent_with_dot --
 *      This routine convert a (short) MSDOS filename as present on disk 
 *      (fixed 8+3 characters, filled with blanks, without separator dot)
 *      to a "normal" format, with between 0 and 8 name chars, 
 *      a separating dot and up to 3 extension characters
 *   Rules to work:
 *      - copy any (0-8) "name" part characters that are non-blank
 *      - if an extension exists, append a dot
 *      - copy any (0-3) non-blank extension characters
 *      - append a '\0' (dont count it for the rturn code
 *
 * PARAMETERS:
 *     dst: pointer to destination char array (must be big enough)
 *     src: pointer to source characters
 *
 *
 * RETURNS:
 *     the number of bytes (without trailing '\0'(written to destination 
 */
static ssize_t 
msdos_format_dirent_with_dot(char *dst,const char *src)
{
  ssize_t len;
  int i;
  const char *src_tmp;

  /*
   * find last non-blank character of base name
   */
  for ((i       =       MSDOS_SHORT_BASE_LEN  ,
	src_tmp = src + MSDOS_SHORT_BASE_LEN-1);
       ((i > 0) &&
	(*src_tmp == ' '));
       i--,src_tmp--)
    {};
  /*
   * copy base name to destination
   */
  src_tmp = src;
  len = i;
  while (i-- > 0) {
    *dst++ = *src_tmp++;
  }
  /*
   * find last non-blank character of extension
   */
  for ((i       =                            MSDOS_SHORT_EXT_LEN  ,
	src_tmp = src + MSDOS_SHORT_BASE_LEN+MSDOS_SHORT_EXT_LEN-1);
       ((i > 0) && 
	(*src_tmp == ' '));
       i--,src_tmp--)
    {};
  /*
   * extension is not empty
   */
  if (i > 0) {
    *dst++ = '.'; /* append dot */
    len += i + 1; /* extension + dot */
    src_tmp = src + MSDOS_SHORT_BASE_LEN;
    while (i-- > 0) {
      *dst++ = *src_tmp++;
      len++;
    }
  }
  *dst = '\0'; /* terminate string */

  return len;
}

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
msdos_dir_read(rtems_libio_t *iop, void *buffer, unsigned32 count)
{
    int                rc = RC_OK;
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info = iop->pathinfo.mt_entry->fs_info; 
    fat_file_fd_t     *fat_fd = iop->file_info;
    fat_file_fd_t     *tmp_fat_fd = NULL;
    struct dirent      tmp_dirent;
    unsigned32         start = 0;
    ssize_t            ret = 0;
    unsigned32         cmpltd = 0;
    unsigned32         j = 0, i = 0;
    unsigned32         bts2rd = 0;
    unsigned32         cur_cln = 0;
  
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
        set_errno_and_return_minus_one(EIO);
 
    while (count > 0)
    {  
        /* 
         * fat-file is already opened by open call, so read it 
         * Always read directory fat-file from the beggining because of MSDOS
         * directories feature :( - we should count elements currently 
         * present in the directory because there may be holes :)
         */
        ret = fat_file_read(iop->pathinfo.mt_entry, fat_fd, (j * bts2rd), 
                            bts2rd, fs_info->cl_buf);
        if (ret < MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE)
        {
            rtems_semaphore_release(fs_info->vol_sema);
            set_errno_and_return_minus_one(EIO);
        }

        for (i = 0; i < ret; i += MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE)
        {
            if ((*MSDOS_DIR_NAME(fs_info->cl_buf + i)) == 
                MSDOS_THIS_DIR_ENTRY_AND_REST_EMPTY)
            {
                rtems_semaphore_release(fs_info->vol_sema);          
                return cmpltd;
            }  
            
            if ((*MSDOS_DIR_NAME(fs_info->cl_buf + i)) == 
                MSDOS_THIS_DIR_ENTRY_EMPTY)
                continue;
      
            /* 
             * skip active entries until get the entry to start from
             */
            if (start)
            {
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
            rc = fat_file_ioctl(iop->pathinfo.mt_entry, fat_fd, F_CLU_NUM,
                                j * bts2rd, &cur_cln);
            if (rc != RC_OK)
            {
                rtems_semaphore_release(fs_info->vol_sema);
                return rc;
            }

            rc = fat_file_open(iop->pathinfo.mt_entry, cur_cln, i, 
                               &tmp_fat_fd);
            if (rc != RC_OK)
            {
                rtems_semaphore_release(fs_info->vol_sema);
                return rc;
            }

            tmp_fat_fd->info_cln = cur_cln;
            tmp_fat_fd->info_ofs = i;

            /* fill in dirent structure */
            /* XXX: from what and in what d_off should be computed ?! */
            tmp_dirent.d_off = start + cmpltd;
            tmp_dirent.d_reclen = sizeof(struct dirent);
            tmp_dirent.d_ino = tmp_fat_fd->ino;
	    /*
	     * convert dir entry from fixed 8+3 format (without dot)
	     * to 0..8 + 1dot + 0..3 format
	     */
	    tmp_dirent.d_namlen = 
	      msdos_format_dirent_with_dot(tmp_dirent.d_name,
					 fs_info->cl_buf + i); /* src text */
            memcpy(buffer + cmpltd, &tmp_dirent, sizeof(struct dirent));
   
            iop->offset = iop->offset + sizeof(struct dirent);
            cmpltd += (sizeof(struct dirent));
            count -= (sizeof(struct dirent));
      
            /* inode number extracted, close fat-file */
            rc = fat_file_close(iop->pathinfo.mt_entry, tmp_fat_fd);
            if (rc != RC_OK)
            {
                rtems_semaphore_release(fs_info->vol_sema);
                return rc;
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

/* msdos_dir_lseek --
 *
 *  This routine will behave in one of three ways based on the state of
 *  argument whence. Based on the state of its value the offset argument will
 *  be interpreted using one of the following methods:
 *
 *     SEEK_SET - offset is the absolute byte offset from the start of the
 *                logical start of the dirent sequence that represents the
 *                directory
 *     SEEK_CUR - offset is used as the relative byte offset from the current
 *                directory position index held in the iop structure
 *     SEEK_END - N/A --> This will cause an assert.
 *
 * PARAMETERS:
 *     iop    - file control block
 *     offset - offset
 *     whence - predefine directive
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured (errno 
 *     set apropriately).
 */
int 
msdos_dir_lseek(rtems_libio_t *iop, off_t offset, int whence)
{
    switch (whence) 
    {
        case SEEK_SET:
        case SEEK_CUR: 
            break;
        /* 
         * Movement past the end of the directory via lseek is not a 
         * permitted operation
         */
        case SEEK_END:
        default:
            set_errno_and_return_minus_one( EINVAL );
            break;
    }
    return RC_OK;
}

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
    buf->st_mode  = S_IFDIR;
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

/* msdos_dir_sync --
 *     The following routine does a syncronization on a MSDOS directory node.
 *     DIR_WrtTime, DIR_WrtDate and DIR_fileSize fields of 32 Bytes Directory 
 *     Entry Structure should not be updated for directories, so only call 
 *     to corresponding fat-file routine.
 *
 * PARAMETERS:
 *     iop - file control block
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured (errno set apropriately).
 */
int
msdos_dir_sync(rtems_libio_t *iop)
{
    int                rc = RC_OK;
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    fat_file_fd_t     *fat_fd = iop->file_info;
    msdos_fs_info_t   *fs_info = iop->pathinfo.mt_entry->fs_info;
  
    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        set_errno_and_return_minus_one(EIO);
  
    rc = fat_file_datasync(iop->pathinfo.mt_entry, fat_fd);

    rtems_semaphore_release(fs_info->vol_sema);
    return rc;
}

/* msdos_dir_rmnod --
 *     Remove directory node. 
 *
 *     Check that this directory node is not opened as fat-file, is empty and 
 *     not filesystem root node. If all this conditions met then delete.
 *
 * PARAMETERS:
 *     pathloc - node description
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured (errno set apropriately).
 */
int 
msdos_dir_rmnod(rtems_filesystem_location_info_t *pathloc)
{
    int                rc = RC_OK;
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info = pathloc->mt_entry->fs_info; 
    fat_file_fd_t     *fat_fd = pathloc->node_access;
    rtems_boolean      is_empty = FALSE;
  
    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        set_errno_and_return_minus_one(EIO);
  
    /*
     * We deny attemp to delete open directory (if directory is current 
     * directory we assume it is open one)
     */
    if (fat_fd->links_num > 1)
    {
        rtems_semaphore_release(fs_info->vol_sema);
        set_errno_and_return_minus_one(EBUSY);
    } 
  
    /*
     * You cannot remove a node that still has children
     */
    rc = msdos_dir_is_empty(pathloc->mt_entry, fat_fd, &is_empty); 
    if (rc != RC_OK)
    {
        rtems_semaphore_release(fs_info->vol_sema);
        return rc;
    }

    if (!is_empty)
    {
        rtems_semaphore_release(fs_info->vol_sema); 
        set_errno_and_return_minus_one(ENOTEMPTY);
    }     

    /*
     * You cannot remove the file system root node.
     */
    if (pathloc->mt_entry->mt_fs_root.node_access == pathloc->node_access)
    {
        rtems_semaphore_release(fs_info->vol_sema);
        set_errno_and_return_minus_one(EBUSY);
    }

    /*
     * You cannot remove a mountpoint.
     * not used - mount() not implemenetd yet.
     */

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
    return rc;
} 

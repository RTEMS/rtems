/*
 *  fat_file.c
 *
 *  General operations on "fat-file"
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 * @(#) $Id$
 *
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include <rtems/libio_.h>

#include "fat.h"
#include "fat_fat_operations.h"
#include "fat_file.h"

static inline void
_hash_insert(Chain_Control *hash, unsigned32 key1, unsigned32 key2, 
             fat_file_fd_t *el);

static inline void
_hash_delete(Chain_Control *hash, unsigned32 key1, unsigned32 key2, 
             fat_file_fd_t *el);

static inline int 
_hash_search(
    rtems_filesystem_mount_table_entry_t  *mt_entry,
    Chain_Control                         *hash, 
    unsigned32                             key1, 
    unsigned32                             key2, 
    void                                 **ret
);

static int 
fat_file_lseek(
    rtems_filesystem_mount_table_entry_t  *mt_entry,
    fat_file_fd_t                         *fat_fd,
    unsigned32                             file_cln,
    unsigned32                            *disk_cln
);

/* fat_file_open --
 *     Open fat-file. Two hash tables are accessed by key 
 *     constructed from cluster num and offset of the node (i.e. 
 *     files/directories are distinguished by location on the disk).
 *     First, hash table("vhash") consists of fat-file descriptors corresponded 
 *     to "valid" files is accessed. Search is made by 2 fields equal to key 
 *     constructed. If descriptor is found in the "vhash" - return it. 
 *     Otherwise search is made in hash table("rhash") consits of fat-file 
 *     descriptors corresponded to "removed-but-still-open" files with the 
 *     same keys.
 *     If search failed, new fat-file descriptor is added to "vhash"
 *     with both key fields equal to constructed key. Otherwise new fat-file 
 *     descriptor is added to "vhash" with first key field equal to key
 *     constructed and the second equal to an unique (unique among all values
 *     of second key fields) value.  
 *
 * PARAMETERS:
 *     mt_entry - mount table entry
 *     cln      - cluster num of the node
 *     ofs      - offset of the node
 *     fat_fd   - placeholder for returned fat-file descriptor
 *
 * RETURNS:
 *     RC_OK and pointer to opened descriptor on success, or -1 if error 
 *     occured (errno set appropriately)
 */
int
fat_file_open(
    rtems_filesystem_mount_table_entry_t  *mt_entry,
    unsigned32                             cln,
    unsigned32                             ofs,  
    fat_file_fd_t                        **fat_fd
    )
{
    int            rc = RC_OK; 
    fat_fs_info_t *fs_info = mt_entry->fs_info;
    fat_file_fd_t *lfat_fd = NULL;
    unsigned32     key = 0;
  
    /* construct key */
    key = fat_construct_key(mt_entry, cln, ofs);
  
    /* access "valid" hash table */
    rc = _hash_search(mt_entry, fs_info->vhash, key, 0, (void **)&lfat_fd);
    if ( rc == RC_OK )  
    {
        /* return pointer to fat_file_descriptor allocated before */
        (*fat_fd) = lfat_fd;
        lfat_fd->links_num++;
        return rc;            
    }

    /* access "removed-but-still-open" hash table */
    rc = _hash_search(mt_entry, fs_info->rhash, key, key, (void **)&lfat_fd);

    lfat_fd = (*fat_fd) = (fat_file_fd_t*)malloc(sizeof(fat_file_fd_t));
    if ( lfat_fd == NULL )
        set_errno_and_return_minus_one( ENOMEM );  

    lfat_fd->links_num = 1;
    lfat_fd->flags &= ~FAT_FILE_REMOVED;
    lfat_fd->map.last_cln = FAT_UNDEFINED_VALUE;
    
    if ( rc != RC_OK )
        lfat_fd->ino = key;
    else
    {
        lfat_fd->ino = fat_get_unique_ino(mt_entry);
    
        if ( lfat_fd->ino == 0 )
        {
            free((*fat_fd));
            /* 
             * XXX: kernel resource is unsufficient, but not the memory, 
             * but there is no suitable errno :(
             */
            set_errno_and_return_minus_one( ENOMEM );  
        }
    }
    _hash_insert(fs_info->vhash, key, lfat_fd->ino, lfat_fd);
    

    /* 
     * other fields of fat-file descriptor will be initialized on upper 
     * level 
     */

    return RC_OK;
}


/* fat_file_reopen --
 *     Increment by 1 number of links    
 *
 * PARAMETERS:
 *     fat_fd - fat-file descriptor
 *
 * RETURNS:
 *     RC_OK  
 */
int
fat_file_reopen(fat_file_fd_t *fat_fd)
{
    fat_fd->links_num++;
    return RC_OK;
}

/* fat_file_close --
 *     Close fat-file. If count of links to fat-file 
 *     descriptor is greater than 1 (i.e. somebody esle holds pointer
 *     to this descriptor) just decrement it. Otherwise 
 *     do the following. If this descriptor corresponded to removed fat-file
 *     then free clusters contained fat-file data, delete descriptor from 
 *     "rhash" table and free memory allocated by descriptor. If descriptor 
 *     correspondes to non-removed fat-file and 'ino' field has value from 
 *     unique inode numbers pool then set count of links to descriptor to zero 
 *     and leave it in hash, otherwise delete descriptor from "vhash" and free 
 *     memory allocated by the descriptor
 *
 * PARAMETERS:
 *     mt_entry - mount table entry
 *     fat_fd   - fat-file descriptor
 *
 * RETURNS:
 *     RC_OK, or -1 if error occured (errno set appropriately)
 */
int
fat_file_close(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_file_fd_t                        *fat_fd
    )
{
    int            rc = RC_OK;
    fat_fs_info_t *fs_info = mt_entry->fs_info;
    unsigned32     key = 0;

    /*
     * if links_num field of fat-file descriptor is greater than 1  
     * decrement the count of links and return
     */
    if (fat_fd->links_num > 1)
    {
        fat_fd->links_num--;
        return rc;
    }

    key = fat_construct_key(mt_entry, fat_fd->info_cln, fat_fd->info_ofs);

    if (fat_fd->flags & FAT_FILE_REMOVED)
    {
        rc = fat_file_truncate(mt_entry, fat_fd, 0);
        if ( rc != RC_OK )
            return rc;

        _hash_delete(fs_info->rhash, key, fat_fd->ino, fat_fd);

        if ( fat_ino_is_unique(mt_entry, fat_fd->ino) )
            fat_free_unique_ino(mt_entry, fat_fd->ino);

        free(fat_fd);
    }
    else
    {
        if (fat_ino_is_unique(mt_entry, fat_fd->ino))
        {
            fat_fd->links_num = 0;
        }
        else
        {
            _hash_delete(fs_info->vhash, key, fat_fd->ino, fat_fd);
            free(fat_fd);
        }  
    }  
    /*
     * flush any modified "cached" buffer back to disk
     */
    rc = fat_buf_release(fs_info);

    return rc;
}

/* fat_file_read --
 *     Read 'count' bytes from 'start' position from fat-file. This 
 *     interface hides the architecture of fat-file, represents it as
 *     linear file
 *
 * PARAMETERS:
 *     mt_entry - mount table entry
 *     fat_fd   - fat-file descriptor
 *     start    - offset in fat-file (in bytes) to read from
 *     count    - count of bytes to read
 *     buf      - buffer provided by user  
 *
 * RETURNS:
 *     the number of bytes read on success, or -1 if error occured (errno 
 *     set appropriately)
 */
ssize_t
fat_file_read(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_file_fd_t                        *fat_fd,
    unsigned32                            start,
    unsigned32                            count,
    char                                 *buf
    )
{
    int            rc = RC_OK;
    ssize_t        ret = 0;
    fat_fs_info_t *fs_info = mt_entry->fs_info;
    unsigned32     cmpltd = 0;
    unsigned32     cur_cln = 0;
    unsigned32     cl_start = 0;
    unsigned32     save_cln = 0;
    unsigned32     ofs = 0;
    unsigned32     save_ofs;
    unsigned32     sec = 0;
    unsigned32     byte = 0;
    unsigned32     c = 0;

    /* it couldn't be removed - otherwise cache update will be broken */
    if (count == 0)
        return cmpltd;

    /* 
     * >= because start is offset and computed from 0 and file_size 
     * computed from 1
     */
    if ( start >= fat_fd->fat_file_size )
        return FAT_EOF;
  
    if ((count > fat_fd->fat_file_size) || 
        (start > fat_fd->fat_file_size - count)) 
        count = fat_fd->fat_file_size - start;
  
    if ((FAT_FD_OF_ROOT_DIR(fat_fd)) && 
        (fs_info->vol.type & (FAT_FAT12 | FAT_FAT16)))  
    {
        sec = fat_cluster_num_to_sector_num(mt_entry, fat_fd->cln);
        sec += (start >> fs_info->vol.sec_log2);
        byte = start & (fs_info->vol.bps - 1);

        ret = _fat_block_read(mt_entry, sec, byte, count, buf); 
        if ( ret < 0 )
            return -1;  

        return ret;
    }     
    
    cl_start = start >> fs_info->vol.bpc_log2; 
    save_ofs = ofs = start & (fs_info->vol.bpc - 1);

    rc = fat_file_lseek(mt_entry, fat_fd, cl_start, &cur_cln);
    if (rc != RC_OK)
        return rc;
        
    while (count > 0)
    {
        c = MIN(count, (fs_info->vol.bpc - ofs));

        sec = fat_cluster_num_to_sector_num(mt_entry, cur_cln);
        sec += (ofs >> fs_info->vol.sec_log2);
        byte = ofs & (fs_info->vol.bps - 1);

        ret = _fat_block_read(mt_entry, sec, byte, c, buf + cmpltd);
        if ( ret < 0 )
            return -1;  

        count -= c;
        cmpltd += c;
        save_cln = cur_cln;
        rc = fat_get_fat_cluster(mt_entry, cur_cln, &cur_cln);
        if ( rc != RC_OK )
            return rc;  

        ofs = 0;
    }

    /* update cache */
    /* XXX: check this - I'm not sure :( */
    fat_fd->map.file_cln = cl_start + 
                           ((save_ofs + cmpltd - 1) >> fs_info->vol.bpc_log2);
    fat_fd->map.disk_cln = save_cln;

    return cmpltd;
}

/* fat_file_write --
 *     Write 'count' bytes of data from user supplied buffer to fat-file 
 *     starting at offset 'start'. This interface hides the architecture 
 *     of fat-file, represents it as linear file
 *
 * PARAMETERS:
 *     mt_entry - mount table entry
 *     fat_fd   - fat-file descriptor
 *     start    - offset(in bytes) to write from
 *     count    - count
 *     buf      - buffer provided by user  
 *
 * RETURNS:
 *     number of bytes actually written to the file on success, or -1 if 
 *     error occured (errno set appropriately)
 */
ssize_t
fat_file_write(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_file_fd_t                        *fat_fd,
    unsigned32                            start,
    unsigned32                            count,
    const char                            *buf
    )
{
    int            rc = 0;
    ssize_t        ret = 0;
    fat_fs_info_t *fs_info = mt_entry->fs_info;
    unsigned32     cmpltd = 0;
    unsigned32     cur_cln = 0;
    unsigned32     save_cln = 0; /* FIXME: This might be incorrect, cf. below */
    unsigned32     cl_start = 0;
    unsigned32     ofs = 0;  
    unsigned32     save_ofs;
    unsigned32     sec = 0;
    unsigned32     byte = 0;
    unsigned32     c = 0;
  
    if ( count == 0 )
        return cmpltd;

    if ( start > fat_fd->fat_file_size )
        set_errno_and_return_minus_one( EIO );

    if ((count > fat_fd->size_limit) || 
        (start > fat_fd->size_limit - count)) 
        set_errno_and_return_minus_one( EIO );  
         
    rc = fat_file_extend(mt_entry, fat_fd, start + count, &c);
    if (rc != RC_OK)
        return rc;

    /* 
     * check whether there was enough room on device to locate 
     * file of 'start + count' bytes
     */
    if (c != (start + count))
        count = c - start;    
  
    if ((FAT_FD_OF_ROOT_DIR(fat_fd)) && 
        (fs_info->vol.type & (FAT_FAT12 | FAT_FAT16)))  
    {
        sec = fat_cluster_num_to_sector_num(mt_entry, fat_fd->cln);
        sec += (start >> fs_info->vol.sec_log2);
        byte = start & (fs_info->vol.bps - 1);

        ret = _fat_block_write(mt_entry, sec, byte, count, buf); 
        if ( ret < 0 )
            return -1;  

        return ret;
    }     
    
    cl_start = start >> fs_info->vol.bpc_log2; 
    save_ofs = ofs = start & (fs_info->vol.bpc - 1);

    rc = fat_file_lseek(mt_entry, fat_fd, cl_start, &cur_cln);
    if (rc != RC_OK)
        return rc;

    while (count > 0)
    {
        c = MIN(count, (fs_info->vol.bpc - ofs));

        sec = fat_cluster_num_to_sector_num(mt_entry, cur_cln);
        sec += (ofs >> fs_info->vol.sec_log2);
        byte = ofs & (fs_info->vol.bps - 1);

        ret = _fat_block_write(mt_entry, sec, byte, c, buf + cmpltd);
        if ( ret < 0 )
            return -1;  

        count -= c;
        cmpltd += c;
        save_cln = cur_cln;
        rc = fat_get_fat_cluster(mt_entry, cur_cln, &cur_cln);
        if ( rc != RC_OK )
            return rc;  

        ofs = 0;
    }

    /* update cache */
    /* XXX: check this - I'm not sure :( */
    fat_fd->map.file_cln = cl_start + 
                           ((save_ofs + cmpltd - 1) >> fs_info->vol.bpc_log2);
    fat_fd->map.disk_cln = save_cln;

    return cmpltd;
}

/* fat_file_extend --
 *     Extend fat-file. If new length less than current fat-file size - 
 *     do nothing. Otherwise calculate necessary count of clusters to add, 
 *     allocate it and add new clusters chain to the end of 
 *     existing clusters chain.
 *
 * PARAMETERS:
 *     mt_entry   - mount table entry
 *     fat_fd     - fat-file descriptor
 *     new_length - new length  
 *     a_length   - placeholder for result - actual new length of file
 *
 * RETURNS:
 *     RC_OK and new length of file on success, or -1 if error occured (errno 
 *     set appropriately)
 */
int
fat_file_extend(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_file_fd_t                        *fat_fd, 
    unsigned32                            new_length,
    unsigned32                           *a_length
    )
{
    int            rc = RC_OK;
    fat_fs_info_t *fs_info = mt_entry->fs_info;
    unsigned32     chain = 0; 
    unsigned32     bytes2add = 0;
    unsigned32     cls2add = 0;
    unsigned32     old_last_cl;
    unsigned32     last_cl = 0;
    unsigned32     bytes_remain = 0;
    unsigned32     cls_added;
  
    *a_length = new_length;

    if (new_length <= fat_fd->fat_file_size)
        return RC_OK;

    if ((FAT_FD_OF_ROOT_DIR(fat_fd)) && 
        (fs_info->vol.type & (FAT_FAT12 | FAT_FAT16)))  
        set_errno_and_return_minus_one( ENOSPC );

    bytes_remain = (fs_info->vol.bpc - 
                   (fat_fd->fat_file_size & (fs_info->vol.bpc - 1))) & 
                   (fs_info->vol.bpc - 1);

    bytes2add = new_length - fat_fd->fat_file_size;
    
    if (bytes2add > bytes_remain)
        bytes2add -= bytes_remain;
    else
        bytes2add = 0;    

    /* 
     * if in last cluster allocated for the file there is enough room to
     * handle extention (hence we don't need to add even one cluster to the 
     * file ) - return
     */
    if (bytes2add == 0)
        return RC_OK;  

    cls2add = ((bytes2add - 1) >> fs_info->vol.bpc_log2) + 1;
    
    rc = fat_scan_fat_for_free_clusters(mt_entry, &chain, cls2add, 
                                        &cls_added, &last_cl);    

    /* this means that low level I/O error occured */
    if (rc != RC_OK)
        return rc;

    /* this means that no space left on device */
    if ((cls_added == 0) && (bytes_remain == 0))
        set_errno_and_return_minus_one(ENOSPC);    

    /*  check wether we satisfied request for 'cls2add' clusters */
    if (cls2add != cls_added)
        *a_length = new_length - 
                    ((cls2add - cls_added - 1) << fs_info->vol.bpc_log2) - 
                    (bytes2add & (fs_info->vol.bpc - 1));

    /* add new chain to the end of existed */
    if ( fat_fd->fat_file_size == 0 )
    {
        fat_fd->map.disk_cln = fat_fd->cln = chain;
        fat_fd->map.file_cln = 0;
    }
    else
    {
        if (fat_fd->map.last_cln != FAT_UNDEFINED_VALUE)
        {
            old_last_cl = fat_fd->map.last_cln;
        }
        else
        {
            rc = fat_file_ioctl(mt_entry, fat_fd, F_CLU_NUM,
                                (fat_fd->fat_file_size - 1), &old_last_cl);
            if ( rc != RC_OK )
            {
                fat_free_fat_clusters_chain(mt_entry, chain);
                return rc;
            }
        }    

        rc = fat_set_fat_cluster(mt_entry, old_last_cl, chain);
        if ( rc != RC_OK )
        {
            fat_free_fat_clusters_chain(mt_entry, chain);
            return rc;  
        }
        fat_buf_release(fs_info);
    }
    
    /* update number of the last cluster of the file if it changed */
    if (cls_added != 0)
    {
        fat_fd->map.last_cln = last_cl;
        if (fat_fd->fat_file_type == FAT_DIRECTORY)
        {
            rc = fat_init_clusters_chain(mt_entry, chain);
            if ( rc != RC_OK )
            {
                fat_free_fat_clusters_chain(mt_entry, chain);
                return rc;  
            }
        }
    }     

    return RC_OK;
}

/* fat_file_truncate --
 *     Truncate fat-file. If new length greater than current fat-file size - 
 *     do nothing. Otherwise find first cluster to free and free all clusters
 *     in the chain starting from this cluster.
 *     
 * PARAMETERS:
 *     mt_entry   - mount table entry
 *     fat_fd     - fat-file descriptor
 *     new_length - new length  
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured (errno set appropriately)
 */
int
fat_file_truncate(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_file_fd_t                        *fat_fd,     
    unsigned32                            new_length
    )
{
    int            rc = RC_OK;
    fat_fs_info_t *fs_info = mt_entry->fs_info;
    unsigned32     cur_cln = 0;
    unsigned32     cl_start = 0;
    unsigned32     new_last_cln = FAT_UNDEFINED_VALUE;
  
    
    if ( new_length >= fat_fd->fat_file_size )
        return rc;

    assert(fat_fd->fat_file_size);
    
    cl_start = (new_length + fs_info->vol.bpc - 1) >> fs_info->vol.bpc_log2;
    
    if ((cl_start << fs_info->vol.bpc_log2) >= fat_fd->fat_file_size)
        return RC_OK;
        
    if (cl_start != 0)
    {
        rc = fat_file_lseek(mt_entry, fat_fd, cl_start - 1, &new_last_cln);
        if (rc != RC_OK)
            return rc;

    }        

    rc = fat_file_lseek(mt_entry, fat_fd, cl_start, &cur_cln);
    if (rc != RC_OK)
        return rc;

    rc = fat_free_fat_clusters_chain(mt_entry, cur_cln);
    if (rc != RC_OK)
        return rc;

    if (cl_start != 0)
    {
        rc = fat_set_fat_cluster(mt_entry, new_last_cln, FAT_GENFAT_EOC);
        if ( rc != RC_OK )
            return rc;  
        fat_fd->map.file_cln = cl_start - 1;
        fat_fd->map.disk_cln = new_last_cln;
        fat_fd->map.last_cln = new_last_cln;
    }     
    return RC_OK;
}                  

/* fat_file_ioctl --
 *     F_CLU_NUM:
 *         make mapping between serial number of the cluster in fat-file and 
 *         its real number on the volume     
 *          
 * PARAMETERS:
 *     fat_fd     - fat-file descriptor
 *     mt_entry   - mount table entry
 *     cmd        - command
 *     ... 
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured and errno set appropriately
 */
int
fat_file_ioctl(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_file_fd_t                        *fat_fd, 
    int                                   cmd, 
    ...)
{
    int            rc = RC_OK;
    fat_fs_info_t *fs_info = mt_entry->fs_info;
    unsigned32     cur_cln = 0;
    unsigned32     cl_start = 0;
    unsigned32     pos = 0;
    unsigned32    *ret;
    va_list        ap; 
  
    va_start(ap, cmd);

    switch (cmd)
    {
        case F_CLU_NUM:
            pos = va_arg(ap, int);
            ret = va_arg(ap, int *);

            /* sanity check */
            if ( pos >= fat_fd->fat_file_size )
                set_errno_and_return_minus_one( EIO );

            if ((FAT_FD_OF_ROOT_DIR(fat_fd)) && 
                (fs_info->vol.type & (FAT_FAT12 | FAT_FAT16)))
            {
                /* cluster 0 (zero) reserved for root dir */
                *ret  = 0;
                return RC_OK;
            }     
    
            cl_start = pos >> fs_info->vol.bpc_log2; 
               
            rc = fat_file_lseek(mt_entry, fat_fd, cl_start, &cur_cln);
            if ( rc != RC_OK )
                return rc;  

            *ret = cur_cln;
            break;

        default:
            errno = EINVAL;
            rc = -1;
            break;  
    }
    return rc;
}

/* fat_file_mark_removed --
 *     Remove the fat-file descriptor from "valid" hash table, insert it 
 *     into "removed-but-still-open" hash table and set up "removed" bit. 
 *  
 * PARAMETERS:
 *     fat_fd     - fat-file descriptor
 *     mt_entry   - mount table entry
 *
 * RETURNS:
 *     None
 */
void
fat_file_mark_removed(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_file_fd_t                        *fat_fd
    )
{
    fat_fs_info_t *fs_info = mt_entry->fs_info;
    unsigned32     key = 0;
  
    key = fat_construct_key(mt_entry, fat_fd->info_cln, fat_fd->info_ofs);
  
    _hash_delete(fs_info->vhash, key, fat_fd->ino, fat_fd);

    _hash_insert(fs_info->rhash, key, fat_fd->ino, fat_fd);

    fat_fd->flags |= FAT_FILE_REMOVED;  
} 

/* fat_file_datasync --
 *     Synchronize fat-file -  flush all buffered data to the media.
 *  
 * PARAMETERS:
 *     mt_entry - mount table entry
 *     fat_fd   - fat-file descriptor
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured and errno set appropriately
 */
int
fat_file_datasync(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_file_fd_t                        *fat_fd
    )
{
    int                rc = RC_OK;
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    fat_fs_info_t     *fs_info = mt_entry->fs_info;
    unsigned32         cur_cln = fat_fd->cln; 
    bdbuf_buffer      *block = NULL;
    unsigned32         sec = 0;
    unsigned32         i = 0;
  
    if (fat_fd->fat_file_size == 0)
        return RC_OK;
        
    /* 
     * we can use only one bdbuf :( and we also know that cache is useless 
     * for sync operation, so don't use it
     */
    rc = fat_buf_release(fs_info);
    if (rc != RC_OK)
        return rc;
        
    /* for each cluster of the file ... */
    while ((cur_cln & fs_info->vol.mask) < fs_info->vol.eoc_val)
    {
        sec = fat_cluster_num_to_sector_num(mt_entry, cur_cln);
        /* for each sector in cluster ... */
        for ( i = 0; i < fs_info->vol.spc; i++ )
        {
            /* ... sync it */
            sc = rtems_bdbuf_read(fs_info->vol.dev, (sec + i), &block);      
            if (sc != RTEMS_SUCCESSFUL)
                set_errno_and_return_minus_one( EIO );

            sc = rtems_bdbuf_sync(block);      
            if ( sc != RTEMS_SUCCESSFUL )
                set_errno_and_return_minus_one( EIO );
        }

        rc = fat_get_fat_cluster(mt_entry, cur_cln, &cur_cln);
        if ( rc != RC_OK )
            return rc;
    }
    return rc;
}   

/* fat_file_size --
 *     Calculate fat-file size - fat-file is nothing that clusters chain, so
 *     go through all clusters in the chain and count it. Only
 *     special case is root directory for FAT12/16 volumes.
 *     This function is used only for directories which are fat-files with
 *     non-zero length, hence 'fat_fd->cln' always contains valid data.
 *     Calculated size is stored in 'fat_file_size' field of fat-file 
 *     descriptor.
 *  
 * PARAMETERS:
 *     mt_entry - mount table entry
 *     fat_fd   - fat-file descriptor
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured (errno set appropriately)
 */
int
fat_file_size( 
    rtems_filesystem_mount_table_entry_t *mt_entry,
    fat_file_fd_t                        *fat_fd
    )
{
    int            rc = RC_OK;
    fat_fs_info_t *fs_info = mt_entry->fs_info;
    unsigned32     cur_cln = fat_fd->cln; 
    unsigned32     save_cln = 0;
  
    /* Have we requested root dir size for FAT12/16? */
    if ((FAT_FD_OF_ROOT_DIR(fat_fd)) && 
        (fs_info->vol.type & (FAT_FAT12 | FAT_FAT16)))  
    {
        fat_fd->fat_file_size = fs_info->vol.rdir_size;
        return rc;
    }
  
    fat_fd->fat_file_size = 0;
  
    while ((cur_cln & fs_info->vol.mask) < fs_info->vol.eoc_val)
    {
        save_cln = cur_cln;
        rc = fat_get_fat_cluster(mt_entry, cur_cln, &cur_cln);
        if ( rc != RC_OK )
            return rc;

        fat_fd->fat_file_size += fs_info->vol.bpc;
    }
    fat_fd->map.last_cln = save_cln;
    return rc;
}  

/* hash support routines */

/* _hash_insert --
 *     Insert elemnt into hash based on key 'key1'
 *
 * PARAMETERS:
 *     hash - hash element will be inserted into
 *     key1 - key on which insertion is based on
 *     key2 - not used during insertion
 *     el   - element to insert
 *
 * RETURNS:
 *     None
 */
static inline void
_hash_insert(Chain_Control *hash, unsigned32 key1, unsigned32 key2, 
             fat_file_fd_t *el)
{
    _Chain_Append((hash) + ((key1) % FAT_HASH_MODULE), &(el)->link);
}


/* _hash_delete --
 *     Remove element from hash
 *
 * PARAMETERS:
 *     hash - hash element will be removed from
 *     key1 - not used
 *     key2 - not used 
 *     el   - element to delete
 *
 * RETURNS:
 *     None
 */
static inline void
_hash_delete(Chain_Control *hash, unsigned32 key1, unsigned32 key2, 
             fat_file_fd_t *el)
{
    _Chain_Extract(&(el)->link);
}   

/* _hash_search --
 *     Search element in hash. If both keys match pointer to found element
 *     is returned
 *
 * PARAMETERS:
 *     mt_entry - mount table entry 
 *     hash     - hash element will be removed from
 *     key1     - search key
 *     key2     - search key
 *     ret      - placeholder for result
 *
 * RETURNS:
 *     0 and pointer to found element on success, -1 otherwise
 */
static inline int 
_hash_search(
    rtems_filesystem_mount_table_entry_t  *mt_entry,
    Chain_Control                         *hash, 
    unsigned32                             key1, 
    unsigned32                             key2, 
    void                                 **ret
    )
{                          
    unsigned32 mod = (key1) % FAT_HASH_MODULE;
    Chain_Node *the_node = ((Chain_Control *)((hash) + mod))->first;

    for ( ; !_Chain_Is_tail((hash) + mod, the_node) ; ) 
    {
        fat_file_fd_t *ffd = (fat_file_fd_t *)the_node;
        unsigned32 ck = 
                fat_construct_key(mt_entry, ffd->info_cln, ffd->info_ofs);

        if ( (key1) == ck)
        {
            if ( ((key2) == 0) || ((key2) == ffd->ino) )
            {
                *ret = (void *)the_node;
                return 0;
            }
        }
        the_node = the_node->next; 
    }
    return -1; 
}

static int 
fat_file_lseek(
    rtems_filesystem_mount_table_entry_t  *mt_entry,
    fat_file_fd_t                         *fat_fd,
    unsigned32                             file_cln,
    unsigned32                            *disk_cln
    )
{
    int rc = RC_OK;
/*    
    assert(fat_fd->fat_file_size);
 */   
    if (file_cln == fat_fd->map.file_cln)
        *disk_cln = fat_fd->map.disk_cln;
    else
    {
        unsigned32 cur_cln;
        unsigned32 count;
        unsigned32 i;
        
        if (file_cln > fat_fd->map.file_cln)
        {
            cur_cln = fat_fd->map.disk_cln;
            count = file_cln - fat_fd->map.file_cln;
        } 
        else
        {
            cur_cln = fat_fd->cln;
            count = file_cln;
        } 

        /* skip over the clusters */
        for (i = 0; i < count; i++)
        {
            rc = fat_get_fat_cluster(mt_entry, cur_cln, &cur_cln);
            if ( rc != RC_OK )
                return rc;  
        }                              

        /* update cache */
        fat_fd->map.file_cln = file_cln;
        fat_fd->map.disk_cln = cur_cln;
        
        *disk_cln = cur_cln;
    }
    return RC_OK;    
}

/*
 *  fat_file.h
 *
 *  Constants/data structures/prototypes for operations on "fat-file"
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
#ifndef __DOSFS_FAT_FILE_H__
#define __DOSFS_FAT_FILE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <rtems/libio_.h>

#include <time.h>

/* "fat-file" representation 
 * 
 * the idea is: fat-file is nothing but a cluster chain, any open fat-file is 
 * represented in system by fat-file descriptor and has well-known 
 * file interface:
 *
 * fat_file_open()
 * fat_file_close()
 * fat_file_read()
 * fat_file_write()
 *
 * Such interface hides the architecture of fat-file and represents it like 
 * linear file
 */
  
typedef rtems_filesystem_node_types_t fat_file_type_t;

#define FAT_DIRECTORY     RTEMS_FILESYSTEM_DIRECTORY
#define FAT_FILE          RTEMS_FILESYSTEM_MEMORY_FILE

typedef struct fat_file_map_s
{
    unsigned32 file_cln;
    unsigned32 disk_cln;
    unsigned32 last_cln;
} fat_file_map_t;
/* 
 * descriptor of a fat-file 
 *
 * To each particular clusters chain 
 */
typedef struct fat_file_fd_s
{
    Chain_Node      link;          /*  
                                    * fat-file descriptors organized into hash;
                                    * collision lists are handled via link 
                                    * field
                                    */
    unsigned32      links_num;     /* 
                                    * the number of fat_file_open call on 
                                    * this fat-file
                                    */
    unsigned32      ino;           /* inode, file serial number :)))) */                                
    fat_file_type_t fat_file_type;
    unsigned32      size_limit;
    unsigned32      fat_file_size; /* length  */
    unsigned32      info_cln;
    unsigned32      cln;
    unsigned16      info_ofs;     
    unsigned char   first_char;
    unsigned8       flags;
    fat_file_map_t  map;
    time_t          mtime;
    
} fat_file_fd_t;


#define FAT_FILE_REMOVED  0x01

#define FAT_FILE_IS_REMOVED(p)\
    (((p)->flags & FAT_FILE_REMOVED) ? 1 : 0)

/* ioctl macros */
#define F_CLU_NUM  0x01

/* 
 * Each file and directory on a MSDOS volume is unique identified by it 
 * location, i.e. location of it 32 Bytes Directory Entry Structure. We can 
 * distinguish them by cluster number it locates on and offset inside this 
 * cluster. But root directory on any volumes (FAT12/16/32) has no 32 Bytes
 * Directory Entry Structure corresponded to it. So we assume 32 Bytes
 * Directory Entry Structure of root directory locates at cluster 1 (invalid
 * cluaster number) and offset 0
 */
#define FAT_ROOTDIR_CLUSTER_NUM 0x01 

#define FAT_FD_OF_ROOT_DIR(fat_fd)  \
  ((fat_fd->info_cln == FAT_ROOTDIR_CLUSTER_NUM ) && \
  (fat_fd->info_ofs == 0))

#define FAT_EOF           0x00

/* fat_construct_key --
 *     Construct key for hash access: convert (cluster num, offset) to
 *     (sector512 num, new offset) and than construct key as 
 *     key = (sector512 num) << 4 | (new offset)
 *
 * PARAMETERS:
 *     cl       - cluster number
 *     ofs      - offset inside cluster 'cl'
 *     mt_entry - mount table entry
 *
 * RETURNS:
 *     constructed key
 */
static inline unsigned32 
fat_construct_key(
    rtems_filesystem_mount_table_entry_t *mt_entry,
    unsigned32                            cl, 
    unsigned32                            ofs)
{
    return ( ((fat_cluster_num_to_sector512_num(mt_entry, cl) + 
              (ofs >> FAT_SECTOR512_BITS)) << 4)              + 
              ((ofs >> 5) & (FAT_DIRENTRIES_PER_SEC512 - 1)) );
}

/* Prototypes for "fat-file" operations */
int 
fat_file_open(rtems_filesystem_mount_table_entry_t  *mt_entry,
              unsigned32                             cln, 
              unsigned32                             ofs,
              fat_file_fd_t                        **fat_fd);

int
fat_file_reopen(fat_file_fd_t *fat_fd);

int 
fat_file_close(rtems_filesystem_mount_table_entry_t *mt_entry,
               fat_file_fd_t                        *fat_fd);

ssize_t
fat_file_read(rtems_filesystem_mount_table_entry_t *mt_entry,
              fat_file_fd_t                        *fat_fd,
              unsigned32                            start,
              unsigned32                            count,
              char                                 *buf);

ssize_t
fat_file_write(rtems_filesystem_mount_table_entry_t *mt_entry,
               fat_file_fd_t                        *fat_fd,
               unsigned32                            start,
               unsigned32                            count,
               const char                            *buf);

int
fat_file_extend(rtems_filesystem_mount_table_entry_t *mt_entry,
                fat_file_fd_t                        *fat_fd,
                unsigned32                            new_length,
                unsigned32                           *a_length);

int
fat_file_truncate(rtems_filesystem_mount_table_entry_t *mt_entry,
                  fat_file_fd_t                        *fat_fd,
                  unsigned32                            new_length);
                  
int
fat_file_datasync(rtems_filesystem_mount_table_entry_t *mt_entry,
                  fat_file_fd_t                        *fat_fd);
                  

int
fat_file_ioctl(rtems_filesystem_mount_table_entry_t *mt_entry,
               fat_file_fd_t                        *fat_fd,
               int                                   cmd,
               ...);

int
fat_file_size(rtems_filesystem_mount_table_entry_t *mt_entry,
              fat_file_fd_t                        *fat_fd);

void
fat_file_mark_removed(rtems_filesystem_mount_table_entry_t *mt_entry,
                      fat_file_fd_t                        *fat_fd);

#ifdef __cplusplus
}
#endif

#endif /* __DOSFS_FAT_FILE_H__ */

/*
 * Routine to create a new MSDOS filesystem node
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.OARcorp.com/rtems/license.html.
 *
 * @(#) $Id$
 *
 */
#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <rtems/libio_.h>
#include <time.h>

#include "fat.h"
#include "fat_fat_operations.h"
#include "fat_file.h"

#include "msdos.h"

/* msdos_creat_node --
 *     Create a new node. If a new node is file, FAT 32 Bytes Directory 
 *     Entry Structure (see M$ White Paper) is initialized, free space is 
 *     found in parent directory and structure is written to the disk.
 *     In case of directory, all above steps present and also new cluster
 *     is allocated for a new directory and dot and dotdot nodes are created 
 *     in alloceted cluster.
 *
 * PARAMETERS:
 *     parent_loc - parent (directory we are going to create node in)
 *     type       - new node type (file or directory)
 *     name       - new node name
 *     mode       - mode
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured (errno set appropriately).
 *     
 */
int 
msdos_creat_node(
    rtems_filesystem_location_info_t  *parent_loc,
    msdos_node_type_t                  type,
    char                              *name,
    mode_t                             mode
    ) 
{
    int              rc = RC_OK;
    ssize_t          ret = 0;
    msdos_fs_info_t *fs_info = parent_loc->mt_entry->fs_info;
    fat_file_fd_t   *parent_fat_fd = parent_loc->node_access;
    fat_file_fd_t   *fat_fd = NULL; 
    time_t           time_ret = 0;
    unsigned16       time_val = 0;
    unsigned16       date = 0;
    fat_auxiliary_t  aux;
    unsigned char    new_node[MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE];
    unsigned char    dot_dotdot[MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE * 2];
    
    memset(new_node, 0, MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE);
    memset(dot_dotdot, 0, MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE * 2);
  
    /* set up name */
    strncpy(MSDOS_DIR_NAME(new_node), name, MSDOS_NAME_MAX);

    /* fill reserved field */
    *MSDOS_DIR_NT_RES(new_node) = MSDOS_RES_NT_VALUE;
  
    /* set up last write date and time */
    time_ret = time(NULL);
    if ( time_ret == -1 )
        return -1;

    msdos_date_unix2dos(time_ret, &time_val, &date);
    *MSDOS_DIR_WRITE_TIME(new_node) = CT_LE_W(time_val);
    *MSDOS_DIR_WRITE_DATE(new_node) = CT_LE_W(date);
  
    /* initialize directory/file size */
    *MSDOS_DIR_FILE_SIZE(new_node) = MSDOS_INIT_DIR_SIZE;

    if (type == MSDOS_DIRECTORY)
        *MSDOS_DIR_ATTR(new_node) |= MSDOS_ATTR_DIRECTORY;
    else
        *MSDOS_DIR_ATTR(new_node) |= MSDOS_ATTR_ARCHIVE;

    /* 
     * find free space in the parent directory and write new initialized 
     * FAT 32 Bytes Directory Entry Structure (see M$ White Paper) 
     * to the disk
     */
    rc = msdos_get_name_node(parent_loc, NULL, &aux, new_node);
    if ( rc != RC_OK )
        return rc;
  
    /* 
     * if we create a new file we are done, if directory there are more steps 
     * to do
     */
    if (type == MSDOS_DIRECTORY)
    {
        /* open new directory as fat-file */
        rc = fat_file_open(parent_loc->mt_entry, aux.cln, aux.ofs, &fat_fd);
        if (rc != RC_OK)
            goto err;

        /* 
         * we opened fat-file for node we just created, so initialize fat-file
         * descritor
         */
        fat_fd->info_cln = aux.cln;
        fat_fd->info_ofs = aux.ofs;
        fat_fd->fat_file_size = 0;
        fat_fd->fat_file_type = FAT_DIRECTORY;
        fat_fd->size_limit = MSDOS_MAX_DIR_LENGHT;
    
        /* 
         * dot and dotdot entries are identical to new node except the 
         * names 
         */
        memcpy(DOT_NODE_P(dot_dotdot), new_node, 
               MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE); 
        memcpy(DOTDOT_NODE_P(dot_dotdot), new_node,
               MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE); 
        memcpy(MSDOS_DIR_NAME(DOT_NODE_P(dot_dotdot)), MSDOS_DOT_NAME, 
               MSDOS_NAME_MAX);
        memcpy(MSDOS_DIR_NAME(DOTDOT_NODE_P(dot_dotdot)), MSDOS_DOTDOT_NAME, 
               MSDOS_NAME_MAX);
           
        /* set up cluster num for dotdot entry */
        /* 
         * here we can ommit FAT32 condition because for all FAT types dirs 
         * right under root dir should contain 0 in dotdot entry but for 
         * FAT12/16 parent_fat_fd->cluster_num always contains such value
         */    
        if ((FAT_FD_OF_ROOT_DIR(parent_fat_fd)) && 
            (fs_info->fat.vol.type & FAT_FAT32))
        {
            *MSDOS_DIR_FIRST_CLUSTER_LOW(DOTDOT_NODE_P(dot_dotdot)) = 0x0000;
            *MSDOS_DIR_FIRST_CLUSTER_HI(DOTDOT_NODE_P(dot_dotdot)) = 0x0000;
        }
        else
        {
            *MSDOS_DIR_FIRST_CLUSTER_LOW(DOTDOT_NODE_P(dot_dotdot)) = 
                CT_LE_W((unsigned16)((parent_fat_fd->cln) & 0x0000FFFF));
            *MSDOS_DIR_FIRST_CLUSTER_HI(DOTDOT_NODE_P(dot_dotdot)) = 
                CT_LE_W((unsigned16)(((parent_fat_fd->cln) & 0xFFFF0000)>>16));
        }        
    
        /* 
         * write dot and dotdot entries to new fat-file: currently fat-file 
         * correspondes to a new node is zero length, so it will be extended 
         * by one cluster and entries will be written
         */
        ret = fat_file_write(parent_loc->mt_entry, fat_fd, 0, 
                             MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE * 2, 
                             dot_dotdot);
        if (ret < 0)
        {
            rc = -1;
            goto error;
        }
    
        /* increment fat-file size by cluster size */
        fat_fd->fat_file_size += fs_info->fat.vol.bpc;

        /* set up cluster num for dot entry */
        *MSDOS_DIR_FIRST_CLUSTER_LOW(DOT_NODE_P(dot_dotdot)) =
                CT_LE_W((unsigned16)((fat_fd->cln) & 0x0000FFFF));
        *MSDOS_DIR_FIRST_CLUSTER_HI(DOT_NODE_P(dot_dotdot)) =
                CT_LE_W((unsigned16)(((fat_fd->cln) & 0xFFFF0000) >> 16));

        /* rewrite dot entry */
        ret = fat_file_write(parent_loc->mt_entry, fat_fd, 0, 
                             MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE,
                             DOT_NODE_P(dot_dotdot));
        if (ret < 0)
        {
            rc = -1;
            goto error;
        }
    
        /* write first cluster num of a new directory to disk */
        rc = msdos_set_first_cluster_num(parent_loc->mt_entry, fat_fd);    
        if (rc != RC_OK)
            goto error;          
  
        fat_file_close(parent_loc->mt_entry, fat_fd);
    }
    return RC_OK;

error:
    fat_file_close(parent_loc->mt_entry, fat_fd);

err:
    /* mark 32bytes structure on the disk as free */
    msdos_set_first_char4file_name(parent_loc->mt_entry, aux.cln, aux.ofs, 
                                   0xE5);
    return rc;
}

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
 *     Entry Structure is initialized, free space is found in parent 
 *     directory and structure is written to the disk. In case of directory, 
 *     all above steps present and also new cluster is allocated for a 
 *     new directory and dot and dotdot nodes are created in alloceted cluster.
 *
 * PARAMETERS:
 *     parent_loc - parent (directory we are going to create node in)
 *     type       - new node type (file or directory)
 *     name       - new node name
 *     mode       - mode
 *     link_info  - fs_info of existing node for a pseudo "hard-link"
 *                  (see msdos_file.c, msdos_link for documentation)
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
    mode_t                             mode,
    const fat_file_fd_t               *link_fd
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
    unsigned char    new_node  [MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE];
    unsigned char    dot_dotdot[MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE * 2];
    unsigned char    link_node [MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE];
    unsigned32       sec = 0;
    unsigned32       byte = 0;
    
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

    if (type == MSDOS_DIRECTORY) {
        *MSDOS_DIR_ATTR(new_node) |= MSDOS_ATTR_DIRECTORY;
    }
    else if (type == MSDOS_HARD_LINK) {
      /*
       * when we establish a (temporary) hard link,
       * we must copy some information from the original 
       * node to the newly created
       */      
      /*
       * read the original directory entry
       */
      sec = fat_cluster_num_to_sector_num(parent_loc->mt_entry, 
					  link_fd->info_cln);
      sec += (link_fd->info_ofs >> fs_info->fat.vol.sec_log2);
      byte = (link_fd->info_ofs & (fs_info->fat.vol.bps - 1));
	  
      ret = _fat_block_read(parent_loc->mt_entry, 
			    sec, byte, MSDOS_DIRECTORY_ENTRY_STRUCT_SIZE,
                            link_node);
      if (ret < 0) {
	return -1;
      } 
      /*
       * copy various attributes
       */
      *MSDOS_DIR_ATTR(new_node)          =*MSDOS_DIR_ATTR(link_node);
      *MSDOS_DIR_CRT_TIME_TENTH(new_node)=*MSDOS_DIR_CRT_TIME_TENTH(link_node);
      *MSDOS_DIR_CRT_TIME(new_node)      =*MSDOS_DIR_CRT_TIME(link_node);
      *MSDOS_DIR_CRT_DATE(new_node)      =*MSDOS_DIR_CRT_DATE(link_node);

      /* 
       * copy/set "file size", "first cluster"
       */
      *MSDOS_DIR_FILE_SIZE(new_node)     =*MSDOS_DIR_FILE_SIZE(link_node);

      *MSDOS_DIR_FIRST_CLUSTER_LOW(new_node) = 
	*MSDOS_DIR_FIRST_CLUSTER_LOW(link_node);
      *MSDOS_DIR_FIRST_CLUSTER_HI(new_node) = 
	*MSDOS_DIR_FIRST_CLUSTER_HI(link_node);
      /*
       * set "archive bit" due to changes
       */
      *MSDOS_DIR_ATTR(new_node) |= MSDOS_ATTR_ARCHIVE;
      /*
       * set "last access" date to today
       */
      *MSDOS_DIR_LAST_ACCESS_DATE(new_node) = CT_LE_W(date);
    }
    else { /* regular file... */
        *MSDOS_DIR_ATTR(new_node) |= MSDOS_ATTR_ARCHIVE;
    }

    /* 
     * find free space in the parent directory and write new initialized 
     * FAT 32 Bytes Directory Entry Structure to the disk
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

/* msdos_file_link --
 *     Replacement for a file "link" operation.
 *     MSDOS FAT FS does not support links, but this call is needed to
 *     allow "rename" operations. The current NEWLIB rename performs a link
 *     from the old to the new name and then deletes the old filename.
 * 
 *     This pseudo-"link" operation will create a new directory entry,
 *     copy the file size and cluster information from the "old"
 *     to the "new" directory entry and then clear the file size and cluster
 *     info from the "old" filename, leaving this file as 
 *     a valid, but empty entry.
 * 
 *     When this "link" call is part of a "rename" sequence, the "old"
 *     entry will be deleted in a subsequent "rmnod" call
 *
 *     This function has been implemented by Thomas Doerfler,
 *     <Thomas.Doerfler@imd-systems.de>
 *
 * PARAMETERS:
 *     to_loc     - node description for "existing" node
 *     par_loc    - node description for "new" node
 *     token      - name of new node
 *
 * RETURNS:
 *     RC_OK on success, or -1 if error occured (errno set appropriately)
 */
int 
msdos_file_link(rtems_filesystem_location_info_t *to_loc,
		rtems_filesystem_location_info_t *par_loc,
		const char                       *token
)
{
    int                rc = RC_OK;
    rtems_status_code  sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t   *fs_info     = to_loc->mt_entry->fs_info;
    fat_file_fd_t     *to_fat_fd   = to_loc->node_access;
    fat_file_fd_t     *par_fat_fd  = par_loc->node_access;
    char                 new_name[ MSDOS_NAME_MAX + 1 ];
    int                  len;

    /* 
     * check spelling and format new node name 
     */ 
    if (MSDOS_NAME != msdos_get_token(token, new_name, &len)) {
      set_errno_and_return_minus_one(ENAMETOOLONG);      
    }
    /*
     * verify, that the existing node can be linked to
     * check that nodes are in same FS/volume?
     */
    if (to_loc->mt_entry->fs_info != par_loc->mt_entry->fs_info) {
      set_errno_and_return_minus_one(EXDEV);
    }
    /*
     * lock volume
     */
    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        set_errno_and_return_minus_one(EIO);


    /*
     * create new directory entry as "hard link",
     * copying relevant info from existing file
     */
    rc = msdos_creat_node(par_loc,MSDOS_HARD_LINK,new_name,S_IFREG,
			  to_loc->node_access);
    /*
     * set file size and first cluster number of old entry to 0
     */
    if (rc == RC_OK) {
      to_fat_fd->fat_file_size = 0;
      to_fat_fd->cln           = FAT_EOF;
      rc = msdos_set_first_cluster_num(to_loc->mt_entry, to_fat_fd);
      if (rc == RC_OK) {
	rc = msdos_set_file_size(par_loc->mt_entry, to_fat_fd);
      }
    }
    /*
     * FIXME: check error/abort handling
     */
    rtems_semaphore_release(fs_info->vol_sema);
    return rc;
}


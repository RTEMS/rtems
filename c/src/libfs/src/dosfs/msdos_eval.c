/*
 *  MSDOS evaluation routines
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>

#include <rtems/libio_.h>

#include "fat.h"
#include "fat_fat_operations.h"
#include "fat_file.h"

#include "msdos.h"

/* msdos_set_handlers --
 *     Set handlers for the node with specified type(i.e. handlers for file 
 *     or directory).
 *
 * PARAMETERS:
 *     loc - node description
 *
 * RETURNS:
 *     None
 */
static void 
msdos_set_handlers(rtems_filesystem_location_info_t *loc)
{
    msdos_fs_info_t *fs_info = loc->mt_entry->fs_info; 
    fat_file_fd_t   *fat_fd = loc->node_access;

    if (fat_fd->fat_file_type == FAT_DIRECTORY)
        loc->handlers = fs_info->directory_handlers;
    else
        loc->handlers = fs_info->file_handlers;
}

/* msdos_eval_path --
 *
 *     The following routine evaluate path for a node that wishes to be
 *     accessed.  Structure 'pathloc' is returned with a pointer to the
 *     node to be accessed.
 *
 * PARAMETERS:
 *     pathname - path for evaluation
 *     flags    - flags
 *     pathloc  - node description (IN/OUT)
 *
 * RETURNS:
 *     RC_OK and filled pathloc on success, or -1 if error occured
 *     (errno set appropriately)
 *       
 */
int 
msdos_eval_path(
    const char                        *pathname,
    int                                flags,
    rtems_filesystem_location_info_t  *pathloc 
    )
{
    int                               rc = RC_OK;
    rtems_status_code                 sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t                  *fs_info = pathloc->mt_entry->fs_info;
    fat_file_fd_t                    *fat_fd = NULL;
    rtems_filesystem_location_info_t  newloc;
    int                               i = 0;
    int                               len = 0;
    msdos_token_types_t               type = MSDOS_CURRENT_DIR;
    char                              token[MSDOS_NAME_MAX + 1];

    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        set_errno_and_return_minus_one(EIO);
  
    if (!pathloc->node_access)
    {
        errno = ENOENT;
        rc = -1;
        goto err;
    }  

    fat_fd = pathloc->node_access; 

    rc = fat_file_reopen(fat_fd);
    if (rc != RC_OK)
        goto err;
    
    while ((type != MSDOS_NO_MORE_PATH) && (type != MSDOS_INVALID_TOKEN)) 
    {
        type = msdos_get_token(&pathname[i], token, &len);
        i += len;
  
        fat_fd = pathloc->node_access; 
  
        switch (type) 
        {
            case MSDOS_UP_DIR:
                /*
                 *  Only a directory can be decended into.
                 */
                if (fat_fd->fat_file_type != FAT_DIRECTORY)
                {
                    errno = ENOTDIR;
                    rc = -1;
                    goto error;
                }  

                /*
                 *  Am I at the root of this mounted filesystem?
                 */
                if (pathloc->node_access == 
                    pathloc->mt_entry->mt_fs_root.node_access) 
                {
                    /*
                     *  Am I at the root of all filesystems?
                     *  XXX: MSDOS is not supposed to be base fs.
                     */
                    if (pathloc->node_access == 
                        rtems_filesystem_root.node_access) 
                    {    
                        break;       /* Throw out the .. in this case */
                    }     
                    else 
                    {
                        newloc = pathloc->mt_entry->mt_point_node;
                        *pathloc = newloc;

                        rc = fat_file_close(pathloc->mt_entry, fat_fd);
                        if (rc != RC_OK)
                            goto err;
                            
                        rtems_semaphore_release(fs_info->vol_sema);
                        return (*pathloc->ops->evalpath_h)(&(pathname[i-len]),
                                                           flags, pathloc);
                    }
                } 
                else 
                {
                    rc = msdos_find_name(pathloc, token);
                    if (rc != RC_OK)
                    {
                        if (rc == MSDOS_NAME_NOT_FOUND_ERR)
                        {
                            errno = ENOENT;
                            rc = -1;
                        }    
                        goto error; 
                    }  
                }
                break;

            case MSDOS_NAME:
                /*
                 *  Only a directory can be decended into.
                 */
                if (fat_fd->fat_file_type != FAT_DIRECTORY)
                {
                    errno = ENOTDIR;
                    rc = -1;
                    goto error;
                }  

                /*
                 *  Otherwise find the token name in the present location and 
                 * set the node access to the point we have found.
                 */
                rc = msdos_find_name(pathloc, token);
                if (rc != RC_OK)
                {
                    if (rc == MSDOS_NAME_NOT_FOUND_ERR)
                    {
                        errno = ENOENT;
                        rc = -1;
                    }    
                    goto error; 
                }  
                break;

            case MSDOS_NO_MORE_PATH:
            case MSDOS_CURRENT_DIR:       
                break;

            case MSDOS_INVALID_TOKEN:
                errno = ENAMETOOLONG;
                rc = -1;
                goto error;
                break;
 
        }
    }

    /*
     *  Always return the root node.
     *
     *  If we are at a node that is a mount point. Set loc to the
     *  new fs root node and let let the mounted filesystem set the handlers.
     *
     *  NOTE: The behavior of stat() on a mount point appears to be 
     *        questionable.
     *  NOTE: MSDOS filesystem currently doesn't support mount functionality ->
     *        action not implemented
     */
    fat_fd = pathloc->node_access;
   
    msdos_set_handlers(pathloc);
   
    rtems_semaphore_release(fs_info->vol_sema);
    return RC_OK;

error:
    fat_file_close(pathloc->mt_entry, fat_fd);

err:
    rtems_semaphore_release(fs_info->vol_sema);
    return rc; 
}

/* msdos_eval4make --
 *     The following routine evaluate path for a new node to be created.
 *     'pathloc' is returned with a pointer to the parent of the new node.
 *     'name' is returned with a pointer to the first character in the
 *     new node name.  The parent node is verified to be a directory.
 *
 * PARAMETERS:
 *     path    - path for evaluation
 *     pathloc - IN/OUT (start point for evaluation/parent directory for 
 *               creation)
 *     name    - new node name
 *
 * RETURNS:
 *     RC_OK, filled pathloc for parent directory and name of new node on 
 *     success, or -1 if error occured (errno set appropriately)
 */
int 
msdos_eval4make(
    const char                         *path, 
    rtems_filesystem_location_info_t   *pathloc,  
    const char                        **name
    )   
{
    int                               rc = RC_OK;
    rtems_status_code                 sc = RTEMS_SUCCESSFUL;
    msdos_fs_info_t                  *fs_info = pathloc->mt_entry->fs_info;
    fat_file_fd_t                    *fat_fd = NULL;
    rtems_filesystem_location_info_t  newloc;
    msdos_token_types_t               type;
    int                               i = 0;
    int                               len;
    char                              token[ MSDOS_NAME_MAX + 1 ];
    rtems_boolean                     done = 0;

    sc = rtems_semaphore_obtain(fs_info->vol_sema, RTEMS_WAIT,
                                MSDOS_VOLUME_SEMAPHORE_TIMEOUT);
    if (sc != RTEMS_SUCCESSFUL)
        set_errno_and_return_minus_one(EIO);
  
    if (!pathloc->node_access)
    {
        errno = ENOENT;
        rc = -1;
        goto err;
    }  

    fat_fd = pathloc->node_access; 

    rc = fat_file_reopen(fat_fd);
    if (rc != RC_OK)
        goto err;

    while (!done) 
    {
        type = msdos_get_token(&path[i], token, &len);
        i += len;
        fat_fd = pathloc->node_access; 

        switch (type) 
        {
            case MSDOS_UP_DIR:
                /*
                 *  Only a directory can be decended into.
                 */
                if (fat_fd->fat_file_type != FAT_DIRECTORY)
                {
                    errno = ENOTDIR;
                    rc = -1;
                    goto error;
                }  

                /*
                 *  Am I at the root of this mounted filesystem?
                 */
                if (pathloc->node_access == 
                    pathloc->mt_entry->mt_fs_root.node_access) 
                {
                    /*
                     *  Am I at the root of all filesystems?
                     *  XXX: MSDOS is not supposed to be base fs.
                     */
                    if (pathloc->node_access == 
                        rtems_filesystem_root.node_access) 
                    {    
                        break;       /* Throw out the .. in this case */
                    }    
                    else 
                    {
                        newloc = pathloc->mt_entry->mt_point_node;
                        *pathloc = newloc;

                        rc = fat_file_close(pathloc->mt_entry, fat_fd);
                        if (rc != RC_OK)
                            goto err;

                        rtems_semaphore_release(fs_info->vol_sema);
                        return (*pathloc->ops->evalformake_h)(&path[i-len], 
                                                              pathloc, name);
                    }
                } 
                else 
                {
                    rc = msdos_find_name(pathloc, token);
                    if (rc != RC_OK)
                    {
                        if (rc == MSDOS_NAME_NOT_FOUND_ERR)
                        {
                            errno = ENOENT;
                            rc = -1;
                        }    
                        goto error; 
                    }  
                }
                break;

            case MSDOS_NAME:
                /*
                 *  Only a directory can be decended into.
                 */
                if (fat_fd->fat_file_type != FAT_DIRECTORY)
                {
                    errno = ENOTDIR;
                    rc = -1;
                    goto error;
                }  

                /*
                 *  Otherwise find the token name in the present location and 
                 * set the node access to the point we have found.
                 */
                rc = msdos_find_name(pathloc, token);
                if (rc)
                {
                    if (rc != MSDOS_NAME_NOT_FOUND_ERR)
                    {
                        errno = ENOENT;
                        rc = -1;
                        goto error;
                    }
                    else
                        done = TRUE;
                }     
                break;

            case MSDOS_NO_MORE_PATH:
                errno = EEXIST;
                rc = -1;
                goto error;
                break;
        
            case MSDOS_CURRENT_DIR:
                break;

            case MSDOS_INVALID_TOKEN:
                errno = ENAMETOOLONG;
                rc = -1;
                goto error;
                break;

        }
    }

    *name = &path[i - len];

    /*
     * We have evaluated the path as far as we can.
     * Verify there is not any invalid stuff at the end of the name.
     */
    for( ; path[i] != '\0'; i++) 
    {
        if (!msdos_is_separator(path[i]))
        {
            errno = ENOENT;
            rc = -1;
            goto error;
        }  
    }

    fat_fd = pathloc->node_access;

    if (fat_fd->fat_file_type != FAT_DIRECTORY)
    {
        errno = ENOTDIR;
        rc = -1;
        goto error;
    }  
    
    msdos_set_handlers(pathloc);
 
    rtems_semaphore_release(fs_info->vol_sema);
    return RC_OK;

error:
    fat_file_close(pathloc->mt_entry, fat_fd);

err:
    rtems_semaphore_release(fs_info->vol_sema);
    return rc; 
}

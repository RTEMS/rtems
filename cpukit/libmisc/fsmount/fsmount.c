/*===============================================================*\
| Project: RTEMS fsmount                                          |
+-----------------------------------------------------------------+
| File: fsmount.c                                                 |
+-----------------------------------------------------------------+
|                    Copyright (c) 2003 IMD                       |
|      Ingenieurbuero fuer Microcomputertechnik Th. Doerfler      |
|               <Thomas.Doerfler@imd-systems.de>                  |
|                       all rights reserved                       |
+-----------------------------------------------------------------+
| this file contains the fsmount functions. These functions       |
| are used to mount a list of filesystems (and create their mount |
| points before)                                                  |
|                                                                 |
|  The license and distribution terms for this file may be        |
|  found in the file LICENSE in this distribution or at           |
|  http://www.rtems.com/license/LICENSE.                     |
|                                                                 |
+-----------------------------------------------------------------+
|   date                      history                        ID   |
| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |
| 02.07.03  creation                                         doe  |
\*===============================================================*/

#ifndef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/fsmount.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <rtems/imfs.h>
#include <sys/stat.h>

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int rtems_fsmount_create_mountpoint
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|  This function will create the mount point given                          |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 const char *mount_point
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    0, if success, -1 and errno if failed                                  |
\*=========================================================================*/
{
  int rc = 0;
  char *tok_buffer = NULL;
  char *token = NULL;
  int    token_len;
  size_t total_len;
  IMFS_token_types token_type;
  struct stat file_info;
  /*
   * allocate temp memory to rebuild path name
   */
  tok_buffer = calloc(strlen(mount_point)+1,sizeof(char));
  token = tok_buffer;
  total_len = 0;
  do {
    /*
     * scan through given string, one segment at a time
     */
    token_type = IMFS_get_token(mount_point+total_len,token,&token_len);
    total_len += token_len;
    strncpy(tok_buffer,mount_point,total_len);
    tok_buffer[total_len] = '\0';

    if ((token_type != IMFS_NO_MORE_PATH) &&
	(token_type != IMFS_CURRENT_DIR)  &&
	(token_type != IMFS_INVALID_TOKEN)) {
      /*
       * check, whether segment exists
       */
      if (0 != stat(tok_buffer,&file_info)) {
	/*
	 * if not, create directory
	 */
	rc = mknod(tok_buffer,S_IRWXU | S_IRWXG | S_IRWXO | S_IFDIR,0);
      }
    }
  } while ((rc == 0) &&
	   (token_type != IMFS_NO_MORE_PATH) &&
	   (token_type != IMFS_INVALID_TOKEN));

  /*
   * return token buffer to heap
   */
  if (tok_buffer != NULL) {
    free(tok_buffer);
  }
  return rc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int rtems_fsmount
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|  This function will create the mount points listed and mount the file     |
|   systems listed in the calling parameters                                |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 const fstab_t *fstab_ptr,
 int fstab_count,
 int *fail_idx
 )
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    0, if success, -1 and errno if failed                                  |
\*=========================================================================*/
{
  int rc = 0;
  int tmp_rc;
  int fstab_idx = 0;
  rtems_filesystem_mount_table_entry_t *tmp_mt_entry;
  bool terminate = false;

  /*
   * scan through all fstab entries;
   */
  while (!terminate &&
	 (fstab_idx < fstab_count)) {
    tmp_rc = 0;
    /*
     * create mount point
     */
    if (tmp_rc == 0) {
      tmp_rc = rtems_fsmount_create_mountpoint(fstab_ptr->mount_point);
      if (tmp_rc != 0) {
	if (0 != (fstab_ptr->report_reasons & FSMOUNT_MNTPNT_CRTERR)) {
	  fprintf(stdout,"fsmount: creation of mount point \"%s\" failed: %s\n",
		 fstab_ptr->mount_point,
		 strerror(errno));
	}
	if (0 != (fstab_ptr->abort_reasons & FSMOUNT_MNTPNT_CRTERR)) {
	  terminate = true;
	  rc = tmp_rc;
	}
      }
    }
    /*
     * mount device to given mount point
     */
    if (tmp_rc == RTEMS_SUCCESSFUL) {
      tmp_rc = mount(&tmp_mt_entry,
		     fstab_ptr->fs_ops,
		     fstab_ptr->mount_options,
		     fstab_ptr->dev,
		     fstab_ptr->mount_point);
      if (tmp_rc != RTEMS_SUCCESSFUL) {
	if (0 != (fstab_ptr->report_reasons & FSMOUNT_MNT_FAILED)) {
	  fprintf(stdout,"fsmount: mounting of \"%s\" to"
		 " \"%s\" failed: %s\n",
		 fstab_ptr->dev,
		 fstab_ptr->mount_point,
		 strerror(errno));
	}
	if (0 != (fstab_ptr->abort_reasons & FSMOUNT_MNT_FAILED)) {
	  terminate = true;
	  rc = tmp_rc;
	}
      }
      else {
	if (0 != (fstab_ptr->report_reasons & FSMOUNT_MNT_OK)) {
	  fprintf(stdout,"fsmount: mounting of \"%s\" to"
		 " \"%s\" succeeded\n",
		 fstab_ptr->dev,
		 fstab_ptr->mount_point);
	}
	if (0 != (fstab_ptr->abort_reasons & FSMOUNT_MNT_OK)) {
	  terminate = true;
	}
      }
    }
    /*
     * proceed to next entry
     */
    if (!terminate) {
      fstab_ptr++;
      fstab_idx++;
    }
  }
  if (fail_idx != NULL) {
    *fail_idx = fstab_idx;
  }
  return rc;
}

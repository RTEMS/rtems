/**
 * @file
 *
 * File system mount functions.
 */

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
#include <rtems/libio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

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
  const rtems_fstab_entry *fstab_ptr,
  size_t fstab_count,
  size_t *fail_idx
 )
/*-------------------------------------------------------------------------*\
  | Return Value:                                                             |
  |    0, if success, -1 and errno if failed                                  |
  \*=========================================================================*/
{
  int rc = 0;
  int tmp_rc;
  size_t fstab_idx = 0;
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
      tmp_rc = rtems_mkdir(fstab_ptr->target, S_IRWXU | S_IRWXG | S_IRWXO);
      if (tmp_rc != 0) {
        if (0 != (fstab_ptr->report_reasons & FSMOUNT_MNTPNT_CRTERR)) {
          fprintf(stdout,"fsmount: creation of mount point \"%s\" failed: %s\n",
                  fstab_ptr->target,
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
    if (tmp_rc == 0) {
      tmp_rc = mount(fstab_ptr->source,
                     fstab_ptr->target,
                     fstab_ptr->type,
                     fstab_ptr->options,
                     NULL);
      if (tmp_rc != 0) {
        if (0 != (fstab_ptr->report_reasons & FSMOUNT_MNT_FAILED)) {
          fprintf(stdout,"fsmount: mounting of \"%s\" to"
                  " \"%s\" failed: %s\n",
                  fstab_ptr->source,
                  fstab_ptr->target,
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
                  fstab_ptr->source,
                  fstab_ptr->target);
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

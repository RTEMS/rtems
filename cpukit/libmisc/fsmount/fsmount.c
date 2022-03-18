/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup rtems_fstab File System Mount Support
 *
 * @brief File System Mount Functions
 *
 * This file contains the fsmount functions. These functions
 * are used to mount a list of filesystems (and create their mount
 * points before).
 */

/*
 * Copyright (c) 2003 IMD Ingenieurbuero fuer Microcomputertechnik
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

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

/*===============================================================*\
| Project: RTEMS fsmount                                          |
+-----------------------------------------------------------------+
| File: fsmount.h                                                 |
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

#ifndef _FSMOUNT_H
#define _FSMOUNT_H

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * bits to define, what errors will cause reporting (via printf) and
 * abort of mount processing
 * Use a combination of these bits
 * for the fields "report_reasons" and "abort_reasons"
 */
#define FSMOUNT_MNT_OK        0x0001 /* mounted ok                 */
#define FSMOUNT_MNTPNT_CRTERR 0x0002 /* cannot create mount point  */
#define FSMOUNT_MNT_FAILED    0x0004 /* mounting failed            */

typedef struct {
  char *dev;
  char *mount_point;
  rtems_filesystem_operations_table *fs_ops;
  rtems_filesystem_options_t mount_options;
  uint16_t   report_reasons;
  uint16_t   abort_reasons;
} fstab_t;


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
 const fstab_t *fstab_ptr,              /* Ptr to filesystem mount table   */
 int fstab_count,                       /* number of entries in mount table*/
 int *fail_idx                          /* return: index of failed entry   */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    0, if success, -1 and errno if failed                                  |
\*=========================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _FSMOUNT_H */

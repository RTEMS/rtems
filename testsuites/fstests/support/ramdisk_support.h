/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#ifndef __RAMDISK_SUPPORT_H
#define __RAMDISK_SUPPORT_H

#define  RAMDISK_BLOCK_SIZE  (512)
#define  RAMDISK_BLOCK_COUNT (1024)
#define  RAMDISK_PATH "/dev/ramdisk"
#ifdef __cplusplus
extern "C" {
#endif
extern void  init_ramdisk(void);
extern void  del_ramdisk(void );
#ifdef __cplusplus
};
#endif

#endif

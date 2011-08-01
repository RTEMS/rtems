/*
 *
 *  $Id$
 */
#ifndef __RAMDISK_SUPPORT_H
#define __RAMDISK_SUPPORT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif 

#include "fstest_support.h"

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

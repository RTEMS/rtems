/*
 * ata.h
 *
 * ATA RTEMS driver header file. This file should be included from an 
 * application.
 *
 * Copyright (C) 2002 OKTET Ltd., St.-Petersburg, Russia
 * Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.OARcorp.com/rtems/license.html.
 * 
 * $Id$
 */
#ifndef __ATA_H__
#define __ATA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <sys/ioctl.h>

#include <rtems/blkdev.h>

rtems_device_driver ata_initialize(
        rtems_device_major_number major,
        rtems_device_minor_number minor,
        void *args);
        
#define ATA_DRIVER_TABLE_ENTRY \
    {ata_initialize, GENERIC_BLOCK_DEVICE_DRIVER_ENTRIES}
    
/* ATA IOCTL request codes */
#define ATAIO_SET_MULTIPLE_MODE          _IO('A', 1)

/* 
 * ATA driver configuration parameters 
 * FIXME: should be configured more easy...
 */
#define ATA_DRIVER_MESSAGE_QUEUE_SIZE    50
#define ATA_DRIVER_TASK_PRIORITY         140
#define ATA_DRIVER_TASK_STACK_SIZE       16*1024 

#ifdef __cplusplus
}
#endif


#endif /* __ATA_H__ */




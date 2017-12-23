/*
 * ide_ctrl.h
 *
 * This file describes the IDE controller driver for all boards.
 *
 * Copyright (C) 2002 OKTET Ltd., St.-Petersburg, Russia
 * Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */
#ifndef __IDE_CTRL_H__
#define __IDE_CTRL_H__

#ifdef __cplusplus
extern "C" {
#endif

rtems_device_driver ide_controller_initialize(
        rtems_device_major_number major,
        rtems_device_minor_number minor,
        void *args);


#define IDE_CONTROLLER_DRIVER_TABLE_ENTRY \
    {ide_controller_initialize, NULL, NULL, NULL, NULL, NULL}


#ifdef __cplusplus
}
#endif


#endif /* __IDE_CTRL_H__ */

/*
 * ide_ctrl_cfg.h
 *
 * LibChip library IDE controller header file - structuers used for 
 * configuration and plugin interface definition.
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
#ifndef __IDE_CTRL_CFG_H__
#define __IDE_CTRL_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/blkdev.h>

/*
 * Avaible drivers for IDE controllers
 */
typedef enum {
    IDE_STD,
    IDE_CUSTOM                /* BSP specific driver */
} ide_ctrl_devs_t;

/*
 * Each driver for a particular controller have to provide following
 * functions in such a structure. The only field which should not be NULL
 * is contInit. 
 */
typedef struct ide_ctrl_fns_s {
    boolean           (*ctrl_probe)(int minor); /* probe routine */
    void              (*ctrl_initialize)(int minor);
    int               (*ctrl_control)(int minor, unsigned32 command, 
                                      void *arg);
    /*
     * Functions which allow read/write registers of a particular controller.
     * (these functions may be used from ide_controller_read_register, 
     * ide_controller_write_register)
     */
    void    (*ctrl_reg_read)(int minor, int regist, unsigned16 *value);
    void    (*ctrl_reg_write)(int minor, int regist, unsigned16 value);
    
    /* 
     * The function allows to escape overhead for read/write register 
     * functions calls
     */
    void  (*ctrl_read_block)(int minor, unsigned16 block_size, 
                             blkdev_sg_buffer *bufs, rtems_unsigned32 *cbuf,
                             rtems_unsigned32 *pos);
    void  (*ctrl_write_block)(int minor, unsigned16 block_size,
                              blkdev_sg_buffer *bufs, rtems_unsigned32 *cbuf,
                              rtems_unsigned32 *pos);
    
    rtems_status_code (*ctrl_config_io_speed)(int minor, 
                                              unsigned8 modes_available);
} ide_ctrl_fns_t;

/* 
 * IDE Controller configuration. Table of such configurations is provided 
 * by BSP
 */
typedef struct ide_controller_bsp_table_s {
    char                *name;  /* device name */
    ide_ctrl_devs_t      type;  /* chip type */
    ide_ctrl_fns_t      *fns;   /* pointer to the set of driver routines */
    boolean              (*probe)(int minor); /* general probe routine */
    unsigned8            status; /* initialized/non initialized. Should be set 
                                  * to zero by static initialization  
                                  */
    unsigned32           port1; /* port number for the port of the device */
    rtems_boolean        int_driven; /* interrupt/poll driven */
    rtems_vector_number  int_vec; /* the interrupt vector of the device */
    void                *params;  /* contains either device specific data or a 
                                   * pointer to s device specific information 
                                   * table
                                   */
} ide_controller_bsp_table_t;

/* IDE controllers Table */
extern ide_controller_bsp_table_t    IDE_Controller_Table[];

/* Number of rows in IDE_Controller_Table */
extern unsigned long                 IDE_Controller_Count;


#define IDE_CTRL_MAX_MINOR_NUMBER   4

#define IDE_CTRL_NON_INITIALIZED    0
#define IDE_CTRL_INITIALIZED        1

#ifdef __cplusplus
}
#endif


#endif /* __IDE_CTRL_CFG_H__ */




/*
 * ide_ctrl_cfg.h
 *
 * LibChip library IDE controller header file - structures used for
 * configuration and plugin interface definition.
 *
 * Copyright (C) 2002 OKTET Ltd., St.-Petersburg, Russia
 * Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */
#ifndef __IDE_CTRL_CFG_H__
#define __IDE_CTRL_CFG_H__

#include <rtems/blkdev.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Avaible drivers for IDE controllers
 */
typedef enum {
    IDE_STD,
    IDE_CUSTOM                /* BSP specific driver */
} ide_ctrl_devs_t;

/* ATA modes: bit masks used in ctrl_config_io_speed call */
#define ATA_MODES_PIO3    0x001
#define ATA_MODES_PIO4    0x002

#define ATA_MODES_PIO     0x003

#define ATA_MODES_DMA0    0x004
#define ATA_MODES_DMA1    0x008
#define ATA_MODES_DMA2    0x010

#define ATA_MODES_UDMA0   0x020
#define ATA_MODES_UDMA1   0x040
#define ATA_MODES_UDMA2   0x080
#define ATA_MODES_UDMA3   0x100
#define ATA_MODES_UDMA4   0x200
#define ATA_MODES_UDMA5   0x400

#define ATA_MODES_UDMA    0x7e0
#define ATA_MODES_DMA     0x7fc


/*
 * Each driver for a particular controller have to provide following
 * functions in such a structure. The only field which should not be NULL
 * is contInit.
 */
typedef struct ide_ctrl_fns_s {
    bool              (*ctrl_probe)(int minor); /* probe routine */
    void              (*ctrl_initialize)(int minor);
    int               (*ctrl_control)(int minor, uint32_t   command,
                                      void *arg);
    /*
     * Functions which allow read/write registers of a particular controller.
     * (these functions may be used from ide_controller_read_register,
     * ide_controller_write_register)
     */
    void    (*ctrl_reg_read)(int minor, int regist, uint16_t   *value);
    void    (*ctrl_reg_write)(int minor, int regist, uint16_t   value);

    /*
     * The function allows to escape overhead for read/write register
     * functions calls
     */
    void  (*ctrl_read_block)(int minor, uint32_t   block_size,
                             rtems_blkdev_sg_buffer *bufs, uint32_t   *cbuf,
                             uint32_t   *pos);
    void  (*ctrl_write_block)(int minor, uint32_t   block_size,
                              rtems_blkdev_sg_buffer *bufs, uint32_t   *cbuf,
                              uint32_t   *pos);

    rtems_status_code (*ctrl_config_io_speed)(int minor,
                                              uint16_t modes_available);
} ide_ctrl_fns_t;

/*
 * IDE Controller configuration. Table of such configurations is provided
 * by BSP
 */
typedef struct ide_controller_bsp_table_s {
    char                *name;  /* device name */
    ide_ctrl_devs_t      type;  /* chip type */
    ide_ctrl_fns_t      *fns;   /* pointer to the set of driver routines */
    bool                 (*probe)(int minor); /* general probe routine */
    uint8_t              status; /* initialized/non initialized. Should be set
                                  * to zero by static initialization
                                  */
    uint32_t             port1; /* port number for the port of the device */
    bool                 int_driven; /* interrupt/poll driven */
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

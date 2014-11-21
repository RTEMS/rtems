/*
 * ata_internal.h
 *
 * ATA RTEMS driver internal header file
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Authors: Eugeny S. Mints     <Eugeny.Mints@oktet.ru>
 *          Alexandra Kossovsky <sasha@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 *
 */
#ifndef __ATA_INTERNAL_H__
#define __ATA_INTERNAL_H__

#include <sys/param.h>
#include <rtems.h>
#include <sys/types.h>
#include <rtems/libio.h>
#include <stdlib.h>

#include <rtems/blkdev.h>
#include <rtems/diskdevs.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Conversion from and to little-endian byte order. (no-op on i386/i486)
 *
 * Naming: Ca_b_c, where a: F = from, T = to, b: LE = little-endian,
 * BE = big-endian, c: W = word (16 bits), L = longword (32 bits)
 */
#if (CPU_BIG_ENDIAN == TRUE)
#    define CF_LE_W(v) CPU_swap_u16(v)
#    define CF_LE_L(v) CPU_swap_u32(v)
#    define CT_LE_W(v) CPU_swap_u16(v)
#    define CT_LE_L(v) CPU_swap_u32(v)
#else
#    define CF_LE_W(v) (v)
#    define CF_LE_L(v) (v)
#    define CT_LE_W(v) (v)
#    define CT_LE_L(v) (v)
#endif

#define ATA_UNDEFINED_VALUE   (-1)

/* Sector size for all ATA devices */
#define ATA_SECTOR_SIZE                 512


#define ATA_MAX_CMD_REG_OFFSET          8


/* ATA Commands */

/* Types of ATA commands */
#define ATA_COMMAND_TYPE_NON_DATA   0
#define ATA_COMMAND_TYPE_PIO_IN     1
#define ATA_COMMAND_TYPE_PIO_OUT    2
#define ATA_COMMAND_TYPE_DMA        3

/* ATA commands opcodes */
/*
 * Commands present in both ATA-2 and ATA-4 specs.
 * Some commands have two values in ATA-2,
 * in such case value from ATA-4 used.
 * Some commands have slightly different names in these specifications,
 * so names from ATA-4 are used.
 */
#define ATA_COMMAND_NOP                          0x00
#define ATA_COMMAND_READ_SECTORS                 0x20
#define ATA_COMMAND_WRITE_SECTORS                0x30
#define ATA_COMMAND_READ_VERIFY_SECTORS          0x40
#define ATA_COMMAND_SEEK                         0x70 /* or 0x7. */
#define ATA_COMMAND_EXECUTE_DEVICE_DIAGNOSTIC    0x90
#define ATA_COMMAND_INITIALIZE_DEVICE_PARAMETERS 0x91
#define ATA_COMMAND_DOWNLOAD_MICROCODE           0x92
#define ATA_COMMAND_READ_MULTIPLE                0xc4
#define ATA_COMMAND_WRITE_MULTIPLE               0xc5
#define ATA_COMMAND_SET_MULTIPLE_MODE            0xc6
#define ATA_COMMAND_READ_DMA                     0xc8
#define ATA_COMMAND_WRITE_DMA                    0xca
#define ATA_COMMAND_STANDBY_IMMEDIATE            0xe0 /* or 0x94 */
#define ATA_COMMAND_IDLE_IMMEDIATE               0xe1 /* or 0x95 */
#define ATA_COMMAND_STANDBY                      0xe2 /* or 0x96 */
#define ATA_COMMAND_IDLE                         0xe3 /* or 0x97 */
#define ATA_COMMAND_READ_BUFFER                  0xe4
#define ATA_COMMAND_CHECK_POWER_MODE             0xe5 /* or 0x98 in ATA-2 */
#define ATA_COMMAND_SLEEP                        0xe6 /* or 0x99 */
#define ATA_COMMAND_WRITE_BUFFER                 0xe8
#define ATA_COMMAND_IDENTIFY_DEVICE              0xec
#define ATA_COMMAND_SET_FEATURES                 0xef

/* Commands present in both ATA-2 and ATA-4 specs: removable media */
#define ATA_COMMAND_MEDIA_LOCK                   0xde
#define ATA_COMMAND_MEDIA_UNLOCK                 0xdf
#define ATA_COMMAND_MEDIA_EJECT                  0xed


/* Commands present in ATA-2, but not in ATA-4 (not used) */
#define ATA_COMMAND_RECALIBRATE                  0x10 /* or 0x1. */
#define ATA_COMMAND_READ_SECTOR_NON_RETRY        0x21
#define ATA_COMMAND_READ_LONG_RETRY              0x22
#define ATA_COMMAND_READ_LONG_NON_RETRY          0x23
#define ATA_COMMAND_WRITE_SECTOR_NON_RETRY       0x31
#define ATA_COMMAND_WRITE_LONG_RETRY             0x32
#define ATA_COMMAND_WRITE_LONG_NON_RETRY         0x33
#define ATA_COMMAND_WRITE_VERIFY                 0x3c
#define ATA_COMMAND_READ_VERIFY_SECTOR_NON_RETRY 0x41
#define ATA_COMMAND_FORMAT_TRACK                 0x50
#define ATA_COMMAND_READ_DMA_NON_RETRY           0xc9
#define ATA_COMMAND_WRITE_DMA_NON_RETRY          0xcb
#define ATA_COMMAND_ACKNOWLEGE_MEDIA_CHANGE      0xdb
#define ATA_COMMAND_BOOT_POST_BOOT               0xdc
#define ATA_COMMAND_BOOT_PRE_BOOT                0xdd
#define ATA_COMMAND_WRITE_SAME                   0xe9

/* Commands from ATA-4 specification: CFA feature set */
#define ATA_COMMAND_CFA_REQUEST_EXTENDED_ERROR_CODE  0x03
#define ATA_COMMAND_CFA_WRITE_SECTORS_WITHOUT_ERASE  0x38
#define ATA_COMMAND_CFA_TRANSLATE_SECTOR             0x87
#define ATA_COMMAND_CFA_ERASE_SECTORS                0xc0
#define ATA_COMMAND_CFA_WRITE_MULTIPLE_WITHOUT_ERASE 0xcd

/* Commands from ATA-4 specification: commands to use with PACKET command */
#define ATA_COMMAND_DEVICE_RESET                 0x08
#define ATA_COMMAND_PACKET                       0xa0
#define ATA_COMMAND_IDENTIFY_PACKET_DEVICE       0xa1
#define ATA_COMMAND_SERVICE                      0xa2

/* Commands from ATA-4 specification: SECURITY commands */
#define ATA_COMMAND_SECURITY_SET_PASSWORD        0xf1
#define ATA_COMMAND_SECURITY_UNLOCK              0xf2
#define ATA_COMMAND_SECURITY_ERASE_PREPARE       0xf3
#define ATA_COMMAND_SECURITY_ERASE_UNIT          0xf4
#define ATA_COMMAND_SECURITY_FREEZE_LOCK         0xf5
#define ATA_COMMAND_SECURITY_DISABLE_PASSWORD    0xf6

/* Commands from ATA-4 specification: other commands */
#define ATA_COMMAND_SMART                        0xb0
#define ATA_COMMAND_READ_DMA_QUEUED              0xc7
#define ATA_COMMAND_WRITE_DMA_QUEUED             0xcc
#define ATA_COMMAND_GET_MEDIA_STATUS             0xda
#define ATA_COMMAND_FLUSH_CACHE                  0xe7
#define ATA_COMMAND_READ_NATIVE_MAX_ADDRESS      0xf8
#define ATA_COMMAND_SET_MAX_ADDRESS              0xf9

#define ATA_REGISTERS_VALUE(reg)    (1 << (reg))

/* ATA IDENTIFY DEVICE command words and bits */
#define ATA_IDENT_WORD_RW_MULT                   47
#define ATA_IDENT_WORD_CAPABILITIES              49
#define ATA_IDENT_WORD_FIELD_VALIDITY            53
#define ATA_IDENT_WORD_NUM_OF_CURR_LOG_CLNDS     54
#define ATA_IDENT_WORD_NUM_OF_CURR_LOG_HEADS     55
#define ATA_IDENT_WORD_NUM_OF_CURR_LOG_SECS      56
#define ATA_IDENT_WORD_MULT_SECS                 59
#define ATA_IDENT_WORD_NUM_OF_USR_SECS0          60
#define ATA_IDENT_WORD_NUM_OF_USR_SECS1          61
#define ATA_IDENT_WORD_PIO_SPPRTD                64

#define ATA_IDENT_BIT_VALID                      0x02

/*
 * It is OR for all ATA_REGISTERS_VALUE(reg), where reg is neccessary
 * for setting block position
 */
#define ATA_REGISTERS_POSITION       0xfc

#define ATA_MINOR_NUM_RESERVED_PER_ATA_DEVICE     64

#define ATA_MAX_RTEMS_INT_VEC_NUMBER              255

#define ATA_MAX_NAME_LENGTH                       10

/* diagnostic codes */
#define ATA_DEV0_PASSED_DEV1_PASSED_OR_NOT_PRSNT  0x01
#define ATA_DEV0_PASSED_DEV1_FAILED               0x81
#define ATA_DEV1_PASSED_DEV0_FAILED               0x80

/*
 * Obtain ata device parameters by controller minor number and device number
 */
#define ATA_DEV_INFO(controller_minor, dev) \
    ata_ide_ctrls[controller_minor].device[dev]

/* ATA RTEMS driver internal data stuctures */

/* Command block registers */
typedef struct ata_registers_s {
    uint16_t   regs[8];  /* command block registers */
    uint16_t   to_read;  /* mask: which ata registers should be read */
    uint16_t   to_write; /* mask: which ata registers should be written */
} ata_registers_t;

/* ATA request */
typedef struct ata_req_s {
    rtems_chain_node  link;   /* link in requests chain */
    char              type;   /* request type */
    ata_registers_t   regs;   /* ATA command */
    uint32_t    cnt;    /* Number of sectors to be exchanged */
    uint32_t    cbuf;   /* number of current buffer from breq in use */
    uint32_t    pos;    /* current position in 'cbuf' */
    rtems_blkdev_request *breq;   /* blkdev_request which corresponds to the
                               * ata request
                               */
    rtems_id          sema;   /* semaphore which is used if synchronous
                               * processing of the ata request is required
                               */
    rtems_status_code status; /* status of ata request processing */
    int               info;  /* device info code */
} ata_req_t;

/* call callback provided by block device request if it is defined */
#define ATA_EXEC_CALLBACK(areq, status) \
    do {\
        if ((areq)->breq != NULL) \
            rtems_blkdev_request_done((areq)->breq, status); \
    } while (0)

/* ATA RTEMS driver events types */
typedef enum ata_msg_type_s {
    ATA_MSG_GEN_EVT = 1,     /* general event */
    ATA_MSG_SUCCESS_EVT,     /* success event */
    ATA_MSG_ERROR_EVT,       /* error event */
    ATA_MSG_PROCESS_NEXT_EVT /* process next request event */
} ata_msg_type_t;

/* ATA RTEMS driver message */
typedef struct ata_queue_msg_s {
    ata_msg_type_t            type;       /* message type */
    rtems_device_minor_number ctrl_minor; /* IDE controller minor number */
    int                       error;      /* error code */
} ata_queue_msg_t;

/* macros for messages processing */
#define ATA_FILL_MSG(msg, evt_type, ctrl, err)\
    do {\
        msg.type = evt_type;\
        msg.ctrl_minor = ctrl;\
        msg.error = err;\
    } while (0)

#define ATA_SEND_EVT(msg, type, ctrl, err)\
    do {\
        rtems_status_code rc;\
        ATA_FILL_MSG(msg, type, ctrl, err);\
        rc = rtems_message_queue_send(ata_queue_id, &msg,\
                                      sizeof(ata_queue_msg_t));\
        if (rc != RTEMS_SUCCESSFUL)\
            rtems_fatal_error_occurred(RTEMS_INTERNAL_ERROR);\
    } while (0)

/*
 * Array of such structures is indexed by interrupt vecotrs and used for
 * mapping of IDE controllers and interrupt vectors
 */
typedef struct ata_int_st_s {
    rtems_chain_node          link;
    rtems_device_minor_number ctrl_minor;
} ata_int_st_t;

/*
 * Mapping of rtems ATA devices to the following pairs:
 * (IDE controller number served the device, device number on the controller)
 */
typedef struct ata_ide_dev_s {
    int ctrl_minor;/* minor number of IDE controller served rtems ATA device */
    int device;    /* device number on IDE controller (0 or 1) */
} ata_ide_dev_t;

/*
 * ATA device description
 */
typedef struct ata_dev_s {
    int8_t      present;     /* 1 -- present, 0 -- not present, */
                             /* -1 -- non-initialized */
    uint16_t    cylinders;
    uint16_t    heads;
    uint16_t    sectors;
    uint32_t    lba_sectors;  /* for small disk */
                              /* == cylinders * heads * sectors */

    uint8_t     lba_avaible;  /* 0 - CHS mode, 1 - LBA mode */

    uint16_t  modes_available; /* OR of values for this modes */
    uint16_t  mode_active;
} ata_dev_t;

/*
 * This structure describes controller state, devices configuration on the
 * controller and chain of ATA requests to the controller. Array of such
 * structures is indexed by controller minor number
 */
typedef struct ata_ide_ctrl_s {
    bool          present;   /* controller state */
    ata_dev_t     device[2]; /* ata diveces description */
    rtems_chain_control reqs; /* requests chain */
} ata_ide_ctrl_t;

/* Block device request with a single buffer provided */
typedef struct blkdev_request1 {
    rtems_blkdev_request   req;
    rtems_blkdev_sg_buffer sg[1];
} blkdev_request1;

void ata_breq_init(blkdev_request1 *breq, uint16_t *sector_buffer);

rtems_status_code ata_identify_device(
  rtems_device_minor_number ctrl_minor,
  int dev,
  uint16_t *sector_buffer,
  ata_dev_t *device_entry
);

void ata_process_request_on_init_phase(
  rtems_device_minor_number ctrl_minor,
  ata_req_t *areq
);

#ifdef __cplusplus
}
#endif

#endif /* __ATA_INTERNAL_H__ */

/*
 * ata.c
 *
 * ATA RTEMS driver. ATA driver is hardware independant implementation of 
 * ATA-2 standart, working draft X3T10/0948D, revision 4c. ATA driver bases 
 * on RTEMS IDE controller driver.
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Authors: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.OARcorp.com/rtems/license.html.
 * 
 * $Id$
 * 
 */
#include <errno.h>
#include <chain.h>
#include <assert.h>
#include <string.h> /* for "memset" declaration */

#include <rtems/diskdevs.h>
#include <rtems/blkdev.h>
#include <libchip/ide_ctrl_io.h>
#include <libchip/ide_ctrl_cfg.h>
#include "ata_internal.h"
#include <libchip/ata.h>

#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#endif

#define SAFE
#ifdef SAFE
typedef rtems_mode preemption_key;

#define DISABLE_PREEMPTION(key) \
    do {                                                               \
        rtems_task_mode(RTEMS_NO_PREEMPT, RTEMS_PREEMPT_MASK, &(key)); \
    } while (0)

#define ENABLE_PREEMPTION(key) \
    do {                                                        \
        rtems_mode temp;                                        \
        rtems_task_mode((key), RTEMS_PREEMPT_MASK, &temp);      \
    } while (0)

#else
 
typedef boolean preemption_key;

#define DISABLE_PREEMPTION(key) \
    do {                                             \
        (key) = _Thread_Executing->is_preemptible;   \
        _Thread_Executing->is_preemptible = 0;       \
    } while (0)
    
#define ENABLE_PREEMPTION(key) \
    do {                                             \
        _Thread_Executing->is_preemptible = (key);   \
        if (_Thread_Evaluate_mode())                 \
            _Thread_Dispatch();                      \
    } while (0)

#endif

/* FIXME: case if ATA device is FLASH device need more attention */
#undef ATA_DEV_IS_FLASH_DISK

/* Block device request with a single buffer provided */
typedef struct blkdev_request1 {
    blkdev_request   req;
    blkdev_sg_buffer sg[1];
} blkdev_request1;


/* Array indexed by controllers minor number */
static ata_ide_ctrl_t ata_ide_ctrls[IDE_CTRL_MAX_MINOR_NUMBER];

/*
 * Mapping from ATA-minor numbers to 
 * controller-minor and device on this controller.
 */
static ata_ide_dev_t ata_devs[2 * IDE_CTRL_MAX_MINOR_NUMBER];
static int ata_devs_number;

/* Flag meaning that ATA driver has already been initialized */
static rtems_boolean ata_initialized = FALSE;


/* task and queue used for asynchronous I/O operations */
static rtems_id ata_task_id;
static rtems_id ata_queue_id;

/* Mapping of interrupt vectors to devices */
static Chain_Control ata_int_vec[ATA_MAX_RTEMS_INT_VEC_NUMBER + 1];  

static void
ata_process_request(rtems_device_minor_number ctrl_minor);

static void
ata_process_request_on_init_phase(rtems_device_minor_number  ctrl_minor,
                                  ata_req_t                 *areq);

static void 
ata_add_to_controller_queue(rtems_device_minor_number  ctrl_minor, 
                            ata_req_t                 *areq);

/*
 * read/write, open/close and ioctl are provided by general block device 
 * driver. Only initialization and ata-specific ioctl are here.
 */

/* ata_io_data_request -- 
 *     Form read/write request for an ATA device and enqueue it to 
 *     IDE controller. 
 * 
 * PARAMETERS:
 *     device - device identifier
 *     req    - read/write request from block device driver
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL on success, or error code if
 *     error occured 
 */
static rtems_status_code
ata_io_data_request(dev_t device, blkdev_request *req)
{
    ata_req_t                 *areq; /* ATA request */
    rtems_device_minor_number  rel_minor; /* relative minor which indexes 
                                           * ata_devs array
                                           */
    rtems_device_minor_number  ctrl_minor;
    unsigned8                  dev;
    
    rel_minor = (rtems_filesystem_dev_minor_t(device)) / 
                ATA_MINOR_NUM_RESERVED_PER_ATA_DEVICE;
    
    /* get controller which serves the ATA device */
    ctrl_minor = ata_devs[rel_minor].ctrl_minor;

    /* get ATA device identifier (0 or 1) */
    dev = ata_devs[rel_minor].device;

    areq = malloc(sizeof(ata_req_t));
    if (areq == NULL)
    {
        return RTEMS_NO_MEMORY;
    }
                                                            
    areq->breq = req;
    areq->cnt = req->count;
    areq->cbuf = 0;
    areq->pos = 0;
    
    /* set up registers masks */   
    areq->regs.to_write = ATA_REGISTERS_POSITION;
    areq->regs.to_read = ATA_REGISTERS_VALUE(IDE_REGISTER_STATUS);
    
    /* choose device on the controller for which the command will be issued */
    areq->regs.regs[IDE_REGISTER_DEVICE_HEAD] = 
                                    (dev << IDE_REGISTER_DEVICE_HEAD_DEV_POS);

    /* Find ATA command and its type */
    if (ATA_DEV_INFO(ctrl_minor, dev).mode_active & ATA_MODES_DMA)
    {
        /* XXX: never has been tested */
        areq->type = ATA_COMMAND_TYPE_DMA;
        if (req->req == BLKDEV_REQ_READ)
            areq->regs.regs[IDE_REGISTER_COMMAND] = ATA_COMMAND_READ_DMA;
        else 
            areq->regs.regs[IDE_REGISTER_COMMAND] = ATA_COMMAND_WRITE_DMA;
    }
    else 
    {
        if (req->req == BLKDEV_REQ_READ)
        {
            areq->type = ATA_COMMAND_TYPE_PIO_IN;
              
            /* 
             * choose command to issue: if the number of blocks to be 
             * exchanged is greater then 1 and for ATA command READ MULTIPLE
             * data block consists of more then 1 sector choose READ MULTIPLE
             * otherwise READ SECTORS
             */
            areq->regs.regs[IDE_REGISTER_COMMAND] = 
                ((ATA_DEV_INFO(ctrl_minor, dev).max_multiple) && 
                 (req->count > 1) && 
                 (ATA_DEV_INFO(ctrl_minor, dev).current_multiple > 1)) ? 
                 ATA_COMMAND_READ_MULTIPLE : 
                 ATA_COMMAND_READ_SECTORS;
        }
        else
        {
            areq->type = ATA_COMMAND_TYPE_PIO_OUT;

            /* 
             * choose command to issue: if the number of blocks to be 
             * exchanged is greater then 1 and for ATA command WRITE MULTIPLE
             * data block consists of more then 1 sector choose WRITE MULTIPLE
             * otherwise WRITE SECTORS
             */
            areq->regs.regs[IDE_REGISTER_COMMAND] = 
              ((ATA_DEV_INFO(ctrl_minor, dev).max_multiple) &&
               (req->count > 1) &&
               (ATA_DEV_INFO(ctrl_minor, dev).current_multiple > 1)) ? 
               ATA_COMMAND_WRITE_MULTIPLE : 
               ATA_COMMAND_WRITE_SECTORS;
        }
    }

    /*
     * Fill position registers
     */
    if (ATA_DEV_INFO(ctrl_minor, dev).lba_avaible)
    {
        areq->regs.regs[IDE_REGISTER_LBA0] = (unsigned8)req->start;
        areq->regs.regs[IDE_REGISTER_LBA1] = (unsigned8)(req->start >> 8);
        areq->regs.regs[IDE_REGISTER_LBA2] = (unsigned8)(req->start >> 16);
        areq->regs.regs[IDE_REGISTER_LBA3] |= (unsigned8) (req->start >> 24);
        areq->regs.regs[IDE_REGISTER_LBA3] |= IDE_REGISTER_LBA3_L;
    }
    else
    {
        unsigned32 count = req->start;

        areq->regs.regs[IDE_REGISTER_SECTOR_NUMBER] =
                        (count % ATA_DEV_INFO(ctrl_minor, dev).sectors) + 1;

        /* now count = number of tracks: */
        count /= ATA_DEV_INFO(ctrl_minor, dev).sectors;
        areq->regs.regs[IDE_REGISTER_DEVICE_HEAD] |= 
                          (count / ATA_DEV_INFO(ctrl_minor, dev).cylinders);

        /* now count = number of cylinders */
        count %= ATA_DEV_INFO(ctrl_minor, dev).cylinders;
        areq->regs.regs[IDE_REGISTER_CYLINDER_LOW] = (unsigned8)count;
        areq->regs.regs[IDE_REGISTER_CYLINDER_HIGH] = (unsigned8)(count >> 8);
        areq->regs.regs[IDE_REGISTER_DEVICE_HEAD] &= 
                                                ~IDE_REGISTER_DEVICE_HEAD_L;
    }

    /* fill sector count register */
    areq->regs.regs[IDE_REGISTER_SECTOR_COUNT] = areq->breq->count;
    
    /* add request to the queue of awaiting requests to the controller */
    ata_add_to_controller_queue(ctrl_minor, areq);
 
    return RTEMS_SUCCESSFUL;
}

/* ata_non_data_request -- 
 *     Form and serve request of NON DATA type for an ATA device. 
 *     Processing of NON DATA request is SYNChronous operation.
 * 
 * PARAMETERS:
 *     device - device identifier
 *     cmd    - command
 *     argp   - arguments for command
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL on success, or error code if
 *     error occured 
 */
static rtems_status_code
ata_non_data_request(dev_t device, int cmd, void *argp)
{
    rtems_status_code          rc;
    ata_req_t                 *areq;       /* ATA request */
    rtems_device_minor_number  rel_minor; /* relative minor which indexes
                                           * ata_devs array
                                           */
    rtems_device_minor_number  ctrl_minor;
    unsigned8                  dev;
    ata_queue_msg_t            msg;    
    
    rel_minor = (rtems_filesystem_dev_minor_t(device)) /
                ATA_MINOR_NUM_RESERVED_PER_ATA_DEVICE; 

    /* get controller which serves the ATA device */
    ctrl_minor = ata_devs[rel_minor].ctrl_minor;

    /* get ATA device identifier (0 or 1) */
    dev = ata_devs[rel_minor].device;

    /* form the request */
    areq = malloc(sizeof(ata_req_t));
    if (areq == NULL)
    {
        return RTEMS_NO_MEMORY;
    }
    memset(areq, 0, sizeof(ata_req_t));    

    areq->type = ATA_COMMAND_TYPE_NON_DATA;
    areq->regs.to_write = ATA_REGISTERS_VALUE(IDE_REGISTER_COMMAND);
    areq->regs.regs[IDE_REGISTER_DEVICE_HEAD] |= 
                                    (dev << IDE_REGISTER_DEVICE_HEAD_DEV_POS);
    areq->breq = NULL;
    areq->regs.to_read = ATA_REGISTERS_VALUE(IDE_REGISTER_ERROR);
    
    /* 
     * depending on command fill command register and additional registers 
     * which are needed for command execution
     */
    switch(cmd)
    { 
        case ATAIO_SET_MULTIPLE_MODE:
            areq->regs.regs[IDE_REGISTER_COMMAND] = 
                                                ATA_COMMAND_SET_MULTIPLE_MODE;
            areq->regs.to_write |= 
                               ATA_REGISTERS_VALUE(IDE_REGISTER_SECTOR_COUNT);
            areq->regs.regs[IDE_REGISTER_SECTOR_COUNT] = *(unsigned8 *)argp;
            break;
        
        default:
            free(areq);
            return RTEMS_INVALID_NUMBER;
            break;                                
    }  

    rc = rtems_semaphore_create(rtems_build_name('I', 'D', 'E', 'S'), 
                                0, 
                                RTEMS_FIFO | RTEMS_COUNTING_SEMAPHORE | 
                                RTEMS_NO_INHERIT_PRIORITY |
                                RTEMS_NO_PRIORITY_CEILING | RTEMS_LOCAL,
                                0,
                                &(areq->sema));
    if (rc != RTEMS_SUCCESSFUL)
    {
        free(areq);
        return rc;
    }                                 
                                        
    ata_add_to_controller_queue(ctrl_minor, areq);
    
    /* wait for request processing... */
    rc = rtems_semaphore_obtain(areq->sema, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    if (rc != RTEMS_SUCCESSFUL)
    {
        free(areq);
        return rc;
    }                                 

    rtems_semaphore_delete(areq->sema);
    
    /* 
     * if no error occurred and if necessary, update internal ata driver data 
     * structures to reflect changes (in device configuration, for example)
     */ 
    if (areq->status == RTEMS_SUCCESSFUL)
    {
        switch(cmd)
        { 
            case ATAIO_SET_MULTIPLE_MODE:
                ATA_DEV_INFO(ctrl_minor, dev).current_multiple = 
                                                           *(unsigned8 *)argp;
                break;
        
            default:
                rc = RTEMS_INVALID_NUMBER;
                break;                                
        }  
    }    
    else    
    {    
        /* XXX: should be correct error processing: for ex, may be 
         * ABRT and then we should return RTEMS_NOT_IMPLEMENTED
         */
        rc = RTEMS_IO_ERROR;
    }    
    
    /* tell ata driver that controller ready to serve next request */
    ATA_SEND_EVT(msg, ATA_MSG_SUCCESS_EVT, ctrl_minor, 0);
                                                 
    return rc;
}

/* ata_process_request -- 
 *     Get first request from controller's queue and process it. 
 * 
 * PARAMETERS:
 *     ctrl_minor - controller identifier
 *
 * RETURNS:
 *     NONE 
 */
static void
ata_process_request(rtems_device_minor_number ctrl_minor)
{
    ata_req_t       *areq;
    unsigned16       byte; /* emphasize that only 8 low bits is meaningful */
    ata_queue_msg_t  msg;
    unsigned8        i, dev;
    unsigned16       val;
    unsigned16       data_bs; /* the number of 512-bytes sectors in one 
                               * data block 
                               */
    ISR_Level        level;                           
        
    /* if no requests to controller then do nothing */
    if (Chain_Is_empty(&ata_ide_ctrls[ctrl_minor].reqs))
        return;

    /* get first request in the controller's queue */
    _ISR_Disable(level);
    areq = (ata_req_t *)(ata_ide_ctrls[ctrl_minor].reqs.first);
    _ISR_Enable(level);
    
    /* get ATA device identifier (0 or 1) */
    dev =  areq->regs.regs[IDE_REGISTER_DEVICE_HEAD] & 
           IDE_REGISTER_DEVICE_HEAD_DEV;

    /* get data block size */
    data_bs = ATA_DEV_INFO(ctrl_minor, dev).current_multiple ?  
              ATA_DEV_INFO(ctrl_minor, dev).current_multiple : 1;      
    
    /* execute device select protocol */
    ide_controller_write_register(ctrl_minor, IDE_REGISTER_DEVICE_HEAD, 
                                  areq->regs.regs[IDE_REGISTER_DEVICE_HEAD]);
    
    do {
        ide_controller_read_register(ctrl_minor, IDE_REGISTER_STATUS, &byte);
    } while ((byte & IDE_REGISTER_STATUS_BSY) || 
             (!(byte & IDE_REGISTER_STATUS_DRDY)));

    /* fill in all  necessary registers on the controller */
    for (i=0; i< ATA_MAX_CMD_REG_OFFSET; i++)
    {
        unsigned32 reg = (1 << i);
        if (areq->regs.to_write & reg)
            ide_controller_write_register(ctrl_minor, i, areq->regs.regs[i]);
    }

    /* continue to execute ATA protocols depending on type of request */
    if (areq->type == ATA_COMMAND_TYPE_PIO_OUT)
    {
        do {
            ide_controller_read_register(ctrl_minor, IDE_REGISTER_STATUS, 
                                         &byte);
        } while (byte & IDE_REGISTER_STATUS_BSY);

        if (byte & IDE_REGISTER_STATUS_DRQ)
        {
            ide_controller_write_data_block(
                ctrl_minor, 
                MIN(data_bs, areq->cnt) * ATA_SECTOR_SIZE,
                areq->breq->bufs, &areq->cbuf,
                &areq->pos);
            areq->cnt -= MIN(data_bs, areq->cnt);
        }    
        else
        {    
            if (IDE_Controller_Table[ctrl_minor].int_driven == 0)
            {
                ide_controller_read_register(
                    ctrl_minor, 
                    IDE_REGISTER_ALTERNATE_STATUS_OFFSET,
                    &val);
                ide_controller_read_register(ctrl_minor, IDE_REGISTER_STATUS,
                                             &val);

                ATA_SEND_EVT(msg, ATA_MSG_ERROR_EVT, ctrl_minor, 
                             RTEMS_IO_ERROR);
            }
        }    
    }

    if (IDE_Controller_Table[ctrl_minor].int_driven == 0)
    {
        do {
            ide_controller_read_register(ctrl_minor, IDE_REGISTER_STATUS, 
                                         &byte);
        } while (byte & IDE_REGISTER_STATUS_BSY);
        
        ATA_SEND_EVT(msg, ATA_MSG_GEN_EVT, ctrl_minor, 0);
    }    
}

/* ata_request_done -- 
 *     Extract request from controller queue, execute callback if necessary
 *     and process next request for the controller.
 * 
 * PARAMETERS:
 *     areq       - ATA request 
 *     ctrl_minor - controller identifier
 *     status     - status with which request has been done
 *     error      - error, if status != RTEMS_SUCCESSFUL
 *
 * RETURNS:
 *     NONE
 */
static inline void
ata_request_done(ata_req_t *areq, rtems_device_minor_number ctrl_minor,
                 rtems_status_code status, int error)
{
    preemption_key key;

    assert(areq);

    DISABLE_PREEMPTION(key);
    ATA_EXEC_CALLBACK(areq, status, error);
    Chain_Extract(&areq->link);
    free(areq);

    if (!Chain_Is_empty(&ata_ide_ctrls[ctrl_minor].reqs))
    {
        ENABLE_PREEMPTION(key);
        ata_process_request(ctrl_minor);
        return;
    }
    ENABLE_PREEMPTION(key); 
}

/* ata_non_data_request_done -- 
 *     Set up request status and release request's semaphore.
 * 
 * PARAMETERS:
 *     areq       - ATA request 
 *     ctrl_minor - controller identifier
 *     status     - status with which request has been done
 *     error      - error, if status != RTEMS_SUCCESSFUL
 *
 * RETURNS:
 *     NONE
 */
static inline void
ata_non_data_request_done(ata_req_t *areq, 
                          rtems_device_minor_number ctrl_minor,
                          rtems_status_code status, int error)
{
    areq->status = status;
    areq->error = error;
    rtems_semaphore_release(areq->sema);
}


/* ata_add_to_controller_queue -- 
 *     Add request to the controller's queue.
 * 
 * PARAMETERS:
 *     ctrl_minor - controller identifier
 *     areq       - ATA request 
 *
 * RETURNS:
 *     NONE
 */
static void
ata_add_to_controller_queue(rtems_device_minor_number  ctrl_minor, 
                            ata_req_t                 *areq)
{
    Chain_Append(&ata_ide_ctrls[ctrl_minor].reqs, &areq->link);
    if (Chain_Has_only_one_node(&ata_ide_ctrls[ctrl_minor].reqs))
    {
        unsigned16      val;
        ata_queue_msg_t msg;

#ifdef DEBUG
        /* 
         * read IDE_REGISTER_ALTERNATE_STATUS instead IDE_REGISTER_STATUS
         * to prevent clearing of pending interrupt
         */
        ide_controller_read_register(ctrl_minor, 
                                     IDE_REGISTER_ALTERNATE_STATUS, 
                                     &val);
        if (val & IDE_REGISTER_STATUS_BSY)
            return;
#endif
        ATA_SEND_EVT(msg, ATA_MSG_PROCESS_NEXT_EVT, ctrl_minor, 0);
    }    
}                            


/* ata_interrupt_handler -- 
 *     ATA driver interrrupt handler. If interrrupt happend it mapped it to
 *     controller (controllerS, if a number of controllers share one int line)
 *     and generates ATA event(s).
 * 
 * PARAMETERS:
 *     vec - interrupt vector  
 *
 * RETURNS:
 *     NONE
 */
rtems_isr
ata_interrupt_handler(rtems_vector_number vec)
{
    Chain_Node      *the_node = ((Chain_Control *)(&ata_int_vec[vec]))->first;
    ata_queue_msg_t  msg;
    unsigned16       byte; /* emphasize that only 8 low bits is meaningful */
    
    for ( ; !Chain_Is_tail(&ata_int_vec[vec], the_node) ; )
    {
        /* if (1) - is temporary hack - currently I don't know how to identify
         * controller which asserted interrupt if few controllers share one
         * interrupt line
         */
        if (1)
        {
            msg.ctrl_minor = ((ata_int_st_t *)the_node)->ctrl_minor;
            ide_controller_read_register(msg.ctrl_minor, IDE_REGISTER_STATUS, 
                                         &byte);
            ATA_SEND_EVT(msg, ATA_MSG_GEN_EVT, msg.ctrl_minor, 0);                             
        }
        the_node = the_node->next;
    }   
}

/* ata_pio_in_protocol -- 
 *     ATA PIO_IN protocol implementation, see specification
 * 
 * PARAMETERS:
 *     ctrl_minor - controller identifier
 *     areq       - ATA request 
 *
 * RETURNS:
 *     NONE
 */
static inline void
ata_pio_in_protocol(rtems_device_minor_number ctrl_minor, ata_req_t *areq)
{
    unsigned16      bs, val;
    unsigned8       dev;
    unsigned32      min_val;
    ata_queue_msg_t msg;
        
    dev =  areq->regs.regs[IDE_REGISTER_DEVICE_HEAD] & 
           IDE_REGISTER_DEVICE_HEAD_DEV;

    bs = ATA_DEV_INFO(ctrl_minor, dev).current_multiple ?  
         ATA_DEV_INFO(ctrl_minor, dev).current_multiple : 1;      
    min_val = MIN(bs, areq->cnt);
    
    ide_controller_read_data_block(ctrl_minor, min_val * ATA_SECTOR_SIZE, 
                                   areq->breq->bufs, &areq->cbuf, &areq->pos);
              
    areq->cnt -= min_val;
    if (areq->cnt == 0)
    {
        ata_request_done(areq, ctrl_minor, RTEMS_SUCCESSFUL, RTEMS_SUCCESSFUL);
    }
    else if (IDE_Controller_Table[ctrl_minor].int_driven == 0)
    {
        do {
           ide_controller_read_register(ctrl_minor, IDE_REGISTER_STATUS, &val);
        } while (val & IDE_REGISTER_STATUS_BSY);
        
        ATA_SEND_EVT(msg, ATA_MSG_GEN_EVT, ctrl_minor, 0);
    }
}

/* ata_pio_out_protocol -- 
 *     ATA PIO_OUT protocol implementation, see specification
 * 
 * PARAMETERS:
 *     ctrl_minor - controller identifier
 *     areq       - ATA request 
 *
 * RETURNS:
 *     NONE
 */
static inline void
ata_pio_out_protocol(rtems_device_minor_number ctrl_minor, ata_req_t *areq)
{
    unsigned16      bs, val;
    unsigned8       dev;
    unsigned32      min_val;
    ata_queue_msg_t msg;
    
    dev =  areq->regs.regs[IDE_REGISTER_DEVICE_HEAD] & 
           IDE_REGISTER_DEVICE_HEAD_DEV;

    bs = ATA_DEV_INFO(ctrl_minor, dev).current_multiple ?  
         ATA_DEV_INFO(ctrl_minor, dev).current_multiple : 1;      

    min_val = MIN(bs, areq->cnt);     

    if (areq->cnt == 0)
    {
        ata_request_done(areq, ctrl_minor, RTEMS_SUCCESSFUL, RTEMS_SUCCESSFUL);
    }
    else 
    {
        ide_controller_write_data_block(ctrl_minor, min_val * ATA_SECTOR_SIZE,
                                        areq->breq->bufs, &areq->cbuf, 
                                        &areq->pos);
        areq->cnt -= min_val;
        if (IDE_Controller_Table[ctrl_minor].int_driven == 0)
        {
            do {
                ide_controller_read_register(ctrl_minor, IDE_REGISTER_STATUS, 
                                             &val);
            } while (val & IDE_REGISTER_STATUS_BSY);

            ATA_SEND_EVT(msg, ATA_MSG_GEN_EVT, ctrl_minor, 0);
        }
  }
}

/* ata_queue_task -- 
 *     Task which manages ATA driver events queue.
 * 
 * PARAMETERS:
 *     arg - ignored
 *
 * RETURNS:
 *     NONE
 *
 * NOTES:
 *     should be non-preemptive 
 */
static rtems_task
ata_queue_task(rtems_task_argument arg)
{
    ata_queue_msg_t            msg;
    rtems_unsigned32           size;
    ata_req_t                 *areq;
    rtems_device_minor_number  ctrl_minor;
    unsigned16                 val;
    unsigned16                 val1;
    rtems_status_code          rc;
    ISR_Level                  level;

    while (1)
    {
        /* get event which has happend */
        rc = rtems_message_queue_receive(ata_queue_id, &msg, &size, RTEMS_WAIT, 
                                         RTEMS_NO_TIMEOUT);
        if (rc != RTEMS_SUCCESSFUL)
            rtems_fatal_error_occurred(RTEMS_INTERNAL_ERROR);
                                             
        /* get controller on which event has happend */
        ctrl_minor = msg.ctrl_minor;
        
        /* get current request to the controller */
        _ISR_Disable(level);        
        areq = (ata_req_t *)(ata_ide_ctrls[ctrl_minor].reqs.first);
        _ISR_Enable(level);
        
        switch(msg.type)
        {
            case ATA_MSG_PROCESS_NEXT_EVT:
                /* process next request in the controller queue */
                ata_process_request(ctrl_minor);
                break;

            case ATA_MSG_SUCCESS_EVT:
                /* 
                 * finish processing of current request with successful 
                 * status and start processing of the next request in the 
                 * controller queue
                 */
                ata_request_done(areq, ctrl_minor, RTEMS_SUCCESSFUL, 
                                 msg.error);
                break;
                
            case ATA_MSG_ERROR_EVT:
                /* 
                 * finish processing of current request with error 
                 * status and start processing of the next request in the 
                 * controller queue
                 */
                ata_request_done(areq, ctrl_minor, RTEMS_UNSATISFIED, 
                                 msg.error);
                break;
                
            case ATA_MSG_GEN_EVT:
                /* 
                 * continue processing of the current request to the 
                 * controller according to current request state and 
                 * ATA protocol
                 */
                ide_controller_read_register(ctrl_minor, IDE_REGISTER_STATUS, 
                                           &val);
                /* process error case */
                if (val & IDE_REGISTER_STATUS_ERR)
                { 
                    ide_controller_read_register(ctrl_minor, 
                                                 IDE_REGISTER_ERROR, 
                                                 &val);
                    if (areq->type == ATA_COMMAND_TYPE_NON_DATA)
                        ata_non_data_request_done(areq, ctrl_minor, 
                                                  RTEMS_UNSATISFIED, 
                                                  RTEMS_IO_ERROR);
                    else                             
                        ata_request_done(areq, ctrl_minor, RTEMS_UNSATISFIED, 
                                         RTEMS_IO_ERROR);
                    break;                                  
                }

                switch(areq->type)
                {
                    case ATA_COMMAND_TYPE_PIO_IN:
                        ata_pio_in_protocol(ctrl_minor, areq);
                        break;
                
                    case ATA_COMMAND_TYPE_PIO_OUT:
                        ata_pio_out_protocol(ctrl_minor, areq);
                        break;
                
                    case ATA_COMMAND_TYPE_NON_DATA:
                        ide_controller_read_register(ctrl_minor, 
                                                      IDE_REGISTER_ERROR, 
                                                     &val1);
                        ata_non_data_request_done(areq, ctrl_minor,
                                                  RTEMS_SUCCESSFUL,
                                                  val1);
                        break;
            
                    default:
                        printf("ata_queue_task: non-supported command type\n");
                        ata_request_done(areq, ctrl_minor,
                                         RTEMS_UNSATISFIED,
                                         RTEMS_NOT_IMPLEMENTED);
                        break;          
                }
                break;    

            default:
                rtems_fatal_error_occurred(RTEMS_INTERNAL_ERROR);
                break;
        }
    }
}

/* ata_ioctl -- 
 *     ATA driver ioctl interface.
 * 
 * PARAMETERS:
 *     device - device identifier
 *     cmd    - command
 *     argp   - arguments
 *
 * RETURNS:
 *     depend on 'cmd'
 */
int 
ata_ioctl(dev_t device, int cmd, void *argp)
{
    rtems_status_code         status;
    rtems_device_minor_number rel_minor;
    
    rel_minor = (rtems_filesystem_dev_minor_t(device)) /
                ATA_MINOR_NUM_RESERVED_PER_ATA_DEVICE;

    /* 
     * in most cases this means that device 'device' is not an registred ATA
     * device
     */ 
    if (ata_devs[rel_minor].device == ATA_UNDEFINED_VALUE)
    {
        errno = ENODEV;
        return -1;
    }    

    switch (cmd) 
    {
        case BLKIO_REQUEST:
            status = ata_io_data_request(device, (blkdev_request *)argp);
            break;
        
        case ATAIO_SET_MULTIPLE_MODE:
            status = ata_non_data_request(device, cmd, argp);
            break;
      
        default:
            errno = EBADRQC;
            return -1;
            break;
    }

    if (status != RTEMS_SUCCESSFUL)
    {
        errno = EIO;
        return -1; 
    }
    return 0;
}

/*
 * ata_initialize --
 *     Initializes all ATA devices found on initialized IDE controllers.
 * 
 * PARAMETERS:
 *     major - device major number
 *     minor - device minor number
 *     args   - arguments
 *
 * RETURNS:
 *     RTEMS_SUCCESSFUL on success, or error code if
 *     error occured 
 */
rtems_device_driver 
ata_initialize(rtems_device_major_number major, 
               rtems_device_minor_number minor_arg, 
               void *args)
{
    unsigned32         ctrl_minor;
    rtems_status_code  status;
    ata_req_t          areq;
    blkdev_request1    breq;     
    unsigned8          i, dev = 0;
    unsigned16        *buffer;
    unsigned16         ec;
    char               name[ATA_MAX_NAME_LENGTH];
    dev_t              device;
    ata_int_st_t      *int_st;
    rtems_isr_entry    old_isr;

    if (ata_initialized)
        return RTEMS_SUCCESSFUL;
            
    /* initialization of disk devices library */
    status = rtems_disk_io_initialize();
    if (status != RTEMS_SUCCESSFUL)
        return status;
        
    /* create queue for asynchronous requests handling */
    status = rtems_message_queue_create(
                 rtems_build_name('A', 'T', 'A', 'Q'),
                 ATA_DRIVER_MESSAGE_QUEUE_SIZE,
                 sizeof(ata_queue_msg_t),
                 RTEMS_FIFO | RTEMS_LOCAL,
                 &ata_queue_id);
    if (status != RTEMS_SUCCESSFUL)
    {
        rtems_disk_io_done();
        return status;
    }    

    /* 
     * create ATA driver task, see comments for task implementation for 
     * details 
     */
    status = rtems_task_create(
                 rtems_build_name ('A', 'T', 'A', 'T'),
                 ATA_DRIVER_TASK_PRIORITY,
                 ATA_DRIVER_TASK_STACK_SIZE,
                 RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_ASR | 
                 RTEMS_INTERRUPT_LEVEL(0),
                 RTEMS_NO_FLOATING_POINT | RTEMS_LOCAL,
                 &ata_task_id);
    if (status != RTEMS_SUCCESSFUL)
    {
        rtems_message_queue_delete(ata_queue_id);
        rtems_disk_io_done();
        return status;
    }    
    
    /* 
     * start ATA driver task. Actually the task will not start immediately - 
     * it will start only after multitasking support will be started
     */ 
    status = rtems_task_start(ata_task_id, ata_queue_task, 0); 
    if (status != RTEMS_SUCCESSFUL)
    {
        rtems_task_delete(ata_task_id);
        rtems_message_queue_delete(ata_queue_id);
        rtems_disk_io_done();
        return status;
    }    
    
    buffer = (unsigned16 *)malloc(ATA_SECTOR_SIZE);
    if (buffer == NULL)
    {
        rtems_task_delete(ata_task_id);
        rtems_message_queue_delete(ata_queue_id);
        rtems_disk_io_done();
        return RTEMS_NO_MEMORY;
    }    
        
    ata_devs_number = 0;

    for (i = 0; i < (2 * IDE_CTRL_MAX_MINOR_NUMBER); i++)
        ata_devs[i].device = ATA_UNDEFINED_VALUE;

    /* prepare ATA driver for handling  interrupt driven devices */
    for (i = 0; i < ATA_MAX_RTEMS_INT_VEC_NUMBER; i++)
        Chain_Initialize_empty(&ata_int_vec[i]);

    /* 
     * during ATA driver initialization EXECUTE DEVICE DIAGNOSTIC and 
     * IDENTIFY DEVICE ATA command should be issued; for these purposes ATA 
     * requests should be formed; ATA requests contain block device request, 
     * so form block device request first
     */
    memset(&breq, 0, sizeof(blkdev_request1));
    breq.req.req_done = NULL;
    breq.req.done_arg = &breq;
    breq.req.bufnum = 1;
    breq.req.count = 1;
    breq.req.bufs[0].length = ATA_SECTOR_SIZE;
    breq.req.bufs[0].buffer = buffer;

    /* 
     * for each presented IDE controller execute EXECUTE DEVICE DIAGNOSTIC
     * ATA command; for each found device execute IDENTIFY DEVICE ATA 
     * command 
     */
    for (ctrl_minor = 0; ctrl_minor < IDE_Controller_Count; ctrl_minor++)
    if (IDE_Controller_Table[ctrl_minor].status == IDE_CTRL_INITIALIZED)
    {
        Chain_Initialize_empty(&ata_ide_ctrls[ctrl_minor].reqs);
        
        if (IDE_Controller_Table[ctrl_minor].int_driven == TRUE)
        {
            int_st = malloc(sizeof(ata_int_st_t));
            if (int_st == NULL)
            {
                free(buffer);
                rtems_task_delete(ata_task_id);
                rtems_message_queue_delete(ata_queue_id);
                rtems_disk_io_done();
                return RTEMS_NO_MEMORY;
            }       

            int_st->ctrl_minor = ctrl_minor;

            status = rtems_interrupt_catch(
                         ata_interrupt_handler, 
                         IDE_Controller_Table[ctrl_minor].int_vec, 
                         &old_isr);
            if (status != RTEMS_SUCCESSFUL)
            {
                free(int_st);
                free(buffer);
                rtems_task_delete(ata_task_id);
                rtems_message_queue_delete(ata_queue_id);
                rtems_disk_io_done();
                return status;
            } 
            Chain_Append(
                &ata_int_vec[IDE_Controller_Table[ctrl_minor].int_vec],
                &int_st->link);

            /* disable interrupts */
            ide_controller_write_register(ctrl_minor, 
                                          IDE_REGISTER_DEVICE_CONTROL_OFFSET, 
                                          IDE_REGISTER_DEVICE_CONTROL_nIEN);
        }

        /*
         * Issue EXECUTE DEVICE DIAGNOSTIC ATA command for explore is
         * there any ATA device on the controller.
         */
        memset(&areq, 0, sizeof(ata_req_t));
        areq.type = ATA_COMMAND_TYPE_NON_DATA;
        areq.regs.to_write = ATA_REGISTERS_VALUE(IDE_REGISTER_COMMAND);
        areq.regs.regs[IDE_REGISTER_COMMAND] = 
                                  ATA_COMMAND_EXECUTE_DEVICE_DIAGNOSTIC;
        areq.regs.to_read = ATA_REGISTERS_VALUE(IDE_REGISTER_ERROR);

        areq.breq = (blkdev_request *)&breq;

        /* 
         * Process the request. Special processing of requests on 
         * initialization phase is needed because at this moment there 
         * is no multitasking enviroment
         */
        ata_process_request_on_init_phase(ctrl_minor, &areq);

        /*
         * check status of I/O operation
         */
        if (breq.req.status != RTEMS_SUCCESSFUL)
            continue;
      
        /* disassemble returned diagnostic codes */ 
        if (breq.req.error == ATA_DEV0_PASSED_DEV1_PASSED_OR_NOT_PRSNT)
        {
            ATA_DEV_INFO(ctrl_minor, 0).present = 1;
            ATA_DEV_INFO(ctrl_minor,1).present = 1; 
        }
        else if (breq.req.error == ATA_DEV0_PASSED_DEV1_FAILED)
        {
            ATA_DEV_INFO(ctrl_minor,0).present = 1;
            ATA_DEV_INFO(ctrl_minor,1).present = 0;
        }
        else if (breq.req.error < ATA_DEV1_PASSED_DEV0_FAILED)
        {
            ATA_DEV_INFO(ctrl_minor,0).present = 0;
            ATA_DEV_INFO(ctrl_minor,1).present = 1;
        }
        else
        {
            ATA_DEV_INFO(ctrl_minor, 0).present = 0;
            ATA_DEV_INFO(ctrl_minor, 1).present = 0;
        }

        /* refine the returned codes */
        if (ATA_DEV_INFO(ctrl_minor, 1).present != 0)
        {
            ide_controller_read_register(ctrl_minor, IDE_REGISTER_ERROR, &ec);
            if (ec & ATA_DEV1_PASSED_DEV0_FAILED)
                ATA_DEV_INFO(ctrl_minor, 1).present = 1;
            else
                ATA_DEV_INFO(ctrl_minor, 1).present = 0;
        }

        /* for each found ATA device obtain it configuration */
        for (dev = 0; dev < 2; dev++)
        if (ATA_DEV_INFO(ctrl_minor, dev).present)
        {
            /*
             * Issue DEVICE IDENTIFY ATA command and get device 
             * configuration           
             */
            memset(&areq, 0, sizeof(ata_req_t));
            areq.type = ATA_COMMAND_TYPE_PIO_IN;
            areq.regs.to_write = ATA_REGISTERS_VALUE(IDE_REGISTER_COMMAND);
            areq.regs.regs[IDE_REGISTER_COMMAND] = 
                                              ATA_COMMAND_IDENTIFY_DEVICE;
            areq.regs.to_read = ATA_REGISTERS_VALUE(IDE_REGISTER_STATUS);
            areq.breq = (blkdev_request *)&breq;
            
            areq.cnt = breq.req.count;

            areq.regs.regs[IDE_REGISTER_DEVICE_HEAD] |=
                                    (dev << IDE_REGISTER_DEVICE_HEAD_DEV_POS);

            /* 
             * Process the request. Special processing of requests on 
             * initialization phase is needed because at this moment there 
             * is no multitasking enviroment
             */
            ata_process_request_on_init_phase(ctrl_minor, &areq);

            /* check status of I/O operation */
            if (breq.req.status != RTEMS_SUCCESSFUL)
                continue;

            /*
             * Parse returned device configuration and fill in ATA internal 
             * device info structure
             */
            ATA_DEV_INFO(ctrl_minor, dev).cylinders = 
                CF_LE_W(buffer[ATA_IDENT_WORD_NUM_OF_CURR_LOG_CLNDS]);
            ATA_DEV_INFO(ctrl_minor, dev).heads = 
                CF_LE_W(buffer[ATA_IDENT_WORD_NUM_OF_CURR_LOG_HEADS]);
            ATA_DEV_INFO(ctrl_minor, dev).sectors = 
                CF_LE_W(buffer[ATA_IDENT_WORD_NUM_OF_CURR_LOG_SECS]);
            ATA_DEV_INFO(ctrl_minor, dev).lba_sectors = 
                (CF_LE_W(buffer[ATA_IDENT_WORD_NUM_OF_USR_SECS0]) << 16) + 
                 CF_LE_W(buffer[ATA_IDENT_WORD_NUM_OF_USR_SECS1]);
            ATA_DEV_INFO(ctrl_minor, dev).lba_avaible = 
                (CF_LE_W(buffer[ATA_IDENT_WORD_CAPABILITIES]) >> 9) & 0x1;
            ATA_DEV_INFO(ctrl_minor, dev).max_multiple = 
                (unsigned8) (CF_LE_W(buffer[ATA_IDENT_WORD_RW_MULT]));
            ATA_DEV_INFO(ctrl_minor, dev).current_multiple = 
                (CF_LE_W(buffer[ATA_IDENT_WORD_MULT_SECS]) & 0x100) ? 
                (unsigned8)(CF_LE_W(buffer[ATA_IDENT_WORD_MULT_SECS])) :
                 0;

            if ((CF_LE_W(buffer[ATA_IDENT_WORD_FIELD_VALIDITY]) & 
                 ATA_IDENT_BIT_VALID) == 0) {
	      /* no "supported modes" info -> use default */
	      ATA_DEV_INFO(ctrl_minor, dev).mode_active = ATA_MODES_PIO3;
	    }
	    else {
	      ATA_DEV_INFO(ctrl_minor, dev).modes_avaible =
		((CF_LE_W(buffer[64]) & 0x1) ? ATA_MODES_PIO3 : 0) |
		((CF_LE_W(buffer[64]) & 0x2) ? ATA_MODES_PIO4 : 0) |
		((CF_LE_W(buffer[63]) & 0x1) ? ATA_MODES_DMA0 : 0) |
		((CF_LE_W(buffer[63]) & 0x2) ? 
		 ATA_MODES_DMA0 | ATA_MODES_DMA1 : 0) |
		((CF_LE_W(buffer[63]) & 0x4) ? 
		 ATA_MODES_DMA0 | ATA_MODES_DMA1 | ATA_MODES_DMA2 : 0);
	      if (ATA_DEV_INFO(ctrl_minor, dev).modes_avaible == 0)
                continue;
	      /* 
	       * choose most appropriate ATA device data I/O speed supported
	       * by the controller
	       */
	      status = ide_controller_config_io_speed(
                ctrl_minor, 
                ATA_DEV_INFO(ctrl_minor, dev).modes_avaible);
	      if (status != RTEMS_SUCCESSFUL)
                continue;
	    }      
            /*
             * Ok, let register new ATA device in the system
             */
            ata_devs[ata_devs_number].ctrl_minor = ctrl_minor;
            ata_devs[ata_devs_number].device = dev;

            /* The space leaves a hole for the character. */
            strcpy(name, "/dev/hd ");
            name[7] = 'a' + 2 * ctrl_minor + dev;

            device = rtems_filesystem_make_dev_t(
                         major, 
                         (ata_devs_number * 
                          ATA_MINOR_NUM_RESERVED_PER_ATA_DEVICE));

            status = rtems_disk_create_phys(device, ATA_SECTOR_SIZE, 
                ATA_DEV_INFO(ctrl_minor, dev).lba_avaible ? 
                ATA_DEV_INFO(ctrl_minor, dev).lba_sectors : 
                (ATA_DEV_INFO(ctrl_minor, dev).heads *
                 ATA_DEV_INFO(ctrl_minor, dev).cylinders *
                 ATA_DEV_INFO(ctrl_minor, dev).sectors),
                (block_device_ioctl) ata_ioctl, name);
            if (status != RTEMS_SUCCESSFUL)
            {
                ata_devs[ata_devs_number].device = ATA_UNDEFINED_VALUE;
                continue;
            }       
            ata_devs_number++;
        }
        if (IDE_Controller_Table[ctrl_minor].int_driven == TRUE)
        {    
            ide_controller_write_register(ctrl_minor, 
                                          IDE_REGISTER_DEVICE_CONTROL_OFFSET, 
                                          0x00);
        }
    }

    free(buffer);
    ata_initialized = TRUE;
    return RTEMS_SUCCESSFUL;
}

/* ata_process_request_on_init_phase -- 
 *     Process the ATA request during system initialization. Request 
 *     processing is syncronous and doesn't use multiprocessing enviroment.
 * 
 * PARAMETERS:
 *     ctrl_minor - controller identifier
 *     areq       - ATA request
 *
 * RETURNS:
 *     NONE 
 */
static void
ata_process_request_on_init_phase(rtems_device_minor_number  ctrl_minor,
                                  ata_req_t                 *areq)
{
    unsigned16         byte;/* emphasize that only 8 low bits is meaningful */
    unsigned8          i, dev;
    unsigned16         val, val1;
    unsigned16         data_bs; /* the number of 512 bytes sectors into one 
                                 * data block 
                                 */
    assert(areq);
    
    dev =  areq->regs.regs[IDE_REGISTER_DEVICE_HEAD] & 
           IDE_REGISTER_DEVICE_HEAD_DEV;

    data_bs = ATA_DEV_INFO(ctrl_minor, dev).current_multiple ?  
              ATA_DEV_INFO(ctrl_minor, dev).current_multiple : 1;      
    
    ide_controller_write_register(ctrl_minor, IDE_REGISTER_DEVICE_HEAD, 
                                  areq->regs.regs[IDE_REGISTER_DEVICE_HEAD]);
    
    do {
        ide_controller_read_register(ctrl_minor, IDE_REGISTER_STATUS, &byte);
    } while ((byte & IDE_REGISTER_STATUS_BSY) || 
             (!(byte & IDE_REGISTER_STATUS_DRDY)));

    for (i=0; i< ATA_MAX_CMD_REG_OFFSET; i++)
    {
        unsigned32 reg = (1 << i);
        if (areq->regs.to_write & reg)
            ide_controller_write_register(ctrl_minor, i, 
                                          areq->regs.regs[i]);
    }

    do {
        ide_controller_read_register(ctrl_minor, IDE_REGISTER_STATUS, &byte);
    } while (byte & IDE_REGISTER_STATUS_BSY);

    ide_controller_read_register(ctrl_minor, IDE_REGISTER_STATUS, &val);
    ide_controller_read_register(ctrl_minor, IDE_REGISTER_ERROR, &val1);

    if (val & IDE_REGISTER_STATUS_ERR)
    {
        areq->breq->status = RTEMS_UNSATISFIED;
        areq->breq->error = RTEMS_IO_ERROR;
        return;
    }
 
    switch(areq->type)
    {
        case ATA_COMMAND_TYPE_PIO_IN:
            ide_controller_read_data_block(
                ctrl_minor, 
                MIN(data_bs, areq->cnt) * ATA_SECTOR_SIZE,
                areq->breq->bufs, &areq->cbuf,
                &areq->pos);
              
            areq->cnt -= MIN(data_bs, areq->cnt);
            if (areq->cnt == 0)
            {
                areq->breq->status = RTEMS_SUCCESSFUL;
            }
            else 
            {
                /* 
                 * this shouldn't happend on the initialization 
                 * phase! 
                 */
                rtems_fatal_error_occurred(RTEMS_INTERNAL_ERROR); 
            }
            break;
                
        case ATA_COMMAND_TYPE_NON_DATA:
            areq->breq->status = RTEMS_SUCCESSFUL;
            areq->breq->error = val1;
            break;
            
        default:
            printf("ata_queue_task: non-supported command type\n");
            areq->breq->status = RTEMS_UNSATISFIED;
            areq->breq->error = RTEMS_NOT_IMPLEMENTED;
            break;          
    }
}


/*
 * ata.c
 *
 * ATA RTEMS driver. ATA driver is hardware independant implementation of
 * ATA-2 standard, working draft X3T10/0948D, revision 4c. ATA driver bases
 * on RTEMS IDE controller driver.
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Authors: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 */
#include <errno.h>
#include <rtems/chain.h>
#include <assert.h>
#include <string.h> /* for "memset" declaration */

#include <rtems/diskdevs.h>
#include <rtems/blkdev.h>
#include <libchip/ide_ctrl_io.h>
#include <libchip/ide_ctrl_cfg.h>
#include "ata_internal.h"
#include <libchip/ata.h>

#define ATA_DEBUG 0

#if ATA_DEBUG
#include <stdio.h>
bool ata_trace;
#define ata_printf if (ata_trace) printf
#endif

#if CPU_SIMPLE_VECTORED_INTERRUPTS != TRUE
#include <rtems/irq.h>
#define ATA_IRQ_CHAIN_MAX_CNT 4 /* support up to 4 ATA devices */
typedef struct {
  rtems_irq_number name;
  rtems_chain_control irq_chain;
} ata_irq_chain_t;

ata_irq_chain_t ata_irq_chain[ATA_IRQ_CHAIN_MAX_CNT];
int ata_irq_chain_cnt = 0;
#endif

#define SAFE
#define SAFE_MUTEX

#ifdef SAFE
#ifdef SAFE_MUTEX
static rtems_id ata_lock;
static void
rtems_ata_lock (void)
{
  rtems_status_code sc = rtems_semaphore_obtain (ata_lock,
                                                 RTEMS_WAIT,
                                                 RTEMS_NO_TIMEOUT);
  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (RTEMS_INTERNAL_ERROR);
}

static void
rtems_ata_unlock (void)
{
  rtems_status_code sc = rtems_semaphore_release (ata_lock);
  if (sc != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (RTEMS_INTERNAL_ERROR);
}

#define RTEMS_ATA_LOCK_ATTRIBS \
  (RTEMS_PRIORITY | RTEMS_BINARY_SEMAPHORE | \
   RTEMS_INHERIT_PRIORITY | RTEMS_NO_PRIORITY_CEILING | RTEMS_LOCAL)

#define PREEMPTION_KEY(key)
#define DISABLE_PREEMPTION(key) rtems_ata_lock ()
#define ENABLE_PREEMPTION(key)  rtems_ata_unlock ()

#else /* !SAFE_MUTEX */
typedef rtems_mode preemption_key;

#define PREEMPTION_KEY(key) preemption_key key

#define DISABLE_PREEMPTION(key)                                        \
    do {                                                               \
        rtems_task_mode(RTEMS_NO_PREEMPT, RTEMS_PREEMPT_MASK, &(key)); \
    } while (0)

#define ENABLE_PREEMPTION(key) \
    do {                                                        \
        rtems_mode temp;                                        \
        rtems_task_mode((key), RTEMS_PREEMPT_MASK, &temp);      \
    } while (0)
#endif
#else /* !SAFE */
typedef bool preemption_key;

#define PREEMPTION_KEY(key) preemption_key key

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

/* Array indexed by controllers minor number */
static ata_ide_ctrl_t ata_ide_ctrls[IDE_CTRL_MAX_MINOR_NUMBER];

/*
 * Mapping from ATA-minor numbers to
 * controller-minor and device on this controller.
 */
static ata_ide_dev_t ata_devs[2 * IDE_CTRL_MAX_MINOR_NUMBER];
static int ata_devs_number;

/* Flag meaning that ATA driver has already been initialized */
static bool ata_initialized = false;


/* task and queue used for asynchronous I/O operations */
static rtems_id ata_task_id;
static rtems_id ata_queue_id;

#if CPU_SIMPLE_VECTORED_INTERRUPTS == TRUE
/* Mapping of interrupt vectors to devices */
static rtems_chain_control ata_int_vec[ATA_MAX_RTEMS_INT_VEC_NUMBER + 1];
#endif

static void
ata_process_request(rtems_device_minor_number ctrl_minor);

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
ata_io_data_request(dev_t device, rtems_blkdev_request *req)
{
    ata_req_t                 *areq; /* ATA request */
    rtems_device_minor_number  rel_minor; /* relative minor which indexes
                                           * ata_devs array
                                           */
    rtems_device_minor_number  ctrl_minor;
    uint8_t                    dev;

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
    areq->cnt = req->bufnum;
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
        if (req->req == RTEMS_BLKDEV_REQ_READ)
            areq->regs.regs[IDE_REGISTER_COMMAND] = ATA_COMMAND_READ_DMA;
        else
            areq->regs.regs[IDE_REGISTER_COMMAND] = ATA_COMMAND_WRITE_DMA;
    }
    else
    {
        if (req->req == RTEMS_BLKDEV_REQ_READ)
        {
            areq->type = ATA_COMMAND_TYPE_PIO_IN;
            areq->regs.regs[IDE_REGISTER_COMMAND] = ATA_COMMAND_READ_SECTORS;
#if ATA_DEBUG
            ata_printf("ata_io_data_request: type: READ: %lu, %lu cmd:%02x\n",
                       req->bufs[0].block, req->bufnum,
                       areq->regs.regs[IDE_REGISTER_COMMAND]);
#endif
        }
        else
        {
            areq->type = ATA_COMMAND_TYPE_PIO_OUT;
            areq->regs.regs[IDE_REGISTER_COMMAND] = ATA_COMMAND_WRITE_SECTORS;
#if ATA_DEBUG
            ata_printf("ata_io_data_request: type: WRITE: %lu, %lu cmd:%02x\n",
                       req->bufs[0].block, req->bufnum,
                       areq->regs.regs[IDE_REGISTER_COMMAND]);
#endif
        }
    }

    /*
     * Fill position registers
     */
    if (ATA_DEV_INFO(ctrl_minor, dev).lba_avaible)
    {
        uint32_t start = req->bufs[0].block;
        areq->regs.regs[IDE_REGISTER_LBA0] = (uint8_t)start;
        areq->regs.regs[IDE_REGISTER_LBA1] = (uint8_t)(start >> 8);
        areq->regs.regs[IDE_REGISTER_LBA2] = (uint8_t)(start >> 16);
        /* Set as the head register write above */
        areq->regs.regs[IDE_REGISTER_LBA3] |= (uint8_t) (start >> 24);
        areq->regs.regs[IDE_REGISTER_LBA3] |= IDE_REGISTER_LBA3_L;
    }
    else
    {
        uint32_t   count = req->bufs[0].block;

        areq->regs.regs[IDE_REGISTER_SECTOR_NUMBER] =
                        (count % ATA_DEV_INFO(ctrl_minor, dev).sectors) + 1;

        /* now count = number of tracks: */
        count /= ATA_DEV_INFO(ctrl_minor, dev).sectors;
        areq->regs.regs[IDE_REGISTER_DEVICE_HEAD] |=
                          (count / ATA_DEV_INFO(ctrl_minor, dev).cylinders);

        /* now count = number of cylinders */
        count %= ATA_DEV_INFO(ctrl_minor, dev).cylinders;
        areq->regs.regs[IDE_REGISTER_CYLINDER_LOW] = (uint8_t)count;
        areq->regs.regs[IDE_REGISTER_CYLINDER_HIGH] = (uint8_t)(count >> 8);
        areq->regs.regs[IDE_REGISTER_DEVICE_HEAD] &=
                                                ~IDE_REGISTER_DEVICE_HEAD_L;
    }

    /*
     * Fill sector count register. We have a number of buffers (bufnum) which
     * can be of a specific length (bufs[0].length / ATA_SECTOR_SIZE).
     */
    areq->regs.regs[IDE_REGISTER_SECTOR_COUNT] =
      areq->breq->bufnum * (areq->breq->bufs[0].length / ATA_SECTOR_SIZE);

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
ata_non_data_request(dev_t device, uint32_t cmd, void *argp)
{
    rtems_status_code          rc;
    ata_req_t                 *areq;       /* ATA request */
    rtems_device_minor_number  rel_minor; /* relative minor which indexes
                                           * ata_devs array
                                           */
    rtems_device_minor_number  ctrl_minor;
    uint8_t                    dev;
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
            areq->regs.regs[IDE_REGISTER_SECTOR_COUNT] = *(uint8_t*)argp;
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
              /* invalid operation now */
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
    uint16_t         byte; /* emphasize that only 8 low bits is meaningful */
    ata_queue_msg_t  msg;
    uint8_t          i;
#if 0
    uint8_t          dev;
#endif
    uint16_t         val;
    ISR_Level        level;

    /* if no requests to controller then do nothing */
    if (rtems_chain_is_empty(&ata_ide_ctrls[ctrl_minor].reqs))
        return;

    /* get first request in the controller's queue */
    _ISR_Disable(level);
    areq = (ata_req_t *)rtems_chain_first(&ata_ide_ctrls[ctrl_minor].reqs);
    _ISR_Enable(level);

#if 0
    /* get ATA device identifier (0 or 1) */
    dev =  areq->regs.regs[IDE_REGISTER_DEVICE_HEAD] &
           IDE_REGISTER_DEVICE_HEAD_DEV;
#endif

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
        uint32_t   reg = (1 << i);
        if (areq->regs.to_write & reg)
            ide_controller_write_register(ctrl_minor, i, areq->regs.regs[i]);
    }

#if ATA_DEBUG
    ata_printf("ata_process_request: type: %d\n", areq->type);
#endif

    /* continue to execute ATA protocols depending on type of request */
    if (areq->type == ATA_COMMAND_TYPE_PIO_OUT)
    {
        do {
            ide_controller_read_register(ctrl_minor, IDE_REGISTER_STATUS,
                                         &byte);
        } while (byte & IDE_REGISTER_STATUS_BSY);

        if (byte & IDE_REGISTER_STATUS_DRQ)
        {
            if (areq->cnt)
            {
              int ccbuf = areq->cbuf;
              ide_controller_write_data_block(ctrl_minor,
                                              areq->breq->bufs[0].length * areq->cnt,
                                              areq->breq->bufs, &areq->cbuf,
                                              &areq->pos);
              ccbuf = areq->cbuf - ccbuf;
              areq->cnt -= ccbuf;
            }
        }
        else
        {
            if (IDE_Controller_Table[ctrl_minor].int_driven == false)
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

    if (IDE_Controller_Table[ctrl_minor].int_driven == false)
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
                 rtems_status_code status)
{
    assert(areq);

#if ATA_DEBUG
    ata_printf("ata_request_done: entry\n");
#endif

    ATA_EXEC_CALLBACK(areq, status);
    rtems_chain_extract(&areq->link);

    if (!rtems_chain_is_empty(&ata_ide_ctrls[ctrl_minor].reqs))
    {
        free(areq);
        ata_process_request(ctrl_minor);
        return;
    }

    free(areq);

#if ATA_DEBUG
    ata_printf("ata_request_done: exit\n");
#endif
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
                          rtems_status_code status, int info)
{
#if ATA_DEBUG
    ata_printf("ata_non_data_request_done: entry\n");
#endif

    areq->status = status;
    areq->info = info;
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
    PREEMPTION_KEY(key);

    DISABLE_PREEMPTION(key);

    rtems_chain_append(&ata_ide_ctrls[ctrl_minor].reqs, &areq->link);
    if (rtems_chain_has_only_one_node(&ata_ide_ctrls[ctrl_minor].reqs))
    {

        ata_queue_msg_t msg;

#if ATA_DEBUG_DOES_NOT_WORK_WITH_QEMU
	uint16_t      val;
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

    ENABLE_PREEMPTION(key);
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
#if CPU_SIMPLE_VECTORED_INTERRUPTS == TRUE
static rtems_isr ata_interrupt_handler(rtems_vector_number vec)
{
    rtems_chain_node *the_node = rtems_chain_first(&ata_int_vec[vec]);
    ata_queue_msg_t  msg;
    uint16_t         byte; /* emphasize that only 8 low bits is meaningful */

    for ( ; !rtems_chain_is_tail(&ata_int_vec[vec], the_node) ; )
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
#else
static void ata_interrupt_handler(rtems_irq_hdl_param handle)
{
  int ata_irq_chain_index = (int) handle;
    rtems_chain_node *the_node =
      rtems_chain_last(&ata_irq_chain[ata_irq_chain_index].irq_chain);
    ata_queue_msg_t  msg;
    uint16_t       byte; /* emphasize that only 8 low bits is meaningful */


    for ( ; !rtems_chain_is_tail(&ata_irq_chain[ata_irq_chain_index].irq_chain,
			   the_node) ; )
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

static void ata_interrupt_on(const rtems_irq_connect_data *ptr)
  {

    /* enable ATA device interrupt */
    ide_controller_write_register(0,
                                  IDE_REGISTER_DEVICE_CONTROL_OFFSET,
                                  0x00
                                 );
  }


static void ata_interrupt_off(const rtems_irq_connect_data *ptr)
  {

    /* disable ATA device interrupt */
    ide_controller_write_register(0,
                                  IDE_REGISTER_DEVICE_CONTROL_OFFSET,
                                  IDE_REGISTER_DEVICE_CONTROL_nIEN
                                 );
  }


static int ata_interrupt_isOn(const rtems_irq_connect_data *ptr)
  {
  uint16_t byte; /* emphasize that only 8 low bits is meaningful */

    /* return int. status od ATA device */
    ide_controller_read_register(0,
                                IDE_REGISTER_DEVICE_CONTROL_OFFSET,
                                &byte
                                );

    return !(byte & IDE_REGISTER_DEVICE_CONTROL_nIEN);
  }


static rtems_irq_connect_data ata_irq_data =
  {

    0, /* filled out before use... */
    ata_interrupt_handler,/* filled out before use... */
    NULL,
    ata_interrupt_on,
    ata_interrupt_off,
    ata_interrupt_isOn
  };
#endif

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
    uint16_t        val;
#if 0
    uint8_t         dev;
#endif
    ata_queue_msg_t msg;

#if 0
    dev =  areq->regs.regs[IDE_REGISTER_DEVICE_HEAD] &
           IDE_REGISTER_DEVICE_HEAD_DEV;
#endif

    if (areq->cnt)
    {
      int ccbuf = areq->cbuf;
      ide_controller_read_data_block(ctrl_minor,
                                     areq->breq->bufs[0].length * areq->cnt,
                                     areq->breq->bufs, &areq->cbuf, &areq->pos);
      ccbuf = areq->cbuf - ccbuf;
      areq->cnt -= ccbuf;
    }

    if (areq->cnt == 0)
    {
        ata_request_done(areq, ctrl_minor, RTEMS_SUCCESSFUL);
    }
    else if (IDE_Controller_Table[ctrl_minor].int_driven == false)
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
    uint16_t        val;
#if 0
    uint8_t         dev;
#endif
    ata_queue_msg_t msg;

#if ATA_DEBUG
    ata_printf("ata_pio_out_protocol:\n");
#endif

#if 0
    dev =  areq->regs.regs[IDE_REGISTER_DEVICE_HEAD] &
           IDE_REGISTER_DEVICE_HEAD_DEV;
#endif

    if (areq->cnt == 0)
    {
        ata_request_done(areq, ctrl_minor, RTEMS_SUCCESSFUL);
    }
    else
    {
        if (areq->cnt)
        {
          int ccbuf = areq->cbuf;
          ide_controller_write_data_block(ctrl_minor,
                                          areq->breq->bufs[0].length * areq->cnt,
                                          areq->breq->bufs, &areq->cbuf,
                                          &areq->pos);
          ccbuf = areq->cbuf - ccbuf;
          areq->cnt -= ccbuf;
        }
        if (IDE_Controller_Table[ctrl_minor].int_driven == false)
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
    size_t                     size;
    ata_req_t                 *areq;
    rtems_device_minor_number  ctrl_minor;
    uint16_t                   val;
    uint16_t                   val1;
    rtems_status_code          rc;
    ISR_Level                  level;

    PREEMPTION_KEY(key);

    DISABLE_PREEMPTION(key);

    while (1)
    {
        ENABLE_PREEMPTION(key);

        /* get event which has happend */
        rc = rtems_message_queue_receive(ata_queue_id, &msg, &size, RTEMS_WAIT,
                                         RTEMS_NO_TIMEOUT);
        if (rc != RTEMS_SUCCESSFUL)
            rtems_fatal_error_occurred(RTEMS_INTERNAL_ERROR);

        /* get controller on which event has happend */
        ctrl_minor = msg.ctrl_minor;

        DISABLE_PREEMPTION(key);

        /* get current request to the controller */
        _ISR_Disable(level);
        areq = (ata_req_t *)rtems_chain_first(&ata_ide_ctrls[ctrl_minor].reqs);
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
                ata_request_done(areq, ctrl_minor, RTEMS_SUCCESSFUL);
                break;

            case ATA_MSG_ERROR_EVT:
                /*
                 * finish processing of current request with error
                 * status and start processing of the next request in the
                 * controller queue
                 */
                ata_request_done(areq, ctrl_minor, RTEMS_IO_ERROR);
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
                    if (val & (IDE_REGISTER_ERROR_UNC |
                               IDE_REGISTER_ERROR_ICRC |
                               IDE_REGISTER_ERROR_IDNF |
                               IDE_REGISTER_ERROR_NM |
                               IDE_REGISTER_ERROR_MED))
                    {
                        if (areq->type == ATA_COMMAND_TYPE_NON_DATA)
                            ata_non_data_request_done(areq, ctrl_minor,
                                                      RTEMS_UNSATISFIED,
                                                      RTEMS_IO_ERROR);
                        else
                            ata_request_done(areq, ctrl_minor, RTEMS_IO_ERROR);
                        break;
                    }
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
#if ATA_DEBUG
                        ata_printf("ata_queue_task: non-supported command type\n");
#endif
                        ata_request_done(areq, ctrl_minor, RTEMS_IO_ERROR);
                        break;
                }
                break;

            default:
#if ATA_DEBUG
                ata_printf("ata_queue_task: internal error\n");
                rtems_task_delete (RTEMS_SELF);
#endif
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
static int
ata_ioctl(rtems_disk_device *dd, uint32_t cmd, void *argp)
{
    dev_t                     device = rtems_disk_get_device_identifier(dd);
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
        case RTEMS_BLKIO_REQUEST:
            status = ata_io_data_request(device, (rtems_blkdev_request *)argp);
            break;

        case ATAIO_SET_MULTIPLE_MODE:
            status = ata_non_data_request(device, cmd, argp);
            break;

        case RTEMS_BLKIO_CAPABILITIES:
            *((uint32_t*) argp)  = RTEMS_BLKDEV_CAP_MULTISECTOR_CONT;
            status = RTEMS_SUCCESSFUL;
            break;

        default:
            return rtems_blkdev_ioctl (dd, cmd, argp);
            break;
    }

    if (status != RTEMS_SUCCESSFUL)
    {
        errno = EIO;
        return -1;
    }
    return 0;
}

static void ata_execute_device_diagnostic(
  rtems_device_minor_number ctrl_minor,
  uint16_t *sector_buffer
)
{
#if ATA_EXEC_DEVICE_DIAGNOSTIC
  ata_req_t areq;
  blkdev_request1 breq;

  ata_breq_init(&breq, sector_buffer);

  /*
   * Issue EXECUTE DEVICE DIAGNOSTIC ATA command for explore is
   * there any ATA device on the controller.
   *
   * This command may fail and it assumes we have a master device and may
   * be a slave device. I think the identify command will handle
   * detection better than this method.
   */
  memset(&areq, 0, sizeof(ata_req_t));
  areq.type = ATA_COMMAND_TYPE_NON_DATA;
  areq.regs.to_write = ATA_REGISTERS_VALUE(IDE_REGISTER_COMMAND);
  areq.regs.regs[IDE_REGISTER_COMMAND] =
                            ATA_COMMAND_EXECUTE_DEVICE_DIAGNOSTIC;
  areq.regs.to_read = ATA_REGISTERS_VALUE(IDE_REGISTER_ERROR);

  areq.breq = (rtems_blkdev_request *)&breq;

  /*
   * Process the request. Special processing of requests on
   * initialization phase is needed because at this moment there
   * is no multitasking enviroment
   */
  ata_process_request_on_init_phase(ctrl_minor, &areq);

  /*
   * check status of I/O operation
   */
  if (breq.req.status == RTEMS_SUCCESSFUL)
  {
    /* disassemble returned diagnostic codes */
    if (areq.info == ATA_DEV0_PASSED_DEV1_PASSED_OR_NOT_PRSNT)
    {
      printk("ATA: ctrl:%d: primary, secondary\n", ctrl_minor);
      ATA_DEV_INFO(ctrl_minor,0).present = true;
      ATA_DEV_INFO(ctrl_minor,1).present = true;
    }
    else if (areq.info == ATA_DEV0_PASSED_DEV1_FAILED)
    {
      printk("ATA: ctrl:%d: primary\n", ctrl_minor);
      ATA_DEV_INFO(ctrl_minor,0).present = true;
      ATA_DEV_INFO(ctrl_minor,1).present = false;
    }
    else if (areq.info < ATA_DEV1_PASSED_DEV0_FAILED)
    {
      printk("ATA: ctrl:%d: secondary\n", ctrl_minor);
      ATA_DEV_INFO(ctrl_minor,0).present = false;
      ATA_DEV_INFO(ctrl_minor,1).present = true;
    }
    else
    {
      printk("ATA: ctrl:%d: none\n", ctrl_minor);
      ATA_DEV_INFO(ctrl_minor, 0).present = false;
      ATA_DEV_INFO(ctrl_minor, 1).present = false;
    }

    /* refine the returned codes */
    if (ATA_DEV_INFO(ctrl_minor, 1).present)
    {
      uint16_t ec = 0;
      ide_controller_read_register(ctrl_minor, IDE_REGISTER_ERROR, &ec);
      if (ec & ATA_DEV1_PASSED_DEV0_FAILED)
      {
        printk("ATA: ctrl:%d: secondary inforced\n", ctrl_minor);
        ATA_DEV_INFO(ctrl_minor, 1).present = true;
      }
      else
      {
        printk("ATA: ctrl:%d: secondary removed\n", ctrl_minor);
        ATA_DEV_INFO(ctrl_minor, 1).present = false;
      }
    }
  }
  else
#endif
  {
    ATA_DEV_INFO(ctrl_minor, 0).present = true;
    ATA_DEV_INFO(ctrl_minor,1).present = true;
  }
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
rtems_ata_initialize(rtems_device_major_number major,
                     rtems_device_minor_number minor_arg,
                     void *args)
{
    uint32_t           ctrl_minor;
    rtems_status_code  status;
    uint16_t          *buffer;
    int                i, dev = 0;
    char               name[ATA_MAX_NAME_LENGTH];
    dev_t              device;
    ata_int_st_t      *int_st;

#if CPU_SIMPLE_VECTORED_INTERRUPTS == TRUE
    rtems_isr_entry    old_isr;
#else
    int ata_irq_chain_use;
#endif

    if (ata_initialized)
        return RTEMS_SUCCESSFUL;

    /* initialization of disk devices library */
    status = rtems_disk_io_initialize();
    if (status != RTEMS_SUCCESSFUL)
        return status;

#ifdef SAFE
#ifdef SAFE_MUTEX
    status = rtems_semaphore_create (rtems_build_name ('A', 'T', 'A', 'L'),
                                     1, RTEMS_ATA_LOCK_ATTRIBS, 0,
                                     &ata_lock);
    if (status != RTEMS_SUCCESSFUL)
        return status;
#endif
#endif

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
                 ((rtems_ata_driver_task_priority > 0)
		  ? rtems_ata_driver_task_priority
		  : ATA_DRIVER_TASK_DEFAULT_PRIORITY),
                 ATA_DRIVER_TASK_STACK_SIZE,
                 RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_NO_ASR |
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

    buffer = (uint16_t*)malloc(ATA_SECTOR_SIZE);
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

#if CPU_SIMPLE_VECTORED_INTERRUPTS == TRUE
    /* prepare ATA driver for handling  interrupt driven devices */
    for (i = 0; i < ATA_MAX_RTEMS_INT_VEC_NUMBER; i++)
        rtems_chain_initialize_empty(&ata_int_vec[i]);
#else
    for (i = 0; i < ATA_IRQ_CHAIN_MAX_CNT; i++) {
      rtems_chain_initialize_empty(&(ata_irq_chain[i].irq_chain));
    }
#endif

    /*
     * during ATA driver initialization EXECUTE DEVICE DIAGNOSTIC and
     * IDENTIFY DEVICE ATA command should be issued; for these purposes ATA
     * requests should be formed; ATA requests contain block device request,
     * so form block device request first
     */

    /*
     * for each presented IDE controller execute EXECUTE DEVICE DIAGNOSTIC
     * ATA command; for each found device execute IDENTIFY DEVICE ATA
     * command
     */
    for (ctrl_minor = 0; ctrl_minor < IDE_Controller_Count; ctrl_minor++)
    if (IDE_Controller_Table[ctrl_minor].status == IDE_CTRL_INITIALIZED)
    {
        rtems_chain_initialize_empty(&ata_ide_ctrls[ctrl_minor].reqs);

        if (IDE_Controller_Table[ctrl_minor].int_driven == true)
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
#if CPU_SIMPLE_VECTORED_INTERRUPTS == TRUE
            status = rtems_interrupt_catch(
                         ata_interrupt_handler,
                         IDE_Controller_Table[ctrl_minor].int_vec,
                         &old_isr);
#else
	    /*
	     * FIXME: check existing entries. if they use the same
	     * IRQ name, then append int_st to respective chain
	     * otherwise, use new ata_irq_chain entry
	     */
	    ata_irq_chain_use = -1;
	    for (i = 0;
		 ((i < ata_irq_chain_cnt) &&
		  (ata_irq_chain_use < 0));i++) {
	      if (ata_irq_chain[i].name ==
		  IDE_Controller_Table[ctrl_minor].int_vec) {
		ata_irq_chain_use = i;
	      }
	    }
	    if (ata_irq_chain_use < 0) {
	      /*
	       * no match found, try to use new channel entry
	       */
	      if (ata_irq_chain_cnt < ATA_IRQ_CHAIN_MAX_CNT) {
		ata_irq_chain_use = ata_irq_chain_cnt++;

		ata_irq_chain[ata_irq_chain_use].name =
		  IDE_Controller_Table[ctrl_minor].int_vec;
		ata_irq_data.name   =
		  IDE_Controller_Table[ctrl_minor].int_vec;
		ata_irq_data.hdl    = ata_interrupt_handler;
		ata_irq_data.handle = (rtems_irq_hdl_param) ctrl_minor;

		status = ((0 == BSP_install_rtems_irq_handler(&ata_irq_data))
			  ? RTEMS_INVALID_NUMBER
			  : RTEMS_SUCCESSFUL);
	      }
	      else {
		status = RTEMS_TOO_MANY;
	      }
	    }
#endif
            if (status != RTEMS_SUCCESSFUL)
            {
                free(int_st);
                free(buffer);
                rtems_task_delete(ata_task_id);
                rtems_message_queue_delete(ata_queue_id);
                rtems_disk_io_done();
                return status;
            }
#if CPU_SIMPLE_VECTORED_INTERRUPTS == TRUE
            rtems_chain_append(
                &ata_int_vec[IDE_Controller_Table[ctrl_minor].int_vec],
                &int_st->link);
#else
            rtems_chain_append(
		&(ata_irq_chain[ata_irq_chain_use].irq_chain),
                &int_st->link);
#endif

            /* disable interrupts */
            ide_controller_write_register(ctrl_minor,
                                          IDE_REGISTER_DEVICE_CONTROL_OFFSET,
                                          IDE_REGISTER_DEVICE_CONTROL_nIEN);
        }

        ata_execute_device_diagnostic(ctrl_minor, buffer);

        /* for each found ATA device obtain it configuration */
        for (dev = 0; dev < 2; dev++)
        if (ATA_DEV_INFO(ctrl_minor, dev).present)
        {
            status = ata_identify_device(
               ctrl_minor,
               dev,
               buffer,
               &ATA_DEV_INFO(ctrl_minor, dev));
            if (status != RTEMS_SUCCESSFUL)
               continue;

	    /*
	     * choose most appropriate ATA device data I/O speed supported
	     * by the controller
	     */
	    status = ide_controller_config_io_speed(
               ctrl_minor,
               ATA_DEV_INFO(ctrl_minor, dev).modes_available);
	    if (status != RTEMS_SUCCESSFUL)
               continue;

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
                ata_ioctl, NULL, name);
            if (status != RTEMS_SUCCESSFUL)
            {
                ata_devs[ata_devs_number].device = ATA_UNDEFINED_VALUE;
                continue;
            }
            ata_devs_number++;
        }
        if (IDE_Controller_Table[ctrl_minor].int_driven == true)
        {
            ide_controller_write_register(ctrl_minor,
                                          IDE_REGISTER_DEVICE_CONTROL_OFFSET,
                                          0x00);
        }
    }

    free(buffer);
    ata_initialized = true;
    return RTEMS_SUCCESSFUL;
}

/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CANBus
 *
 * @brief Controller Area Network (CAN) Bus Implementation
 *
 */

/*
 * Copyright (C) 2022 Prashanth S (fishesprashanth@gmail.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>

#include <rtems/imfs.h>
#include <rtems/thread.h>

#include <dev/can/canqueueimpl.h>
#include <dev/can/can.h>

#define can_interrupt_lock_acquire(bus) \
                      do {              \
                           CAN_DEBUG_LOCK("acquiring lock\n");    \
                           real_can_interrupt_lock_acquire(bus);  \
                         } while (0);

#define can_interrupt_lock_release(bus) \
                      do {              \
                           CAN_DEBUG_LOCK("releasing lock\n");    \
                           real_can_interrupt_lock_release(bus);  \
                         } while (0);

static int
can_bus_open(rtems_libio_t *iop, const char *path, int oflag, mode_t mode);
static ssize_t 
can_bus_read(rtems_libio_t *iop, void *buffer, size_t count);
static ssize_t 
can_bus_write(rtems_libio_t *iop, const void *buffer, size_t count);
static int
can_bus_ioctl(rtems_libio_t *iop, ioctl_command_t request, void *buffer);

static int can_xmit(struct can_bus *bus);

static int can_bus_create_sem(struct can_bus *);
static int try_sem(struct can_bus *);
static int take_sem(struct can_bus *);
static int give_sem(struct can_bus *);


static void can_bus_obtain(can_bus *bus)
{
  CAN_DEBUG("can_bus_obtain Entry\n");
  rtems_mutex_lock(&bus->mutex);
  CAN_DEBUG("can_bus_obtain Exit\n");
}

static void can_bus_release(can_bus *bus)
{
  CAN_DEBUG("can_bus_release Entry\n");
  rtems_mutex_unlock(&bus->mutex);
  CAN_DEBUG("can_bus_release Exit\n");
}

static void can_bus_destroy_mutex(struct can_bus *bus)
{
  rtems_mutex_destroy(&bus->mutex);
}

static int can_bus_create_sem(struct can_bus *bus)
{
  int ret = 0;

  ret = rtems_semaphore_create(rtems_build_name('c', 'a', 'n', bus->index), 
      CAN_TX_BUF_COUNT, RTEMS_FIFO | RTEMS_COUNTING_SEMAPHORE | RTEMS_LOCAL, 
      0, &bus->tx_fifo_sem_id);

  if (ret != 0) {
    CAN_ERR("can_create_sem: rtems_semaphore_create failed %d\n", ret);
  }

  return ret;
}

static void can_bus_free_tx_semaphore(struct can_bus *bus)
{
  rtems_semaphore_delete(bus->tx_fifo_sem_id);
}

static void real_can_interrupt_lock_acquire(struct can_bus *bus)
{
  bus->can_dev_ops->dev_int(bus->priv, false);
  can_bus_obtain(bus);
}

static void real_can_interrupt_lock_release(struct can_bus *bus)
{
  can_bus_release(bus);
  bus->can_dev_ops->dev_int(bus->priv, true);
}

static int take_sem(struct can_bus *bus)
{
  int ret = rtems_semaphore_obtain(bus->tx_fifo_sem_id, RTEMS_WAIT, 
                                RTEMS_NO_TIMEOUT);
#ifdef CAN_DEBUG_LOCK
  if (ret == RTEMS_SUCCESSFUL) {
    bus->sem_count++;
    CAN_DEBUG_LOCK("take_sem: Semaphore count = %d\n", bus->sem_count);
    if (bus->sem_count > CAN_TX_BUF_COUNT) {
      CAN_ERR("take_sem error: sem_count is misleading\n");
      return RTEMS_INTERNAL_ERROR;
    }
  }
#endif /* CAN_DEBUG_LOCK */

  return ret;
}

static int give_sem(struct can_bus *bus)
{
  int ret = rtems_semaphore_release(bus->tx_fifo_sem_id);

#ifdef CAN_DEBUG_LOCK
  if (ret == RTEMS_SUCCESSFUL) {
    bus->sem_count--;
    CAN_DEBUG_LOCK("give_sem: Semaphore count = %d\n", bus->sem_count);
    if (bus->sem_count < 0) {
      CAN_ERR("give_sem error: sem_count is misleading\n");
      return RTEMS_INTERNAL_ERROR;
    }
  }
#endif /* CAN_DEBUG_LOCK */

  return ret;
}

static int try_sem(struct can_bus *bus)
{
  int ret = rtems_semaphore_obtain(bus->tx_fifo_sem_id, RTEMS_NO_WAIT, 
                                RTEMS_NO_TIMEOUT);
#ifdef CAN_DEBUG_LOCK
  if (ret == RTEMS_SUCCESSFUL) {
    bus->sem_count++;
    CAN_DEBUG_LOCK("try_sem: Semaphore count = %d\n", bus->sem_count);
    if (bus->sem_count > CAN_TX_BUF_COUNT) {
      CAN_ERR("take_sem error: sem_count is misleading\n");
      return RTEMS_INTERNAL_ERROR;
    }
  }
#endif /* CAN_DEBUG_LOCK */

  return ret;
}

static int
can_bus_open(rtems_libio_t *iop, const char *path, int oflag, mode_t mode)
{
  CAN_DEBUG("can_bus_open\n");

  return 0;
} 

/* Should be called only with CAN interrupts disabled */
int can_receive(struct can_bus *bus, struct can_msg *msg)
{
  memcpy(&bus->can_rx_msg, msg, CAN_MSG_LEN(msg));

  return CAN_MSG_LEN(msg);
}

/* FIXME: Should be modified to read count number of bytes, Now sending one can_msg */
static ssize_t can_bus_read(rtems_libio_t *iop, void *buffer, size_t count)
{
  int len;

  can_bus *bus = IMFS_generic_get_context_by_iop(iop);
  if (bus == NULL) {
    return -RTEMS_NOT_DEFINED;
  }

  struct can_msg *msg = (struct can_msg *)buffer;

  len = CAN_MSG_LEN(&bus->can_rx_msg);

  if (count < len) {
    CAN_DEBUG("can_bus_read: buffer size is small min "
              "sizeof(struct can_msg) = %" PRIuPTR "\n",
                    sizeof(struct can_msg));
    return -RTEMS_INVALID_SIZE;
  }

  can_interrupt_lock_acquire(bus);
  memcpy(msg, &bus->can_rx_msg, len);
  can_interrupt_lock_release(bus);

  return len;
}

/* This function is a critical section and should be called 
 * with CAN interrupts disabled and mutually exclusive
 */
static int can_xmit(struct can_bus *bus)
{
  int ret = RTEMS_SUCCESSFUL;

  struct can_msg *msg = NULL;

  CAN_DEBUG(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> can_xmit Entry\n");

  while (1) {
    CAN_DEBUG("can_dev_ops->dev_tx_ready\n");
    if (bus->can_dev_ops->dev_tx_ready(bus->priv) != true) {
      break;
    }

    CAN_DEBUG("can_tx_get_data_buf\n");
    msg = can_bus_tx_get_data_buf(bus);
    if (msg == NULL) {
      break;
    }

    CAN_DEBUG("can_dev_ops->dev_tx\n");
    ret = bus->can_dev_ops->dev_tx(bus->priv, msg);
    if (ret != RTEMS_SUCCESSFUL) {
        CAN_ERR("can_xmit: dev_send failed\n");
        break;
    }

    ret = give_sem(bus);
    if (ret != RTEMS_SUCCESSFUL) {
      CAN_ERR("can_xmit: rtems_semaphore_release failed = %d\n", ret);
      break;
    }
  }

  CAN_DEBUG(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> can_xmit Exit\n");

  return ret;
}

void can_print_msg(struct can_msg const *msg)
{
#ifdef CAN_DEBUG
  CAN_DEBUG("\n----------------------------------------------------------------\n");
  CAN_DEBUG("id = %d len = %d flags = 0x%08X\n", msg->id, msg->len, msg->flags);

  CAN_DEBUG("msg->data[0] = 0x%08x\n", ((uint32_t *)msg->data)[0]);
  CAN_DEBUG("msg->data[1] = 0x%08x\n", ((uint32_t *)msg->data)[1]);

  for (int i = 0; i < msg->len; i++) {
    CAN_DEBUG("%02x\n", ((char *)msg->data)[i]);
  }

  CAN_DEBUG("\n-----------------------------------------------------------------\n");
#endif /* CAN_DEBUG */
}

/* can_tx_done should be called only with CAN interrupts disabled */
int can_tx_done(struct can_bus *bus)
{
  CAN_DEBUG("------------ can_tx_done Entry\n");
  int ret = RTEMS_SUCCESSFUL;

  if (bus->can_dev_ops->dev_tx_ready(bus->priv) == true) {
    ret = can_xmit(bus);
  }
  CAN_DEBUG("------------ can_tx_done Exit\n");
  
  return ret;
}

/* FIXME: Add support to send multiple CAN msgs for can_bus_write */
static ssize_t 
can_bus_write(rtems_libio_t *iop, const void *buffer, size_t count)
{
  can_bus *bus = IMFS_generic_get_context_by_iop(iop);

  if (bus == NULL || bus->can_dev_ops->dev_tx == NULL) {
    return -RTEMS_NOT_DEFINED;
  }

  int ret = RTEMS_SUCCESSFUL;

  struct can_msg const *msg = buffer;
  struct can_msg *fifo_buf = NULL;

  uint32_t msg_size = CAN_MSG_LEN(msg);

  CAN_DEBUG_TX("can_bus_write: can_msg_size = %u\n", msg_size);
  if (msg_size > sizeof(struct can_msg)) {
    CAN_ERR("can_bus_write: can message "
            "len error msg_size = %u struct can_msg = %" PRIxPTR "\n",
          msg_size, sizeof(struct can_msg));
    return -RTEMS_INVALID_SIZE;
  }

  if ((iop->flags & O_NONBLOCK) != 0) {
    ret = try_sem(bus);
    if (ret != RTEMS_SUCCESSFUL) {
      return -ret;
    }
  } else {
    ret = take_sem(bus);
    if (ret != RTEMS_SUCCESSFUL) {
      CAN_ERR("can_bus_write: cannot take semaphore\n");
      return -ret;
    }
  }

  can_interrupt_lock_acquire(bus);

  fifo_buf = can_bus_tx_get_empty_buf(bus);
  if (fifo_buf == NULL) {
    /* This error will not happen until buffer counts are not synchronized */
    CAN_ERR("can_bus_write: Buffer counts are not synchronized with semaphore count\n");
    ret = -RTEMS_INTERNAL_ERROR;
    goto release_lock_and_return;
  }

  CAN_DEBUG_TX("can_bus_write: empty_count = %u\n", bus->tx_fifo.empty_count);

  CAN_DEBUG_TX("can_bus_write: copying msg from application to driver buffer\n");
  memcpy(fifo_buf, msg, msg_size);

  if (bus->can_dev_ops->dev_tx_ready(bus->priv) == true) {
    ret = can_xmit(bus);
    if (ret != RTEMS_SUCCESSFUL) {
      ret = -ret;
      goto release_lock_and_return;
    }
  }

  ret = msg_size;

release_lock_and_return:
  can_interrupt_lock_release(bus);
  return ret;
}

static int
can_bus_ioctl(rtems_libio_t *iop, ioctl_command_t request, void *buffer)
{
  can_bus *bus = IMFS_generic_get_context_by_iop(iop);

  if (bus == NULL || bus->can_dev_ops->dev_ioctl == NULL) {
    return -RTEMS_NOT_DEFINED;
  }

  can_bus_obtain(bus);

  bus->can_dev_ops->dev_ioctl(bus->priv, NULL, 0);

  can_bus_release(bus);

  return RTEMS_SUCCESSFUL;
}

static const rtems_filesystem_file_handlers_r can_bus_handler = {
  .open_h = can_bus_open,
  .close_h = rtems_filesystem_default_close,
  .read_h = can_bus_read,
  .write_h = can_bus_write,
  .ioctl_h = can_bus_ioctl,
  .lseek_h = rtems_filesystem_default_lseek,
  .fstat_h = IMFS_stat,
  .ftruncate_h = rtems_filesystem_default_ftruncate,
  .fsync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fdatasync_h = rtems_filesystem_default_fsync_or_fdatasync,
  .fcntl_h = rtems_filesystem_default_fcntl,
  .kqfilter_h = rtems_filesystem_default_kqfilter,
  .mmap_h = rtems_filesystem_default_mmap,
  .poll_h = rtems_filesystem_default_poll,
  .readv_h = rtems_filesystem_default_readv,
  .writev_h = rtems_filesystem_default_writev
};

static void can_bus_node_destroy(IMFS_jnode_t *node)
{
  can_bus *bus;

  bus = IMFS_generic_get_context_by_node(node);
  (*bus->destroy)(bus);

  IMFS_node_destroy_default(node);
}

static const 
IMFS_node_control can_bus_node_control = IMFS_GENERIC_INITIALIZER(&can_bus_handler, 
                    IMFS_node_initialize_generic, can_bus_node_destroy);

rtems_status_code can_bus_register(can_bus *bus, const char *bus_path)
{
  int ret = RTEMS_SUCCESSFUL;

  static uint8_t index = 0;

  if (index == CAN_BUS_REG_MAX) {
    CAN_ERR("can_bus_register: can bus registration limit reached\n");
    return RTEMS_TOO_MANY;
  }

  bus->index = index++;
  CAN_DEBUG("Registering CAN bus index = %u\n", bus->index);

  ret = IMFS_make_generic_node(bus_path, S_IFCHR | S_IRWXU | S_IRWXG | S_IRWXO,
                              &can_bus_node_control, bus);
  if (ret != RTEMS_SUCCESSFUL) {
    CAN_ERR("can_bus_register: Creating node failed = %d\n", ret);
    goto fail;
  }

  if ((ret = can_bus_create_sem(bus)) != RTEMS_SUCCESSFUL) {
    CAN_ERR("can_bus_register: can_create_sem failed = %d\n", ret);
    goto fail;
  }

  if ((ret = can_bus_create_tx_buffers(bus)) != RTEMS_SUCCESSFUL) {
    CAN_ERR("can_bus_register: can_create_tx_buffers failed = %d\n", ret);
    goto free_tx_semaphore;
  }

  return ret;

free_tx_semaphore:
  rtems_semaphore_delete(bus->tx_fifo_sem_id);

fail:
  can_bus_destroy_mutex(bus);
  return ret;

}

static void can_bus_destroy(can_bus *bus)
{
  can_bus_free_tx_buffers(bus);
  can_bus_free_tx_semaphore(bus);
  can_bus_destroy_mutex(bus);
}

static int can_bus_do_init(can_bus *bus, void (*destroy)(can_bus *bus))
{
  rtems_mutex_init(&bus->mutex, "CAN Bus");
  bus->destroy = can_bus_destroy;

  return RTEMS_SUCCESSFUL;
}

static void can_bus_destroy_and_free(can_bus *bus)
{
  can_bus_destroy(bus);
  free(bus);
}

int can_bus_init(can_bus *bus)
{
  memset(bus, 0, sizeof(*bus));

  return can_bus_do_init(bus, can_bus_destroy);
}

can_bus *can_bus_alloc_and_init(size_t size)
{
  can_bus *bus = NULL;

  if (size >= sizeof(*bus)) {
    bus = calloc(1, size);
    if (bus == NULL) {
      CAN_ERR("can_bus_alloc_and_init: calloc failed\b");
      return NULL;
    }

    int rv = can_bus_do_init(bus, can_bus_destroy_and_free);
    if (rv != 0) {
      CAN_ERR("can_bus_alloc_and_init: can_bus_do_init failed\n");
      return NULL;
    }
  }
  return bus;
}

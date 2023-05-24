/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2022 On-Line Applications Research Corporation (OAR)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
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

#include <string.h>

#include <bsp/irq.h>
#include <bsp/jffs2_xqspipsu.h>
#include <rtems/jffs2.h>
#include <rtems/libio.h>
#include <xqspipsu-flash-helper.h>

typedef struct {
  rtems_jffs2_flash_control super;
  XQspiPsu *qspipsu;
} flash_control;

#define FLASH_DEVICE_ID 0xbb20 /* Type: 0xbb, Capacity: 0x20 */

static flash_control *get_flash_control( rtems_jffs2_flash_control *super )
{
  return (flash_control *) super;
}

static int do_read(
  rtems_jffs2_flash_control *super,
  uint32_t offset,
  unsigned char *buffer,
  size_t size_of_buffer
)
{
  int Status;

  flash_control *self = get_flash_control( super );
  XQspiPsu *QspiPsuPtr = self->qspipsu;
  u8* ReadBuffer = NULL;

  Status = QspiPsu_NOR_Read(
    QspiPsuPtr,
    offset,
    size_of_buffer,
    &ReadBuffer
  );
  if ( Status != XST_SUCCESS ) {
    return Status;
  }

  /*
   * We have to copy since we can't be sure that buffer is properly aligned.
   */
  memcpy( buffer, ReadBuffer, size_of_buffer );

  return 0;
}

static int do_write(
  rtems_jffs2_flash_control *super,
  uint32_t offset,
  const unsigned char *buffer,
  size_t size_of_buffer
)
{
  int Status;

  flash_control *self = get_flash_control( super );
  XQspiPsu *QspiPsuPtr = self->qspipsu;

  Status = QspiPsu_NOR_Write(
    QspiPsuPtr,
    offset,
    size_of_buffer,
    (unsigned char *) buffer
  );
  if ( Status != XST_SUCCESS ) {
    return Status;
  }

  return 0;
}

static int do_erase(
  rtems_jffs2_flash_control *super,
  uint32_t offset
)
{
  int Status;

  flash_control *self = get_flash_control( super );
  XQspiPsu *QspiPsuPtr = self->qspipsu;

  Status = QspiPsu_NOR_Erase(
    QspiPsuPtr,
    offset,
    super->block_size
  );
  if ( Status != XST_SUCCESS ) {
    return Status;
  }

  return 0;
}

static void do_destroy( rtems_jffs2_flash_control *super )
{
  flash_control *self = get_flash_control( super );

  rtems_interrupt_handler_remove(
    ZYNQMP_IRQ_QSPI,
    (rtems_interrupt_handler) XQspiPsu_InterruptHandler,
    self->qspipsu
  );
}

static flash_control flash_instance = {
  .super = {
    .read              = do_read,
    .write             = do_write,
    .erase             = do_erase,
    .destroy           = do_destroy,
    .device_identifier = FLASH_DEVICE_ID
  }
};

static rtems_jffs2_mount_data mount_data = {
  .flash_control      = &flash_instance.super,
  .compressor_control = NULL
};

int xilinx_zynqmp_nor_jffs2_initialize(
  const char *mount_dir,
  XQspiPsu *qspipsu_ptr
)
{
  int rv = 0;

  flash_instance.qspipsu = qspipsu_ptr;

  rv = QspiPsu_NOR_Initialize(
    flash_instance.qspipsu,
    ZYNQMP_IRQ_QSPI
  );
  if ( rv != 0 ) {
    return rv;
  }

  uint32_t sect_size = QspiPsu_NOR_Get_Sector_Size(qspipsu_ptr);
  uint32_t flash_size = QspiPsu_NOR_Get_Device_Size(qspipsu_ptr);
  flash_instance.super.flash_size = flash_size;
  flash_instance.super.block_size = sect_size;

  rv = mount(
    NULL,
    mount_dir,
    RTEMS_FILESYSTEM_TYPE_JFFS2,
    RTEMS_FILESYSTEM_READ_WRITE,
    &mount_data
  );
  if ( rv != 0 ) {
    return rv;
  }

  return 0;
}

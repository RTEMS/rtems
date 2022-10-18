/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsMicroblaze
 *
 * @brief MicroBlaze AXI QSPI JFFS2 flash driver implementation
 */

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

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <bspopts.h>
#include <dev/spi/xilinx-axi-spi.h>
#include <linux/spi/spidev.h>
#include <rtems/jffs2.h>
#include <rtems/libio.h>

#include <bsp.h>
#include <bsp/jffs2_qspi.h>

#define BLOCK_SIZE (64UL * 1024UL)
#define FLASH_SIZE (32UL * BLOCK_SIZE)
#define FLASH_PAGE_SIZE        256
#define FLASH_NUM_CS           2
#define FLASH_DEVICE_ID        0xbb19 /* Type: 0xbb, Capacity: 0x19 */
#define BUS_PATH          "/dev/spi-0"
#define FLASH_MOUNT_POINT "/mnt"

#define READ_WRITE_EXTRA_BYTES 4
#define WRITE_ENABLE_BYTES     1
#define SECTOR_ERASE_BYTES     4

#define COMMAND_QUAD_WRITE     0x32
#define COMMAND_SECTOR_ERASE   0xD8
#define COMMAND_QUAD_READ      0x6B
#define COMMAND_STATUSREG_READ 0x05
#define COMMAND_WRITE_ENABLE   0x06
#define FLASH_SR_IS_READY_MASK 0x01

typedef struct {
  rtems_jffs2_flash_control super;
  int                       fd;
} flash_control;

static uint8_t ReadBuffer[FLASH_PAGE_SIZE + READ_WRITE_EXTRA_BYTES + 4];
static uint8_t WriteBuffer[FLASH_PAGE_SIZE + READ_WRITE_EXTRA_BYTES + 4];

static flash_control *get_flash_control( rtems_jffs2_flash_control *super )
{
  return (flash_control *) super;
}

static int flash_wait_for_ready( flash_control *self )
{
  uint8_t rv     = 0;
  uint8_t status = 0;

  WriteBuffer[0] = COMMAND_STATUSREG_READ;

  struct spi_ioc_transfer mesg = {
    .tx_buf        = WriteBuffer,
    .rx_buf        = ReadBuffer,
    .len           = 2,
    .bits_per_word = 8,
    .cs            = 0
  };

  do {
    rv = ioctl( self->fd, SPI_IOC_MESSAGE( 1 ), &mesg );
    if ( rv != 0 ) {
      return -EIO;
    }

    status = ReadBuffer[1];
  } while ( (status & FLASH_SR_IS_READY_MASK) != 0 );

  return 0;
}

static int flash_write_enable( flash_control *self )
{
  uint8_t rv = 0;

  rv = flash_wait_for_ready( self );
  if ( rv != 0 ) {
    return rv;
  }

  WriteBuffer[0] = COMMAND_WRITE_ENABLE;

  struct spi_ioc_transfer mesg = {
    .tx_buf        = WriteBuffer,
    .len           = WRITE_ENABLE_BYTES,
    .bits_per_word = 8,
    .cs            = 0
  };

  rv = ioctl( self->fd, SPI_IOC_MESSAGE( 1 ), &mesg );
  if ( rv != 0 ) {
    return -EIO;
  }

  return 0;
}

static int flash_read(
  rtems_jffs2_flash_control *super,
  uint32_t                   offset,
  unsigned char             *buffer,
  size_t                     size_of_buffer
)
{
  int rv = 0;
  uint32_t current_offset = offset;
  uint32_t bytes_left     = size_of_buffer;

  flash_control *self = get_flash_control( super );

  rv = flash_wait_for_ready( self );
  if ( rv != 0 ) {
    return rv;
  }

  WriteBuffer[0] = COMMAND_QUAD_READ;

  /* Read in 256-byte chunks */
  do {
    uint32_t chunk_size = bytes_left > FLASH_PAGE_SIZE ? FLASH_PAGE_SIZE : bytes_left;

    struct spi_ioc_transfer mesg = {
      .tx_buf        = WriteBuffer,
      .rx_buf        = ReadBuffer,
      .len           = chunk_size + 8,
      .bits_per_word = 8,
      .cs            = 0
    };

    WriteBuffer[1] = (uint8_t) (current_offset >> 16);
    WriteBuffer[2] = (uint8_t) (current_offset >> 8);
    WriteBuffer[3] = (uint8_t) current_offset;

    rv = ioctl( self->fd, SPI_IOC_MESSAGE( 1 ), &mesg );
    if ( rv != 0 ) {
      return -EIO;
    }

    memcpy( &buffer[current_offset - offset], &ReadBuffer[8], chunk_size );

    current_offset += chunk_size;
    bytes_left     -= chunk_size;
  } while ( bytes_left > 0 );

  return 0;
}

static int flash_write(
  rtems_jffs2_flash_control *super,
  uint32_t                   offset,
  const unsigned char       *buffer,
  size_t                     size_of_buffer
)
{
  int rv = 0;

  flash_control *self = get_flash_control( super );

  rv = flash_write_enable( self );
  if ( rv != 0 ) {
    return rv;
  }

  rv = flash_wait_for_ready( self );
  if ( rv != 0 ) {
    return rv;
  }

  WriteBuffer[0] = COMMAND_QUAD_WRITE;
  WriteBuffer[1] = (uint8_t) (offset >> 16);
  WriteBuffer[2] = (uint8_t) (offset >> 8);
  WriteBuffer[3] = (uint8_t) offset;

  memcpy( &WriteBuffer[4], buffer, size_of_buffer );

  struct spi_ioc_transfer mesg = {
    .tx_buf        = WriteBuffer,
    .len           = size_of_buffer + 4,
    .bits_per_word = 8,
    .cs            = 0
  };

  rv = ioctl( self->fd, SPI_IOC_MESSAGE( 1 ), &mesg );
  if ( rv != 0 ) {
    return -EIO;
  }

  return 0;
}

static int flash_erase(
  rtems_jffs2_flash_control *super,
  uint32_t                   offset
)
{
  int rv = 0;

  flash_control *self = get_flash_control( super );

  rv = flash_write_enable( self );
  if ( rv != 0 ) {
    return rv;
  }

  rv = flash_wait_for_ready( self );
  if ( rv != 0 ) {
    return rv;
  }

  WriteBuffer[0] = COMMAND_SECTOR_ERASE;
  WriteBuffer[1] = (uint8_t) (offset >> 16);
  WriteBuffer[2] = (uint8_t) (offset >> 8);
  WriteBuffer[3] = (uint8_t) offset;

  struct spi_ioc_transfer mesg = {
    .tx_buf        = WriteBuffer,
    .len           = SECTOR_ERASE_BYTES,
    .bits_per_word = 8,
    .cs            = 0
  };

  rv = ioctl( self->fd, SPI_IOC_MESSAGE( 1 ), &mesg );
  if ( rv != 0 ) {
    return -EIO;
  }

  return 0;
}

static flash_control flash_instance = {
  .super = {
    .block_size        = BLOCK_SIZE,
    .flash_size        = FLASH_SIZE,
    .read              = flash_read,
    .write             = flash_write,
    .erase             = flash_erase,
    .device_identifier = FLASH_DEVICE_ID
  }
};

static rtems_jffs2_mount_data mount_data = {
  .flash_control      = &flash_instance.super,
  .compressor_control = NULL
};

int microblaze_jffs2_initialize( const char* mount_dir )
{
  int rv = 0;
  int fd = -1;

  uintptr_t mblaze_spi_base = try_get_prop_from_device_tree(
    "xlnx,xps-spi-2.00.a",
    "reg",
    BSP_MICROBLAZE_FPGA_SPI_BASE
  );

  rtems_vector_number mblaze_spi_irq_num = try_get_prop_from_device_tree(
    "xlnx,xps-spi-2.00.a",
    "interrupts",
    BSP_MICROBLAZE_FPGA_SPI_IRQ_NUM
  );

  rv = spi_bus_register_xilinx_axi(
    BUS_PATH,
    mblaze_spi_base,
    FLASH_PAGE_SIZE,
    FLASH_NUM_CS,
    mblaze_spi_irq_num
  );
  if ( rv != 0 ) {
    return rv;
  }

  fd = open( BUS_PATH, O_RDWR );
  if ( fd < 0 ) {
    return -1;
  }

  flash_instance.fd = fd;

  rv = mount_and_make_target_path(
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

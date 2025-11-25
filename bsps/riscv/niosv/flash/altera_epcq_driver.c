/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *
 * Copyright (C) 2024 Kevin Kirspel
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2008 Altera Corporation, San Jose, California, USA.           *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
*                                                                             *
******************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <altera_avalon_epcq_regs.h>
#include <altera_avalon_timer_regs.h>

#include <rtems/counter.h>

#ifndef ALT_MAX_NUMBER_OF_FLASH_REGIONS
#define ALT_MAX_NUMBER_OF_FLASH_REGIONS 8
#endif /* ALT_MAX_NUMBER_OF_FLASH_REGIONS */

/* MACROS */
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

/*
 * Description of a single Erase region
 */
typedef struct flash_region
{
  int   offset;
  int   region_size;
  int   number_of_blocks;
  int   block_size;
}flash_region;

/**
 *  Description of the flash device
 */
typedef struct alt_flash_dev alt_flash_dev;

/**
 *  Description of the flash device api
 */
typedef int (*alt_flash_read)(
  alt_flash_dev* flash,
  int offset,
  void* dest_addr,
  int length
);

/**
 *  Description of the flash device implementation
 */
struct alt_flash_dev
{
  const char*               name;
  alt_flash_read            read;
  void*                     base_addr;
  int                       length;
  int                       number_of_regions;
  flash_region              region_info[ALT_MAX_NUMBER_OF_FLASH_REGIONS];
};


/**
 *  Description of the EPCQ controller
 */
typedef struct alt_epcq_controller2_dev
{
  alt_flash_dev dev;

  uint32_t data_base;         /* base address of data slave */
  uint32_t data_end;          /* end address of data slave (not inclusive) */
  uint32_t csr_base;          /* base address of CSR slave */
  uint32_t size_in_bytes;     /* size of memory in bytes */
  uint32_t is_epcs;           /* 1 if device is an EPCS device */
  uint32_t number_of_sectors; /* number of flash sectors */
  uint32_t sector_size;       /* size of each flash sector */
  uint32_t page_size;         /* page size */
  uint32_t silicon_id;        /* ID of silicon used with EPCQ IP */
} alt_epcq_controller2_dev;

static int altera_epcq_controller2_init(alt_epcq_controller2_dev *dev);
static int alt_epcq_controller2_erase_block(
  alt_flash_dev *flash_info,
  int block_offset
);
static int alt_epcq_controller2_write_block(
  alt_flash_dev *flash_info,
  int block_offset,
  int data_offset,
  const void *data, int length
);
static int alt_epcq_controller2_write(
  alt_flash_dev *flash_info,
  int offset,
  const void *src_addr,
  int length,
  bool erase
);
static int alt_epcq_controller2_read(
  alt_flash_dev *flash_info,
  int offset,
  void *dest_addr,
  int length
);
static inline int alt_epcq_validate_read_write_arguments(
  alt_epcq_controller2_dev *flash_info,
  uint32_t offset,
  uint32_t length
);
static int alt_epcq_poll_for_write_in_progress(
  alt_epcq_controller2_dev* flash_info
);

/**
*   Macros used by alt_sys_init.c to create data storage for driver instance
*/
#define ALTERA_EPCQ_CSR_INSTANCE(epcq_name, avl_mem, avl_csr, epcq_dev)        \
static alt_epcq_controller2_dev epcq_dev = {                                   \
  .dev = {                                                                     \
    .read = alt_epcq_controller2_read,                                         \
    .base_addr = ((void*)(avl_mem##_BASE)),                                    \
    .length = ((int)(avl_mem##_SPAN)),                                         \
  },                                                                           \
  .data_base = ((uint32_t)(avl_mem##_BASE)),                                   \
  .data_end = ((uint32_t)(avl_mem##_BASE) + (uint32_t)(avl_mem##_SPAN)),       \
  .csr_base = ((uint32_t)(avl_csr##_BASE)),                                    \
  .size_in_bytes = ((uint32_t)(avl_mem##_SPAN)),                               \
  .is_epcs = ((uint32_t)(avl_mem##_IS_EPCS)),                                  \
  .number_of_sectors = ((uint32_t)(avl_mem##_NUMBER_OF_SECTORS)),              \
  .sector_size = ((uint32_t)(avl_mem##_SECTOR_SIZE)),                          \
  .page_size = ((uint32_t)(avl_mem##_PAGE_SIZE))  ,                            \
}

#define ALTERA_EPCQ_CONTROLLER2_INIT(name, dev)   \
  altera_epcq_controller2_init(&dev);

/**
*   The EPCQ device instance
*/
ALTERA_EPCQ_CSR_INSTANCE(
  EPCQ_CONTROLLER,
  EPCQ_CONTROLLER_AVL_MEM,
  EPCQ_CONTROLLER_AVL_CSR,
  epcq_controller
);

/**
 * epcq_initialize
 *
 * Initializes the epcq device interface.
 *
**/
void epcq_initialize( void )
{
  ALTERA_EPCQ_CONTROLLER2_INIT( EPCQ_CONTROLLER, epcq_controller);
}

/**
 * epcq_read_buffer
 *
 * Reads data from the epcq device.
 *
 * Arguments:
 * - offset: offset within the memory area to read.
 * - dest_addr: the location to store the returned data.
 * - length: The number of bytes to read.
 *
 * Returns:
 * 0 -> The number of bytes actually read
 * -EINVAL -> Invalid arguments.
**/
int epcq_read_buffer( int offset, uint8_t *dest_addr, int length )
{
  return alt_epcq_controller2_read(
    &epcq_controller.dev,
    offset,
    dest_addr,
    length
  );
}

/**
 * epcs_write_buffer
 *
 * Writes data to the epcq device.
 *
 * Arguments:
 * - offset: offset within the memory area to read.
 * - src_addr: the data top store.
 * - length: The number of bytes to write.
 *
 * Returns:
 * 0 -> The number of bytes actually read
 * -EINVAL -> Invalid arguments.
**/
int epcq_write_buffer (
  int offset,
  const uint8_t* src_addr,
  int length,
  bool erase
)
{
  return alt_epcq_controller2_write (
    &epcq_controller.dev,
    offset,
    src_addr,
    length,
    erase
  );
}

/**
 * altera_epcq_controller2_init
 *
 * Information in system.h is checked against expected values that are
 * determined by the silicon_id. If the information doesn't match then this
 * system is configured incorrectly. Most likely the wrong type of EPCS or EPCQ
 * device was selected when instantiating the soft IP.
 *
 * Arguments:
 * - *flash: Pointer to EPCQ flash device structure.
 *
 * Returns:
 * 0 -> success
 * -EINVAL -> Invalid arguments.
 * -ENODEV -> System is configured incorrectly.
**/
static int altera_epcq_controller2_init(alt_epcq_controller2_dev *flash)
{
  uint32_t silicon_id = 0;
  uint32_t size_in_bytes = 0;
  uint32_t number_of_sectors = 0;

  /* return -EINVAL if flash is NULL */
  if ( NULL == flash ) {
    return -EINVAL;
  }

  /* return -ENODEV if CSR slave is not attached */
  if ( NULL == ( void * )flash->csr_base ) {
    return -ENODEV;
  }

  /*
   * If flash is an EPCQ device, we read the EPCQ_RD_RDID register for the ID
   * If flash is an EPCS device, we read the EPCQ_RD_SID register for the ID
   *
   * Whether or not the flash is a EPCQ or EPCS is indicated in the system.h.
   * The system.h gets this value from the hw.tcl of the IP. If this value is
   * set incorrectly, then things will go badly.
   *
   * In both cases, we can determine the number of sectors, which we can use
   * to calculate a size. We compare that size to the system.h value to make
   * sure the EPCQ soft IP was configured correctly.
   */
  if ( 0 == flash->is_epcs ) {
    /* If we're an EPCQ, we read EPCQ_RD_RDID for the silicon ID */
    silicon_id = EPCQ_REGS->rd_rdid;
    silicon_id &= ALTERA_EPCQ_CONTROLLER2_RDID_MASK;

    /* Determine which EPCQ device so we can figure out the number of sectors */
    /* EPCQ share the same ID for the same capacity*/
    switch( silicon_id ) {
      case ALTERA_EPCQ_CONTROLLER2_RDID_EPCQ16:
        number_of_sectors = 32;
        break;
      case ALTERA_EPCQ_CONTROLLER2_RDID_EPCQ32:
        number_of_sectors = 64;
        break;
      case ALTERA_EPCQ_CONTROLLER2_RDID_EPCQ64:
        number_of_sectors = 128;
        break;
      case ALTERA_EPCQ_CONTROLLER2_RDID_EPCQ128:
        number_of_sectors = 256;
        break;
      case ALTERA_EPCQ_CONTROLLER2_RDID_EPCQ256:
        number_of_sectors = 512;
        break;
      case ALTERA_EPCQ_CONTROLLER2_RDID_EPCQ512:
        number_of_sectors = 1024;
        break;
      case ALTERA_EPCQ_CONTROLLER2_RDID_EPCQ1024:
        number_of_sectors = 2048;
        break;
      default:
        return -ENODEV;
    }
  } else {
    /* If we're an EPCS, we read EPCQ_RD_SID for the silicon ID */
    silicon_id = EPCQ_REGS->rd_sid;
    silicon_id &= ALTERA_EPCQ_CONTROLLER2_SID_MASK;

    /* Determine which EPCS device so we can figure out various properties */
    switch(silicon_id) {
      case ALTERA_EPCQ_CONTROLLER2_SID_EPCS16:
        number_of_sectors = 32;
        break;
      case ALTERA_EPCQ_CONTROLLER2_SID_EPCS64:
        number_of_sectors = 128;
        break;
      case ALTERA_EPCQ_CONTROLLER2_SID_EPCS128:
        number_of_sectors = 256;
        break;
      default:
        return -ENODEV;
    }
  }

  /* Calculate size of flash based on number of sectors */
  size_in_bytes = number_of_sectors * flash->sector_size;

  /*
   * Make sure calculated size is the same size given in system.h
   * Also check number of sectors is the same number given in system.h
   * Otherwise the EPCQ IP was not configured correctly
   */
  if (
    size_in_bytes != flash->size_in_bytes ||
    number_of_sectors != flash->number_of_sectors
  ) {
    flash->dev.number_of_regions = 0;
    return -ENODEV;
  } else {
    flash->silicon_id = silicon_id;
    flash->number_of_sectors = number_of_sectors;

    /*
     * populate fields of region_info required to conform to HAL API
     * create 1 region that composed of "number_of_sectors" blocks
     */
    flash->dev.number_of_regions = 1;
    flash->dev.region_info[0].offset = 0;
    flash->dev.region_info[0].region_size = size_in_bytes;
    flash->dev.region_info[0].number_of_blocks = number_of_sectors;
    flash->dev.region_info[0].block_size = flash->sector_size;
  }

  return 0;
}

/**
  * alt_epcq_controller2_erase_block
  *
  * This function erases a single flash sector.
  *
  * Arguments:
  * - *flash_info: Pointer to QSPI flash device structure.
  * - block_offset: byte-addressed offset, from start of flash, of the sector to
  *   be erased
  *
  * Returns:
  * 0 -> success
  * -EINVAL -> Invalid arguments
  * -EIO -> write failed, sector might be protected
**/
static int alt_epcq_controller2_erase_block(
  alt_flash_dev *flash_info,
  int block_offset
)
{
  int32_t ret_code = 0;
  uint32_t mem_op_value = 0; /* value to write to EPCQ_MEM_OP register */
  alt_epcq_controller2_dev* epcq_flash_info = NULL;
  uint32_t sector_number = 0;

  /* return -EINVAL if flash_info is NULL */
  if ( NULL == flash_info ) {
    return -EINVAL;
  }

  epcq_flash_info = (alt_epcq_controller2_dev*)flash_info;

  /*
   * Sanity checks that block_offset is within the flash memory span and that
   * the block offset is sector aligned.
   *
   */
  if (
    ( block_offset < 0 ) ||
    ( block_offset >= epcq_flash_info->size_in_bytes ) ||
    ( block_offset & ( epcq_flash_info->sector_size - 1 )) != 0
  ) {
    return -EINVAL;
  }

  alt_epcq_poll_for_write_in_progress(epcq_flash_info);

  /* calculate current sector/block number */
  sector_number = (block_offset/(epcq_flash_info->sector_size));

  /* sector value should occupy bits 23:8 */
  mem_op_value = (sector_number << 8) &
    ALTERA_EPCQ_CONTROLLER2_MEM_OP_SECTOR_VALUE_MASK;

   /* write enable command */
  mem_op_value |= ALTERA_EPCQ_CONTROLLER2_MEM_OP_WRITE_ENABLE_CMD;

  /*
   * write sector erase command to EPCQ_MEM_OP register to erase sector
   * "sector_number"
   */
  EPCQ_REGS->mem_op = mem_op_value;

  /* sector value should occupy bits 23:8 */
  mem_op_value = (sector_number << 8) &
    ALTERA_EPCQ_CONTROLLER2_MEM_OP_SECTOR_VALUE_MASK;

  /* sector erase commands 0b10 occupies lower 2 bits */
  mem_op_value |= ALTERA_EPCQ_CONTROLLER2_MEM_OP_SECTOR_ERASE_CMD;

  /*
   * write sector erase command to QSPI_MEM_OP register to erase sector
   * "sector_number"
   */
  EPCQ_REGS->mem_op = mem_op_value;

  alt_epcq_poll_for_write_in_progress(epcq_flash_info);

  /* check whether erase triggered a illegal erase interrupt  */
  if (
    ( EPCQ_REGS->isr & ALTERA_EPCQ_CONTROLLER2_ISR_ILLEGAL_ERASE_MASK ) ==
    ALTERA_EPCQ_CONTROLLER2_ISR_ILLEGAL_ERASE_ACTIVE
  ) {
    /* clear register */
    /* QSPI_ISR access is write one to clear (W1C) */
    EPCQ_REGS->isr = ALTERA_EPCQ_CONTROLLER2_ISR_ILLEGAL_ERASE_MASK;
    return -EIO; /* erase failed, sector might be protected */
  }

  return ret_code;
}

/**
 * alt_epcq_controller2_write_block
 *
 * This function writes one block/sector of data to flash. The length of the
 * write can NOT spill into the adjacent sector.
 *
 * It assumes that someone has already erased the appropriate sector(s).
 *
 * Arguments:
 * - *flash_info: Pointer to QSPI flash device structure.
 * - block_offset: byte-addressed offset, from the start of flash, of the sector
 *   to written to
 * - data-offset: Byte offset (unaligned access) of write into flash memory.
 *   For best performance, word(32 bits - aligned access) offset of write is
 *   recommended.
 * - *src_addr: source buffer
 * - length: size of writing
 *
 * Returns:
 * 0 -> success
 * -EINVAL -> Invalid arguments
 * -EIO -> write failed, sector might be protected
**/
static int alt_epcq_controller2_write_block (
  alt_flash_dev *flash_info, /** flash device info */
  int block_offset, /** sector/block offset in byte addressing */
  int data_offset, /** offset of write from base address */
  const void *data, /** data to be written */
  int length /** bytes of data to be written, >0 */
)
{
  uint32_t buffer_offset = 0; /** offset into data buffer to get write data */
  uint32_t remaining_length = length; /** length left to write */
  uint32_t write_offset = data_offset; /** offset into flash to write too */
  uint32_t write_offset_32;

  alt_epcq_controller2_dev *epcq_flash_info =
    (alt_epcq_controller2_dev*)flash_info;

  /*
   * Sanity checks that data offset is not larger then a sector, that block
   * offset is sector aligned and within the valid flash memory range and a
   * write doesn't spill into the adjacent flash sector.
   */
  if (
    block_offset < 0 ||
    data_offset < 0 ||
    NULL == flash_info ||
    NULL == data ||
    data_offset >= epcq_flash_info->size_in_bytes ||
    block_offset >= epcq_flash_info->size_in_bytes ||
    length > (epcq_flash_info->sector_size - (data_offset - block_offset)) ||
    length < 0 ||
    (block_offset & (epcq_flash_info->sector_size - 1)) != 0
  ) {
    return -EINVAL;
  }

  /*
   * Do writes one 32-bit word at a time.
   * We need to make sure that we pad the first few bytes so they're word
   * aligned if they are not already.
   */
  while ( remaining_length > 0 ) {
    volatile uint32_t dummy_read;
    /** initialize word to write to blank word */
    uint32_t word_to_write = 0xFFFFFFFF;
    /** bytes to pad the next word that is written */
    uint32_t padding = 0;
    /** number of bytes from source to copy */
    uint32_t bytes_to_copy = sizeof(uint32_t);

    /*
     * we need to make sure the write is word aligned
     * this should only be true at most 1 time
     */
    if ( 0 != ( write_offset & ( sizeof( uint32_t ) - 1 ))) {
      /*
       * data is not word aligned
       * calculate padding bytes need to add before start of a data offset
       */
      padding = write_offset & (sizeof(uint32_t) - 1);

      /* update variables to account for padding being added */
      bytes_to_copy -= padding;

      if(bytes_to_copy > remaining_length) {
        bytes_to_copy = remaining_length;
      }

      write_offset = write_offset - padding;
      if ( 0 != ( write_offset & ( sizeof( uint32_t ) - 1 ))) {
        return -EINVAL;
      }
    } else {
      if ( bytes_to_copy > remaining_length ) {
        bytes_to_copy = remaining_length;
      }
    }

    /* prepare the word to be written */
    memcpy (
      ((( void * )&word_to_write)) + padding,
      (( void * )data) + buffer_offset,
      bytes_to_copy
    );

    /* update offset and length variables */
    buffer_offset += bytes_to_copy;
    remaining_length -= bytes_to_copy;

    /* write to flash 32 bits at a time */
    write_offset_32 = write_offset >> 2;
    EPCQ_MEM_32[write_offset_32] = word_to_write;
    alt_epcq_poll_for_write_in_progress(epcq_flash_info);
    if ( EPCQ_MEM_32[write_offset_32] != word_to_write ) {
      EPCQ_MEM_32[write_offset_32] = word_to_write;
      alt_epcq_poll_for_write_in_progress(epcq_flash_info);
      dummy_read = EPCQ_MEM_32[write_offset_32];
      (void) dummy_read;
    }

    /* check whether write triggered a illegal write interrupt */
    if (
      ( EPCQ_REGS->isr & ALTERA_EPCQ_CONTROLLER2_ISR_ILLEGAL_WRITE_MASK ) ==
        ALTERA_EPCQ_CONTROLLER2_ISR_ILLEGAL_WRITE_ACTIVE
    ) {
      /* clear register */
      EPCQ_REGS->isr = ALTERA_EPCQ_CONTROLLER2_ISR_ILLEGAL_WRITE_MASK;
      return -EIO; /** write failed, sector might be protected */
    }

    /* update current offset */
    write_offset = write_offset + sizeof(uint32_t);
  }

  return 0;
}

/**
 * alt_epcq_controller2_write
 *
 * Program the data into the flash at the selected address.
 *
 * The different between this function and alt_epcq_controller2_write_block
 * function is that this function (alt_epcq_controller2_write) will
 * automatically erase a block as needed
 *
 * Arguments:
 * - *flash_info: Pointer to QSPI flash device structure.
 * - offset: Byte offset (unaligned access) of write to flash memory. For best
 *   performance, word(32 bits - aligned access) offset of write is recommended.
 * - *src_addr: source buffer
 * - length: size of writing
 *
 * Returns:
 * 0 -> success
 * -EINVAL -> Invalid arguments
 * -EIO -> write failed, sector might be protected
 *
**/
static int alt_epcq_controller2_write (
  alt_flash_dev *flash_info, /** device info */
  int offset, /** offset of write from base address */
  const void *src_addr, /** source buffer */
  int length, /** size of writing */
  bool erase /** whether or not to erase before writing */
)
{
  int32_t ret_code = 0;

  alt_epcq_controller2_dev *epcq_flash_info = NULL;

/** address of next byte to write */
  uint32_t write_offset = offset;
  /** length of write data left to be written */
  uint32_t remaining_length = length;
  /** offset into source buffer to get write data */
  uint32_t buffer_offset = 0;
  uint32_t i = 0;

  /* return -EINVAL if flash_info and src_addr are NULL */
  if( NULL == flash_info || NULL == src_addr ) {
    return -EINVAL;
  }

  epcq_flash_info = (alt_epcq_controller2_dev*)flash_info;

  /* make sure the write parameters are within the bounds of the flash */
  ret_code = alt_epcq_validate_read_write_arguments (
    epcq_flash_info,
    offset,
    length
  );

  if ( 0 != ret_code ) {
    return ret_code;
  }

  /*
   * This loop erases and writes data one sector at a time. We check for write
   * completion before starting the next sector.
   */
  for (
    i = offset / epcq_flash_info->sector_size;
    i < epcq_flash_info->number_of_sectors;
    i++
  ) {
    /** block offset in byte addressing */
    uint32_t block_offset = 0;
    /** offset into current sector to write */
    uint32_t offset_within_current_sector = 0;
    /** length to write to current sector */
    uint32_t length_to_write = 0;

    if( 0 >= remaining_length ) {
      break; /* out of data to write */
    }

    /* calculate current sector/block offset in byte addressing */
    block_offset = write_offset & ~(epcq_flash_info->sector_size - 1);

    /* calculate offset into sector/block if there is one */
    if ( block_offset != write_offset ) {
      offset_within_current_sector = write_offset - block_offset;
    }

    /* erase sector */
    if( erase ) {
      ret_code = alt_epcq_controller2_erase_block(flash_info, block_offset);

      if( 0 != ret_code ) {
        return ret_code;
      }
    }

    /* calculate the byte size of data to be written in a sector */
    length_to_write = MIN (
      epcq_flash_info->sector_size - offset_within_current_sector,
      remaining_length
    );

    /* write data to erased block */
    ret_code = alt_epcq_controller2_write_block (
      flash_info,
      block_offset,
      write_offset,
      src_addr + buffer_offset,
      length_to_write
    );

    if( 0 != ret_code ) {
      return ret_code;
    }

    /* update remaining length and buffer_offset pointer */
    remaining_length -= length_to_write;
    buffer_offset += length_to_write;
    write_offset += length_to_write;
  }

  return ret_code;
}

/**
 * alt_epcq_controller2_read
 *
 * There's no real need to use this function as opposed to using memcpy
 * directly. It does do some sanity checks on the bounds of the read.
 *
 * Arguments:
 * - *flash_info: Pointer to general flash device structure.
 * - offset: offset read from flash memory.
 * - *dest_addr: destination buffer
 * - length: size of reading
 *
 * Returns:
 * 0 -> success
 * -EINVAL -> Invalid arguments
**/
static int alt_epcq_controller2_read (
  alt_flash_dev *flash_info, /** device info */
  int offset, /** offset of read from base address */
  void *dest_addr, /** destination buffer */
  int length /** size of read */
)
{
  int32_t ret_code = 0;
  alt_epcq_controller2_dev *epcq_flash_info = NULL;

  /* return -EINVAL if flash_info and dest_addr are NULL */
  if ( NULL == flash_info || NULL == dest_addr ) {
    return -EINVAL;
  }

  epcq_flash_info = (alt_epcq_controller2_dev*)flash_info;

  /* validate arguments */
  ret_code = alt_epcq_validate_read_write_arguments (
    epcq_flash_info,
    offset,
    length
  );

  /* copy data from flash to destination address */
  if( 0 == ret_code ) {
    memcpy(dest_addr, (uint8_t*)epcq_flash_info->data_base + offset, length);
  }

  return ret_code;
}

/*
 *    Helper functions used by Public API functions.
 *
 * Arguments:
 * - *flash_info: Pointer to EPCQ flash device structure.
 * - offset: Offset of read/write from base address.
 * - length: Length of read/write in bytes.
 *
 * Returns:
 * 0 -> success
 * -EINVAL -> Invalid arguments
 */
/**
 * Used to check that arguments to a read or write are valid
 */
static inline int alt_epcq_validate_read_write_arguments (
  alt_epcq_controller2_dev *flash_info,
  uint32_t offset,
  uint32_t length
)
{
  alt_epcq_controller2_dev *epcq_flash_info = NULL;
  uint32_t start_address = 0;
  int32_t end_address = 0;

  /* return -EINVAL if flash_info is NULL */
  if( NULL == flash_info ) {
    return -EINVAL;
  }

  epcq_flash_info = (alt_epcq_controller2_dev*)flash_info;

  /* first address of read or write */
  start_address = epcq_flash_info->data_base + offset;
  /* last address of read or write (not inclusive) */
  end_address = start_address + length;

  /* make sure start and end address is less then the end address of the flash */
  if (
    start_address >= epcq_flash_info->data_end ||
    end_address > epcq_flash_info->data_end
  ) {
    return -EINVAL;
  }

  return 0;
}

/*
 * Private function that polls write in progress bit QSPI_RD_STATUS.
 *
 * Write in progress will be set if any of the following operations are in
 * progress:
 *     -WRITE STATUS REGISTER
 *     -WRITE NONVOLATILE CONFIGURATION REGISTER
 *     -PROGRAM
 *     -ERASE
 *
 * Assumes QSPI was configured correctly.
 *
 * If ALTERA_EPCQ_CONTROLLER2_1US_TIMEOUT_VALUE is set, the function will time
 * out after a period of time determined by that value.
 *
 * Arguments:
 * - *epcq_flash_info: Pointer to QSPI flash device structure.
 *
 * Returns:
 * 0 -> success
 * -EINVAL -> Invalid arguments
 * -ETIME  -> Time out and skipping the looping after 0.7 sec.
 */
static int alt_epcq_poll_for_write_in_progress (
 alt_epcq_controller2_dev* epcq_flash_info
)
{
  /* we'll want to implement timeout if a timeout value is specified */
#if ALTERA_EPCQ_CONTROLLER2_1US_TIMEOUT_VALUE > 0
  uint32_t timeout = ALTERA_EPCQ_CONTROLLER2_1US_TIMEOUT_VALUE;
  uint32_t counter = 0;
#endif

  /* return -EINVAL if epcq_flash_info is NULL */
  if ( NULL == epcq_flash_info ) {
    return -EINVAL;
  }

  /* while Write in Progress bit is set, we wait */
  while (
    (EPCQ_REGS->rd_status & ALTERA_EPCQ_CONTROLLER2_STATUS_WIP_MASK) ==
    ALTERA_EPCQ_CONTROLLER2_STATUS_WIP_BUSY
  ) {
    if ( counter > (ALTERA_EPCQ_CONTROLLER2_1US_TIMEOUT_VALUE >> 1 )) {
      rtems_counter_delay_ticks(2); /* delay 2us */
    }
#if ALTERA_EPCQ_CONTROLLER2_1US_TIMEOUT_VALUE > 0
    if ( timeout <= counter ) {
      return -ETIME;
    }

    counter++;
#endif
  }

  return 0;
}

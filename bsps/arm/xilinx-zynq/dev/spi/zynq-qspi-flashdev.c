/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2024 Aaron Nyholm
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS IS
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

#include <bsp/irq.h>
#include <dev/spi/zynq-qspi-flashdev.h>
#include <dev/spi/zynq-qspi-flash.h>

#include <stdlib.h>
#include <string.h>

/*
 * @brief Struct allocating memory space for flash regions. Used by
 * rtems_flashdev to store region allocations.
 */
typedef struct zqspi_flash_region_table {
  rtems_flashdev_region zqspi_flash_regions[ZQSPI_FLASH_MAX_REGIONS];
  uint32_t zqspi_flash_bit_allocator;
} zqspi_flash_region_table;

static uint32_t zqspi_get_jedec_id(rtems_flashdev *flash) {
  uint32_t jedec = 0;
  zqspi_readid(flash->driver, &jedec);
  return (jedec);
}

static int zqspi_get_flash_type(
  rtems_flashdev *flash,
  rtems_flashdev_flash_type *type
)
{
  (void) flash;

  *type = RTEMS_FLASHDEV_NOR;
  return 0;
}

static int zqspi_read_wrapper(
    rtems_flashdev *flash,
    uintptr_t offset,
    size_t count,
    void *buffer
)
{
  zqspiflash *flash_driver = (zqspiflash*)flash->driver;
  return zqspi_read(flash_driver, (uint32_t)offset, buffer, count);
}

static int zqspi_page_info_by_off(
  rtems_flashdev *flash,
  off_t search_offset,
  off_t *page_offset,
  size_t *page_size
)
{
  zqspiflash *flash_driver = (zqspiflash*)flash->driver;
  *page_size = (size_t)flash_driver->flash_page_size;
  *page_offset = search_offset - (search_offset%((off_t)(*page_size)));
  return 0;
}

static int zqspi_page_info_by_index(
  rtems_flashdev *flash,
  off_t search_index,
  off_t *page_offset,
  size_t *page_size
)
{
  zqspiflash *flash_driver = (zqspiflash*)flash->driver;
  *page_size = (size_t)flash_driver->flash_page_size;
  *page_offset = *page_size * search_index;
  return 0;
}

static int zqspi_page_count(
  rtems_flashdev *flash,
  int *page_count
)
{
  zqspiflash *flash_driver = (zqspiflash*)flash->driver;
  *page_count = flash_driver->flash_size /
                  flash_driver->flash_page_size;
  return 0;
}

static int zqspi_write_block_size(
  rtems_flashdev *flash,
  size_t *write_block_size
)
{
  (void) flash;

  *write_block_size = 1u;
  return 0;
}

static int zqspi_write_wrapper(
  rtems_flashdev *flash,
  uintptr_t offset,
  size_t count,
  const void *buffer
)
{
  zqspiflash *flash_driver = (zqspiflash*)flash->driver;
  return zqspi_write(flash_driver, (uint32_t)offset, buffer, count);
}

static int zqspi_erase_wrapper(
  rtems_flashdev *flash,
  uintptr_t offset,
  size_t count
)
{
  zqspiflash *flash_driver = (zqspiflash*)flash->driver;
  return zqspi_erase(flash_driver, (uint32_t)offset, count);
}

static int zqspi_sector_info_by_off(
  rtems_flashdev *flash,
  off_t search_offset,
  off_t *sector_offset,
  size_t *sector_size
)
{
  zqspiflash *flash_driver = (zqspiflash*)flash->driver;
  *sector_size = (size_t)flash_driver->flash_erase_sector_size;
  *sector_offset = search_offset - (search_offset%((off_t)(*sector_size)));
  return 0;
}

static int zqspi_sector_count(
  rtems_flashdev *flash,
  int *sector_count
)
{
  zqspiflash *flash_driver = (zqspiflash*)flash->driver;
  *sector_count = flash_driver->flash_size /
                  flash_driver->flash_erase_sector_size;
  return 0;
}

static void zqspi_priv_destroy(rtems_flashdev* flash)
{
  free(flash->region_table->regions);
  free(flash->region_table);
}

rtems_flashdev* zqspi_flashdev_init(zqspiflash *bmdriver)
{
  zqspi_flash_region_table *xtable =
    calloc(1, sizeof(zqspi_flash_region_table));

  if (xtable == NULL) {
    return NULL;
  }

  rtems_flashdev_region_table *ftable =
    calloc(1, sizeof(rtems_flashdev_region_table));

  if (ftable == NULL) {
    free(xtable);
    return NULL;
  }

  ftable->regions = (rtems_flashdev_region*)xtable;
  ftable->max_regions = ZQSPI_FLASH_MAX_REGIONS;
  ftable->bit_allocator = &(xtable->zqspi_flash_bit_allocator);

  rtems_flashdev *flash =
    rtems_flashdev_alloc_and_init(sizeof(rtems_flashdev));

  if (flash == NULL) {
    free(xtable);
    free(ftable);
    return NULL;
  }

  flash->driver = bmdriver;
  flash->priv_destroy = &zqspi_priv_destroy;
  flash->read = &zqspi_read_wrapper;
  flash->write = &zqspi_write_wrapper;
  flash->erase = &zqspi_erase_wrapper;
  flash->jedec_id = &zqspi_get_jedec_id;
  flash->flash_type = &zqspi_get_flash_type;
  flash->page_info_by_offset = &zqspi_page_info_by_off;
  flash->page_info_by_index = &zqspi_page_info_by_index;
  flash->page_count = &zqspi_page_count;
  flash->write_block_size = &zqspi_write_block_size;
  flash->sector_info_by_offset = &zqspi_sector_info_by_off;
  flash->sector_count = &zqspi_sector_count;
  flash->region_table = ftable;

  return flash;
}

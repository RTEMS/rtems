/*
 * Copyright (C) 2023, 2023 Aaron Nyholm
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
#include <dev/spi/xqspi_flash.h>
#include <dev/spi/xqspipsu.h>
#include <dev/spi/xqspipsu-flash-helper.h>
#include <dev/spi/xqspipsu_flash_config.h>

#include <stdlib.h>
#include <string.h>

/*
 * @brief Struct allocating memory space for flash regions. Used by
 * rtems_flashdev to store region allocations.
 */
typedef struct xqspi_flash_region_table {
  rtems_flashdev_region xqspi_flash_regions[XQSPI_FLASH_MAX_REGIONS];
  uint32_t xqspi_flash_bit_allocator;
} xqspi_flash_region_table;

static uint32_t xqspi_get_jedec_id(rtems_flashdev *flash) {
  return QspiPsu_NOR_Get_JEDEC_ID(flash->driver);
}

static int xqspi_get_flash_type(
  rtems_flashdev *flash,
  rtems_flashdev_flash_type *type
)
{
  (void) flash;

  *type = RTEMS_FLASHDEV_NOR;
  return 0;
}

static int xqspi_read_wrapper(
    rtems_flashdev *flash,
    uintptr_t offset,
    size_t count,
    void *buffer
)
{
  (void) flash;

  XQspiPsu *flash_driver = (XQspiPsu*)flash->driver;
  return QspiPsu_NOR_Read(flash_driver, (uint32_t)offset, (uint32_t)count, buffer);
}

static int xqspi_page_info_by_off(
  rtems_flashdev *flash,
  off_t search_offset,
  off_t *page_offset,
  size_t *page_size
)
{
  *page_size = QspiPsu_NOR_Get_Page_Size(flash->driver);
  *page_offset = search_offset - (search_offset%((off_t)(*page_size)));
  return 0;
}

static int xqspi_page_info_by_index(
  rtems_flashdev *flash,
  off_t search_index,
  off_t *page_offset,
  size_t *page_size
)
{
  *page_size = QspiPsu_NOR_Get_Page_Size(flash->driver);
  *page_offset = *page_size * search_index;
  return 0;
}

static int xqspi_page_count(
  rtems_flashdev *flash,
  int *page_count
)
{
  *page_count = QspiPsu_NOR_Get_Device_Size(flash->driver) /
                  QspiPsu_NOR_Get_Page_Size(flash->driver);
  return 0;
}

static int xqspi_sector_count(
  rtems_flashdev *flash,
  int *sector_count
)
{
  *sector_count = QspiPsu_NOR_Get_Device_Size(flash->driver) /
                  QspiPsu_NOR_Get_Sector_Size(flash->driver);
  return 0;
}

static int xqspi_sector_info_by_off(
  rtems_flashdev *flash,
  off_t search_offset,
  off_t *sector_offset,
  size_t *sector_size
)
{
  *sector_size = QspiPsu_NOR_Get_Sector_Size(flash->driver);
  *sector_offset = search_offset - (search_offset%((off_t)(*sector_size)));
  return 0;
}

static int xqspi_write_block_size(
  rtems_flashdev *flash,
  size_t *write_block_size
)
{
  *write_block_size = QspiPsu_NOR_Get_Page_Size(flash->driver);
  return 0;
}

static int xqspi_write_wrapper(
  rtems_flashdev *flash,
  uintptr_t offset,
  size_t count,
  const void *buffer
)
{
  XQspiPsu *flash_driver = (XQspiPsu*)flash->driver;
  return QspiPsu_NOR_Write(flash_driver, (uint32_t)offset, (uint32_t)count,
    (void*)buffer);
}

static int xqspi_erase_wrapper(
  rtems_flashdev *flash,
  uintptr_t offset,
  size_t count
)
{
  XQspiPsu *flash_driver = (XQspiPsu*)flash->driver;
  return QspiPsu_NOR_Erase(flash_driver, (uint32_t)offset, (uint32_t)count);
}

static void xqspi_flash_priv_destroy(rtems_flashdev* flash)
{
  free(flash->region_table->regions);
  free(flash->region_table);
}

rtems_flashdev* xqspi_flash_init(XQspiPsu *xQspiDev)
{
  xqspi_flash_region_table *xtable =
    calloc(1, sizeof(xqspi_flash_region_table));

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
  ftable->max_regions = XQSPI_FLASH_MAX_REGIONS;
  ftable->bit_allocator = &(xtable->xqspi_flash_bit_allocator);

  rtems_flashdev *flash =
    rtems_flashdev_alloc_and_init(sizeof(rtems_flashdev));

  if (flash == NULL) {
    free(xtable);
    free(ftable);
    return NULL;
  }

  flash->driver = xQspiDev;
  flash->priv_destroy = &xqspi_flash_priv_destroy;
  flash->read = &xqspi_read_wrapper;
  flash->write = &xqspi_write_wrapper;
  flash->erase = &xqspi_erase_wrapper;
  flash->jedec_id = &xqspi_get_jedec_id;
  flash->flash_type = &xqspi_get_flash_type;
  flash->page_info_by_offset = &xqspi_page_info_by_off;
  flash->page_info_by_index = &xqspi_page_info_by_index;
  flash->page_count = &xqspi_page_count;
  flash->write_block_size = &xqspi_write_block_size;
  flash->sector_count = &xqspi_sector_count;
  flash->sector_info_by_offset = &xqspi_sector_info_by_off;
  flash->region_table = ftable;

  return flash;
}

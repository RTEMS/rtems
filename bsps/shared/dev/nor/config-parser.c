/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2023 On-Line Applications Research Corporation (OAR)
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

#include <dev/nor/config-parser.h>
#include <string.h>

static int read_config_byte(
  uint8_t *config_raw,
  size_t config_raw_len,
  int datalen,
  int desired_byte,
  uint8_t *read_byte
)
{
  /* access outside buffer */
  if (desired_byte >= config_raw_len) {
    return 1;
  }
  if (datalen != 0) {
    /* access past self-declared limit */
    if (desired_byte >= datalen) {
      return 1;
    }
  }
  *read_byte = config_raw[desired_byte];
  return 0;
}

static int verify_config_string(
  uint8_t *config_raw,
  size_t config_raw_len,
  int datalen,
  int startbyte,
  char *config_string
)
{
  uint8_t bufbyte;
  int endbyte = strlen(config_string) + startbyte;
  for (;startbyte < endbyte; startbyte++, config_string++) {
    if (read_config_byte(config_raw, config_raw_len, datalen, startbyte, &bufbyte)) {
      return 1;
    }
    if (bufbyte != *config_string) {
      return 1;
    }
  }
  return 0;
}

/* Read two consecutive bytes as a little endian short */
static int read_config_short(
  uint8_t *config_raw,
  size_t config_raw_len,
  int datalen,
  int startbyte,
  uint16_t *config_short
)
{
  uint8_t bufbyte;
  if (read_config_byte(config_raw, config_raw_len, datalen, startbyte + 1, &bufbyte)) {
    return 1;
  }
  *config_short = bufbyte;
  *config_short <<= 8;
  if (read_config_byte(config_raw, config_raw_len, datalen, startbyte, &bufbyte)) {
    return 1;
  }
  *config_short |= bufbyte;
  return 0;
}

/* Read two consecutive shorts as an int */
static int read_config_int(
  uint8_t *config_raw,
  size_t config_raw_len,
  int datalen,
  int startbyte,
  uint32_t *config_int
)
{
  uint16_t bufshort;
  if (read_config_short(config_raw, config_raw_len, datalen, startbyte + 2, &bufshort)) {
    return 1;
  }
  *config_int = bufshort;
  *config_int <<= 16;
  if (read_config_short(config_raw, config_raw_len, datalen, startbyte, &bufshort)) {
    return 1;
  }
  *config_int |= bufshort;
  return 0;
}

rtems_status_code rtems_flash_CFI_parse_from_buffer(
  uint8_t *cfi_raw,
  size_t cfi_raw_len,
  rtems_flash_NOR_config_data *data
)
{
  int datalen;
  int region;
  uint16_t num_sectors_sub;
  uint16_t page_power;
  uint16_t sector_base;
  uint8_t bufbyte;
  uint8_t num_regions;

  memset(data, 0, sizeof(rtems_flash_NOR_config_data));

  if (cfi_raw_len < 4 || cfi_raw_len > 512) {
    return RTEMS_INVALID_SIZE;
  }

  data->jedec_id = ((cfi_raw[0] << 16) | (cfi_raw[1] << 8) | cfi_raw[2]);
  datalen = cfi_raw[3];
  if (datalen != 0) {
    /*
     * datalen as read is remaining valid bytes after 0x3 (the 4th byte), so
     * account for those bytes since this is used as an absolute index limit
     * */
    datalen += 4;
  }

  /* verify query string ("QRY") exists */
  if (verify_config_string(cfi_raw, cfi_raw_len, datalen, 0x10, "QRY")) {
    return RTEMS_INVALID_NAME;
  }

  /* get device geometry */
  /* Device size for at least s25fl512s is off by 1, calculate with sectors */
  if (read_config_byte(cfi_raw, cfi_raw_len, datalen, 0x27, &bufbyte)) {
    return RTEMS_INVALID_ADDRESS;
  }
  data->device_size = 1UL << bufbyte;

  if (read_config_short(cfi_raw, cfi_raw_len, datalen, 0x2a, &page_power)) {
    return RTEMS_INVALID_ADDRESS;
  }
  data->page_size = 1UL << page_power;

  if (read_config_byte(cfi_raw, cfi_raw_len, datalen, 0x2c, &bufbyte)) {
    return RTEMS_INVALID_ADDRESS;
  }

  /* Get largest block */
  num_regions = bufbyte;
  data->sector_size = 0;
  for (region = 0; region < num_regions; ++region) {
    if (read_config_short(cfi_raw, cfi_raw_len, datalen, 0x2d + (region * 4),
          &num_sectors_sub)) {
      return RTEMS_INVALID_ADDRESS;
    }

    if (read_config_short(cfi_raw, cfi_raw_len, datalen, 0x2f + (region * 4),
          &sector_base)) {
      return RTEMS_INVALID_ADDRESS;
    }

    if (data->sector_size < (sector_base * 256UL)) {
      data->num_sectors = num_sectors_sub + 1;
      data->sector_size = sector_base * 256UL;
    }
  }

  if (num_regions == 1) {
    /* Device size for at least s25fl512s is off by 1, calculate with sectors */
    data->device_size = data->num_sectors * data->sector_size;
  } else {
    data->num_sectors = data->device_size / data->sector_size;
  }

  return RTEMS_SUCCESSFUL;
}

static int parse_sfdp_jedec_table(
  uint8_t *tdata,
  size_t tlen,
  int version,
  rtems_flash_NOR_config_data *data
)
{
  int datalen = 0;
  int page_info_dword_offset;
  int page_info_offset;
  int raw_size_offset;
  int sect_erase_index;
  int sector_erase_dword_offset;
  int sector_erase_offset;
  int sector_info_offset;
  uint32_t raw_size;
  uint8_t bufbyte;
  /* locals to be applied once parsing is guaranteed successful */
  rtems_flash_NOR_config_data local = {0,};
  local.page_size = 256;

  /* Read raw size from second dword */
  raw_size_offset = 4;
  if (read_config_int(tdata, tlen, datalen, raw_size_offset, &raw_size)) {
    return -1;
  }
  if (raw_size & 0x80000000UL) {
    raw_size &= 0x7FFFFFFFUL;
    local.device_size = 1ULL << raw_size;
  } else {
    local.device_size = raw_size + 1;
  }
  /* Device size is specified in bits, convert to bytes */
  local.device_size /= 8;

  /* Parse sector information from dwords 8 and 9 */
  sector_erase_dword_offset = 7;
  sector_erase_offset = 4 * sector_erase_dword_offset;
  for (sect_erase_index = 0; sect_erase_index < 4; sect_erase_index++) {
    sector_info_offset = sector_erase_offset + 2 * sect_erase_index;
    if (read_config_byte(tdata, tlen, datalen, sector_info_offset, &bufbyte)) {
      return -1;
    }
    if (bufbyte == 0) {
      /* empty sector descriptor */
      continue;
    }
    int sector_size = 1UL << bufbyte;
    if (read_config_byte(tdata, tlen, datalen, sector_info_offset + 1, &bufbyte)) {
      return -1;
    }

    if (bufbyte == 0xD8) {
      /* This is the 'normal' sector size */
      local.sector_size = sector_size;
      local.num_sectors = (uint32_t)(local.device_size/local.sector_size);
    } else {
      /* store the alternate sector size/command */
      local.alt_sector_size = sector_size;
      local.alt_sector_cmd = bufbyte;
      local.num_alt_sectors = (uint32_t)(local.device_size/local.alt_sector_size);
    }
  }

  if (version >= 5) {
    page_info_dword_offset = 10;
    page_info_offset = 4 * page_info_dword_offset;
    if (read_config_byte(tdata, tlen, datalen, page_info_offset, &bufbyte)) {
      return -1;
    }
    bufbyte >>= 4;
    bufbyte &= 0xF;
    local.page_size = 1U << bufbyte;
  }

  /* Apply local parse data now that parsing has succeeded */
  *data = local;
  return 0;
}

static int parse_sfdp_basic_header(
  rtems_flash_NOR_config_accessor *accessor,
  uint8_t *header_base,
  rtems_flash_NOR_config_data *data
)
{
  int datalen = 0;
  int hlen = 8;
  int tlen;
  int version;
  uint32_t toffset = 0;
  uint8_t *table_data;
  uint8_t bufbyte;

  /* check minor version */
  if (read_config_byte(header_base, hlen, datalen, 0x1, &bufbyte)) {
    return -1;
  }
  version = bufbyte;

  /* get table length in 4-byte words */
  if (read_config_byte(header_base, hlen, datalen, 0x3, &bufbyte)) {
    return -1;
  }
  tlen = bufbyte;
  tlen *= 4;

  /* read table offset */
  if (read_config_int(header_base, hlen, datalen, 0x4, &toffset)) {
    return -1;
  }
  /* mask off the top byte */
  toffset &= 0xFFFFFFUL;

  table_data = accessor->acquire(accessor->context, toffset, tlen);
  if (table_data == NULL) {
    return RTEMS_INVALID_SIZE;
  }
  if (parse_sfdp_jedec_table(table_data, tlen, version, data)) {
    accessor->release(accessor->context, table_data);
    table_data = NULL;
    return -1;
  }
  accessor->release(accessor->context, table_data);
  table_data = NULL;
  return 0;
}

rtems_status_code rtems_flash_SFDP_parse(
  rtems_flash_NOR_config_accessor *accessor,
  rtems_flash_NOR_config_data *data
)
{
  int datalen = 0;
  int param_version_seen = -1;
  int pheader_index;
  int pheaders_count;
  int ptable_offset;
  int version;
  size_t header_len = 8;
  size_t ptd_len = 8;
  size_t total_ptd_len;
  uint8_t *header_base;
  uint8_t *header_data;
  uint8_t *ptable_desc;
  uint8_t bufbyte;

  memset(data, 0, sizeof(rtems_flash_NOR_config_data));

  header_data = accessor->acquire(accessor->context, 0, header_len);
  if (header_data == NULL) {
    return RTEMS_INVALID_SIZE;
  }

  /* verify SFDP ID string ("SFDP") exists */
  if (verify_config_string(header_data, header_len, datalen, 0x0, "SFDP")) {
    return RTEMS_INVALID_NAME;
  }

  /* check major version */
  if (read_config_byte(header_data, header_len, datalen, 0x05, &bufbyte)) {
    return RTEMS_INVALID_ADDRESS;
  }
  if (bufbyte != 1) {
    return RTEMS_TOO_MANY;
  }

  /* get count of parameter headers */
  if (read_config_byte(header_data, header_len, datalen, 0x06, &bufbyte)) {
    return RTEMS_INVALID_ADDRESS;
  }
  pheaders_count = bufbyte + 1;
  accessor->release(accessor->context, header_data);
  header_data = NULL;

  /* get parameter table descriptors */
  total_ptd_len = ptd_len * pheaders_count;
  ptable_desc = accessor->acquire(accessor->context, header_len, total_ptd_len);
  if (ptable_desc == NULL) {
    return RTEMS_INVALID_SIZE;
  }

  for (pheader_index = 0; pheader_index < pheaders_count; pheader_index++) {
    ptable_offset = pheader_index * ptd_len;
    header_base = ptable_desc + ptable_offset;

    /* only parse JEDEC SFDP Basic SPI Flash Parameter */
    if (read_config_byte(header_base, ptd_len, datalen, 0x0, &bufbyte)) {
      accessor->release(accessor->context, ptable_desc);
      ptable_desc = NULL;
      return -1;
    }
    if (bufbyte != 0) {
      continue;
    }

    /* check minor version */
    if (read_config_byte(header_base, ptd_len, datalen, 0x1, &bufbyte)) {
      accessor->release(accessor->context, ptable_desc);
      ptable_desc = NULL;
      return -1;
    }
    version = bufbyte;
    if (version > param_version_seen) {
      /* only parse if interested in this version */
      if (parse_sfdp_basic_header(accessor, header_base, data)) {
        /* parse failed, continue */
        continue;
      }
      param_version_seen = version;
    }
  }

  accessor->release(accessor->context, ptable_desc);
  ptable_desc = NULL;

  if (param_version_seen == -1) {
    return RTEMS_INVALID_ADDRESS;
  }
  return RTEMS_SUCCESSFUL;
}

struct buffer_context {
  uint8_t *data;
  size_t length;
};

static uint8_t *buffer_acquire(
  void *arg,
  uint32_t offset,
  size_t length
)
{
  struct buffer_context *context = arg;
  if (offset + length > context->length) return NULL;
  return context->data + offset;
}

static void buffer_release(
  void *arg,
  uint8_t *data
)
{
  (void) arg;
  (void) data;
  /* Do nothing */
}

rtems_status_code rtems_flash_SFDP_parse_from_buffer(
  uint8_t *sfdp_raw,
  size_t sfdp_raw_len,
  rtems_flash_NOR_config_data *data
)
{
  struct buffer_context context = {sfdp_raw, sfdp_raw_len};
  rtems_flash_NOR_config_accessor accessor;
  accessor.context = &context;
  accessor.acquire = buffer_acquire;
  accessor.release = buffer_release;
  return rtems_flash_SFDP_parse(&accessor, data);
}

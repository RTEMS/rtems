/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (C) 2022 Chris Johns (Contemporary Software)
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

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dev/pm/pm.h>
#include <dev/pm/pm-image.h>
#include <xilpdi/xilpdi.h>

#define PFI_BOOT_HEADER (0x3f0 - sizeof(uint32_t))

static const char* error_labels[] = {
  "success",
  "not supported",
  "invalid header length",
  "invalid header checksum",
  "invalid header",
};

static const uint8_t boot_header_X8[] = {
  0x00, 0x00, 0x00, 0xDD, 0x11, 0x22, 0x33, 0x44,
  0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC
};

static const uint8_t boot_header_X16[] = {
  0x00, 0x00, 0xDD, 0x00, 0x22, 0x11, 0x44, 0x33,
  0x66, 0x55, 0x88, 0x77, 0xAA, 0x99, 0xCC, 0xBB
};

static const uint8_t boot_header_X32[] = {
  0xDD, 0x00, 0x00, 0x00, 0x44, 0x33, 0x22, 0x11,
  0x88, 0x77, 0x66, 0x55, 0xCC, 0xBB, 0xAA, 0x99
};

const char* rtems_pm_image_error_text(rtems_pm_image_status status) {
  if (status < (sizeof(error_labels) / sizeof(const char*))) {
    return error_labels[status];
  }
  return "invalid error code";
}

static uint32_t rtems_pm_checksum(const uint32_t* words, size_t size) {
  uint32_t checksum = 0;
  size_t count;
  for (count = 0; count < size; ++count) {
    checksum += words[count];
  }
  return checksum ^ 0xffffffffU;
}

static rtems_pm_image_status rtems_pm_image_header_checksum(
  const void* header, size_t size) {
  const uint32_t* words = (const uint32_t*) header;
  uint32_t checksum;
  size /= sizeof(uint32_t);
  if (size < 2) {
    return RTEMS_PM_IMAGE_INVALID_LENGTH;
  }
  checksum = rtems_pm_checksum(header, size - 1);
  return
    words[size] == checksum ? RTEMS_PM_IMAGE_SUCCESS : RTEMS_PM_IMAGE_INVALID_CSUM;
}

rtems_pm_image_status rtems_pm_acap_verify_image_header(const void* header) {
  return rtems_pm_image_header_checksum(header, XIH_IHT_LEN - sizeof(uint32_t));
}

static rtems_pm_image_status rtems_pm_acap_verify_pdi_header(
  const void* header) {
  if (memcmp(header, boot_header_X8, sizeof(boot_header_X8)) == 0) {
    return RTEMS_PM_IMAGE_NOT_SUPPORTED;
  }
  if (memcmp(header, boot_header_X16, sizeof(boot_header_X16)) == 0) {
    return RTEMS_PM_IMAGE_NOT_SUPPORTED;
  }
  if (memcmp(header, boot_header_X32, sizeof(boot_header_X32)) != 0) {
    return RTEMS_PM_IMAGE_INVALID_HEADER;
  }
  return RTEMS_PM_IMAGE_SUCCESS;
}

rtems_pm_image_status rtems_pm_acap_verify_boot_header(const void* header) {
  XilPdi_BootHdr* bhdr;
  rtems_pm_image_status status;
  status = rtems_pm_acap_verify_pdi_header(header);
  if (status != RTEMS_PM_IMAGE_SUCCESS) {
    return status;
  }
  bhdr = (XilPdi_BootHdr*) (header + sizeof(boot_header_X32));
  if (bhdr->ImgIden != XIH_BH_IMAGE_IDENT) {
    return RTEMS_PM_IMAGE_INVALID_IDENT;
  }
  /*
   * This length is hardcoded as a value in bootgen. The SMAP header
   * is not included yet the table in UG1283 includes the SMAP header
   * the checksum offset in the table.
   */
  return rtems_pm_image_header_checksum(bhdr, 0xf20);
}

void rtems_pm_acap_image_print(const void* image) {
  int r = rtems_pm_acap_verify_image_header(image);
  if (r != RTEMS_PM_IMAGE_SUCCESS) {
    printf("error: image header: %s\n", rtems_pm_image_error_text(r));
    return;
  }
}

const void* rtems_pm_acap_image_header(const void* image) {
  rtems_pm_image_status status;
  XilPdi_ImgHdrTbl* ihdrtab;
  ihdrtab = (XilPdi_ImgHdrTbl*) (image + sizeof(boot_header_X32));
  status = rtems_pm_acap_verify_image_header(ihdrtab);
  if (status != RTEMS_PM_IMAGE_SUCCESS) {
    printf("error: ACAP image header: %s\n", rtems_pm_image_error_text(status));
    return NULL;
  }
  return ihdrtab;
}

void rtems_pm_acap_print(const void* image, size_t size) {
  rtems_pm_image_status status;
  XilPdi_BootHdr* bhdr;
  XilPdi_ImgHdrTbl* ihdrtab;
  XilPdi_ImgHdr* ihdr;
  XilPdi_PrtnHdr* phdr;
  int max = 0;
  uint32_t u;
  status = rtems_pm_acap_verify_boot_header(image);
  if (status == RTEMS_PM_IMAGE_SUCCESS) {
    bhdr = (XilPdi_BootHdr*) (image + sizeof(boot_header_X32));
    printf("ACAP (PDI) Image @ %p len: %zu\n", image, size);
    printf(" PLM:\n");
    printf("  PLM source offset  : 0x%08" PRIx32 "\n", bhdr->DpiSrcOfst);
    printf("  PMC load offset    : 0x%08" PRIx32 "\n", bhdr->DpiStartOfst);
    printf("  PMC length         : %" PRIu32 "\n", bhdr->DataPrtnLen);
    printf("  PMC total length   : %" PRIu32 "\n", bhdr->TotalDataPrtnLen);
    printf("  PLM length         : %" PRIu32 "\n", bhdr->PlmLen);
    printf("  PLM total length   : %" PRIu32 "\n", bhdr->TotalPlmLen);
    printf(" Boot attributes     : 0x%08" PRIx32 "\n", bhdr->ImgAttrb);
    printf(" PMC metadata offset : 0x%08" PRIx32 "\n", bhdr->BootHdrFwRsvd.MetaHdrOfst);
    ihdrtab = (XilPdi_ImgHdrTbl*) (image + bhdr->BootHdrFwRsvd.MetaHdrOfst);
  } else {
    ihdrtab = (XilPdi_ImgHdrTbl*) (image + sizeof(boot_header_X32));
  }
  status = rtems_pm_acap_verify_image_header(ihdrtab);
  if (status != RTEMS_PM_IMAGE_SUCCESS) {
    printf("error: ACAP boot header: %s\n", rtems_pm_image_error_text(status));
    return;
  }
  printf(" Image Header Table (metadata):\n");
  printf("  Version            : %i.%i\n", ihdrtab->Version >> 16, ihdrtab->Version & 0xffff);
  printf("  Images             : %" PRIu32 "\n", ihdrtab->NoOfImgs);
  printf("  Image offset       : 0x%08" PRIx32 "\n", ihdrtab->ImgHdrAddr);
  printf("  Paritions          : %" PRIu32 "\n", ihdrtab->NoOfPrtns);
  printf("  Partition offset   : 0x%08" PRIx32 "\n", ihdrtab->PrtnHdrAddr);
  printf("  Device Id          : 0x%08" PRIx32 "\n", ihdrtab->Idcode);
  printf("  Attributes         : 0x%08" PRIx32 "\n", ihdrtab->Attr);
  printf("  Parition Id        : %" PRIu32 "\n", ihdrtab->PdiId);
  ihdr = (XilPdi_ImgHdr*) (image + ihdrtab->ImgHdrAddr * XIH_PRTN_WORD_LEN);
  for (u = 0; u < ihdrtab->NoOfImgs; ++u) {
    int len = strnlen((char*) ihdr[u].ImgName, 64);
    if (len > max) {
      max = len;
    }
  }
  printf(" Images:\n");
  printf("    %-*s parts node id  uid      puid     func id  DDR addr\n", max, "name");
  for (u = 0; u < ihdrtab->NoOfImgs; ++u) {
    printf(
      " %2u %-*s %-5" PRIu32 " %08" PRIx32 " %08" PRIx32 " %08" PRIx32 " %08" PRIx32 "\n",
      u, max, ihdr[u].ImgName, ihdr[u].NoOfPrtns, ihdr[u].ImgID, ihdr[u].UID, ihdr[u].PUID,
      ihdr[u].FuncID);
  }
  phdr = (XilPdi_PrtnHdr*) (image + ihdrtab->PrtnHdrAddr * XIH_PRTN_WORD_LEN);
  printf(" Partitions:\n");
  printf("    id   secs offset   load addr        exec addr\n");
  for (u = 0; u < ihdrtab->NoOfPrtns; ++u) {
    printf(
      " %2u %-4" PRIu32 " %-4" PRIu32 " %08" PRIx32 " %016" PRIx64 " %016" PRIx64 "\n",
      u, phdr[u].PrtnId, phdr[u].SectionCount, phdr[u].DataWordOfst * XIH_PRTN_WORD_LEN,
      phdr[u].DstnLoadAddr, phdr[u].DstnExecutionAddr);
  }
}

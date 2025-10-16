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

#ifndef _RTEMS_DEV_PM_PM_IMAGE_H
#define _RTEMS_DEV_PM_PM_IMAGE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PM_GET_FPGA_STATUS_RESET   (1U << 6)
#define PM_GET_FPGA_STATUS_EOS     (1U << 4)
#define PM_GET_FPGA_STATUS_DONE    (1U << 3)
#define PM_GET_FPGA_STATUS_INIT    (1U << 2)
#define PM_GET_FPGA_STATUS_RD_IDLE (1U << 1)
#define PM_GET_FPGA_STATUS_WR_IDLE (1U << 0)

typedef enum {
  RTEMS_PM_IMAGE_SUCCESS,
  RTEMS_PM_IMAGE_NOT_SUPPORTED,
  RTEMS_PM_IMAGE_INVALID_LENGTH,
  RTEMS_PM_IMAGE_INVALID_CSUM,
  RTEMS_PM_IMAGE_INVALID_HEADER,
  RTEMS_PM_IMAGE_INVALID_IDENT,
} rtems_pm_image_status;

const char* rtems_pm_image_error_text(rtems_pm_image_status status);
rtems_pm_image_status rtems_pm_pdi_header_checksum(
  const void* header, size_t size);

rtems_pm_image_status rtems_pm_acap_verify_image_header(const void* header);
rtems_pm_image_status rtems_pm_acap_verify_boot_header(const void* header);

const void* rtems_pm_acap_image_header(const void* image);

void rtems_pm_acap_image_print(const void* image);
void rtems_pm_acap_print(const void* image, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_DEV_PM_PM_IMAGE_H */

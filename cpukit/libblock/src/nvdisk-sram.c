/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup libblock
 *
 * @brief Provide SRAM support for the NV Disk
 */

/*
 * Copyright 2007 Chris Johns (chrisj@rtems.org)
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <rtems.h>

#include <rtems/nvdisk-sram.h>

#ifndef NVDISK_SRAM_ERROR_TRACE
#define NVDISK_SRAM_ERROR_TRACE (0)
#endif

static int
rtems_nvdisk_sram_read (uint32_t device RTEMS_UNUSED,
                        uint32_t flags RTEMS_UNUSED,
                        void*    base,
                        uint32_t offset,
                        void*    buffer,
                        size_t   size)
{
  memcpy (buffer, (base + offset), size);
  return 0;
}

static int
rtems_nvdisk_sram_write (uint32_t    device RTEMS_UNUSED,
                         uint32_t    flags RTEMS_UNUSED,
                         void*       base,
                         uint32_t    offset,
                         const void* buffer,
                         size_t      size)
{
  memcpy ((base + offset), buffer, size);
  return 0;
}

static int
rtems_nvdisk_sram_verify (uint32_t    device RTEMS_UNUSED,
                          uint32_t    flags RTEMS_UNUSED,
                          void*       base,
                          uint32_t    offset,
                          const void* buffer,
                          size_t      size)
{
  return memcmp ((base + offset), buffer, size) == 0 ? 0 : EIO;
}


const rtems_nvdisk_driver_handlers rtems_nvdisk_sram_handlers =
{
  .read   = rtems_nvdisk_sram_read,
  .write  = rtems_nvdisk_sram_write,
  .verify = rtems_nvdisk_sram_verify
};

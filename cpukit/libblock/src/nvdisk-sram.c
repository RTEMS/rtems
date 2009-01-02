/*
 *  $Id$
 *
 * RTEMS Project (http://www.rtems.org/)
 *
 * Copyright 2007 Chris Johns (chrisj@rtems.org)
 */
/**
 * Provide SRAM support for the NV Disk.
 */

#if HAVE_CONFIG_H
#include "config.h" 
#endif

#include <stdio.h>
#include <errno.h>

#include <rtems.h>

#include <rtems/nvdisk-sram.h>

#ifndef NVDISK_SRAM_ERROR_TRACE
#define NVDISK_SRAM_ERROR_TRACE (0)
#endif

static int
rtems_nvdisk_sram_read (uint32_t device __attribute__((unused)),
                        uint32_t flags __attribute__((unused)),
                        uint32_t base, 
                        uint32_t offset,
                        void*    buffer,
                        uint32_t size)
{
  memcpy (buffer, (char*) (base + offset), size);
  return 0;
}

static int
rtems_nvdisk_sram_write (uint32_t    device __attribute__((unused)),
                         uint32_t    flags __attribute__((unused)),
                         uint32_t    base, 
                         uint32_t    offset,
                         const void* buffer,
                         uint32_t    size)
{
  memcpy ((char*) (base + offset), buffer, size);
  return 0;
}

static int
rtems_nvdisk_sram_verify (uint32_t    device __attribute__((unused)),
                          uint32_t    flags __attribute__((unused)),
                          uint32_t    base, 
                          uint32_t    offset,
                          const void* buffer,
                          uint32_t    size)
{
  return memcmp ((char*) (base + offset), buffer, size) == 0 ? 0 : EIO;
}


const rtems_nvdisk_driver_handlers rtems_nvdisk_sram_handlers =
{
  read:   rtems_nvdisk_sram_read,
  write:  rtems_nvdisk_sram_write,
  verify: rtems_nvdisk_sram_verify
};

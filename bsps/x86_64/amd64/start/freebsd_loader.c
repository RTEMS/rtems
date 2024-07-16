/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsX8664AMD64
 *
 * @brief Routines for parsing information passed by the FreeBSD bootloader
 */

/*
 * Copyright (C) 2024 Matheus Pecoraro
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

#include <acpi/acpi.h>
#include <freebsd_loader.h>

#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#define MODINFO_NAME    0x0001    /* Name of module (string) */
#define MODINFO_TYPE    0x0002    /* Type of module (string) */

#define MODINFOMD_ENVP    0x0006  /* env variables (envp[]) */
#define MODINFO_METADATA  0x8000  /* Module-specfic */

static char* modules_metadata = NULL;

/**
 * Retrieves the handle to the metadata of the module.
 *
 * This routine was taken from sys/kern/subr_module.c in the FreeBSD source code
 * and modified for readability and coding convention purposes.
 *
 * Copyright (c) 1998 Michael Smith
 * All rights reserved.
 * Copyright (c) 2020 NetApp Inc.
 * Copyright (c) 2020 Klara Inc.
 */
static const char* modules_metadata_search_by_type(const char* type)
{
  if (modules_metadata != NULL) {
    const char* curr_addr = modules_metadata;
    const char* last_name = NULL;

    for (;;) {
      int next;
      uint32_t* hdr = (uint32_t*) curr_addr;

      if (hdr[0] == 0 && hdr[1] == 0) {
        break;
      }

      /* Remember the start of each record */
      if (hdr[0] == MODINFO_NAME) {
        last_name = curr_addr;
      }

      /* Search for a MODINFO_TYPE field */
      if ((hdr[0] == MODINFO_TYPE) &&
          !strcmp(type, curr_addr + sizeof(uint32_t) * 2))
      {
        return last_name;
      }

      /* Skip to next field */
      next = sizeof(uint32_t) * 2 + hdr[1];
      next = roundup(next, sizeof(u_long));
      curr_addr += next;
    }
  }

  return NULL;
}

/**
 * Given a module metadata handle return the specified attribute
 *
 * This routine was taken from sys/kern/subr_module.c in the FreeBSD source code
 * and modified for readability and coding convention purposes.
 *
 * Copyright (c) 1998 Michael Smith
 * All rights reserved.
 * Copyright (c) 2020 NetApp Inc.
 * Copyright (c) 2020 Klara Inc.
 */
static const char* module_metadata_search_info(const char* mod, int info)
{
  const char* curr_addr;
  uint32_t type = 0;

  if (mod == NULL) {
    return (NULL);
  }

  curr_addr = mod;
  for (;;) {
    uint32_t *hdr;
    int next;

    hdr = (uint32_t*) curr_addr;

    /* End of module data? */
    if (hdr[0] == 0 && hdr[1] == 0) {
        break;
    }

    /*
     * We give up once we've looped back to what we were looking at
     * first - this should normally be a MODINFO_NAME field.
     */
    if (type == 0) {
        type = hdr[0];
    }
    else if (hdr[0] == type) {
      break;
    }

    /*
     * Attribute match? Return pointer to data.
     * Consumer may safely assume that size value precedes data.
     */
    if (hdr[0] == info)
      return(curr_addr + (sizeof(uint32_t) * 2));

    /* Skip to next field */
    next = sizeof(uint32_t) * 2 + hdr[1];
    next = roundup(next, sizeof(u_long));
    curr_addr += next;
  }

  return NULL;
}

static const char* get_static_env(const char* envp, const char* name)
{
  size_t name_len = strlen(name);

  while (*envp != '\0') {
    if (strncmp(envp, name, name_len) == 0 && envp[name_len] == '=') {
      return envp + name_len + 1;
    }

    while (*envp != '\0') {
      envp++;
    }
    envp++;
  }

  return NULL;
}

void retrieve_info_from_freebsd_loader(uint32_t modules_metadata_addr)
{
  const char* kernel_mod = NULL;
  const char* envp = NULL;
  const char* rsdp_str = NULL;
  long rsdp_addr = 0;

  modules_metadata = (char*) ((uint64_t) modules_metadata_addr);

  kernel_mod = modules_metadata_search_by_type("elf kernel");
  if (kernel_mod == NULL) {
    kernel_mod = modules_metadata_search_by_type("elf64 kernel");
  }

  envp = module_metadata_search_info(kernel_mod, MODINFO_METADATA | MODINFOMD_ENVP);
  if (envp != NULL) {
    envp = (char*) *((uint64_t*) envp);
  }

  rsdp_str = get_static_env(envp, "acpi.rsdp");
  if (rsdp_str != NULL) {
    char* end_ptr;
    rsdp_addr = strtol(rsdp_str, &end_ptr, 16);
    acpi_rsdp_addr = rsdp_addr;
  }
}

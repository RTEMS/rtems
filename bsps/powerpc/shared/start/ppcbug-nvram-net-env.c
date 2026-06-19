/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2026 Chris Johns <chrisj@rtems.org>.  All rights reserved.
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

#include <stdio.h>
#include <stdlib.h>

#include <bsp/ppcbug-nvram-net-env.h>

/*
 * The NVRAM Variables has held in PPCBug memory
 */
struct ppcbug_nvram {
  uint32_t packet_version_identifier;
  uint32_t node_control_memory_address;
  uint32_t bootfile_load_address;
  uint32_t bootfile_exec_address;
  uint32_t bootfile_exec_delay;
  uint32_t bootfile_length;
  uint32_t bootfile_byte_offset;
  uint32_t trace_buffer_address;
  uint32_t client_ip;
  uint32_t server_ip;
  uint32_t netmask;
  uint32_t broadcast;
  uint32_t gateway_ip;
  uint8_t bootp_rarp_retry;
  uint8_t tftp_rarp_retry;
  uint8_t bootp_rarp_control;
  uint8_t update_control;
  char bootfile[64];
  char arguments[64];
};

typedef struct ppcbug_nvram ppcbug_nvram_data;

static int indirect_get(uintptr_t nvram_base, ppcbug_nvram_data* data) {
  volatile uint8_t* iaddr_l = (volatile uint8_t*)nvram_base;
  volatile uint8_t* iaddr_h = (volatile uint8_t*)nvram_base + 1;
  volatile uint8_t* idata = (volatile uint8_t*)nvram_base + 3;
  int offset = 0x1000;
  uint8_t* dst = (uint8_t*)data;
  const uint8_t* end = dst + sizeof(ppcbug_nvram_data);
  while (dst < end) {
    *iaddr_l = offset & 0xff;
    *iaddr_h = (offset >> 8) & 0xff;
    *dst++ = *idata;
    ++offset;
  }
  return 0;
}

static int direct_get(uintptr_t nvram_base, ppcbug_nvram_data* data) {
  volatile uint8_t* src = (volatile uint8_t*)nvram_base;
  uint8_t* dst = (uint8_t*)data;
  const uint8_t* end = dst + sizeof(ppcbug_nvram_data);
  while (dst < end) {
    *dst++ = *src++;
  }
  return 0;
}

static int nvram_get(uintptr_t nvram_base, bool indirect,
                     ppcbug_nvram_data* data) {
  if (indirect) {
    return indirect_get(nvram_base, data);
  }
  return direct_get(nvram_base, data);
}

static int ip_str(char* buf, const size_t buf_len, uint32_t addr) {
  if (addr == 0) {
    return -1;
  }
  snprintf(buf, buf_len, "%i.%i.%i.%i", (int)((addr >> 24) & 0xff),
           (int)((addr >> 16) & 0xff), (int)((addr >> 8) & 0xff),
           (int)((addr >> 0) & 0xff));
  return 0;
}

static void populate_env(const size_t unit, const char* env_name,
                         const char* var) {
  char name[128];
  snprintf(name, sizeof(name), env_name, unit);
  setenv(name, &var[0], 1);
}

static void ip_env(const size_t unit, const char* env_name,
                   const uint32_t addr) {
  char ip[16];
  int r = ip_str(ip, sizeof(ip), addr);
  if (r == 0) {
    populate_env(unit, env_name, &ip[0]);
  }
}

void ppcbug_nvram_set_net_envs(uintptr_t nvram_base, bool indirect,
                               const size_t unit, const char* label) {
  ppcbug_nvram_data nvram;
  int r;
  if (unit < 1) {
    return;
  }
  r = nvram_get(nvram_base, indirect, &nvram);
  if (r == 0) {
    populate_env(unit, "RTEMS_NET_IFACE_%i", label);
    ip_env(unit, "RTEMS_NET_IF_%i_IP_ADDR", nvram.client_ip);
    ip_env(unit, "RTEMS_NET_IF_%i_NETMASK", nvram.netmask);
    ip_env(unit, "RTEMS_NET_SERVER_IP", nvram.server_ip);
    ip_env(unit, "RTEMS_NET_GATEWAY_IP", nvram.gateway_ip);
    ip_env(unit, "RTEMS_NET_HOSTNAME", nvram.client_ip);
    ip_env(unit, "RTEMS_NET_NTP_IP", nvram.server_ip);
    ip_env(unit, "RTEMS_NET_DNS_IP", nvram.server_ip);
    populate_env(unit, "RTEMS_NET_BOOT_FILE", nvram.bootfile);
    populate_env(unit, "RTEMS_BOOT_CMD_LINE", nvram.arguments);
    populate_env(unit, "RTEMS_NFS_MOUNT_PATH", nvram.arguments);
  }
}

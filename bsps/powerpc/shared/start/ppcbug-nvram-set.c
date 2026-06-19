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

#include <string.h>

#include <rtems/sysinit.h>

#include <bsp/ppcbug-nvram-net-env.h>
#include <bsp/ppcbug-nvram-set.h>

#define MAX_NET_UNITS (4)

struct net_unit_data {
  bool valid;
  char label[8];
};

static bool init_run;
static uintptr_t nvram_base;
static bool nvram_indirect;
static struct net_unit_data net_units[MAX_NET_UNITS];

static void ppcbug_nvram_scan(void) {
  init_run = true;
  if (nvram_base != 0) {
    for (size_t unit = 0; unit < MAX_NET_UNITS; ++unit) {
      if (net_units[unit].valid && net_units[unit].label[0] != '\0') {
        ppcbug_nvram_set_net_envs(nvram_base, unit + 1, nvram_indirect,
                                  net_units[unit].label);
      }
    }
  }
}

void ppcbug_nvram_set_nvbase(uintptr_t nvram_base_, bool nvram_indirect_) {
  nvram_base = nvram_base_;
  nvram_indirect = nvram_indirect_;
  if (init_run && nvram_base != 0) {
    ppcbug_nvram_scan();
  }
}

void ppcbug_nvram_set_net_unit(const size_t unit) {
  if (unit - 1 < MAX_NET_UNITS) {
    net_units[unit - 1].valid = true;
    if (init_run && nvram_base != 0 && net_units[unit - 1].label[0] != '\0') {
      ppcbug_nvram_set_net_envs(nvram_base, unit, nvram_indirect,
                                net_units[unit - 1].label);
    }
  }
}

void ppcbug_nvram_set_net_label(const size_t unit, const char* label) {
  if (unit - 1 < MAX_NET_UNITS) {
    strlcpy(&net_units[unit - 1].label[0], label,
            sizeof(net_units[unit - 1].label));
    if (init_run && nvram_base != 0 && net_units[unit - 1].valid) {
      ppcbug_nvram_set_net_envs(nvram_base, unit, nvram_indirect,
                                net_units[unit - 1].label);
    }
  }
}

RTEMS_SYSINIT_ITEM(ppcbug_nvram_scan, RTEMS_SYSINIT_LAST,
                   RTEMS_SYSINIT_ORDER_LAST);

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

#include <bsp/motload-gev-net-env.h>
#include <bsp/motload-gev.h>

struct gev_net_var {
  char server[32];
  char ip[32];
  char netmask[32];
  char gateway[32];
  char bootfile[64];
};

static void copy_opt(char* out, const size_t out_len, const char* in,
                     const char* in_end, const char* opt,
                     const size_t opt_len) {
  const char* out_end = out + out_len;
  const size_t in_len = in_end - in;
  memset(out, 0, out_len);
  char* op = strnstr(in, opt, in_len);
  if (op != NULL) {
    char* i = op + opt_len;
    while (*i != ' ' && *i != '\0' && i < in_end && out < out_end) {
      *out++ = *i++;
    }
  }
}

static void populate_env(uintptr_t nvram_base, const size_t unit,
                         const char* env_name, const char* gev_name,
                         const char* alt_var, char* var, const size_t var_len) {
  char name[32];
  int r;
  snprintf(name, sizeof(name), gev_name, unit);
  r = motload_gev_get(nvram_base, name, var, var_len);
  snprintf(name, sizeof(name), env_name, unit);
  if (r > 0) {
    setenv(name, &var[0], 1);
  } else if (alt_var != NULL && alt_var[0] != '\0') {
    setenv(name, &alt_var[0], 1);
  }
}

static void process_epics_script(uintptr_t nvram_base, char* var,
                                 const size_t var_len) {
  int r = motload_gev_get(nvram_base, "epics-script", var, var_len);
  if (r > 0) {
    /*
     * The `epics-script` is of the form nfs_server:nfs_export:<path>.
     *
     * The `nfs_export` component will be used as:
     *  - the path to the directory exported from the NFS server
     *  - the local mount point
     *  - a prefix of <path>
     *
     * For example, the argument string:
     *  `romeo:/export/users:smith/ioc/iocexample/st.cmd`
     * would split into:
     *  - mount `/export/users` from NFS server `romeo` on
     -    `/export/users`
     *  - boot cmd line is mount point plus <path> so
     *    `/export/users/smith/ioc/iocexample/st.cmd`
     */
    char* nfs_server = &var[0];
    char* nfs_export = strchr(nfs_server, ':');
    if (nfs_export != NULL) {
      *nfs_export = '\0';
      ++nfs_export;
      /* Check for `romeo:/export/users` */
      char* nfs_bootfile = strchr(nfs_export, ' ');
      if (nfs_bootfile == NULL) {
        /* Check for `/export/users:smith/ioc/....` */
        nfs_bootfile = strchr(nfs_export, ':');
        if (nfs_bootfile != NULL) {
          *nfs_bootfile = '/';
        }
      }
      if (nfs_bootfile != NULL) {
        setenv("RTEMS_BOOT_SCRIPT", nfs_export, 1);
        *nfs_bootfile = '\0';
      }
      *(nfs_export - 1) = ':';
      setenv("RTEMS_NFS_MOUNT_PATH", nfs_server, 1);
    } else {
      printf("error: GEV epics-script: no server delimiter found\n");
    }
  }
}

void motload_gev_set_net_envs(uintptr_t nvram_base, const size_t unit,
                              const char* label) {
  /*
   * The boot script is parsed for `tftpGet` settings -s -g and -m
   * and the values are used if there are no explictic names set.
   */
  char var[256];
  struct gev_net_var vars;
  int r;
  if (unit < 1) {
    return;
  }
  memset(&vars, 0, sizeof(vars));
  snprintf(&var[0], sizeof(var), "RTEMS_NET_IFACE_%i", unit);
  setenv(var, label, 1);
  r = motload_gev_get(nvram_base, "mot-script-boot", &var[0], sizeof(var));
  if (r > 0) {
    const char* tftp_get = strnstr(var, "tftpGet", r);
    if (tftp_get != NULL) {
      const char* tftp_get_end = strnstr(tftp_get, "\n", r);
      char dev_name[32];
      char dev[32];
      if (tftp_get_end == NULL) {
        tftp_get_end = tftp_get + r;
      }
      snprintf(&dev_name[0], sizeof(dev_name), "/dev/enet%i", unit);
      copy_opt(&dev[0], sizeof(dev), tftp_get, tftp_get_end, "-d", 2);
      if (strcmp(dev_name, dev) == 0) {
        copy_opt(&vars.server[0], sizeof(vars.server), tftp_get, tftp_get_end,
                 "-s", 2);
        copy_opt(&vars.ip[0], sizeof(vars.ip), tftp_get, tftp_get_end, "-c", 2);
        copy_opt(&vars.netmask[0], sizeof(vars.netmask), tftp_get, tftp_get_end,
                 "-m", 2);
        copy_opt(&vars.gateway[0], sizeof(vars.gateway), tftp_get, tftp_get_end,
                 "-g", 2);
        copy_opt(&vars.bootfile[0], sizeof(vars.bootfile), tftp_get,
                 tftp_get_end, "-f", 2);
      }
    }
  }
  populate_env(nvram_base, unit, "RTEMS_NET_IF_%i_IP_ADDR",
               "mot-/dev/enet%i-cipa", &vars.ip[0], &var[0], sizeof(var));
  populate_env(nvram_base, unit, "RTEMS_NET_IF_%i_NETMASK",
               "mot-/dev/enet%i-snma", &vars.netmask[0], &var[0], sizeof(var));
  populate_env(nvram_base, unit, "RTEMS_NET_SERVER_IP", "mot-/dev/enet%i-sipa",
               &vars.server[0], &var[0], sizeof(var));
  populate_env(nvram_base, unit, "RTEMS_NET_GATEWAY_IP", "mot-/dev/enet%i-gipa",
               &vars.gateway[0], &var[0], sizeof(var));
  populate_env(nvram_base, unit, "RTEMS_NET_HOSTNAME", "rtems-client-name",
               NULL, &var[0], sizeof(var));
  populate_env(nvram_base, unit, "RTEMS_NET_NTP_IP", "epics-ntpserver",
               &vars.server[0], &var[0], sizeof(var));
  populate_env(nvram_base, unit, "RTEMS_NET_DNS_IP", "rtems-dns-server",
               &vars.server[0], &var[0], sizeof(var));
  populate_env(nvram_base, unit, "RTEMS_NET_DOMAIN_NAME",
               "rtems-dns-domainname", NULL, &var[0], sizeof(var));
  populate_env(nvram_base, unit, "RTEMS_NET_BOOT_FILE", "mot-/dev/enet%i-file",
               &vars.bootfile[0], &var[0], sizeof(var));
  populate_env(nvram_base, unit, "RTEMS_BOOT_SCRIPT", "mot-script-boot", NULL,
               &var[0], sizeof(var));
  populate_env(nvram_base, unit, "RTEMS_BOOT_CMD_LINE", "epics-script", NULL,
               &var[0], sizeof(var));
  populate_env(nvram_base, unit, "RTEMS_NFS_MOUNT_PATH", "epics-nfsmount", NULL,
               &var[0], sizeof(var));
  populate_env(nvram_base, unit, "TZ", "epics-tz", NULL, &var[0], sizeof(var));
  process_epics_script(nvram_base, &var[0], sizeof(var));
}

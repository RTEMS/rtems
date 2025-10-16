/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (V) 2022 Contemporary Software (Chris Johns)
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

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/error.h>
#include <rtems/shell.h>

#include <dev/pm/pm.h>
#include <dev/pm/pm-image.h>
#include <dev/pm/pm-trace.h>

typedef struct
{
  const char* subcmd;
  const char* help_brief;
  int (*command) (int argc, char *argv[]);
  void (*help) (int argc, char *argv[]);
} pm_shell_subcmd;

#define NUMOF(_a) (sizeof(_a) / sizeof(_a[0]))

static const char* api_id_labels[PM_API_MAX] = {
  "PM_GET_API_VERSION",
  "PM_SET_CONFIGURATION",
  "PM_GET_NODE_STATUS",
  "PM_GET_OP_CHARACTERISTIC",
  "PM_REGISTER_NOTIFIER",
  "PM_REQUEST_SUSPEND",
  "PM_SELF_SUSPEND",
  "PM_FORCE_POWERDOWN",
  "PM_ABORT_SUSPEND",
  "PM_REQUEST_WAKEUP",
  "PM_SET_WAKEUP_SOURCE",
  "PM_SYSTEM_SHUTDOWN",
  "PM_REQUEST_NODE",
  "PM_RELEASE_NODE",
  "PM_SET_REQUIREMENT",
  "PM_SET_MAX_LATENCY",
  "PM_RESET_ASSERT",
  "PM_RESET_GET_STATUS",
  "PM_MMIO_WRITE",
  "PM_MMIO_READ",
  "PM_INIT_FINALIZE",
  "PM_FPGA_LOAD",
  "PM_FPGA_GET_STATUS",
  "PM_GET_CHIPID",
  "PM_SECURE_RSA_AES",
  "PM_SECURE_SHA",
  "PM_SECURE_RSA",
  "PM_PINCTRL_REQUEST",
  "PM_PINCTRL_RELEASE",
  "PM_PINCTRL_GET_FUNCTION",
  "PM_PINCTRL_SET_FUNCTION",
  "PM_PINCTRL_CONFIG_PARAM_GET",
  "PM_PINCTRL_CONFIG_PARAM_SET",
  "PM_IOCTL",
  "PM_QUERY_DATA",
  "PM_CLOCK_ENABLE",
  "PM_CLOCK_DISABLE",
  "PM_CLOCK_GETSTATE",
  "PM_CLOCK_SETDIVIDER",
  "PM_CLOCK_GETDIVIDER",
  "PM_CLOCK_SETRATE",
  "PM_CLOCK_GETRATE",
  "PM_CLOCK_SETPARENT",
  "PM_CLOCK_GETPARENT",
  "PM_SECURE_IMAGE",
  "PM_FPGA_READ",
  "PM_API_RESERVED_1",
  "PM_PLL_SET_PARAMETER",
  "PM_PLL_GET_PARAMETER",
  "PM_PLL_SET_MODE",
  "PM_PLL_GET_MODE",
  "PM_REGISTER_ACCESS",
  "PM_EFUSE_ACCESS",
  "PM_ADD_SUBSYSTEM",
  "PM_DESTROY_SUBSYSTEM",
  "PM_DESCRIBE_NODES",
  "PM_ADD_NODE",
  "PM_ADD_NODE_PARENT",
  "PM_ADD_NODE_NAME",
  "PM_ADD_REQUIREMENT",
  "PM_SET_CURRENT_SUBSYSTEM",
  "PM_INIT_NODE",
  "PM_FEATURE_CHECK",
  "PM_ISO_CONTROL",
  "PM_ACTIVATE_SUBSYSTEM",
  "PM_WRITE_AES_KEY",
  "PM_LOAD_PDI",
  "PM_GET_UID_INFO_LIST",
  "PM_GET_META_HEADER_INFO_LIST",
  "GET_CALLBACK_DATA",
  "PM_SET_SUSPEND_MODE",
  "PM_GET_TRUSTZONE_VERSION",
  "TF_A_PM_REGISTER_SGI",
  "PM_BBRAM_WRITE_KEY",
  "PM_BBRAM_ZEROIZE",
  "PM_BBRAM_WRITE_USERDATA",
  "PM_BBRAM_READ_USERDATA",
  "PM_BBRAM_LOCK_USERDATA",
};

typedef struct {
  void* image;
  size_t size;
} image_loader;

static int pm_image_load(const char* name, image_loader* image) {
  struct stat sb;
  char* in;
  ssize_t size;
  int fd;
  int r;
  image->image = NULL;
  image->size = 0;
  r = stat(name, &sb);
  if (r < 0) {
    printf("error: stat: %s: %s\n", name, strerror(errno));
    return errno;
  }
  image->size = sb.st_size;
  image->image = malloc(image->size);
  if (image->image == NULL) {
    printf("error: no memory for image buffer\n");
    return ENOMEM;
  }
  printf("load: %s size: %zu bytes\n", name, image->size);
  fd = open(name, O_RDONLY);
  if (fd < 0) {
    free(image->image);
    image->image = NULL;
    printf("error: open: %s: %s\n", name, strerror(errno));
    return errno;
  }
  #define LOAD_BLOCK_SIZE (32 * 1024)
  in = image->image;
  size = image->size;
  while (size > 0) {
    ssize_t block = size > LOAD_BLOCK_SIZE ? LOAD_BLOCK_SIZE : size;
    ssize_t read_in = read(fd, in, block);
    if (read_in != block) {
      close(fd);
      free(image->image);
      if (read_in < 0) {
        printf("error: read: %s: %s\n", name, strerror(errno));
        return errno;
      }
      printf("error: read: %s: block read too small\n", name);
      return EIO;
    }
    size -= block;
    in += block;
  }
  close(fd);
  return 0;
}

static void pm_image_free(image_loader* image) {
  free(image->image);
}

static int pm_subcmd_help(
  const char* command, const pm_shell_subcmd subcmds[], const size_t count,
  int argc, char* argv[]) {
  (void) argv;
  size_t i;
  if (argc == 0) {
    size_t len = 0;
    if (command == NULL) {
      printf("pm command [options] ...\n");
    } else {
      printf("pm %s command [options] ...\n", command);
    }
    printf(" where command is:\n");
    for (i = 0; i < count; ++i) {
      size_t sl = strlen(subcmds[i].subcmd);
      if (sl > len) {
        len = sl;
      }
    }
    if (command == NULL) {
      printf("  %-*s : this help\n", (int) len, "help");
    }
    for (i = 0; i < count; ++i) {
      const pm_shell_subcmd* subcmd = &subcmds[i];
      printf("  %-*s : %s\n", (int) len, subcmd->subcmd, subcmd->help_brief);
    }
  }
  return 0;
}

static int pm_shell_subcommand(
  const char* command, const pm_shell_subcmd subcmds[], const size_t count,
  int argc, char* argv[]) {
  int match = -1;
  size_t i;
  if (argc == 0 || strcmp(argv[0], "help") == 0) {
    if (argc > 0) {
      --argc;
      ++argv;
    }
    return pm_subcmd_help(command, subcmds, count, argc, argv);
  }
  for (i = 0; i < count; ++i) {
    const pm_shell_subcmd* subcmd = &subcmds[i];
    size_t len = strlen(argv[0]);
    if (len <= strlen(subcmd->subcmd)) {
      size_t c;
      for (c = 0; c < len; ++c) {
        if (argv[0][c] != subcmd->subcmd[c]) {
          break;
        }
      }
      if (c == len) {
        if (match < 0) {
          match = i;
        } else {
          printf("error: invalid command: matches more than one command%s\n", argv[0]);
          return 1;
        };
      }
    }
  }
  if (match < 0) {
    printf("error: invalid command: %s\n", argv[0]);
    return 1;
  }
  return subcmds[match].command(argc, argv);
}

static int pm_subcmd_version(int argc, char *argv[]) {
  (void) argc;
  (void) argv;
  pm_data_api_version data;
  int r = rtems_pm_api_version(&data);
  if (r < 0) {
    printf("error: smc: version: %s\n", strerror(errno));
    return 1;
  }
  printf("PM API Version: %d.%d\n", data.major, data.minor);
  return 0;
}


static int pm_subcmd_chipid(int argc, char *argv[]) {
  (void) argc;
  (void) argv;
  pm_data_chipid data;
  int r = rtems_pm_chipid(&data);
  if (r < 0) {
    printf("error: smc: chipid: %s\n", strerror(errno));
    return 1;
  }
  printf("Chip ID: 0x%" PRIx32 "\nVersion: 0x%" PRIx32 "\n", data.idcode, data.version);
  return 0;
}

static int pm_subcmd_features(int argc, char *argv[]) {
  (void) argc;
  (void) argv;
  uint32_t i;
  size_t max = 0;
  for (i = 0; i < NUMOF(api_id_labels); ++i) {
    size_t len = strlen(api_id_labels[i]);
    if (len > max) {
      max = len;
    }
  }
  for (i = 0; i < NUMOF(api_id_labels); ++i) {
    rtems_pm_feature_check(i);
    printf(
      "%-*s (0x%04" PRIx32 ") : %s\n", (int) max,
      api_id_labels[i], rtems_pm_get_api_id(i), strerror(errno));
  }
  return 0;
}

static int pm_subcmd_fpga_load(int argc, char *argv[]) {
  uint32_t flags = PM_FPGA_FULL;
  uint32_t status;
  image_loader image;
  int r;
  --argc;
  ++argv;
  if (argc == 0) {
    printf("error: FPGA bitfile file missing\n");
    return 1;
  }
  if (argc > 1) {
    while (argc > 0) {
      size_t len = strnlen(argv[0], 128);
      if (argv[0][0] != '-') {
        break;
      }
      if (len < 2) {
        printf("error: invalid option: %s\n", argv[0]);
        return 1;
      }
      if (len == 2) {
        if (argv[0][1] == 'p') {
          flags |= PM_FPGA_PARTIAL;
        } else {
          printf("error: invalid option: %s\n", argv[0]);
          return 1;
        }
      } else if (argv[0][1] == '-') {
        if (strcmp(argv[0], "--auth-ddr") == 0) {
          if ((flags & PM_FPGA_AUTHENTICATION_OCM) != 0) {
            printf("error: OCM authentication already set");
            return 1;
          }
          flags |= PM_FPGA_AUTHENTICATION_DDR;
        } else if (strcmp(argv[0], "--auth-ocm") == 0) {
          if ((flags & PM_FPGA_AUTHENTICATION_DDR) != 0) {
            printf("error: DDR authentication already set");
            return 1;
          }
          flags |= PM_FPGA_AUTHENTICATION_OCM;
        } else if (strcmp(argv[0], "--userkey") == 0) {
          if ((flags & PM_FPGA_ENCRYPTION_DEVKEY) != 0) {
            printf("error: dev key already set");
            return 1;
          }
          flags |= PM_FPGA_ENCRYPTION_USERKEY;
        } else if (strcmp(argv[0], "--devkey") == 0) {
          if ((flags & PM_FPGA_ENCRYPTION_USERKEY) != 0) {
            printf("error: user key already set");
            return 1;
          }
          flags |= PM_FPGA_ENCRYPTION_DEVKEY;
        } else {
          printf("error: invalid option: %s\n", argv[0]);
          return 1;
        }
      } else {
        printf("error: invalid option: %s\n", argv[0]);
        return 1;
      }
      --argc;
      ++argv;
    }
    if (argc != 1) {
      printf("error: FPGA bitfile file mising or not last\n");
      return 1;
    }
  }
  r = pm_image_load(argv[0], &image);
  if (r != 0) {
    return 1;
  }
  r = rtems_pm_fpga_load(image.image, image.size, flags, &status);
  pm_image_free(&image);
  if (r < 0) {
    printf("error: fpga: load: %s\n", strerror(errno));
    return 1;
  }
  printf("FPGA status: %" PRIu32 "\n", status);
  return 0;
}

static int pm_subcmd_fpga_status(int argc, char *argv[]) {
  (void) argc;
  (void) argv;
  uint32_t status;
  int r = rtems_pm_fpga_get_status(&status);
  if (r < 0) {
    printf("error: fpga: status: %s\n", strerror(errno));
    return 1;
  }
  printf("FPGA status: 0x%" PRIx32 "\n", status);
  printf(" Write idle: %s\n",
      (status & PM_GET_FPGA_STATUS_RD_IDLE) ? "idle" : "writing to FPGA");
  printf("  Read idle: %s\n",
      (status & PM_GET_FPGA_STATUS_RD_IDLE) ? "idle" : "reading from FPGA");
  printf("       Init: %s\n",
      (status & PM_GET_FPGA_STATUS_INIT) ? "initialised" : "uninitialised");
  printf("       Done: %s\n",
      (status & PM_GET_FPGA_STATUS_DONE) ? "done" : "not done");
  printf("        EOS: %s\n",
      (status & PM_GET_FPGA_STATUS_EOS) ? "end of startup" : "startup");
  printf("      Reset: %s\n",
      (status & PM_GET_FPGA_STATUS_RESET) ? "reset held" : "clear");
  return 0;
}

static pm_shell_subcmd fpga_subcmds[] = {
  { "load", "Load the PFGA bitfile", pm_subcmd_fpga_load, NULL },
  { "status", "Print FPGA status", pm_subcmd_fpga_status, NULL },
};

static int pm_subcmd_fpga(int argc, char *argv[]) {
  return pm_shell_subcommand(
    argv[0], fpga_subcmds, NUMOF(fpga_subcmds), argc - 1, argv + 1);
}

static int pm_subcmd_acap_load(int argc, char *argv[]) {
  image_loader image;
  uint32_t status;
  int r;
  --argc;
  ++argv;
  if (argc == 0) {
    printf("error: ACAP file missing\n");
    return 1;
  }
  if (argc > 1) {
    while (argc > 0) {
      size_t len = strnlen(argv[0], 128);
      if (argv[0][0] != '-') {
        break;
      }
      if (len < 2) {
        printf("error: invalid option: %s\n", argv[0]);
        return 1;
      }
      if (len == 2) {
        if (argv[0][1] == 'i') {
        } else {
          printf("error: invalid option: %s\n", argv[0]);
          return 1;
        }
      } else if (argv[0][1] == '-') {
        if (strcmp(argv[0], "--info") == 0) {
        } else {
          printf("error: invalid option: %s\n", argv[0]);
          return 1;
        }
      } else {
        printf("error: invalid option: %s\n", argv[0]);
        return 1;
      }
      --argc;
      ++argv;
    }
    if (argc != 1) {
      printf("error: ACAP file mising or not last\n");
      return 1;
    }
  }
  r = pm_image_load(argv[0], &image);
  if (r != 0) {
    return 1;
  }
  r = rtems_pm_acap_load(image.image, image.size, &status);
  pm_image_free(&image);
  if (r < 0) {
    printf("error: acapi: load: %s\n", strerror(errno));
    return 1;
  }
  printf("ACAP status: %" PRIu32 "\n", status);
  return 0;
}

static int pm_subcmd_acap_info(int argc, char *argv[]) {
  image_loader image;
  int r;
  --argc;
  ++argv;
  if (argc == 0) {
    printf("error: ACAP file missing\n");
    return 1;
  }
  if (argc != 1) {
    printf("error: ACAP file: invalid command line\n");
    return 1;
  }
  r = pm_image_load(argv[0], &image);
  if (r != 0) {
    return 1;
  }
  rtems_pm_acap_print(image.image, image.size);
  pm_image_free(&image);
  return 0;
}

static pm_shell_subcmd acap_subcmds[] = {
  { "load", "Load the ACAP (PDI) image", pm_subcmd_acap_load, NULL },
  { "info", "Print ACAP (PDI) information", pm_subcmd_acap_info, NULL },
};

static int pm_subcmd_acap(int argc, char *argv[]) {
  return pm_shell_subcommand(
    argv[0], acap_subcmds, NUMOF(acap_subcmds), argc - 1, argv + 1);
}

/*
 * Top level.
 */
static pm_shell_subcmd top_subcmds[] = {
  { "version", "Print PMUFW API version", pm_subcmd_version, NULL },
  { "chipid", "Print chip Id and version", pm_subcmd_chipid, NULL },
  { "features", "List the API fewtures available", pm_subcmd_features, NULL },
  { "fpga", "FPGA commands", pm_subcmd_fpga, NULL },
  { "acap", "ACAP commands", pm_subcmd_acap, NULL },
};

static int pm_shell_command (int argc, char* argv[]) {
  return pm_shell_subcommand(
    NULL, top_subcmds, NUMOF(top_subcmds), argc - 1, argv + 1);
}

struct rtems_shell_cmd_tt bsp_pm_load_shell_command = {
  .name     = "pm",
  .usage    = "Xilinx platform management commands",
  .topic    = "xilinx",
  .command  = pm_shell_command,
  .alias    = NULL,
  .next     = NULL
};

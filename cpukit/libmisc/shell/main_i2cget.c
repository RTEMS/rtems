/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2020 embedded brains GmbH.
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

/*
 * The command implemented here has a similar interface like the one from Linux
 * i2c tools. Think of it as a heavily simplified version of them. Instead of
 * the bus number they expect a bus path.
 *
 * Additionally the i2cget has a continuous read mode that isn't available on
 * Linux but does something similar to i2cdump.
 */

#include <dev/i2c/i2c.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <rtems/shell.h>

static const char rtems_i2cget_shell_usage [] =
  "i2cget <I2C_BUS> <CHIP-ADDRESS> <DATA-ADDRESS> [<NR-BYTES>]\n"
  "\tGet one or more bytes from an EEPROM like i2c device.\n"
  "\tNote that multiple bytes will be read in continuous mode.\n";

static int read_bytes(
  int fd,
  uint16_t i2c_address,
  uint8_t data_address,
  uint16_t nr_bytes
)
{
  int rv;
  uint8_t value[nr_bytes];
  i2c_msg msgs[] = {{
    .addr = i2c_address,
    .flags = 0,
    .buf = &data_address,
    .len = 1,
  }, {
    .addr = i2c_address,
    .flags = I2C_M_RD,
    .buf = value,
    .len = nr_bytes,
  }};
  struct i2c_rdwr_ioctl_data payload = {
    .msgs = msgs,
    .nmsgs = sizeof(msgs)/sizeof(msgs[0]),
  };
  uint16_t i;

  rv = ioctl(fd, I2C_RDWR, &payload);
  if (rv < 0) {
    perror("ioctl failed");
  } else {
    for (i = 0; i < nr_bytes; ++i) {
      printf("0x%02x ", value[i]);
    }
    printf("\n");
  }

  return rv;
}

static int rtems_i2cget_shell_main(int argc, char *argv[])
{
  int fd;
  int rv;
  const char *bus;
  uint16_t chip_address;
  uint8_t data_address;
  uint16_t nr_bytes;

  if (argc < 4 || argc > 5) {
    printf(rtems_i2cget_shell_usage);
    return 1;
  }

  errno = 0;
  chip_address = (uint16_t) strtoul(argv[2], NULL, 0);
  if (errno != 0) {
    perror("Couldn't read chip address");
    return 1;
  }

  errno = 0;
  data_address = (uint8_t) strtoul(argv[3], NULL, 0);
  if (errno != 0) {
    perror("Couldn't read data address");
    return 1;
  }

  nr_bytes = 1;
  if (argc == 5) {
    errno = 0;
    nr_bytes = (uint16_t) strtoul(argv[4], NULL, 0);
    if (errno != 0) {
      perror("Couldn't read number of bytes");
      return 1;
    }
  }

  bus = argv[1];
  fd = open(bus, O_RDWR);
  if (fd < 0) {
    perror("Couldn't open bus");
    return 1;
  }

  rv = read_bytes(fd, chip_address, data_address, nr_bytes);

  close(fd);

  return rv;
}

rtems_shell_cmd_t rtems_shell_I2CGET_Command = {
  .name = "i2cget",
  .usage = rtems_i2cget_shell_usage,
  .topic = "misc",
  .command = rtems_i2cget_shell_main,
};

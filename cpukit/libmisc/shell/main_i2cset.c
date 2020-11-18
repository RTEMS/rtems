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
 * Additionally it is possible to write multiple values as a continuous write.
 */

#include <dev/i2c/i2c.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <rtems/shell.h>

static const char rtems_i2cset_shell_usage [] =
  "i2cset <I2C_BUS> <CHIP-ADDRESS> <DATA-ADDRESS> <VALUE> [<VALUE> [...]]\n"
  "\tset one byte of an EEPROM like i2c device\n";

static int
rtems_i2cset_shell_main(int argc, char *argv[])
{
  int fd;
  int rv;
  const char *bus;
  uint16_t chip_address;
  /* Necessary: data-address and values. This will be a bit more. */
  uint8_t writebuff[argc];
  size_t len;
  size_t i;
  i2c_msg msgs[] = {{
    .flags = 0,
    .buf = writebuff,
    .len = 0,
  }};
  struct i2c_rdwr_ioctl_data payload = {
    .msgs = msgs,
    .nmsgs = sizeof(msgs)/sizeof(msgs[0]),
  };

  if (argc < 5) {
    printf(rtems_i2cset_shell_usage);
    return 1;
  }

  errno = 0;
  chip_address = (uint16_t) strtoul(argv[2], NULL, 0);
  if (errno != 0) {
    perror("Couldn't read CHIP_ADDRESS");
    return 1;
  }
  msgs[0].addr = chip_address;

  errno = 0;
  writebuff[0] = (uint8_t) strtoul(argv[3], NULL, 0);
  if (errno != 0) {
    perror("Couldn't read DATA_ADDRESS");
    return 1;
  }

  /* Read values starting from the fifth argument (index 4) */
  i = 4;
  len = 0;
  while (i < argc) {
    errno = 0;
    writebuff[len + 1] = (uint8_t) strtoul(argv[i], NULL, 0);
    if (errno != 0) {
      perror("Couldn't read VALUE");
      return 1;
    }
    ++i;
    ++len;
  }
  msgs[0].len = len + 1; /* Don't forget address */

  bus = argv[1];
  fd = open(bus, O_RDWR);
  if (fd < 0) {
    perror("Couldn't open bus");
    return 1;
  }

  rv = ioctl(fd, I2C_RDWR, &payload);
  if (rv < 0) {
    perror("ioctl failed");
  }
  close(fd);

  return rv;
}

rtems_shell_cmd_t rtems_shell_I2CSET_Command = {
  .name = "i2cset",
  .usage = rtems_i2cset_shell_usage,
  .topic = "misc",
  .command = rtems_i2cset_shell_main,
};

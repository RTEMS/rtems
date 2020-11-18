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

#include <dev/spi/spi.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <rtems/shell.h>

static const char rtems_spi_shell_usage [] =
  "simple SPI read / write\n"
  "\n"
  "spi [-loh] [-c <cs>] [-s <speed>] [-m <mode>] <SPI_BUS> xx [xx [..]]\n"
  "    <SPI_BUS>  Bus device to use\n"
  "    xx         Hex value of a byte to send\n"
  "    -c <cs>    Use chip select <cs> (default: None)\n"
  "    -m <mode>  Use SPI mode <mode> (default: 0)\n"
  "    -l         Send LSB first\n"
  "    -o         Use loopback mode\n"
  "    -s <speed> Bus speed in hz\n"
  "    -h         Print this help\n";

static int rtems_spi_shell_main(int argc, char *argv[])
{
  uint8_t buffer[argc - 1];
  size_t len = 0;
  int i;
  size_t j;
  int rv;
  int fd;
  char *bus = NULL;
  unsigned long mode;
  spi_ioc_transfer msg = {
    .len = 0,
    .rx_buf = buffer,
    .tx_buf = buffer,
    .speed_hz = 100000,
    .bits_per_word = 8,
    .cs_change = true,
    .mode = SPI_MODE_0 | SPI_NO_CS,
  };

  for (i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
      case ('c'):
        errno = 0;
        msg.mode &= ~SPI_NO_CS;
        msg.cs = (uint8_t) strtoul(argv[i+1], NULL, 0);
        ++i;
        if (errno != 0) {
          printf("Couldn't process chip select\n");
          return 1;
        }
        break;
      case ('m'):
        errno = 0;
        mode = strtoul(argv[i+1], NULL, 0);
        ++i;
        if (errno != 0 || mode > 3) {
          printf("Couldn't process mode\n");
          return 1;
        }
        msg.mode &= ~(SPI_CPOL | SPI_CPHA);
        msg.mode |= mode;
        break;
      case ('s'):
        errno = 0;
        msg.speed_hz = (uint32_t) strtoul(argv[i+1], NULL, 0);
        ++i;
        if (errno != 0) {
          printf("Couldn't process speed\n");
          return 1;
        }
        break;
      case ('l'):
        msg.mode |= SPI_LSB_FIRST;
        break;
      case ('o'):
        msg.mode |= SPI_LOOP;
        break;
      case ('h'):
        /* fallthrough */
      default:
        printf(rtems_spi_shell_usage);
        return 1;
      }
    } else if (bus == NULL) {
      bus = argv[i];
    } else {
      errno = 0;
      buffer[len] = (uint8_t) strtol(argv[i], NULL, 16);
      if (errno != 0) {
        printf("Couldn't process '%s'\n", argv[i]);
        return 1;
      }
      ++len;
    }
  }

  if (len == 0) {
    printf("Nothing to do\n");
    return 0;
  }

  fd = open(bus, O_RDWR);
  if (fd < 0) {
    perror("Couldn't open bus");
    return 1;
  }
  msg.len = len;
  rv = ioctl(fd, SPI_IOC_MESSAGE(1), &msg);
  if (rv == -1) {
    perror("Couldn't send the message");
  } else {
    printf("received:");
    for (j = 0; j < len; ++j) {
      printf(" %02x", buffer[j]);
    }
    printf("\n");
  }
  close(fd);

  return 0;
}

rtems_shell_cmd_t rtems_shell_SPI_Command = {
  .name = "spi",
  .usage = rtems_spi_shell_usage,
  .topic = "misc",
  .command = rtems_spi_shell_main,
};

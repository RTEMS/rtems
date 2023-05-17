/*
 * Copyright (C) 2023 Aaron Nyholm
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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <rtems/shell.h>

#include <dev/flash/flashdev.h>

static int flashdev_shell_read(char *dev_path, int argc, char *argv[]);
static int flashdev_shell_write(char *dev_path, int argc, char *argv[]);
static int flashdev_shell_erase(char *dev_path, int argc, char *argv[]);
static int flashdev_shell_type(char *dev_path);
static int flashdev_shell_jedecid(char *dev_path);
static int flashdev_shell_page_off(char *dev_path, int argc, char *argv[]);
static int flashdev_shell_page_idx(char *dev_path, int argc, char *argv[]);
static int flashdev_shell_pg_count(char *dev_path);
static int flashdev_shell_wb_size(char *dev_path);

static int flashdev_shell_ioctl_value(
  char *dev_path,
  int ioctl_call,
  void *ret
);

static int flashdev_shell_page(
  char *dev_path,
  int argc,
  char *argv[],
  int ioctl_call
);

static const char rtems_flashdev_shell_usage [] =
  "simple flash read / write / erase\n"
  "\n"
  "flashdev <FLASH_DEV_PATH> [OPTION]\n"
  "   -r <address> <bytes>  Read at address for bytes\n"
  "   -w <address> <file>   Write file to address\n"
  "   -e <address> <bytes>  Erase at address for bytes\n"
  "   -t                    Print the flash type\n"
  "   -d                    Print the JEDEC ID of flash device\n"
  "   -o <address>          Print the page information of page at address\n"
  "   -i <index>            Print the page information of page at index\n"
  "   -p                    Print the number of pages\n"
  "   -b                    Print the write block size\n"
  "   -h                    Print this help\n";


static int rtems_flashdev_shell_main( int argc, char *argv[] ) {

  char *dev_path = NULL;
  int i;

  for (i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      /*
       * Check that a path to flashdev has been provided before running
       * command.
       */
      if (dev_path == NULL) {
        printf("Please input FLASH_DEV_PATH before instruction\n");
        return 1;
      }
      /* Run command */
      switch (argv[i][1]) {
      case ('r'):
        /* Read */
        return flashdev_shell_read(dev_path, argc, &argv[i]);
      case ('w'):
        /* Write */
        return flashdev_shell_write(dev_path, argc, &argv[i]);
      case ('e'):
        /* Erase */
        return flashdev_shell_erase(dev_path, argc, &argv[i]);
      case ('t'):
        /* Flash Type */
        return flashdev_shell_type(dev_path);
      case ('d'):
        /* JEDEC Id */
        return flashdev_shell_jedecid(dev_path);
      case ('o'):
        /* Page info by offset */
        return flashdev_shell_page_off(dev_path, argc, &argv[i]);
      case ('i'):
        /* Page info by index */
        return flashdev_shell_page_idx(dev_path, argc, &argv[i]);
      case ('p'):
        /* Page count */
        return flashdev_shell_pg_count(dev_path);
      case ('b'):
        /* Write block size */
        return flashdev_shell_wb_size(dev_path);
      case ('h'):
      default:
        /* Help */
        printf(rtems_flashdev_shell_usage);
        break;
      }
    } else if (dev_path == NULL) {
      dev_path = argv[i];
    } else {
      printf("Invalid argument: %s\n", argv[i]);
      return 1;
    }
  }

  if (argc == 1) {
    printf(rtems_flashdev_shell_usage);
  }

  return 0;
}

int flashdev_shell_read(
  char *dev_path,
  int argc,
  char *argv[]
)
{
  uint32_t address;
  uint32_t bytes;
  int fd;
  int status;
  void *buffer;

  /* Check arguments */
  if (argc < 5) {
    printf("Missing argument\n");
    return -1;
  }

  /* Get arguments */
  errno = 0;
  address = (uint32_t) strtoul(argv[1], NULL, 0);
  if (errno != 0) {
    printf("Could not read address\n");
  }
  errno = 0;
  bytes = (uint32_t) strtoul(argv[2], NULL, 0);
  if (errno != 0) {
    printf("Could not read address\n");
  }

  /* Open flash device */
  fd = open(dev_path, O_RDONLY);
  if (fd == -1) {
    printf("Couldn't open %s\n", dev_path);
    return -1;
  }

  /* Move to address */
  status = lseek(fd, address, SEEK_SET);
  if (status == -1) {
    printf("Reading failed\n");
    close(fd);
    return -1;
  }

  /* Create a buffer to read into */
  buffer = calloc((bytes + bytes%4), 1);
  if (buffer == NULL) {
    printf("Failed to allocate read buffer\n");
    close(fd);
    return -1;
  }

  /* Read into buffer */
  status = read(fd, buffer, bytes);
  if (status == -1) {
    printf("Reading failed\n");
    free(buffer);
    close(fd);
    return -1;
  }

  /* Print buffer out in 32bit blocks */
  printf("Reading %s at 0x%08x for %d bytes\n", dev_path, address, bytes);
  for (int i = 0; i < (bytes/4); i++) {
    printf("%08x ", ((uint32_t*)buffer)[i]);
    if ((i+1)%4 == 0) {
      printf("\n");
    }
  }
  printf("\n");

  /* Clean up */
  free(buffer);
  close(fd);
  return 0;
}

int flashdev_shell_write(
  char *dev_path,
  int argc,
  char *argv[]
)
{
  uint32_t address;
  int flash;
  int file;
  int status;
  int read_len;
  off_t length;
  void *buffer;
  uint32_t offset;
  char *file_path;

  /* Check arguments */
  if (argc < 5) {
    printf("Missing argument\n");
    return -1;
  }

  /* Get arguments */
  errno = 0;
  address = (uint32_t) strtoul(argv[1], NULL, 0);
  if (errno != 0) {
    printf("Could not read address\n");
  }
  errno = 0;
  file_path = argv[2];

  /* Open flash device and move to write offset */
  flash = open(dev_path, O_WRONLY);
  if (flash == -1) {
    printf("Couldn't open %s\n", dev_path);
    return -1;
  }
  status = lseek(flash, address, SEEK_SET);
  if (status == -1) {
    printf("Reading failed\n");
    close(flash);
    return -1;
  }

  /* Open file and get file length */
  file = open(file_path, O_RDONLY);
  if (file == -1) {
    printf("Couldn't open %s\n", file_path);
    close(flash);
    return -1;
  }

  length = lseek(file, 0, SEEK_END);
  if (length == -1) {
    close(flash);
    close(file);
    printf("Couldn't find length of file\n");
    return -1;
  }

  if (lseek(file, 0, SEEK_SET) == -1) {
    close(flash);
    close(file);
    printf("Couldn't find length of file\n");
    return -1;
  }

  /* Create buffer */
  buffer = calloc(1, 0x1000);

  /* Write file to flash device in 0x1000 byte chunks */
  offset = 0;
  while (offset != length) {

    read_len = length - offset;
    if (read_len > 0x1000) {
      read_len = 0x1000;
    }

    status = read(file, buffer, read_len);
    if (status == -1) {
      free(buffer);
      close(flash);
      close(file);
      printf("Can't read %s\n", file_path);
      return -1;
    }

    status = write(flash, buffer, read_len);
    if (status == -1) {
      free(buffer);
      close(flash);
      close(file);
      printf("Can't write %s\n", dev_path);
      return -1;
    }

    offset = offset + read_len;
  }

  /* Clean up */
  close(flash);
  close(file);
  free(buffer);
  return 0;
}

int flashdev_shell_erase(
  char *dev_path,
  int argc,
  char *argv[]
)
{
  uint32_t address;
  uint32_t bytes;
  int fd;
  int status;
  rtems_flashdev_region args;

  /* Check arguments */
  if (argc < 5) {
    printf("Missing argument\n");
    return -1;
  }

  /* Get arguments */
  errno = 0;
  address = (uint32_t) strtoul(argv[1], NULL, 0);
  if (errno != 0) {
    printf("Could not read address\n");
  }
  errno = 0;
  bytes = (uint32_t) strtoul(argv[2], NULL, 0);
  if (errno != 0) {
    printf("Could not read address\n");
  }

  /* Open flash device */
  fd = open(dev_path, O_RDWR);
  if (fd == -1) {
    printf("Couldn't open %s\n", dev_path);
    return -1;
  }

  printf("Erasing at %08x for %x bytes\n", address, bytes);

  /* Erase flash */
  args.offset = address;
  args.size = bytes;

  status = ioctl(fd, RTEMS_FLASHDEV_IOCTL_ERASE, &args);
  if (status == -1) {
    printf("Erase failed\n");
    close(fd);
    return -1;
  }

  /* Clean up */
  close(fd);

  return 0;
}

int flashdev_shell_type( char *dev_path )
{
  int type;
  int status;

  /* Get type */
  status = flashdev_shell_ioctl_value(
    dev_path,
    RTEMS_FLASHDEV_IOCTL_TYPE,
    &type
  );

  if (status) {
    printf("Failed to get flash type\n");
    return status;
  }

  /* Print type */
  switch(type) {
    case RTEMS_FLASHDEV_NOR:
      printf("NOR flash\n");
      break;
    case RTEMS_FLASHDEV_NAND:
      printf("NAND flash\n");
      break;
    default:
      printf("Unknown type\n");
  }

  return 0;
}

int flashdev_shell_jedecid( char *dev_path ) {
  uint32_t ret;
  int status;

  /* Get JEDEC Id */
  status = flashdev_shell_ioctl_value(
    dev_path,
    RTEMS_FLASHDEV_IOCTL_JEDEC_ID,
    &ret
  );

  /* Print JEDEC Id */
  if (status) {
    printf("Failed to get JEDEC Id\n");
    return status;
  } else {
    printf("JEDEC Id: 0x%x\n", ret);
  }
  return 0;
}

static int flashdev_shell_page_off(
  char *dev_path,
  int argc,
  char *argv[]
)
{
  return flashdev_shell_page(
    dev_path,
    argc,
    argv,
    RTEMS_FLASHDEV_IOCTL_PAGEINFO_BY_OFFSET
  );
}

static int flashdev_shell_page_idx(
  char *dev_path,
  int argc,
  char *argv[]
)
{
  return flashdev_shell_page(
    dev_path,
    argc,
    argv,
    RTEMS_FLASHDEV_IOCTL_PAGEINFO_BY_INDEX
  );
}

static int flashdev_shell_pg_count( char *dev_path )
{
  uint32_t ret;
  int status;

  /* Get Page Count */
  status = flashdev_shell_ioctl_value(
    dev_path,
    RTEMS_FLASHDEV_IOCTL_PAGE_COUNT,
    &ret
  );

  /* Print Page Count */
  if (status) {
    printf("Failed to get page count\n");
    return status;
  } else {
    printf("Page count: 0x%x\n", ret);
  }
  return 0;
}

static int flashdev_shell_wb_size( char *dev_path )
{
  size_t ret;
  int status;

  /* Get Write Block Size */
  status = flashdev_shell_ioctl_value(
    dev_path,
    RTEMS_FLASHDEV_IOCTL_WRITE_BLOCK_SIZE,
    &ret
  );

  /* Print Write Block Size */
  if (status) {
    printf("Failed to get write block size\n");
    return status;
  } else {
    printf("Write block size: 0x%zx\n", ret);
  }
  return 0;
}

static int flashdev_shell_ioctl_value(
  char *dev_path,
  int ioctl_call,
  void *ret
)
{
  int fd;
  int status;

  fd = open(dev_path, O_RDONLY);
  if (fd == -1) {
    printf("Couldn't open %s\n", dev_path);
    return -1;
  }

  status = ioctl(fd, ioctl_call, ret);
  if (status == -1) {
    close(fd);
    return -1;
  }

  close(fd);
  return 0;
}

static int flashdev_shell_page(
  char *dev_path,
  int argc,
  char *argv[],
  int ioctl_call
)
{
  rtems_flashdev_ioctl_page_info pg_info;
  int fd;
  int status;

  /* Check arguments */
  if (argc < 4) {
    printf("Missing argument\n");
    return -1;
  }

  /* Get arguments */
  errno = 0;
  pg_info.location = (off_t) strtoul(argv[1], NULL, 0);
  if (errno != 0) {
    printf("Could not read address\n");
  }

  /* Open flash device */
  fd = open(dev_path, O_RDWR);
  if (fd == -1) {
    printf("Couldn't open %s\n", dev_path);
    return -1;
  }

  status = ioctl(fd, ioctl_call, &pg_info);
  if (status == -1) {
    printf("Failed to get page info\n");
    close(fd);
    return -1;
  }

  printf(
    "Page offset: 0x%jx\nPage length: 0x%zx\n",
    pg_info.page_info.offset,
    pg_info.page_info.size
  );

  /* Clean up */
  close(fd);
  return 0;
}

rtems_shell_cmd_t rtems_shell_FLASHDEV_Command = {
  .name = "flashdev",
  .usage = rtems_flashdev_shell_usage,
  .topic = "misc",
  .command = rtems_flashdev_shell_main,
};

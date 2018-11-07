/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT
#include "system.h"
#include <sys/param.h>
#include <crypt.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <rtems.h>
#include <fcntl.h>
#include <inttypes.h>
#include <rtems/error.h>
#include <rtems/dosfs.h>
#include <ctype.h>
#include <rtems/bdpart.h>
#include <rtems/libcsupport.h>
#include <rtems/fsmount.h>
#include <rtems/ramdisk.h>
#include <rtems/nvdisk.h>
#include <rtems/nvdisk-sram.h>
#include <rtems/shell.h>

const char rtems_test_name[] = "FILE I/O";

#if FILEIO_BUILD

/**
 * Let the IO system allocation the next available major number.
 */
#define RTEMS_DRIVER_AUTO_MAJOR (0)

/**
 * The NV Device descriptor. For this test it is just DRAM.
 */
rtems_nvdisk_device_desc rtems_nv_heap_device_descriptor[] =
{
  {
    flags:  0,
    base:   0,
    size:   (size_t) 1024 * 1024,
    nv_ops: &rtems_nvdisk_sram_handlers
  }
};

/**
 * The NV Disk configuration.
 */
const rtems_nvdisk_config rtems_nvdisk_configuration[] =
{
  {
    block_size:         512,
    device_count:       1,
    devices:            &rtems_nv_heap_device_descriptor[0],
    flags:              0,
    info_level:         0
  }
};

/**
 * The number of NV Disk configurations.
 */
uint32_t rtems_nvdisk_configuration_size = 1;

/*
 * Table of FAT file systems that will be mounted
 * with the "fsmount" function.
 * See cpukit/libmisc/fsmount for definition of fields
 */
fstab_t fs_table[] = {
  {
    "/dev/hda1","/mnt/hda1", "dosfs",
    RTEMS_FILESYSTEM_READ_WRITE,
    FSMOUNT_MNT_OK | FSMOUNT_MNTPNT_CRTERR | FSMOUNT_MNT_FAILED,
    0
  },
  {
    "/dev/hda2","/mnt/hda2", "dosfs",
    RTEMS_FILESYSTEM_READ_WRITE,
    FSMOUNT_MNT_OK | FSMOUNT_MNTPNT_CRTERR | FSMOUNT_MNT_FAILED,
    0
  },
  {
    "/dev/hda3","/mnt/hda3", "dosfs",
    RTEMS_FILESYSTEM_READ_WRITE,
    FSMOUNT_MNT_OK | FSMOUNT_MNTPNT_CRTERR | FSMOUNT_MNT_FAILED,
    0
  },
  {
    "/dev/hda4","/mnt/hda4", "dosfs",
    RTEMS_FILESYSTEM_READ_WRITE,
    FSMOUNT_MNT_OK | FSMOUNT_MNTPNT_CRTERR | FSMOUNT_MNT_FAILED,
    0
  },
  {
    "/dev/hdc1","/mnt/hdc1", "dosfs",
    RTEMS_FILESYSTEM_READ_WRITE,
    FSMOUNT_MNT_OK | FSMOUNT_MNTPNT_CRTERR | FSMOUNT_MNT_FAILED,
    0
  },
  {
    "/dev/hdc2","/mnt/hdc2", "dosfs",
    RTEMS_FILESYSTEM_READ_WRITE,
    FSMOUNT_MNT_OK | FSMOUNT_MNTPNT_CRTERR | FSMOUNT_MNT_FAILED,
    0
  },
  {
    "/dev/hdc3","/mnt/hdc3", "dosfs",
    RTEMS_FILESYSTEM_READ_WRITE,
    FSMOUNT_MNT_OK | FSMOUNT_MNTPNT_CRTERR | FSMOUNT_MNT_FAILED,
    0
  },
  {
    "/dev/hdc4","/mnt/hdc4", "dosfs",
    RTEMS_FILESYSTEM_READ_WRITE,
    FSMOUNT_MNT_OK | FSMOUNT_MNTPNT_CRTERR | FSMOUNT_MNT_FAILED,
    0
  }
};

#define USE_SHELL

#ifdef USE_SHELL

static int
shell_nvdisk_trace (int argc, char* argv[])
{
  const char* driver;
  int         level;

  if (argc != 3)
  {
    printf ("error: invalid number of options\n");
    return 1;
  }

  driver = argv[1];
  level  = strtoul (argv[2], 0, 0);
  
  int fd = open (driver, O_WRONLY, 0);
  if (fd < 0)
  {
    printf ("error: driver open failed: %s\n", strerror (errno));
    return 1;
  }
  
  if (ioctl (fd, RTEMS_NVDISK_IOCTL_INFO_LEVEL, level) < 0)
  {
    printf ("error: driver set level failed: %s\n", strerror (errno));
    return 1;
  }
  
  close (fd);
  
  return 0;
}

static int
shell_nvdisk_erase (int argc, char* argv[])
{
  const char* driver = NULL;
  int         arg;
  int         fd;
  
  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] == '-')
    {
      printf ("error: invalid option: %s\n", argv[arg]);
      return 1;
    }
    else
    {
      if (!driver)
        driver = argv[arg];
      else
      {
        printf ("error: only one driver name allowed: %s\n", argv[arg]);
        return 1;
      }
    }
  }
  
  printf ("erase nv disk: %s\n", driver);
  
  fd = open (driver, O_WRONLY, 0);
  if (fd < 0)
  {
    printf ("error: nvdisk driver open failed: %s\n", strerror (errno));
    return 1;
  }
  
  if (ioctl (fd, RTEMS_NVDISK_IOCTL_ERASE_DISK) < 0)
  {
    printf ("error: nvdisk driver erase failed: %s\n", strerror (errno));
    return 1;
  }
  
  close (fd);
  
  printf ("nvdisk erased successful\n");

  return 0;
}

static int
shell_bdbuf_trace (int argc, char* argv[])
{
#if RTEMS_BDBUF_TRACE
  extern bool rtems_bdbuf_tracer;
  rtems_bdbuf_tracer = !rtems_bdbuf_tracer;
  printf ("bdbuf trace: %d\n", rtems_bdbuf_tracer);
#else
  printf ("bdbuf trace disabled. Rebuild with enabled.\n");
#endif
  return 0;
}

static int
disk_test_set_block_size (rtems_disk_device *dd, size_t size)
{
  return dd->ioctl (dd, RTEMS_BLKIO_SETBLKSIZE, &size);
}

static int
disk_test_write_blocks (const char *name, int start, int count, size_t size)
{
  int                 block;
  uint32_t*           ip;
  uint32_t            value = 0;
  int                 i;
  rtems_bdbuf_buffer* bd;
  rtems_status_code   sc;
  int                 rv = 0;
  rtems_disk_device*  dd;
  int                 fd;

  fd = open(name, O_RDWR);
  if (fd < 0) {
    printf ("error: cannot open disk\n");
    rv = 1;
  }

  if (rv == 0 && rtems_disk_fd_get_disk_device(fd, &dd) != 0)
  {
    printf ("error: cannot obtain disk\n");
    rv = 1;
  }

  if (fd >= 0 && close (fd) != 0)
  {
    printf ("error: close disk failed\n");
    rv = 1;
  }

  if (rv == 0 && disk_test_set_block_size (dd, size) < 0)
  {
    printf ("error: set block size failed: %s\n", strerror (errno));
    rv = 1;
  }

  for (block = start; rv == 0 && block < (start + count); block++)
  {
    sc = rtems_bdbuf_read (dd, block, &bd);
    if (sc == RTEMS_SUCCESSFUL)
    {
      ip = (uint32_t*) bd->buffer;
      for (i = 0; i < (size / sizeof (uint32_t)); i++, ip++, value++)
        *ip = (size << 16) | value;

      sc = rtems_bdbuf_release_modified (bd);
      if (sc != RTEMS_SUCCESSFUL)
      {
        printf ("error: release block %d bd failed: %s\n",
                block, rtems_status_text (sc));
        rv = 1;
      }
    }
    else
    {
      printf ("error: get block %d bd failed: %s\n",
              block, rtems_status_text (sc));
      rv = 1;
    }
  }

  return rv;
}

static int
disk_test_block_sizes (int argc, char *argv[])
{
  char* name;
  int   start;
  int   count;
  int   size;

  if (argc != (4 + 1))
  {
    printf ("error: need to supply a device path, start, block and size\n");
    return 1;
  }

  name = argv[1];

  start = strtoul (argv[2], 0, 0);
  count = strtoul (argv[3], 0, 0);
  size  = strtoul (argv[4], 0, 0);

  return disk_test_write_blocks (name, start, count, size);
}

static uint32_t
parse_size_arg (const char* arg)
{
  uint32_t size;
  uint32_t scalar = 1;

  size = strtoul (arg, 0, 0);
  switch (arg[strlen (arg) - 1])
  {
    case 'b':
      scalar = 1;
      break;
    case 'M':
      scalar = 1024000;
      break;
    case 'm':
      scalar = 1000000;
      break;
    case 'K':
      scalar = 1024;
      break;
    case 'k':
      scalar = 1000;
      break;
    default:
      printf ("error: invalid scalar (b,M/m/K/k): %c\n", arg[strlen (arg) - 1]);
      return 0;
  }
  return size * scalar;
}

static int
create_ramdisk (int argc, char *argv[])
{
  rtems_status_code         sc;
  int                       arg;
  uint32_t                  size = 524288;
  uint32_t                  block_size = 512;
  uint32_t                  block_count;

  for (arg = 0; arg < argc; ++arg)
  {
    if (argv[arg][0] == '-')
    {
      switch (argv[arg][1])
      {
        case 's':
          ++arg;
          if (arg == argc)
          {
            printf ("error: -s needs a size\n");
            return 1;
          }
          size = parse_size_arg (argv[arg]);
          if (size == 0)
            return 1;
          break;
        case 'b':
          ++arg;
          if (arg == argc)
          {
            printf ("error: -b needs a size\n");
            return 1;
          }
          block_size = parse_size_arg (argv[arg]);
          if (size == 0)
            return 1;
          break;
        default:
          printf ("error: invalid option: %s\n", argv[arg]);
          return 1;
      }
    }
  }

  block_count = size / block_size;

  /*
   * Register the RAM Disk driver.
   */
  printf ("Register RAM Disk Driver [blocks=%" PRIu32 \
          " block-size=%" PRIu32 "]:",
          block_count,
          block_size);

  sc = ramdisk_register(block_size, block_count, false, "/dev/rda");
  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: ramdisk driver not initialised: %s\n",
            rtems_status_text (sc));
    return 1;
  }

  printf ("successful\n");

  return 0;
}

static int
create_nvdisk (int argc, char *argv[])
{
  rtems_status_code         sc;
  int                       arg;
  uint32_t                  size = 0;
#if ADD_WHEN_NVDISK_HAS_CHANGED
  uint32_t                  block_size = 0;
#endif
  
  for (arg = 0; arg < argc; ++arg)
  {
    if (argv[arg][0] == '-')
    {
      switch (argv[arg][1])
      {
        case 's':
          ++arg;
          if (arg == argc)
          {
            printf ("error: -s needs a size\n");
            return 1;
          }
          size = parse_size_arg (argv[arg]);
          if (size == 0)
            return 1;
          break;
#if ADD_WHEN_NVDISK_HAS_CHANGED
        case 'b':
          ++arg;
          if (arg == argc)
          {
            printf ("error: -b needs a size\n");
            return 1;
          }
          block_size = parse_size_arg (argv[arg]);
          if (size == 0)
            return 1;
          break;
#endif
        default:
          printf ("error: invalid option: %s\n", argv[arg]);
          return 1;
      }
    }
  }

#if ADD_WHEN_NVDISK_HAS_CHANGED
  if (block_size)
    rtems_nvdisk_configuration[0].block_size = block_size;
#endif
  if (size)
    rtems_nv_heap_device_descriptor[0].size = size;
    
  /*
   * For our test we do not have any static RAM or EEPROM devices so
   * we allocate the memory from the heap.
   */
  rtems_nv_heap_device_descriptor[0].base =
    malloc (rtems_nv_heap_device_descriptor[0].size);

  if (!rtems_nv_heap_device_descriptor[0].base)
  {
    printf ("error: no memory for NV disk\n");
    return 1;
  }
  
  /*
   * Register the RAM Disk driver.
   */
  printf ("Register NV Disk Driver [size=%" PRIu32 \
          " block-size=%" PRIu32"]:",
          rtems_nv_heap_device_descriptor[0].size,
          rtems_nvdisk_configuration[0].block_size);

  sc = rtems_nvdisk_initialize (0, 0, NULL);
  if (sc != RTEMS_SUCCESSFUL)
  {
    printf ("error: nvdisk driver not initialised: %s\n",
            rtems_status_text (sc));
    return 1;
  }
  
  printf ("successful\n");

  return 0;
}

static void writeFile(
  const char *name,
  mode_t      mode,
  const char *contents
)
{
  int sc;
  sc = setuid(0);
  if ( sc ) {
    printf( "setuid failed: %s: %s\n", name, strerror(errno) );
  }

  rtems_shell_write_file( name, contents );

  sc = chmod ( name, mode );
  if ( sc ) {
    printf( "chmod %s: %s\n", name, strerror(errno) );
  }
}

#define writeScript( _name, _contents ) \
        writeFile( _name, 0777, _contents )

static void fileio_start_shell(void)
{
  int sc;

  sc = mkdir("/scripts", 0777);
  if ( sc ) {
    printf( "mkdir /scripts: %s:\n", strerror(errno) );
  }

  sc = mkdir("/etc", 0777);
  if ( sc ) {
    printf( "mkdir /etc: %s:\n", strerror(errno) );
  }

  sc = mkdir("/chroot", 0777);
  if ( sc ) {
    printf( "mkdir /chroot: %s:\n", strerror(errno) );
  }

  printf(
    "Creating /etc/passwd and group with four useable accounts:\n"
    "  root/pwd\n"
    "  test/pwd\n"
    "  rtems/NO PASSWORD\n"
    "  chroot/NO PASSWORD\n"
    "Only the root user has access to all available commands.\n"
  );

  writeFile(
    "/etc/passwd",
    0644,
    "root:$6$$FuPOhnllx6lhW2qqlnmWvZQLJ8Thr/09I7ESTdb9VbnTOn5.65"
      "/Vh2Mqa6FoKXwT0nHS/O7F0KfrDc6Svb/sH.:0:0:root::/:/bin/sh\n"
    "rtems::1:1:RTEMS Application::/:/bin/sh\n"
    "test:$1$$oPu1Xt2Pw0ngIc7LyDHqu1:2:2:test account::/:/bin/sh\n"
    "tty:*:3:3:tty owner::/:/bin/false\n"
    "chroot::4:2:chroot account::/chroot:/bin/sh\n"
  );
  writeFile(
    "/etc/group",
    0644,
    "root:x:0:root\n"
    "rtems:x:1:rtems\n"
    "test:x:2:test\n"
    "tty:x:3:tty\n"
  );

  writeScript(
    "/scripts/js",
    "#! joel\n"
    "\n"
    "date\n"
    "echo Script successfully ran\n"
    "date\n"
    "stackuse\n"
  );

  writeScript(
    "/scripts/j1",
    "#! joel -s 20480 -t JESS\n"
    "stackuse\n"
  );

  rtems_shell_write_file(
    "/scripts/j2",
    "echo j2 TEST FILE\n"
    "echo j2   SHOULD BE non-executable AND\n"
    "echo j2   DOES NOT have the magic first line\n"
  );

  rtems_shell_add_cmd ("mkrd", "files",
                       "Create a RAM disk driver", create_ramdisk);
  rtems_shell_add_cmd ("mknvd", "files",
                       "Create a NV disk driver", create_nvdisk);
  rtems_shell_add_cmd ("nverase", "misc",
                       "nverase driver", shell_nvdisk_erase);
  rtems_shell_add_cmd ("nvtrace", "misc",
                       "nvtrace driver level", shell_nvdisk_trace);
  rtems_shell_add_cmd ("bdbuftrace", "files",
                       "bdbuf trace toggle", shell_bdbuf_trace);
  rtems_shell_add_cmd ("td", "files",
                       "Test disk", disk_test_block_sizes);
#if RTEMS_RFS_TRACE
  rtems_shell_add_cmd ("rfs", "files",
                       "RFS trace",
                       rtems_rfs_trace_shell_command);
#endif
#if RTEMS_RFS_RTEMS_TRACE
  rtems_shell_add_cmd ("rrfs", "files",
                       "RTEMS RFS trace",
                       rtems_rfs_rtems_trace_shell_command);
#endif

  printf(" =========================\n");
  printf(" starting shell\n");
  printf(" =========================\n");
  rtems_shell_init(
    "SHLL",                          /* task_name */
    RTEMS_MINIMUM_STACK_SIZE * 4,    /* task_stacksize */
    100,                             /* task_priority */
    "/dev/foobar",                   /* devname */
    /* device is currently ignored by the shell if it is not a pty */
    false,                           /* forever */
    true,                            /* wait */
    rtems_shell_login_check          /* login */
  );
}
#endif /* USE_SHELL */

static void fileio_print_free_heap(void)
{
  printf("--- unused dynamic memory: %lu bytes ---\n",
	 (unsigned long) malloc_free_space());
}


static void fileio_part_table_initialize(void)
{
  char devname[64];
  rtems_status_code rc;

  printf(" =========================\n");
  printf(" Initialize partition table\n");
  printf(" =========================\n");
  fileio_print_free_heap();
  printf(" Enter device to initialize ==>");
  fflush(stdout);
  fgets(devname,sizeof(devname)-1,stdin);
  while (devname[strlen(devname)-1] == '\n') {
    devname[strlen(devname)-1] = '\0';
  }
  /*
   * call function
   */
  rc = rtems_bdpart_register_from_disk(devname);
  printf("result = %d\n",rc);
  fileio_print_free_heap();
}

static void fileio_fsmount(void)
{
  rtems_status_code rc;

  printf(" =========================\n");
  printf(" Process fsmount table\n");
  printf(" =========================\n");
  fileio_print_free_heap();
  /*
   * call function
   */
  rc = rtems_fsmount( fs_table,
		      sizeof(fs_table)/sizeof(fs_table[0]),
		      NULL);
  printf("result = %d\n",rc);
  fileio_print_free_heap();
}

static void fileio_list_file(void)
{
  char fname[1024];
  char *buf_ptr = NULL;
  ssize_t   flen = 0;
  int fd = -1;
  ssize_t n;
  size_t buf_size = 100;

  rtems_interval start_tick,curr_tick,ticks_per_sec;

  printf(" =========================\n");
  printf(" LIST FILE ...            \n");
  printf(" =========================\n");
  fileio_print_free_heap();
  printf(" Enter filename to list ==>");
  fflush(stdout);
  fgets(fname,sizeof(fname)-1,stdin);
  while (fname[strlen(fname)-1] == '\n') {
    fname[strlen(fname)-1] = '\0';
  }
  /*
   * allocate buffer of given size
   */
  if (buf_size > 0) {
    buf_ptr = malloc(buf_size);
  }

  if (buf_ptr != NULL) {
    printf("\n Trying to open file \"%s\" for read\n",fname);
    fd = open(fname,O_RDONLY);
    if (fd < 0) {
      printf("*** file failed to open, errno = %d(%s)\n",errno,strerror(errno));
    }
  }

  if (fd >= 0) {
    start_tick = rtems_clock_get_ticks_since_boot();
    do {
      n = read(fd,buf_ptr,buf_size);
      if (n > 0) {
	write(1,buf_ptr,(size_t) n);
	flen += n;
      }
    } while (n > 0);

    curr_tick = rtems_clock_get_ticks_since_boot();

    printf("\n ******** End of file reached, flen = %zd\n",flen);
    close(fd);

    ticks_per_sec = rtems_clock_get_ticks_per_second();
    printf("time elapsed for read:  %g seconds\n",
	   ((double)curr_tick-start_tick)/ticks_per_sec);
  }
  /*
   * free buffer
   */
  if (buf_ptr != NULL) {
    free(buf_ptr);
  }
  fileio_print_free_heap();
}

/*
 * convert a size string (like 34K or 12M) to actual byte count
 */
static bool fileio_str2size(const char *str,uint32_t   *res_ptr)
{
  bool failed = false;
  unsigned long size;
  unsigned char suffix = ' ';

  if (1 > sscanf(str,"%lu%c",&size,&suffix)) {
    failed = true;
  }
  else if (toupper((int)suffix) == 'K') {
    size *= 1024;
  }
  else if (toupper((int)suffix) == 'M') {
    size *= 1024UL*1024UL;
  }
  else if (isalpha((int)suffix)) {
    failed = true;
  }

  if (!failed) {
    *res_ptr = size;
  }
  return failed;
}

static void fileio_write_file(void)
{
  char fname[1024];
  char tmp_str[32];
  uint32_t   file_size = 0;
  uint32_t   buf_size  = 0;
  size_t curr_pos,bytes_to_copy;
  int fd = -1;
  ssize_t n;
  rtems_interval start_tick,curr_tick,ticks_per_sec;
  char *bufptr = NULL;
  bool failed = false;
  static const char write_test_string[] =
    "The quick brown fox jumps over the lazy dog\n";
  static const char write_block_string[] =
    "\n----- end of write buffer ------\n";

  printf(" =========================\n");
  printf(" WRITE FILE ...           \n");
  printf(" =========================\n");
  fileio_print_free_heap();
  /*
   * get number of ticks per second
   */
  ticks_per_sec = rtems_clock_get_ticks_per_second();

  /*
   * get path to file to write
   */
  if (!failed) {
    printf("Enter path/filename ==>");
    fflush(stdout);
    fgets(fname,sizeof(fname)-1,stdin);
    while (fname[strlen(fname)-1] == '\n') {
      fname[strlen(fname)-1] = '\0';
    }
    if (0 == strlen(fname)) {
      printf("*** no filename entered, aborted\n");
      failed = true;
    }
  }
  /*
   * get total file size to write
   */
  if (!failed) {
    printf("use suffix K for Kbytes, M for Mbytes or no suffix for bytes:\n"
	   "Enter filesize to write ==>");
    fflush(stdout);
    fgets(tmp_str,sizeof(tmp_str)-1,stdin);
    failed = fileio_str2size(tmp_str,&file_size);
    if (failed) {
      printf("*** illegal file size, aborted\n");
    }
  }
  /*
   * get block size to write
   */
  if (!failed) {
    printf("use suffix K for Kbytes, M for Mbytes or no suffix for bytes:\n"
	   "Enter block size to use for write calls ==>");
    fflush(stdout);
    fgets(tmp_str,sizeof(tmp_str)-1,stdin);
    failed = fileio_str2size(tmp_str,&buf_size);
    if (failed) {
      printf("*** illegal block size, aborted\n");
    }
  }

  /*
   * allocate buffer
   */
  if (!failed) {
    printf("... allocating %lu bytes of buffer for write data\n",
	   (unsigned long)buf_size);
    bufptr = malloc(buf_size+1); /* extra space for terminating NUL char */
    if (bufptr == NULL) {
      printf("*** malloc failed, aborted\n");
      failed = true;
    }
  }
  /*
   * fill buffer with test pattern
   */
  if (!failed) {
    printf("... filling buffer with write data\n");
    curr_pos = 0;
    /*
     * fill buffer with test string
     */
    while (curr_pos < buf_size) {
      bytes_to_copy = MIN(buf_size-curr_pos,
			  sizeof(write_test_string)-1);
      memcpy(bufptr+curr_pos,write_test_string,bytes_to_copy);
      curr_pos += bytes_to_copy;
    }
    /*
     * put "end" mark at end of buffer
     */
    bytes_to_copy = sizeof(write_block_string)-1;
    if (buf_size >= bytes_to_copy) {
      memcpy(bufptr+buf_size-bytes_to_copy,
	     write_block_string,
	     bytes_to_copy);
    }
  }
  /*
   * create file
   */
  if (!failed) {
    printf("... creating file \"%s\"\n",fname);
    fd = open(fname,O_WRONLY | O_CREAT | O_TRUNC,S_IREAD|S_IWRITE);
    if (fd < 0) {
      printf("*** file create failed, errno = %d(%s)\n",errno,strerror(errno));
      failed = true;
    }
  }
  /*
   * write file
   */
  if (!failed) {
    printf("... writing to file\n");
    start_tick = rtems_clock_get_ticks_since_boot();
    curr_pos = 0;
    do {
      bytes_to_copy = buf_size;
      do {
	n = write(fd,
	  bufptr + (buf_size-bytes_to_copy),
		  MIN(bytes_to_copy,file_size-curr_pos));
	if (n > 0) {
	  bytes_to_copy -= (size_t) n;
	  curr_pos      += (size_t) n;
	}
      } while ((bytes_to_copy > 0)  && (n > 0));
    } while ((file_size > curr_pos) && (n > 0));
    curr_tick = rtems_clock_get_ticks_since_boot();
    if (n < 0) {
      failed = true;
      printf("*** file write failed, "
	     "%lu bytes written, "
	     "errno = %d(%s)\n",
	     (unsigned long)curr_pos,errno,strerror(errno));
    }
    else {
      printf("time elapsed for write:  %g seconds\n",
	     ((double)curr_tick-start_tick)/ticks_per_sec);
      printf("write data rate: %g KBytes/second\n",
	     (((double)file_size) / 1024.0 /
	      (((double)curr_tick-start_tick)/ticks_per_sec)));
    }
  }
  if (fd >= 0) {
    printf("... closing file\n");
    close(fd);
  }
  if (bufptr != NULL) {
    printf("... deallocating buffer\n");
    free(bufptr);
    bufptr = NULL;
  }
  printf("\n ******** End of file write\n");
  fileio_print_free_heap();
}

static void fileio_read_file(void)
{
  char fname[1024];
  char tmp_str[32];
  uint32_t   buf_size  = 0;
  size_t curr_pos;
  int fd = -1;
  ssize_t n;
  rtems_interval start_tick,curr_tick,ticks_per_sec;
  char *bufptr = NULL;
  bool failed = false;

  printf(" =========================\n");
  printf(" READ FILE ...            \n");
  printf(" =========================\n");
  fileio_print_free_heap();
  /*
   * get number of ticks per second
   */
  ticks_per_sec = rtems_clock_get_ticks_per_second();

  /*
   * get path to file to read
   */
  if (!failed) {
    printf("Enter path/filename ==>");
    fflush(stdout);
    fgets(fname,sizeof(fname)-1,stdin);
    while (fname[strlen(fname)-1] == '\n') {
      fname[strlen(fname)-1] = '\0';
    }
    if (0 == strlen(fname)) {
      printf("*** no filename entered, aborted\n");
      failed = true;
    }
  }
  /*
   * get block size to read
   */
  if (!failed) {
    printf("use suffix K for Kbytes, M for Mbytes or no suffix for bytes:\n"
	   "Enter block size to use for read calls ==>");
    fflush(stdout);
    fgets(tmp_str,sizeof(tmp_str)-1,stdin);
    failed = fileio_str2size(tmp_str,&buf_size);
    if (failed) {
      printf("*** illegal block size, aborted\n");
    }
  }

  /*
   * allocate buffer
   */
  if (!failed) {
    printf("... allocating %lu bytes of buffer for write data\n",
	   (unsigned long)buf_size);
    bufptr = malloc(buf_size+1); /* extra space for terminating NUL char */
    if (bufptr == NULL) {
      printf("*** malloc failed, aborted\n");
      failed = true;
    }
  }
  /*
   * open file
   */
  if (!failed) {
    printf("... opening file \"%s\"\n",fname);
    fd = open(fname,O_RDONLY);
    if (fd < 0) {
      printf("*** failed to open file, errno = %d(%s)\n",errno,strerror(errno));
      failed = true;
    }
  }
  /*
   * read file
   */
  if (!failed) {
    printf("... reading from file\n");
    start_tick = rtems_clock_get_ticks_since_boot();
    curr_pos = 0;
    do {
      n = read(fd,
	       bufptr,
	       buf_size);
      if (n > 0) {
	curr_pos      += (size_t) n;
      }
    } while (n > 0);
    curr_tick = rtems_clock_get_ticks_since_boot();
    if (n < 0) {
      failed = true;
      printf("*** file read failed, "
	     "%lu bytes read, "
	     "errno = %d(%s)\n",
	     (unsigned long)curr_pos,errno,strerror(errno));
    }
    else {
      printf("%lu bytes read\n",
	     (unsigned long)curr_pos);
      printf("time elapsed for read:  %g seconds\n",
	     ((double)curr_tick-start_tick)/ticks_per_sec);
      printf("read data rate: %g KBytes/second\n",
	     (((double)curr_pos) / 1024.0 /
	      (((double)curr_tick-start_tick)/ticks_per_sec)));
    }
  }
  if (fd >= 0) {
    printf("... closing file\n");
    close(fd);
  }
  if (bufptr != NULL) {
    printf("... deallocating buffer\n");
    free(bufptr);
    bufptr = NULL;
  }
  printf("\n ******** End of file read\n");
  fileio_print_free_heap();

}

static void fileio_menu (void)
{
  char inbuf[10];

  /*
   * Wait for characters from console terminal
   */
  for (;;) {
    printf(" =========================\n");
    printf(" RTEMS FILE I/O Test Menu \n");
    printf(" =========================\n");
    printf("   p -> part_table_initialize\n");
    printf("   f -> mount all disks in fs_table\n");
    printf("   l -> list  file\n");
    printf("   r -> read  file\n");
    printf("   w -> write file\n");
#ifdef USE_SHELL
    printf("   s -> start shell\n");
#endif
    printf("   Enter your selection ==>");
    fflush(stdout);

    inbuf[0] = '\0';
    fgets(inbuf,sizeof(inbuf),stdin);
    switch (inbuf[0]) {
    case 'l':
      fileio_list_file ();
      break;
    case 'r':
      fileio_read_file ();
      break;
    case 'w':
      fileio_write_file ();
      break;
    case 'p':
      fileio_part_table_initialize ();
      break;
    case 'f':
      fileio_fsmount ();
      break;
#ifdef USE_SHELL
    case 's':
      fileio_start_shell ();
      break;
#endif
    default:
      printf("Selection `%c` not implemented\n",inbuf[0]);
      break;
    }

  }
  exit (0);
}

/*
 * RTEMS File Menu Task
 */
static rtems_task
fileio_task (rtems_task_argument ignored)
{
  fileio_menu();
}

static void
notification (int fd, int seconds_remaining, void *arg)
{
  printf(
    "Press any key to start file I/O sample (%is remaining)\n",
    seconds_remaining
  );
}

/*
 * RTEMS Startup Task
 */
rtems_task
Init (rtems_task_argument ignored)
{
  rtems_name Task_name;
  rtems_id   Task_id;
  rtems_status_code status;

  TEST_BEGIN();

  crypt_add_format(&crypt_md5_format);
  crypt_add_format(&crypt_sha512_format);

  status = rtems_shell_wait_for_input(
    STDIN_FILENO,
    20,
    notification,
    NULL
  );
  if (status == RTEMS_SUCCESSFUL) {
    Task_name = rtems_build_name('F','M','N','U');

    status = rtems_task_create(
      Task_name, 1, RTEMS_MINIMUM_STACK_SIZE * 2,
      RTEMS_DEFAULT_MODES ,
      RTEMS_FLOATING_POINT | RTEMS_DEFAULT_ATTRIBUTES, &Task_id
    );
    directive_failed( status, "create" ); 

    status = rtems_task_start( Task_id, fileio_task, 1 );
    directive_failed( status, "start" ); 

    rtems_task_exit();
  } else {
    TEST_END();

    rtems_test_exit( 0 );
  }
}

#if defined(USE_SHELL)
/*
 *  RTEMS Shell Configuration -- Add a command and an alias for it
 */

static int main_usercmd(int argc, char **argv)
{
  int i;
  printf( "UserCommand: argc=%d\n", argc );
  for (i=0 ; i<argc ; i++ )
    printf( "argv[%d]= %s\n", i, argv[i] );
  return 0;
}

static rtems_shell_cmd_t Shell_USERCMD_Command = {
  "usercmd",                                       /* name */
  "usercmd n1 [n2 [n3...]]     # echo arguments",  /* usage */
  "user",                                          /* topic */
  main_usercmd,                                    /* command */
  NULL,                                            /* alias */
  NULL                                             /* next */
};

static rtems_shell_alias_t Shell_USERECHO_Alias = {
  "usercmd",                 /* command */
  "userecho"                 /* alias */
};


#define CONFIGURE_SHELL_USER_COMMANDS &Shell_USERCMD_Command
#define CONFIGURE_SHELL_USER_ALIASES &Shell_USERECHO_Alias
#define CONFIGURE_SHELL_COMMANDS_INIT
#define CONFIGURE_SHELL_COMMANDS_ALL
#define CONFIGURE_SHELL_MOUNT_MSDOS
#define CONFIGURE_SHELL_MOUNT_RFS
#define CONFIGURE_SHELL_DEBUGRFS

#include <rtems/shellconfig.h>
#endif

#else
/*
 * RTEMS Startup Task
 */
rtems_task
Init (rtems_task_argument ignored)
{
  puts( "\n\n*** FILE I/O SAMPLE AND TEST ***" );
  puts( "\n\n*** NOT ENOUGH MEMORY TO BUILD AND RUN ***" );
}
#endif

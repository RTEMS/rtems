/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is called from init_exec and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  init.c,v 1.11 2000/06/12 15:00:12 joel Exp
 */

#define CONFIGURE_INIT
#include "system.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <rtems.h>
#include <fcntl.h>
#include <rtems/error.h>
#include <rtems/dosfs.h>
#include <ctype.h>
#include <rtems/ide_part_table.h>
#include <rtems/libcsupport.h>
#include <rtems/fsmount.h>

/*
 * Table of FAT file systems that will be mounted
 * with the "fsmount" function.
 * See cpukit/libmisc/fsmount for definition of fields
 */
fstab_t fs_table[] = {
  {
    "/dev/hda1","/mnt/hda1",
    &msdos_ops, RTEMS_FILESYSTEM_READ_WRITE,
    FSMOUNT_MNT_OK | FSMOUNT_MNTPNT_CRTERR | FSMOUNT_MNT_FAILED,
    0
  },
  {
    "/dev/hda2","/mnt/hda2",
    &msdos_ops, RTEMS_FILESYSTEM_READ_WRITE,
    FSMOUNT_MNT_OK | FSMOUNT_MNTPNT_CRTERR | FSMOUNT_MNT_FAILED,
    0
  },
  {
    "/dev/hda3","/mnt/hda3",
    &msdos_ops, RTEMS_FILESYSTEM_READ_WRITE,
    FSMOUNT_MNT_OK | FSMOUNT_MNTPNT_CRTERR | FSMOUNT_MNT_FAILED,
    0
  },
  {
    "/dev/hda4","/mnt/hda4",
    &msdos_ops, RTEMS_FILESYSTEM_READ_WRITE,
    FSMOUNT_MNT_OK | FSMOUNT_MNTPNT_CRTERR | FSMOUNT_MNT_FAILED,
    0
  },
  {
    "/dev/hdc1","/mnt/hdc1",
    &msdos_ops, RTEMS_FILESYSTEM_READ_WRITE,
    FSMOUNT_MNT_OK | FSMOUNT_MNTPNT_CRTERR | FSMOUNT_MNT_FAILED,
    0
  },
  {
    "/dev/hdc2","/mnt/hdc2",
    &msdos_ops, RTEMS_FILESYSTEM_READ_WRITE,
    FSMOUNT_MNT_OK | FSMOUNT_MNTPNT_CRTERR | FSMOUNT_MNT_FAILED,
    0
  },
  {
    "/dev/hdc3","/mnt/hdc3",
    &msdos_ops, RTEMS_FILESYSTEM_READ_WRITE,
    FSMOUNT_MNT_OK | FSMOUNT_MNTPNT_CRTERR | FSMOUNT_MNT_FAILED,
    0
  },
  {
    "/dev/hdc4","/mnt/hdc4",
    &msdos_ops, RTEMS_FILESYSTEM_READ_WRITE,
    FSMOUNT_MNT_OK | FSMOUNT_MNTPNT_CRTERR | FSMOUNT_MNT_FAILED,
    0
  }
};

#define MIN(a,b) (((a) > (b)) ? (b) : (a))

#define USE_SHELL

#ifdef USE_SHELL
#include <rtems/shell.h>

void writeScript(
  const char *name,
  const char *contents
)
{
  int sc;
  sc = setuid(0);
  if ( sc ) {
    printf( "setuid failed: %s:\n", name, strerror(errno) );
  }

  rtems_shell_write_file( name, contents );
  sc = chmod ( name, 0777 );
  if ( sc ) {
    printf( "chmod %s: %s:\n", name, strerror(errno) );
  }
}

void fileio_start_shell(void)
{
  int sc;
  sc = mkdir("/scripts", 0777);
  if ( sc ) {
    printf( "mkdir /scripts: %s:\n", strerror(errno) );
  }

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

  printf(" =========================\n");
  printf(" starting shell\n");
  printf(" =========================\n");
  rtems_shell_init(
    "SHLL",                          /* task_name */
    RTEMS_MINIMUM_STACK_SIZE * 4,    /* task_stacksize */
    100,                             /* task_priority */
    "/dev/console",                  /* devname */
    0,                               /* forever */
    1                                /* wait */
  );
}

#endif /* USE_SHELL */

void fileio_print_free_heap(void)
{
  printf("--- unused dynamic memory: %lu bytes ---\n",
	 (unsigned long) malloc_free_space());
}


void fileio_part_table_initialize(void)
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
  rc = rtems_ide_part_table_initialize(devname);
  printf("result = %d\n",rc);
  fileio_print_free_heap();
}

void fileio_fsmount(void)
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

void fileio_list_file(void)
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
      printf("*** file open failed, errno = %d(%s)\n",errno,strerror(errno));
    }
  }

  if (fd >= 0) {
    rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &start_tick);
    do {
      n = read(fd,buf_ptr,buf_size);
      if (n > 0) {
	write(1,buf_ptr,n);
	flen += n;
      }
    } while (n > 0);

    rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &curr_tick);

    printf("\n ******** End of file reached, flen = %d\n",flen);
    close(fd);

    rtems_clock_get(RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticks_per_sec);
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
bool fileio_str2size(const char *str,uint32_t   *res_ptr)
{
  bool failed = false;
  unsigned long size;
  char suffix = ' ';

  if (1 > sscanf(str,"%lu%c",&size,&suffix)) {
    failed = true;
  }
  else if (toupper(suffix) == 'K') {
    size *= 1024;
  }
  else if (toupper(suffix) == 'M') {
    size *= 1024UL*1024UL;
  }
  else if (isalpha(suffix)) {
    failed = true;
  }

  if (!failed) {
    *res_ptr = size;
  }
  return failed;
}

void fileio_write_file(void)
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
  rtems_clock_get(RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticks_per_sec);

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
    rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &start_tick);
    curr_pos = 0;
    do {
      bytes_to_copy = buf_size;
      do {
	n = write(fd,
	  bufptr + (buf_size-bytes_to_copy),
		  MIN(bytes_to_copy,file_size-curr_pos));
	if (n > 0) {
	  bytes_to_copy -= n;
	  curr_pos      += n;
	}
      } while ((bytes_to_copy > 0)  && (n > 0));
    } while ((file_size > curr_pos) && (n > 0));
    rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &curr_tick);
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

void fileio_read_file(void)
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
  rtems_clock_get(RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticks_per_sec);

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
      printf("*** file open failed, errno = %d(%s)\n",errno,strerror(errno));
      failed = true;
    }
  }
  /*
   * read file
   */
  if (!failed) {
    printf("... reading from file\n");
    rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &start_tick);
    curr_pos = 0;
    do {
      n = read(fd,
	       bufptr,
	       buf_size);
      if (n > 0) {
	curr_pos      += n;
      }
    } while (n > 0);
    rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &curr_tick);
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

void fileio_menu (void)
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

int menu_tid;

/*
 * RTEMS Startup Task
 */
rtems_task
Init (rtems_task_argument ignored)
{
  puts( "\n\n*** FILE I/O SAMPLE AND TEST ***" );

  fileio_menu();
}

/*
 *  RTEMS Shell Configuration -- Add a command and an alias for it
 */

int main_usercmd(int argc, char **argv)
{
  int i;
  printf( "UserCommand: argc=%d\n", argc );
  for (i=0 ; i<argc ; i++ )
    printf( "argv[%d]= %s\n", i, argv[i] );
  return 0;
}

rtems_shell_cmd_t Shell_USERCMD_Command = {
  "usercmd",                                       /* name */
  "usercmd n1 [n2 [n3...]]     # echo arguments",  /* usage */
  "user",                                          /* topic */
  main_usercmd,                                    /* command */
  NULL,                                            /* alias */
  NULL                                             /* next */
};

rtems_shell_alias_t Shell_USERECHO_Alias = {
  "usercmd",                 /* command */
  "userecho"                 /* alias */
};
  

#define CONFIGURE_SHELL_USER_COMMANDS &Shell_USERCMD_Command
#define CONFIGURE_SHELL_USER_ALIASES &Shell_USERECHO_Alias
#define CONFIGURE_SHELL_COMMANDS_INIT
#define CONFIGURE_SHELL_COMMANDS_ALL
#define CONFIGURE_SHELL_MOUNT_MSDOS

#include <rtems/shellconfig.h>


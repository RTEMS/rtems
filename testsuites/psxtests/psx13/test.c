/*
 *  Psx13
 *  Chris Bond (working under Jennifer's account)
 *
 *  This test exercises the following routines:
 *
 *     device_lseek - test implemented
 *     dup          - test implemented
 *     dup2         - test implemented
 *     fdatasync    - test implemented
 *     fsync        - test implemented
 *     pathconf     - test implemented
 *     fpathconf    - test implemented
 *     umask        - test implemented
 *     utime        - test implemented
 *     utimes       - test implemented
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/libio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <utime.h>

#include <stdio.h>
#include <unistd.h>

#include <pmacros.h>

int InitFiles(void);
int DeviceLSeekTest(void);
int DupTest(void);
int Dup2Test(void);
int FDataSyncTest(void);
int UMaskTest(void);
int UTimeTest(void);
int UTimesTest(void);
int PathConfTest(void);
int FPathConfTest(void);
int FSyncTest(void);

/*-------------------------------------------------------------------
 * InitFiles function
 *
 * Initializes the three files to be used in the test.
 *
 * arguments: none
 * assumptions: fopen, fprintf, fwrite, FILE are available
 * actions: creates testfile1, a text file with 'a'..'z' listed 4 times.
 *          creates testfile2, a text file with 'a'..'z' listed 4 times.
 *          creates testfile3, a binary file with 0..9 listed 4 times.
 * returns: TRUE if files opened successfully.
 *          FALSE if fail on file open for write.
 *
 * ------------------------------------------------------------------
 */

int InitFiles (void)
{
  int count;
  FILE *fp1, *fp2, *fp3;
  char letter;
  int number;
  int retval;

  fp1 = fopen("testfile1.tst", "wt");
  fp2 = fopen("testfile2.tst", "wt");
  fp3 = fopen("testfile4.tst", "wb");

  if ((fp1 != NULL) && (fp2 != NULL) && (fp3 !=NULL)) {

    letter = 'a';

    for (count=0 ; count<(26*4); ++count) {
      fprintf (fp1, "%c", letter);
      fprintf (fp2, "%c", letter);

      ++letter;
      if (letter > 'z')
	letter = 'a';
    }

    number = 0;

    for (count = 0; count <40; ++count) {

      fwrite (&number, 1, sizeof(int), fp3);

      ++number;
      if (number > 9)
	number = 0;
    }

    fclose(fp1);
    fclose(fp2);
    fclose(fp3);

    retval = TRUE;
  }

  else
    retval = FALSE;

  /* assert (retval == TRUE);*/

  return (retval);
}

/* ---------------------------------------------------------------
 * DeviceLSeekTest function
 *
 * Hits the device_lseek code by lseeking on the console.
 *
 * arguments: none
 * assumptions: lseek available
 * actions: hits lseek with some dummy arguments.
 * returns: value of return from lseek.
 *
 * ---------------------------------------------------------------
 */

int DeviceLSeekTest (void)
{
  int error = -1, retval = FALSE;

  int fd = open ("/dev/console", O_RDONLY);

  error = lseek(fd, 5, SEEK_SET);

  if (error == 0)
    retval = TRUE;
  else
    retval = FALSE;

  close( fd );  /* assert (retval == TRUE);*/

  return (retval);
}

/* ---------------------------------------------------------------
 * DupTest function
 *
 * Hits the dup code.
 *
 * arguments: none
 * assumptions: dup, open, close, fcntl available.
 * actions: Gets a file descriptor(fd1) for test file1.
 *          dups fd1 to fd2.
 *          sets fd1 to append mode
 *          checks fd2 to ensure it's in append mode, also.
 * returns: success if fd2 is indeed a copy of fd1.
 *
 * ---------------------------------------------------------------
 */

int DupTest(void)
{
  int fd1, fd2;

  int flags = 0, retval = FALSE;

  fd1 = open ("testfile1.tst", O_RDONLY);
  fd2 = dup(fd1);

  if (fd2 != -1) {

    fcntl(fd1, F_SETFL, O_APPEND);
    flags = fcntl(fd2, F_GETFL);

    close (fd1);

    flags = (flags & O_APPEND);

    retval = (flags == O_APPEND);
  }

  else
    retval = FALSE;

  close( fd1 );
  close( fd2 );
  /* assert (retval == TRUE);*/

  return (retval);
}

/* ---------------------------------------------------------------
 * Dup2Test function
 *
 * Hits the dup2 code.
 *
 * arguments: none
 * assumptions: dup, dup2, open, close, fcntl available.
 * actions: Gets a file descriptor(fd1) for test file1.
 *          dups fd1 to fd2.
 *          sets fd1 to append mode
 *          checks fd2 to ensure it's in append mode, also.
 *          sets fd1 to invalid value, fd2 to valid, tries to dup2.
 *          sets fd2 to invalid value, fd1 to valid tries to dup2.
 * returns: success if fd2 is a copy of fd1, and invalid fd1 or fd2 produce errors.
 *
 * ---------------------------------------------------------------
 */

int Dup2Test(void)
{
  int fd1, fd2;

  int flags = 0, retval = FALSE;

  int error = 0;

  fd1 = open ("testfile1.tst", O_RDONLY);
  fd2 = open ("testfile2.tst", O_RDONLY);
  error = dup2(fd1, fd2);

  /* make sure dup2 works if both fd1 and fd2 are valid file descriptors. */

  if (error != -1) {

    fcntl(fd1, F_SETFL, O_APPEND);
    flags = fcntl(fd1, F_GETFL);

    flags = (flags & O_APPEND);
    retval = (flags == O_APPEND);
  }

  else {
    retval = FALSE;
    close(fd2);
  }

  if (retval == TRUE) {

    /* make sure dup2 fails correctly if one or the other arguments are invalid. */
    /* this assumes -1 is an invalid value for a file descriptor!!! (POSIX book, p.135) */

    fd1 = -1;

    if (dup2 (fd1, fd2) != -1)
      retval = FALSE;
    else {
      fd1 = dup(fd2);
      fd2 = -1;

      if (dup2(fd1, fd2) != -1)
	retval = FALSE;
    }
  }

  close (fd1);
  close (fd2);
  /* assert (retval == TRUE);*/

  return (retval);
}

/* ---------------------------------------------------------------
 * FDataSyncTest function
 *
 * Hits the fdatasync code. Does NOT test the functionality of the
 * underlying fdatasync entry in the IMFS op table.
 *
 * arguments: none
 * assumptions: open, close, fdatasync functions available.
 * actions: attempts to fdatasync a file descriptor flagged as read-only.
 *          attempts to fdatasync an invalid file descriptor (-1).
 *          attempts to fdatasync a perfectly valid fd opened as RDWR
 *
 * returns: TRUE if attempt to fdatasync invalid and read-only filed
 *           descriptor fail, and fdatasync succeeds on valid fd.
 *          FALSE otherwise.
 *
 * ---------------------------------------------------------------
 */

int FDataSyncTest(void)
{
  int fd = -1;
  int error = 0, retval = TRUE;

  /* Try it with a RD_ONLY file. */

  fd = open ("testfile1.tst", O_RDONLY);

  error = fdatasync(fd);
  if ((error == -1) && (errno == EINVAL))
    retval = TRUE;
  else
    retval = FALSE;

  close (fd);

  if (retval == TRUE) {

    /* Try it with a bad file descriptor */

    fd = -1;

    error = fdatasync(fd);
    if ((errno == EBADF) && (error == -1))
      retval = TRUE;
    else
      retval = FALSE;
  }

  /* Okay - now the success case... */

  if (retval == TRUE) {
    fd = open ("testfile1.tst", O_RDWR);
    error = fdatasync(fd);

    if (error == 0)
      retval = TRUE;
    else
      retval = FALSE;

    close (fd);
  }

  /* assert (retval == TRUE);*/

  return (retval);
}

/* ---------------------------------------------------------------
 * UMaskTest function
 *
 * Hits the umask code.
 *
 * arguments: none
 * assumptions: umask function available.
 * actions: set umask to 0ctal 23.
 *          set umask to Octal 22, retrieve the old value.
 *
 * returns: TRUE if old value is 23,
 *          FALSE otherwise.
 *
 * ---------------------------------------------------------------
 */

int UMaskTest (void)
{
  mode_t error = 0;
  int    retval = FALSE;

  umask(023);
  error = umask(022);

  if (error == 023)
    retval = TRUE;
  else
    retval = FALSE;

  /* assert (retval == TRUE);*/

  return(retval);
}

/* ---------------------------------------------------------------
 * UTimeTest function
 *
 * Hits the utime code. Does NOT test the functionality of the underlying utime
 * entry in the IMFS op table.
 *
 * arguments: none
 * assumptions: utime function available.
 * actions: set utime for an invalid filename.
 *          set utime for a valid filename.
 *
 * returns: TRUE if time on valid file is set correctly and utime failed on
 *          an invalid filename.
 *          FALSE otherwise.
 *
 * ---------------------------------------------------------------
 */

int UTimeTest (void)
{
  int error = 0, retval = FALSE;
  struct utimbuf time;
  struct stat fstat;

  /* First, an invalid filename. */
  error = utime("!This is an =invalid p@thname!!! :)", NULL);

  if (error == -1)
    retval = TRUE;
  else
    retval = FALSE;

  /* Now, the success test. */
  if (retval == TRUE) {

    time.actime  = 12345;
    time.modtime = 54321;

    error = utime("testfile1.tst", &time);

    if (error == 0) {

      /* But, did it set the time? */
      stat ("testfile1.tst", &fstat);

      if ((fstat.st_atime == 12345) && (fstat.st_mtime == 54321 ))
	retval = TRUE;
      else
	retval = FALSE;
    }
    else
      retval = FALSE;

    error = utime("testfile1.tst", NULL );
    retval &= (error == 0) ? TRUE : FALSE;
  }

  /* assert (retval == TRUE);*/

  return (retval);
}

/* ---------------------------------------------------------------
 * UTimesTest function
 *
 * Hits the utimes code. Does NOT test the functionality of the underlying utime
 * entry in the IMFS op table.
 *
 * arguments: none
 * assumptions: utimes function available.
 * actions: set utimes for an invalid filename.
 *          set utimes for a valid filename.
 *
 * returns: TRUE if time on valid file is set correctly and utimes failed on
 *          an invalid filename.
 *          FALSE otherwise.
 *
 * ---------------------------------------------------------------
 */

int UTimesTest (void)
{
  int error = 0, retval = FALSE;
  struct timeval time[2];
  struct stat fstat;

  /* First, an invalid filename. */
  error = utimes("!This is an =invalid p@thname!!! :)", NULL);

  if (error == -1)
    retval = TRUE;
  else
    retval = FALSE;

  /* Now, the success test. */
  if (retval == TRUE) {

    time[0].tv_sec = 12345;
    time[1].tv_sec = 54321;

    error = utimes("testfile1.tst", (struct timeval *)&time);

    if (error == 0) {

      /* But, did it set the time? */
      stat ("testfile1.tst", &fstat);

      if ((fstat.st_atime == 12345) && (fstat.st_mtime == 54321 ))
	retval = TRUE;
      else
	retval = FALSE;
    }

    else
      retval = FALSE;
  }

  /* assert (retval == TRUE);*/

  return (retval);
}

/* ---------------------------------------------------------------
 * PathConfTest function
 *
 * Hits the pathconf code.
 *
 * arguments: none
 * assumptions: pathconf function available.
 * actions: Try to pathconf a bad filename.
 *          Try to pathconf a good filename.
 *
 * returns: TRUE if pathconf fails on bad file, succeeds on good file.
 *          FALSE otherwise.
 *
 * ---------------------------------------------------------------
 */

int PathConfTest (void)
{
  int error = 0, retval = FALSE;

  error = pathconf("thisfiledoesnotexist", _PC_LINK_MAX);

  if (error == -1) {
    error = pathconf("testfile1.tst", _PC_LINK_MAX);

    if (error != -1)
      retval = TRUE;
    else
      retval = FALSE;
  }

  else
    retval = FALSE;

  /* assert (retval == TRUE);*/

  return(retval);
}

/* ---------------------------------------------------------------
 * FPathConfTest function
 *
 * Hits the fpathconf code.
 *
 * arguments: none
 * assumptions: fpathconf function available.
 * actions: Call fpathconf with all arguments, plus an invalid.
 *
 * returns: TRUE always.
 *
 * ---------------------------------------------------------------
 */

int FPathConfTest (void)
{
  int error = 0, retval = TRUE;

  int fd  = -1;

  error = fpathconf(fd, _PC_LINK_MAX);

  if (error == -1) {
    fd = open("testfile1.tst", O_RDWR);

    error = fpathconf(fd, _PC_LINK_MAX);
    error = fpathconf(fd, _PC_MAX_CANON);
    error = fpathconf(fd, _PC_MAX_INPUT);
    error = fpathconf(fd, _PC_NAME_MAX);
    error = fpathconf(fd, _PC_PATH_MAX);
    error = fpathconf(fd, _PC_PIPE_BUF);
    error = fpathconf(fd, _PC_CHOWN_RESTRICTED);
    error = fpathconf(fd, _PC_NO_TRUNC);
    error = fpathconf(fd, _PC_VDISABLE);
    error = fpathconf(fd, _PC_ASYNC_IO);
    error = fpathconf(fd, _PC_PRIO_IO);
    error = fpathconf(fd, _PC_SYNC_IO);
    error = fpathconf(fd, 255);

    close(fd);

    fd = open("testfile1.tst", O_WRONLY);
    
    error = fpathconf(fd, _PC_LINK_MAX);

    retval = TRUE;
  }

  else
    retval = FALSE;

  /* assert (retval == TRUE);*/

  return(retval);
}

/* ---------------------------------------------------------------
 * FSyncTest function
 *
 * Hits the fsync code.
 *
 * arguments: none
 * assumptions: open, fsync functions available.
 * actions: open test file,
 *          try to fsync it.
 *
 * returns: TRUE if fsync doesn't return -1,
 *          FALSE otherwise.
 *
 * ---------------------------------------------------------------
 */

int FSyncTest (void)
{
  int error = 0, retval = FALSE;
  int fd = -1;

  fd = open("testfile1.tst", O_RDWR);

  if (fd != -1) {

    error = fsync(fd);

    if (error != -1)
      retval = TRUE;
    else
      retval = FALSE;

    close(fd);
  }

  else
    retval = FALSE;

  /* assert (retval == TRUE);*/

  return(retval);
}

/* ---------------------------------------------------------------
 * Main function
 *
 *  main entry point to the test
 *
 * ---------------------------------------------------------------
 */

#if defined(__rtems__)
int test_main(void);
int test_main(void)
#else
int main(
  int    argc,
  char **argv
)
#endif
{
  puts( "*** POSIX TEST 13 ***" );

  if (InitFiles() == TRUE) {
    printf ("\nFiles initialized successfully.\n");

    printf ("Testing device_lseek()... ");
    if (DeviceLSeekTest() == TRUE)
      printf ("Success.\n");
    else
      printf ("Failed!!!\n");

    printf ("Testing dup()............ ");
    if (DupTest() == TRUE)
      printf ("Success.\n");
    else
      printf ("Failed!!!\n");

    printf ("Testing dup2()........... ");
    if (Dup2Test() == TRUE)
      printf ("Success.\n");
    else
      printf ("Failed!!!\n");

    printf ("Testing fdatasync()...... ");
    if (FDataSyncTest() == TRUE)
      printf ("Success.\n");
    else
      printf ("Failed!!!\n");

    printf ("Testing umask().......... ");
    if (UMaskTest() == TRUE)
      printf ("Success.\n");
    else
      printf ("Failed!!!\n");

   printf ("Testing utime().......... ");
    if (UTimeTest() == TRUE)
      printf ("Success.\n");
    else
      printf ("Failed!!!\n");

   printf ("Testing utimes().......... ");
    if (UTimesTest() == TRUE)
      printf ("Success.\n");
    else
      printf ("Failed!!!\n");

   printf ("Testing fsync().......... ");
    if (FSyncTest() == TRUE)
      printf ("Success.\n");
    else
      printf ("Failed!!!\n");

   printf ("Testing pathconf()....... ");
    if (PathConfTest() == TRUE)
      printf ("Success.\n");
    else
      printf ("Failed!!!\n");

   printf ("Testing fpathconf()...... ");
    if (FPathConfTest() == TRUE)
      printf ("Success.\n");
    else
      printf ("Failed!!!\n");

    printf ("Testing sync()...... ");
    sync();
  }
  else
    printf ("\n\nError opening files for write!!!!\n");

  printf( "\n\n*** END OF TEST PSX13 ***" );
  rtems_test_exit(0);
}

/**
 *  @file
 *
 *  This test exercises the following routines:
 *
 *    + readv
 *    + writev
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <utime.h>
#include <string.h>
#include <inttypes.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/uio.h>

#if defined(__rtems__)
  #include <rtems.h>
  #include <rtems/libio.h>
  #include <pmacros.h>
#else
  #define TRUE  1
  #define FALSE 0
  #include <stdlib.h>
  #define rtems_test_exit(_s) exit(_s)
#endif

/* forward declarations to avoid warnings */
int test_main(void);
int fillPatternBuffer(void);
int doFunctionalTest(void);
int doErrorTest(void);

#define TESTFILE "testfile1.tst"

/* This buffer size is assumed in the iovec initialization below */
#define MAX_BUFFER 1000
unsigned char PatternBuffer[MAX_BUFFER];
unsigned char ReadBuffer[MAX_BUFFER];

/*
 * fillPatternBuffer function
 *
 * Fill the test buffer.
 *
 * Returns: TRUE if buffer filled
 *          FALSE if buffer failed to fill
 *
 */
int fillPatternBuffer(void)
{
  int retval = TRUE;
  int i;

  for (i=0 ; i<200  ; i++ ) PatternBuffer[i] = 'a';
  for (    ; i<400  ; i++ ) PatternBuffer[i] = 'b';
  for (    ; i<600  ; i++ ) PatternBuffer[i] = 'c';
  for (    ; i<800  ; i++ ) PatternBuffer[i] = 'd';
  for (    ; i<1000 ; i++ ) PatternBuffer[i] = 'e';
  return retval;
}

/*
 * doFunctionalTest function
 *
 * Write a file with writev and then read it back with readv.
 *
 * Returns: TRUE if all operations worked as expected
 *          FALSE if an operation did not work as expected.
 *
 */
int doFunctionalTest(void)
{
  FILE         *fp;
  int           fd;
  struct iovec  rdvec[4];
  struct iovec  wrvec[4];
  int           rc;


  /*
   * Setup the iovec
   */
  wrvec[0].iov_base = &PatternBuffer[0];
  wrvec[0].iov_len  = 100;
  wrvec[1].iov_base = &PatternBuffer[100];
  wrvec[1].iov_len  = 200;
  wrvec[2].iov_base = &PatternBuffer[300];
  wrvec[2].iov_len  = 300;
  wrvec[3].iov_base = &PatternBuffer[600];
  wrvec[3].iov_len  = 400;

  rdvec[0].iov_base = &ReadBuffer[0];
  rdvec[0].iov_len  = 400;
  rdvec[1].iov_base = &ReadBuffer[400];
  rdvec[1].iov_len  = 300;
  rdvec[2].iov_base = &ReadBuffer[700];
  rdvec[2].iov_len  = 200;
  rdvec[3].iov_base = &ReadBuffer[900];
  rdvec[3].iov_len  = 100;

  /*
   * Write the File
   */
  fp = fopen(TESTFILE, "wt");
  if ( fp == NULL ) {
    printf( "fopen for write: %d=%s\n", errno, strerror(errno));
    return FALSE;
  }
  fd = fileno(fp);

  rc = writev(fd, wrvec, 4);
  if ( rc <= 0 ) {
    printf( "writev: %d=%s\n", errno, strerror(errno) );
    return FALSE;
  }

  fclose(fp);

  puts("File written using writev .. OK");

  /*
   * Now read it back and check it
   */

  fp = fopen(TESTFILE, "rt");
  if ( fp == NULL ) {
    printf( "fopen for write: %d=%s\n", errno, strerror(errno));
    return FALSE;
  }
  fd = fileno(fp);

  rc = readv(fd, rdvec, 4);
  if ( rc <= 0 ) {
    printf( "rd: %d=%s\n", errno, strerror(errno) );
    return FALSE;
  }

  if ( memcmp( PatternBuffer, ReadBuffer, MAX_BUFFER ) ) {
    puts("readv .. Buffers do not match");
    return FALSE;
  }

  puts("File read using readv .. OK");

  return TRUE;
}

/*
 * doErrorTest function
 *
 * Hit all the error cases in readv/writev.
 *
 * Returns: TRUE if all operations worked as expected
 *          FALSE if an operation did not work as expected.
 *
 */
int doErrorTest(void)
{
  FILE         *fp;
  int           fd;
  struct iovec  vec[4];
  int           rc;

  /*
   * Open and close the file to get a bad file descriptor
   */
  fp = fopen(TESTFILE, "wt");
  if ( fp == NULL ) {
    printf( "fopen for error 1: %d=%s\n", errno, strerror(errno));
    return FALSE;
  }
  fd = fileno(fp);
  fclose(fp);

  /* writev -- bad file descriptor */
  puts("writev bad file descriptor -- EBADF");
  rc = writev(fd, vec, 4);
  if ( (rc != -1) || (errno != EBADF) ) {
    printf( "writev error 1: %d=%s\n", errno, strerror(errno) );
    return FALSE;
  }

  /* readv -- bad file descriptor */
  puts("readv bad file descriptor -- EBADF");
  rc = read(fd, vec, 4);
  if ( (rc != -1) || (errno != EBADF) ) {
    printf( "readv error 1: %d=%s\n", errno, strerror(errno) );
    return FALSE;
  }

  /*
   * Open the file for the rest of the tests
   */
  fp = fopen(TESTFILE, "w+");
  if ( fp == NULL ) {
    printf( "fopen for error 2: %d=%s\n", errno, strerror(errno));
    return FALSE;
  }
  fd = fileno(fp);

  /* writev --  bad iovec pointer */
  puts("writev bad iovec pointer -- EINVAL");
  rc = writev(fd, NULL, 4);
  if ( (rc != -1) || (errno != EINVAL) ) {
    printf( "writev error 2: %d=%s\n", errno, strerror(errno) );
    fclose(fp);
    return FALSE;
  }

  /* readv --  bad iovec pointer */
  puts("readv bad iovec pointer -- EINVAL");
  rc = readv(fd, NULL, 4);
  if ( (rc != -1) || (errno != EINVAL) ) {
    printf( "readv error 2: %d=%s\n", errno, strerror(errno) );
    fclose(fp);
    return FALSE;
  }

  /* writev --  bad iovcnt 0 */
  puts("readv bad iovcnt of 0 -- EINVAL");
  rc = writev(fd, vec, 0);
  if ( (rc != -1) || (errno != EINVAL) ) {
    printf( "writev error 3: %d=%s\n", errno, strerror(errno) );
    fclose(fp);
    return FALSE;
  }

  /* readv --  bad iovcnt 0 */
  puts("readv bad iovcnt of 0 -- EINVAL");
  rc = readv(fd, vec, 0);
  if ( (rc != -1) || (errno != EINVAL) ) {
    printf( "readv error 3: %d=%s\n", errno, strerror(errno) );
    fclose(fp);
    return FALSE;
  }

  /* writev --  bad iovcnt negative */
  puts("writev bad iovcnt negative -- EINVAL");
  rc = writev(fd, vec, -2);
  if ( (rc != -1) || (errno != EINVAL) ) {
    printf( "writev error 4: %d=%s\n", errno, strerror(errno) );
    fclose(fp);
    return FALSE;
  }

  /* readv --  bad iovcnt negative */
  puts("readv bad iovcnt negative -- EINVAL");
  rc = readv(fd, vec, -100);
  if ( (rc != -1) || (errno != EINVAL) ) {
    printf( "readv error 4: %d=%s\n", errno, strerror(errno) );
    fclose(fp);
    return FALSE;
  }

  /* writev --  bad iov[i].iov_base */
  vec[0].iov_base = vec;
  vec[0].iov_len = 100;
  vec[1].iov_base = NULL;
  vec[1].iov_len = 100;
  puts("writev bad iov[i].iov_base -- EINVAL");
  rc = writev(fd, vec, 2);
  if ( (rc != -1) || (errno != EINVAL) ) {
    printf( "writev error 5: %d=%s\n", errno, strerror(errno) );
    fclose(fp);
    return FALSE;
  }

  /*  readv --  bad iov[i].iov_base */
  vec[0].iov_base = vec;
  vec[0].iov_len = 100;
  vec[1].iov_base = NULL;
  vec[1].iov_len = 100;
  puts("readv bad iov[i].iov_base -- EINVAL");
  rc = readv(fd, vec, 2);
  if ( (rc != -1) || (errno != EINVAL) ) {
    printf( "readv error 5: %d=%s\n", errno, strerror(errno) );
    fclose(fp);
    return FALSE;
  }

  /*  writev --  bad iov[i].iov_len < 0 */
  vec[0].iov_base = vec;
  vec[0].iov_len = 100;
  vec[1].iov_base = vec;
  vec[1].iov_len = -10;
  puts("writev bad iov[i].iov_len < 0 -- EINVAL");
  rc = writev(fd, vec, 2);
  if ( (rc != -1) || (errno != EINVAL) ) {
    printf( "writev error 6: %d=%s\n", errno, strerror(errno) );
    fclose(fp);
    return FALSE;
  }

  /*  readv --  bad iov[i].iov_len = 0 */
  vec[0].iov_base = vec;
  vec[0].iov_len = 100;
  vec[1].iov_base = vec;
  vec[1].iov_len = -1024;
  puts("readv bad iov[i].iov_len = 0 -- EINVAL");
  rc = readv(fd, vec, 2);
  if ( (rc != -1) || (errno != EINVAL) ) {
    printf( "readv error 6: %d=%s\n", errno, strerror(errno) );
    fclose(fp);
    return FALSE;
  }

  /*  writev --  iov_len total overflows */
  vec[0].iov_base = vec;
  vec[0].iov_len = SIZE_MAX;
  vec[1].iov_base = vec;
  vec[1].iov_len = SIZE_MAX;
  vec[2].iov_base = vec;
  vec[2].iov_len = SIZE_MAX;
  puts("writev iov_len total overflows -- EINVAL");
  rc = writev(fd, vec, 3);
  if ( (rc != -1) || (errno != EINVAL) ) {
    printf( "writev error 7: rc=%d %d=%s\n", rc, errno, strerror(errno) );
    fclose(fp);
    return FALSE;
  }

  /*  readv --  iov_len total overflows */
  vec[0].iov_base = vec;
  vec[0].iov_len = SIZE_MAX;
  vec[1].iov_base = vec;
  vec[1].iov_len = SIZE_MAX;
  puts("readv iov_len total overflows -- EINVAL");
  rc = readv(fd, vec, 2);
  if ( (rc != -1) || (errno != EINVAL) ) {
    printf( "read error 7: rc=%d %d=%s\n", rc, errno, strerror(errno) );
    fclose(fp);
    return FALSE;
  }

  /*  writev --  all zero length buffers */
  vec[0].iov_base = vec;
  vec[0].iov_len = 0;
  vec[1].iov_base = vec;
  vec[1].iov_len = 0;
  puts("writev iov_len works with no effect -- OK");
  rc = writev(fd, vec, 2);
  if ( (rc != 0) ) {
    printf( "writev error 8: %d=%s\n", errno, strerror(errno) );
    fclose(fp);
    return FALSE;
  }

  /*  readv --  all zero length buffers */
  vec[0].iov_base = vec;
  vec[0].iov_len = 0;
  vec[1].iov_base = vec;
  vec[1].iov_len = 0;
  puts("readv iov_len works with no effect -- OK");
  rc = readv(fd, vec, 2);
  if ( (rc != 0) ) {
    printf( "readv error 8: %d=%s\n", errno, strerror(errno) );
    fclose(fp);
    return FALSE;
  }

  fclose(fp);
  return TRUE;
}


/* ---------------------------------------------------------------
 * Main function
 *
 *  main entry point to the test
 *
 * ---------------------------------------------------------------
 */

#if defined(__rtems__)
int test_main(void)
#else
int main(
  int    argc,
  char **argv
)
#endif
{
  puts( "*** POSIX TEST READV/WRITEV ***" );

  if ( fillPatternBuffer() != TRUE ) {
    puts("Error filling pattern buffer" );
    rtems_test_exit(0);
  }

  if (doErrorTest() != TRUE) {
    puts("Error during error test!!!!");
    rtems_test_exit(0);
  }
  if (doFunctionalTest() != TRUE) {
    puts("Error during functional test!!!!");
    rtems_test_exit(0);
  }

  unlink(TESTFILE);
  puts( "*** END OF TEST PSXRDWRV ***" );
  rtems_test_exit(0);
}

/*
 *  Simple test program to exercise some of the basic functionality of 
 *  POSIX Files and Directories Support.
 *
 *  This test assumes that the file system is initialized with the 
 *  following directory structure:
 *
 *  XXXX fill this in.
 *    /
 *    /dev
 *    /dev/XXX   [where XXX includes at least console]
 *
 *
 */

#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include <assert.h>

/*
 *  Main entry point of the test
 */

#if defined(__rtems__)
int test_main(void)
#else
int main(
  int argc,
  char **argv
)
#endif
{
  printf( "\n\n*** FILE TEST 2 ***\n" );

  /*
   *  XXX test code goes here
   */

  printf( "*** END OF FILE TEST 2 ***\n" );
  exit( 0 );
}

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include "test_support.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <rtems/libcsupport.h>

const char rtems_test_name[] = "SPFIFO 2";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void create_all_barriers(void);
void delete_barrier(void);
void create_fifo(void);
void open_fifo(int expected, int flags);

#define MAXIMUM 10
#define NUM_OPEN_REQ 26

rtems_id Barriers[MAXIMUM];
int BarrierCount;

rtems_id Semaphores[MAXIMUM];
int SemaphoreCount;

void create_all_barriers(void)
{
  rtems_status_code status;
  int               i;

  BarrierCount = 0;

  memset( Barriers, 0, sizeof(Barriers) );
  for ( i=0 ; i<MAXIMUM ; i++ ) {
    status = rtems_barrier_create(
      rtems_build_name( 'B', 'A', 'R', 0x30+i ),
      RTEMS_BARRIER_MANUAL_RELEASE,
      0,
      &Barriers[i]
    );
    if ( status == RTEMS_TOO_MANY ) {
      printf( "%d Barriers created\n", BarrierCount+1 );
      return;
    } 

    directive_failed( status, "barrier create" );
    BarrierCount++;
  }
}

void delete_barrier(void)
{
  rtems_status_code status;
  
  BarrierCount--;
  printf( "Deleting barrier id=0x%08x\n",
    (unsigned int)Barriers[BarrierCount] );
  status = rtems_barrier_delete( Barriers[BarrierCount] );
  directive_failed( status, "barrier delete" );
}

void create_fifo(void)
{
  int status;

  status = mkfifo("/fifo01", 0777);
  rtems_test_assert(status == 0);
}

void open_fifo(int expected, int flags)
{
  int fd;

  fd = open("/fifo01", flags);
  printf( "expect status=%d errno=%d/(%s)\n", fd, errno, strerror(errno) );
  if ( expected ) {
    rtems_test_assert(fd == -1);
    rtems_test_assert(errno == expected); 
  } else {
    rtems_test_assert(fd != -1);
    close( fd );
  }
}

rtems_task Init(
  rtems_task_argument argument
)
{
  void *alloc_ptr = (void *)0;
  int num_opens = 0;
  int index = 0;

  TEST_BEGIN();

  puts( "Creating all barriers" );
  create_all_barriers();

  puts( "Creating FIFO" );
  create_fifo();

  alloc_ptr = malloc( malloc_free_space() - 4 );
  puts("Opening FIFO.. expect ENOMEM since no memory is available");
  open_fifo(ENOMEM, O_RDWR);

  free(alloc_ptr);
  puts( "Opening FIFO.. expect ENOMEM (barrier-1 for pipe could not be created)" );
  open_fifo(ENOMEM, O_RDWR);

  delete_barrier();
  puts( "Opening FIFO.. expect ENOMEM (barrier-2 for pipe could not be created" );
  open_fifo(ENOMEM, O_RDWR);

  delete_barrier();
  puts( "Opening FIFO in RDWR mode. Expect OK" );
  open_fifo(0, O_RDWR);
  ++num_opens;

  puts( "Opening FIFO in non blocking RDONLY mode. Expect OK");
  open_fifo(0, O_RDONLY | O_NONBLOCK);
  ++num_opens;

  puts( "Opening FIFO in non blocking WRONLY mode. Expect ENXIO");
  open_fifo(ENXIO, O_WRONLY | O_NONBLOCK);
  ++num_opens;

  puts("\nMultiple opens\n");
  index = 0;
  do {

    printf("%d... ", index+1);
    open_fifo(0, O_RDONLY | O_NONBLOCK);
    ++index;
  } while ( index < NUM_OPEN_REQ - num_opens );

  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 5
#define CONFIGURE_MAXIMUM_FIFOS 1

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */

/*
/////////////////////////////////////////////////////////////////////////////
// $Header$
//
// Copyright (c) 2000 - Rosimildo da Silva
//
// MODULE DESCRIPTION:
// This module implements the input devices interface used by MicroWindows
// in an embedded system environment.
// It uses the RTEMS message queue as the repository for the messages posted
// by the devices registered.
//
// MODIFICATION/HISTORY:
//
// $Log$
// Revision 1.6  2004/03/26 06:59:18  ralf
// 2004-03-26	Ralf Corsepius <ralf_corsepius@rtems.org>
//
// 	* libmisc/capture/capture-cli.c, libmisc/capture/capture.c,
// 	libmisc/capture/capture.h, libmisc/cpuuse/cpuuse.c,
// 	libmisc/devnull/devnull.c, libmisc/fsmount/fsmount.h,
// 	libmisc/monitor/mon-config.c, libmisc/monitor/mon-dname.c,
// 	libmisc/monitor/mon-driver.c, libmisc/monitor/mon-extension.c,
// 	libmisc/monitor/mon-itask.c, libmisc/monitor/mon-monitor.c,
// 	libmisc/monitor/mon-mpci.c, libmisc/monitor/mon-object.c,
// 	libmisc/monitor/mon-prmisc.c, libmisc/monitor/mon-queue.c,
// 	libmisc/monitor/mon-server.c, libmisc/monitor/mon-symbols.c,
// 	libmisc/monitor/monitor.h, libmisc/monitor/symbols.h,
// 	libmisc/mw-fb/mw_uid.c, libmisc/rtmonuse/rtmonuse.c,
// 	libmisc/serdbg/serdbg.h, libmisc/serdbg/serdbgio.c,
// 	libmisc/serdbg/termios_printk.c, libmisc/serdbg/termios_printk.h,
// 	libmisc/shell/shell.c, libmisc/shell/shell.h, libmisc/stackchk/check.c,
// 	libmisc/stackchk/internal.h: Convert to using c99 fixed size types.
//
// Revision 1.5  2003/07/08 08:38:48  ralf
// 2003-07-08	Ralf Corsepius <corsepiu@faw.uni-ulm.de>
//
// 	* capture/capture-cli.c: Add config-header support.
// 	* capture/capture.c: Add config-header support.
// 	* cpuuse/cpuuse.c: Add config-header support.
// 	* devnull/devnull.c: Add config-header support.
// 	* dummy/dummy.c: Add config-header support.
// 	* dumpbuf/dumpbuf.c: Add config-header support.
// 	* monitor/mon-command.c: Add config-header support.
// 	* monitor/mon-config.c: Add config-header support.
// 	* monitor/mon-dname.c: Add config-header support.
// 	* monitor/mon-driver.c: Add config-header support.
// 	* monitor/mon-extension.c: Add config-header support.
// 	* monitor/mon-itask.c: Add config-header support.
// 	* monitor/mon-manager.c: Add config-header support.
// 	* monitor/mon-monitor.c: Add config-header support.
// 	* monitor/mon-mpci.c: Add config-header support.
// 	* monitor/mon-object.c: Add config-header support.
// 	* monitor/mon-prmisc.c: Add config-header support.
// 	* monitor/mon-queue.c: Add config-header support.
// 	* monitor/mon-server.c: Add config-header support.
// 	* monitor/mon-symbols.c: Add config-header support.
// 	* monitor/mon-task.c: Add config-header support.
// 	* mw-fb/mw_fb.c: Add config-header support.
// 	* mw-fb/mw_uid.c: Add config-header support.
// 	* rtmonuse/rtmonuse.c: Add config-header support.
// 	* serdbg/serdbg.c: Add config-header support.
// 	* serdbg/serdbgio.c: Add config-header support.
// 	* serdbg/termios_printk.c: Add config-header support.
// 	* shell/cmds.c: Add config-header support.
// 	* stackchk/check.c: Add config-header support.
// 	* untar/untar.c: Add config-header support.
//
// Revision 1.4  2002/01/04 18:32:48  joel
// 2002-01-04	Ralf Corsepius <corsepiu@faw.uni-ulm.de>
//
// 	* mw-fb/mw_uid.c: Apply rtems_set_errno_and_return_minus_one.
//
// Revision 1.3  2000/11/30 14:36:46  joel
// 2000-11-30	Joel Sherrill <joel@OARcorp.com>
//
// 	* mw-fb/mw_uid.c: Removed unnecessary dependency on <bsp.h>.
//
// Revision 1.2  2000/08/30 17:12:55  joel
// 2000-08-30	Joel Sherrill <joel@OARcorp.com>
//
// 	* Many files: Moved posix/include/rtems/posix/seterr.h to
// 	score/include/rtems/seterr.h so it would be available within
// 	all APIs.
//
// Revision 1.1  2000/08/30 08:21:24  joel
// 2000-08-26  Rosimildo da Silva  <rdasilva@connecttel.com>
//
// 	* Added generic Micro FrameBuffer interface for MicroWindows.
// 	This interface allows MicroWindows to under RTEMS. A sample
// 	driver has been developed for the pc386 BSP. See
// 	pc386/fb_vga.c as a sample.
// 	* Added Uniform Input Device interface for MicroWindows.
// 	See PC386 bsp for sample drivers for mouse and keyboard (console).
// 	* mw-bf: New directory.
// 	* Makefile.am, configure.in, wrapup/Makefile.am: Account for mw-fb.
// 	* mw-fb/Makefile.am: New file.
// 	* mw-fb/mw_fb.c: New file.
// 	* mw-fb/mw_fb.h: New file.
// 	* mw-fb/mw_uid.c: New file.
// 	* mw-fb/mw_uid.h: New file.
//
//
/////////////////////////////////////////////////////////////////////////////
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <rtems.h>

#include <rtems/mw_uid.h>
#include <rtems/seterr.h>

static rtems_id   queue_id = 0;
static int open_count = 0;

/*
#define MW_DEBUG_ON     1
*/

/* open a message queue with the kernel */
int uid_open_queue( const char *q_name, int flags, size_t max_msgs )
{
   static rtems_name queue_name;

   /*
    * For the first device calling this function we would create the queue.
    * It is assumed that this call is done at initialization, and no concerns
    * regarding multi-threading is taken in consideration here.
    */
   if( !open_count )
   {
      rtems_status_code status;
      queue_name = rtems_build_name( q_name[0],
                                     q_name[1],
                                     q_name[2],
                                     q_name[3] );
      status = rtems_message_queue_create( queue_name,
                                           max_msgs,
                                           sizeof( struct MW_UID_MESSAGE ),
                                           RTEMS_FIFO | RTEMS_LOCAL,
                                           &queue_id );
      if( status != RTEMS_SUCCESSFUL )
      {
#ifdef MW_DEBUG_ON
        printk( "UID_Queue: error creating queue: %d\n", status );
#endif
        return -1;
      }
#ifdef MW_DEBUG_ON
      printk( "UID_Queue: id=%X\n", queue_id );
#endif
   }
   open_count++;
   return 0;
}


/* close message queue */
int uid_close_queue( void )
{
  if( open_count == 1 )
  {
     rtems_message_queue_delete( queue_id );
     queue_id = 0;
  }
  open_count--;
  return 0;
}

/* reads for a message from the device */
int uid_read_message( struct MW_UID_MESSAGE *m, unsigned long timeout )
{
  rtems_status_code status;
  uint32_t   size = 0;
  unsigned long micro_secs = timeout*1000;
  int wait = ( timeout != 0 );

  status = rtems_message_queue_receive( queue_id,
                                       (void*)m,
                                       &size,
                                       wait ? RTEMS_WAIT : RTEMS_NO_WAIT,
                                       TOD_MICROSECONDS_TO_TICKS(micro_secs ) );

  if( status == RTEMS_SUCCESSFUL )
  {
     return size;
  }
  else if( ( status == RTEMS_UNSATISFIED ) || ( status == RTEMS_TIMEOUT ) )
  {
     /* this macro returns -1 */
     rtems_set_errno_and_return_minus_one( ETIMEDOUT );
  }
  /* Here we have one error condition */
#ifdef MW_DEBUG_ON
  printk( "UID_Queue: error reading queue: %d\n", status );
#endif
  return -1;
}


/*
 * add a message to the queue of events. This method cna be used to
 * simulate hardware events, and it can be very handy during development
 * a new interface.
 */
int uid_send_message( struct MW_UID_MESSAGE *m )
{
  rtems_status_code status;
  status = rtems_message_queue_send( queue_id, ( void * )m,
                                    sizeof( struct MW_UID_MESSAGE ) );
  return status == RTEMS_SUCCESSFUL ? 0 : -1;
}

/*
 * register the device to insert events to the message
 * queue named as the value passed in q_name
 */
int uid_register_device( int fd, const char *q_name )
{
  return ioctl( fd, MW_UID_REGISTER_DEVICE, q_name );
}

/* tell this device to stop adding events to the queue */
int uid_unregister_device( int fd )
{
  return ioctl( fd, MW_UID_UNREGISTER_DEVICE, NULL );
}

/* set the keyboard */
int uid_set_kbd_mode( int fd, int mode, int *old_mode )
{
   if (ioctl( fd, MV_KDGKBMODE, old_mode) < 0)
   {
      return -1;
   }
   if (ioctl(fd, MV_KDSKBMODE, mode ) < 0 )
   {
      return -1;
   }
   return 0;
}

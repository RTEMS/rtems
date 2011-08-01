
/*
 *
 *  $Id$
 */
#include <rtems/ramdisk.h>
#include <rtems/blkdev.h>
#include <rtems/libio.h>

#include "ramdisk_support.h"
/*
 * Ramdisk information 
 */

dev_t dev = 0;

void init_ramdisk(void)
{

  int rc=0;
  rc =rtems_disk_io_initialize();
  rtems_test_assert( rc == 0 );
  rc =ramdisk_register(RAMDISK_BLOCK_SIZE,RAMDISK_BLOCK_COUNT,\
      false,RAMDISK_PATH,&dev);
  rtems_test_assert( rc == 0 );

}
void del_ramdisk(void )
{
  int rc=0;
  rtems_device_major_number major=0;
  rtems_device_minor_number minor=0;

  rc=rtems_disk_delete (dev);
  rtems_test_assert( rc == 0 );

  rtems_filesystem_split_dev_t(dev,major,minor);

  rc=rtems_io_unregister_driver(major);
  rtems_test_assert( rc == 0 );

  rc=rtems_disk_io_done();
  rtems_test_assert( rc == 0 );
}



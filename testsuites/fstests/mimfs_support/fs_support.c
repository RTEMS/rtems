/*
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/stat.h>
#include <sys/types.h>

#include "pmacros.h"
#include "fs_config.h"

#include <rtems/libio.h>

void test_initialize_filesystem(void)
{
  int rc;

  rc = mkdir( BASE_FOR_TEST, 0777 );
  rtems_test_assert( rc == 0 );

  rc = mount(
    "null",
     BASE_FOR_TEST,
    "imfs",
    RTEMS_FILESYSTEM_READ_ONLY,
    NULL
  );
  rtems_test_assert( rc == 0 );
}

void test_shutdown_filesystem(void)
{
  int rc;

  rc = unmount( BASE_FOR_TEST );
  rtems_test_assert( rc == 0 );

}

#include <rtems.h>
#include <rtems/error.h>
#include <sys/types.h>
#include <rtems/rtems_bsdnet.h>

/*
 * Perform a BOOTP request
 */
void
rtems_bsdnet_do_bootp (void)
{
	rtems_bsdnet_semaphore_obtain ();
	bootpc_init ();
	rtems_bsdnet_semaphore_release ();
}

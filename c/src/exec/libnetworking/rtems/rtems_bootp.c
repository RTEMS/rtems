/*
 *  $Id$
 */

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
	bootpc_init (FALSE);
	rtems_bsdnet_semaphore_release ();
}

/*
 * Perform a BOOTP request and update "standard" files in /etc
 * with the results.
 */
void
rtems_bsdnet_do_bootp_and_rootfs (void)
{
	rtems_bsdnet_semaphore_obtain ();
	bootpc_init (TRUE);
	rtems_bsdnet_semaphore_release ();
}

/*
 * RTEMS multi-tasking support
 */

#include <rpc/rpc.h>
#include <rtems.h>
#include <stdlib.h>

/*
 * RPC variables for single-thread
 */
static struct _rtems_rpc_task_variables rpc_default = {
	-1,		/* svc_maxfd */
};

/*
 * RPC values for initializing a new per-task set of variables
 */
static const struct _rtems_rpc_task_variables rpc_init = {
	-1,		/* svc_maxfd */
};

/*
 * Per-task pointer to RPC data
 */
struct _rtems_rpc_task_variables *rtems_rpc_task_variables = &rpc_default;

/*
 * Set up per-task RPC variables
 */
int rtems_rpc_task_init (void)
{
	rtems_status_code sc;
	struct _rtems_rpc_task_variables *tvp;

	if (rtems_rpc_task_variables == &rpc_default) {
		tvp = malloc (sizeof *tvp);
		if (tvp == NULL)
			return RTEMS_NO_MEMORY;
		/*
		 * FIXME: Should have destructor which cleans up
		 * all RPC stuff:
		 *	- Close all files
		 *	- Go through and free linked list elements
		 *	- Free other allocated memory (e.g. clnt_perror_buf)
		 */
		sc = rtems_task_variable_add (RTEMS_SELF, &rtems_rpc_task_variables, NULL);
		if (sc != RTEMS_SUCCESSFUL) {
			free (tvp);
			return sc;
		}
		*tvp = rpc_init;
		rtems_rpc_task_variables = tvp;
	}
	return RTEMS_SUCCESSFUL;
}

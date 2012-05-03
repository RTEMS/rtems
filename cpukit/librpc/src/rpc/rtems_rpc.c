/*
 * RTEMS multi-tasking support
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rpc/rpc.h>
#include <rtems.h>
#include <stdlib.h>

/*
 * RPC variables for single-thread
 */
static struct _rtems_rpc_task_variables rpc_default = {
	-1,		/* svc_maxfd */
	{{0}},		/* svc_svc_fdset */
	NULL,		/* svc_xports */
	0,		/* svc_xportssize */
	0,		/* svc__svc_fdsetsize */		
	0,		/* svc__svc_fdset */
	NULL,		/* svc_svc_head */
	0,		/* clnt_perror_buf */
	0,		/* clnt_raw_private */
	0,		/* call_rpc_private */
	0,		/* svc_raw_private */

	0,		/* svc_simple_proglst */
	0,		/* svc_simple_pl */
	0,		/* svc_simple_transp */

	0,		/* rpcdname_default_domain */
	0		/* svc_auths_Auths */
};

/*
 * RPC values for initializing a new per-task set of variables
 */
static const struct _rtems_rpc_task_variables rpc_init = {
	-1,		/* svc_maxfd */
	{{0}},		/* svc_svc_fdset */
	NULL,		/* svc_xports */
	0,		/* svc_xportssize */
	0,		/* svc__svc_fdsetsize */		
	0,		/* svc__svc_fdset */
	NULL,		/* svc_svc_head */
	0,		/* clnt_perror_buf */
	0,		/* clnt_raw_private */
	0,		/* call_rpc_private */
	0,		/* svc_raw_private */

	0,		/* svc_simple_proglst */
	0,		/* svc_simple_pl */
	0,		/* svc_simple_transp */

	0,		/* rpcdname_default_domain */
	0		/* svc_auths_Auths */
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
		sc = rtems_task_variable_add (
			RTEMS_SELF, (void *)&rtems_rpc_task_variables, NULL);
		if (sc != RTEMS_SUCCESSFUL) {
			free (tvp);
			return sc;
		}
		*tvp = rpc_init;
		rtems_rpc_task_variables = tvp;
	}
	return RTEMS_SUCCESSFUL;
}

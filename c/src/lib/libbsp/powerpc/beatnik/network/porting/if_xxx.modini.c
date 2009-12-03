#include <rtems.h>
#include <porting/rtemscompat.h>

/* CEXP module initialization/finalization */

/* Copyright: Till Straumann <strauman@slac.stanford.edu>, 2005;
 * License:   see LICENSE file.
 */

void
_cexpModuleInitialize(void *unused)
{
extern void NET_EMBEMB(rtems_,NETDRIVER_PREFIX,_bringup)(char *);
	METHODSPTR = &METHODS;
/*
#ifdef DEBUG
	NET_EMBEMB(rtems_,NETDRIVER_PREFIX,_bringup)("192.168.2.13/255.255.255.0");
#endif
*/
}

int
_cexpModuleFinalize(void *unused)
{
#ifdef DEBUG
extern int NET_EMBEMB(rtems_,NETDRIVER_PREFIX,_bringdown)();
	if (NET_EMBEMB(rtems_,NETDRIVER_PREFIX,_bringdown)())
		return -1;
	METHODSPTR = 0;
	return 0;
#else
	return -1;
#endif
}

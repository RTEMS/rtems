/*
 * RTEMS versions of hostname functions
 * FIXME: Not thread-safe
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <errno.h>
#include <rtems/rtems_bsdnet.h>
#include <sys/param.h>
#include <sys/malloc.h>
#include <sys/kernel.h>

static char *rtems_hostname;

int
gethostname (char *name, size_t namelen)
{
	char *cp = rtems_hostname;

	if (cp == NULL)
		cp = "";
	strncpy (name, cp, namelen);
	return 0;
}

int
sethostname (char *name, size_t namelen)
{
	char *old, *new;

	if (namelen >= MAXHOSTNAMELEN) {
		errno = EINVAL;
		return -1;
	}
	new = malloc (namelen + 1, M_HTABLE, M_NOWAIT);
	if (new == NULL) {
		errno = ENOMEM;
		return -1;
	}
	strncpy (new, name, namelen);
	new[namelen] = '\0';
	old = rtems_hostname;
	rtems_hostname = new;
	if (old)
		free (old, M_HTABLE);
	return 0;
}

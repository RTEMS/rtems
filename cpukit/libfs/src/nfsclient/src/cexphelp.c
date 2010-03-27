#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <librtemsNfs.h>
#include <cexpHelp.h>
CEXP_HELP_TAB_BEGIN(rtemsNfs)
	HELP(
"Mount a remote filesystem (NFS). The mount point (must not be a NFS dir)\n"
"is created on the fly if not existing already.\n"
"uid/gid to use may be specified:\n"
"   hostspec: [uid.gid@]hostname_or_ipaddr\n"
	, int, nfsMount, (char *hostspec, char *exportdir, char *mntpoint)
		),
	HELP(
"Print all currently mounted NFS directories to open file handle.\n"
"Pass f = 0 to print to stdout\n"
	, int, nfsMountsShow, (FILE *f)
		),
CEXP_HELP_TAB_END

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "librtemsNfs.h"

/* CEXP dynamic loader support */

void
_cexpModuleInitialize(void *mod)
{
#if defined(DEBUG)
	/* print load address (in case we crash while initializing) */
unsigned lr;
	__asm__ __volatile__(
			"	    bl     thisis_loaded_at \n"
			"thisis_loaded_at:              \n"
			"       mflr   %0               \n"
			: "=r"(lr) ::"lr");
	printf("thisis_loaded_at: 0x%08x\n",lr);
#endif
	nfsInit(0,0);
}

int
_cexpModuleFinalize(void *mod)
{
	return nfsCleanup();
}



##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I ../../../../../aclocal

## Check configure.in for conditions to set the conditionals below
if shared
SHARED_LIB = shared
endif

## Use new or old exception processing based on the CPU
if new_exception_processing
EXCEPTION_SUBDIR = new_exception_processing
endif

if old_exception_processing
EXCEPTION_SUBDIR = old_exception_processing
endif

if mpc505
CPU_SUBDIR = mpc505
endif
if mpc6xx
CPU_SUBDIR = mpc6xx
endif
if mpc8xx
CPU_SUBDIR = mpc8xx
endif
if mpc8260
CPU_SUBDIR = mpc8260
endif
if ppc403
CPU_SUBDIR = ppc403
endif

SUBDIRS = $(SHARED_LIB) $(EXCEPTION_SUBDIR) $(CPU_SUBDIR) wrapup

include $(top_srcdir)/../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../automake/local.am

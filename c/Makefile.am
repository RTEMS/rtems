ACLOCAL_AMFLAGS = -I ../aclocal

## NOTE: This Makefile.am is rather fragile to the value of RTEMS_BSP
## Esp. make clean and make distclean have unwanted side-effects
## if RTEMS_BSP is not properly set up.

RTEMS_BSP = $(RTEMS_BSP_LIST)
## The '.' in SUBDIRS ensures that local make-targets (xxx-am) will be
## triggered before $(RTEMS_BSP_LIST) subdirectories are made.
SUBDIRS = . $(RTEMS_BSP)
DIST_SUBDIRS = $(RTEMS_BSP)

## Let all RTEMS' make targets depend on ${RTEMS_BSP}
all-local: ${RTEMS_BSP}
preinstall-am: ${RTEMS_BSP}

## Pull in extra files intro the distribution
EXTRA_DIST = ACKNOWLEDGEMENTS
EXTRA_DIST += TOOL_TARGETS

include $(top_srcdir)/../automake/host.am
include $(top_srcdir)/../automake/subdirs.am

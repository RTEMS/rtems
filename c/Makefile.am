##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../aclocal

## NOTE: This Makefile.am is rather fragile to the value of RTEMS_BSP
## Esp. make clean and make distclean have unwanted side-effects
## if RTEMS_BSP is not properly set up.

RTEMS_BSP = $(RTEMS_BSP_LIST)
## The '.' in SUBDIRS ensures that local make-targets (xxx-am) will be
## triggered before $(RTEMS_BSP_LIST) subdirectories are made.
SUBDIRS = . $(RTEMS_BSP)

## Use @RTEMS_BSP_LIST@ here, using $(RTEMS_BSP_LIST) would trigger
## this rule for invalid BSPs
@RTEMS_BSP_LIST@: src/configure
	@set fnord $(MAKEFLAGS); amf=$$2; \
	test -n "${RTEMS_BSP}" && for i in ${RTEMS_BSP}; do \
	  echo "Configuring RTEMS_BSP=$$i"; \
	  $(mkinstalldirs) $$i; \
	  ( cd $$i && \
	    CONFIG_SHELL=$(SHELL) RTEMS_BSP=$$i $(rtems_bsp_configure) \
	    && $(MAKE) $(AM_MAKEFLAGS) preinstall ) \
	  || case "$$amf" in *=*) exit 1;; *k*) fail=yes;; *) exit 1;; esac; \
	done

## Let all RTEMS' make targets depend on ${RTEMS_BSP}
all-local: ${RTEMS_BSP}
preinstall-am: ${RTEMS_BSP}
depend-am: ${RTEMS_BSP}

## Pull in extra files intro the distribution
EXTRA_DIST = ACKNOWLEDGEMENTS README.DOS REQUIRES PROBLEMS TESTED README \
    UPDATE_HELP

DIST_SUBDIRS = ${RTEMS_BSP}
## -------------------------------------------------------
include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/host.am

## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

if HAS_NETWORKING
# We only build the Network library if HAS_NETWORKING was defined
NETWORK = network
endif

# wrapup is the one that actually builds and installs the library
#  from the individual .rel files built in other directories
SUBDIRS = . include tools start startup clock console timer $(NETWORK) wrapup

include $(top_srcdir)/../../bsp.am

EXTRA_DIST = bsp_specs

include $(top_srcdir)/../../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../../automake/local.am

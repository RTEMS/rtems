## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

if HAS_NETWORKING
# We only build the Network library if HAS_NETWORKING was defined
# dec21140 is supported via libchip
NETWORK = ne2000 wd8003 3c509
endif

# wrapup is the one that actually builds and installs the library
#  from the individual .rel files built in other directories
SUBDIRS = . include tools start startup clock console timer $(NETWORK) wrapup

include $(top_srcdir)/../../bsp.am

EXTRA_DIST = HOWTO README.dec21140 STATUS bsp_specs times_i486dx times_p5

include $(top_srcdir)/../../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../../automake/local.am

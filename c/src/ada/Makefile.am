#
# $Id$
#

if RTEMS_ADA
include_adadir = $(includedir)/adainclude
include_ada_HEADERS = rtems.adb rtems.ads
include_ada_HEADERS += rtems-multiprocessing.adb rtems-multiprocessing.ads
endif

all-local: $(PREINSTALL_FILES)

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am

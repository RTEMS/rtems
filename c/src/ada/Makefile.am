#
# $Id$
#

if RTEMS_ADA
include_adadir = $(includedir)/adainclude
include_ada_HEADERS = rtems.adb rtems.ads
include_ada_HEADERS += rtems-multiprocessing.adb rtems-multiprocessing.ads
endif

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

if RTEMS_ADA
$(PROJECT_INCLUDE)/adainclude/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/adainclude
	@: > $(PROJECT_INCLUDE)/adainclude/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/adainclude/$(dirstamp)

$(PROJECT_INCLUDE)/adainclude/rtems.adb: rtems.adb $(PROJECT_INCLUDE)/adainclude/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/adainclude/rtems.adb
PREINSTALL_FILES += $(PROJECT_INCLUDE)/adainclude/rtems.adb

$(PROJECT_INCLUDE)/adainclude/rtems.ads: rtems.ads $(PROJECT_INCLUDE)/adainclude/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/adainclude/rtems.ads
PREINSTALL_FILES += $(PROJECT_INCLUDE)/adainclude/rtems.ads

$(PROJECT_INCLUDE)/adainclude/rtems-multiprocessing.adb: rtems-multiprocessing.adb $(PROJECT_INCLUDE)/adainclude/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/adainclude/rtems-multiprocessing.adb
PREINSTALL_FILES += $(PROJECT_INCLUDE)/adainclude/rtems-multiprocessing.adb

$(PROJECT_INCLUDE)/adainclude/rtems-multiprocessing.ads: rtems-multiprocessing.ads $(PROJECT_INCLUDE)/adainclude/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/adainclude/rtems-multiprocessing.ads
PREINSTALL_FILES += $(PROJECT_INCLUDE)/adainclude/rtems-multiprocessing.ads
endif

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/automake/local.am

#
# $Id$
#

adaincludedir = $(includedir)/adainclude

if RTEMS_ADA
adainclude_HEADERS = rtems.adb rtems.ads 
adainclude_HEADERS += rtems-multiprocessing.adb rtems-multiprocessing.ads
endif

all-local: $(TMPINSTALL_FILES)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/adainclude/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/adainclude
	@: > $(PROJECT_INCLUDE)/adainclude/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/adainclude/$(dirstamp)

if RTEMS_ADA
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

include $(top_srcdir)/automake/local.am

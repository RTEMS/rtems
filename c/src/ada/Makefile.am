#
# $Id$
#

$(PROJECT_INCLUDE)/adainclude:
	$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/adainclude/%: %
	$(INSTALL_DATA) $< $@

adaincludedir = $(includedir)/adainclude

if RTEMS_ADA
adainclude_HEADERS = rtems.adb rtems.ads 
adainclude_HEADERS += rtems-multiprocessing.adb rtems-multiprocessing.ads

TMPINSTALL_FILES = \
  $(PROJECT_INCLUDE)/adainclude \
  $(adainclude_HEADERS:%=$(PROJECT_INCLUDE)/adainclude/%) \
  $(nodist_adalib_HEADERS:%=$(PROJECT_INCLUDE)/adainclude/%)
endif

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/automake/local.am

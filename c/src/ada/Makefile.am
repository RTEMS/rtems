#
# $Id$
#

ACLOCAL_AMFLAGS = -I ../aclocal

$(PROJECT_INCLUDE)/adainclude:
	$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/adainclude/%: %
	$(INSTALL_DATA) $< $@

adaincludedir = $(includedir)/adainclude

adainclude_HEADERS = rtems.adb rtems.ads 
adainclude_HEADERS += rtems-multiprocessing.adb rtems-multiprocessing.ads

TMPINSTALL_FILES = \
  $(PROJECT_INCLUDE)/adainclude \
  $(adainclude_HEADERS:%=$(PROJECT_INCLUDE)/adainclude/%) \
  $(nodist_adalib_HEADERS:%=$(PROJECT_INCLUDE)/adainclude/%)

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../automake/local.am

##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

project_bspdir = $(PROJECT_ROOT)/@RTEMS_BSP@

noinst_HEADERS = include/tmacros.h include/buffer_test_io.h

TMPINSTALLFILES = $(project_bspdir)/lib/include \
    $(project_bspdir)/lib/include/tmacros.h \
    $(project_bspdir)/lib/include/buffer_test_io.h

$(project_bspdir)/lib/include:
	$(mkinstalldirs) $@
$(project_bspdir)/lib/include/%.h: include/%.h
	$(INSTALL_DATA) $< $@

CLEANFILES = $(project_bspdir)/lib/include/tmacros.h \
    $(project_bspdir)/lib/include/buffer_test_io.h

all-local: $(TMPINSTALLFILES)

include $(top_srcdir)/../../../../automake/local.am

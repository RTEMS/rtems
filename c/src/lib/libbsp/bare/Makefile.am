## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

include $(top_srcdir)/../bsp.am

SUBDIRS = . include

$(PROJECT_RELEASE)/bsp_specs: bsp_specs
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += \
$(PROJECT_RELEASE)/bsp_specs

all-local : $(PREINSTALL_FILES)

EXTRA_DIST = \
build-tools bsp_specs do-bare-bsp-build

include $(top_srcdir)/../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../automake/local.am

##
## $Id$
##

PGM = $(ARCH)/network.rel

C_FILES = network.c 
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

include_HEADERS = 

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#
AM_CPPFLAGS += -D__INSIDE_RTEMS_BSD_TCPIP_STACK__

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

$(PGM): $(OBJS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

if HAS_NETWORKING
all-local: $(ARCH) $(OBJS) $(PGM)
endif

.PRECIOUS: $(PGM)

EXTRA_DIST = network.c 

include $(top_srcdir)/../../../../../../automake/local.am

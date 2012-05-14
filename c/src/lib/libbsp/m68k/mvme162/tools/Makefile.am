ACLOCAL_AMFLAGS = -I ../../../../../aclocal

sload_SOURCES = sload.c

if SLOAD_BUILDABLE
noinst_PROGRAMS = sload

## HACK: install to build-tree
install-exec-local: $(PROGRAMS)
	$(MKDIR_P) $(PROJECT_ROOT)/@RTEMS_BSP@/bin
	$(INSTALL_PROGRAM) $(PROGRAMS) $(PROJECT_ROOT)/@RTEMS_BSP@/bin
endif

include $(top_srcdir)/../../../../../automake/host.am

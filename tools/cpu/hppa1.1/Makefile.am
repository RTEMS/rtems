##
##  $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

noinst_PROGRAMS = genoffsets

genoffsets_SOURCES = genoffsets.c

## We use files that have not been installed yet.

CPU_DIR = ../../cpu/$(RTEMS_CPU)

AM_CPPFLAGS = -I$(PROJECT_INCLUDE) -I$(CPU_DIR)

PREINSTALL_FILES = genoffsets

include $(top_srcdir)/../../../automake/host.am

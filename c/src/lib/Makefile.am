_SUBDIRS = libcpu libbsp

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am

#
# The libsp cannot be built until libcpu is built.
#
libbsp: libcpu

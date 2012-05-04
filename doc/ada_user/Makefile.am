#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.

PROJECT = ada_user

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

COMMON_FILES += \
    $(top_builddir)/user/barrier.texi $(top_builddir)/user/bsp.texi \
    $(top_builddir)/user/clock.texi $(top_builddir)/user/concepts.texi \
    $(top_builddir)/user/datatypes.texi $(top_builddir)/user/conf.texi \
    $(top_builddir)/user/dirstat.texi $(top_builddir)/user/dpmem.texi \
    $(top_builddir)/user/event.texi $(top_builddir)/user/fatal.texi \
    $(top_builddir)/user/glossary.texi $(top_builddir)/user/init.texi \
    $(top_builddir)/user/intr.texi $(top_builddir)/user/io.texi \
    $(top_builddir)/user/mp.texi $(top_builddir)/user/msg.texi \
    $(top_builddir)/user/overview.texi $(top_builddir)/user/part.texi \
    $(top_builddir)/user/preface.texi $(top_builddir)/user/region.texi \
    $(top_builddir)/user/rtmon.texi $(top_builddir)/user/schedule.texi \
    $(top_builddir)/user/sem.texi $(top_builddir)/user/signal.texi \
    $(top_builddir)/user/task.texi $(top_builddir)/user/timer.texi \
    $(top_builddir)/user/userext.texi $(top_builddir)/user/stackchk.texi \
    $(top_builddir)/user/cpuuse.texi $(top_srcdir)/common/cpright.texi \
    $(top_builddir)/user/object.texi $(top_builddir)/user/cbs.texi

FILES = example.texi

ObjectId-16Bits.eps: $(top_srcdir)/user/ObjectId-16Bits.eps
	$(LN_S) $<
ObjectId-32Bits.eps: $(top_srcdir)/user/ObjectId-32Bits.eps
	$(LN_S) $<
rtemspie.eps: $(top_srcdir)/user/rtemspie.eps
	$(LN_S) $<
semaphore_attributes.eps: $(top_srcdir)/user/semaphore_attributes.eps
	$(LN_S) $<
states.eps: $(top_srcdir)/user/states.eps
	$(LN_S) $<
CLEANFILES += ObjectId-16Bits.eps ObjectId-32Bits.eps rtemspie.eps semaphore_attributes.eps states.eps

ObjectId-16Bits.png: $(top_srcdir)/user/ObjectId-16Bits.png
	$(LN_S) $<
ObjectId-32Bits.png: $(top_srcdir)/user/ObjectId-32Bits.png
	$(LN_S) $<
rtemsarc.png: $(top_srcdir)/user/rtemsarc.png
	$(LN_S) $<
rtemspie.png: $(top_srcdir)/user/rtemspie.png
	$(LN_S) $<
semaphore_attributes.png: $(top_srcdir)/user/semaphore_attributes.png
	$(LN_S) $<
states.png: $(top_srcdir)/user/states.png
	$(LN_S) $<
CLEANFILES += ObjectId-16Bits.png ObjectId-32Bits.png rtemsarc.png rtemspie.png semaphore_attributes.png states.png

info_TEXINFOS = ada_user.texi
ada_user_TEXINFOS = $(FILES) $(COMMON_FILES)

if USE_HTML
html_project_DATA += rtemsarc.png rtemspie.png states.png \
    ObjectId-16Bits.png ObjectId-32Bits.png semaphore_attributes.png
endif

$(PROJECT).dvi: rtemspie.eps states.eps ObjectId-16Bits.eps \
    ObjectId-32Bits.eps semaphore_attributes.eps
PDF_IMAGES = rtemspie.pdf states.pdf ObjectId-16Bits.pdf \
    ObjectId-32Bits.pdf semaphore_attributes.pdf

CLEANFILES += ada_user.info ada_user.info-? ada_user.info-??

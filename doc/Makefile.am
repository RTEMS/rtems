#
#  $Id$
#

ACLOCAL_AMFLAGS = -I ../aclocal

# NOTE: The order of the directories is essential.
#       + tools, common and images are shared across many documents

SUBDIRS = tools started user bsp_howto porting develenv posix_users \
    posix1003.1 filesystem itron3.0 networking ada_user started_ada \
    new_chapters relnotes cpu_supplement shell

if USE_HTML
html_DATA = index.html HELP.html
endif

EXTRA_DIST = HELP.html rtems_header.html.in rtems_footer.html.in

EXTRA_DIST += common/cpright.texi common/setup.texi \
    common/treedef.tex common/rtems.texi.in

html_imagesdir = $(htmldir)/images

HTML_IMAGES = images/dir-arrow.gif images/dvi.gif images/missing-arrow.gif \
    images/next-arrow.gif images/oaronly.jpg images/pdf.gif images/pdf1.gif \
    images/rtems_logo.jpg images/prev-arrow.gif images/ps.gif \
    images/up-arrow.gif

if USE_HTML
html_images_DATA = $(HTML_IMAGES)
endif

EXTRA_DIST += $(HTML_IMAGES)

CLEANFILES = common/rtems.texi
DISTCLEANFILES = common/rtems.sed

# HACK: This should not be here, but it makes "make distcheck" work.
EXTRA_DIST += ../scripts/setup.def ../COPYING

ACLOCAL_AMFLAGS = -I ../aclocal

SUBDIRS =
if SMPTESTS
SUBDIRS += smp01
SUBDIRS += smp02
SUBDIRS += smp03
SUBDIRS += smp05
SUBDIRS += smp07
SUBDIRS += smp08
SUBDIRS += smp09
SUBDIRS += smpaffinity01
SUBDIRS += smpatomic01
SUBDIRS += smpcache01
SUBDIRS += smpfatal01
SUBDIRS += smpfatal02
SUBDIRS += smpfatal03
SUBDIRS += smpfatal04
SUBDIRS += smpfatal05
SUBDIRS += smpfatal08
SUBDIRS += smpipi01
SUBDIRS += smpload01
SUBDIRS += smplock01
SUBDIRS += smpmigration01
SUBDIRS += smpmigration02
SUBDIRS += smpmrsp01
SUBDIRS += smpschedaffinity01
SUBDIRS += smpschedaffinity02
SUBDIRS += smpschedaffinity03
SUBDIRS += smpschedaffinity04
SUBDIRS += smpschedaffinity05
SUBDIRS += smpscheduler01
SUBDIRS += smpscheduler02
SUBDIRS += smpscheduler03
SUBDIRS += smpschedsem01
SUBDIRS += smpsignal01
SUBDIRS += smpswitchextension01
SUBDIRS += smpthreadlife01
SUBDIRS += smpunsupported01
SUBDIRS += smpwakeafter01
if HAS_POSIX
SUBDIRS += smppsxaffinity01
SUBDIRS += smppsxaffinity02
SUBDIRS += smppsxsignal01
endif
endif

include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am

#
#  COPYRIGHT (c) 1988-1999.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign

PROJECT=porting

include $(top_srcdir)/project.am

COMMON_FILES=$(top_srcdir)/common/cpright.texi

FILES=preface.texi developtools.texi sourcecode.texi cpumodels.texi \
  cpuinit.texi interrupts.texi taskcontext.texi idlethread.texi \
  prioritybitmap.texi codetuning.texi miscellaneous.texi $(COMMON_FILES)

GENERATED_FILES=developtools.texi sourcecode.texi cpumodels.texi \
  cpuinit.texi interrupts.texi taskcontext.texi idlethread.texi \
  prioritybitmap.texi codetuning.texi miscellaneous.texi

info_TEXINFOS = porting.texi
porting_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

developtools.texi: developtools.t
	$(BMENU) -p "Preface" \
	    -u "Top" \
	    -n "Source Code Organization" $<

sourcecode.texi: sourcecode.t
	$(BMENU) -p "Development Tools" \
	    -u "Top" \
	    -n "CPU Model Variations" $<

cpumodels.texi: cpumodels.t
	$(BMENU) -p "Source Code Organization Introduction" \
	    -u "Top" \
	    -n "CPU Initialization" $<

cpuinit.texi: cpuinit.t
	$(BMENU) -p "CPU Model Variations Coding Issues" \
	    -u "Top" \
	    -n "Interrupts" $<

interrupts.texi: interrupts.t
	$(BMENU) -p "CPU Initialization Initializing the CPU" \
	    -u "Top" \
	    -n "Task Context Management" $<

taskcontext.texi: taskcontext.t
	$(BMENU) -p "Interrupts Pointer to _Thread_Dispatch Routine" \
	    -u "Top" \
	    -n "IDLE Thread" $<

idlethread.texi: idlethread.t
	$(BMENU) -p "Task Context Management Restoring a Floating Point Context" \
	    -u "Top" \
	    -n "Priority Bitmap Manipulation" $<

prioritybitmap.texi: prioritybitmap.t
	$(BMENU) -p "IDLE Thread Idle Thread Body" \
	    -u "Top" \
	    -n "Code Tuning Parameters" $<

codetuning.texi: codetuning.t
	$(BMENU) -p "Priority Bitmap Manipulation Bit Scan Support" \
	    -u "Top" \
	    -n "Miscellaneous" $<

miscellaneous.texi: miscellaneous.t
	$(BMENU) -p "Code Tuning Parameters Partition Element Alignment" \
	    -u "Top" \
	    -n "Command and Variable Index" $<

EXTRA_DIST = *.t

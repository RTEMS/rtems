#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.

PROJECT = porting

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

FILES = preface.texi developtools.texi sourcecode.texi cpumodels.texi \
    cpuinit.texi interrupts.texi taskcontext.texi idlethread.texi \
    prioritybitmap.texi codetuning.texi miscellaneous.texi

GENERATED_FILES = developtools.texi sourcecode.texi cpumodels.texi \
    cpuinit.texi interrupts.texi taskcontext.texi idlethread.texi \
    prioritybitmap.texi codetuning.texi miscellaneous.texi

COMMON_FILES += $(top_srcdir)/common/cpright.texi

info_TEXINFOS = porting.texi
porting_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

developtools.texi: developtools.t
	$(BMENU2) -p "Preface" \
	    -u "Top" \
	    -n "Source Code Organization" < $< > $@

sourcecode.texi: sourcecode.t
	$(BMENU2) -p "Development Tools" \
	    -u "Top" \
	    -n "CPU Model Variations" < $< > $@

cpumodels.texi: cpumodels.t
	$(BMENU2) -p "Source Code Organization Introduction" \
	    -u "Top" \
	    -n "CPU Initialization" < $< > $@

cpuinit.texi: cpuinit.t
	$(BMENU2) -p "CPU Model Variations Coding Issues" \
	    -u "Top" \
	    -n "Interrupts" < $< > $@

interrupts.texi: interrupts.t
	$(BMENU2) -p "CPU Initialization Initializing the CPU" \
	    -u "Top" \
	    -n "Task Context Management" < $< > $@

taskcontext.texi: taskcontext.t
	$(BMENU2) -p "Interrupts Pointer to _Thread_Dispatch Routine" \
	    -u "Top" \
	    -n "IDLE Thread" < $< > $@

idlethread.texi: idlethread.t
	$(BMENU2) -p "Task Context Management Restoring a Floating Point Context" \
	    -u "Top" \
	    -n "Priority Bitmap Manipulation" < $< > $@

prioritybitmap.texi: prioritybitmap.t
	$(BMENU2) -p "IDLE Thread Idle Thread Body" \
	    -u "Top" \
	    -n "Code Tuning Parameters" < $< > $@

codetuning.texi: codetuning.t
	$(BMENU2) -p "Priority Bitmap Manipulation Bit Scan Support" \
	    -u "Top" \
	    -n "Miscellaneous" < $< > $@

miscellaneous.texi: miscellaneous.t
	$(BMENU2) -p "Code Tuning Parameters Partition Element Alignment" \
	    -u "Top" \
	    -n "Command and Variable Index" < $< > $@

EXTRA_DIST = codetuning.t cpuinit.t cpumodels.t developtools.t idlethread.t \
    interrupts.t miscellaneous.t prioritybitmap.t sourcecode.t \
    taskcontext.t

CLEANFILES += porting.info porting.info-?

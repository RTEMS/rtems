#
#  COPYRIGHT (c) 1988-1998.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.
#
#  $Id$
#

include ../Make.config

PROJECT=posix1003_1

all:  

COMMON_FILES=../common/cpright.texi ../common/setup.texi

GENERATED_FILES=ch01.texi ch02.texi ch03.texi ch04.texi ch05.texi \
                ch06.texi ch07.texi ch08.texi ch08.texi ch10.texi \
                ch11.texi ch12.texi ch13.texi ch14.texi ch15.texi \
                ch16.texi ch17.texi ch18.texi 

FILES= $(PROJECT).texi \
  preface.texi $(GENERATED_FILES)

all:

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: $(PROJECT)
	#cp $(wildcard $(PROJECT) $(PROJECT)-*) $(INFO_INSTALL)
	cp $(PROJECT) $(INFO_INSTALL)

$(P{ROJECT): $(FILES)
	$(MAKEINFO) $(PROJECT).texi

vinfo: info
	$(INFO) -f $(PROJECT)

dvi: $(PROJECT).dvi
ps: $(PROJECT).ps
	
$(PROJECT).ps: $(PROJECT).dvi
	dvips -o $(PROJECT).ps $(PROJECT).dvi
	cp $(PROJECT).ps $(PS_INSTALL)

dv: dvi
	$(XDVI) $(PROJECT).dvi

view: ps
	$(GHOSTVIEW) $(PROJECT).ps

$(PROJECT).dvi: $(FILES)
	$(TEXI2DVI) $(PROJECT).texi

ch01.t.texi: ch01.t
	$(BMENU) -p "Preface" \
	    -u "Top" \
	    -n "Terminology and General Requirements" $*

ch02.t.texi: ch02.t
	$(BMENU) -p "Conformance" \
	    -u "Top" \
	    -n "Process Primitives" $*

ch03.t.texi: ch03.t
	$(BMENU) -p "Symbolic Constants" \
	    -u "Top" \
	    -n "Process Environment" $*

ch04.t.texi: ch04.t
	$(BMENU) -p "Delay Process Execution" \
	    -u "Top" \
	    -n "Files and Directories" $*

ch05.t.texi: ch05.t
	$(BMENU) -p "Get Configurable System Variables" \
	    -u "Top" \
	    -n "Input and Output Primitives" $*

ch06.t.texi: ch06.t
	$(BMENU) -p "Get Configurable Pathname Variables" \
	    -u "Top" \
	    -n "Device- and Class-Specific Functions" $*

ch07.t.texi: ch07.t
	$(BMENU) -p " Asynchronous File Synchronization" \
	    -u "Top" \
	    -n "Language-Specific Services for the C Programming Language" $*

ch08.t.texi: ch08.t
	$(BMENU) -p "Set Foreground Process Group ID" \
	    -u "Top" \
	    -n "System Databases" $*

ch09.t.texi: ch09.t
	$(BMENU) -p "Pseudo-Random Sequence Generation Functions" \
	    -u "Top" \
	    -n "Data Interchange Format" $*

ch10.t.texi: ch10.t
	$(BMENU) -p "User Database Access" \
	    -u "Top" \
	    -n "Synchronization" $*

ch11.t.texi: ch11.t
	$(BMENU) -p "Multiple Volumes" \
	    -u "Top" \
	    -n "Memory Management" $*

ch12.t.texi: ch12.t
	$(BMENU) -p "Waiting on a Condition" \
	    -u "Top" \
	    -n "Execution Scheduling" $*

ch13.t.texi: ch13.t
	$(BMENU) -p "Remove a Shared Memory Object" \
	    -u "Top" \
	    -n "Clocks and Timers" $*

ch14.t.texi: ch14.t
	$(BMENU) -p "Change the Priority Ceiling of a Mutex" \
	    -u "Top" \
	    -n "Message Passing" $*

ch15.t.texi: ch15.t
	$(BMENU) -p "High Resolution Sleep" \
	    -u "Top" \
	    -n "Thread Management" $*

ch16.t.texi: ch16.t
	$(BMENU) -p "Get Message Queue Attributes" \
	    -u "Top" \
	    -n "Thread-Specific Data" $*

ch17.t.texi: ch17.t
	$(BMENU) -p "Dynamic Package Initialization" \
	    -u "Top" \
	    -n "Thread Cancellation" $*

ch18.t.texi: ch18.t
	$(BMENU) -p "Thread-Specific Data Key Deletion" \
	    -u "Top" \
	    -n "Command and Variable Index" $*


html: $(FILES)
	-mkdir $(WWW_INSTALL)/$(PROJECT)
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-* $(GENERATED_FILES)
	rm -f *.fixed _*


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
                ch06.texi ch07.texi ch08.texi ch09.texi ch10.texi \
                ch11.texi ch12.texi ch13.texi ch14.texi ch15.texi \
                ch16.texi ch17.texi ch18.texi 

FILES= $(PROJECT).texi \
  preface.texi $(GENERATED_FILES)

all:

INFOFILES=$(wildcard $(PROJECT) $(PROJECT)-*)

info: $(PROJECT)
	#cp $(wildcard $(PROJECT) $(PROJECT)-*) $(INFO_INSTALL)
	cp $(PROJECT) $(INFO_INSTALL)

$(PROJECT): $(FILES)
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

ch01.texi: ch01.t
	$(BMENU) -c -p "Preface" \
	    -u "Top" \
	    -n "Terminology and General Requirements" ${*}.t

ch02.texi: ch02.t
	$(BMENU) -c -p "Conformance" \
	    -u "Top" \
	    -n "Process Primitives" ${*}.t

ch03.texi: ch03.t
	$(BMENU) -c -p "Symbolic Constants" \
	    -u "Top" \
	    -n "Process Environment" ${*}.t

ch04.texi: ch04.t
	$(BMENU) -c -p "Delay Process Execution" \
	    -u "Top" \
	    -n "Files and Directories" ${*}.t

ch05.texi: ch05.t
	$(BMENU) -c -p "Get Configurable System Variables" \
	    -u "Top" \
	    -n "Input and Output Primitives" ${*}.t

ch06.texi: ch06.t
	$(BMENU) -c -p "Get Configurable Pathname Variables" \
	    -u "Top" \
	    -n "Device- and Class-Specific Functions" ${*}.t

ch07.texi: ch07.t
	$(BMENU) -c -p " Asynchronous File Synchronization" \
	    -u "Top" \
	    -n "Language-Specific Services for the C Programming Language" ${*}.t

ch08.texi: ch08.t
	$(BMENU) -c -p "Set Foreground Process Group ID" \
	    -u "Top" \
	    -n "System Databases" ${*}.t

ch09.texi: ch09.t
	$(BMENU) -c -p "Pseudo-Random Sequence Generation Functions" \
	    -u "Top" \
	    -n "Data Interchange Format" ${*}.t

ch10.texi: ch10.t
	$(BMENU) -c -p "User Database Access" \
	    -u "Top" \
	    -n "Synchronization" ${*}.t

ch11.texi: ch11.t
	$(BMENU) -c -p "Multiple Volumes" \
	    -u "Top" \
	    -n "Memory Management" ${*}.t

ch12.texi: ch12.t
	$(BMENU) -c -p "Waiting on a Condition" \
	    -u "Top" \
	    -n "Execution Scheduling" ${*}.t

ch13.texi: ch13.t
	$(BMENU) -c -p "Remove a Shared Memory Object" \
	    -u "Top" \
	    -n "Clocks and Timers" ${*}.t

ch14.texi: ch14.t
	$(BMENU) -c -p "Change the Priority Ceiling of a Mutex" \
	    -u "Top" \
	    -n "Message Passing" ${*}.t

ch15.texi: ch15.t
	$(BMENU) -c -p "High Resolution Sleep" \
	    -u "Top" \
	    -n "Thread Management" ${*}.t

ch16.texi: ch16.t
	$(BMENU) -c -p "Get Message Queue Attributes" \
	    -u "Top" \
	    -n "Thread-Specific Data" ${*}.t

ch17.texi: ch17.t
	$(BMENU) -c -p "Dynamic Package Initialization" \
	    -u "Top" \
	    -n "Thread Cancellation" ${*}.t

ch18.texi: ch18.t
	$(BMENU) -c -p "Thread-Specific Data Key Deletion" \
	    -u "Top" \
	    -n "Command and Variable Index" ${*}.t


html: $(FILES)
	-mkdir $(WWW_INSTALL)/$(PROJECT)
	$(TEXI2WWW) $(TEXI2WWW_ARGS) -dir $(WWW_INSTALL)/$(PROJECT) \
 	    $(PROJECT).texi

clean:
	rm -f *.o $(PROG) *.txt core 
	rm -f *.dvi *.ps *.log *.aux *.cp *.fn *.ky *.pg *.toc *.tp *.vr $(BASE)
	rm -f $(PROJECT) $(PROJECT)-* $(GENERATED_FILES)
	rm -f *.fixed _*


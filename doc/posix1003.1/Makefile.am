#
#  COPYRIGHT (c) 1988-2002.
#  On-Line Applications Research Corporation (OAR).
#  All rights reserved.

PROJECT = posix1003_1

include $(top_srcdir)/project.am
include $(top_srcdir)/main.am

GENERATED_FILES = ch01.texi ch02.texi ch03.texi ch04.texi ch05.texi \
    ch06.texi ch07.texi ch08.texi ch09.texi ch10.texi ch11.texi ch12.texi \
    ch13.texi ch14.texi ch15.texi ch16.texi ch17.texi ch18.texi ch19.texi

COMMON_FILES += $(top_srcdir)/common/cpright.texi

FILES = preface.texi

info_TEXINFOS = posix1003_1.texi
posix1003_1_TEXINFOS = $(FILES) $(COMMON_FILES) $(GENERATED_FILES)

ch01.texi: ch01.t
	$(BMENU2) -c -p "Preface" \
	    -u "Top" \
	    -n "Terminology and General Requirements" < $< > $@

ch02.texi: ch02.t
	$(BMENU2) -c -p "Conformance" \
	    -u "Top" \
	    -n "Process Primitives" < $< > $@

ch03.texi: ch03.t
	$(BMENU2) -c -p "Execution-Time Symbolic Constants for Portability Specifications" \
	    -u "Top" \
	    -n "Process Environment" < $< > $@

ch04.texi: ch04.t
	$(BMENU2) -c -p "Delay Process Execution" \
	    -u "Top" \
	    -n "Files and Directories" < $< > $@

ch05.texi: ch05.t
	$(BMENU2) -c -p "Get Configurable System Variables" \
	    -u "Top" \
	    -n "Input and Output Primitives" < $< > $@

ch06.texi: ch06.t
	$(BMENU2) -c -p "Get Configurable Pathname Variables" \
	    -u "Top" \
	    -n "Device- and Class-Specific Functions" < $< > $@

ch07.texi: ch07.t
	$(BMENU2) -c -p " Asynchronous File Synchronization" \
	    -u "Top" \
	    -n "Language-Specific Services for the C Programming Language" < $< > $@

ch08.texi: ch08.t
	$(BMENU2) -c -p "Set Foreground Process Group ID" \
	    -u "Top" \
	    -n "System Databases" < $< > $@

ch09.texi: ch09.t
	$(BMENU2) -c -p "Pseudo-Random Sequence Generation Functions" \
	    -u "Top" \
	    -n "Data Interchange Format" < $< > $@

ch10.texi: ch10.t
	$(BMENU2) -c -p "User Database Access" \
	    -u "Top" \
	    -n "Synchronization" < $< > $@

ch11.texi: ch11.t
	$(BMENU2) -c -p "Multiple Volumes" \
	    -u "Top" \
	    -n "Memory Management" < $< > $@

ch12.texi: ch12.t
	$(BMENU2) -c -p "Waiting on a Condition" \
	    -u "Top" \
	    -n "Execution Scheduling" < $< > $@

ch13.texi: ch13.t
	$(BMENU2) -c -p "Remove a Shared Memory Object" \
	    -u "Top" \
	    -n "Clocks and Timers" < $< > $@

ch14.texi: ch14.t
	$(BMENU2) -c -p "Change the Priority Ceiling of a Mutex" \
	    -u "Top" \
	    -n "Message Passing" < $< > $@

ch15.texi: ch15.t
	$(BMENU2) -c -p "High Resolution Sleep" \
	    -u "Top" \
	    -n "Thread Management" < $< > $@

ch16.texi: ch16.t
	$(BMENU2) -c -p "Get Message Queue Attributes" \
	    -u "Top" \
	    -n "Thread-Specific Data" < $< > $@

ch17.texi: ch17.t
	$(BMENU2) -c -p "Dynamic Package Initialization" \
	    -u "Top" \
	    -n "Thread Cancellation" < $< > $@

ch18.texi: ch18.t
	$(BMENU2) -c -p "Thread-Specific Data Key Deletion" \
	    -u "Top" \
	    -n "Compliance Summary" < $< > $@

ch19.texi: ch01.t ch02.t ch03.t ch04.t ch05.t ch06.t ch07.t ch08.t \
    ch09.t ch10.t ch11.t ch12.t ch13.t ch14.t ch15.t ch16.t ch17.t ch18.t
	$(srcdir)/summarize $^ | \
	$(BMENU2) -c -p "List of Cancellation Points" \
	    -u "Top" \
	    -n "Command and Variable Index" > $@

noinst_SCRIPTS = summarize

EXTRA_DIST = ch01.t ch02.t ch03.t ch04.t ch05.t ch06.t ch07.t ch08.t ch09.t \
    ch10.t ch11.t ch12.t ch13.t ch14.t ch15.t ch16.t ch17.t ch18.t \
    compliance_list summarize

CLEANFILES += posix1003_1.info posix1003_1.info-?

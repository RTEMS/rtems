/*
 * gdb_if.h - definition of the interface between the stub and gdb
 *
 *                   THIS SOFTWARE IS NOT COPYRIGHTED
 *
 *  The following software is offered for use in the public domain.
 *  There is no warranty with regard to this software or its performance
 *  and the user must accept the software "AS IS" with all faults.
 *
 *  THE CONTRIBUTORS DISCLAIM ANY WARRANTIES, EXPRESS OR IMPLIED, WITH
 *  REGARD TO THIS SOFTWARE INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef _GDB_IF_H
#define _GDB_IF_H

/* Max number of threads in qM response */
#define QM_MAX_THREADS (20)

struct rtems_gdb_stub_thread_info {
  char display[256];
  char name[256];
  char more_display[256];
};

/*
 *  Prototypes
 */

int parse_zbreak(const char *in, int *type, unsigned char **addr, int *len);

char* mem2hstr(char *buf, const unsigned char *mem, int count);
int   hstr2mem(unsigned char *mem, const char *buf, int count);
void  set_mem_err(void);
unsigned char get_byte(const unsigned char *ptr);
void  set_byte(unsigned char *ptr, int val);
char* thread2vhstr(char *buf, int thread);
char* thread2fhstr(char *buf, int thread);
const char* fhstr2thread(const char *buf, int *thread);
const char* vhstr2thread(const char *buf, int *thread);
char* int2fhstr(char *buf, int val);
char* int2vhstr(char *buf, int vali);
const char* fhstr2int(const char *buf, int *ival);
const char* vhstr2int(const char *buf, int *ival);
int   hstr2byte(const char *buf, int *bval);
int   hstr2nibble(const char *buf, int *nibble);

Thread_Control *rtems_gdb_index_to_stub_id(int);
int rtems_gdb_stub_thread_support_ok(void);
int rtems_gdb_stub_get_current_thread(void);
int rtems_gdb_stub_get_next_thread(int);
int rtems_gdb_stub_get_offsets(
  unsigned char **text_addr,
  unsigned char **data_addr,
  unsigned char **bss_addr
);
int rtems_gdb_stub_get_thread_regs(
  int thread,
  unsigned int *registers
);
int rtems_gdb_stub_set_thread_regs(
  int thread,
  unsigned int *registers
);
void rtems_gdb_process_query(
  char *inbuffer,
  char *outbuffer,
  int   do_threads,
  int   thread
);

/*
 * MIPS registers, numbered in the order in which gdb expects to see them.
 */
#define	ZERO		0
#define	AT		1
#define	V0		2
#define	V1		3
#define	A0		4
#define	A1		5
#define	A2		6
#define	A3		7

#define	T0		8
#define	T1		9
#define	T2		10
#define	T3		11
#define	T4		12
#define	T5		13
#define	T6		14
#define	T7		15

#define	S0		16
#define	S1		17
#define	S2		18
#define	S3		19
#define	S4		20
#define	S5		21
#define	S6		22
#define	S7		23

#define	T8		24
#define	T9		25
#define	K0		26
#define	K1		27
#define	GP		28
#define	SP		29
#define	S8		30
#define	RA		31

#define	SR		32
#define	LO		33
#define	HI		34
#define	BAD_VA		35
#define	CAUSE		36
#define	PC		37

#define	F0		38
#define	F1		39
#define	F2		40
#define	F3		41
#define	F4		42
#define	F5		43
#define	F6		44
#define	F7		45

#define	F8		46
#define	F9		47
#define	F10		48
#define	F11		49
#define	F12		50
#define	F13		51
#define	F14		52
#define	F15		53

#define	F16		54
#define	F17		55
#define	F18		56
#define	F19		57
#define	F20		58
#define	F21		59
#define	F22		60
#define	F23		61

#define	F24		62
#define	F25		63
#define	F26		64
#define	F27		65
#define	F28		66
#define	F29		67
#define	F30		68
#define	F31		69

#define	FCSR		70
#define	FIRR		71

#define	NUM_REGS	72

void mips_gdb_stub_install(int enableThreads) ;

#define MEMOPT_READABLE   1
#define MEMOPT_WRITEABLE  2

#ifndef NUM_MEMSEGS
#define NUM_MEMSEGS     10
#endif

int gdbstub_add_memsegment(unsigned,unsigned,int);

#endif /* _GDB_IF_H */

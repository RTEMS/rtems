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

/* Exception IDs */
#define LM32_EXCEPTION_RESET                  0x0
#define LM32_EXCEPTION_INST_BREAKPOINT        0x1
#define LM32_EXCEPTION_INST_BUS_ERROR         0x2
#define LM32_EXCEPTION_DATA_BREAKPOINT        0x3
#define LM32_EXCEPTION_DATA_BUS_ERROR         0x4
#define LM32_EXCEPTION_DIVIDE_BY_ZERO         0x5
#define LM32_EXCEPTION_INTERRUPT              0x6
#define LM32_EXCEPTION_SYSTEM_CALL            0x7

/* Breakpoint instruction */
#define LM32_BREAK                            0xac000002UL

/* This numbering must be consistant with GDBs numbering in gdb/lm32-tdep.c */
enum lm32_regnames {
  LM32_REG_R0, LM32_REG_R1, LM32_REG_R2, LM32_REG_R3, LM32_REG_R4, LM32_REG_R5,
  LM32_REG_R6, LM32_REG_R7, LM32_REG_R8, LM32_REG_R9, LM32_REG_R10,
  LM32_REG_R11, LM32_REG_R12, LM32_REG_R13, LM32_REG_R14, LM32_REG_R15,
  LM32_REG_R16, LM32_REG_R17, LM32_REG_R18, LM32_REG_R19, LM32_REG_R20,
  LM32_REG_R21, LM32_REG_R22, LM32_REG_R23, LM32_REG_R24, LM32_REG_R25,
  LM32_REG_GP, LM32_REG_FP, LM32_REG_SP, LM32_REG_RA, LM32_REG_EA, LM32_REG_BA,
  LM32_REG_PC, LM32_REG_EID, LM32_REG_EBA, LM32_REG_DEBA, LM32_REG_IE, NUM_REGS
};

/* keep this in sync with the debug isr handler in lm32-debug.S */
enum lm32_int_regnames {
  LM32_INT_REG_R1, LM32_INT_REG_R2, LM32_INT_REG_R3, LM32_INT_REG_R4,
  LM32_INT_REG_R5, LM32_INT_REG_R6, LM32_INT_REG_R7, LM32_INT_REG_R8,
  LM32_INT_REG_R9, LM32_INT_REG_R10, LM32_INT_REG_RA, LM32_INT_REG_EA,
  LM32_INT_REG_BA, LM32_INT_REG_R11, LM32_INT_REG_R12, LM32_INT_REG_R13,
  LM32_INT_REG_R14, LM32_INT_REG_R15, LM32_INT_REG_R16, LM32_INT_REG_R17,
  LM32_INT_REG_R18, LM32_INT_REG_R19, LM32_INT_REG_R20, LM32_INT_REG_R21,
  LM32_INT_REG_R22, LM32_INT_REG_R23, LM32_INT_REG_R24, LM32_INT_REG_R25,
  LM32_INT_REG_GP, LM32_INT_REG_FP, LM32_INT_REG_SP, LM32_INT_REG_PC,
  LM32_INT_REG_EID, LM32_INT_REG_EBA, LM32_INT_REG_DEBA, LM32_INT_REG_IE,
};

#endif /* _GDB_IF_H */

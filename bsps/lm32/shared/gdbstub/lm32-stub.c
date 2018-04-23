/*
 * Low-level support for LM32 remote debuging with GDB.
 * Contributed by Jon Beniston <jon@beniston.com>
 * Modified for RTEMS with thread support by Michael Walle <michael@walle.cc>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include <bsp.h>
#include <string.h>
#include <signal.h>
#include <rtems.h>
#include <rtems/score/cpu.h>
#include "gdb_if.h"

/* Enable support for run-length encoding */
#undef GDB_RLE_ENABLED
/* Enable support for restart packets */
#undef GDB_RESTART_ENABLED

#define GDB_STUB_ENABLE_THREAD_SUPPORT

/*
 * The following external functions provide character input and output.
 */
extern char gdb_get_debug_char(void);
extern void gdb_put_debug_char(char);
extern void gdb_console_init(void);
extern void gdb_ack_irq(void);
extern void *_deba;

/* Function prototypes */
static void allow_nested_exception(void);
static void disallow_nested_exception(void);
static char *mem2hex(unsigned char *mem, char *buf, int count);
static unsigned char *hex2mem(char *buf, unsigned char *mem, int count);
static unsigned char *bin2mem(char *buf, unsigned char *mem, int count);
static int compute_signal(int eid);
static void flush_cache(void);
static int hex2int(char **ptr, int *int_value);
static char *getpacket(void);
static void putpacket(char *buffer);

unsigned int registers[NUM_REGS];

/* BUFMAX defines the maximum number of characters in inbound/outbound buffers */
#define BUFMAX 1500

/* I/O packet buffers */
static char remcomInBuffer[BUFMAX];
static char remcomOutBuffer[BUFMAX];

/*
 * Set by debugger to indicate that when handling memory faults (bus errors), the
 * handler should set the mem_err flag and skip over the faulting instruction
 */
static volatile int may_fault;

/*
 * Set by bus error exception handler, this indicates to caller of mem2hex,
 * hex2mem or bin2mem that there has been an error.
 */
static volatile int mem_err;

/* Indicates if we're single stepping */
static unsigned char stepping;
static char branch_step;

/* Saved instructions */
static unsigned int *seq_ptr;
static unsigned int seq_insn;
static unsigned int *branch_ptr;
static unsigned int branch_insn;

#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
static char do_threads;
int current_thread_registers[NUM_REGS];
#endif

/* this mapping is used to copy the registers from a debug interrupt frame
 * see gdb_handle_break() */
static unsigned char reg_map[] = {
  0, LM32_INT_REG_R1, LM32_INT_REG_R2, LM32_INT_REG_R3, LM32_INT_REG_R4,
  LM32_INT_REG_R5, LM32_INT_REG_R6, LM32_INT_REG_R7, LM32_INT_REG_R8,
  LM32_INT_REG_R9, LM32_INT_REG_R10, LM32_INT_REG_R11, LM32_INT_REG_R12,
  LM32_INT_REG_R13, LM32_INT_REG_R14, LM32_INT_REG_R15, LM32_INT_REG_R16,
  LM32_INT_REG_R17, LM32_INT_REG_R18, LM32_INT_REG_R19, LM32_INT_REG_R20,
  LM32_INT_REG_R21, LM32_INT_REG_R22, LM32_INT_REG_R23, LM32_INT_REG_R24,
  LM32_INT_REG_R25, LM32_INT_REG_GP, LM32_INT_REG_FP, LM32_INT_REG_SP,
  LM32_INT_REG_RA, LM32_INT_REG_EA, LM32_INT_REG_BA, LM32_INT_REG_PC,
  LM32_INT_REG_EID, LM32_INT_REG_EBA, LM32_INT_REG_DEBA, LM32_INT_REG_IE
};

/*
 * Conversion helper functions
 */

/* For integer to ASCII conversion */
#define highhex(x) gdb_hexchars [(x >> 4) & 0xf]
#define lowhex(x) gdb_hexchars [x & 0xf]
const char gdb_hexchars[]="0123456789abcdef";

/* Convert ch from a hex digit to an int */
static int hex(
  unsigned char ch
)
{
  if (ch >= 'a' && ch <= 'f')
    return ch-'a'+10;
  if (ch >= '0' && ch <= '9')
    return ch-'0';
  if (ch >= 'A' && ch <= 'F')
    return ch-'A'+10;
  return -1;
}

/*
 * Convert the memory pointed to by mem into hex, placing result in buf.
 * Return a pointer to the last char put in buf ('\0'), in case of mem fault,
 * return NULL.
 */
static char *mem2hex(
  unsigned char *mem,
  char *buf, int count
)
{
  unsigned char ch;

  while (count-- > 0)
  {
    ch = *mem++;
    if (mem_err)
      return NULL;
    *buf++ = highhex(ch);
    *buf++ = lowhex(ch);
  }

  *buf = '\0';
  return buf;
}

/*
 * Convert the hex array pointed to by buf into binary to be placed in mem.
 * Return a pointer to the character AFTER the last byte written.
 */
static unsigned char *hex2mem(
  char *buf,
  unsigned char *mem,
  int count
)
{
  int i;
  unsigned char ch;

  for (i = 0; i < count; i++)
  {
    /* Convert hex data to 8-bit value */
    ch = hex(*buf++) << 4;
    ch |= hex(*buf++);
    /* Attempt to write data to memory */
    *mem++ = ch;
    /* Return NULL if write caused an exception */
    if (mem_err)
      return NULL;
  }
  return mem;
}

/*
 * Copy the binary data pointed to by buf to mem and return a pointer to the
 * character AFTER the last byte written $, # and 0x7d are escaped with 0x7d.
 */
static unsigned char *bin2mem(
  char *buf,
  unsigned char *mem,
  int count
)
{
  int i;
  unsigned char c;

  for (i = 0; i < count; i++)
  {
    /* Convert binary data to unsigned byte */
    c = *buf++;
    if (c == 0x7d)
      c = *buf++ ^ 0x20;
    /* Attempt to write value to memory */
    *mem++ = c;
    /* Return NULL if write caused an exception */
    if (mem_err)
      return NULL;
  }

  return mem;
}

/*
 * While we find nice hex chars, build an int.
 * Return number of chars processed.
 */
static int hex2int(
  char **ptr,
  int *int_value
)
{
  int num_chars = 0;
  int hex_value;

  *int_value = 0;

  while(**ptr)
  {
    hex_value = hex(**ptr);
    if (hex_value < 0)
      break;

    *int_value = (*int_value << 4) | hex_value;
    num_chars ++;

    (*ptr)++;
  }

  return (num_chars);
}

/* Convert the exception identifier to a signal number. */
static int compute_signal(
  int eid
)
{
  switch (eid)
  {
    case LM32_EXCEPTION_RESET:
      return 0;
    case LM32_EXCEPTION_INTERRUPT:
      return SIGINT;
    case LM32_EXCEPTION_DATA_BREAKPOINT:
    case LM32_EXCEPTION_INST_BREAKPOINT:
      return SIGTRAP;
    case LM32_EXCEPTION_INST_BUS_ERROR:
    case LM32_EXCEPTION_DATA_BUS_ERROR:
      return SIGSEGV;
    case LM32_EXCEPTION_DIVIDE_BY_ZERO:
      return SIGFPE;
  }

  return SIGHUP;  /* default for things we don't know about */
}

/* Scan for the sequence $<data>#<checksum> */
static char *getpacket(void)
{
  char *buffer = &remcomInBuffer[0];
  unsigned char checksum;
  unsigned char xmitcsum;
  int count;
  char ch;

  while (1)
  {
    /* wait around for the start character, ignore all other characters */
    while ((ch = gdb_get_debug_char()) != '$');

retry:
    checksum = 0;
    xmitcsum = -1;
    count = 0;

    /* now, read until a # or end of buffer is found */
    while (count < BUFMAX)
    {
      ch = gdb_get_debug_char();
      if (ch == '$')
        goto retry;
      if (ch == '#')
        break;
      checksum = checksum + ch;
      buffer[count] = ch;
      count = count + 1;
    }
    buffer[count] = 0;

    if (ch == '#')
    {
      ch = gdb_get_debug_char();
      xmitcsum = hex(ch) << 4;
      ch = gdb_get_debug_char();
      xmitcsum += hex(ch);

      if (checksum != xmitcsum)
      {
	    /* failed checksum */
        gdb_put_debug_char('-');
      }
      else
      {
	    /* successful transfer */
        gdb_put_debug_char('+');

        /* if a sequence char is present, reply the sequence ID */
        if (buffer[2] == ':')
        {
          gdb_put_debug_char(buffer[0]);
          gdb_put_debug_char(buffer[1]);

          return &buffer[3];
        }

        return &buffer[0];
      }
    }
  }
}

/* Send the packet in buffer.  */
static void putpacket(
  char *buffer
)
{
  unsigned char checksum;
  int count;
  unsigned char ch;

#ifdef GDB_RLE_ENABLED
  int run_length;
  int run_idx;
  char run_length_char;
#endif

  /*  $<packet info>#<checksum>. */
  do {
    gdb_put_debug_char('$');
    checksum = 0;
    count = 0;

#ifdef GDB_RLE_ENABLED
    while (ch = buffer[count])
    {
      /* Transmit character */
      gdb_put_debug_char(ch);
      checksum += ch;
      count += 1;

      /*
       * Determine how many consecutive characters there are that are the same
       * as the character we just transmitted
       */
      run_length = 0;
      run_idx = count;
      while ((buffer[run_idx++] == ch) && (run_length < 97))
        run_length++;
      /* Encode run length as an ASCII character */
      run_length_char = (char)(run_length + 29);
      if (   (run_length >= 3)
          && (run_length_char != '$')
          && (run_length_char != '#')
          && (run_length_char != '+')
          && (run_length_char != '-')
         )
      {
        /* Transmit run-length */
        gdb_put_debug_char('*');
        checksum += '*';
        gdb_put_debug_char(run_length_char);
        checksum += run_length_char;
        count += run_length;
      }
    }
#else
    while ((ch = buffer[count]))
    {
      gdb_put_debug_char(ch);
      checksum += ch;
      count += 1;
    }
#endif

    gdb_put_debug_char('#');
    gdb_put_debug_char(highhex(checksum));
    gdb_put_debug_char(lowhex(checksum));
  } while (gdb_get_debug_char() != '+');
}

static void allow_nested_exception(void)
{
  mem_err = 0;
  may_fault = 1;
}

static void disallow_nested_exception(void)
{
  mem_err = 0;
  may_fault = 0;
}

/* Flush the instruction cache */
static void flush_cache(void)
{
  /*
   * Executing this does no harm on CPUs without a cache. We flush data cache as
   * well as instruction cache in case the debugger has accessed memory
   * directly.
   */
  __asm__ __volatile__ ("wcsr ICC, r0\n"
                        "nop\n"
                        "nop\n"
                        "nop\n"
                        "wcsr DCC, r0\n"
                        "nop\n"
                        "nop\n"
                        "nop"
                       );
}

/* Set a h/w breakpoint at the given address */
static int set_hw_breakpoint(
  int address,
  int length
)
{
  int bp;

  /* Find a free break point register and then set it */
  __asm__ ("rcsr  %0, BP0" : "=r" (bp));
  if ((bp & 0x01) == 0)
  {
    __asm__ ("wcsr  BP0, %0" : : "r" (address | 1));
    return 1;
  }
  __asm__ ("rcsr  %0, BP1" : "=r" (bp));
  if ((bp & 0x01) == 0)
  {
    __asm__ ("wcsr  BP1, %0" : : "r" (address | 1));
    return 1;
  }
  __asm__ ("rcsr  %0, BP2" : "=r" (bp));
  if ((bp & 0x01) == 0)
  {
    __asm__ ("wcsr  BP2, %0" : : "r" (address | 1));
    return 1;
  }
  __asm__ ("rcsr  %0, BP3" : "=r" (bp));
  if ((bp & 0x01) == 0)
  {
    __asm__ ("wcsr  BP3, %0" : : "r" (address | 1));
    return 1;
  }

  /* No free breakpoint registers */
  return -1;
}

/* Remove a h/w breakpoint which should be set at the given address */
static int disable_hw_breakpoint(
  int address,
  int length
)
{
  int bp;

  /* Try to find matching breakpoint register */
  __asm__ ("rcsr  %0, BP0" : "=r" (bp));
  if ((bp & 0xfffffffc) == (address & 0xfffffffc))
  {
    __asm__ ("wcsr  BP0, %0" : : "r" (0));
    return 1;
  }
  __asm__ ("rcsr  %0, BP1" : "=r" (bp));
  if ((bp & 0xfffffffc) == (address & 0xfffffffc))
  {
    __asm__ ("wcsr  BP1, %0" : : "r" (0));
    return 1;
  }
  __asm__ ("rcsr  %0, BP2" : "=r" (bp));
  if ((bp & 0xfffffffc) == (address & 0xfffffffc))
  {
    __asm__ ("wcsr  BP2, %0" : : "r" (0));
    return 1;
  }
  __asm__ ("rcsr  %0, BP3" : "=r" (bp));
  if ((bp & 0xfffffffc) == (address & 0xfffffffc))
  {
    __asm__ ("wcsr  BP3, %0" : : "r" (0));
    return 1;
  }

  /* Breakpoint not found */
  return -1;
}

/*
 *  This support function prepares and sends the message containing the
 *  basic information about this exception.
 */
static void gdb_stub_report_exception_info(
  int thread
)
{
  char *ptr;
  int sigval;

  /* Convert exception ID to a signal number */
  sigval = compute_signal(registers[LM32_REG_EID]);

  /* Set pointer to start of output buffer */
  ptr = remcomOutBuffer;

  *ptr++ = 'T';
  *ptr++ = highhex(sigval);
  *ptr++ = lowhex(sigval);

  *ptr++ = highhex(LM32_REG_PC);
  *ptr++ = lowhex(LM32_REG_PC);
  *ptr++ = ':';
  ptr    = mem2hex((unsigned char *)&(registers[LM32_REG_PC]), ptr, 4);
  *ptr++ = ';';

  *ptr++ = highhex(LM32_REG_SP);
  *ptr++ = lowhex(LM32_REG_SP);
  *ptr++ = ':';
  ptr    = mem2hex((unsigned char *)&(registers[LM32_REG_SP]), ptr, 4);
  *ptr++ = ';';

#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
  if (do_threads)
  {
    *ptr++ = 't';
    *ptr++ = 'h';
    *ptr++ = 'r';
    *ptr++ = 'e';
    *ptr++ = 'a';
    *ptr++ = 'd';
    *ptr++ = ':';
    ptr   = thread2vhstr(ptr, thread);
    *ptr++ = ';';
  }
#endif

  *ptr++ = '\0';
}

/*
 * This function does all command procesing for interfacing to gdb. The error
 * codes we return are errno numbers.
 */
void handle_exception(void)
{
  int addr;
  int length;
  char *ptr;
  int err;
  int reg;
  unsigned insn;
  unsigned opcode;
  unsigned branch_target = 0;
  int current_thread;
  int thread;
  void *regptr;
  int host_has_detached = 0;
  int binary;

  thread = 0;
#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
  if (do_threads)
    thread = rtems_gdb_stub_get_current_thread();
#endif
  current_thread = thread;

  /*
   * Check for bus error caused by this code (rather than the program being
   * debugged)
   */
  if (may_fault && (registers[LM32_REG_EID] == LM32_EXCEPTION_DATA_BUS_ERROR))
  {
    /* Indicate that a fault occured */
    mem_err = 1;
    /* Skip over faulting instruction */
    registers[LM32_REG_PC] += 4;
    /* Resume execution */
    return;
  }

  if (stepping)
  {
    /* Remove breakpoints */
    *seq_ptr = seq_insn;
    if (branch_step)
      *branch_ptr = branch_insn;
    stepping = 0;
  }

  /* Reply to host that an exception has occured with some basic info */
  gdb_stub_report_exception_info(thread);
  putpacket(remcomOutBuffer);

  while (!host_has_detached)
  {
    remcomOutBuffer[0] = '\0';
    ptr = getpacket();
    binary = 0;

    switch (*ptr++)
    {
      /* Return last signal */
      case '?':
        gdb_stub_report_exception_info(thread);
        break;

      /* Detach - exit from debugger */
	  case 'D':
	    strcpy(remcomOutBuffer, "OK");
		host_has_detached = 1;
		break;

      /* Return the value of the CPU registers */
      case 'g':
        regptr = registers;
#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
        if (do_threads && current_thread != thread )
          regptr = &current_thread_registers;
#endif
        ptr = mem2hex((unsigned char*)regptr, remcomOutBuffer, NUM_REGS * 4);
        break;

      /* Set the value of the CPU registers */
      case 'G':
        regptr = registers;
#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
        if (do_threads && current_thread != thread )
          regptr = &current_thread_registers;
#endif
        hex2mem(ptr, (unsigned char*)regptr, NUM_REGS * 4);
        strcpy(remcomOutBuffer, "OK");
        break;

      /* Return the value of the specified register */
      case 'p':
        if (hex2int(&ptr, &reg))
        {
          ptr = remcomOutBuffer;
          ptr = mem2hex((unsigned char *)&registers[reg], ptr, 4);
        } else
          strcpy(remcomOutBuffer, "E22");
        break;

      /* Set the specified register to the given value */
      case 'P':
        if (hex2int(&ptr, &reg)
            && *ptr++ == '=')
        {
          hex2mem(ptr, (unsigned char *)&registers[reg], 4);
          strcpy(remcomOutBuffer, "OK");
        }
        else
          strcpy(remcomOutBuffer, "E22");
        break;

      /* Read memory */
      case 'm':
        /* Try to read %x,%x.  */
        if (hex2int(&ptr, &addr)
            && *ptr++ == ','
            && hex2int(&ptr, &length)
            && length < (sizeof(remcomOutBuffer)/2))
        {
          allow_nested_exception();
          if (NULL == mem2hex((unsigned char *)addr, remcomOutBuffer, length))
            strcpy(remcomOutBuffer, "E14");
          disallow_nested_exception();
        }
        else
          strcpy(remcomOutBuffer,"E22");
        break;

      /* Write memory */
      case 'X':
	    binary = 1;
      case 'M':
        /* Try to read '%x,%x:'.  */
        if (hex2int(&ptr, &addr)
            && *ptr++ == ','
            && hex2int(&ptr, &length)
            && *ptr++ == ':')
        {
          allow_nested_exception();
          if (binary)
            err = (int)bin2mem(ptr, (unsigned char *)addr, length);
          else
            err = (int)hex2mem(ptr, (unsigned char *)addr, length);
          if (err)
            strcpy(remcomOutBuffer, "OK");
          else
            strcpy(remcomOutBuffer, "E14");
          disallow_nested_exception();
        }
        else
          strcpy(remcomOutBuffer, "E22");
        break;

      /* Continue */
      case 'c':
        /* try to read optional parameter, pc unchanged if no parm */
        if (hex2int(&ptr, &addr))
          registers[LM32_REG_PC] = addr;
        flush_cache();
        return;

      /* Step */
      case 's':
        /* try to read optional parameter, pc unchanged if no parm */
        if (hex2int(&ptr, &addr))
          registers[LM32_REG_PC] = addr;
        stepping = 1;
        /* Is instruction a branch? */
        insn = *(unsigned int*)registers[LM32_REG_PC];
        opcode = insn & 0xfc000000;
        if (   (opcode == 0xe0000000)
            || (opcode == 0xf8000000)
           )
        {
          branch_step = 1;
          branch_target = registers[LM32_REG_PC]
              + (((signed)insn << 6) >> 4);
        }
        else if (   (opcode == 0x44000000)
                 || (opcode == 0x48000000)
                 || (opcode == 0x4c000000)
                 || (opcode == 0x50000000)
                 || (opcode == 0x54000000)
                 || (opcode == 0x5c000000)
                )
        {
          branch_step = 1;
          branch_target = registers[LM32_REG_PC] +
              + (((signed)insn << 16) >> 14);
        }
        else if (   (opcode == 0xd8000000)
                 || (opcode == 0xc0000000)
                )
        {
          branch_step = 1;
          branch_target = registers[(insn >> 21) & 0x1f];
        }
        else
          branch_step = 0;

        /* Set breakpoint after instruction we're stepping */
        seq_ptr = (unsigned int *)registers[LM32_REG_PC];
        seq_ptr++;
        seq_insn = *seq_ptr;
        *seq_ptr = LM32_BREAK;

        /* Make sure one insn doesn't get replaced twice */
        if (seq_ptr == (unsigned int*)branch_target)
          branch_step = 0;

        if (branch_step)
        {
          /* Set breakpoint on branch target */
          branch_ptr = (unsigned int*)branch_target;
          branch_insn = *branch_ptr;
          *branch_ptr = LM32_BREAK;
        }
        flush_cache();
        return;

      case 'Z':
        switch (*ptr++)
        {
          /* Insert h/w breakpoint */
          case '1':
            if (*ptr++ == ','
                && hex2int(&ptr, &addr)
                && *ptr++ == ','
                && hex2int(&ptr, &length))
            {
              err = set_hw_breakpoint(addr, length);
              if (err > 0)
                strcpy(remcomOutBuffer, "OK");
              else if (err < 0)
                strcpy(remcomOutBuffer, "E28");
            }
            else
              strcpy(remcomOutBuffer, "E22");
            break;
        }
        break;

      case 'z':
        switch (*ptr++)
        {
          /* Remove h/w breakpoint */
          case '1':
            if (*ptr++ == ','
                && hex2int(&ptr, &addr)
                && *ptr++ == ','
                && hex2int(&ptr, &length))
            {
              err = disable_hw_breakpoint(addr, length);
              if (err > 0)
                strcpy(remcomOutBuffer, "OK");
              else if (err < 0)
                strcpy(remcomOutBuffer, "E28");
            }
            else
              strcpy(remcomOutBuffer, "E22");
            break;
        }
        break;

      /* Query */
      case 'q':
#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
        rtems_gdb_process_query(
            remcomInBuffer,
            remcomOutBuffer,
            do_threads,
            thread );
#endif
        break;

#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
      /* Thread alive */
      case 'T':
      {
        int testThread;

        if (vhstr2thread(&remcomInBuffer[1], &testThread) == NULL)
        {
          strcpy(remcomOutBuffer, "E01");
          break;
        }

        if (rtems_gdb_index_to_stub_id(testThread) == NULL)
          strcpy(remcomOutBuffer, "E02");
        else
          strcpy(remcomOutBuffer, "OK");
      }
      break;
#endif

      /* Set thread */
      case 'H':
#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
        if (remcomInBuffer[1] != 'g')
          break;

        if (!do_threads)
          break;

        {
          int tmp, ret;

          /* Set new generic thread */
          if (vhstr2thread(&remcomInBuffer[2], &tmp) == NULL)
          {
            strcpy(remcomOutBuffer, "E01");
            break;
          }

          /* 0 means `thread' */
          if (tmp == 0)
            tmp = thread;

          if (tmp == current_thread)
          {
            /* No changes */
            strcpy(remcomOutBuffer, "OK");
            break;
          }

          /* Save current thread registers if necessary */
          if (current_thread != thread)
          {
            ret = rtems_gdb_stub_set_thread_regs(
                current_thread, (unsigned int *) &current_thread_registers);
          }

          /* Read new registers if necessary */
          if (tmp != thread)
          {
            ret = rtems_gdb_stub_get_thread_regs(
                tmp, (unsigned int *) &current_thread_registers);

            if (!ret)
            {
              /* Thread does not exist */
              strcpy(remcomOutBuffer, "E02");
              break;
            }
          }

          current_thread = tmp;
          strcpy(remcomOutBuffer, "OK");
        }
#endif
      break;

#ifdef GDB_RESTART_ENABLED
      /* Reset */
      case 'r':
      case 'R':
        /* We reset by branching to the reset exception handler. */
        registers[LM32_REG_PC] = 0;
        return;
#endif
      }

    /* reply to the request */
    putpacket(remcomOutBuffer);
  }
}

void gdb_handle_break(rtems_vector_number vector, CPU_Interrupt_frame *frame)
{
  int i;
  unsigned int *int_regs = (unsigned int*)frame;

  /* copy extended frame to registers */
  registers[LM32_REG_R0]   = 0;
  for (i = 1; i < NUM_REGS; i++)
  {
    registers[i] = int_regs[reg_map[i]];
  }

  /* now call the real handler */
  handle_exception();
  gdb_ack_irq();

  /* copy registers back to extended frame */
  for (i = 1; i < NUM_REGS; i++)
  {
    int_regs[reg_map[i]] = registers[i];
  }
}

void lm32_gdb_stub_install(int enable_threads)
{
  unsigned int dc;

  /* set DEBA and remap all exception */
  __asm__("wcsr DEBA, %0" : : "r" (&_deba));
  __asm__("rcsr %0, DC" : "=r" (dc));
  dc |= 0x2;
  __asm__("wcsr DC, %0" : : "r" (dc));

#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
  if( enable_threads )
    do_threads = 1;
  else
    do_threads = 0;
#endif

  gdb_console_init();
}


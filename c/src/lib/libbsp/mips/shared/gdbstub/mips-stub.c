#define  GDB_STUB_ENABLE_THREAD_SUPPORT 1
/*******************************************************************************

                     THIS SOFTWARE IS NOT COPYRIGHTED

    The following software is offered for use in the public domain.
    There is no warranty with regard to this software or its performance
    and the user must accept the software "AS IS" with all faults.

    THE CONTRIBUTORS DISCLAIM ANY WARRANTIES, EXPRESS OR IMPLIED, WITH
    REGARD TO THIS SOFTWARE INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

********************************************************************************
*
*   r46kstub.c -- target debugging stub for the IDT R4600 Orion processor
*
*   This module is based on the stub for the Hitachi SH processor written by
*   Ben Lee and Steve Chamberlain and supplied with gdb 4.16.  The latter
*   in turn "is originally based on an m68k software stub written by Glenn
*   Engel at HP, but has changed quite a bit."  The changes for the R4600
*   were written by C. M. Heard at VVNET.  They were based in part on the
*   Algorithmics R4000 version of Phil Bunce's PMON program.
*
*  Remote communication protocol:
*
*  A debug packet whose contents are <data>
*  is encapsulated for transmission in the form:
*
*       $ <data> # CSUM1 CSUM2
*
*       <data> must be ASCII alphanumeric and cannot include characters
*       '$' or '#'.  If <data> starts with two characters followed by
*       ':', then the existing stubs interpret this as a sequence number.
*
*       CSUM1 and CSUM2 are ascii hex representation of an 8-bit
*       checksum of <data>, the most significant nibble is sent first.
*       the hex digits 0-9,a-f are used.
*
*  Receiver responds with:
*
*       +    if CSUM is correct
*       -    if CSUM is incorrect
*
*  <data> is as follows.  All values are encoded in ascii hex digits.
*
*       Request         Packet
*
*       read registers  g
*       reply           XX....X         Each byte of register data
*                                       is described by two hex digits.
*                                       Registers are in the internal order
*                                       for GDB, and the bytes in a register
*                                       are in the same order the machine uses.
*                       or ENN          for an error.
*
*       write regs      GXX..XX         Each byte of register data
*                                       is described by two hex digits.
*       reply           OK              for success
*                       ENN             for an error
*
*       write reg       Pn...=r...      Write register n... with value r....
*       reply           OK              for success
*                       ENN             for an error
*
*       read mem        mAA..AA,LLLL    AA..AA is address, LLLL is length.
*       reply           XX..XX          XX..XX is mem contents
*                                       Can be fewer bytes than requested
*                                       if able to read only part of the data.
*                       or ENN          NN is errno
*
*       write mem       MAA..AA,LLLL:XX..XX
*                                       AA..AA is address,
*                                       LLLL is number of bytes,
*                                       XX..XX is data
*       reply           OK              for success
*                       ENN             for an error (this includes the case
*                                       where only part of the data was
*                                       written).
*
*       cont            cAA..AA         AA..AA is address to resume
*                                       If AA..AA is omitted,
*                                       resume at same address.
*
*       step            sAA..AA         AA..AA is address to resume
*                                       If AA..AA is omitted,
*                                       resume at same address.
*
*       There is no immediate reply to step or cont.
*       The reply comes when the machine stops.
*       It is           SAA             AA is the "signal number"
*
*       last signal     ?               Reply with the reason for stopping.
*                                       This is the same reply as is generated
*                                       for step or cont: SAA where AA is the
*                                       signal number.
*
*       detach          D               Host is detaching.  Reply OK and
*                                       end remote debugging session.
*
*       reserved        <other>         On other requests, the stub should
*                                       ignore the request and send an empty
*                                       response ($#<checksum>).  This way
*                                       we can extend the protocol and GDB
*                                       can tell whether the stub it is
*                                       talking to uses the old or the new.
*
*       Responses can be run-length encoded to save space.  A '*' means that
*       the next character is an ASCII encoding giving a repeat count which
*       stands for that many repetitions of the character preceding the '*'.
*       The encoding is n+29, yielding a printable character when n >=3
*       (which is where rle starts to win).  Don't use n > 99 since gdb
*       masks each character is receives with 0x7f in order to strip off
*       the parity bit.
*
*       As an example, "0* " means the same thing as "0000".
*
*******************************************************************************/


#include <string.h>
#include <signal.h>
#include "mips_opcode.h"
/* #include "memlimits.h" */
#include <rtems.h>
#include <rtems/bspIo.h>
#include "gdb_if.h"

/* Change it to something meaningful when debugging */
#undef ASSERT
#define ASSERT(x) if(!(x)) printk("ASSERT: stub: %d\n", __LINE__)

/***************/
/* Exception Codes */
#define	EXC_INT		0		/* External interrupt */
#define	EXC_MOD		1		/* TLB modification exception */
#define	EXC_TLBL	2		/* TLB miss (Load or Ifetch) */
#define	EXC_TLBS	3		/* TLB miss (Store) */
#define	EXC_ADEL	4		/* Address error (Load or Ifetch) */
#define	EXC_ADES	5		/* Address error (Store) */
#define	EXC_IBE		6		/* Bus error (Ifetch) */
#define	EXC_DBE		7		/* Bus error (data load or store) */
#define	EXC_SYS		8		/* System call */
#define	EXC_BP		9		/* Break point */
#define	EXC_RI		10		/* Reserved instruction */
#define	EXC_CPU		11		/* Coprocessor unusable */
#define	EXC_OVF		12		/* Arithmetic overflow */
#define	EXC_TRAP	13		/* Trap exception */
#define	EXC_FPE		15		/* Floating Point Exception */

/* FPU Control/Status register fields */
#define	CSR_FS		0x01000000	/* Set to flush denormals to zero */
#define	CSR_C		0x00800000	/* Condition bit (set by FP compare) */

#define	CSR_CMASK	(0x3f<<12)
#define	CSR_CE		0x00020000
#define	CSR_CV		0x00010000
#define	CSR_CZ		0x00008000
#define	CSR_CO		0x00004000
#define	CSR_CU		0x00002000
#define	CSR_CI		0x00001000

#define	CSR_EMASK	(0x1f<<7)
#define	CSR_EV		0x00000800
#define	CSR_EZ		0x00000400
#define	CSR_EO		0x00000200
#define	CSR_EU		0x00000100
#define	CSR_EI		0x00000080

#define	CSR_FMASK	(0x1f<<2)
#define	CSR_FV		0x00000040
#define	CSR_FZ		0x00000020
#define	CSR_FO		0x00000010
#define	CSR_FU		0x00000008
#define	CSR_FI		0x00000004

#define	CSR_RMODE_MASK	(0x3<<0)
#define	CSR_RM		0x00000003
#define	CSR_RP		0x00000002
#define	CSR_RZ		0x00000001
#define	CSR_RN		0x00000000

/***************/

/*
 * Saved register information.  Must be prepared by the exception
 * preprocessor before handle_exception is invoked.
 */
#if (__mips == 3)
typedef long long mips_register_t;
#define R_SZ 8
#elif (__mips == 1)
typedef unsigned int mips_register_t;
#define R_SZ 4
#else
#error "unknown MIPS ISA"
#endif
static mips_register_t *registers;

#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
static char do_threads;      /* != 0 means we are supporting threads */
#endif

/*
 * The following external functions provide character input and output.
 */
extern char getDebugChar (void);
extern void putDebugChar (char);

/*
 * The following definitions are used for the gdb stub memory map
 */
struct memseg
{
      unsigned begin, end, opts;
};

static int is_readable(unsigned,unsigned);
static int is_writeable(unsigned,unsigned);
static int is_steppable(unsigned);

/*
 * BUFMAX defines the maximum number of characters in the inbound & outbound
 * packet buffers.  At least 4+(sizeof registers)*2 bytes will be needed for
 * register packets.  Memory dump packets can profitably use even more.
 */
#define BUFMAX 1500

static char inBuffer[BUFMAX];
static char outBuffer[BUFMAX];

/* Structure to keep info on a z-breaks */
#define BREAKNUM 32

struct z0break
{
  /* List support */
  struct z0break *next;
  struct z0break *prev;

  /* Location, preserved data */

  /* the address pointer, really, really must be a pointer to
  ** a 32 bit quantity (likely 64 on the R4k), so the full instruction is read &
  ** written.  Making it a char * as on the i386 will cause
  ** the zbreaks to mess up the breakpoint instructions
  */
  unsigned char *address;
  unsigned instr;
};

static struct z0break z0break_arr[BREAKNUM];
static struct z0break *z0break_avail = NULL;
static struct z0break *z0break_list  = NULL;


/*
 * Convert an int to hex.
 */
const char gdb_hexchars[] = "0123456789abcdef";

#define highhex(x) gdb_hexchars [(x >> 4) & 0xf]
#define lowhex(x) gdb_hexchars [x & 0xf]

/*
 * Convert length bytes of data starting at addr into hex, placing the
 * result in buf.  Return a pointer to the last (null) char in buf.
 */
static char *
mem2hex (void *_addr, int length, char *buf)
{
  unsigned int addr = (unsigned int) _addr;

  if (((addr & 0x7) == 0) && ((length & 0x7) == 0))      /* dword aligned */
    {
      long long *source = (long long *) (addr);
      long long *limit  = (long long *) (addr + length);

      while (source < limit)
        {
          int i;
          long long k = *source++;

          for (i = 15; i >= 0; i--)
            *buf++ = gdb_hexchars [(k >> (i*4)) & 0xf];
        }
    }
  else if (((addr & 0x3) == 0) && ((length & 0x3) == 0)) /* word aligned */
    {
      int *source = (int *) (addr);
      int *limit  = (int *) (addr + length);

      while (source < limit)
        {
          int i;
          int k = *source++;

          for (i = 7; i >= 0; i--)
            *buf++ = gdb_hexchars [(k >> (i*4)) & 0xf];
        }
    }
  else if (((addr & 0x1) == 0) && ((length & 0x1) == 0)) /* halfword aligned */
    {
      short *source = (short *) (addr);
      short *limit  = (short *) (addr + length);

      while (source < limit)
        {
          int i;
          short k = *source++;

          for (i = 3; i >= 0; i--)
            *buf++ = gdb_hexchars [(k >> (i*4)) & 0xf];
        }
    }
  else                                                   /* byte aligned */
    {
      char *source = (char *) (addr);
      char *limit  = (char *) (addr + length);

      while (source < limit)
        {
          int i;
          char k = *source++;

          for (i = 1; i >= 0; i--)
            *buf++ = gdb_hexchars [(k >> (i*4)) & 0xf];
        }
    }

  *buf = '\0';
  return (buf);
}


/*
 * Convert a hex character to an int.
 */
static int
hex (char ch)
{
  if ((ch >= 'a') && (ch <= 'f'))
    return (ch - 'a' + 10);
  if ((ch >= '0') && (ch <= '9'))
    return (ch - '0');
  if ((ch >= 'A') && (ch <= 'F'))
    return (ch - 'A' + 10);
  return (-1);
}

/*
 * Convert a string from hex to int until a non-hex digit
 * is found.  Return the number of characters processed.
 */
static int
hexToInt (char **ptr, int *intValue)
{
  int numChars = 0;
  int hexValue;

  *intValue = 0;

  while (**ptr)
    {
      hexValue = hex (**ptr);
      if (hexValue >= 0)
        {
          *intValue = (*intValue << 4) | hexValue;
          numChars++;
        }
      else
        break;

      (*ptr)++;
    }

  return (numChars);
}

/*
 * Convert a string from hex to long long until a non-hex
 * digit is found.  Return the number of characters processed.
 */
static int
hexToLongLong (char **ptr, long long *intValue)
{
  int numChars = 0;
  int hexValue;

  *intValue = 0;

  while (**ptr)
    {
      hexValue = hex (**ptr);
      if (hexValue >= 0)
        {
          *intValue = (*intValue << 4) | hexValue;
          numChars++;
        }
      else
        break;

      (*ptr)++;
    }

  return (numChars);
}

/*
 * Convert the hex array buf into binary, placing the result at the
 * specified address.  If the conversion fails at any point (i.e.,
 * if fewer bytes are written than indicated by the size parameter)
 * then return 0;  otherwise return 1.
 */
static int
hex2mem (char *buf, void *_addr, int length)
{
  unsigned int addr = (unsigned int) _addr;
  if (((addr & 0x7) == 0) && ((length & 0x7) == 0))      /* dword aligned */
    {
      long long *target = (long long *) (addr);
      long long *limit  = (long long *) (addr + length);

      while (target < limit)
        {
          int i, j;
          long long k = 0;

          for (i = 0; i < 16; i++)
            if ((j = hex(*buf++)) < 0)
              return 0;
            else
              k = (k << 4) + j;
          *target++ = k;
        }
    }
  else if (((addr & 0x3) == 0) && ((length & 0x3) == 0)) /* word aligned */
    {
      int *target = (int *) (addr);
      int *limit  = (int *) (addr + length);

      while (target < limit)
        {
          int i, j;
          int k = 0;

          for (i = 0; i < 8; i++)
            if ((j = hex(*buf++)) < 0)
              return 0;
            else
              k = (k << 4) + j;
          *target++ = k;
        }
    }
  else if (((addr & 0x1) == 0) && ((length & 0x1) == 0)) /* halfword aligned */
    {
      short *target = (short *) (addr);
      short *limit  = (short *) (addr + length);

      while (target < limit)
        {
          int i, j;
          short k = 0;

          for (i = 0; i < 4; i++)
            if ((j = hex(*buf++)) < 0)
              return 0;
            else
              k = (k << 4) + j;
          *target++ = k;
        }
    }
  else                                                   /* byte aligned */
    {
      char *target = (char *) (addr);
      char *limit  = (char *) (addr + length);

      while (target < limit)
        {
          int i, j;
          char k = 0;

          for (i = 0; i < 2; i++)
            if ((j = hex(*buf++)) < 0)
              return 0;
            else
              k = (k << 4) + j;
          *target++ = k;
        }
    }

  return 1;
}

/* Convert the binary stream in BUF to memory.

   Gdb will escape $, #, and the escape char (0x7d).
   COUNT is the total number of bytes to write into
   memory. */
static unsigned char *
bin2mem (
  char *buf,
  unsigned char *mem,
  int   count
)
{
  int i;

  for (i = 0; i < count; i++) {
      /* Check for any escaped characters. Be paranoid and
         only unescape chars that should be escaped. */
      if (*buf == 0x7d) {
          switch (*(buf+1)) {
            case 0x3:  /* # */
            case 0x4:  /* $ */
            case 0x5d: /* escape char */
              buf++;
              *buf |= 0x20;
              break;
            default:
              /* nothing */
              break;
            }
        }

      *mem++ = *buf++;
    }

  return mem;
}


/*
 * Scan the input stream for a sequence for the form $<data>#<checksum>.
 */
static void
getpacket (char *buffer)
{
  unsigned char checksum;
  unsigned char xmitcsum;
  int i;
  int count;
  char ch;
  do
    {
      /* wait around for the start character, ignore all other characters */
      while ((ch = getDebugChar ()) != '$');
      checksum = 0;
      xmitcsum = -1;

      count = 0;

      /* now, read until a # or end of buffer is found */
      while ( (count < BUFMAX-1) && ((ch = getDebugChar ()) != '#') )
          checksum += (buffer[count++] = ch);

      /* make sure that the buffer is null-terminated */
      buffer[count] = '\0';

      if (ch == '#')
        {
          xmitcsum = hex (getDebugChar ()) << 4;
          xmitcsum += hex (getDebugChar ());
          if (checksum != xmitcsum)
            putDebugChar ('-'); /* failed checksum */
          else
            {
              putDebugChar ('+');       /* successful transfer */
              /* if a sequence char is present, reply the sequence ID */
              if (buffer[2] == ':')
                {
                  putDebugChar (buffer[0]);
                  putDebugChar (buffer[1]);
                  /* remove sequence chars from buffer */
                  for (i = 3; i <= count; i++)
                    buffer[i - 3] = buffer[i];
                }
            }
        }
    }
  while (checksum != xmitcsum);
}

/*
 * Get a positive/negative acknowledgment for a transmitted packet.
 */
static char
getAck (void)
{
  char c;

  do
    {
      c = getDebugChar ();
    }
  while ((c != '+') && (c != '-'));

  return c;
}

/*
 * Send the packet in buffer and wait for a positive acknowledgement.
 */
static void
putpacket (char *buffer)
{
  int checksum;

  /* $<packet info>#<checksum> */
  do
    {
      char *src = buffer;
      putDebugChar ('$');
      checksum = 0;

      while (*src != '\0')
        {
          int runlen = 0;

          /* Do run length encoding */
          while ((src[runlen] == src[0]) && (runlen < 99))
            runlen++;
          if (runlen > 3)
            {
              int encode;
              /* Got a useful amount */
              putDebugChar (*src);
              checksum += *src;
              putDebugChar ('*');
              checksum += '*';
              checksum += (encode = (runlen - 4) + ' ');
              putDebugChar (encode);
              src += runlen;
            }
          else
            {
              putDebugChar (*src);
              checksum += *src;
              src++;
             }
        }

      putDebugChar ('#');
      putDebugChar (highhex (checksum));
      putDebugChar (lowhex (checksum));
    }
  while  (getAck () != '+');
}


/*
 * Saved instruction data for single step support
 */
static struct
  {
    unsigned *targetAddr;
    unsigned savedInstr;
  }
instrBuffer;

/*
 * If a step breakpoint was planted restore the saved instruction.
 */
static void
undoSStep (void)
{
  if (instrBuffer.targetAddr != NULL)
    {
      *instrBuffer.targetAddr = instrBuffer.savedInstr;
      instrBuffer.targetAddr = NULL;
    }
  instrBuffer.savedInstr = NOP_INSTR;
}

/*
 * If a single step is requested put a temporary breakpoint at the instruction
 * which logically follows the next one to be executed.  If the next instruction
 * is a branch instruction then skip the instruction in the delay slot.  NOTE:
 * ERET instructions are NOT handled, as it is impossible to single-step through
 * the exit code in an exception handler.  In addition, no attempt is made to
 * do anything about BC0T and BC0F, since a condition bit for coprocessor 0
 * is not defined on the R4600.  Finally, BC2T and BC2F are ignored since there
 * is no coprocessor 2 on a 4600.
 */
static void
doSStep (void)
{
   InstFmt inst;

   instrBuffer.targetAddr = (unsigned *)(registers[PC]+4);    /* set default */

   inst.word = *(unsigned *)registers[PC];     /* read the next instruction  */

   switch (inst.RType.op) {                    /* override default if branch */
      case OP_SPECIAL:
         switch (inst.RType.func) {
            case OP_JR:
            case OP_JALR:
               instrBuffer.targetAddr =
                  (unsigned *)registers[inst.RType.rs];
               break;
         };
         break;

      case OP_REGIMM:
         switch (inst.IType.rt) {
            case OP_BLTZ:
            case OP_BLTZL:
            case OP_BLTZAL:
            case OP_BLTZALL:
               if (registers[inst.IType.rs] < 0 )
                  instrBuffer.targetAddr =
                     (unsigned *)(((signed short)inst.IType.imm<<2)
                                  + (registers[PC]+4));
               else
                  instrBuffer.targetAddr = (unsigned*)(registers[PC]+8);
               break;
            case OP_BGEZ:
            case OP_BGEZL:
            case OP_BGEZAL:
            case OP_BGEZALL:
               if (registers[inst.IType.rs] >= 0 )
                  instrBuffer.targetAddr =
                     (unsigned *)(((signed short)inst.IType.imm<<2)
                                  + (registers[PC]+4));
               else
                  instrBuffer.targetAddr = (unsigned*)(registers[PC]+8);
               break;
         };
         break;

      case OP_J:
      case OP_JAL:
         instrBuffer.targetAddr =
            (unsigned *)((inst.JType.target<<2) + ((registers[PC]+4)&0xf0000000));
         break;

      case OP_BEQ:
      case OP_BEQL:
         if (registers[inst.IType.rs] == registers[inst.IType.rt])
            instrBuffer.targetAddr =
               (unsigned *)(((signed short)inst.IType.imm<<2) + (registers[PC]+4));
         else
            instrBuffer.targetAddr = (unsigned*)(registers[PC]+8);
         break;
      case OP_BNE:
      case OP_BNEL:
         if (registers[inst.IType.rs] != registers[inst.IType.rt])
            instrBuffer.targetAddr =
               (unsigned *)(((signed short)inst.IType.imm<<2) + (registers[PC]+4));
         else
            instrBuffer.targetAddr = (unsigned*)(registers[PC]+8);
         break;
      case OP_BLEZ:
      case OP_BLEZL:
         if (registers[inst.IType.rs] <= 0)
            instrBuffer.targetAddr =
               (unsigned *)(((signed short)inst.IType.imm<<2) + (registers[PC]+4));
         else
            instrBuffer.targetAddr = (unsigned*)(registers[PC]+8);
         break;
      case OP_BGTZ:
      case OP_BGTZL:
         if (registers[inst.IType.rs] > 0)
            instrBuffer.targetAddr =
               (unsigned *)(((signed short)inst.IType.imm<<2) + (registers[PC]+4));
         else
            instrBuffer.targetAddr = (unsigned*)(registers[PC]+8);
         break;

      case OP_COP1:
         if (inst.RType.rs == OP_BC)
            switch (inst.RType.rt) {
               case COPz_BCF:
               case COPz_BCFL:
                  if (registers[FCSR] & CSR_C)
                     instrBuffer.targetAddr = (unsigned*)(registers[PC]+8);
                  else
                     instrBuffer.targetAddr =
                        (unsigned *)(((signed short)inst.IType.imm<<2)
                                     + (registers[PC]+4));
                  break;
               case COPz_BCT:
               case COPz_BCTL:
                  if (registers[FCSR] & CSR_C)
                     instrBuffer.targetAddr =
                        (unsigned *)(((signed short)inst.IType.imm<<2)
                                     + (registers[PC]+4));
                  else
                     instrBuffer.targetAddr = (unsigned*)(registers[PC]+8);
                  break;
            };
         break;
   }

   if( is_steppable((unsigned)instrBuffer.targetAddr) && *(instrBuffer.targetAddr) != BREAK_INSTR )
   {
      instrBuffer.savedInstr = *instrBuffer.targetAddr;
      *instrBuffer.targetAddr = BREAK_INSTR;
   }
   else
   {
      instrBuffer.targetAddr = NULL;
      instrBuffer.savedInstr = NOP_INSTR;
   }
   return;
}


/*
 * Translate the R4600 exception code into a Unix-compatible signal.
 */
static int
computeSignal (void)
{
  int exceptionCode = (registers[CAUSE] & CAUSE_EXCMASK) >> CAUSE_EXCSHIFT;

  switch (exceptionCode)
    {
    case EXC_INT:
      /* External interrupt */
      return SIGINT;

    case EXC_RI:
      /* Reserved instruction */
    case EXC_CPU:
      /* Coprocessor unusable */
      return SIGILL;

    case EXC_BP:
      /* Break point */
      return SIGTRAP;

    case EXC_OVF:
      /* Arithmetic overflow */
    case EXC_TRAP:
      /* Trap exception */
    case EXC_FPE:
      /* Floating Point Exception */
      return SIGFPE;

    case EXC_IBE:
      /* Bus error (Ifetch) */
    case EXC_DBE:
      /* Bus error (data load or store) */
      return SIGBUS;

    case EXC_MOD:
      /* TLB modification exception */
    case EXC_TLBL:
      /* TLB miss (Load or Ifetch) */
    case EXC_TLBS:
      /* TLB miss (Store) */
    case EXC_ADEL:
      /* Address error (Load or Ifetch) */
    case EXC_ADES:
      /* Address error (Store) */
      return SIGSEGV;

    case EXC_SYS:
      /* System call */
      return SIGSYS;

    default:
      return SIGTERM;
    }
}

/*
 *  This support function prepares and sends the message containing the
 *  basic information about this exception.
 */

void gdb_stub_report_exception_info(
  rtems_vector_number vector,
  CPU_Interrupt_frame *frame,
  int                  thread
)
{
   char *optr;
   int sigval;

   optr = outBuffer;
   *optr++ = 'T';
   sigval = computeSignal ();
   *optr++ = highhex (sigval);
   *optr++ = lowhex (sigval);

   *optr++ = highhex(SP); /*gdb_hexchars[SP]; */
   *optr++ = lowhex(SP);
   *optr++ = ':';
   optr    = mem2hstr(optr, (unsigned char *)&frame->sp, R_SZ );
   *optr++ = ';';

   *optr++ = highhex(PC); /*gdb_hexchars[PC]; */
   *optr++ = lowhex(PC);
   *optr++ = ':';
   optr    = mem2hstr(optr, (unsigned char *)&frame->epc, R_SZ );
   *optr++ = ';';

#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
   if (do_threads)
   {
      *optr++ = 't';
      *optr++ = 'h';
      *optr++ = 'r';
      *optr++ = 'e';
      *optr++ = 'a';
      *optr++ = 'd';
      *optr++ = ':';
      optr   = thread2vhstr(optr, thread);
      *optr++ = ';';
   }
#endif
   *optr++ = '\0';
}



/*
 * Scratch frame used to retrieve contexts for different threads, so as
 * not to disrupt our current context on the stack
 */
CPU_Interrupt_frame current_thread_registers;

/*
 * This function handles all exceptions.  It only does two things:
 * it figures out why it was activated and tells gdb, and then it
 * reacts to gdb's requests.
 */

extern void clear_cache(void);
void handle_exception (rtems_vector_number vector, CPU_Interrupt_frame *frame)
{
   int          host_has_detached = 0;
   int          regno, addr, length;
   char         *ptr;
   int          current_thread;  /* current generic thread */
   int          thread;          /* stopped thread: context exception happened in */

   long long    regval;
   void         *regptr;
   int          binary;

   registers = (mips_register_t *)frame;

   thread = 0;
#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
   if (do_threads) {
      thread = rtems_gdb_stub_get_current_thread();
   }
#endif
   current_thread = thread;

   {
      /* reapply all breakpoints regardless of how we came in */
      struct z0break *z0, *zother;

      for (zother=z0break_list; zother!=NULL; zother=zother->next)
      {
         if( zother->instr == 0xffffffff )
         {
            /* grab the instruction */
            zother->instr = *(zother->address);
            /* and insert the breakpoint */
            *(zother->address) = BREAK_INSTR;
         }
      }

      /* see if we're coming from a breakpoint */
      if( *((unsigned *)frame->epc) == BREAK_INSTR )
      {
         /* see if its one of our zbreaks */
         for (z0=z0break_list; z0!=NULL; z0=z0->next)
         {
            if( (unsigned)z0->address == frame->epc)
               break;
         }
         if( z0 )
         {
            /* restore the original instruction */
            *(z0->address) = z0->instr;
            /* flag the breakpoint */
            z0->instr = 0xffffffff;

            /*
               now when we return, we'll execute the original code in
               the original state.  This leaves our breakpoint inactive
               since the break instruction isn't there, but we'll reapply
               it the next time we come in via step or breakpoint
            */
         }
         else
         {
            /* not a zbreak, see if its our trusty stepping code */

            /*
             * Restore the saved instruction at
             * the single-step target address.
             */
            undoSStep();
         }
      }
   }

   /* reply to host that an exception has occurred with some basic info */
   gdb_stub_report_exception_info(vector, frame, thread);
   putpacket (outBuffer);

   while (!(host_has_detached)) {
      outBuffer[0] = '\0';
      getpacket (inBuffer);
      binary = 0;

      switch (inBuffer[0]) {
         case '?':
            gdb_stub_report_exception_info(vector, frame, thread);
            break;

         case 'd': /* toggle debug flag */
            /* can print ill-formed commands in valid packets & checksum errors */
            break;

         case 'D':
            /* remote system is detaching - return OK and exit from debugger */
            strcpy (outBuffer, "OK");
            host_has_detached = 1;
            break;

         case 'g':               /* return the values of the CPU registers */
            regptr = registers;
#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
            if (do_threads && current_thread != thread )
               regptr = &current_thread_registers;
#endif
            mem2hex (regptr, NUM_REGS * (sizeof registers), outBuffer);
            break;

         case 'G':       /* set the values of the CPU registers - return OK */
            regptr = registers;
#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
            if (do_threads && current_thread != thread )
               regptr = &current_thread_registers;
#endif
            if (hex2mem (&inBuffer[1], regptr, NUM_REGS * (sizeof registers)))
               strcpy (outBuffer, "OK");
            else
               strcpy (outBuffer, "E00"); /* E00 = bad "set register" command */
            break;

         case 'P':
            /* Pn...=r...  Write register n... with value r... - return OK */
            ptr = &inBuffer[1];
            if (hexToInt(&ptr, &regno) &&
                *ptr++ == '=' &&
                hexToLongLong(&ptr, &regval))
            {
               registers[regno] = regval;
               strcpy (outBuffer, "OK");
            }
            else
               strcpy (outBuffer, "E00"); /* E00 = bad "set register" command */
            break;

         case 'm':
            /* mAA..AA,LLLL  Read LLLL bytes at address AA..AA */
            ptr = &inBuffer[1];
            if (hexToInt (&ptr, &addr)
                && *ptr++ == ','
                && hexToInt (&ptr, &length)
                && is_readable (addr, length)
                && (length < (BUFMAX - 4)/2))
               mem2hex ((void *)addr, length, outBuffer);
            else
               strcpy (outBuffer, "E01"); /* E01 = bad 'm' command */
            break;

         case 'X':  /* XAA..AA,LLLL:<binary data>#cs */
            binary = 1;
         case 'M':
            /* MAA..AA,LLLL:  Write LLLL bytes at address AA..AA - return OK */
            ptr = &inBuffer[1];
            if (hexToInt (&ptr, &addr)
                && *ptr++ == ','
                && hexToInt (&ptr, &length)
                && *ptr++ == ':'
                && is_writeable (addr, length) ) {
               if ( binary )
                  hex2mem (ptr, (void *)addr, length);
               else
                  bin2mem (ptr, (void *)addr, length);
               strcpy (outBuffer, "OK");
            }
            else
               strcpy (outBuffer, "E02"); /* E02 = bad 'M' command */
            break;

         case 'c':
            /* cAA..AA    Continue at address AA..AA(optional) */
         case 's':
            /* sAA..AA    Step one instruction from AA..AA(optional) */
         {
            /* try to read optional parameter, pc unchanged if no parm */
            ptr = &inBuffer[1];
            if (hexToInt (&ptr, &addr))
               registers[PC] = addr;

            if (inBuffer[0] == 's')
               doSStep ();
         }
         goto stubexit;

         case 'k':  /* remove all zbreaks if any */
        dumpzbreaks:
         {
            {
               /* Unlink the entire list */
               struct z0break *z0, *znxt;

               while( (z0= z0break_list) )
               {

                  /* put back the instruction */
                  if( z0->instr != 0xffffffff )
                     *(z0->address) = z0->instr;

                  /* pop off the top entry */
                  znxt = z0->next;
                  if( znxt ) znxt->prev = NULL;
                  z0break_list = znxt;

                  /* and put it on the free list */
                  z0->prev = NULL;
                  z0->next = z0break_avail;
                  z0break_avail = z0;
               }
            }

            strcpy(outBuffer, "OK");
         }
         break;

         case 'q':   /* queries */
#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
            rtems_gdb_process_query( inBuffer, outBuffer, do_threads, thread );
#endif
            break;

#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
         case 'T':
         {
            int testThread;

            if( vhstr2thread(&inBuffer[1], &testThread) == NULL )
            {
               strcpy(outBuffer, "E01");
               break;
            }

            if( rtems_gdb_index_to_stub_id(testThread) == NULL )
            {
               strcpy(outBuffer, "E02");
            }
            else
            {
               strcpy(outBuffer, "OK");
            }
         }
         break;
#endif

         case 'H':  /* set new thread */
#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
            if (inBuffer[1] != 'g') {
               break;
            }

            if (!do_threads) {
               break;
            }

            {
               int tmp, ret;

               /* Set new generic thread */
               if (vhstr2thread(&inBuffer[2], &tmp) == NULL) {
                  strcpy(outBuffer, "E01");
                  break;
               }

               /* 0 means `thread' */
               if (tmp == 0) {
                  tmp = thread;
               }

               if (tmp == current_thread) {
                  /* No changes */
                  strcpy(outBuffer, "OK");
                  break;
               }

               /* Save current thread registers if necessary */
               if (current_thread != thread) {
                  ret = rtems_gdb_stub_set_thread_regs(
                     current_thread,
                     (unsigned int *) (void *)&current_thread_registers);
                  ASSERT(ret);
               }

               /* Read new registers if necessary */
               if (tmp != thread) {
                  ret = rtems_gdb_stub_get_thread_regs(
                     tmp, (unsigned int *) (void *)&current_thread_registers);

                  if (!ret) {
                     /* Thread does not exist */
                     strcpy(outBuffer, "E02");
                     break;
                  }
               }

               current_thread = tmp;
               strcpy(outBuffer, "OK");
            }
#endif
            break;

         case 'Z':  /* Add breakpoint */
         {
            int ret, type, len;
            unsigned char *address;
            struct z0break *z0;

            ret = parse_zbreak(inBuffer, &type, &address, &len);
            if (!ret) {
               strcpy(outBuffer, "E01");
               break;
            }

            if (type != 0) {
               /* We support only software break points so far */
               strcpy(outBuffer, "E02");
               break;
            }

            if (len != R_SZ) {     /* was 1 */
               strcpy(outBuffer, "E03");
               break;
            }

            /* Let us check whether this break point already set */
            for (z0=z0break_list; z0!=NULL; z0=z0->next) {
               if (z0->address == address) {
                  break;
               }
            }

            if (z0 != NULL) {
               /* we already have a breakpoint for this address */
               strcpy(outBuffer, "E04");
               break;
            }

            /* Let us allocate new break point */
            if (z0break_avail == NULL) {
               strcpy(outBuffer, "E05");
               break;
            }

            /* Get entry */
            z0 = z0break_avail;
            z0break_avail = z0break_avail->next;

            /* Let us copy memory from address add stuff the break point in */
            /*
            *if (mem2hstr(z0->buf, address, 1) == NULL ||
              !hstr2mem(address, "cc" , 1)) {

               * Memory error *
               z0->next = z0break_avail;
               z0break_avail = z0;
               strcpy(outBuffer, "E05");
               break;
            }*/

            /* Fill it */
            z0->address = address;

            if( z0->address == (unsigned char *) frame->epc )
            {
               /* re-asserting the breakpoint that put us in here, so
               we'll add the breakpoint but leave the code in place
               since we'll be returning to it when the user continues */
               z0->instr = 0xffffffff;
            }
            else
            {
               /* grab the instruction */
               z0->instr = *(z0->address);
               /* and insert the break */
               *(z0->address) = BREAK_INSTR;
            }

            /* Add to the list */
            {
               struct z0break *znxt = z0break_list;

               z0->prev = NULL;
               z0->next = znxt;

               if( znxt ) znxt->prev = z0;
               z0break_list = z0;
            }

            strcpy(outBuffer, "OK");
         }
         break;

         case 'z': /* remove breakpoint */
            if (inBuffer[1] == 'z')
            {
               goto dumpzbreaks;

               /*
                * zz packet - remove all breaks *
                z0last = NULL;

                for (z0=z0break_list; z0!=NULL; z0=z0->next)
                {
                if(!hstr2mem(z0->address, z0->buf, R_SZ))
                {
                ret = 0;
                }
                z0last = z0;
                }

                * Free entries if any *
                if (z0last != NULL) {
                z0last->next  = z0break_avail;
                z0break_avail = z0break_list;
                z0break_list  = NULL;
                }

                if (ret) {
                strcpy(outBuffer, "OK");
                } else {
                strcpy(outBuffer, "E04");
                }
                break;
               */
            }
            else
            {
               int ret, type, len;
               unsigned char *address;
               struct z0break *z0;

               ret = parse_zbreak(inBuffer, &type, &address, &len);
               if (!ret) {
                  strcpy(outBuffer, "E01");
                  break;
               }

               if (type != 0) {
                  /* We support only software break points so far */
                  break;
               }

               if (len != R_SZ) {
                  strcpy(outBuffer, "E02");
                  break;
               }

               /* Let us check whether this break point set */
               for (z0=z0break_list; z0!=NULL; z0=z0->next) {
                  if (z0->address == address) {
                     break;
                  }
               }

               if (z0 == NULL) {
                  /* Unknown breakpoint */
                  strcpy(outBuffer, "E03");
                  break;
               }

               /*
               if (!hstr2mem(z0->address, z0->buf, R_SZ)) {
                  strcpy(outBuffer, "E04");
                  break;
                  }*/

               if( z0->instr != 0xffffffff )
               {
                  /* put the old instruction back  */
                  *(z0->address) = z0->instr;
               }

               /* Unlink entry */
               {
                  struct z0break *zprv = z0->prev, *znxt = z0->next;

                  if( zprv ) zprv->next = znxt;
                  if( znxt ) znxt->prev = zprv;

                  if( !zprv ) z0break_list = znxt;

                  znxt = z0break_avail;

                  z0break_avail = z0;
                  z0->prev = NULL;
                  z0->next = znxt;
               }

               strcpy(outBuffer, "OK");
            }
            break;

         default: /* do nothing */
            break;
      }

      /* reply to the request */
      putpacket (outBuffer);
   }

  stubexit:

   /*
    *  The original code did this in the assembly wrapper.  We should consider
    *  doing it here before we return.
    *
    *  On exit from the exception handler invalidate each line in the I-cache
    *  and write back each dirty line in the D-cache.  This needs to be done
    *  before the target program is resumed in order to ensure that software
    *  breakpoints and downloaded code will actually take effect.  This
    *  is because modifications to code in ram will affect the D-cache,
    *  but not necessarily the I-cache.
    */

   clear_cache();
}

static int numsegs;
static struct memseg   memsegments[NUM_MEMSEGS];

int gdbstub_add_memsegment( unsigned base, unsigned end, int opts )
{
   if( numsegs == NUM_MEMSEGS ) return -1;

   memsegments[numsegs].begin = base;
   memsegments[numsegs].end   = end;
   memsegments[numsegs].opts   = opts;

   ++numsegs;
   return RTEMS_SUCCESSFUL;
}

static int is_readable(unsigned ptr, unsigned len)
{
   struct memseg *ms;
   int i;

   if( (ptr & 0x3) ) return -1;

   for(i=0; i<numsegs; i++)
   {
      ms= &memsegments[i];

      if( ms->begin <= ptr && ptr+len <= ms->end && (ms->opts & MEMOPT_READABLE) )
         return -1;
   }
   return 0;
}

static int is_writeable(unsigned ptr, unsigned len)
{
   struct memseg *ms;
   int i;

   if( (ptr & 0x3) ) return -1;

   for(i=0; i<numsegs; i++)
   {
      ms= &memsegments[i];

      if( ms->begin <= ptr && ptr+len <= ms->end && (ms->opts & MEMOPT_WRITEABLE) )
         return -1;
   }
   return 0;
}

static int is_steppable(unsigned ptr)
{
   struct memseg *ms;
   int i;

   if( (ptr & 0x3) ) return -1;

   for(i=0; i<numsegs; i++)
   {
      ms= &memsegments[i];

      if( ms->begin <= ptr && ptr <= ms->end && (ms->opts & MEMOPT_WRITEABLE) )
         return -1;
   }
   return 0;
}

static char initialized = 0;   /* 0 means we are not initialized */

void mips_gdb_stub_install(int enableThreads)
{
   /*
     These are the RTEMS-defined vectors for all the MIPS exceptions
   */
   int exceptionVector[]= { MIPS_EXCEPTION_MOD, \
                            MIPS_EXCEPTION_TLBL, \
                            MIPS_EXCEPTION_TLBS, \
                            MIPS_EXCEPTION_ADEL, \
                            MIPS_EXCEPTION_ADES, \
                            MIPS_EXCEPTION_IBE, \
                            MIPS_EXCEPTION_DBE, \
                            MIPS_EXCEPTION_SYSCALL, \
                            MIPS_EXCEPTION_BREAK, \
                            MIPS_EXCEPTION_RI, \
                            MIPS_EXCEPTION_CPU, \
                            MIPS_EXCEPTION_OVERFLOW, \
                            MIPS_EXCEPTION_TRAP, \
                            MIPS_EXCEPTION_VCEI, \
                            MIPS_EXCEPTION_FPE, \
                            MIPS_EXCEPTION_C2E, \
                            MIPS_EXCEPTION_WATCH, \
                            MIPS_EXCEPTION_VCED, \
                            -1 };
   int  i;
   rtems_isr_entry old;

   if (initialized)
   {
      ASSERT(0);
      return;
   }

   memset( memsegments,0,sizeof(struct memseg)*NUM_MEMSEGS );
   numsegs = 0;

#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
   if( enableThreads )
      do_threads = 1;
   else
      do_threads = 0;
#endif

   {
      struct z0break *z0;

      z0break_avail = NULL;
      z0break_list  = NULL;

      /* z0breaks list init, now we'll do it so it makes sense... */
      for (i=0; i<BREAKNUM; i++)
      {
         memset( (z0= &z0break_arr[i]), 0, sizeof(struct z0break));

         z0->next = z0break_avail;
         z0break_avail = z0;
      }
   }

   for(i=0; exceptionVector[i] > -1; i++)
   {
      rtems_interrupt_catch( (rtems_isr_entry) handle_exception, exceptionVector[i], &old );
   }

   initialized = 1;

   /* get the attention of gdb */
   /* mips_break(1);  disabled so user code can choose to invoke it or not */
}

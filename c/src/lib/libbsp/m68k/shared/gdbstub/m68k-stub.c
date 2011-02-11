#define  GDB_STUB_ENABLE_THREAD_SUPPORT 1
/****************************************************************************

		THIS SOFTWARE IS NOT COPYRIGHTED

   HP offers the following for use in the public domain.  HP makes no
   warranty with regard to the software or it's performance and the
   user accepts the software "AS IS" with all faults.

   HP DISCLAIMS ANY WARRANTIES, EXPRESS OR IMPLIED, WITH REGARD
   TO THIS SOFTWARE INCLUDING BUT NOT LIMITED TO THE WARRANTIES
   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

****************************************************************************/

/****************************************************************************
 *  Header: remcom.c,v 1.34 91/03/09 12:29:49 glenne Exp $
 *
 *  Module name: remcom.c $
 *  Revision: 1.34 $
 *  Date: 91/03/09 12:29:49 $
 *  Contributor:     Lake Stevens Instrument Division$
 *
 *  Description:     low level support for gdb debugger. $
 *
 *  Considerations:  only works on target hardware $
 *
 *  Written by:      Glenn Engel $
 *  ModuleState:     Experimental $
 *
 *  NOTES:           See Below $
 *
 *  To enable debugger support, two things need to happen.  One, a
 *  call to set_debug_traps() is necessary in order to allow any breakpoints
 *  or error conditions to be properly intercepted and reported to gdb.
 *  Two, a breakpoint needs to be generated to begin communication.  This
 *  is most easily accomplished by a call to breakpoint().  Breakpoint()
 *  simulates a breakpoint by executing a trap #1.  The breakpoint instruction
 *  is hardwired to trap #1 because not to do so is a compatibility problem--
 *  there either should be a standard breakpoint instruction, or the protocol
 *  should be extended to provide some means to communicate which breakpoint
 *  instruction is in use (or have the stub insert the breakpoint).
 *
 *  Some explanation is probably necessary to explain how exceptions are
 *  handled.  When an exception is encountered the 68000 pushes the current
 *  program counter and status register onto the supervisor stack and then
 *  transfers execution to a location specified in it's vector table.
 *  The handlers for the exception vectors are hardwired to jmp to an address
 *  given by the relation:  (exception - 256) * 6.  These are decending
 *  addresses starting from -6, -12, -18, ...  By allowing 6 bytes for
 *  each entry, a jsr, jmp, bsr, ... can be used to enter the exception
 *  handler.  Using a jsr to handle an exception has an added benefit of
 *  allowing a single handler to service several exceptions and use the
 *  return address as the key differentiation.  The vector number can be
 *  computed from the return address by [ exception = (addr + 1530) / 6 ].
 *  The sole purpose of the routine _catchException is to compute the
 *  exception number and push it on the stack in place of the return address.
 *  The external function exceptionHandler() is
 *  used to attach a specific handler to a specific m68k exception.
 *  For 68020 machines, the ability to have a return address around just
 *  so the vector can be determined is not necessary because the '020 pushes an
 *  extra word onto the stack containing the vector offset
 *
 *  Because gdb will sometimes write to the stack area to execute function
 *  calls, this program cannot rely on using the supervisor stack so it
 *  uses it's own stack area reserved in the int array remcomStack.
 *
 *************
 *
 *    The following gdb commands are supported:
 *
 * command          function                               Return value
 *
 *    g             return the value of the CPU registers  hex data or ENN
 *    G             set the value of the CPU registers     OK or ENN
 *
 *    mAA..AA,LLLL  Read LLLL bytes at address AA..AA      hex data or ENN
 *    MAA..AA,LLLL: Write LLLL bytes at address AA.AA      OK or ENN
 *
 *    c             Resume at current address              SNN   ( signal NN)
 *    cAA..AA       Continue at address AA..AA             SNN
 *
 *    s             Step one instruction                   SNN
 *    sAA..AA       Step one instruction from AA..AA       SNN
 *
 *    k             kill
 *
 *    ?             What was the last sigval ?             SNN   (signal NN)
 *
 * All commands and responses are sent with a packet which includes a
 * checksum.  A packet consists of
 *
 * $<packet info>#<checksum>.
 *
 * where
 * <packet info> :: <characters representing the command or response>
 * <checksum>    :: < two hex digits computed as modulo 256 sum of <packetinfo>>
 *
 * When a packet is received, it is first acknowledged with either '+' or '-'.
 * '+' indicates a successful transfer.  '-' indicates a failed transfer.
 *
 * Example:
 *
 * Host:                  Reply:
 * $m0,10#2a               +$00010203040506070809101112131415#42
 *
 ***************************************************************************
 *    added 05/27/02 by IMD, Thomas Doerfler:
 *    XAA..AA,LLLL: Write LLLL binary bytes at address AA.AA      OK or ENN
 ****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <setjmp.h>
/* #include <asm.h>*/
#include <rtems.h>
#include <rtems/score/cpu.h>
#include "gdb_if.h"

#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
static char do_threads = 1;      /* != 0 means we are supporting threads */
int current_thread_registers[NUMREGBYTES/4];
#endif

/************************************************************************
 *
 * external low-level support routines
 */
typedef void (*ExceptionHook)(int);   /* pointer to function with int parm */
typedef void (*Function)();           /* pointer to a function */

extern void putDebugChar();	/* write a single character      */
extern int getDebugChar();	/* read and return a single char */

/************************/
/* FORWARD DECLARATIONS */
/************************/
static void
initializeRemcomErrorFrame ();

/************************************************************************/
/* BUFMAX defines the maximum number of characters in inbound/outbound buffers*/
/* at least NUMREGBYTES*2 are needed for register packets */
#define BUFMAX 400

static bool initialized = false ;  /* boolean flag. != 0 means we've been initialized */

int     remote_debug;
/*  debug >  0 prints ill-formed commands in valid packets & checksum errors */

const char gdb_hexchars[]="0123456789abcdef";
#define highhex(x) gdb_hexchars [(x >> 4) & 0xf]
#define lowhex(x) gdb_hexchars [x & 0xf]


/* We keep a whole frame cache here.  "Why?", I hear you cry, "doesn't
   GDB handle that sort of thing?"  Well, yes, I believe the only
   reason for this cache is to save and restore floating point state
   (fsave/frestore).  A cleaner way to do this would be to make the
 fsave data part of the registers which GDB deals with like any
   other registers.  This should not be a performance problem if the
   ability to read individual registers is added to the protocol.  */

typedef struct FrameStruct
{
    struct FrameStruct  *previous;
    int       exceptionPC;      /* pc value when this frame created */
    int       exceptionVector;  /* cpu vector causing exception     */
    short     frameSize;        /* size of cpu frame in words       */
    short     sr;               /* for 68000, this not always sr    */
    int       pc;
    short     format;
    int       fsaveHeader;
    int       morejunk[0];        /* exception frame, fp save... */
} Frame;

#define FRAMESIZE 500
int   gdbFrameStack[FRAMESIZE];
static Frame *lastFrame;

/*
 * these should not be static cuz they can be used outside this module
 */
int registers[NUMREGBYTES/4];
int superStack;

#define STACKSIZE 10000
int remcomStack[STACKSIZE/sizeof(int)];
static int* stackPtr = &remcomStack[STACKSIZE/sizeof(int) - 1];

/*
 * In many cases, the system will want to continue exception processing
 * when a continue command is given.
 * oldExceptionHook is a function to invoke in this case.
 */

static ExceptionHook oldExceptionHook;
/* the size of the exception stack on the 68020/30/40/CPU32 varies with
 * the type of exception.  The following table is the number of WORDS used
 * for each exception format.
 * This table should be common for all 68k with VBR
 * although each member only implements some of the formats
 */
const short exceptionSize[] = { 4,4,6,6,4,4,4,30,29,10,16,46,12,4,4,4 };

/************* jump buffer used for setjmp/longjmp **************************/
jmp_buf remcomEnv;

/***************************  ASSEMBLY CODE MACROS *************************/
/* 									   */

#ifdef __HAVE_68881__
/* do an fsave, then remember the address to begin a restore from */
#define SAVE_FP_REGS()    __asm__ (" fsave   -(%a0)");		\
			  __asm__ (" fmovem.x %fp0-%fp7,registers+72");        \
			  __asm__ (" fmovem.l %fpcr/%fpsr/%fpi,registers+168");
#define RESTORE_FP_REGS()                              \
__asm__ ("                                                \n\
    fmovem.l  registers+168,%fpcr/%fpsr/%fpi            \n\
    fmovem.x  registers+72,%fp0-%fp7                   \n\
    cmp.l     #-1,(%a0)     |  skip frestore flag set ? \n\
    beq      skip_frestore                           \n\
    frestore (%a0)+                                    \n\
skip_frestore:                                       \n\
");

#else
#define SAVE_FP_REGS()
#define RESTORE_FP_REGS()
#endif /* __HAVE_68881__ */

void return_to_super();
void return_to_user();
extern void _catchException ();

void m68k_exceptionHandler
(
 int vecnum,                            /* vector index to hook at         */
 void *vector                           /* address of handler function     */
)
{
  void **vec_base = NULL;
#if M68K_HAS_VBR
  /*
   * get vector base register
   */
  __asm__ (" movec.l %%vbr,%0":"=a" (vec_base));
#endif
  vec_base[vecnum] = vector;
}

ExceptionHook exceptionHook;  /* hook variable for errors/exceptions */

void m68k_stub_dummy_asm_wrapper()
     /*
      * this dummy wrapper function ensures,
      * that the C compiler manages sections properly
      */
{
__asm__ ("\n\
.globl return_to_super 							\n\
return_to_super:							\n\
        move.l   registers+60,%sp /* get new stack pointer */         	\n\
        move.l   lastFrame,%a0   /* get last frame info  */           	\n\
        bra     return_to_any                                         	\n\
									\n\
.globl _return_to_user							\n\
return_to_user:								\n\
        move.l   registers+60,%a0 /* get usp */                       	\n\
        move.l   %a0,%usp           /* set usp */		  	\n\
        move.l   superStack,%sp  /* get original stack pointer */       \n\
									\n\
return_to_any: 								\n\
        move.l   lastFrame,%a0   /* get last frame info  */             \n\
        move.l   (%a0)+,lastFrame /* link in previous frame     */      \n\
        addq.l   #8,%a0           /* skip over pc, vector#*/            \n\
        move.w   (%a0)+,%d0         /* get # of words in cpu frame */   \n\
        add.w    %d0,%a0           /* point to end of data        */    \n\
        add.w    %d0,%a0           /* point to end of data        */    \n\
        move.l   %a0,%a1                                                \n\
#                                                                       \n\
# copy the stack frame                                                  \n\
        subq.l   #1,%d0                                                 \n\
copyUserLoop:                                                           \n\
        move.w   -(%a1),-(%sp) 						\n\
        dbf     %d0,copyUserLoop                                        \n\
");
        RESTORE_FP_REGS()
   __asm__ ("   movem.l  registers,%d0-%d7/%a0-%a6");
   __asm__ ("   rte");  /* pop and go! */

#define DISABLE_INTERRUPTS()   __asm__ ("         oriw   #0x0700,%sr");
#define BREAKPOINT() __asm__ ("   trap #2");

/* this function is called immediately when a level 7 interrupt occurs */
/* if the previous interrupt level was 7 then we're already servicing  */
/* this interrupt and an rte is in order to return to the debugger.    */
/* For the 68000, the offset for sr is 6 due to the jsr return address */
__asm__ ("						\n\
.text						\n\
.globl _debug_level7				\n\
_debug_level7:					\n\
	move.w   %d0,-(%sp)");
#if M68K_HAS_VBR
__asm__ ("	move.w   2(%sp),%d0");
#else
__asm__ ("	move.w   6(%sp),%d0");
#endif
__asm__ ("	andi.w   #0x700,%d0			\n\
	cmpi.w   #0x700,%d0			\n\
	beq     already7			\n\
        move.w   (%sp)+,%d0			\n\
        bra     _catchException			\n\
already7:					\n\
	move.w   (%sp)+,%d0");
#if !M68K_HAS_VBR
__asm__ ("	lea     4(%sp),%sp");     /* pull off 68000 return address */
#endif
__asm__ ("	rte");

#if M68K_HAS_VBR
/* This function is called when a 68020 exception occurs.  It saves
 * all the cpu and fpcp regs in the _registers array, creates a frame on a
 * linked list of frames which has the cpu and fpcp stack frames needed
 * to properly restore the context of these processors, and invokes
 * an exception handler (remcom_handler).
 *
 * stack on entry:                       stack on exit:
 *   N bytes of junk                     exception # MSWord
 *   Exception Format Word               exception # MSWord
 *   Program counter LSWord
 *   Program counter MSWord
 *   Status Register
 *
 *
 */
__asm__ (" 						\n\
.text						\n\
.globl _catchException				\n\
_catchException:");
DISABLE_INTERRUPTS();
__asm__ ("									\n\
        movem.l  %d0-%d7/%a0-%a6,registers /* save registers        */	\n\
	move.l	lastFrame,%a0	/* last frame pointer */		\n\
");
SAVE_FP_REGS();
__asm__ ("\n\
	lea     registers,%a5   /* get address of registers     */\n\
        move.w  (%sp),%d1        /* get status register          */\n\
        move.w   %d1,66(%a5)      /* save sr		 	*/	\n\
	move.l   2(%sp),%a4      /* save pc in a4 for later use  */\n\
	move.w   6(%sp),%d0	/* get '020 exception format	*/\n\
        move.w   %d0,%d2        /* make a copy of format word   */\n\
#\n\
# compute exception number\n\
	and.l    #0xfff,%d2   	/* mask off vector offset	*/\n\
	lsr.w    #2,%d2   	/* divide by 4 to get vect num	*/\n\
/* #if 1 */\n\
        cmp.l    #33,%d2\n\
        bne      nopc_adjust\n\
        subq.l   #2,%a4\n\
nopc_adjust:\n\
/* #endif */\n\
        move.l   %a4,68(%a5)     /* save pc in _regisers[]      	*/\n\
\n\
#\n\
# figure out how many bytes in the stack frame\n\
        andi.w   #0xf000,%d0    /* mask off format type         */\n\
        rol.w    #5,%d0         /* rotate into the low byte *2  */\n\
        lea     exceptionSize,%a1   \n\
        add.w    %d0,%a1        /* index into the table         */\n\
	move.w   (%a1),%d0      /* get number of words in frame */\n\
        move.w   %d0,%d3        /* save it                      */\n\
        sub.w    %d0,%a0	/* adjust save pointer          */\n\
        sub.w    %d0,%a0	/* adjust save pointer(bytes)   */\n\
	move.l   %a0,%a1        /* copy save pointer            */\n\
	subq.l   #1,%d0         /* predecrement loop counter    */\n\
#\n\
# copy the frame\n\
saveFrameLoop:\n\
	move.w 	(%sp)+,(%a1)+\n\
	dbf     %d0,saveFrameLoop\n\
#\n\
# now that the stack has been clenaed,\n\
# save the a7 in use at time of exception\n\
        move.l   %sp,superStack  /* save supervisor sp           */\n\
        andi.w   #0x2000,%d1      /* were we in supervisor mode ? */\n\
        beq      userMode       \n\
        move.l   %a7,60(%a5)      /* save a7                  */\n\
        bra      a7saveDone\n\
userMode:  \n\
	move.l   %usp,%a1    	\n\
        move.l   %a1,60(%a5)      /* save user stack pointer	*/\n\
a7saveDone:\n\
\n\
#\n\
# save size of frame\n\
        move.w   %d3,-(%a0)\n\
\n\
        move.l   %d2,-(%a0)     /* save vector number           */\n\
#\n\
# save pc causing exception\n\
        move.l   %a4,-(%a0)\n\
#\n\
# save old frame link and set the new value\n\
	move.l	lastFrame,%a1	/* last frame pointer */\n\
	move.l   %a1,-(%a0)		/* save pointer to prev frame	*/\n\
        move.l   %a0,lastFrame\n\
\n\
        move.l   %d2,-(%sp)  	    /* push exception num           */\n\
	move.l   exceptionHook,%a0  /* get address of handler */\n\
        jbsr    (%a0)             /* and call it */\n\
        clr.l   (%sp)            /* replace exception num parm with frame ptr */\n\
        jbsr     _returnFromException   /* jbsr, but never returns */\n\
");
#else /* mc68000 */
/* This function is called when an exception occurs.  It translates the
 * return address found on the stack into an exception vector # which
 * is then handled by either handle_exception or a system handler.
 * _catchException provides a front end for both.
 *
 * stack on entry:                       stack on exit:
 *   Program counter MSWord              exception # MSWord
 *   Program counter LSWord              exception # MSWord
 *   Status Register
 *   Return Address  MSWord
 *   Return Address  LSWord
 */
__asm__ ("\n\
.text\n\
.globl _catchException\n\
_catchException:");
DISABLE_INTERRUPTS();
__asm__ ("\
        moveml %d0-%d7/%a0-%a6,registers  /* save registers               */ \n\
	movel	lastFrame,%a0	/* last frame pointer */ \n\
");
SAVE_FP_REGS();
__asm__ (" \n\
        lea     registers,%a5   /* get address of registers     */ \n\
        movel   (%sp)+,%d2         /* pop return address           */ \n\
	addl 	#1530,%d2        /* convert return addr to 	*/ \n\
	divs 	#6,%d2   	/*  exception number		*/ \n\
	extl    %d2    \n\
 \n\
        moveql  #3,%d3           /* assume a three word frame     */ \n\
 \n\
        cmpiw   #3,%d2           /* bus error or address error ? */ \n\
        bgt     normal          /* if >3 then normal error      */ \n\
        movel   (%sp)+,-(%a0)       /* copy error info to frame buff*/ \n\
        movel   (%sp)+,-(%a0)       /* these are never used         */ \n\
        moveql  #7,%d3           /* this is a 7 word frame       */ \n\
      \n\
normal:    \n\
	movew   (%sp)+,%d1         /* pop status register          */ \n\
        movel   (%sp)+,%a4         /* pop program counter          */ \n\
        movew   %d1,66(%a5)      /* save sr		 	*/	 \n\
        movel   %a4,68(%a5)      /* save pc in _regisers[]      	*/ \n\
        movel   %a4,-(%a0)         /* copy pc to frame buffer      */ \n\
	movew   %d1,-(%a0)         /* copy sr to frame buffer      */ \n\
 \n\
        movel   %sp,superStack  /* save supervisor sp          */ \n\
 \n\
        andiw   #0x2000,%d1      /* were we in supervisor mode ? */ \n\
        beq     userMode        \n\
        movel   %a7,60(%a5)      /* save a7                  */ \n\
        bra     saveDone              \n\
userMode: \n\
        movel   %usp,%a1    	/* save user stack pointer 	*/ \n\
        movel   %a1,60(%a5)      /* save user stack pointer	*/ \n\
saveDone: \n\
 \n\
        movew   %d3,-(%a0)         /* push frame size in words     */ \n\
        movel   %d2,-(%a0)         /* push vector number           */ \n\
        movel   %a4,-(%a0)         /* push exception pc            */ \n\
 \n\
# \n\
# save old frame link and set the new value \n\
	movel	_lastFrame,%a1	/* last frame pointer */ \n\
	movel   %a1,-(%a0)		/* save pointer to prev frame	*/ \n\
        movel   %a0,lastFrame \n\
 \n\
        movel   %d2,-(%sp)		/* push exception num           */ \n\
	movel   exceptionHook,%a0  /* get address of handler */ \n\
        jbsr    (%a0)             /* and call it */ \n\
        clrl    (%sp)             /* replace exception num parm with frame ptr */ \n\
        jbsr     _returnFromException   /* jbsr, but never returns */ \n\
");
#endif

/*
 * remcomHandler is a front end for handle_exception.  It moves the
 * stack pointer into an area reserved for debugger use in case the
 * breakpoint happened in supervisor mode.
 */
__asm__ ("remcomHandler:");
__asm__ ("           add.l    #4,%sp");        /* pop off return address     */
__asm__ ("           move.l   (%sp)+,%d0");      /* get the exception number   */
__asm__ ("		move.l   stackPtr,%sp"); /* move to remcom stack area  */
__asm__ ("		move.l   %d0,-(%sp)");	/* push exception onto stack  */
__asm__ ("		jbsr    handle_exception");    /* this never returns */
__asm__ ("           rts");                  /* return */
} /* end of stub_dummy_asm_wrapper function */

void _returnFromException( Frame *frame )
{
    /* if no passed in frame, use the last one */
    if (! frame)
    {
        frame = lastFrame;
	frame->frameSize = 4;
        frame->format = 0;
        frame->fsaveHeader = -1; /* restore regs, but we dont have fsave info*/
    }

#if !M68K_HAS_VBR
    /* a 68000 cannot use the internal info pushed onto a bus error
     * or address error frame when doing an RTE so don't put this info
     * onto the stack or the stack will creep every time this happens.
     */
    frame->frameSize=3;
#endif

    /* throw away any frames in the list after this frame */
    lastFrame = frame;

    frame->sr = registers[(int) PS];
    frame->pc = registers[(int) PC];

    if (registers[(int) PS] & 0x2000)
    {
        /* return to supervisor mode... */
        return_to_super();
    }
    else
    { /* return to user mode */
        return_to_user();
    }
}

int hex(ch)
char ch;
{
  if ((ch >= 'a') && (ch <= 'f')) return (ch-'a'+10);
  if ((ch >= '0') && (ch <= '9')) return (ch-'0');
  if ((ch >= 'A') && (ch <= 'F')) return (ch-'A'+10);
  return (-1);
}

/* scan for the sequence $<data>#<checksum>     */
void getpacket(buffer)
char * buffer;
{
  unsigned char checksum;
  unsigned char xmitcsum;
  int  i;
  int  count;
  char ch;

  do {
    /* wait around for the start character, ignore all other characters */
    while ((ch = getDebugChar()) != '$');
    checksum = 0;
    xmitcsum = -1;

    count = 0;

    /* now, read until a # or end of buffer is found */
    while (count < BUFMAX) {
      ch = getDebugChar();
      if (ch == '#') break;
      checksum = checksum + ch;
      buffer[count] = ch;
      count = count + 1;
      }
    buffer[count] = 0;

    if (ch == '#') {
      xmitcsum = hex(getDebugChar()) << 4;
      xmitcsum += hex(getDebugChar());
      if ((remote_debug ) && (checksum != xmitcsum)) {
        fprintf (stderr,"bad checksum.  My count = 0x%x, sent=0x%x. buf=%s\n",
						     checksum,xmitcsum,buffer);
      }

      if (checksum != xmitcsum) putDebugChar('-');  /* failed checksum */
      else {
	 putDebugChar('+');  /* successful transfer */
	 /* if a sequence char is present, reply the sequence ID */
	 if (buffer[2] == ':') {
	    putDebugChar( buffer[0] );
	    putDebugChar( buffer[1] );
	    /* remove sequence chars from buffer */
	    count = strlen(buffer);
	    for (i=3; i <= count; i++) buffer[i-3] = buffer[i];
	 }
      }
    }
  } while (checksum != xmitcsum);

}

/* send the packet in buffer.  The host get's one chance to read it.
   This routine does not wait for a positive acknowledge.  */

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
  while  (getDebugChar () != '+');
}

char  remcomInBuffer[BUFMAX];
char  remcomOutBuffer[BUFMAX];
static short error;

void debug_error(
  char * format,
  char * parm
)
{
  if (remote_debug) fprintf (stderr,format,parm);
}

/* convert the memory pointed to by mem into hex, placing result in buf */
/* return a pointer to the last char put in buf (null) */
char* mem2hex(mem, buf, count)
char* mem;
char* buf;
int   count;
{
      int i;
      unsigned char ch;
      for (i=0;i<count;i++) {
          ch = *mem++;
          *buf++ = gdb_hexchars[ch >> 4];
          *buf++ = gdb_hexchars[ch % 16];
      }
      *buf = 0;
      return(buf);
}

/* convert the hex array pointed to by buf into binary to be placed in mem */
/* return a pointer to the character AFTER the last byte written */
char* hex2mem(buf, mem, count)
char* buf;
char* mem;
int   count;
{
      int i;
      unsigned char ch;
      for (i=0;i<count;i++) {
          ch = hex(*buf++) << 4;
          ch = ch + hex(*buf++);
          *mem++ = ch;
      }
      return(mem);
}

/* Convert the binary stream in BUF to memory.

   Gdb will escape $, #, and the escape char (0x7d).
   COUNT is the total number of bytes to write into
   memory. */
static unsigned char *
bin2mem (
  unsigned char *buf,
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

/* a bus error has occurred, perform a longjmp
   to return execution and allow handling of the error */

void handle_buserror()
{
  longjmp(remcomEnv,1);
}

/* this function takes the 68000 exception number and attempts to
   translate this number into a unix compatible signal value */
int computeSignal( exceptionVector )
int exceptionVector;
{
  int sigval;
  switch (exceptionVector) {
    case 2 : sigval = 10; break; /* bus error           */
    case 3 : sigval = 10; break; /* address error       */
    case 4 : sigval = 4;  break; /* illegal instruction */
    case 5 : sigval = 8;  break; /* zero divide         */
    case 6 : sigval = 8; break; /* chk instruction     */
    case 7 : sigval = 8; break; /* trapv instruction   */
    case 8 : sigval = 11; break; /* privilege violation */
    case 9 : sigval = 5;  break; /* trace trap          */
    case 10: sigval = 4;  break; /* line 1010 emulator  */
    case 11: sigval = 4;  break; /* line 1111 emulator  */

      /* Coprocessor protocol violation.  Using a standard MMU or FPU
	 this cannot be triggered by software.  Call it a SIGBUS.  */
    case 13: sigval = 10;  break;

    case 31: sigval = 2;  break; /* interrupt           */
    case 33: sigval = 5;  break; /* GDB breakpoint      */
    case 34: sigval = 5;  break; /* coded breakpoint    */

      /* This is a trap #8 instruction.  Apparently it is someone's software
	 convention for some sort of SIGFPE condition.  Whose?  How many
	 people are being screwed by having this code the way it is?
	 Is there a clean solution?  */
    case 40: sigval = 8;  break; /* floating point err  */

    case 48: sigval = 8;  break; /* floating point err  */
    case 49: sigval = 8;  break; /* floating point err  */
    case 50: sigval = 8;  break; /* zero divide         */
    case 51: sigval = 8;  break; /* underflow           */
    case 52: sigval = 8;  break; /* operand error       */
    case 53: sigval = 8;  break; /* overflow            */
    case 54: sigval = 8;  break; /* NAN                 */
    default:
      sigval = 7;         /* "software generated"*/
  }
  return (sigval);
}

/**********************************************/
/* WHILE WE FIND NICE HEX CHARS, BUILD AN INT */
/* RETURN NUMBER OF CHARS PROCESSED           */
/**********************************************/
int hexToInt(char **ptr, int *intValue)
{
    int numChars = 0;
    int hexValue;

    *intValue = 0;

    while (**ptr)
    {
        hexValue = hex(**ptr);
        if (hexValue >=0)
        {
            *intValue = (*intValue <<4) | hexValue;
            numChars ++;
        }
        else
            break;

        (*ptr)++;
    }

    return (numChars);
}

/*
 *  This support function prepares and sends the message containing the
 *  basic information about this exception.
 */

void gdb_stub_report_exception_info(
  int vector,
  int *regs,
  int thread
)
{
   char *optr;
   int sigval;

   optr = remcomOutBuffer;
   *optr++ = 'T';
   sigval = computeSignal (vector);
   *optr++ = highhex (sigval);
   *optr++ = lowhex (sigval);

   *optr++ = highhex(A7);
   *optr++ = lowhex(A7);
   *optr++ = ':';
   optr    = mem2hstr(optr,
		      (unsigned char *)&(regs[A7]),
		      sizeof(regs[A7]));
   *optr++ = ';';

   *optr++ = highhex(PC);
   *optr++ = lowhex(PC);
   *optr++ = ':';
   optr    = mem2hstr(optr,
		      (unsigned char *)&(regs[PC]),
		      sizeof(regs[PC]) );
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
 * This function does all command procesing for interfacing to gdb.
 */
void handle_exception(int exceptionVector)
{
  int    host_has_detached = 0;
  int    addr, length;
  char * ptr;
  int    newPC;
  Frame  *frame;
  int          current_thread;  /* current generic thread */
  int          thread;          /* stopped thread: context exception happened in */
   void         *regptr;
   int binary;

  if (remote_debug) printf("vector=%d, sr=0x%x, pc=0x%x\n",
			    exceptionVector,
			    registers[ PS ],
			    registers[ PC ]);

   thread = 0;
#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
   if (do_threads) {
      thread = rtems_gdb_stub_get_current_thread();
   }
#endif
   current_thread = thread;

#if 0
  /* reply to host that an exception has occurred */
  sigval = computeSignal( exceptionVector );
  remcomOutBuffer[0] = 'S';
  remcomOutBuffer[1] =  gdb_hexchars[sigval >> 4];
  remcomOutBuffer[2] =  gdb_hexchars[sigval % 16];
  remcomOutBuffer[3] = 0;
#else
   /* reply to host that an exception has occurred with some basic info */
   gdb_stub_report_exception_info(exceptionVector, registers, thread);
#endif

  putpacket(remcomOutBuffer);

  while (!(host_has_detached)) {
    binary = 0;
    error = 0;
    remcomOutBuffer[0] = 0;
    getpacket(remcomInBuffer);
    switch (remcomInBuffer[0]) {
#if 0
      case '?' :   remcomOutBuffer[0] = 'S';
                   remcomOutBuffer[1] =  gdb_hexchars[sigval >> 4];
                   remcomOutBuffer[2] =  gdb_hexchars[sigval % 16];
                   remcomOutBuffer[3] = 0;
                 break;
#else
      case '?' : gdb_stub_report_exception_info(exceptionVector,
						registers,
						thread);
	         break;
#endif
      case 'd' : remote_debug = !(remote_debug);  /* toggle debug flag */
                 break;
      case 'D' : /* host has detached */
	         strcpy(remcomOutBuffer,"OK");
	         host_has_detached = 1;
                 break;
         case 'g':               /* return the values of the CPU registers */
            regptr = registers;
#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
            if (do_threads && current_thread != thread )
               regptr = &current_thread_registers;
#endif
            mem2hex (regptr, remcomOutBuffer, sizeof registers);
            break;

         case 'G':       /* set the values of the CPU registers - return OK */
	   regptr = registers;
#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
	   if (do_threads && current_thread != thread )
	     regptr = &current_thread_registers;
#endif
	   if (hex2mem (&remcomInBuffer[1],
			regptr,
			sizeof registers)) {
	     strcpy (remcomOutBuffer, "OK");
	   }
	   else {
	     strcpy (remcomOutBuffer, "E00"); /* E00 = bad "set register" command */
	   }
	   break;

      /* mAA..AA,LLLL  Read LLLL bytes at address AA..AA */
      case 'm' :
	        if (setjmp(remcomEnv) == 0)
                {
                    m68k_exceptionHandler(2,handle_buserror);

		    /* TRY TO READ %x,%x.  IF SUCCEED, SET PTR = 0 */
                    ptr = &remcomInBuffer[1];
                    if (hexToInt(&ptr,&addr))
                        if (*(ptr++) == ',')
                            if (hexToInt(&ptr,&length))
                            {
                                ptr = 0;
                                mem2hex((char*) addr, remcomOutBuffer, length);
                            }

                    if (ptr)
                    {
		      strcpy(remcomOutBuffer,"E01");
		      debug_error("malformed read memory command: %s",remcomInBuffer);
                  }
                }
		else {
		  m68k_exceptionHandler(2,_catchException);
		  strcpy(remcomOutBuffer,"E03");
		  debug_error("bus error", 0);
		  }

		/* restore handler for bus error */
		m68k_exceptionHandler(2,_catchException);
		break;

      /* XAA..AA,LLLL: Write LLLL bytes at address AA.AA return OK */
      /* Transfer is in binary, '$', '#' and 0x7d is escaped with 0x7d */
      case 'X' :
                binary = 1;
      /* MAA..AA,LLLL: Write LLLL bytes at address AA.AA return OK */
      case 'M' :
	        if (setjmp(remcomEnv) == 0) {
		    m68k_exceptionHandler(2,handle_buserror);

		    /* TRY TO READ '%x,%x:'.  IF SUCCEED, SET PTR = 0 */
                    ptr = &remcomInBuffer[1];
                    if (hexToInt(&ptr,&addr))
                        if (*(ptr++) == ',')
                            if (hexToInt(&ptr,&length))
                                if (*(ptr++) == ':')
                                {
				  if (binary) {
				    bin2mem(ptr,(char *)addr,length);
				  }
				  else {
                                    hex2mem(ptr, (char*) addr, length);
				  }
				  ptr = 0;
				  strcpy(remcomOutBuffer,"OK");
                                }
                    if (ptr)
                    {
		      strcpy(remcomOutBuffer,"E02");
		      debug_error("malformed write memory command: %s",remcomInBuffer);
		      }
                }
		else {
		  m68k_exceptionHandler(2,_catchException);
		  strcpy(remcomOutBuffer,"E03");
		  debug_error("bus error", 0);
		  }

                /* restore handler for bus error */
                m68k_exceptionHandler(2,_catchException);
                break;

     /* cAA..AA    Continue at address AA..AA(optional) */
     /* sAA..AA   Step one instruction from AA..AA(optional) */
     case 'c' :
     case 's' :
          /* try to read optional parameter, pc unchanged if no parm */
         ptr = &remcomInBuffer[1];
         if (hexToInt(&ptr,&addr))
             registers[ PC ] = addr;

          newPC = registers[ PC];

          /* clear the trace bit */
          registers[ PS ] &= 0x7fff;

          /* set the trace bit if we're stepping */
          if (remcomInBuffer[0] == 's') registers[ PS ] |= 0x8000;

          /*
           * look for newPC in the linked list of exception frames.
           * if it is found, use the old frame it.  otherwise,
           * fake up a dummy frame in returnFromException().
           */
          if (remote_debug) printf("new pc = 0x%x\n",newPC);
          frame = lastFrame;
          while (frame)
          {
              if (remote_debug)
                  printf("frame at 0x%x has pc=0x%x, except#=%d\n",
                         (uint32_t) frame,
			 (uint32_t) frame->exceptionPC,
                         (uint32_t) frame->exceptionVector);
              if (frame->exceptionPC == newPC) break;  /* bingo! a match */
              /*
               * for a breakpoint instruction, the saved pc may
               * be off by two due to re-executing the instruction
               * replaced by the trap instruction.  Check for this.
               */
              if ((frame->exceptionVector == 33) &&
                  (frame->exceptionPC == (newPC+2))) break;
              if (frame == frame->previous)
	      {
	          frame = 0; /* no match found */
	          break;
	      }
	      frame = frame->previous;
          }

          /*
           * If we found a match for the PC AND we are not returning
           * as a result of a breakpoint (33),
           * trace exception (9), nmi (31), jmp to
           * the old exception handler as if this code never ran.
           */
          if (frame)
          {
              if ((frame->exceptionVector != 9)  &&
                  (frame->exceptionVector != 31) &&
                  (frame->exceptionVector != 33))
              {
                  /*
                   * invoke the previous handler.
                   */
                  if (oldExceptionHook)
                      (*oldExceptionHook) (frame->exceptionVector);
                  newPC = registers[ PC ];    /* pc may have changed  */
                  if (newPC != frame->exceptionPC)
                  {
                      if (remote_debug)
                          printf("frame at 0x%x has pc=0x%x, except#=%d\n",
                                 (uint32_t) frame,
				 frame->exceptionPC,
                                 frame->exceptionVector);
                      /* re-use the last frame, we're skipping it (longjump?)*/
		      frame = (Frame *) 0;
	              _returnFromException( frame );  /* this is a jump */
                  }
              }
          }

    	  /* if we couldn't find a frame, create one */
          if (frame == 0)
	  {
    	      frame = lastFrame -1 ;

	      /* by using a bunch of print commands with breakpoints,
    	         it's possible for the frame stack to creep down.  If it creeps
		 too far, give up and reset it to the top.  Normal use should
    	         not see this happen.
    	      */
	      if ((unsigned int) (frame-2) < (unsigned int) &gdbFrameStack)
    	      {
    	         initializeRemcomErrorFrame();
    	         frame = lastFrame;
	      }
    	      frame->previous = lastFrame;
              lastFrame = frame;
              frame = 0;  /* null so _return... will properly initialize it */
	  }

	  _returnFromException( frame ); /* this is a jump */

          break;

         case 'q':   /* queries */
#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
            rtems_gdb_process_query( remcomInBuffer,
				     remcomOutBuffer,
				     do_threads,
				     thread );
#endif
            break;

#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
         case 'T':
         {
            int testThread;

            if( vhstr2thread(&remcomInBuffer[1], &testThread) == NULL )
            {
               strcpy(remcomOutBuffer, "E01");
               break;
            }

            if( rtems_gdb_index_to_stub_id(testThread) == NULL )
            {
               strcpy(remcomOutBuffer, "E02");
            }
            else
            {
               strcpy(remcomOutBuffer, "OK");
            }
         }
         break;
#endif

         case 'H':  /* set new thread */
#if defined(GDB_STUB_ENABLE_THREAD_SUPPORT)
            if (remcomInBuffer[1] != 'g') {
               break;
            }

            if (!do_threads) {
               break;
            }

            {
               int tmp, ret;

               /* Set new generic thread */
               if (vhstr2thread(&remcomInBuffer[2], &tmp) == NULL) {
                  strcpy(remcomOutBuffer, "E01");
                  break;
               }

               /* 0 means `thread' */
               if (tmp == 0) {
                  tmp = thread;
               }

               if (tmp == current_thread) {
                  /* No changes */
                  strcpy(remcomOutBuffer, "OK");
                  break;
               }

               /* Save current thread registers if necessary */
               if (current_thread != thread) {
                  ret = rtems_gdb_stub_set_thread_regs(
                     current_thread, (unsigned int *) &current_thread_registers);
               }

               /* Read new registers if necessary */
               if (tmp != thread) {
                  ret = rtems_gdb_stub_get_thread_regs(
                     tmp, (unsigned int *) &current_thread_registers);

                  if (!ret) {
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

      /* kill the program */
      case 'k' :  /* do nothing */
                break;
      } /* switch */

    /* reply to the request */
    putpacket(remcomOutBuffer);
    }
}

void
initializeRemcomErrorFrame()
{
    lastFrame = ((Frame *) &gdbFrameStack[FRAMESIZE-1]) - 1;
    lastFrame->previous = lastFrame;
}

/* this function is used to set up exception handlers for tracing and
   breakpoints */
void set_debug_traps()
{
  extern void _debug_level7();
  extern void remcomHandler();
  int exception;

  initializeRemcomErrorFrame();
  stackPtr  = &remcomStack[STACKSIZE/sizeof(int) - 1];

  for (exception = 2; exception <= 23; exception++)
      m68k_exceptionHandler(exception,_catchException);

  /* level 7 interrupt              */
  m68k_exceptionHandler(31,_debug_level7);

  /* GDB breakpoint exception (trap #1) */
  m68k_exceptionHandler(33,_catchException);

  /* coded breakpoint exception (trap #2) */
  m68k_exceptionHandler(34,_catchException);

  /* This is a trap #8 instruction.  Apparently it is someone's software
     convention for some sort of SIGFPE condition.  Whose?  How many
     people are being screwed by having this code the way it is?
     Is there a clean solution?  */
  m68k_exceptionHandler(40,_catchException);

  /* 48 to 54 are floating point coprocessor errors */
  for (exception = 48; exception <= 54; exception++)
      m68k_exceptionHandler(exception,_catchException);

  if (oldExceptionHook != remcomHandler)
  {
      oldExceptionHook = exceptionHook;
      exceptionHook    = remcomHandler;
  }

  initialized = true;

}

/* This function will generate a breakpoint exception.  It is used at the
   beginning of a program to sync up with a debugger and can be used
   otherwise as a quick means to stop program execution and "break" into
   the debugger. */

void breakpoint()
{
  if (initialized) BREAKPOINT();
}

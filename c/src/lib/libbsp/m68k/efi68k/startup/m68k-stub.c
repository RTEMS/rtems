/*  m68k-stub.c
 *
 *  $Id$
 */

#include <efi68k.h>
#include <rtems.h>

m68k_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);


void (*exceptionHook)() = 0;

void outbyte(char c);
char putDebugChar( char c)
{
  outbyte(c);
  return c;
}

char inbyte(void);
char getDebugChar(void)
{
  return inbyte();
}

void flush_i_cache(void)
{
  return;
}

void *memset(void *p, int c, int n)
{
  register int i;
  void *s=p;

  for (i=0;i<n;i++) *((char *)p)++=(char)c;
  return s;
}

char *db_strcpy(char *s, const char *t)
{
  char *save=s;
  while((*s++ = *t++) != '\0');
  return save;
}

int db_strlen(const char *s)
{
  int n;

  for (n=0; *s!='\0'; s++) n++;
  return n;
}

/************************************************************************
 *
 * external low-level support routines 
 */
typedef void (*ExceptionHook)(int);   /* pointer to function with int parm */
typedef void (*Function)();           /* pointer to a function */

/* extern Function exceptionHandler(); */ /* assign an exception handler */
extern ExceptionHook exceptionHook;  /* hook variable for errors/exceptions */
/* extern void exceptionHandler(int vect, void (*function)()); */

/************************/
/* FORWARD DECLARATIONS */
/************************/
static void initializeRemcomErrorFrame();

/************************************************************************/
/* BUFMAX defines the maximum number of characters in inbound/outbound buffers*/
/* at least NUMREGBYTES*2 are needed for register packets */
#define BUFMAX 400

static char initialized;  /* boolean flag. != 0 means we've been initialized */

int     remote_debug;
/*  debug >  0 prints ill-formed commands in valid packets & checksum errors */ 

static const char hexchars[]="0123456789abcdef";

/* there are 180 bytes of registers on a 68020 w/68881      */
/* many of the fpa registers are 12 byte (96 bit) registers */
#define NUMREGBYTES 180
enum regnames {D0,D1,D2,D3,D4,D5,D6,D7, 
               A0,A1,A2,A3,A4,A5,A6,A7, 
               PS,PC,
               FP0,FP1,FP2,FP3,FP4,FP5,FP6,FP7,
               FPCONTROL,FPSTATUS,FPIADDR
              };



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

#define FRAMESIZE 10
int   gdbFrameStack[FRAMESIZE];
static Frame *lastFrame;

/*
 * these should not be static cuz they can be used outside this module
 */
int registers[NUMREGBYTES/4];
int superStack;

#define STACKSIZE 600
int remcomStack[STACKSIZE/sizeof(int)];
static int* stackPtr = &remcomStack[STACKSIZE/sizeof(int) - 1];

static ExceptionHook oldExceptionHook;

#define SAVE_FP_REGS()
#define RESTORE_FP_REGS()

void return_to_super();
void return_to_user();

asm("
.text
.align 2
.globl return_to_super
return_to_super:
        oriw    #0x0700,%sr
        movel   registers+60,%sp /* get new stack pointer */        
        movel   lastFrame,%a0   /* get last frame info  */              
        bra     return_to_any

.globl return_to_user
return_to_user:
        oriw    #0x0700,%sr
        movel   registers+60,%a0 /* get usp */                          
        movel   %a0,%usp           /* set usp */
        movel   superStack,%sp  /* get original stack pointer */        

return_to_any:
        movel   lastFrame,%a0   /* get last frame info  */              
        movel   %a0@+,lastFrame /* link in previous frame     */        
        addql   #8,%a0           /* skip over pc, vector#*/              
        movew   %a0@+,%d0         /* get # of words in cpu frame */       
        addw    %d0,%a0           /* point to end of data        */       
        addw    %d0,%a0           /* point to end of data        */       
        movel   %a0,%a1                                                   
#                                                                       
# copy the stack frame                                                  
        subql   #1,%d0                                                   
copyUserLoop:                                                               
        movew   %a1@-,%sp@-                                               
        dbf     %d0,copyUserLoop                                             
");                                                                     
        RESTORE_FP_REGS()                                              
   asm("   moveml  registers,%d0-%d7/%a0-%a6");			        
   asm("   rte");  /* pop and go! */                                    

#define DISABLE_INTERRUPTS()   asm("         oriw   #0x0700,%sr");
#define BREAKPOINT() asm("   trap #1");

/* this function is called immediately when a level 7 interrupt occurs */
/* if the previous interrupt level was 7 then we're already servicing  */
/* this interrupt and an rte is in order to return to the debugger.    */
/* For the 68000, the offset for sr is 6 due to the jsr return address */
asm("
.text
.align 2
.globl _debug_level7
_debug_level7:
	movew   %d0,%sp@-");
asm("	movew   %sp@(6),%d0");

asm("	andiw   #0x700,%d0
	cmpiw   #0x700,%d0
	beq     already7
        movew   %sp@+,%d0	
        bra     _catchException
already7:
	movew   %sp@+,%d0");
asm("	lea     %sp@(4),%sp");     /* pull off 68000 return address */

asm("	rte");

extern void _catchException ();

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

asm("
.text
.align 2
.globl _catchException
_catchException:
");
DISABLE_INTERRUPTS();
asm("
        moveml %d0-%d7/%a0-%a6,registers  /* save registers               */
	movel	lastFrame,%a0	/* last frame pointer */

        move.b #64,(0x00600001)
        move.b (0x00600007),%d0
        andib #-64,%d0
        move.b %d0,(0x00600007)

");
SAVE_FP_REGS();        
asm("
        lea     registers,%a5   /* get address of registers     */
        movel   %sp@+,%d2         /* pop return address           */
        addq.l  #6,%d2
        sub.l   #0x200000,%d2
        divs    #6,%d2
/*	addl 	#1530,%d2  */      /* convert return addr to 	*/
/*	divs 	#6,%d2     */	/*  exception number		*/
	extl    %d2   

        moveql  #3,%d3           /* assume a three word frame     */

        cmpiw   #3,%d2           /* bus error or address error ? */
        bgt     normal          /* if >3 then normal error      */
        movel   %sp@+,%a0@-       /* copy error info to frame buff*/
        movel   %sp@+,%a0@-       /* these are never used         */
        moveql  #7,%d3           /* this is a 7 word frame       */
     
normal:   
	movew   %sp@+,%d1         /* pop status register          */
        movel   %sp@+,%a4         /* pop program counter          */
        movew   %d1,%a5@(66)      /* save sr		 	*/	
        movel   %a4,%a5@(68)      /* save pc in _regisers[]      	*/
        movel   %a4,%a0@-         /* copy pc to frame buffer      */
	movew   %d1,%a0@-         /* copy sr to frame buffer      */

        movel   %sp,superStack  /* save supervisor sp          */

        andiw   #0x2000,%d1      /* were we in supervisor mode ? */
        beq     userMode       
        movel   %a7,%a5@(60)      /* save a7                  */
        bra     saveDone             
userMode:
        movel   %usp,%a1    	/* save user stack pointer 	*/
        movel   %a1,%a5@(60)      /* save user stack pointer	*/
saveDone:

        movew   %d3,%a0@-         /* push frame size in words     */
        movel   %d2,%a0@-         /* push vector number           */
        movel   %a4,%a0@-         /* push exception pc            */

#
# save old frame link and set the new value
	movel	lastFrame,%a1	/* last frame pointer */
	movel   %a1,%a0@-		/* save pointer to prev frame	*/
        movel   %a0,lastFrame

        movel   %d2,%sp@-		/* push exception num           */
	movel   exceptionHook,%a0  /* get address of handler */
        jbsr    %a0@             /* and call it */
        clrl    %sp@             /* replace exception num parm with frame ptr*/
        jbsr     _returnFromException   /* jbsr, but never returns */
");


/*
 * remcomHandler is a front end for handle_exception.  It moves the
 * stack pointer into an area reserved for debugger use in case the
 * breakpoint happened in supervisor mode.
 */
asm("remcomHandler:");
asm("           addl    #4,%sp");        /* pop off return address     */
asm("           movel   %sp@+,%d0");      /* get the exception number   */
asm("		movel   stackPtr,%sp"); /* move to remcom stack area  */
asm("		movel   %d0,%sp@-");	/* push exception onto stack  */
asm("           andiw   #0xf8ff,%sr");
asm("		jbsr    handle_exception");    /* this never returns */
asm("           rts");                  /* return */

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


    /* a 68000 cannot use the internal info pushed onto a bus error
     * or address error frame when doing an RTE so don't put this info
     * onto the stack or the stack will creep every time this happens.
     */
    frame->frameSize=3;


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
      
      if (checksum != xmitcsum) putDebugChar('-');  /* failed checksum */ 
      else {
	 putDebugChar('+');  /* successful transfer */
	 /* if a sequence char is present, reply the sequence ID */
	 if (buffer[2] == ':') {
	    putDebugChar( buffer[0] );
	    putDebugChar( buffer[1] );
	    /* remove sequence chars from buffer */
	    count = db_strlen(buffer);
	    for (i=3; i <= count; i++) buffer[i-3] = buffer[i];
	 } 
      } 
    } 
  } while (checksum != xmitcsum);
  
}

/* send the packet in buffer.  The host get's one chance to read it.  
   This routine does not wait for a positive acknowledge.  */


void putpacket(buffer)
char * buffer;
{
  unsigned char checksum;
  int  count;
  char ch;
  
  /*  $<packet info>#<checksum>. */
  do {
  putDebugChar('$');
  checksum = 0;
  count    = 0;
  
  while ((ch=buffer[count])) {
    if (! putDebugChar(ch)) return;
    checksum += ch;
    count += 1;
  }
  
  putDebugChar('#');
  putDebugChar(hexchars[checksum >> 4]);
  putDebugChar(hexchars[checksum % 16]);

  } while (1 == 0);  /* (getDebugChar() != '+'); */
  
}

char  remcomInBuffer[BUFMAX];
char  remcomOutBuffer[BUFMAX];
static short error;


void debug_error(format, parm)
char * format;
char * parm;
{
  return;
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
          *buf++ = hexchars[ch >> 4];
          *buf++ = hexchars[ch % 16];
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
    case 33: sigval = 5;  break; /* breakpoint          */

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
 * This function does all command procesing for interfacing to gdb.
 */
void handle_exception(int exceptionVector)
{
  int    sigval;
  int    addr, length;
  char * ptr;
  int    newPC;
  Frame  *frame;
  
  /* reply to host that an exception has occurred */
  sigval = computeSignal( exceptionVector );
  remcomOutBuffer[0] = 'S';
  remcomOutBuffer[1] =  hexchars[sigval >> 4];
  remcomOutBuffer[2] =  hexchars[sigval % 16];
  remcomOutBuffer[3] = 0;

  putpacket(remcomOutBuffer); 

  while (1==1) { 
    error = 0;
    remcomOutBuffer[0] = 0;
    getpacket(remcomInBuffer);
    switch (remcomInBuffer[0]) {
      case '?' :   remcomOutBuffer[0] = 'S';
                   remcomOutBuffer[1] =  hexchars[sigval >> 4];
                   remcomOutBuffer[2] =  hexchars[sigval % 16];
                   remcomOutBuffer[3] = 0;
                 break; 
      case 'd' : remote_debug = !(remote_debug);  /* toggle debug flag */
                 break; 
      case 'g' : /* return the value of the CPU registers */
                mem2hex((char*) registers, remcomOutBuffer, NUMREGBYTES);
                break;
      case 'G' : /* set the value of the CPU registers - return OK */
                hex2mem(&remcomInBuffer[1], (char*) registers, NUMREGBYTES);
                db_strcpy(remcomOutBuffer,"OK");
                break;
      
      /* mAA..AA,LLLL  Read LLLL bytes at address AA..AA */
      case 'm' : 
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
	    db_strcpy(remcomOutBuffer,"E01");
	    debug_error("malformed read memory command: %s",remcomInBuffer);
	  }     
        break;
      
      /* MAA..AA,LLLL: Write LLLL bytes at address AA.AA return OK */
      case 'M' : 
	/* TRY TO READ '%x,%x:'.  IF SUCCEED, SET PTR = 0 */
	ptr = &remcomInBuffer[1];
	if (hexToInt(&ptr,&addr))
	  if (*(ptr++) == ',')
	    if (hexToInt(&ptr,&length))
	      if (*(ptr++) == ':')
		{
		  hex2mem(ptr, (char*) addr, length);
		  ptr = 0;
		  db_strcpy(remcomOutBuffer,"OK");
		}
	if (ptr)
	  {
	    db_strcpy(remcomOutBuffer,"E02");
	    debug_error("malformed write memory command: %s",remcomInBuffer);
	  }     
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
          frame = lastFrame;
          while (frame)
          {
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
      set_vector(_catchException,exception,0);   

  /* level 7 interrupt              */
  set_vector(_debug_level7,31,0);    
  
  /* breakpoint exception (trap #1) */
  set_vector(_catchException,33,0);
  
  /* This is a trap #8 instruction.  Apparently it is someone's software
     convention for some sort of SIGFPE condition.  Whose?  How many
     people are being screwed by having this code the way it is?
     Is there a clean solution?  */
  set_vector(_catchException,40,0);
  
  /* 48 to 54 are floating point coprocessor errors */
  for (exception = 48; exception <= 54; exception++)
      set_vector(_catchException,exception,0);   

  if (oldExceptionHook != remcomHandler)
  {
      oldExceptionHook = exceptionHook;
      exceptionHook    = remcomHandler;
  }
  
  initialized = 1;

}

/* This function will generate a breakpoint exception.  It is used at the
   beginning of a program to sync up with a debugger and can be used
   otherwise as a quick means to stop program execution and "break" into
   the debugger. */
   
void breakpoint()
{
  if (initialized) BREAKPOINT();
}


/* Support files for newlib/GDB simulator.
 *  $Id$
 */

#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#include <reent.h>
#include "swi.h"

#include <bspopts.h>

int armulator_stdin;
int armulator_stdout;
int armulator_stderr;

#ifdef ARM_RDI_MONITOR

static inline int
do_AngelSWI (int reason, void * arg)
{
  int value;
  asm volatile ("mov r0, %1; mov r1, %2; swi %a3; mov %0, r0"
       : "=r" (value) /* Outputs */
       : "r" (reason), "r" (arg), "i" (AngelSWI) /* Inputs */
       : "r0", "r1", "lr"
		/* Clobbers r0 and r1, and lr if in supervisor mode */);
  return value;
}
#endif /* ARM_RDI_MONITOR */

void
initialize_monitor_handles (void)
{
#ifdef ARM_RDI_MONITOR
  int volatile block[3];
  
  block[0] = (int) ":tt";
  block[2] = 3;     /* length of filename */
  block[1] = 0;     /* mode "r" */
  armulator_stdin = do_AngelSWI (AngelSWI_Reason_Open, (void *)block);

  block[0] = (int) ":tt";
  block[2] = 3;     /* length of filename */
  block[1] = 4;     /* mode "w" */
  armulator_stdout = armulator_stderr = do_AngelSWI (AngelSWI_Reason_Open, (void *)block);
#else
  int fh;
  const char * name;

  name = ":tt";
  asm ("mov r0,%2; mov r1, #0; swi %a1; mov %0, r0"
       : "=r"(fh)
       : "i" (SWI_Open),"r"(name)
       : "r0","r1");
  armulator_stdin = fh;

  name = ":tt";
  asm ("mov r0,%2; mov r1, #4; swi %a1; mov %0, r0"
       : "=r"(fh)
       : "i" (SWI_Open),"r"(name)
       : "r0","r1");
  armulator_stdout = armulator_stderr = fh;
#endif
}

/* Returns # chars not! read */

int
_swiread (int file,
	  char * ptr,
	  int len)
{
  int fh = file;
#ifdef ARM_RDI_MONITOR
  int block[3];
  
  block[0] = fh;
  block[1] = (int) ptr;
  block[2] = len;
  
  return do_AngelSWI (AngelSWI_Reason_Read, block);
#else
  asm ("mov r0, %1; mov r1, %2;mov r2, %3; swi %a0"
       : /* No outputs */
       : "i"(SWI_Read), "r"(fh), "r"(ptr), "r"(len)
       : "r0","r1","r2");
#endif
}

/* Returns #chars not! written */
int
_swiwrite (
	   int    file,
	   char * ptr,
	   int    len)
{
  int fh = file;
#ifdef ARM_RDI_MONITOR
  int block[3];
  
  block[0] = fh;
  block[1] = (int) ptr;
  block[2] = len;
  
  return do_AngelSWI (AngelSWI_Reason_Write, block);
#else
  asm ("mov r0, %1; mov r1, %2;mov r2, %3; swi %a0"
       : /* No outputs */
       : "i"(SWI_Write), "r"(fh), "r"(ptr), "r"(len)
       : "r0","r1","r2");
#endif
}

/*
 *  Move me 
 */

void
bsp_cleanup (void )
{
  /* FIXME: return code is thrown away */
  
#ifdef ARM_RDI_MONITOR
  do_AngelSWI (AngelSWI_Reason_ReportException,
	      (void *) ADP_Stopped_ApplicationExit);
#else
  asm ("swi %a0" :: "i" (SWI_Exit));
#endif
}

/*
 *  Technically could use this as guts of a "real-time clock driver"
 */

#if 0
int
_gettimeofday (struct timeval * tp, struct timezone * tzp)
{

  if (tp)
    {
    /* Ask the host for the seconds since the Unix epoch */
#ifdef ARM_RDI_MONITOR
      tp->tv_sec = do_AngelSWI (AngelSWI_Reason_Time,NULL);
#else
      {
        int value;
        asm ("swi %a1; mov %0, r0" : "=r" (value): "i" (SWI_Time) : "r0");
        tp->tv_sec = value;
      }
#endif
      tp->tv_usec = 0;
    }

  /* Return fixed data for the timezone */
  if (tzp)
    {
      tzp->tz_minuteswest = 0;
      tzp->tz_dsttime = 0;
    }

  return 0;
}
#endif

